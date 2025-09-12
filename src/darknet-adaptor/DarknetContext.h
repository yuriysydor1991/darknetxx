#ifndef DARKNET_CXX_PORT_PROJECT_DARKNETCONTEXT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DARKNETCONTEXT_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/darknet-adaptor/DarknetStrings.h"
#include "src/detector/detector_context.h"

namespace darknet_adaptor
{

/**
 * @brief The adaptors data context class.
 */
class DarknetContext
{
 public:
  using AppCtxPtr = app::AppCtxPtr;
  using DarknetContextPtr = std::shared_ptr<DarknetContext>;
  using detector_context_ptr = std::shared_ptr<detector_context>;
  using stop_subs = app::ApplicationContext::stop_subs;

  virtual ~DarknetContext();
  explicit DarknetContext(AppCtxPtr nactx);

  bool init();

  detector_context_ptr get_darknet_ctx();

  const bool& get_Dont_show() const;
  void set_Dont_show(const bool& ndont_show);
  void set_Dont_show(bool&& ndont_show);
  const bool& get_Benchmark() const;
  void set_Benchmark(const bool& nbenchmark);
  void set_Benchmark(bool&& nbenchmark);
  const bool& get_Benchmark_layers() const;
  void set_Benchmark_layers(const bool& nbenchmark_layers);
  void set_Benchmark_layers(bool&& nbenchmark_layers);
  const bool& get_Show() const;
  void set_Show(const bool& nshow);
  void set_Show(bool&& nshow);
  const bool& get_Letter_box() const;
  void set_Letter_box(const bool& nletter_box);
  void set_Letter_box(bool&& nletter_box);
  const bool& get_Map() const;
  void set_Map(const bool& nmap);
  void set_Map(bool&& nmap);
  const int& get_Points() const;
  void set_Points(const int& npoints);
  void set_Points(int&& npoints);
  const bool& get_Show_imgs() const;
  void set_Show_imgs(const bool& nshow_imgs);
  void set_Show_imgs(bool&& nshow_imgs);
  const int& get_Mjpeg_port() const;
  void set_Mjpeg_port(const int& nmjpeg_port);
  void set_Mjpeg_port(int&& nmjpeg_port);
  const int& get_Avgframes() const;
  void set_Avgframes(const int& navgframes);
  void set_Avgframes(int&& navgframes);
  const bool& get_Dontdraw_bbox() const;
  void set_Dontdraw_bbox(const bool& ndontdraw_bbox);
  void set_Dontdraw_bbox(bool&& ndontdraw_bbox);
  const int& get_Json_port() const;
  void set_Json_port(const int& njson_port);
  void set_Json_port(int&& njson_port);
  const std::string& get_Http_post_host() const;
  void set_Http_post_host(const std::string& nhttp_post_host);
  void set_Http_post_host(std::string&& nhttp_post_host);
  const int& get_Time_limit_sec() const;
  void set_Time_limit_sec(const int& ntime_limit_sec);
  void set_Time_limit_sec(int&& ntime_limit_sec);
  const std::string& get_Out_filename() const;
  void set_Out_filename(const std::string& nout_filename);
  void set_Out_filename(std::string&& nout_filename);
  const std::string& get_Json_file_output() const;
  void set_Json_file_output(const std::string& njson_file_output);
  void set_Json_file_output(std::string&& njson_file_output);
  const std::string& get_Outfile() const;
  void set_Outfile(const std::string& noutfile);
  void set_Outfile(std::string&& noutfile);
  const std::string& get_Prefix() const;
  void set_Prefix(const std::string& nprefix);
  void set_Prefix(std::string&& nprefix);
  const float& get_Thresh() const;
  void set_Thresh(const float& nthresh);
  void set_Thresh(float&& nthresh);
  const float& get_Iou_thresh() const;
  void set_Iou_thresh(const float& niou_thresh);
  void set_Iou_thresh(float&& niou_thresh);
  const float& get_Hier_thresh() const;
  void set_Hier_thresh(const float& nhier_thresh);
  void set_Hier_thresh(float&& nhier_thresh);
  const int& get_Cam_index() const;
  void set_Cam_index(const int& ncam_index);
  void set_Cam_index(int&& ncam_index);
  const int& get_Frame_skip() const;
  void set_Frame_skip(const int& nframe_skip);
  void set_Frame_skip(int&& nframe_skip);
  const int& get_Num_of_clusters() const;
  void set_Num_of_clusters(const int& nnum_of_clusters);
  void set_Num_of_clusters(int&& nnum_of_clusters);
  const int& get_Width() const;
  void set_Width(const int& nwidth);
  void set_Width(int&& nwidth);
  const int& get_Height() const;
  void set_Height(const int& nheight);
  void set_Height(int&& nheight);
  const bool& get_Ext_output() const;
  void set_Ext_output(const bool& next_output);
  void set_Ext_output(bool&& next_output);
  const bool& get_Save_labels() const;
  void set_Save_labels(const bool& nsave_labels);
  void set_Save_labels(bool&& nsave_labels);
  const std::string& get_Chart_path() const;
  void set_Chart_path(const std::string& nchart_path);
  void set_Chart_path(std::string&& nchart_path);
  const int& get_MAP_epochs() const;
  void set_MAP_epochs(const int& nmAP_epochs);
  void set_MAP_epochs(int&& nmAP_epochs);
  const std::string& get_Gpus() const;
  void set_Gpus(const std::string& ngpus);
  void set_Gpus(std::string&& ngpus);
  const bool& get_Clear() const;
  void set_Clear(const bool& nclear);
  void set_Clear(bool&& nclear);
  bool get_Dont_save_charts_every_iter() const;
  void set_Dont_save_charts_every_iter(
      const bool& ndont_save_charts_every_iter);
  void set_Dont_save_charts_every_iter(bool&& ndont_save_charts_every_iter);
  const bool& get_Draw_precision() const;
  void set_Draw_precision(const bool& ndraw_precision);
  void set_Draw_precision(bool&& ndraw_precision);
  const std::string& get_Train_cfg() const;
  void set_Train_cfg(const std::string& ntrain_cfg);
  void set_Train_cfg(std::string&& ntrain_cfg);
  const std::string& get_Detect_cfg() const;
  void set_Detect_cfg(const std::string& ndetect_cfg);
  void set_Detect_cfg(std::string&& ndetect_cfg);
  const std::string& get_Latest_weights() const;
  void set_Latest_weights(const std::string& nlatest_weights);
  void set_Latest_weights(std::string&& nlatest_weights);
  bool get_Dont_resize_network() const;
  void set_Dont_resize_network(const bool& ndont_resize_network);
  const std::string& get_image() const;
  void set_image(const std::string& nimage);
  void set_image(std::string&& nimage);

