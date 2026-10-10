#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONTEXT_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONTEXT_CLASS_H

#include <atomic>
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
  /// @brief The training charts are saved every 100 iterations by default
  /// (the original Darknet cadence), the --save-charts-every-iter flag saves
  /// them on every iteration, the cfg savechart= key overrides both.
  bool dont_save_charts_every_iter{true};
  bool draw_precision{false};
  std::string train_cfg;
  std::string detect_cfg;
  /// @brief The class names file. Takes precedence over the names= key of the
  /// network cfg [net] section when given.
  std::string names;
  std::string latest_weights;
  /// @brief If enable training network resize (disabling worse the result)
  bool dont_resize_network{false};
  /// @brief The image to try to detect on
  std::string image;
  /// @brief If to load a new training batch for every iteration (disabling it
  /// reuses the loaded batch, which worsens the result). Lowered by the
  /// --dont-reload-data-every-iteration flag, while the --data-reload-nth one
  /// takes precedence over it.
  bool reload_data{true};
  /// @brief The OpenMP max threads
  unsigned int threads{0};
  /// @brief true if application should stop it's execution. Raised by the
  /// app::StopSignalsHandler on a SIGINT, a SIGTERM, a SIGQUIT or a SIGHUP
  /// arrival.
  std::atomic_bool stop{false};
  /// @brief The number of the OS signal which raised the stop flag, or zero.
  /// The application reports it with the 128 + signal number exit status, the
  /// way a shell does for a process the signal killed.
  std::atomic_int stop_signal{0};
  /// @brief true while the application was asked to hold it's work without
  /// leaving the IApplication::run method. Raised by the
  /// app::PauseSignalsHandler and lowered by the app::ContinueSignalsHandler.
  std::atomic_bool pause{false};
  /// @brief true once the application was asked to reread it's configuration
  /// by the app::ReloadSignalsHandler. The application is expected to lower it
  /// back on it's own once the configuration is reread.
  std::atomic_bool reload{false};
  /// @brief true while the first user defined request is pending. Raised by
  /// the app::UserSignalsHandler on a SIGUSR1 arrival. The meaning of the
  /// request is the application own one and the application is expected to
  /// lower the flag back on it's own after serving it.
  std::atomic_bool first_user_request{false};
  /// @brief true while the second user defined request is pending. Raised by
  /// the app::UserSignalsHandler on a SIGUSR2 arrival. See the
  /// first_user_request flag.
  std::atomic_bool second_user_request{false};
  /// @brief Every 10 train iterations (by default) resize network
  unsigned int net_nth_resize{10};
  /// @brief if non-zero the training data will reload every specified iteration
  unsigned int nth_iteration_reload{0U};
  /// @brief stop training when this value reached or less
  double stop_less_avg_loss{0.0};
  /// @brief The network weights file format the training saves, the name
  /// given after the --save-format command line flag.
  std::string save_format{"dxxwjz3"};
  /// @brief The PNG file the training chart of the dxxwjz3 or dxxwjz2
  /// --weights file is drawn into, given after the --draw-chart command line
  /// flag.
  std::string draw_chart;

  /// @brief The network weights file format to read, the name given after the
  /// --convert-from command line flag (see the
  /// darknetxx011::LibraryFacade::conversion_formats).
  std::string convert_from;
  /// @brief The network weights file to read, given after the --convert-src
  /// command line flag.
  std::string convert_src;
  /// @brief The network weights file format to write, the name given after the
  /// --convert-to command line flag.
  std::string convert_to;
  /// @brief The network weights file to write, given after the --convert-dst
  /// command line flag.
  std::string convert_dst;
  /// @brief The network cfg file of the converted weights, given after the
  /// --convert-cfg command line flag.
  std::string convert_cfg;
  /// @brief The class names file of the converted weights, given after the
  /// --convert-names command line flag.
  std::string convert_names;

  /**
   * @brief Tells the network weights file conversion was requested on the
   * command line, which is what makes the ApplicationFactory create the
   * converter application instead of the training or the detection one.
   */
  bool conversion_requested() const;

  /// @brief Tells the training chart of the weights file was requested on
  /// the command line, which no training or detection goes with.
  bool chart_requested() const;
};

using AppCtxPtr = std::shared_ptr<ApplicationContext>;

}  // namespace app

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONTEXT_CLASS_H
