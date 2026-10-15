#include "src/gui/gtkmm4/net-editor-window/LayerRow.h"

#include <gdkmm/contentprovider.h>
#include <glibmm/markup.h>
#include <gtkmm/dragsource.h>
#include <gtkmm/droptarget.h>
#include <gtkmm/widgetpaintable.h>

#include "src/gui/gtkmm4/net-editor-window/GtkmmIcons.h"
#include "src/gui/net-editor/EditorIcons.h"
#include "src/gui/net-editor/OptionPresenter.h"
#include "src/gui/net-editor/SectionSchemas.h"

namespace darknetxxGtkmm4::window::widgets
{

LayerRow::LayerRow(const net_editor::NetworkModel& nmodel,
                   net_editor::NetworkSectionPtr nsection, Handlers nhandlers)
    : model{nmodel},
      msection{std::move(nsection)},
      handlers{std::move(nhandlers)},
      mcolumn{Gtk::Orientation::VERTICAL},
      mrow{Gtk::Orientation::HORIZONTAL, 6},
      mheader{Gtk::Orientation::HORIZONTAL, 8},
      mbody{Gtk::Orientation::VERTICAL, 6}
{
  static constexpr const int INDEX_CHARS = 4;
  static constexpr const int ABOUT_MIN_CHARS = 30;
  static constexpr const int MARGIN = 4;
  static constexpr const int BODY_MARGIN = 72;
  auto* about = Gtk::make_managed<Gtk::Label>(
      net_editor::SectionSchemas::of(msection->type()).about());

  GtkmmIcons::set(mhandle, net_editor::EditorIcons::DRAG);
  mhandle.set_tooltip_text("Drag the layer onto the place of another one");
  mhandle.set_cursor("grab");

  mindex.set_width_chars(INDEX_CHARS);
  mindex.set_xalign(1.0F);
  mindex.add_css_class("monospace");

  mtype.set_markup("<b>[" + Glib::Markup::escape_text(msection->type()) +
                   "]</b>");
  msummary.set_ellipsize(Pango::EllipsizeMode::END);
  msummary.set_hexpand(true);
  msummary.set_xalign(0.0F);
  msummary.add_css_class("dim-label");
  merrors.add_css_class("error");
  mheader.append(mtype);
  mheader.append(msummary);
  mheader.append(merrors);
  mheader.set_hexpand(true);

  about->set_xalign(0.0F);
  about->set_wrap(true);
  about->set_width_chars(ABOUT_MIN_CHARS);
  about->add_css_class("dim-label");
  mgrid.set_row_spacing(4);
  mgrid.set_column_spacing(12);
  mbody.set_margin_start(BODY_MARGIN);
  mbody.set_margin_end(MARGIN);
  mbody.set_margin_bottom(MARGIN);
  mbody.set_visible(false);
  mbody.append(*about);
  mbody.append(mgrid);

  mexpander.set_label_widget(mheader);
  mexpander.set_hexpand(true);
  mexpander.set_valign(Gtk::Align::CENTER);
  mexpander.property_expanded().signal_changed().connect(
      sigc::mem_fun(*this, &LayerRow::on_expanded));

  auto* removeImage = Gtk::make_managed<Gtk::Image>();

  GtkmmIcons::set(*removeImage, net_editor::EditorIcons::REMOVE);
  mremove.set_child(*removeImage);
  mremove.set_tooltip_text("Remove the layer");
  mremove.add_css_class("flat");
  mremove.signal_clicked().connect([this]() { handlers.removed(*this); });

  mrow.set_margin_start(MARGIN);
  mrow.set_margin_end(MARGIN);
  mrow.set_margin_top(2);
  mrow.set_margin_bottom(2);
  mrow.append(mhandle);
  mrow.append(mindex);
  mrow.append(mexpander);
  mrow.append(mremove);

  // the option rows below the header take the whole width
  mcolumn.append(mrow);
  mcolumn.append(mbody);
  set_child(mcolumn);

  // the index of the layer dragged by the handle
  auto source = Gtk::DragSource::create();

  source->set_actions(Gdk::DragAction::MOVE);
  source->signal_prepare().connect(
      [this](double, double) {
        Glib::Value<int> index;

        index.init(Glib::Value<int>::value_type());
        index.set(model.index_of(*msection));

        return Gdk::ContentProvider::create(index);
      },
      false);
  source->signal_drag_begin().connect(
      [this, source = source.get()](const Glib::RefPtr<Gdk::Drag>&) {
        source->set_icon(Gtk::WidgetPaintable::create(mheader), 0, 0);
      },
      false);
  mhandle.add_controller(source);

  auto target = Gtk::DropTarget::create(Glib::Value<int>::value_type(),
                                        Gdk::DragAction::MOVE);

  target->signal_drop().connect(
      [this](const Glib::ValueBase& value, double, double) {
        if (!G_VALUE_HOLDS_INT(value.gobj())) {
          return false;
        }

        handlers.moved(g_value_get_int(value.gobj()),
                       model.index_of(*msection));
        return true;
      },
      false);
  add_controller(target);

  refresh();
}

void LayerRow::refresh()
{
  std::size_t wrong = 0U;
  std::string first;

  for (const auto& row : net_editor::OptionPresenter::of(model, msection)) {
    const std::string error = row->error();

    if (!error.empty() && wrong++ == 0U) {
      first = row->key() + ": " + error;
    }
  }

  mindex.set_text(std::to_string(model.index_of(*msection)));
  msummary.set_text(msection->summary());
  merrors.set_text(wrong == 0U ? "" : "⚠ " + std::to_string(wrong) + " wrong");
  merrors.set_tooltip_text(first);

  if (moptions != nullptr) {
    moptions->refresh();
  }
}

const net_editor::NetworkSectionPtr& LayerRow::section() const
{
  return msection;
}

Gtk::Expander& LayerRow::expander() { return mexpander; }

Gtk::Button& LayerRow::remove_button() { return mremove; }

Gtk::Label& LayerRow::index_label() { return mindex; }

Gtk::Label& LayerRow::summary_label() { return msummary; }

Gtk::Label& LayerRow::errors_label() { return merrors; }

OptionsGrid* LayerRow::options() { return moptions.get(); }

void LayerRow::on_expanded()
{
  if (moptions == nullptr && mexpander.get_expanded()) {
    moptions =
        std::make_unique<OptionsGrid>(mgrid, model, msection, handlers.edited);
  }

  mbody.set_visible(mexpander.get_expanded());
}

}  // namespace darknetxxGtkmm4::window::widgets