  bool get_reload_data() const;
  void set_reload_data(const bool nreload_data);

  void set_stop(const bool nstop);
  bool get_stop();

  void set_net_nth_resize(const unsigned int& nth);
  const unsigned int& get_net_nth_resize();

  void nth_iteration_reload(const unsigned int& nth);
  const unsigned int& nth_iteration_reload();

  void stop_less_avg_loss(const double& avglosslim);
  const double& stop_less_avg_loss();

 private:
  static detector_context_ptr create_subcontext();

  static std::vector<int> prepare_gpus_list(
      const std::vector<std::string>& src);

  bool propagate_context_params();

  app::ApplicationContext::stop_subs mysub;

  /// @brief The app context
  AppCtxPtr actx;

  detector_context_ptr internal;

  std::string http_post_host;
  std::string out_filename;
  std::string json_file_output;
  std::string outfile;
  std::string prefix;
  std::string chart_path;
  std::string gpus;
  std::string train_cfg;
  std::string detect_cfg;
  std::vector<int> gpuslist;
  std::string latest_weights;
  std::string image;
};

using DarknetContextPtr = DarknetContext::DarknetContextPtr;

}  // namespace darknet_adaptor

#endif  // DARKNET_CXX_PORT_PROJECT_DARKNETCONTEXT_CLASS_H
