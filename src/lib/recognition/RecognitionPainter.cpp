#include "src/lib/recognition/RecognitionPainter.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

#include "src/log/log.h"

namespace darknetxx_impl
{

std::vector<unsigned char> RecognitionPainter::paint(
    const cv::Mat& image,
    const std::vector<darknetxx011::RecognizedObject>& objects)
{
  // the size the small images are enlarged to and the bright colors, which
  // the black labels are read on
  static constexpr const int MIN_SIZE = 480;
  static const cv::Scalar COLORS[] = {
      {255, 0, 255}, {255, 128, 0}, {255, 255, 0},   {0, 255, 0},
      {0, 255, 255}, {0, 128, 255}, {128, 128, 255}, {0, 0, 255}};

  const int zoom = std::max(1, MIN_SIZE / std::max(image.rows, image.cols));
  const int size = std::max(image.rows, image.cols) * zoom;
  const int thickness = std::max(1, static_cast<int>(std::lround(size / 320.)));
  const double scale = std::max(.5, size / 1000.);
  std::vector<unsigned char> png;

  try {
    cv::Mat canvas;

    cv::resize(image, canvas, {}, zoom, zoom, cv::INTER_NEAREST);

    if (canvas.channels() == 1) {
      cv::cvtColor(canvas, canvas, cv::COLOR_GRAY2BGR);
    }

    for (const auto& object : objects) {
      if (!object.boxed()) {
        continue;
      }

      const cv::Scalar& color =
          COLORS[std::hash<std::string>{}(object.name) % std::size(COLORS)];
      const cv::Rect box{object.x * zoom, object.y * zoom, object.width * zoom,
                         object.height * zoom};
      const std::string label =
          object.name + " " +
          std::to_string(std::lround(object.probability * 100.f)) + "%";
      int baseline = 0;
      const cv::Size text = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX,
                                            scale, thickness, &baseline);
      // above the box, or inside it at the top of the image
      const cv::Point corner{box.x, std::max(box.y, text.height + baseline)};

      cv::rectangle(canvas, box, color, thickness);
      cv::rectangle(canvas, corner - cv::Point{0, text.height + baseline},
                    corner + cv::Point{text.width, 0}, color, cv::FILLED);
      cv::putText(canvas, label, corner - cv::Point{0, baseline},
                  cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar::all(0),
                  thickness, cv::LINE_AA);
    }

    // the fast compression, the image is shown at once
    if (!cv::imencode(".png", canvas, png, {cv::IMWRITE_PNG_COMPRESSION, 1})) {
      png.clear();
    }
  }
  catch (const cv::Exception& e) {
    LOGE("Fail to draw the recognized objects: " << e.what());
    png.clear();
  }

  return png;
}

}  // namespace darknetxx_impl
