#include "src/app/CommandLineParser.h"

#include <stdlib.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include "src/app/ApplicationContext.h"
#include "src/app/CMDParamNames.h"
#include "src/log/log.h"

namespace app
{

namespace
{

/**
 * @brief Converts the whole value text into an int.
 *
 * @throws std::invalid_argument or std::out_of_range for a malformed value,
 * including a value with trailing garbage like "10abc".
 */
int to_int(const std::string& value)
{
  std::size_t parsed = 0;
  const int result = std::stoi(value, &parsed);

  if (parsed != value.size()) {
    throw std::invalid_argument{value};
  }

  return result;
}

/// @brief The to_int for the counts, which may not be negative.
unsigned int to_unsigned(const std::string& value)
{
  const int result = to_int(value);

  if (result < 0) {
    throw std::out_of_range{value};
  }

  return static_cast<unsigned int>(result);
}

/// @brief The to_int for the float values.
float to_float(const std::string& value)
{
  std::size_t parsed = 0;
  const float result = std::stof(value, &parsed);

  if (parsed != value.size()) {
    throw std::invalid_argument{value};
  }

  return result;
}

/// @brief The to_int for the double values.
double to_double(const std::string& value)
{
  std::size_t parsed = 0;
  const double result = std::stod(value, &parsed);

  if (parsed != value.size()) {
    throw std::invalid_argument{value};
  }

  return result;
}

/**
 * @brief Checks the value to be a comma separated list of the GPU indexes.
 *
 * @throws std::invalid_argument or std::out_of_range otherwise.
 */
void check_gpus_list(const std::string& value)
{
  if (value.empty() || value.back() == ',') {
    throw std::invalid_argument{value};
  }

  std::stringstream indexes{value};
  std::string index;

  while (std::getline(indexes, index, ',')) {
    to_unsigned(index);
  }
}

/// @brief Reports the error of the command line, true for none.
bool reported(const std::shared_ptr<ApplicationContext>& ctx,
              const std::string& error)
{
  if (error.empty()) {
    return true;
  }

  ctx->print_help_and_exit = true;
  ctx->push_error(error);
  LOGE(error);

  return false;
}

/**
 * @brief Checks the network weights file conversion flags: the four of them
 * go together (or the destination ones with the network cfg one, which make
 * the network to train of no source file), the network cfg one goes with them
 * and neither of them goes with the training or the detection.
 */
bool check_conversion(const std::shared_ptr<ApplicationContext>& ctx)
{
  const bool requested = ctx->conversion_requested();
  const bool destined = !ctx->convert_to.empty() && !ctx->convert_dst.empty();
  const bool sourced = !ctx->convert_from.empty() && !ctx->convert_src.empty();
  const bool unsourced = ctx->convert_from.empty() &&
                         ctx->convert_src.empty() && !ctx->convert_cfg.empty();

  std::string error;

  if (requested && !(destined && (sourced || unsourced))) {
    error = "A conversion requires all of the " + CMDParamNames::convert_fromw +
            ", " + CMDParamNames::convert_srcw + ", " +
            CMDParamNames::convert_tow + " and " + CMDParamNames::convert_dstw +
            " parameters, or the " + CMDParamNames::convert_tow + ", " +
            CMDParamNames::convert_dstw + " and " +
            CMDParamNames::convert_cfgw +
            " ones of the dxxwjz2 or dxxwjz3 network to train";
  } else if (!requested && !ctx->convert_cfg.empty()) {
    error = "The " + CMDParamNames::convert_cfgw +
            " parameter goes with the conversion ones alone";
  } else if (!requested && !ctx->convert_names.empty()) {
    error = "The " + CMDParamNames::convert_namesw +
            " parameter goes with the conversion ones alone";
  } else if (requested &&
             (!ctx->train_cfg.empty() || !ctx->detect_cfg.empty())) {
    error = "The conversion can't be combined with the " +
            CMDParamNames::train_cfgw + " and the " +
            CMDParamNames::detect_cfgw + " parameters";
  }

  return reported(ctx, error);
}

/**
 * @brief Checks the training chart flag: it draws the chart of the --weights
 * file with no training, no detection and no conversion.
 */
bool check_chart(const std::shared_ptr<ApplicationContext>& ctx)
{
  std::string error;

  if (ctx->chart_requested() && ctx->latest_weights.empty()) {
    error = "The " + CMDParamNames::draw_chartw + " parameter requires the " +
            CMDParamNames::latest_weightsw +
            " file the training record of which it draws";
  } else if (ctx->chart_requested() &&
             (!ctx->train_cfg.empty() || !ctx->detect_cfg.empty() ||
              ctx->conversion_requested())) {
    error = "The " + CMDParamNames::draw_chartw +
            " parameter can't be combined with the " +
            CMDParamNames::train_cfgw + ", the " + CMDParamNames::detect_cfgw +
            " and the conversion parameters";
  }

  return reported(ctx, error);
}

/**
 * @brief Checks the network cfg dump flag: it prints the cfg of the file with
 * no training, no detection, no chart (of the --weights file) and no
 * conversion.
 */
bool check_dump(const std::shared_ptr<ApplicationContext>& ctx)
{
  std::string error;

  if (ctx->cfg_dump_requested() &&
      (!ctx->train_cfg.empty() || !ctx->detect_cfg.empty() ||
       !ctx->latest_weights.empty() || ctx->conversion_requested())) {
    error = "The " + CMDParamNames::dump_cfgw +
            " parameter can't be combined with the " +
            CMDParamNames::train_cfgw + ", the " + CMDParamNames::detect_cfgw +
            ", the " + CMDParamNames::latest_weightsw +
            " and the conversion parameters";
  }

  return reported(ctx, error);
}

}  // namespace

bool CommandLineParser::parse_args(std::shared_ptr<ApplicationContext> ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid application context provided");
    return false;
  }

