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

menu_t::menu_t(menuBar_t&)
    : type_(BAR)
{
    // note: the native pointer will be wxMenuBar
    //   and that wxMenuBar has already been created in menuBar_t ctor
}

menu_t::menu_t()
    : type_(MENU)
{
    wrMenu* tmp = new wrMenu(*this);
    set_native_pointer(static_cast<wxMenu*>(tmp));
}

menu_t::menu_t(const string_t& title)
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
    wxMenuItem* tmp = new wxMenuItem(m, get_an_id(), setup.Text, jjT(""), k2wxk(setup.Value));
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
    wxMenuItem* tmp = new wxMenuItem(m, get_an_id()/*wxID_SEPARATOR*/, jjT(""), jjT(""), wxITEM_SEPARATOR);
    m->Append(tmp);
    item_t x(new menuItem_t(*this, tmp->GetId()));
    items_.push_back(x);
    return x;
}

std::weak_ptr<menuItem_t> menu_t::append(menu_t* sub, menuItem_t::subOptions_t setup)
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
        wxMenuItem* tmp = new wxMenuItem(m, get_an_id(), setup.Text, jjT(""), wxITEM_NORMAL, GET<wxMenu>::from(sub));
        m->Append(tmp);
        item_t x(new menuItem_t(*this, tmp->GetId()));
        items_.push_back(x);
        return x;
    }
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
