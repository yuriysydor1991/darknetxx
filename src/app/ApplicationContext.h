#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONTEXT_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONTEXT_CLASS_H

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace app
{

/**
 * @brief The class that holds necessary information for the application
 * to run properly.
 *
 * May hold parsed parameters, configuration values and error messages
 * with application status code. The command line parameters are parsed by
 * the CommandLineParser class instance inside the ApplicationFactory class.
 * The ApplicationHelpPrinter class instance is printing command line help
 * for available command line parameters that may be stored in
 * the current class instance (if registered properly).
 */
struct ApplicationContext
{
  /// @brief Command line arguments count passed by the OS through the main
  /// executable function.
  int& argc;

  /// @brief The command line arguments array passed by the OS through
  /// the main executable function.
  char**& argv;

  /**
   * @brief An ApplicationContext constructor.
   *
   * @param gargc Command line arguments count passed by the OS through the main
   * executable function that will be referenced in the argc member field.
   * @param gargv  The command line arguments array passed by the OS through
   * the main executable function that will be referenced in the argv member
   * field.
   */
  ApplicationContext(int& gargc, char**& gargv);

  /**
   * @brief Set up this flag to true so the ApplicationFactory will create
   * a ApplicationHelpPrinter instance during the application execution.
   * All other classes are ignoring this flag.
   */
  bool print_help_and_exit{false};

  /**
   * @brief Set up this flag value to a true in order to ApplicationFactory
   * to create ApplicationVersionPrinter and execute it.
   */
  bool print_version_and_exit{false};

  /**
   * @brief Errors description.
   */
  std::vector<std::string> errors;

  /**
   * @brief Pushes a new error description into the errors field;
   *
   * @param errorDescription The application error description.
   */
  void push_error(const std::string& errorDescription);

  /**
   * @brief Extracted command line params from the detector and tester
   * original Darknet sub routines with default values.
   */
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
  bool dont_save_charts_every_iter{false};
  bool draw_precision{false};
  std::string train_cfg;
  std::string detect_cfg;
  std::string latest_weights;
  /// @brief If enable training network resize (disabling worse the result)
  bool dont_resize_network{false};
  /// @brief The image to try to detect on
  std::string image;
  /// @brief If to reaload data for the each iteration (disabling worse the
  /// result)
  bool reload_data{false};
  /// @brief The OpenMP max threads
  unsigned int threads{0};
  /// @brief true if application should stop it's execution
  std::atomic_bool stop{false};
  /// @brief Every 10 train iterations (by default) resize network
  unsigned int net_nth_resize{10};
  /// @brief if non-zero the training data will reload every specified iteration
  unsigned int nth_iteration_reload{0U};
  /// @brief stop training when this value reached or less
  double stop_less_avg_loss{0.0};

  using stop_subs = std::shared_ptr<std::function<void()>>;

  std::vector<stop_subs> stop_subscribers;
};

using AppCtxPtr = std::shared_ptr<ApplicationContext>;

}  // namespace app

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONTEXT_CLASS_H