  for (int iter = 1; iter < ctx->argc; ++iter) {
    const int nextIter = iter + 1;
    const bool hasNext = nextIter < ctx->argc;

    const std::string param = ctx->argv[iter];
    const std::string nextParam = hasNext ? ctx->argv[nextIter] : std::string{};

    if (param.empty()) {
      LOGT("Skipping empty param");
      continue;
    }

    if (!parse_arg(ctx, param, hasNext, nextParam, iter)) {
      LOGE("Failure to parse arg: " << param);
      return false;
    }
  }

  // Both modes share the network cfg of the computation, so a combination of
  // them would run one mode with the cfg of the other one.
  if (!ctx->train_cfg.empty() && !ctx->detect_cfg.empty()) {
    ctx->print_help_and_exit = true;
    ctx->push_error("The " + CMDParamNames::train_cfgw + " and the " +
                    CMDParamNames::detect_cfgw +
                    " parameters can't be combined");
    LOGE("The " << CMDParamNames::train_cfgw << " and the "
                << CMDParamNames::detect_cfgw
                << " parameters can't be combined");
    return false;
  }

  return check_conversion(ctx) && check_chart(ctx) && check_dump(ctx);
}

bool CommandLineParser::check_4_data(
    std::shared_ptr<ApplicationContext> ctx, const std::string& param,
    const bool hasNext, [[maybe_unused]] const std::string& nextParam)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid application context provided");
    return false;
  }

  const bool requiresData = requires_data(param);

  if (requiresData && !hasNext) {
    ctx->print_help_and_exit = true;
    ctx->push_error("Parameter " + param + " requires the data next to it.");
    LOGE("Parameter " << param << " requires the data next to it.");
    return false;
  }

  return true;
}

bool CommandLineParser::parse_arg(std::shared_ptr<ApplicationContext> ctx,
                                  const std::string& param, const bool hasNext,
                                  [[maybe_unused]] const std::string& nextParam,
                                  int& paramIndex)
{
  assert(ctx != nullptr);

  if (!check_4_data(ctx, param, hasNext, nextParam)) {
    LOGE("Failure with param data");
    return false;
  }

  // The value conversions throw on a malformed value, which is reported as
  // an invalid command line instead of the application termination.
  try {
    if (!parse_known_arg(ctx, param, nextParam)) {
      return false;
    }
  }
  catch (const std::logic_error&) {
    ctx->print_help_and_exit = true;
    ctx->push_error("Invalid value '" + nextParam + "' of the parameter " +
                    param);
    LOGE("Invalid value '" << nextParam << "' of the parameter " << param);
    return false;
  }

  if (hasNext && requires_data(param)) {
    LOGD("Increasing the paramIndex by one");
    paramIndex++;
  }

  return true;
}

