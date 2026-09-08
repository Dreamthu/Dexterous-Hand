#include "offline_io.hpp"
#include <exception>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

int main(int argc, char **argv)
{
  try {
    if (argc == 3 && std::string(argv[1]) == "--check-config") {
      lbot_vision::offline::load_config(argv[2]);
      std::cout << "Configuration valid; OpenCV " << CV_VERSION << '\n';
      return 0;
    }
    if (argc != 3) {
      std::cerr << "Internal adapter usage: nut_detector_offline CONFIG_JSON IMAGE\n";
      return 2;
    }
    const auto config = lbot_vision::offline::load_config(argv[1]);
    // IGNORE_ORIENTATION keeps pixels in the same raw orientation as ROS BGR8.
    const auto image = cv::imread(argv[2], cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION);
    const auto result = lbot_vision::detect_2d(image, config);
    lbot_vision::offline::write_result(result, image.size(), "result.json");
    auto save = [](const std::string &name, const cv::Mat &mat) {
      if (!mat.empty() && !cv::imwrite(name + ".png", mat))
        throw std::runtime_error("Cannot write debug image: " + name);
    };
    save("annotated", result.annotated);
    save("value_channel", result.value_channel);
    save("black_mask_before_close", result.black_mask_before_close);
    save("black_mask", result.black_mask);
    save("frame_candidates", result.frame_candidate_debug);
    save("blue_mask", result.blue_mask);
    save("roi_mask", result.roi_mask);
    save("adaptive_mask", result.adaptive_mask);
    save("blackhat_mask", result.blackhat_mask);
    auto rejected = result.annotated.clone();
    for (size_t i = 0; i < result.candidates.size(); ++i) {
      const auto &c = result.candidates[i];
      if (c.accepted) continue;
      if (!c.contour.empty()) cv::polylines(rejected, c.contour, true, cv::Scalar(0, 165, 255), 1);
      cv::putText(rejected, std::to_string(i) + ":" + c.reason, c.center,
                  cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 165, 255), 1);
    }
    save("rejected", rejected);
    if (result.black_frame_debug_enabled) {
      for (const auto &candidate : result.frame_candidates) {
        if (!candidate.accepted) continue;
        std::cout << "frame area=" << candidate.area
                  << " bounding_rect=[" << candidate.bounding_rect.x << ','
                  << candidate.bounding_rect.y << ',' << candidate.bounding_rect.width << ','
                  << candidate.bounding_rect.height << "] vertices=" << candidate.vertex_count
                  << " aspect_ratio=" << candidate.aspect_ratio
                  << " fill_ratio=" << candidate.fill_ratio
                  << " mean_gray=" << candidate.mean_gray
                  << " score=" << candidate.score << '\n';
      }
    }
    std::cout << "OpenCV " << CV_VERSION << "; selected=" << result.circles.size() << '\n';
    return 0;
  } catch (const std::exception &error) {
    std::cerr << "Offline detector: " << error.what() << '\n';
    return 2;
  }
}
