#ifndef DARKNET_CXX_PORT_PROJECT_DARKNETCONTEXT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DARKNETCONTEXT_CLASS_H

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "src/darknet-adaptor/DarknetStrings.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/Yolo4Annotations.h"
#include "src/detector/detector_context.h"
#include "src/lib/facade/public/LibraryContext.h"

namespace darknet_adaptor
{

namespace adaptors
{
class ANetworkAdaptor;
class TrainingJournal;
}  // namespace adaptors

/**
 * @brief The adaptors data context class.
 */
class DarknetContext
{
 public:
  using LibCtxPtr = darknetxx011::LibraryContextPtr;
  using DarknetContextPtr = std::shared_ptr<DarknetContext>;
  using detector_context_ptr = std::shared_ptr<detector_context>;
  using ANetworkAdaptorPtr = std::shared_ptr<adaptors::ANetworkAdaptor>;
  using ANetworkAdaptorPtrs = std::vector<ANetworkAdaptorPtr>;
  using TrainingJournalPtr = std::shared_ptr<adaptors::TrainingJournal>;

  /// @brief Releases the DarknetContext::watch_the_app_stop_flag poller and
  /// the networks of the computation context.
  virtual ~DarknetContext();
  explicit DarknetContext(LibCtxPtr nlctx);

  bool init();

  detector_context_ptr get_darknet_ctx();

  bool get_Dont_show() const;
  void set_Dont_show(const bool& ndont_show);
  void set_Dont_show(bool&& ndont_show);
  bool get_Benchmark() const;
  void set_Benchmark(const bool& nbenchmark);
  void set_Benchmark(bool&& nbenchmark);
  bool get_Benchmark_layers() const;
  void set_Benchmark_layers(const bool& nbenchmark_layers);
  void set_Benchmark_layers(bool&& nbenchmark_layers);
  bool get_Show() const;
  void set_Show(const bool& nshow);
  void set_Show(bool&& nshow);
  bool get_Letter_box() const;
  void set_Letter_box(const bool& nletter_box);
  void set_Letter_box(bool&& nletter_box);
  bool get_Map() const;
  void set_Map(const bool& nmap);
  void set_Map(bool&& nmap);
  const int& get_Points() const;
  void set_Points(const int& npoints);
  void set_Points(int&& npoints);
  bool get_Show_imgs() const;
  void set_Show_imgs(const bool& nshow_imgs);
  void set_Show_imgs(bool&& nshow_imgs);
  const int& get_Mjpeg_port() const;
  void set_Mjpeg_port(const int& nmjpeg_port);
  void set_Mjpeg_port(int&& nmjpeg_port);
  const int& get_Avgframes() const;
  void set_Avgframes(const int& navgframes);
  void set_Avgframes(int&& navgframes);
  bool get_Dontdraw_bbox() const;
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
  bool get_Ext_output() const;
  void set_Ext_output(const bool& next_output);
  void set_Ext_output(bool&& next_output);
  bool get_Save_labels() const;
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
  bool get_Clear() const;
  void set_Clear(const bool& nclear);
  void set_Clear(bool&& nclear);
  bool get_Dont_save_charts_every_iter() const;
  void set_Dont_save_charts_every_iter(
      const bool& ndont_save_charts_every_iter);
  void set_Dont_save_charts_every_iter(bool&& ndont_save_charts_every_iter);
  bool get_Draw_precision() const;
  void set_Draw_precision(const bool& ndraw_precision);
  void set_Draw_precision(bool&& ndraw_precision);
  const std::string& get_Train_cfg() const;
  void set_Train_cfg(const std::string& ntrain_cfg);
  void set_Train_cfg(std::string&& ntrain_cfg);
  const std::string& get_Detect_cfg() const;
  void set_Detect_cfg(const std::string& ndetect_cfg);
  void set_Detect_cfg(std::string&& ndetect_cfg);
  const std::string& get_Names() const;
  void set_Names(const std::string& nnames);
  void set_Names(std::string&& nnames);
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

  const std::string& get_Save_format() const;

  /**
   * @brief Hands the journal of the training to the computation: the saved
   * weights files keep it's record, the new chart starts with it's losses
   * and mAPs.
   */
  void set_training_journal(TrainingJournalPtr njournal);
  const TrainingJournalPtr& get_training_journal() const;

  /// @brief The base name of the files of the training, which the
  /// computation borrows; empty for the one of the cfg file.
  void set_training_base(std::string nbase);

