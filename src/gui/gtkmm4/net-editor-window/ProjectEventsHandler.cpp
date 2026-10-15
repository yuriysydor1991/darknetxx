#include "src/gui/gtkmm4/net-editor-window/ProjectEventsHandler.h"

#include <giomm/file.h>
#include <glibmm/main.h>

#include <utility>

#include "src/gui/gtkmm4/net-editor-window/GtkmmIcons.h"
#include "src/gui/gtkmm4/net-editor-window/NetEditorWindowContext.h"
#include "src/gui/net-editor/EditorIcons.h"
#include "src/log/log.h"

namespace darknetxxGtkmm4::window::event_handlers
{

namespace
{

/// @brief The network files the editor opens and the projects it saves.
Glib::RefPtr<Gtk::FileFilter> networks_filter(bool every)
{
  auto filter = Gtk::FileFilter::create();

  filter->set_name(every ? "Network files" : "The dxxwjz3p projects");
  filter->add_suffix("dxxwjz3p");

  if (every) {
    for (const char* suffix : {"dxxwjz3", "dxxwjz2p", "dxxwjz2"}) {
      filter->add_suffix(suffix);
    }
  }

  return filter;
}

}  // namespace

ProjectEventsHandler::ProjectEventsHandler(
    std::shared_ptr<NetEditorWindowContext> nmwctx)
    : mwctx{std::move(nmwctx)}
{
}

bool ProjectEventsHandler::subscribe()
{
  using net_editor::EditorIcons;

  auto& loader = mwctx->wloader;
  const std::string& file = mwctx->libctx->train_cfg;

  widgets::GtkmmIcons::set(*loader.new_image(), EditorIcons::CREATE);
  widgets::GtkmmIcons::set(*loader.open_image(), EditorIcons::OPEN);
  widgets::GtkmmIcons::set(*loader.save_image(), EditorIcons::SAVE);
  widgets::GtkmmIcons::set(*loader.save_as_image(), EditorIcons::SAVE_AS);

  loader.new_button()->signal_clicked().connect(
      [this]() { confirm("start the new one", [this]() { create(); }); });
  loader.open_button()->signal_clicked().connect(
      [this]() { confirm("open another one", [this]() { open(); }); });
  loader.save_button()->signal_clicked().connect(
      sigc::mem_fun(*this, &ProjectEventsHandler::save));
  loader.save_as_button()->signal_clicked().connect(
      sigc::mem_fun(*this, &ProjectEventsHandler::save_as));
  loader.window()->signal_close_request().connect(
      sigc::mem_fun(*this, &ProjectEventsHandler::on_close_request), false);
  mwctx->edited.connect(
      sigc::mem_fun(*this, &ProjectEventsHandler::show_title));

  // the title the window gets once shown goes first
  Glib::signal_idle().connect_once(
      sigc::mem_fun(*this, &ProjectEventsHandler::on_shown));

  std::string message;

  if (!file.empty() && !mwctx->project.open(file, message)) {
    mwctx->tell(message, true);
  } else {
    mwctx->tell(file.empty() ? "The new network" : "Opened the " + file);
  }

  reload();

  return true;
}

Gtk::MessageDialog* ProjectEventsHandler::prompt() { return mprompt.get(); }

Gtk::FileChooserDialog* ProjectEventsHandler::save_chooser()
{
  return msave.get();
}

void ProjectEventsHandler::create()
{
  mwctx->project.create();
  mwctx->tell("The new network");
  reload();
}

void ProjectEventsHandler::open()
{
  if (mopen == nullptr) {
    mopen = std::make_unique<Gtk::FileChooserDialog>(
        *mwctx->window(), "Open the network", Gtk::FileChooser::Action::OPEN);

    mopen->set_modal(true);
    mopen->set_hide_on_close(true);
    mopen->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    mopen->add_button("_Open", Gtk::ResponseType::ACCEPT);
    mopen->add_filter(networks_filter(true));
    mopen->signal_response().connect(
        sigc::mem_fun(*this, &ProjectEventsHandler::on_open_chosen));
  }

  mopen->present();
}

void ProjectEventsHandler::save()
{
  const std::string& path = mwctx->project.path();

  if (path.empty()) {
    save_as();
  } else if (save_into(path) && maction) {
    std::exchange(maction, {})();
  }
}

void ProjectEventsHandler::save_as()
{
  const std::string& path = mwctx->project.path();

  if (msave == nullptr) {
    msave = std::make_unique<Gtk::FileChooserDialog>(
        *mwctx->window(), "Save the network as the project",
        Gtk::FileChooser::Action::SAVE);

    msave->set_modal(true);
    msave->set_hide_on_close(true);
    msave->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    msave->add_button("_Save", Gtk::ResponseType::ACCEPT);
    msave->add_filter(networks_filter(false));
    msave->signal_response().connect(
        sigc::mem_fun(*this, &ProjectEventsHandler::on_save_chosen));
  }

  try {
    if (path.empty()) {
      msave->set_current_name("network" +
                              net_editor::NetworkProject::EXTENSION);
    } else {
      msave->set_file(Gio::File::create_for_path(path));
    }
  }
  catch (const Glib::Error& e) {
    LOGD("The chooser starts at it's own folder: " << e.what());
  }

  msave->present();
}

bool ProjectEventsHandler::save_into(const std::string& path)
{
  std::string message;
  const bool saved = mwctx->project.save(path, message);

  mwctx->tell(
      saved ? "Saved into the " + path + (message.empty() ? "" : ". " + message)
            : message,
      !saved);
  show_title();

  return saved;
}

void ProjectEventsHandler::confirm(const std::string& name,
                                   std::function<void()> action)
{
  if (!mwctx->project.modified()) {
    action();
    return;
  }

  const bool named = !mwctx->project.path().empty();

  maction = std::move(action);
  mprompt = std::make_unique<Gtk::MessageDialog>(
      *mwctx->window(), "The network has changes of no save", false,
      Gtk::MessageType::QUESTION, Gtk::ButtonsType::NONE, true);
  mprompt->set_secondary_text("Save them to " + name + ", or discard them?");
  mprompt->set_hide_on_close(true);
  mprompt->add_button(named ? "_Save and " + name : "_Save as… and " + name,
                      SAVE);
  mprompt->add_button("_Cancel", Gtk::ResponseType::CANCEL);
  mprompt->add_button("_Discard the changes", DISCARD);
  mprompt->set_default_response(SAVE);
  mprompt->signal_response().connect(
      sigc::mem_fun(*this, &ProjectEventsHandler::on_prompted));
  mprompt->present();
}

bool ProjectEventsHandler::on_close_request()
{
  if (mclosing || !mwctx->project.modified()) {
    return false;
  }

  // the stop signal closes the window again and again
  if (mprompt != nullptr && mprompt->get_visible()) {
    return true;
  }

  confirm("exit", [this]() {
    mclosing = true;
    mwctx->window()->close();
  });

  return true;
}

void ProjectEventsHandler::on_prompted(const int response)
{
  mprompt->set_visible(false);

  if (response == SAVE) {
    save();
  } else if (response == DISCARD) {
    std::exchange(maction, {})();
  } else {
    maction = {};
  }
}

void ProjectEventsHandler::on_open_chosen(const int response)
{
  mopen->set_visible(false);

  const auto chosen = mopen->get_file();
  std::string message;

  if (response != Gtk::ResponseType::ACCEPT || !chosen) {
    return;
  }

  if (!mwctx->project.open(chosen->get_path(), message)) {
    mwctx->tell(message, true);
    return;
  }

  mwctx->tell("Opened the " + chosen->get_path());
  reload();
}

void ProjectEventsHandler::on_save_chosen(const int response)
{
  msave->set_visible(false);

  const auto chosen = msave->get_file();

  if (response != Gtk::ResponseType::ACCEPT || !chosen) {
    maction = {};
    return;
  }

  if (save_into(net_editor::NetworkProject::project_of(chosen->get_path())) &&
      maction) {
    std::exchange(maction, {})();
  } else {
    maction = {};
  }
}

void ProjectEventsHandler::on_shown()
{
  mtitle = mwctx->window()->get_title();
  show_title();
}

void ProjectEventsHandler::show_title()
{
  auto& loader = mwctx->wloader;
  const std::string& path = mwctx->project.path();

  if (mtitle) {
    const Glib::ustring title = mwctx->project.title();

    loader.window()->set_title(mtitle->empty() ? title
                                               : *mtitle + " - " + title);
  }

  loader.file_label()->set_text(mwctx->project.title());
  loader.file_label()->set_tooltip_text(path);
}

void ProjectEventsHandler::reload()
{
  mwctx->reloaded.emit();
  show_title();
}

}  // namespace darknetxxGtkmm4::window::event_handlers
