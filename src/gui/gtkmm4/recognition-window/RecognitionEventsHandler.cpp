#include "src/gui/gtkmm4/recognition-window/RecognitionEventsHandler.h"

#include <gdkmm/texture.h>
#include <glibmm/bytes.h>

#include <string>
#include <utility>

#include "LibraryFacade.h"
#include "src/gui/gtkmm4/recognition-window/RecognitionWindowContext.h"
#include "src/log/log.h"

namespace darknetxxGtkmm4::window::event_handlers
{

RecognitionEventsHandler::~RecognitionEventsHandler()
{
  if (mworker.joinable()) {
    mworker.join();
  }
}

RecognitionEventsHandler::RecognitionEventsHandler(
    std::shared_ptr<RecognitionWindowContext> nmwctx)
    : mwctx{std::move(nmwctx)},
      recognizer{darknetxx011::LibraryFacade::create_recognizer()}
{
}

bool RecognitionEventsHandler::subscribe()
{
  auto& loader = mwctx->wloader;
  const auto& libctx = *mwctx->libctx;
  const auto start = sigc::mem_fun(*this, &RecognitionEventsHandler::recognize);

  loader.weights_entry()->set_text(libctx.latest_weights);
  loader.cfg_entry()->set_text(libctx.detect_cfg);
  loader.names_entry()->set_text(libctx.names);
  loader.image_entry()->set_text(libctx.image);
  loader.thresh_spin()->set_value(static_cast<double>(libctx.thresh));

  for (auto* entry : {loader.weights_entry(), loader.cfg_entry(),
                      loader.names_entry(), loader.image_entry()}) {
    entry->signal_activate().connect(start);
  }

  loader.thresh_spin()->signal_value_changed().connect(start);
  loader.recognize_button()->signal_clicked().connect(start);
  mrecognized.connect(
      sigc::mem_fun(*this, &RecognitionEventsHandler::show_recognition));

  // the files of the command line at once
  if (!libctx.latest_weights.empty() && !libctx.image.empty()) {
    recognize();
  }

  return true;
}

void RecognitionEventsHandler::recognize()
{
  auto& loader = mwctx->wloader;
  auto& libctx = *mwctx->libctx;

  // the entries of the moment the running recognition ends
  if (mworker.joinable()) {
    mpending = true;
    return;
  }

  libctx.latest_weights = loader.weights_entry()->get_text().raw();
  libctx.detect_cfg = loader.cfg_entry()->get_text().raw();
  libctx.names = loader.names_entry()->get_text().raw();
  libctx.image = loader.image_entry()->get_text().raw();
  libctx.thresh = static_cast<float>(loader.thresh_spin()->get_value());

  if (libctx.latest_weights.empty() || libctx.image.empty()) {
    loader.status_label()->set_text(
        libctx.latest_weights.empty() ? "Choose the weights file of the network"
                                      : "Choose the image to recognize");
    return;
  }

  loader.recognize_button()->set_sensitive(false);
  loader.status_label()->set_text("Recognizing the " + libctx.image);

  // the context stays as it is till the recognition is shown
  mworker = std::thread{[this]() {
    mrecognition = recognizer->recognize(*mwctx->libctx);
    mrecognized.emit();
  }};
}

void RecognitionEventsHandler::show_recognition()
{
  auto& loader = mwctx->wloader;
  const auto& png = mrecognition.png;
  std::string objects;

  // the recognition is there once it's thread ends
  mworker.join();

  for (const auto& object : mrecognition.objects) {
    objects += object.text() + "\n";
  }

  loader.status_label()->set_text(mrecognition.summary_text());
  loader.objects_label()->set_text(objects);
  loader.recognize_button()->set_sensitive(true);

  try {
    loader.image_picture()->set_paintable(
        png.empty() ? Glib::RefPtr<Gdk::Texture>{}
                    : Gdk::Texture::create_from_bytes(
                          Glib::Bytes::create(png.data(), png.size())));
  }
  catch (const Glib::Error& e) {
    LOGE("Fail to show the recognized image: " << e.what());
  }

  if (std::exchange(mpending, false)) {
    recognize();
  }
}

}  // namespace darknetxxGtkmm4::window::event_handlers
