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

// Synthetic fixtures assign metric diameters explicitly; production callers
// must supply the detector's independently rectified sizes.
void observe(lbot_vision::NutSequence &sequence, std::int64_t stamp, bool frame,
             cv::Size image_size, const std::vector<cv::Vec3f> &observations)
{
  std::vector<double> sizes;
  for (const auto &observation : observations) sizes.push_back(2.0 * observation[2]);
  sequence.observe(stamp, frame, image_size, observations, sizes);
}

void observe_stable(lbot_vision::NutSequence &sequence, std::int64_t first_stamp,
                    const std::vector<cv::Vec3f> &observations)
{
  for (int i = 0; i < 3; ++i) {
    observe(sequence, first_stamp + i * 100, true, {640, 480}, observations);
  }
}

}  // namespace

int main()
{
  try {
    const std::vector<cv::Vec3f> three{{100, 100, 30}, {250, 120, 20}, {400, 140, 10}};
    std::string reason;

    // Perspective reverses the pixel-radius order; identities must use mm.
    lbot_vision::NutSequence perspective(config());
    for (int i = 0; i < 3; ++i)
      perspective.observe(i * 100, true, {640, 480}, three, {15.0, 25.0, 40.0});
    require(perspective.snapshot().observation_valid, "metric sequence did not confirm");
    require(perspective.snapshot().targets[0].observation_index == 2 &&
            perspective.snapshot().targets[2].observation_index == 0,
            "pixel radius overrode rectified size");
    require(perspective.snapshot().targets[0].last_observation[2] == 10.0F,
            "metric size corrupted the pixel-radius diagnostic");
    perspective.observe(300, true, {640, 480}, three, {});
    require(perspective.snapshot().status == "invalid_rectified_sizes",
            "missing sizes fell back to pixel classification");
    perspective.observe(400, true, {640, 480}, three,
                        {15.0, std::numeric_limits<double>::quiet_NaN(), 40.0});
    require(!perspective.snapshot().observation_valid, "non-finite size accepted");

    lbot_vision::NutSequence cold(config());
    observe(cold, 0, true, {640, 480}, {});
    require(!cold.snapshot().initialized, "zero observations initialized the sequence");
    observe(cold, 100, true, {640, 480}, {three[1], three[2]});
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
            initial.targets[2].observation_index == 1, "initial metric-size ranking is wrong");

    observe(sequence, 300, true, {640, 480}, {three[1], three[2]});
    require(sequence.snapshot().current_target_id == 1, "a missing nut advanced the stage");
    require(sequence.snapshot().targets[0].state == "pending", "a missing nut completed large");
    require(!sequence.snapshot().observation_valid, "count mismatch remained graspable");
    require(!sequence.event(1, 1, 2, "start", reason), "medium started before large");

    observe_stable(sequence, 400, three);
    require(sequence.event(1, 1, 1, "start", reason), "large did not start");
    observe(sequence, 700, true, {640, 480}, {three[1], three[2]});
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

    observe(sequence, 1700, true, {640, 480}, {three[2]});
    require(!sequence.snapshot().observation_valid && sequence.snapshot().status == "observation_count_mismatch",
            "visible object after completion was accepted");
    require(sequence.event(1, 9, 0, "reset", reason), "reset failed");
    require(sequence.snapshot().round == 2 && !sequence.snapshot().initialized &&
            sequence.snapshot().expected_count == 3, "reset did not start a clean round");
    require(sequence.event(1, 9, 0, "reset", reason) && sequence.snapshot().round == 2,
            "idempotent reset incremented the round twice");

    lbot_vision::NutSequence ambiguous(config());
    const std::vector<cv::Vec3f> close_sizes{{100, 100, 30}, {200, 100, 29}, {300, 100, 10}};
    observe(ambiguous, 0, true, {640, 480}, close_sizes);
    require(ambiguous.snapshot().status == "ambiguous_rectified_sizes", "ambiguous sizes were accepted");

    auto relaxed = config();
    relaxed.sequence_confirmation_window_ms = 3000.0;
    relaxed.sequence_min_size_gap_ratio = 0.0;
    lbot_vision::NutSequence intermittent(relaxed);
    observe(intermittent, 0, true, {640, 480}, close_sizes);
    observe(intermittent, 200, false, {640, 480}, {});
    require(!intermittent.snapshot().observation_valid, "missing frame reused old target positions");
    // Radii may fluctuate and change ranking; spatially matching detections
    // still confirm the same group, even across a gap longer than max_gap_ms.
    auto fluctuating = close_sizes;
    fluctuating[0][2] = 25;
    observe(intermittent, 1600, true, {640, 480}, fluctuating);
    require(!intermittent.snapshot().initialized, "two hits passed the three-hit threshold");
    observe(intermittent, 1700, true, {640, 480}, {close_sizes[0]});
    observe(intermittent, 2500, true, {640, 480}, close_sizes);
    require(intermittent.snapshot().observation_valid, "nonconsecutive hits did not confirm the group");
    require(intermittent.snapshot().targets[0].last_seen_ms == 2500,
            "confirmation did not use the current observation");
    require(intermittent.event(1, 1, 1, "start", reason), "confirmed target could not start");
    require(intermittent.event(1, 2, 1, "retry", reason), "confirmed target could not retry");
    observe(intermittent, 2600, true, {640, 480}, close_sizes);
    require(!intermittent.snapshot().observation_valid, "retry reused pre-action confirmations");
    observe(intermittent, 2700, true, {640, 480}, close_sizes);
    observe(intermittent, 2800, true, {640, 480}, close_sizes);
    require(intermittent.event(1, 3, 1, "start", reason), "fresh retry confirmations failed");
    require(intermittent.event(1, 4, 1, "complete", reason), "confirmed target could not complete");
    observe(intermittent, 2900, true, {640, 480}, {close_sizes[1], close_sizes[2]});
    require(!intermittent.snapshot().observation_valid, "next stage reused previous target confirmations");
    require(intermittent.event(1, 5, 0, "reset", reason), "windowed reset failed");
    observe(intermittent, 3000, true, {640, 480}, close_sizes);
    require(!intermittent.snapshot().initialized, "reset reused previous-round confirmations");

    lbot_vision::NutSequence expired(relaxed);
    observe(expired, 0, true, {640, 480}, three);
    observe(expired, 200, true, {640, 480}, three);
    observe(expired, 3300, true, {640, 480}, three);
    require(!expired.snapshot().observation_valid, "expired hits counted toward confirmation");
    observe(expired, 3400, true, {640, 480}, three);
    require(!expired.snapshot().observation_valid, "expired history was revived");
    observe(expired, 3500, true, {640, 480}, three);
    require(expired.snapshot().observation_valid, "fresh window did not confirm");

    lbot_vision::NutSequence moved(relaxed);
    observe(moved, 0, true, {640, 480}, three);
    observe(moved, 100, true, {640, 480}, three);
    auto elsewhere = three;
    for (auto &circle : elsewhere) circle[1] += 100;
    observe(moved, 200, true, {640, 480}, elsewhere);
    require(!moved.snapshot().observation_valid, "unrelated target locations shared confirmations");
    observe(moved, 200, true, {640, 480}, elsewhere);
    require(moved.snapshot().status == "non_increasing_timestamp", "duplicate frame counted as a hit");
    observe(moved, 300, true, {640, 480}, elsewhere);
    require(!moved.snapshot().observation_valid, "duplicate frame preserved confirmations");

    auto invalid_window = config();
    invalid_window.sequence_confirmation_window_ms = -1.0;
    bool invalid_window_rejected = false;
    try { lbot_vision::NutSequence invalid(invalid_window); }
    catch (const std::invalid_argument &) { invalid_window_rejected = true; }
    require(invalid_window_rejected, "negative confirmation window was accepted");

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
