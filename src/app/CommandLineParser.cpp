#include "src/app/CommandLineParser.h"

#include <stdlib.h>

#include <algorithm>
#include <cassert>
#include <memory>
#include <set>
#include <string>

#include "src/app/ApplicationContext.h"
#include "src/app/CMDParamNames.h"
#include "src/log/log.h"

namespace app
{

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

  return true;
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
    ctx->print_version_and_exit = true;
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

  // add a new params parse over here
  // Also register new command line parameters in the ApplicationhelpPrinter's
  // help.
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
  } else if (param == CMDParamNames::benchmarkw) {
    ctx->benchmark = true;
  } else if (param == CMDParamNames::showw) {
    ctx->show = true;
  } else if (param == CMDParamNames::letter_boxw) {
    ctx->letter_box = true;
  } else if (param == CMDParamNames::mapw) {
    ctx->map = true;
  } else if (param == CMDParamNames::pointsw) {
    ctx->points = std::stoi(nextParam);
  } else if (param == CMDParamNames::show_imgsw) {
    ctx->show_imgs = true;
  } else if (param == CMDParamNames::mjpeg_portw) {
    ctx->mjpeg_port = std::stoi(nextParam);
  } else if (param == CMDParamNames::avgframesw) {
    ctx->avgframes = std::stoi(nextParam);
  } else if (param == CMDParamNames::dontdraw_bboxw) {
    ctx->dontdraw_bbox = false;
  } else if (param == CMDParamNames::json_portw) {
    ctx->json_port = std::stoi(nextParam);
  } else if (param == CMDParamNames::http_post_hostw) {
    ctx->http_post_host = nextParam;
  } else if (param == CMDParamNames::time_limit_secw) {
    ctx->time_limit_sec = std::stoi(nextParam);
  } else if (param == CMDParamNames::out_filenamew) {
    ctx->out_filename = nextParam;
  } else if (param == CMDParamNames::json_file_outputw) {
    ctx->json_file_output = nextParam;
  } else if (param == CMDParamNames::outw) {
    ctx->outfile = nextParam;
  } else if (param == CMDParamNames::prefixw) {
    ctx->prefix = nextParam;
  } else if (param == CMDParamNames::threshw) {
    ctx->thresh = std::stof(nextParam);
  } else if (param == CMDParamNames::iou_threshw) {
    ctx->iou_thresh = std::stof(nextParam);
  } else if (param == CMDParamNames::hierw) {
    ctx->hier_thresh = std::stof(nextParam);
  } else if (param == CMDParamNames::cam_indexw) {
    ctx->cam_index = std::stoi(nextParam);
  } else if (param == CMDParamNames::frame_skipw) {
    ctx->frame_skip = std::stoi(nextParam);
  } else if (param == CMDParamNames::num_of_clustersw) {
    ctx->num_of_clusters = std::stoi(nextParam);
  } else if (param == CMDParamNames::widthw) {
    ctx->width = std::stoi(nextParam);
  } else if (param == CMDParamNames::heightw) {
    ctx->height = std::stoi(nextParam);
  } else if (param == CMDParamNames::ext_outputw) {
    ctx->ext_output = true;
  } else if (param == CMDParamNames::save_labelsw) {
    ctx->save_labels = true;
  } else if (param == CMDParamNames::chartw) {
    ctx->chart_path = nextParam;
  } else if (param == CMDParamNames::mAP_epochsw) {
    ctx->mAP_epochs = std::stoi(nextParam);
  } else if (param == CMDParamNames::gpusw) {
    ctx->gpus = nextParam;
  } else if (param == CMDParamNames::clearw) {
    ctx->clear = true;
  } else if (param == CMDParamNames::dont_save_charts_every_iterw) {
    ctx->dont_save_charts_every_iter = true;
  } else if (param == CMDParamNames::draw_precisionw) {
    ctx->draw_precision = true;
  } else if (param == CMDParamNames::train_cfgw) {
    ctx->train_cfg = nextParam;
    LOGD("Found training cfg file: " << ctx->train_cfg);
  } else if (param == CMDParamNames::detect_cfgw) {
    ctx->detect_cfg = nextParam;
    LOGD("Found detect cfg file: " << ctx->detect_cfg);
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
    LOGD("Found image file given: " << ctx->image);
  } else if (param == CMDParamNames::threadsw) {
    ctx->threads = static_cast<unsigned int>(std::stoi(nextParam));
    LOGD("Found threads parameter: " << ctx->threads);
  } else if (param == CMDParamNames::netnthresizew) {
    /// @todo: stoi may fire exception here
    ctx->net_nth_resize = static_cast<unsigned int>(std::stoi(nextParam));
    LOGD("Found every nth iteration net resize: " << ctx->threads);
  } else if (param == CMDParamNames::data_reload_nthw) {
    /// @todo: stoi may fire exception here
    ctx->nth_iteration_reload = static_cast<unsigned int>(std::stoi(nextParam));
    LOGD(
        "Found every nth iteration data reload: " << ctx->nth_iteration_reload);
  } else if (param == CMDParamNames::stop_less_avg_lossw) {
    /// @todo: stoi may fire exception here
    ctx->stop_less_avg_loss = std::stod(nextParam);
    LOGD("Found stop during avg loss is reached or less: "
         << ctx->stop_less_avg_loss);
  } else {
    ctx->print_help_and_exit = true;
    ctx->push_error("Unknown parameter: " + param);
    LOGE("Unknown parameter: " << param);
    return false;
  }

  if (hasNext && requires_data(param)) {
    LOGD("Increasing the paramIndex by one");
    paramIndex++;
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
      CMDParamNames::threshw,
      CMDParamNames::iou_threshw,
      CMDParamNames::hierw,
      CMDParamNames::cam_indexw,
      CMDParamNames::num_of_clustersw,
      CMDParamNames::widthw,
      CMDParamNames::heightw,
      CMDParamNames::ext_outputw,
      CMDParamNames::chartw,
      CMDParamNames::mAP_epochsw,
      CMDParamNames::gpusw,
      CMDParamNames::detect_cfgw,
      CMDParamNames::train_cfgw,
      CMDParamNames::latest_weightsw,
      CMDParamNames::detect_imagew,
      CMDParamNames::threadsw,
      CMDParamNames::netnthresizew,
      CMDParamNames::data_reload_nthw,
      CMDParamNames::stop_less_avg_lossw};

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
