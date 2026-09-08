#include "lbot_vision/nut_sequence.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace lbot_vision {

void NutSequenceConfig::validate() const
{
  auto require = [](bool valid, const char *message) {
    if (!valid) throw std::invalid_argument(std::string("Invalid nut sequence configuration: ") + message);
  };
  require(sequence_stable_frames >= 1, "sequence_stable_frames must be >= 1");
  require(std::isfinite(sequence_max_center_shift_px) && sequence_max_center_shift_px >= 0.0,
          "sequence_max_center_shift_px must be finite and >= 0");
  require(std::isfinite(sequence_max_radius_change_ratio) &&
          sequence_max_radius_change_ratio >= 0.0 && sequence_max_radius_change_ratio < 1.0,
          "sequence_max_radius_change_ratio must be finite and in [0, 1)");
  require(std::isfinite(sequence_min_size_gap_ratio) && sequence_min_size_gap_ratio >= 0.0 &&
          sequence_min_size_gap_ratio < 1.0,
          "sequence_min_size_gap_ratio must be finite and in [0, 1)");
  require(std::isfinite(sequence_max_gap_ms) && sequence_max_gap_ms > 0.0,
          "sequence_max_gap_ms must be finite and > 0");
}

NutSequence::NutSequence(const NutSequenceConfig &config) : config_(config)
{
  config_.validate();
  initialize_targets();
}

void NutSequence::initialize_targets()
{
  snapshot_.targets.clear();
  snapshot_.targets.push_back({1, "nut_large"});
  snapshot_.targets.push_back({2, "nut_medium"});
  snapshot_.targets.push_back({3, "nut_small"});
}

std::size_t NutSequence::completed_count() const
{
  return static_cast<std::size_t>(std::count_if(
      snapshot_.targets.begin(), snapshot_.targets.end(),
      [](const NutTarget &target) { return target.state == "completed"; }));
}

bool NutSequence::matches(const cv::Vec3f &a, const cv::Vec3f &b) const
{
  if (a[2] <= 0.0F || b[2] <= 0.0F) return false;
  const double center_shift = std::hypot(
      static_cast<double>(a[0] - b[0]), static_cast<double>(a[1] - b[1]));
  const double radius_change = std::abs(static_cast<double>(a[2] - b[2])) /
                               std::max(static_cast<double>(a[2]), static_cast<double>(b[2]));
  return center_shift <= config_.sequence_max_center_shift_px &&
         radius_change <= config_.sequence_max_radius_change_ratio;
}

std::vector<NutSequence::OrderedObservation> NutSequence::order_by_size(
    const std::vector<cv::Vec3f> &observations) const
{
  std::vector<OrderedObservation> ordered;
  ordered.reserve(observations.size());
  for (std::size_t i = 0; i < observations.size(); ++i) {
    ordered.push_back({static_cast<int>(i), observations[i]});
  }
  std::sort(ordered.begin(), ordered.end(), [](const auto &a, const auto &b) {
    return a.circle[2] > b.circle[2];
  });
  return ordered;
}

void NutSequence::invalidate(const std::string &reason)
{
  snapshot_.observation_valid = false;
  snapshot_.status = reason;
  stable_count_ = 0;
  stability_seed_.clear();
  for (auto &target : snapshot_.targets) {
    target.visible = false;
    target.observation_index = -1;
  }
}

