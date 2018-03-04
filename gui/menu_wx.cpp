#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "jj/gui/common_wx.h"
#include "jj/gui/menu.h"
#include "jj/gui/keys_wx.h"

namespace jj
{
namespace gui
{

namespace AUX
{
template<> struct COGET<menu_t> { typedef wxMenu TYPE; };
} // namespace AUX

namespace // <anonymous>
{

struct wrItem
{
    menuItem_t* item_;
    void evtMenu(wxCommandEvent& evt)
    {
        item_->OnClick(*item_);
    }
};

struct wrMenu : public nativeWrapper_t<menu_t, wxMenu>
{
    template<typename ... Ps>
    wrMenu(menu_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }

    std::vector<std::shared_ptr<wrItem>> wrappers_;
};

static wxItemKind k2wxk(menuItem_t::kind_t k)
{
    switch (k)
    {
    case menuItem_t::NORMAL:
    default:
        return wxITEM_NORMAL;
    case menuItem_t::RADIO:
        return wxITEM_RADIO;
    case menuItem_t::CHECK:
        return wxITEM_CHECK;
    }
}

static wxMenuItem& get_menu_item(menu_t& m, int id)
{
    wrMenu* menu = dynamic_cast<wrMenu*>(GET<wxMenu>::from(&m));
    if (menu == nullptr)
        throw std::invalid_argument("Internal error: no menu in menu_t");
    wxMenuItem* item = menu->FindChildItem(id, nullptr);
    if (item == nullptr)
        throw std::invalid_argument("Internal error: no menuitem found for id");
    if (item->GetId() != id)
        throw std::invalid_argument("Internal error: ids of menuitems do not match");
    return *item;
}

} // namespace <anonymous>

menuItem_t::menuItem_t(menu_t& owner, int id) 
    : idHolder_t(id), owner_(owner)
{
    if (owner.type_ == menu_t::MENU)
    {
        wrMenu* m = dynamic_cast<wrMenu*>(GET<wxMenu>::from(&owner_));
        wrItem* pw;
        std::shared_ptr<wrItem> w(pw = new wrItem);
        w->item_ = this;
        m->wrappers_.push_back(w);
        AUX::setup_first_last_callbacks(OnClick,
            [this, m, pw] { m->Bind(wxEVT_MENU, &wrItem::evtMenu, pw, this->get_id()); },
            [this, m, pw] { m->Unbind(wxEVT_MENU, &wrItem::evtMenu, pw, this->get_id()); }
        );
    }
}

bool menuItem_t::checked() const
{
    wxMenuItem& item = get_menu_item(owner_, get_id());
    return item.IsCheckable() && item.IsChecked();
}

void menuItem_t::check(bool v)
{
    wxMenuItem& item = get_menu_item(owner_, get_id());
    if (!item.IsCheckable())
        return;
    item.Check(v);
}

bool menuItem_t::enabled() const
{
    return get_menu_item(owner_, get_id()).IsEnabled();
}

void menuItem_t::enable(bool v)
{
    get_menu_item(owner_, get_id()).Enable(v);
}

string_t menuItem_t::text() const
{
    return wxs2s<string_t>::cvt(get_menu_item(owner_, get_id()).GetItemLabel());
}

void menuItem_t::text(const string_t& v)
{
    get_menu_item(owner_, get_id()).SetItemLabel(v);
}

menu_t::menu_t(idGenerator_t& idsrc, menuBar_t&)
    : idsrc_(idsrc), type_(BAR)
{
    // note: the native pointer will be wxMenuBar
    //   and that wxMenuBar has already been created in menuBar_t ctor
}

menu_t::menu_t(idGenerator_t& idsrc)
    : idsrc_(idsrc), type_(MENU)
{
    wrMenu* tmp = new wrMenu(*this);
    set_native_pointer(static_cast<wxMenu*>(tmp));
}

menu_t::menu_t(idGenerator_t& idsrc, const string_t& title)
    : idsrc_(idsrc), type_(MENU)
{
    wrMenu* tmp = new wrMenu(*this, title);
    set_native_pointer(static_cast<wxMenu*>(tmp));
}

void menu_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
}

void menu_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
}

std::weak_ptr<menuItem_t> menu_t::append(menuItem_t::options_t setup)
{
    if (type_ == menu_t::BAR) // TODO this (in theory) is also allowed (via wxMenuBar::Append(0, "txt"); try and implement
        throw std::invalid_argument("no regular items in menu bar");
    wxMenu* m = GET<wxMenu>::from(this);
    wxMenuItem* tmp = new wxMenuItem(m, idsrc_.get_an_id(), setup.Text, jj::str::Empty, k2wxk(setup.Value));
    m->Append(tmp);
    if (!setup.Accelerator.empty())
    {
        wxAcceleratorEntry ae(keys::a2ae(setup.Accelerator));
        tmp->SetAccel(&ae);
    }
    item_t x(new menuItem_t(*this, tmp->GetId()));
    items_.push_back(x);
    return x;
}

std::weak_ptr<menuItem_t> menu_t::append(menuItem_t::separator_t)
{
    if (type_ == menu_t::BAR)
        throw std::invalid_argument("no separators in menu bar");
    wxMenu* m = GET<wxMenu>::from(this);
    wxMenuItem* tmp = new wxMenuItem(m, idsrc_.get_an_id()/*wxID_SEPARATOR*/, jj::str::Empty, jj::str::Empty, wxITEM_SEPARATOR);
    m->Append(tmp);
    item_t x(new menuItem_t(*this, tmp->GetId()));
    items_.push_back(x);
    return x;
}

std::weak_ptr<menuItem_t> menu_t::append(menu_t* sub, menuItem_t::submenuOptions_t setup)
{
    if (type_ == menu_t::BAR)
    {
        wxMenuBar* m = reinterpret_cast<wxMenuBar*>(native_pointer());
        m->Append(GET<wxMenu>::from(sub), setup.Text);
        item_t x(new menuItem_t(*this, int(m->GetMenuCount() - 1)));
        items_.push_back(x);
        return x;
    }
    else
    {
        wxMenu* m = GET<wxMenu>::from(this);
        wxMenuItem* tmp = new wxMenuItem(m, idsrc_.get_an_id(), setup.Text, jj::str::Empty, wxITEM_NORMAL, GET<wxMenu>::from(sub));
        m->Append(tmp);
        item_t x(new menuItem_t(*this, tmp->GetId()));
        items_.push_back(x);
        return x;
    }
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
