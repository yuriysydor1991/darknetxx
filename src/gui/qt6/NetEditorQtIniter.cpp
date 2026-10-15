#include "src/gui/qt6/NetEditorQtIniter.h"

#include <QApplication>
#include <QIcon>
#include <QTimer>
#include <algorithm>
#include <cassert>

#include "project-global-decls.h"
#include "src/gui/qt6/NetEditorMainWindow.h"
#include "src/log/log.h"

namespace darknetxxQt6
{

bool NetEditorQtIniter::run(app::AppCtxPtr ctx,
                            darknetxx011::LibraryContextPtr libctx)
{
  static constexpr const int WATCH_INTERVAL_MS = 200;

  assert(ctx != nullptr);
  assert(libctx != nullptr);

  if (ctx == nullptr || libctx == nullptr) {
    LOGE("No valid application or library context provided");
    return false;
  }

  // the command line options are the application ones, not the toolkit ones
  margc = std::min(ctx->argc, 1);

  QApplication app{margc, ctx->argv};

  // the Wayland app id, which names the desktop entry and the icon
  QApplication::setDesktopFileName(QString::fromStdString(
      project_decls::PROJECT_NET_EDITOR_QT_APPLICATION_ID));
  QApplication::setWindowIcon(
      QIcon{QStringLiteral(":/ua/org/kytok/darknetxx/darknet-logo.png")});

  NetEditorMainWindow window{project_decls::PROJECT_NET_EDITOR_QT_GUI_NAME,
                             libctx->train_cfg};
  QTimer watch;

  // the OS stop signal closes the window the way the user does
  QObject::connect(&watch, &QTimer::timeout, [&ctx, &window]() {
    if (ctx->stop_signal.load() > 0) {
      window.close();
    }
  });
  watch.start(WATCH_INTERVAL_MS);
  window.show();

  return QApplication::exec() == 0;
}

void NetEditorQtIniter::deinit() {}

}  // namespace darknetxxQt6