bool CommandLineParser::parse_known_arg(std::shared_ptr<ApplicationContext> ctx,
                                        const std::string& param,
                                        const std::string& nextParam)
{
  assert(ctx != nullptr);

  // The parameters of the original Darknet modes the darknetxx has no code of
  // (the video demo, the results streaming and the anchors calculation) are
  // rejected instead of being silently ignored.
  static const std::set<std::string> notSupported{
      CMDParamNames::widthw,           CMDParamNames::heightw,
      CMDParamNames::num_of_clustersw, CMDParamNames::prefixw,
      CMDParamNames::out_filenamew,    CMDParamNames::dontdraw_bboxw,
      CMDParamNames::cam_indexw,       CMDParamNames::frame_skipw,
      CMDParamNames::avgframesw,       CMDParamNames::time_limit_secw,
      CMDParamNames::mjpeg_portw,      CMDParamNames::json_portw,
      CMDParamNames::http_post_hostw,  CMDParamNames::json_file_outputw,
  };

  if (notSupported.count(param) != 0U) {
    ctx->print_help_and_exit = true;
    ctx->push_error("The parameter " + param + " is not supported yet");
    LOGE("The parameter " << param << " is not supported yet");
    return false;
  }

  // add a new params parse over here
  // Also register new command line parameters in the ApplicationhelpPrinter's
  // help and the value taking ones in the get_params_requiring_data.
  if (param == CMDParamNames::HELPW || param == CMDParamNames::HELP) {
    ctx->print_help_and_exit = true;
  } else if (param == CMDParamNames::VERSIONW ||
             param == CMDParamNames::VERSION) {
    ctx->print_version_and_exit = true;
  } else if (param == CMDParamNames::LOGPATHW ||
             param == CMDParamNames::LOGPATH) {
    // skipping already parsed cmd params
  } else if (param == CMDParamNames::dont_showw) {
    ctx->dont_show = true;
    ctx->show = false;
  } else if (param == CMDParamNames::benchmarkw) {
    ctx->benchmark = true;
  } else if (param == CMDParamNames::benchmark_layersw) {
    ctx->benchmark_layers = true;
  } else if (param == CMDParamNames::showw) {
    ctx->show = true;
    ctx->dont_show = false;
  } else if (param == CMDParamNames::letter_boxw) {
    ctx->letter_box = true;
  } else if (param == CMDParamNames::mapw) {
    ctx->map = true;
  } else if (param == CMDParamNames::pointsw) {
    ctx->points = to_int(nextParam);
  } else if (param == CMDParamNames::show_imgsw) {
    ctx->show_imgs = true;
  } else if (param == CMDParamNames::mjpeg_portw) {
    ctx->mjpeg_port = to_int(nextParam);
  } else if (param == CMDParamNames::avgframesw) {
    ctx->avgframes = to_int(nextParam);
  } else if (param == CMDParamNames::dontdraw_bboxw) {
    ctx->dontdraw_bbox = true;
  } else if (param == CMDParamNames::json_portw) {
    ctx->json_port = to_int(nextParam);
  } else if (param == CMDParamNames::http_post_hostw) {
    ctx->http_post_host = nextParam;
  } else if (param == CMDParamNames::time_limit_secw) {
    ctx->time_limit_sec = to_int(nextParam);
  } else if (param == CMDParamNames::out_filenamew) {
    ctx->out_filename = nextParam;
  } else if (param == CMDParamNames::json_file_outputw) {
    ctx->json_file_output = nextParam;
  } else if (param == CMDParamNames::outw) {
    ctx->outfile = nextParam;
  } else if (param == CMDParamNames::prefixw) {
    ctx->prefix = nextParam;
  } else if (param == CMDParamNames::threshw) {
    ctx->thresh = to_float(nextParam);
  } else if (param == CMDParamNames::iou_threshw) {
    ctx->iou_thresh = to_float(nextParam);
  } else if (param == CMDParamNames::hierw) {
    ctx->hier_thresh = to_float(nextParam);
  } else if (param == CMDParamNames::cam_indexw) {
    ctx->cam_index = to_int(nextParam);
  } else if (param == CMDParamNames::frame_skipw) {
    ctx->frame_skip = to_int(nextParam);
  } else if (param == CMDParamNames::num_of_clustersw) {
    ctx->num_of_clusters = to_int(nextParam);
  } else if (param == CMDParamNames::widthw) {
    ctx->width = to_int(nextParam);
  } else if (param == CMDParamNames::heightw) {
    ctx->height = to_int(nextParam);
  } else if (param == CMDParamNames::ext_outputw) {
    ctx->ext_output = true;
  } else if (param == CMDParamNames::save_labelsw) {
    ctx->save_labels = true;
  } else if (param == CMDParamNames::chartw) {
    ctx->chart_path = nextParam;
  } else if (param == CMDParamNames::mAP_epochsw) {
    ctx->mAP_epochs = to_int(nextParam);
  } else if (param == CMDParamNames::gpusw) {
    check_gpus_list(nextParam);
    ctx->gpus = nextParam;
  } else if (param == CMDParamNames::clearw) {
    ctx->clear = true;
  } else if (param == CMDParamNames::dont_save_charts_every_iterw) {
    ctx->dont_save_charts_every_iter = true;
  } else if (param == CMDParamNames::save_charts_every_iterw) {
    ctx->dont_save_charts_every_iter = false;
  } else if (param == CMDParamNames::draw_precisionw) {
    ctx->draw_precision = true;
  } else if (param == CMDParamNames::train_cfgw) {
    ctx->train_cfg = nextParam;
    LOGD("Found training cfg file: " << ctx->train_cfg);
  } else if (param == CMDParamNames::detect_cfgw) {
    ctx->detect_cfg = nextParam;
    LOGD("Found detect cfg file: " << ctx->detect_cfg);
  } else if (param == CMDParamNames::namesw) {
    ctx->names = nextParam;
    LOGD("Found class names file: " << ctx->names);
  } else if (param == CMDParamNames::latest_weightsw) {
    ctx->latest_weights = nextParam;
    LOGD("Found the weights file: " << ctx->latest_weights);
  } else if (param == CMDParamNames::dont_resize_networkw) {
    ctx->dont_resize_network = true;
    LOGD("Found don't resize network flag");
  } else if (param == CMDParamNames::detect_imagew) {
    ctx->image = nextParam;
    LOGD("Found image file given: " << ctx->image);
  } else if (param == CMDParamNames::dontreload_dataw) {
    ctx->reload_data = false;
    LOGD("Found don't reload the data every iteration flag");
  } else if (param == CMDParamNames::threadsw) {
    ctx->threads = to_unsigned(nextParam);
    LOGD("Found threads parameter: " << ctx->threads);
  } else if (param == CMDParamNames::netnthresizew) {
    ctx->net_nth_resize = to_unsigned(nextParam);
    LOGD("Found every nth iteration net resize: " << ctx->net_nth_resize);
  } else if (param == CMDParamNames::data_reload_nthw) {
    ctx->nth_iteration_reload = to_unsigned(nextParam);
    LOGD(
        "Found every nth iteration data reload: " << ctx->nth_iteration_reload);
  } else if (param == CMDParamNames::stop_less_avg_lossw) {
    ctx->stop_less_avg_loss = to_double(nextParam);
    LOGD("Found stop during avg loss is reached or less: "
         << ctx->stop_less_avg_loss);
  } else if (param == CMDParamNames::save_formatw) {
    ctx->save_format = nextParam;
    LOGD("Found the weights format to save: " << ctx->save_format);
  } else if (param == CMDParamNames::draw_chartw) {
    ctx->draw_chart = nextParam;
    LOGD("Found the training chart to draw: " << ctx->draw_chart);
  } else if (param == CMDParamNames::convert_fromw) {
    ctx->convert_from = nextParam;
    LOGD("Found the weights format to convert from: " << ctx->convert_from);
  } else if (param == CMDParamNames::convert_srcw) {
    ctx->convert_src = nextParam;
    LOGD("Found the weights file to convert: " << ctx->convert_src);
  } else if (param == CMDParamNames::convert_tow) {
    ctx->convert_to = nextParam;
    LOGD("Found the weights format to convert to: " << ctx->convert_to);
  } else if (param == CMDParamNames::convert_dstw) {
    ctx->convert_dst = nextParam;
    LOGD("Found the weights file to convert into: " << ctx->convert_dst);
  } else if (param == CMDParamNames::convert_cfgw) {
    ctx->convert_cfg = nextParam;
    LOGD("Found the network cfg file of the conversion: " << ctx->convert_cfg);
  } else if (param == CMDParamNames::convert_namesw) {
    ctx->convert_names = nextParam;
    LOGD(
        "Found the class names file of the conversion: " << ctx->convert_names);
  } else if (param == CMDParamNames::dump_cfgw) {
    ctx->dump_cfg = nextParam;
    LOGD("Found the weights file of the network cfg to print: "
         << ctx->dump_cfg);
  } else {
    ctx->print_help_and_exit = true;
    ctx->push_error("Unknown parameter: " + param);
    LOGE("Unknown parameter: " << param);
    return false;
  }

  return true;
}

