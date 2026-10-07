#include "src/app/ApplicationHelpPrinter.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include "LibraryFacade.h"
#include "src/app/ApplicationBinaryName.h"
#include "src/app/CMDParamNames.h"
#include "src/log/log.h"

namespace app
{

namespace
{

/// @brief Column at which the option descriptions start being printed.
constexpr std::string::size_type DESCRIPTION_COLUMN = 44;

/**
 * @brief Print a single command line option help entry in an aligned manner.
 *
 * @param out The output stream to print the entry into.
 * @param names The option name(s), for example "--help or -h".
 * @param value Placeholder for the value the option expects (for example
 * "<N>"), or an empty string for a flag that does not take any value.
 * @param description Human readable description of the option.
 */
void print_option(std::ostream& out, const std::string& names,
                  const std::string& value, const std::string& description)
{
  std::string left = "  " + names;

  if (!value.empty()) {
    left += " " + value;
  }

  out << left;

  if (left.size() + 1 < DESCRIPTION_COLUMN) {
    out << std::string(DESCRIPTION_COLUMN - left.size(), ' ');
  } else {
    out << '\n' << std::string(DESCRIPTION_COLUMN, ' ');
  }

  out << description << '\n';
}

/**
 * @brief Print the network weights files conversion options and the formats
 * they accept.
 *
 * @param out The output stream to print the options into.
 */
void print_conversion_options(std::ostream& out)
{
  out << "\nNetwork weights files conversion options (the four of them go\n"
      << "together and convert the weights file with no training nor\n"
      << "detection started, the --convert-to dxxwjz2, --convert-dst and\n"
      << "--convert-cfg ones alone make the network to train of no weights):\n";
  print_option(out, CMDParamNames::convert_fromw, "<FORMAT>",
               "the network weights file format to read");
  print_option(out, CMDParamNames::convert_srcw, "<FILE>",
               "the network weights file to read");
  print_option(out, CMDParamNames::convert_tow, "<FORMAT>",
               "the network weights file format to write");
  print_option(out, CMDParamNames::convert_dstw, "<FILE>",
               "the network weights file to write (replaced)");
  print_option(out, CMDParamNames::convert_cfgw, "<FILE>",
               "the network cfg file of the weights: required to read the "
               "weights FORMAT, validates the rest, the dxxwjz2 one keeps it");
  print_option(out, CMDParamNames::convert_namesw, "<FILE>",
               "the class names file the dxxwjz2 FORMAT keeps (overrides the "
               "cfg [net] names= key)");

  out << "\n  Where FORMAT is one of:\n";

  for (const auto& [name, description] :
       darknetxx011::LibraryFacade::conversion_formats()) {
    print_option(out, "  " + name, "", description);
  }
}

}  // namespace

int ApplicationHelpPrinter::run(std::shared_ptr<ApplicationContext> ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid application context provided");
    return INVALID;
  }

  // All the options below mirror the command line parameters handled by the
  // CommandLineParser class. When a new flag is registered there please add a
  // matching print_option() call here so the help stays in sync.
  std::ostream& out = std::cout;

  out << "Usage:\n\n"
      << '\t' << application_binary_name() << " [OPTIONS]\n\n"
      << "Introduce a new command line flag by registering it in both\n"
      << "the ApplicationHelpPrinter and the CommandLineParser classes.\n\n"
      << "Where OPTIONS may be next:\n\n";

  out << "General options:\n";
  print_option(out, CMDParamNames::HELPW + " or " + CMDParamNames::HELP, "",
               "print this help message and exit");
  print_option(out, CMDParamNames::VERSIONW + " or " + CMDParamNames::VERSION,
               "",
               "print application version, build git commit and "
               "configure date, then exit");
  print_option(out, CMDParamNames::LOGPATHW + " or " + CMDParamNames::LOGPATH,
               "<FILE>", "write the application log into the given file");

  print_conversion_options(out);

#ifdef DARKNETXX_CONVERTER_ONLY
  // the converter only binary runs no training nor detection
  out << std::endl;

  return ctx->errors.empty() ? 0 : INVALID;
#endif  // DARKNETXX_CONVERTER_ONLY

  out << "\nNetwork and model options:\n";
  print_option(out, CMDParamNames::train_cfgw, "<FILE>",
               "training network configuration (*.cfg or *.dxxwjz2) file");
  print_option(out, CMDParamNames::detect_cfgw, "<FILE>",
               "detection network configuration (*.cfg or *.dxxwjz2) file");
  print_option(out, CMDParamNames::namesw, "<FILE>",
               "class names file (overrides the dxxwjz2 classes and the cfg "
               "[net] names= key)");
  print_option(out, CMDParamNames::latest_weightsw, "<FILE>",
               "network weights file to load of any FORMAT; the dxxwjz2 one "
               "alone is detected with, or trained of no weights");
  print_option(out, CMDParamNames::detect_imagew, "<FILE>",
               "image file to run the detection on");
  print_option(out, CMDParamNames::gpusw, "<LIST>",
               "comma separated list of GPU indexes to use");
  print_option(out, CMDParamNames::threadsw, "<N>",
               "number of worker threads to use");

