#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <opencv2/core.hpp>

namespace lbot_vision {

struct NutSequenceConfig {
#define LBOT_SEQUENCE_FIELD(type, name) type name{};
#include "lbot_vision/sequence_fields.inc"
#undef LBOT_SEQUENCE_FIELD
  void validate() const;
};

struct NutTarget {
  int id{0};                         // 1=large, 2=medium, 3=small for one round
  std::string size_class;
  std::string state{"pending"};     // pending | in_progress | completed
  bool visible{false};
  int observation_index{-1};         // valid only when observation_valid=true
  cv::Vec3f last_observation{};
  std::int64_t last_seen_ms{-1};
};

struct NutSequenceSnapshot {
  std::uint64_t round{1};
  bool initialized{false};
  bool observation_valid{false};
  std::size_t observed_count{0};
  std::size_t expected_count{3};
  int current_target_id{1};          // 0 after all three explicit completions
  std::string status{"waiting_for_three"};
  std::vector<NutTarget> targets;
};

// Stage-aware identity for the fixed large->medium->small task. This is not a
// generic spatial tracker: each stage re-ranks the remaining observations by
// pixel radius. Only explicit events advance task state.
class NutSequence {
public:
  explicit NutSequence(const NutSequenceConfig &config);

  const NutSequenceSnapshot &observe(
      std::int64_t stamp_ms, bool frame_found, cv::Size image_size,
      const std::vector<cv::Vec3f> &observations);
  const NutSequenceSnapshot &snapshot() const { return snapshot_; }

  // Used by adapters when the source image itself is stale or invalid.
  void invalidate(const std::string &reason);

  // Explicit feedback only. Exact retries of the last accepted event are
  // idempotent; event_sequence must otherwise increase, including after reset.
  bool event(std::uint64_t round, std::uint64_t event_sequence, int target_id,
             const std::string &action, std::string &reason);

private:
  struct OrderedObservation {
    int input_index{-1};
    cv::Vec3f circle{};
  };

  NutSequenceConfig config_;
  NutSequenceSnapshot snapshot_;
  std::vector<cv::Vec3f> stability_seed_;
  int stable_count_{0};
  std::int64_t last_stamp_ms_{-1};
  std::uint64_t last_event_sequence_{0};
  std::uint64_t last_event_round_{0};
  int last_event_target_id_{0};
  std::string last_event_action_;

  void initialize_targets();
  std::size_t completed_count() const;
  bool matches(const cv::Vec3f &a, const cv::Vec3f &b) const;
  std::vector<OrderedObservation> order_by_size(
      const std::vector<cv::Vec3f> &observations) const;
};

}  // namespace lbot_vision