  /// @brief The class names of the work, which the computation borrows.
  void set_class_names(std::vector<std::string> nnames);

  /**
   * @brief Hands the training networks, one per GPU, to the original Darknet
   * computation: the nets array and the net of the computation context
   * become their views, the networks are kept alive and registered for the
   * darknet C API calls of the computation (resize_network() and the like)
   * until the release_networks() call.
   */
  void set_train_networks(ANetworkAdaptorPtrs nnets);

  /**
   * @brief Hands the mAP calculation network to the computation: the net_map
   * of the computation context becomes it's view. None clears the net_map.
   */
  void set_map_network(ANetworkAdaptorPtr nnet);

  /// @brief Hands the detection network to the computation: the net of the
  /// computation context becomes it's view.
  void set_detect_network(ANetworkAdaptorPtr nnet);

  /**
   * @brief Hands the image lists and the labels of the YOLO4 annotations of
   * the dxxwjz2p project to the computation (see the TrainingDataRegistry)
   * until the release_networks() call, the networks of which read the lists
   * of the TrainingDataRegistry names.
   */
  void set_training_annotations(const loaders::Yolo4Annotations& annotations);

  const ANetworkAdaptorPtrs& get_train_networks() const;
  const ANetworkAdaptorPtr& get_map_network() const;
  const ANetworkAdaptorPtr& get_detect_network() const;

  /**
   * @brief Takes the networks back from the computation context (it's views
   * cleared) and releases them.
   */
  void release_networks();

 private:
  static detector_context_ptr create_subcontext();

  static std::vector<int> prepare_gpus_list(
      const std::vector<std::string>& src);

  bool propagate_context_params();

  /// @brief Hands the training iterations to the listener of the app context,
  /// when it has one.
  void listen_training();

  /// @brief Records the iteration into the journal and hands it to the
  /// listener of the app context.
  static void report_iteration(const detector_context* ctx, void* data);

  /// @brief The weights_saver of the computation: saves by the journal.
  static int save_weights(const detector_context* ctx, const char* stem,
                          int ema, void* data);

  /// @brief Records the losses of the iteration and it's mAP, if calculated,
  /// into the journal.
  void record(const detector_context& ctx) const;

  /**
   * @brief Starts the DarknetContext::watch_the_app_stop_flag poller, so a
   * requested stop reaches the original Darknet computation context.
   *
   * The whole Darknet work happens inside a single blocking call of the
   * original C code, so nothing of the C++ side is reached while it runs and
   * the poller is the only one to mirror the flag in time.
   */
  void start_stop_watcher();

  /// @brief Asks the DarknetContext::watch_the_app_stop_flag poller to return
  /// and waits for it's thread. Harmless when no poller was started.
  void stop_stop_watcher();

  /**
   * @brief The poller body, which mirrors the stop request of the library
   * context into the original Darknet computation context until the stop is
   * requested or the DarknetContext::stop_stop_watcher method is called.
   *
   * The stop is requested by an atomic flag, the one an OS signal handler
   * raises, for example, since such a handler is allowed to use the async
   * signal safe calls only. See the handling the OS signals section of the
   * project documentation.
   */
  void watch_the_app_stop_flag();

  /// @brief The poller thread of the latest DarknetContext::init call.
  std::thread mstop_watcher;

  /// @brief Lowered to ask the DarknetContext::watch_the_app_stop_flag poller
  /// to return.
  std::atomic_bool mwatching{false};

  /// @brief The app context
  LibCtxPtr lctx;

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
  std::string names;
  std::vector<int> gpuslist;
  std::string latest_weights;
  std::string image;

  /// @brief The networks the computation context computes with.
  ANetworkAdaptorPtrs train_networks;
  ANetworkAdaptorPtr map_network;
  ANetworkAdaptorPtr detect_network;

  /// @brief The views of the training networks, the nets array of the
  /// computation context.
  std::vector<network> train_views;

  TrainingJournalPtr journal;
  std::string training_base;
  /// @brief The training data of the annotations is published.
  bool annotated{false};

  /// @brief The class names and the names array of the computation context.
  std::vector<std::string> class_names;
  std::vector<char*> class_names_view;
};

using DarknetContextPtr = DarknetContext::DarknetContextPtr;

}  // namespace darknet_adaptor

#endif  // DARKNET_CXX_PORT_PROJECT_DARKNETCONTEXT_CLASS_H
