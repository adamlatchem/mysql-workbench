#ifndef __GRID_VIEW_MODEL_H__
#define __GRID_VIEW_MODEL_H__


#include "linux_utilities/listmodel_wrapper.h"
#include "grt/tree_model.h"


class GridView;


class GridViewModel : public ListModelWrapper
{
public:
  typedef Glib::RefPtr<GridViewModel> Ref;
  static Ref create(bec::GridModel::Ref model, GridView *view, const std::string &name);
  ~GridViewModel();

  virtual bool handle_popup_event(GdkEvent* event);
  int refresh(bool reset_columns);
  int column_index(Gtk::TreeViewColumn* col);
  void row_numbers_visible(bool value) { _row_numbers_visible= value; }
  bool row_numbers_visible() { return _row_numbers_visible; }
  void set_ellipsize(const int column, const bool on);

  sigc::slot<void, const int, Glib::ValueBase*>   before_render;

protected:
  GridViewModel(bec::GridModel::Ref model, GridView *view, const std::string &name);
  virtual void get_value_vfunc(const iterator& iter, int column, Glib::ValueBase& value) const;

private:
  bec::GridModel::Ref                   _model;
  GridView                             *_view;
  std::map<Gtk::TreeViewColumn*, int>   _col_index_map;
  bool                                  _row_numbers_visible;

  template <typename ValueTypeTraits>
  Gtk::TreeViewColumn * add_column(int index, const std::string &name, Editable editable, Gtk::TreeModelColumnBase *color_column);

  void get_cell_value(const iterator& iter, int column, GType type, Glib::ValueBase& value);
  void set_cell_value(const iterator& itier, int column, GType type, const Glib::ValueBase& value);
};


#endif // __GRID_VIEW_MODEL_H__
