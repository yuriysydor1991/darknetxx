#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_COMMANDLINEPARAMETERNAMES_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_COMMANDLINEPARAMETERNAMES_CLASS_H

#include <memory>
#include <set>

#include "src/app/ApplicationContext.h"

namespace app
{

/**
 * @brief Class for encapsulating the command line parameters.
 */
class CMDParamNames
{
 public:
  virtual ~CMDParamNames() = default;
  CMDParamNames() = default;

  inline static const std::string HELPW{"--help"};
  inline static const std::string HELP{"-h"};
  inline static const std::string VERSIONW{"--version"};
  inline static const std::string VERSION{"-v"};
  inline static const std::string LOGPATHW{"--log-file"};
  inline static const std::string LOGPATH{"-l"};

  /// @brief darknet related slightly modified cmd params names
  inline static const std::string dont_showw{"--dont-show"};
  inline static const std::string benchmarkw{"--benchmark"};
  inline static const std::string benchmark_layersw{"--benchmark-layers"};
  inline static const std::string showw{"--show"};
  inline static const std::string letter_boxw{"--letter-box"};
  inline static const std::string mapw{"--map"};
  inline static const std::string pointsw{"--points"};
  inline static const std::string show_imgsw{"--show-imgs"};
  inline static const std::string mjpeg_portw{"--mjpeg-port"};
  inline static const std::string avgframesw{"--avg-frames"};
  inline static const std::string dontdraw_bboxw{"--dontdraw-bbox"};
  inline static const std::string json_portw{"--json-port"};
  inline static const std::string http_post_hostw{"--http-post-host"};
  inline static const std::string time_limit_secw{"--time-limit-sec"};
  inline static const std::string out_filenamew{"--out-filename"};
  inline static const std::string json_file_outputw{"--json-file-output"};
  inline static const std::string outw{"--out"};
  inline static const std::string prefixw{"--prefix"};
  inline static const std::string threshw{"--thresh"};
  inline static const std::string iou_threshw{"--iou-thresh"};
  inline static const std::string hierw{"--hier"};
  inline static const std::string cam_indexw{"--cam-index"};
  inline static const std::string frame_skipw{"--frame-skip"};
  inline static const std::string num_of_clustersw{"--num-of-clusters"};
  inline static const std::string widthw{"--width"};
  inline static const std::string heightw{"--height"};
  inline static const std::string ext_outputw{"--ext-output"};
  inline static const std::string save_labelsw{"--save-labels"};
  inline static const std::string chartw{"--chart-path"};
  inline static const std::string mAP_epochsw{"--mAP-epochs"};
  inline static const std::string gpusw{"--gpus"};
  inline static const std::string clearw{"--clear"};
  inline static const std::string dont_save_charts_every_iterw{
      "--dont-save-charts-every-iter"};
  inline static const std::string draw_precisionw{"--draw-precision"};

  /// @brief newly introduced
  inline static const std::string train_cfgw{"--train-cfg"};
  inline static const std::string detect_cfgw{"--detect-cfg"};
  inline static const std::string latest_weightsw{"--weights"};
  inline static const std::string dont_resize_networkw{"--dont-resize-network"};
  inline static const std::string detect_imagew{"--image"};
  inline static const std::string dontreload_dataw{
      "--dont-reload-data-every-iteration"};
  inline static const std::string threadsw{"--threads"};
  inline static const std::string netnthresizew{"--net-resize-nth"};
  inline static const std::string data_reload_nthw{"--data-reload-nth"};
  inline static const std::string stop_less_avg_lossw{"--stop-less-avg-loss"};
};

}  // namespace app

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_COMMANDLINEPARAMETERNAMES_CLASS_H