const std::set<std::string>& CommandLineParser::get_params_requiring_data()
{
  // Place here command line parameters that are requiring
  // some data after it.
  static const std::set<std::string> requireNext{
      CMDParamNames::LOGPATHW,
      CMDParamNames::LOGPATH,
      CMDParamNames::pointsw,
      CMDParamNames::mjpeg_portw,
      CMDParamNames::avgframesw,
      CMDParamNames::json_portw,
      CMDParamNames::http_post_hostw,
      CMDParamNames::time_limit_secw,
      CMDParamNames::out_filenamew,
      CMDParamNames::json_file_outputw,
      CMDParamNames::outw,
      CMDParamNames::prefixw,
      CMDParamNames::threshw,
      CMDParamNames::iou_threshw,
      CMDParamNames::hierw,
      CMDParamNames::cam_indexw,
      CMDParamNames::frame_skipw,
      CMDParamNames::num_of_clustersw,
      CMDParamNames::widthw,
      CMDParamNames::heightw,
      CMDParamNames::chartw,
      CMDParamNames::mAP_epochsw,
      CMDParamNames::gpusw,
      CMDParamNames::detect_cfgw,
      CMDParamNames::namesw,
      CMDParamNames::train_cfgw,
      CMDParamNames::latest_weightsw,
      CMDParamNames::detect_imagew,
      CMDParamNames::threadsw,
      CMDParamNames::netnthresizew,
      CMDParamNames::data_reload_nthw,
      CMDParamNames::stop_less_avg_lossw,
      CMDParamNames::save_formatw,
      CMDParamNames::draw_chartw,
      CMDParamNames::convert_fromw,
      CMDParamNames::convert_srcw,
      CMDParamNames::convert_tow,
      CMDParamNames::convert_dstw,
      CMDParamNames::convert_cfgw,
      CMDParamNames::convert_namesw,
      CMDParamNames::dump_cfgw};

  return requireNext;
}

bool CommandLineParser::requires_data(const std::string& param)
{
  const auto& requireNext = get_params_requiring_data();

  return std::find(requireNext.cbegin(), requireNext.cend(), param) !=
         requireNext.cend();
}

std::string CommandLineParser::get_custom_logfile(const int& gargc,
                                                  char** const& gargv)
{
  std::string logf;

  for (int iter = 1; iter < gargc; ++iter) {
    if ((gargv[iter] == CMDParamNames::LOGPATHW ||
         gargv[iter] == CMDParamNames::LOGPATH) &&
        (iter + 1) < gargc) {
      logf = gargv[iter + 1];
    }
  }

  return logf;
}

}  // namespace app
