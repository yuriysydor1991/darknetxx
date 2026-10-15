#include "src/gui/gtkmm4/net-editor-window/OptionRow.h"

#include <gtkmm/box.h>
#include <gtkmm/text.h>

#include <algorithm>

namespace darknetxxGtkmm4::window::widgets
{

OptionRow::OptionRow(Gtk::Grid& grid, const int row,
                     net_editor::OptionPresenterPtr npresenter,
                     std::function<void()> nedited)
    : mpresenter{std::move(npresenter)}, edited{std::move(nedited)}
{
  static constexpr const int EDITOR_WIDTH = 220;
  // the wrapped lines of no fewer characters, so the narrowest description
  // isn't the tallest one the row measures
  static constexpr const int DESCRIPTION_MIN_CHARS = 30;
  static constexpr const int DESCRIPTION_CHARS = 70;

  auto* key = Gtk::make_managed<Gtk::Label>(mpresenter->key());
  auto* about = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
  const std::string value = mpresenter->value();

  key->set_xalign(0.0F);
  key->set_valign(Gtk::Align::START);
  key->set_margin_top(6);
  key->set_selectable(true);
  key->add_css_class("monospace");

  mchoices = mpresenter->choices();

  if (mchoices.empty()) {
    mentry = Gtk::make_managed<Gtk::Entry>();
    mentry->set_text(value);
    mentry->set_size_request(EDITOR_WIDTH, -1);
    mentry->set_valign(Gtk::Align::START);

    // the characters of no value never get in (the text delegate of the
    // entry inserts the typed ones)
    Glib::wrap(
        GTK_TEXT(gtk_editable_get_delegate(GTK_EDITABLE(mentry->gobj()))))
        ->signal_insert_text()
        .connect(sigc::mem_fun(*this, &OptionRow::on_insert), false);
    mentry->signal_changed().connect(
        sigc::mem_fun(*this, &OptionRow::on_changed));
    grid.attach(*mentry, 1, row);
  } else {
    auto names = Gtk::StringList::create();

    mchoices.insert(mchoices.begin(), std::string{});

    // the wrong value of the file stays to be told
    if (std::find(mchoices.begin(), mchoices.end(), value) == mchoices.end()) {
      mchoices.push_back(value);
    }

    for (const auto& choice : mchoices) {
      names->append(choice.empty() ? mpresenter->placeholder() : choice);
    }

    mdropdown = Gtk::make_managed<Gtk::DropDown>(names);
    mdropdown->set_selected(static_cast<guint>(
        std::find(mchoices.begin(), mchoices.end(), value) - mchoices.begin()));
    mdropdown->set_size_request(EDITOR_WIDTH, -1);
    mdropdown->set_valign(Gtk::Align::START);
    mdropdown->property_selected().signal_changed().connect(
        sigc::mem_fun(*this, &OptionRow::on_selected));
    grid.attach(*mdropdown, 1, row);
  }

  mdescription = Gtk::make_managed<Gtk::Label>();
  mdescription->set_xalign(0.0F);
  mdescription->set_wrap(true);
  mdescription->set_width_chars(DESCRIPTION_MIN_CHARS);
  mdescription->set_max_width_chars(DESCRIPTION_CHARS);
  mdescription->add_css_class("dim-label");

  merror = Gtk::make_managed<Gtk::Label>();
  merror->set_xalign(0.0F);
  merror->set_wrap(true);
  merror->set_width_chars(DESCRIPTION_MIN_CHARS);
  merror->set_max_width_chars(DESCRIPTION_CHARS);
  merror->add_css_class("error");

  about->set_hexpand(true);
  about->set_margin_top(6);
  about->append(*mdescription);
  about->append(*merror);

  grid.attach(*key, 0, row);
  grid.attach(*about, 2, row);

  refresh();
}

void OptionRow::refresh()
{
  const std::string error = mpresenter->error();

  mdescription->set_text(mpresenter->description());
  merror->set_text(error);
  merror->set_visible(!error.empty());

  if (mentry != nullptr) {
    mentry->set_placeholder_text(mpresenter->placeholder());

    if (error.empty()) {
      mentry->remove_css_class("error");
    } else {
      mentry->add_css_class("error");
    }
  }
}

const net_editor::OptionPresenter& OptionRow::presenter() const
{
  return *mpresenter;
}

Gtk::Entry* OptionRow::entry() const { return mentry; }

Gtk::DropDown* OptionRow::dropdown() const { return mdropdown; }

Gtk::Label& OptionRow::description() const { return *mdescription; }

Gtk::Label& OptionRow::error() const { return *merror; }

void OptionRow::on_insert(const Glib::ustring& text, int* position)
{
  Glib::ustring result = mentry->get_text();

  result.insert(static_cast<Glib::ustring::size_type>(*position), text);

  if (!mpresenter->typable(result.raw())) {
    g_signal_stop_emission_by_name(
        gtk_editable_get_delegate(GTK_EDITABLE(mentry->gobj())), "insert-text");
    mentry->error_bell();
  }
}

void OptionRow::on_changed()
{
  mpresenter->value(mentry->get_text().raw());
  edited();
}

void OptionRow::on_selected()
{
  const guint selected = mdropdown->get_selected();

  mpresenter->value(selected < mchoices.size() ? mchoices[selected]
                                               : std::string{});
  edited();
}

}  // namespace darknetxxGtkmm4::window::widgets
