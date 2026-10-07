#ifndef DARKNETXX_011_LIBRARYCONTEXT_CLASS_H
#define DARKNETXX_011_LIBRARYCONTEXT_CLASS_H

#include <atomic>
#include <memory>
#include <string>

#include "ITrainingListener.h"
#include "LibraryAPI.h"

/**
 * @brief The namespace of the installable library interface.
 */
namespace darknetxx011
{

/**
 * @brief The parameters of a library call: the training, the detection or
 * the network weights files conversion one (see the LibraryFacade), named the
 * way the darknetxx command line parameters are.
 */
class DARKNETXX_LIB_API LibraryContext
{
 public:
  using LibraryContextPtr = std::shared_ptr<LibraryContext>;

  virtual ~LibraryContext() = default;
  LibraryContext() = default;

  /// @brief Tells the context asks for the network weights file conversion.
  bool conversion_requested() const;

  /// @brief Asks the running call to stop after it's current step, the
  /// training saving it's weights. Thread safe.
  void request_stop();

  /// @brief Withdraws the request_stop() one.
  void withdraw_stop();

  /// @brief The stop asked by the request_stop() or by the stop_with() flag.
  /// Thread safe.
  bool stop_requested() const;

  /// @brief The raised flag asks the stop as well, the one an OS signal
  /// handler raises, for example. It has to outlive the library calls.
  void stop_with(const std::atomic_bool& flag);

  // the training and the detection
  std::string train_cfg;
  std::string detect_cfg;
  /// @brief The class names file, over the names= key of the cfg [net] one.
  std::string names;
  std::string latest_weights;
  std::string image;
  std::string gpus;
  /// @brief The OpenMP threads, zero for the OpenMP default.
  unsigned int threads{0};
  bool dont_show{false};
  bool benchmark{false};
  bool benchmark_layers{false};
  bool show{true};
  bool letter_box{false};
  bool map{false};
  int points{0};
  bool show_imgs{false};
  int mjpeg_port{-1};
  int avgframes{3};
  bool dontdraw_bbox{false};
  int json_port{-1};
  std::string http_post_host;
  int time_limit_sec{0};
  std::string out_filename;
  std::string json_file_output;
  std::string outfile;
  std::string prefix;
  float thresh{0.25f};
  float iou_thresh{0.5f};
  float hier_thresh{0.5f};
  int cam_index{0};
  int frame_skip{0};
  int num_of_clusters{5};
  int width{-1};
  int height{-1};
  bool ext_output{false};
  bool save_labels{false};
  std::string chart_path;
  int mAP_epochs{4};
  bool clear{false};
  bool dont_save_charts_every_iter{true};
  bool draw_precision{false};
  bool dont_resize_network{false};
  bool reload_data{true};
  unsigned int net_nth_resize{10};
  unsigned int nth_iteration_reload{0U};
  double stop_less_avg_loss{0.0};
  /// @brief The network weights file format the training saves, see the
  /// LibraryFacade::conversion_formats.
  std::string save_format{"dxxwjz2"};
  /// @brief Listens to the training, none by default.
  ITrainingListenerPtr training_listener;

  // the network weights file conversion, see the
  // LibraryFacade::conversion_formats
  std::string convert_from;
  std::string convert_src;
  std::string convert_to;
  std::string convert_dst;
  std::string convert_cfg;
  /// @brief The class names file the formats of the network description
  /// keep, over the names= key of the cfg [net] section.
  std::string convert_names;

 private:
  std::atomic_bool mstop{false};
  std::atomic<const std::atomic_bool*> mstopFlag{nullptr};
};

using LibraryContextPtr = LibraryContext::LibraryContextPtr;

}  // namespace darknetxx011

#endif  // DARKNETXX_011_LIBRARYCONTEXT_CLASS_H