const NutSequenceSnapshot &NutSequence::observe(
    std::int64_t stamp_ms, bool frame_found, cv::Size image_size,
    const std::vector<cv::Vec3f> &observations)
{
  snapshot_.observed_count = observations.size();
  snapshot_.expected_count = 3 - completed_count();
  snapshot_.current_target_id = snapshot_.expected_count == 0
      ? 0 : static_cast<int>(completed_count() + 1);
  for (auto &target : snapshot_.targets) {
    target.visible = false;
    target.observation_index = -1;
  }
  snapshot_.observation_valid = false;

  if (stamp_ms < 0 || stamp_ms <= last_stamp_ms_) {
    invalidate("non_increasing_timestamp");
    return snapshot_;
  }
  const bool observation_gap = last_stamp_ms_ >= 0 &&
      static_cast<double>(stamp_ms - last_stamp_ms_) > config_.sequence_max_gap_ms;
  last_stamp_ms_ = stamp_ms;
  if (observation_gap) {
    invalidate("observation_gap");
    return snapshot_;
  }
  if (!frame_found || image_size.width <= 0 || image_size.height <= 0) {
    invalidate("frame_not_found");
    return snapshot_;
  }
  if (observations.size() > 3) {
    invalidate("too_many_candidates");
    return snapshot_;
  }
  for (const auto &circle : observations) {
    if (!std::isfinite(circle[0]) || !std::isfinite(circle[1]) || !std::isfinite(circle[2]) ||
        circle[2] <= 0.0F || circle[0] < 0.0F || circle[1] < 0.0F ||
        circle[0] >= image_size.width || circle[1] >= image_size.height) {
      invalidate("invalid_observation");
      return snapshot_;
    }
  }

  if (observations.size() != snapshot_.expected_count) {
    invalidate("observation_count_mismatch");
    return snapshot_;
  }
  if (snapshot_.expected_count == 0) {
    snapshot_.observation_valid = true;
    snapshot_.status = "round_completed";
    return snapshot_;
  }

  auto ordered = order_by_size(observations);
  for (std::size_t i = 1; i < ordered.size(); ++i) {
    const double larger = ordered[i - 1].circle[2];
    const double gap = (larger - ordered[i].circle[2]) / larger;
    if (gap < config_.sequence_min_size_gap_ratio) {
      invalidate("ambiguous_pixel_sizes");
      return snapshot_;
    }
  }

  bool stable = stability_seed_.size() == ordered.size();
  if (stable) {
    for (std::size_t i = 0; i < ordered.size(); ++i) {
      if (!matches(stability_seed_[i], ordered[i].circle)) {
        stable = false;
        break;
      }
    }
  }
  stability_seed_.clear();
  for (const auto &item : ordered) stability_seed_.push_back(item.circle);
  stable_count_ = stable ? std::min(stable_count_ + 1, config_.sequence_stable_frames) : 1;
  snapshot_.status = snapshot_.initialized ? "stabilizing_stage" : "stabilizing_three";
  if (stable_count_ < config_.sequence_stable_frames) return snapshot_;

  const std::size_t first_remaining = completed_count();
  for (std::size_t i = 0; i < ordered.size(); ++i) {
    auto &target = snapshot_.targets[first_remaining + i];
    target.visible = true;
    target.observation_index = ordered[i].input_index;
    target.last_observation = ordered[i].circle;
    target.last_seen_ms = stamp_ms;
  }
  snapshot_.initialized = true;
  snapshot_.observation_valid = true;
  const auto &current = snapshot_.targets[first_remaining];
  snapshot_.status = current.state == "in_progress" ? "target_in_progress" : "ready";
  return snapshot_;
}

bool NutSequence::event(std::uint64_t round, std::uint64_t event_sequence, int target_id,
                        const std::string &action, std::string &reason)
{
  auto reject = [&](const char *why) {
    reason = why;
    return false;
  };
  if (event_sequence != 0 && event_sequence == last_event_sequence_ &&
      round == last_event_round_ && target_id == last_event_target_id_ &&
      action == last_event_action_) {
    reason = "already_applied";
    return true;
  }
  if (round != snapshot_.round) return reject("wrong_round");
  if (event_sequence == 0 || event_sequence <= last_event_sequence_) {
    return reject("stale_event_sequence");
  }

  if (action == "reset") {
    if (target_id != 0) return reject("reset_requires_target_zero");
    const auto next_round = snapshot_.round + 1;
    snapshot_ = NutSequenceSnapshot{};
    snapshot_.round = next_round;
    initialize_targets();
    stability_seed_.clear();
    stable_count_ = 0;
    last_stamp_ms_ = -1;
  } else {
    if (!snapshot_.initialized || target_id < 1 || target_id > 3) {
      return reject("unknown_target");
    }
    const std::size_t completed = completed_count();
    const int expected_target_id = completed == 3 ? 0 : static_cast<int>(completed + 1);
    if (target_id != expected_target_id) return reject("size_order_violation");
    auto &target = snapshot_.targets[static_cast<std::size_t>(target_id - 1)];

    if (action == "start") {
      if (!snapshot_.observation_valid || !target.visible) {
        return reject("target_not_currently_visible");
      }
      if (target.state != "pending") return reject("start_requires_pending");
      target.state = "in_progress";
      snapshot_.status = "target_in_progress";
    } else if (action == "complete") {
      if (target.state != "in_progress") return reject("complete_requires_in_progress");
      target.state = "completed";
      snapshot_.expected_count = 3 - completed_count();
      snapshot_.current_target_id = snapshot_.expected_count == 0 ? 0 : target_id + 1;
      invalidate(snapshot_.expected_count == 0 ? "round_completed" : "feedback_requires_new_observation");
    } else if (action == "retry") {
      if (target.state != "in_progress") return reject("retry_requires_in_progress");
      target.state = "pending";
      invalidate("feedback_requires_new_observation");
    } else {
      return reject("unknown_action");
    }
  }

  last_event_sequence_ = event_sequence;
  last_event_round_ = round;
  last_event_target_id_ = target_id;
  last_event_action_ = action;
  reason = "applied";
  return true;
}

}  // namespace lbot_vision
