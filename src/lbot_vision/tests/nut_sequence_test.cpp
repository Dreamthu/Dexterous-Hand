#include "lbot_vision/nut_sequence.hpp"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void require(bool condition, const std::string &message)
{
  if (!condition) throw std::runtime_error(message);
}

lbot_vision::NutSequenceConfig config()
{
  lbot_vision::NutSequenceConfig value;
  value.sequence_stable_frames = 3;
  value.sequence_max_center_shift_px = 8.0;
  value.sequence_max_radius_change_ratio = 0.10;
  value.sequence_min_size_gap_ratio = 0.10;
  value.sequence_max_gap_ms = 1000.0;
  return value;
}

void observe_stable(lbot_vision::NutSequence &sequence, std::int64_t first_stamp,
                    const std::vector<cv::Vec3f> &observations)
{
  for (int i = 0; i < 3; ++i) {
    sequence.observe(first_stamp + i * 100, true, {640, 480}, observations);
  }
}

}  // namespace

int main()
{
  try {
    const std::vector<cv::Vec3f> three{{100, 100, 30}, {250, 120, 20}, {400, 140, 10}};
    std::string reason;

    lbot_vision::NutSequence cold(config());
    cold.observe(0, true, {640, 480}, {});
    require(!cold.snapshot().initialized, "zero observations initialized the sequence");
    cold.observe(100, true, {640, 480}, {three[1], three[2]});
    require(!cold.snapshot().initialized, "two observations initialized the sequence");
    require(cold.snapshot().status == "observation_count_mismatch", "wrong cold-start status");

    lbot_vision::NutSequence sequence(config());
    observe_stable(sequence, 0, {three[1], three[2], three[0]});
    const auto &initial = sequence.snapshot();
    require(initial.initialized && initial.observation_valid, "three stable frames did not initialize");
    require(initial.expected_count == 3 && initial.current_target_id == 1, "wrong initial stage");
    require(initial.targets[0].id == 1 && initial.targets[0].size_class == "nut_large", "large ID missing");
    require(initial.targets[1].id == 2 && initial.targets[1].size_class == "nut_medium", "medium ID missing");
    require(initial.targets[2].id == 3 && initial.targets[2].size_class == "nut_small", "small ID missing");
    require(initial.targets[0].observation_index == 2 && initial.targets[1].observation_index == 0 &&
            initial.targets[2].observation_index == 1, "initial pixel-size ranking is wrong");

    sequence.observe(300, true, {640, 480}, {three[1], three[2]});
    require(sequence.snapshot().current_target_id == 1, "a missing nut advanced the stage");
    require(sequence.snapshot().targets[0].state == "pending", "a missing nut completed large");
    require(!sequence.snapshot().observation_valid, "count mismatch remained graspable");
    require(!sequence.event(1, 1, 2, "start", reason), "medium started before large");

    observe_stable(sequence, 400, three);
    require(sequence.event(1, 1, 1, "start", reason), "large did not start");
    sequence.observe(700, true, {640, 480}, {three[1], three[2]});
    require(sequence.snapshot().targets[0].state == "in_progress", "disappearance completed large");
    require(!sequence.snapshot().observation_valid, "in-progress disappearance became valid");
    require(sequence.event(1, 2, 1, "retry", reason), "large retry was rejected");
    require(!sequence.event(1, 3, 1, "start", reason), "retry reused a stale observation");

    observe_stable(sequence, 800, three);
    require(sequence.event(1, 3, 1, "start", reason), "large restart failed");
    require(sequence.event(1, 3, 1, "start", reason) && reason == "already_applied",
            "accepted event was not idempotent");
    require(!sequence.event(1, 3, 1, "complete", reason), "same sequence changed action");
    require(sequence.event(1, 4, 1, "complete", reason), "large completion failed");
    require(sequence.snapshot().expected_count == 2 && sequence.snapshot().current_target_id == 2,
            "large completion did not select the two-nut stage");

    observe_stable(sequence, 1100, {three[2], three[1]});
    require(sequence.snapshot().targets[0].state == "completed" &&
            !sequence.snapshot().targets[0].visible, "completed large revived");
    require(sequence.snapshot().targets[1].visible && sequence.snapshot().targets[2].visible,
            "remaining targets were not visible");
    require(sequence.snapshot().targets[1].observation_index == 1 &&
            sequence.snapshot().targets[2].observation_index == 0,
            "remaining two were not remapped to medium/small by size");
    require(sequence.event(1, 5, 2, "start", reason), "medium did not start");
    require(sequence.event(1, 6, 2, "complete", reason), "medium did not complete");

    observe_stable(sequence, 1400, {three[2]});
    require(sequence.snapshot().expected_count == 1 && sequence.snapshot().current_target_id == 3,
            "one-nut stage is wrong");
    require(sequence.snapshot().targets[2].size_class == "nut_small" &&
            sequence.snapshot().targets[2].visible, "last observation was not fixed to nut_small");
    require(sequence.event(1, 7, 3, "start", reason), "small did not start");
    require(sequence.event(1, 8, 3, "complete", reason), "small did not complete");
    require(sequence.snapshot().status == "round_completed" && sequence.snapshot().expected_count == 0,
            "round did not complete after explicit feedback");

    sequence.observe(1700, true, {640, 480}, {three[2]});
    require(!sequence.snapshot().observation_valid && sequence.snapshot().status == "observation_count_mismatch",
            "visible object after completion was accepted");
    require(sequence.event(1, 9, 0, "reset", reason), "reset failed");
    require(sequence.snapshot().round == 2 && !sequence.snapshot().initialized &&
            sequence.snapshot().expected_count == 3, "reset did not start a clean round");
    require(sequence.event(1, 9, 0, "reset", reason) && sequence.snapshot().round == 2,
            "idempotent reset incremented the round twice");

    lbot_vision::NutSequence ambiguous(config());
    const std::vector<cv::Vec3f> close_sizes{{100, 100, 30}, {200, 100, 29}, {300, 100, 10}};
    ambiguous.observe(0, true, {640, 480}, close_sizes);
    require(ambiguous.snapshot().status == "ambiguous_pixel_sizes", "ambiguous sizes were accepted");

    auto bad = config();
    bad.sequence_max_gap_ms = std::numeric_limits<double>::quiet_NaN();
    bool rejected = false;
    try { lbot_vision::NutSequence invalid(bad); } catch (const std::invalid_argument &) { rejected = true; }
    require(rejected, "non-finite sequence config was accepted");

    std::cout << "Nut sequence contracts passed: stable 3-2-1 ranking, explicit feedback, retry, idempotency, reset\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
