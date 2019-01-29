#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "common_wx.h"
#include "jj/exceptionTypes.h"
#include "jj/gui/tree.h"

namespace jj
{
namespace gui
{

class tree_base_t::treeImpl_t
{
    tree_base_t& owner_;
public:
    treeImpl_t(tree_base_t& owner) : owner_(owner) {}

    bool on_before_select(id_t newid, id_t oldid) { return owner_.on_before_select(newid, oldid); }
    void on_select(id_t newid, id_t oldid) { owner_.on_select(newid, oldid); }
    void on_activate(id_t id) { owner_.on_activate(id); }
};

namespace // <anonymous>
{
struct wrTreeCtrl : public nativeWrapper_t<tree_base_t, wxTreeCtrl>
{
    template<typename ... Ps>
    wrTreeCtrl(tree_base_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }

    void evtBeforeSelect(wxTreeEvent& evt)
    {
        if (!owner_->impl().on_before_select(evt.GetItem(), evt.GetOldItem()))
            evt.Veto();
    }
    void evtSelect(wxTreeEvent& evt)
    {
        owner_->impl().on_select(evt.GetItem(), evt.GetOldItem());
    }
    void evtActivate(wxTreeEvent& evt)
    {
        owner_->impl().on_activate(evt.GetItem());
    }
};

static long tcf2wxtcs(const tree_base_t::flags1_t& v)
{
    long style = 0;
    if (v * tree_base_t::EDITABLE) style |= wxTR_EDIT_LABELS;
    if (v * tree_base_t::ROOT_IS_HIDDEN) style |= wxTR_HIDE_ROOT;
    if (v * tree_base_t::LINES)
        ; // nothing to be done - default
    else
        style |= wxTR_NO_LINES;
    if (v * tree_base_t::LINES_AT_ROOT) style |= wxTR_LINES_AT_ROOT;
    if (v * tree_base_t::FULL_ROW_HIGHLIGHT) style |= wxTR_FULL_ROW_HIGHLIGHT;
    return style;
}

static long tcb2wxtcs(const tree_base_t::buttonType1_t& v)
{
    switch (v.Value)
    {
    case tree_base_t::NONE:
    default:
        return wxTR_NO_BUTTONS;
    case tree_base_t::REGULAR:
        return wxTR_HAS_BUTTONS;
    case tree_base_t::TWIST:
        return wxTR_TWIST_BUTTONS;
    }
}

static long tcs2wxtcs(const tree_base_t::selection1_t& v)
{
    switch (v.Value)
    {
    case tree_base_t::SINGLE:
    default:
        return wxTR_SINGLE;
    case tree_base_t::RANGE:
        return wxTR_MULTIPLE;
    }
}

} // namespace <anonymous>

namespace AUX
{
template<> struct COGET<tree_base_t> { typedef wxTreeCtrl TYPE; };
} // namespace AUX

treeItem_base_t::~treeItem_base_t()
{
}

treeItem_base_t::id_t treeItem_base_t::parent() const
{
    return GET<wxTreeCtrl>::from(&parent_)->GetItemParent(id_);
}

void treeItem_base_t::text(const jj::string_t& t)
{
    GET<wxTreeCtrl>::from(&parent_)->SetItemText(id_, s2wxs<string_t>::cvt(t));
}

jj::string_t treeItem_base_t::text() const
{
    return wxs2s<string_t>::cvt(GET<wxTreeCtrl>::from(&parent_)->GetItemText(id_));
}

void treeItem_base_t::bold(bool b)
{
    GET<wxTreeCtrl>::from(&parent_)->SetItemBold(id_, b);
}

bool treeItem_base_t::bold() const
{
    return GET<wxTreeCtrl>::from(&parent_)->IsBold(id_);
}

bool treeItem_base_t::expanded() const
{
    return GET<wxTreeCtrl>::from(&parent_)->IsExpanded(id_);
}

void treeItem_base_t::collapse()
{
    GET<wxTreeCtrl>::from(&parent_)->Collapse(id_);
}

void treeItem_base_t::expand()
{
    GET<wxTreeCtrl>::from(&parent_)->Expand(id_);
}

void treeItem_base_t::collapse_recursively()
{
    GET<wxTreeCtrl>::from(&parent_)->CollapseAllChildren(id_);
}

void treeItem_base_t::expand_recursively()
{
    GET<wxTreeCtrl>::from(&parent_)->ExpandAllChildren(id_);
}

treeItem_base_t::id_t treeItem_base_t::new_item(const jj::string_t& text)
{
    return GET<wxTreeCtrl>::from(&parent_)->AppendItem(id_, s2wxs<string_t>::cvt(text));
}
treeItem_base_t::id_t treeItem_base_t::new_item(size_t index, const jj::string_t& text)
{
    return GET<wxTreeCtrl>::from(&parent_)->InsertItem(id_, index, s2wxs<string_t>::cvt(text));
}
void treeItem_base_t::remove_item(id_t id)
{
    GET<wxTreeCtrl>::from(&parent_)->Delete(id);
}

void treeItem_base_t::throw_invalid_index(size_t index, size_t max) const
{
    throw jj::exception::out_of_range<size_t>(index, max);
}

treeItem_base_t::id_t tree_base_t::current_item() const
{
    return GET<wxTreeCtrl>::from(this)->GetFocusedItem();
}

bool tree_base_t::is_valid_item(id_t id) const
{
    return id != nullptr;
}

tree_base_t::tree_base_t(contentHolder_t& owner, options_t setup)
    : parent_t(owner), native_t(*this), impl_(new treeImpl_t(*this))
{
    wrTreeCtrl* tmp = new wrTreeCtrl(*this, GET<wxWindow>::from(&owner),
        get_id(),
        wxPoint(setup.Position.Left, setup.Position.Top),
        wxSize(setup.Size.Width, setup.Size.Height),
        tcb2wxtcs(setup) | tcf2wxtcs(setup) | tcs2wxtcs(setup));
    set_native_pointer(static_cast<wxTreeCtrl*>(tmp));

    rootId_ = tmp->AddRoot(s2wxs<string_t>::cvt(setup.Text));

    startBeforeSelect = [this, tmp] { GET<wxTreeCtrl>::from(this)->Bind(wxEVT_TREE_SEL_CHANGING, &wrTreeCtrl::evtBeforeSelect, tmp, this->get_id()); };
    stopBeforeSelect = [this, tmp] { GET<wxTreeCtrl>::from(this)->Unbind(wxEVT_TREE_SEL_CHANGING, &wrTreeCtrl::evtBeforeSelect, tmp, this->get_id()); };
    startOnSelect = [this, tmp] { GET<wxTreeCtrl>::from(this)->Bind(wxEVT_TREE_SEL_CHANGED, &wrTreeCtrl::evtSelect, tmp, this->get_id()); };
    stopOnSelect = [this, tmp] { GET<wxTreeCtrl>::from(this)->Unbind(wxEVT_TREE_SEL_CHANGED, &wrTreeCtrl::evtSelect, tmp, this->get_id()); };
    startOnActivate = [this, tmp] { GET<wxTreeCtrl>::from(this)->Bind(wxEVT_TREE_ITEM_ACTIVATED, &wrTreeCtrl::evtActivate, tmp, this->get_id()); };
    stopOnActivate = [this, tmp] { GET<wxTreeCtrl>::from(this)->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &wrTreeCtrl::evtActivate, tmp, this->get_id()); };
}

void tree_base_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxControl>::from(this));
}

void tree_base_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

void tree_base_t::collapse_recursive()
{
    GET<wxTreeCtrl>::from(this)->CollapseAll();
}

void tree_base_t::expand_recursive()
{
    GET<wxTreeCtrl>::from(this)->ExpandAll();
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
