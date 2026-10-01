#include "src/app/ApplicationFactory.h"

#include <cassert>
#include <csignal>
#include <cstdio>
#include <iostream>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/ApplicationHelpPrinter.h"
#include "src/app/ApplicationVersionPrinter.h"
#include "src/app/CMDParamNames.h"
#include "src/app/CommandLineParser.h"
#include "src/app/IApplication.h"
#include "src/app/converter/ApplicationConverterComponent.h"
#include "src/app/converter/ApplicationFormatsConverter.h"
#include "src/app/converter/IFormatsConverter.h"
#include "src/app/signals-handlers/ISignalsHandler.h"
#include "src/app/signals-handlers/SignalsHandler.h"
#include "src/log/log.h"

#ifndef DARKNETXX_CONVERTER_ONLY
// The training and the detection application, and with it the whole darknet
// computation it starts. The network weights files converter only binary is
// compiled without it, which is the single place in this project where the
// two binaries part ways.
#include "src/app/Application.h"
#endif  // DARKNETXX_CONVERTER_ONLY

namespace app
{

std::shared_ptr<ApplicationContext> ApplicationFactory::create_default_context(
    int& gargc, char**& gargv)
{
  return std::make_shared<ApplicationContext>(gargc, gargv);
}

std::shared_ptr<CommandLineParser>
ApplicationFactory::create_default_arg_parser()
{
  return std::make_shared<CommandLineParser>();
}

std::shared_ptr<ISignalsHandler>
ApplicationFactory::create_default_signals_handler()
{
  return std::make_shared<SignalsHandler>();
}

std::shared_ptr<ApplicationContext> ApplicationFactory::create_context(
    int& gargc, char**& gargv)
{
  std::shared_ptr<ApplicationContext> ctx =
      create_default_context(gargc, gargv);
  std::shared_ptr<CommandLineParser> argParser = create_default_arg_parser();

  assert(ctx != nullptr);
  assert(argParser != nullptr);

  if (ctx == nullptr) {
    LOGE("Fail to create the application context");
    return {};
  }

  if (argParser == nullptr) {
    LOGE("Fail to create the argument parser");
    return {};
  }

  if (!argParser->parse_args(ctx)) {
    LOGE("Fail to parse the command line parameters");

    // Nothing but the usage runs for an invalid command line, and the pushed
    // error makes the help printer to report the failure.
    ctx->print_help_and_exit = true;

    if (ctx->errors.empty()) {
      ctx->push_error("Invalid command line parameters");
    }
  }

  return ctx;
}

std::shared_ptr<IApplication> ApplicationFactory::create_default_application()
{
#ifdef DARKNETXX_CONVERTER_ONLY
  // There is no default application in a converter only binary: what it can
  // do is named by the conversion command line flags, so the help message
  // which lists them is what an empty command line deserves.
  return create_help_printer();
#else
  return std::make_shared<Application>();
#endif  // DARKNETXX_CONVERTER_ONLY
}

std::shared_ptr<converter::IFormatsConverter>
ApplicationFactory::create_formats_converter()
{
  return std::make_shared<converter::ApplicationFormatsConverter>();
}

std::shared_ptr<IApplication>
ApplicationFactory::create_formats_converter_application()
{
  return std::make_shared<converter::ApplicationConverterComponent>(
      create_formats_converter());
}

std::shared_ptr<IApplication> ApplicationFactory::create_help_printer()
{
  return std::make_shared<ApplicationHelpPrinter>();
}

std::shared_ptr<IApplication> ApplicationFactory::create_version_printer()
{
  return std::make_shared<ApplicationVersionPrinter>();
}

std::shared_ptr<IApplication> ApplicationFactory::create_application(
    std::shared_ptr<ApplicationContext> ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid application context provided");
    return {};
  }

  if (ctx->print_help_and_exit) {
    LOGT("Creating help printer application");
    return create_help_printer();
  }

  if (ctx->print_version_and_exit) {
    LOGT("Creating version printer application");
    return create_version_printer();
  }

  if (ctx->conversion_requested()) {
    LOGT("Creating the formats converter application");
    return create_formats_converter_application();
  }

#ifdef DARKNETXX_CONVERTER_ONLY
  // the help printer reports the failure of the command line, which asks for
  // nothing the binary does
  LOGE("No network weights file conversion was requested");
  ctx->push_error("No network weights file conversion was requested");
#endif  // DARKNETXX_CONVERTER_ONLY

  LOGT("Creating the default application object");

  return create_default_application();
}

int ApplicationFactory::run(int& gargc, char**& gargv)
{
  const std::string custom_log =
      CommandLineParser::get_custom_logfile(gargc, gargv);

  if (custom_log.empty()) {
    LOG_INIT_DEFAULTS();
  } else {
    LOG_INIT_PATH(custom_log);
  }

  std::shared_ptr<ApplicationContext> ctx = create_context(gargc, gargv);

  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid application context object provided");
    return IApplication::INVALID;
  }

  std::shared_ptr<IApplication> app = create_application(ctx);

  assert(app != nullptr);

  if (app == nullptr) {
    LOGE("Fail to create the application object");
    return IApplication::INVALID;
  }

  std::shared_ptr<ISignalsHandler> signalsHandler =
      create_default_signals_handler();

  assert(signalsHandler != nullptr);

  if (signalsHandler == nullptr) {
    LOGE("Fail to create the signals handler object");
    return IApplication::INVALID;
  }

  // The handler lives till the current method returns, which is when the
  // covered signals are given back to the OS default disposition.
  if (!signalsHandler->install(ctx)) {
    LOGW("Fail to handle the application OS signals");
  }

  LOGD("Starting the application");

  const int status = app->run(ctx);

  mstop_signal = ctx->stop.load() ? ctx->stop_signal.load() : 0;

  return status;
}

void ApplicationFactory::end_by_the_stop_signal(const int signalNumber)
{
  if (signalNumber <= 0) {
    return;
  }

#ifdef SIGQUIT
  if (signalNumber == SIGQUIT) {
    return;
  }
#endif  // SIGQUIT

  LOGD("Ending the process by the stop signal " << signalNumber);

  std::cout.flush();
  std::cerr.flush();
  std::fflush(nullptr);

  std::signal(signalNumber, SIG_DFL);
  std::raise(signalNumber);
}

int ApplicationFactory::execute(int& gargc, char**& gargv)
{
  std::shared_ptr<ApplicationFactory> factory =
      std::make_shared<ApplicationFactory>();

  const int status = factory->run(gargc, gargv);
  const int stopSignal = factory->mstop_signal;

  // the signals handler and the whole work are released at this point
  factory.reset();

  end_by_the_stop_signal(stopSignal);

  return status;
}

}  // namespace app
