#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATION_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATION_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/IApplication.h"

namespace app
{

/**
 * @brief The default application implementation should be placed into
 * into current class.
 */
class Application : public IApplication,
                    public std::enable_shared_from_this<Application>
{
 public:
  using AppPtr = std::shared_ptr<Application>;

  virtual ~Application() = default;
  Application(const Application&) = delete;
  Application(Application&&) = delete;
  Application();

  /**
   * @brief Implemented default application. See IApplication interface.
   *
   * @param ctx Application's run context with command line parameters etc.
   *
   * @return Returns the application return status. A zero value on the success
   * and other value otherwise.
   */
  virtual int run(AppCtxPtr ctx) override;

 protected:
  virtual bool deal_omp(AppCtxPtr);
  virtual bool stop_catch();

  static bool init_signals();
  static void handle_sigint(int sig);

  inline static AppPtr apt{};

  AppCtxPtr actx;
};

}  // namespace app

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATION_CLASS_H