  out << "\nTraining options:\n";
  print_option(out, CMDParamNames::clearw, "",
               "clear (reset) the training iteration counters");
  print_option(out, CMDParamNames::mapw, "",
               "calculate mAP during the training");
  print_option(out, CMDParamNames::mAP_epochsw, "<N>",
               "calculate mAP every given number of epochs (default 4)");
  print_option(out, CMDParamNames::dont_resize_networkw, "",
               "do not resize the network during the training");
  print_option(out, CMDParamNames::netnthresizew, "<N>",
               "resize the network every Nth training iteration (default 10)");
  print_option(out, CMDParamNames::data_reload_nthw, "<N>",
               "load a new training batch every Nth iteration only");
  print_option(out, CMDParamNames::dontreload_dataw, "",
               "keep training on the loaded batch (a resize reloads it)");
  print_option(out, CMDParamNames::stop_less_avg_lossw, "<VALUE>",
               "stop the training once the average loss reaches "
               "the given value or less");
  print_option(out, CMDParamNames::save_formatw, "<FORMAT>",
               "the weights FORMAT the training saves (default dxxwjz2)");
  print_option(out, CMDParamNames::chartw, "<FILE>",
               "chart image the training chart continues (every training "
               "start saves the new chart_<base>_<date>--<time>.png, the "
               "record of the dxxwjz2 weights redraws it instead)");
  print_option(out, CMDParamNames::draw_chartw, "<FILE>",
               "draw the training chart of the dxxwjz2 --weights file into "
               "the PNG file, nothing else");
  print_option(out, CMDParamNames::save_charts_every_iterw, "",
               "save the training chart on every iteration, not every 100");
  print_option(out, CMDParamNames::dont_save_charts_every_iterw, "",
               "save the training chart every 100 iterations (the default)");
  print_option(out, CMDParamNames::draw_precisionw, "",
               "draw the mAP curve on every iteration once it's calculated");

  out << "\nDetection and output options:\n";
  print_option(out, CMDParamNames::threshw, "<VALUE>",
               "detection confidence threshold (default 0.25)");
  print_option(out, CMDParamNames::iou_threshw, "<VALUE>",
               "intersection over union threshold (default 0.5)");
  print_option(out, CMDParamNames::hierw, "<VALUE>",
               "hierarchical detection threshold (default 0.5)");
  print_option(out, CMDParamNames::pointsw, "<N>",
               "recall points of the training mAP calculation (101 for MS "
               "COCO, 11 for PascalVOC 2007, 0 for the area under the curve)");
  print_option(out, CMDParamNames::letter_boxw, "",
               "resize the image keeping the aspect ratio (letter box)");
  print_option(out, CMDParamNames::ext_outputw, "",
               "print extended output with the bounding box coordinates");
  print_option(out, CMDParamNames::save_labelsw, "",
               "save the detected labels");
  print_option(out, CMDParamNames::outw, "<FILE>",
               "file to write the results into");
  print_option(out, CMDParamNames::benchmarkw, "",
               "benchmark the detection performance (implies --dont-show)");
  print_option(out, CMDParamNames::benchmark_layersw, "",
               "benchmark every network layer performance (GPU builds only)");

  out << "\nWindow options:\n";
  print_option(
      out, CMDParamNames::showw, "",
      "show the GUI windows (enabled by default, reverts --dont-show)");
  print_option(out, CMDParamNames::dont_showw, "",
               "do not show the GUI windows");
  print_option(out, CMDParamNames::show_imgsw, "",
               "show the augmented training images");

  out << "\nNot supported yet options (the original Darknet video, streaming\n"
      << "and anchors calculation ones, rejected when given):\n";
  print_option(out, CMDParamNames::widthw + ", " + CMDParamNames::heightw,
               "<N>", "the anchors calculation input size");
  print_option(out, CMDParamNames::num_of_clustersw, "<N>",
               "the number of the anchor clusters");
  print_option(out, CMDParamNames::prefixw, "<PREFIX>",
               "the prefix of the saved video frames");
  print_option(out, CMDParamNames::out_filenamew, "<FILE>",
               "the output video file");
  print_option(out, CMDParamNames::dontdraw_bboxw, "",
               "no bounding boxes on the video frames");
  print_option(out, CMDParamNames::cam_indexw, "<N>",
               "the camera (web cam) to capture from");
  print_option(out, CMDParamNames::frame_skipw, "<N>",
               "the frames to skip between the detections");
  print_option(out, CMDParamNames::avgframesw, "<N>",
               "the frames to average the detections over");
  print_option(out, CMDParamNames::time_limit_secw, "<SEC>",
               "the video processing time limit");
  print_option(out, CMDParamNames::mjpeg_portw, "<PORT>",
               "the port to stream the MJPEG result on");
  print_option(out, CMDParamNames::json_portw, "<PORT>",
               "the port to stream the JSON result on");
  print_option(out, CMDParamNames::http_post_hostw, "<HOST>",
               "the host to send the HTTP POST results to");
  print_option(out, CMDParamNames::json_file_outputw, "<FILE>",
               "the file to write the video JSON results into");

  out << std::endl;

  // the usage printed because of an invalid command line is a failure
  return ctx->errors.empty() ? 0 : INVALID;
}

}  // namespace app
