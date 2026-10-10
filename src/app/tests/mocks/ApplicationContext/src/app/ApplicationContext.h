#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONTEXT_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONTEXT_CLASS_H

#include <gmock/gmock.h>

#include <atomic>
#include <memory>
#include <string>
#include <vector>

namespace app
{

/**
 * @brief Test mock for the ApplicationContext. Mirrors the real data members so
 * that the production sources (CommandLineParser, Application, ...) compile
 * against it, while push_error stays a gmock method so tests can assert on it.
 */
struct ApplicationContext
{
 public:
  int& argc;
  char**& argv;

  ApplicationContext(int& gargc, char**& gargv);

  bool print_help_and_exit{false};
  bool print_version_and_exit{false};

  std::vector<std::string> errors;

  MOCK_METHOD(void, push_error, (const std::string& errorDescription));

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
  float hier_thresh{0.5};
  int cam_index{0};
  int frame_skip{0};
  int num_of_clusters{5};
  int width{-1};
  int height{-1};
  bool ext_output{false};
  bool save_labels{false};
  std::string chart_path;
  int mAP_epochs{4};
  std::string gpus;
  bool clear{false};
  bool dont_save_charts_every_iter{true};
  bool draw_precision{false};
  std::string train_cfg;
  std::string detect_cfg;
  std::string names;
  std::string latest_weights;
  bool dont_resize_network{false};
  std::string image;
  bool reload_data{true};
  unsigned int threads{0};
  std::atomic_bool stop{false};
  std::atomic_int stop_signal{0};
  std::atomic_bool pause{false};
  std::atomic_bool reload{false};
  std::atomic_bool first_user_request{false};
  std::atomic_bool second_user_request{false};
  unsigned int net_nth_resize{10};
  unsigned int nth_iteration_reload{0U};
  double stop_less_avg_loss{0.0};
  std::string save_format{"dxxwjz3"};
  std::string draw_chart;
  std::string convert_from;
  std::string convert_src;
  std::string convert_to;
  std::string convert_dst;
  std::string convert_cfg;
  std::string convert_names;

  bool conversion_requested() const;
  bool chart_requested() const;
};

using AppCtxPtr = std::shared_ptr<ApplicationContext>;

}  // namespace app

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONTEXT_CLASS_H
