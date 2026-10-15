#include "src/gui/gtkmm4/net-editor-window/NetworkEventsHandler.h"

#include <glibmm/main.h>
#include <gtkmm/stringlist.h>

#include <algorithm>
#include <sstream>

#include "src/gui/gtkmm4/net-editor-window/GtkmmIcons.h"
#include "src/gui/gtkmm4/net-editor-window/NetEditorWindowContext.h"
#include "src/gui/net-editor/EditorIcons.h"
#include "src/gui/net-editor/SectionSchemas.h"

namespace darknetxxGtkmm4::window::event_handlers
{

NetworkEventsHandler::NetworkEventsHandler(
    std::shared_ptr<NetEditorWindowContext> nmwctx)
    : mwctx{std::move(nmwctx)}
{
}

bool NetworkEventsHandler::subscribe()
{
  auto& loader = mwctx->wloader;
  auto types = Gtk::StringList::create();

  for (const auto& type : net_editor::SectionSchemas::layer_types()) {
    types->append(type);
  }

  loader.layer_type_dropdown()->set_model(types);
  loader.layer_type_dropdown()->property_selected().signal_changed().connect(
      sigc::mem_fun(*this, &NetworkEventsHandler::on_type_selected));
  widgets::GtkmmIcons::set(*loader.add_layer_image(),
                           net_editor::EditorIcons::ADD);
  loader.add_layer_button()->signal_clicked().connect(
      sigc::mem_fun(*this, &NetworkEventsHandler::add_layer));
  loader.classes_view()->get_buffer()->signal_changed().connect(
      sigc::mem_fun(*this, &NetworkEventsHandler::on_classes_changed));
  mwctx->reloaded.connect(
      sigc::mem_fun(*this, &NetworkEventsHandler::show_network));

  on_type_selected();
  show_network();

  return true;
}

const std::vector<widgets::LayerRow*>& NetworkEventsHandler::rows() const
{
  return mrows;
}

widgets::OptionsGrid& NetworkEventsHandler::net_rows() { return *mnet; }

void NetworkEventsHandler::show_network()
{
  auto& loader = mwctx->wloader;
  auto& model = mwctx->project.model;
  const auto edited = [this]() { changed(); };
  std::string classes;

  while (auto* child = loader.net_grid()->get_first_child()) {
    loader.net_grid()->remove(*child);
  }

  for (auto* row : mrows) {
    loader.layers_box()->remove(*row);
  }

  mrows.clear();
  mnet = std::make_unique<widgets::OptionsGrid>(*loader.net_grid(), model,
                                                model.net(), edited);

  for (const auto& name : model.classes) {
    classes += name + "\n";
  }

  mshowing = true;
  loader.classes_view()->get_buffer()->set_text(classes);
  mshowing = false;

  for (const auto& layer : model.layers()) {
    append(layer);
  }
}

void NetworkEventsHandler::add_layer()
{
  const guint selected = mwctx->wloader.layer_type_dropdown()->get_selected();
  const auto& types = net_editor::SectionSchemas::layer_types();

  if (selected >= types.size()) {
    return;
  }

  const std::string& type = types[selected];

  append(mwctx->project.model.add(
             type, net_editor::SectionSchemas::of(type).initial()))
      ->expander()
      .set_expanded(true);
  changed();
}

void NetworkEventsHandler::remove_layer(widgets::LayerRow& row)
{
  auto& model = mwctx->project.model;
  auto* box = mwctx->wloader.layers_box();
  const int index = model.index_of(*row.section());

  model.remove(index);
  mrows.erase(mrows.begin() + index);

  // the row of the clicked button goes once the click is handled
  row.set_visible(false);
  Glib::signal_idle().connect_once(
      sigc::bind(sigc::mem_fun(*box, &Gtk::Box::remove), std::ref(row)));
  changed();
}

void NetworkEventsHandler::move_layer(const int from, const int to)
{
  auto* box = mwctx->wloader.layers_box();

  if (from == to || from < 0 || to < 0) {
    return;
  }

  widgets::LayerRow* moved = mrows[static_cast<std::size_t>(from)];

  mwctx->project.model.move(from, to);
  mrows.erase(mrows.begin() + from);
  mrows.insert(mrows.begin() + to, moved);

  if (to == 0) {
    box->reorder_child_at_start(*moved);
  } else {
    box->reorder_child_after(*moved, *mrows[static_cast<std::size_t>(to - 1)]);
  }

  changed();
}

void NetworkEventsHandler::changed()
{
  mnet->refresh();

  for (auto* row : mrows) {
    row->refresh();
  }

  mwctx->edited.emit();
}

widgets::LayerRow* NetworkEventsHandler::append(
    const net_editor::NetworkSectionPtr& section)
{
  auto* row = Gtk::make_managed<widgets::LayerRow>(
      mwctx->project.model, section,
      widgets::LayerRow::Handlers{
          [this]() { changed(); },
          [this](widgets::LayerRow& removed) { remove_layer(removed); },
          [this](int from, int to) { move_layer(from, to); }});

  mwctx->wloader.layers_box()->append(*row);
  mrows.push_back(row);

  return row;
}

void NetworkEventsHandler::on_classes_changed()
{
  if (mshowing) {
    return;
  }

  std::istringstream text{
      mwctx->wloader.classes_view()->get_buffer()->get_text().raw()};
  std::string name;
  auto& classes = mwctx->project.model.classes;

  classes.clear();

  // the empty lines name no class
  while (std::getline(text, name)) {
    name.erase(std::remove(name.begin(), name.end(), '\r'), name.end());

    if (!name.empty()) {
      classes.push_back(name);
    }
  }

  changed();
}

void NetworkEventsHandler::on_type_selected()
{
  auto& loader = mwctx->wloader;
  const guint selected = loader.layer_type_dropdown()->get_selected();
  const auto& types = net_editor::SectionSchemas::layer_types();

  loader.layer_type_label()->set_text(
      selected < types.size()
          ? net_editor::SectionSchemas::of(types[selected]).about()
          : std::string{});
}

}  // namespace darknetxxGtkmm4::window::event_handlers
