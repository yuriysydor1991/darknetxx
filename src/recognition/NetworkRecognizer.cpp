#include "src/recognition/NetworkRecognizer.h"

#include <algorithm>
#include <chrono>
#include <exception>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "image_opencv.h"
#include "src/OMP/OMPController.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/log/log.h"
#include "src/recognition/ImageClassification.h"
#include "src/recognition/ObjectsDetection.h"
#include "src/recognition/RecognitionPainter.h"

namespace darknetxx_impl
{

darknetxx011::Recognition NetworkRecognizer::recognize(
    const LibraryContext& ctx)
{
  darknetxx011::Recognition recognition;

  // the OpenMP threads of the calling thread
  ompi::OMPController::create()->adjust_threads(ctx.threads);

  try {
    if (load(ctx, recognition.error)) {
      recognize_image(ctx, recognition);
    }
  }
  catch (const std::exception& e) {
    // the image too big for the OpenCV, for example
    recognition = {};
    recognition.error =
        "Couldn't recognize the image: " + std::string{e.what()};
    LOGE(recognition.error);
  }

  return recognition;
}

void NetworkRecognizer::recognize_image(const LibraryContext& ctx,
                                        darknetxx011::Recognition& recognition)
{
  network& net = *mnet->get();
  const cv::Mat read = cv::imread(
      ctx.image, net.c == 1 ? cv::IMREAD_GRAYSCALE : cv::IMREAD_COLOR);

  if (read.empty() || read.channels() != net.c) {
    recognition.error = read.empty() ? "Couldn't read the image " + ctx.image
                                     : "The network takes the images of " +
                                           std::to_string(net.c) + " channels";
    return;
  }

  // the darknet image of the RGB planes, the read one is painted later
  cv::Mat rgb;

  if (net.c == 3) {
    cv::cvtColor(read, rgb, cv::COLOR_BGR2RGB);
  } else {
    rgb = read;
  }

  image im = mat_to_image_cv(reinterpret_cast<mat_cv*>(&rgb));
  const auto start = std::chrono::steady_clock::now();

  recognition.objects = mmethod->recognize(net, im, ctx.thresh, mnames);
  recognition.seconds =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - start)
          .count();

  free_image(im);

  // the network order of the equally probable ones
  std::stable_sort(recognition.objects.begin(), recognition.objects.end(),
                   [](const auto& a, const auto& b) {
                     return a.probability > b.probability;
                   });

  recognition.png = RecognitionPainter::paint(read, recognition.objects);
}

bool NetworkRecognizer::load(const LibraryContext& ctx, std::string& error)
{
  using darknet_adaptor::loaders::NetworkSource;
  using darknet_adaptor::loaders::OrigDefaultLoader;

  // the dxxwjz2 weights file keeps the network it is the weights of
  const std::string& cfg =
      ctx.detect_cfg.empty() ? ctx.latest_weights : ctx.detect_cfg;
  const std::string files = ctx.latest_weights + '\n' + cfg + '\n' + ctx.names;

  if (mnet != nullptr && files == mfiles) {
    return true;
  }

  mnet.reset();

  if (ctx.latest_weights.empty()) {
    error = "No network weights file given";
  } else if (cfg == ctx.latest_weights && !NetworkSource::describes(cfg)) {
    error = "The " + cfg + " file keeps no network, give it's cfg file too";
  } else {
    try {
      mnet = OrigDefaultLoader::create()->load_inference_network(
          cfg, ctx.latest_weights, ctx.names, mnames);
    }
    catch (const std::exception& e) {
      error = "Couldn't load the network: " + std::string{e.what()};
    }
  }

  if (mnet == nullptr) {
    LOGE(error);
    return false;
  }

  mfiles = files;
  mmethod = ObjectsDetection::detection_layer(*mnet->get()) != nullptr
                ? IRecognitionMethodPtr{std::make_shared<ObjectsDetection>()}
                : std::make_shared<ImageClassification>();

  LOGI("Recognizing by the network of " << cfg << " and the weights of "
                                        << ctx.latest_weights);

  return true;
}

}  // namespace darknetxx_impl
