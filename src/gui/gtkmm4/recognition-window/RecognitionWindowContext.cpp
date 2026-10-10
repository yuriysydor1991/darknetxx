#include "src/gui/gtkmm4/recognition-window/RecognitionWindowContext.h"

#include <string>
#include <vector>

#include "LibraryFacade.h"
#include "src/gui/gtkmm4/ChooserEventsHandler.h"
#include "src/gui/gtkmm4/StopSignalEventsHandler.h"
#include "src/gui/gtkmm4/recognition-window/RecognitionEventsHandler.h"

namespace darknetxxGtkmm4
{

namespace
{

/// @brief The files of the suffixes.
Glib::RefPtr<Gtk::FileFilter> files_filter(
    const Glib::ustring& name, const std::vector<std::string>& suffixes)
{
  auto filter = Gtk::FileFilter::create();

  filter->set_name(name);

  for (const auto& suffix : suffixes) {
    filter->add_suffix(suffix);
  }

  return filter;
}

}  // namespace

window::AWindowLoader& RecognitionWindowContext::loader() { return wloader; }

RecognitionWindowContext::Handlers RecognitionWindowContext::create_handlers()
{
  using namespace window::event_handlers;

  auto& window = *wloader.window();
  auto images = Gtk::FileFilter::create();
  // the weights files of every format, the suffixes of no dot
  std::vector<std::string> formats;

  for (const std::string& extension :
       darknetxx011::LibraryFacade::weights_extensions()) {
    formats.push_back(extension.substr(1));
  }

  images->set_name("Images");
  images->add_mime_type("image/*");

  const auto chooser = [&window](Gtk::Entry* entry, Gtk::Button* button,
                                 const char* title,
                                 Glib::RefPtr<Gtk::FileFilter> filter) {
    return std::make_shared<ChooserEventsHandler>(
        window, *entry, *button, title, Gtk::FileChooser::Action::OPEN,
        std::move(filter));
  };

  return {
      std::make_shared<RecognitionEventsHandler>(shared_from_this()),
      chooser(wloader.weights_entry(), wloader.weights_button(),
              "Choose the weights file of the network",
              files_filter("Network weights files", formats)),
      chooser(wloader.cfg_entry(), wloader.cfg_button(),
              "Choose the cfg file of the network",
              files_filter("Network cfg files", {"cfg", "dxxwjz2", "dxxwjz2p",
                                                 "dxxwjz3", "dxxwjz3p"})),
      chooser(wloader.names_entry(), wloader.names_button(),
              "Choose the class names file",
              files_filter("Class names files", {"names", "txt"})),
      chooser(wloader.image_entry(), wloader.image_button(),
              "Choose the image to recognize", images),
      std::make_shared<StopSignalEventsHandler>(actx, window)};
}

}  // namespace darknetxxGtkmm4
