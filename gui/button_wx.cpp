#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "jj/gui/common_wx.h"
#include "jj/gui/button.h"

namespace jj
{
namespace gui
{

namespace // <anonymous>
{
struct wrButton : public nativeWrapper_t<button_t, wxButton>
{
    template<typename ... Ps>
    wrButton(button_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }

    void evtButton(wxCommandEvent& evt)
    {
        owner_->OnClick(*owner_);
    }
};

static long bf2wxbs(const button_t::flags1_t& v)
{
    long ret = 0;
    if (v*button_t::EXACT_FIT) ret |= wxBU_EXACTFIT;
    if (v*button_t::NO_BORDER) ret |= wxBORDER_NONE;
    return ret;
}
static long ha2wxbs(const opt::e<align_t>& v)
{
    switch (v.Value)
    {
    case align_t::LEFT: return wxBU_LEFT;
    case align_t::RIGHT: return wxBU_RIGHT;
    default: return 0;
    }
}
static long va2wxbs(const opt::e<alignv_t>& v)
{
    switch (v.Value)
    {
    case alignv_t::TOP: return wxBU_TOP;
    case alignv_t::BOTTOM: return wxBU_BOTTOM;
    default: return 0;
    }
}
} // namespace <anonymous>

namespace AUX
{
template<> struct COGET<button_t> { typedef wxButton TYPE; };
} // namespace AUX

button_t::button_t(contentHolder_t& owner, options_t setup)
    : parent_t(owner), native_t(*this)
{
    wrButton* tmp = new wrButton(*this, GET<wxWindow>::from(&owner), 
        get_id(), setup.Title, 
        wxPoint(setup.Position.Left, setup.Position.Top), 
        wxSize(setup.Size.Width, setup.Size.Height), 
        bf2wxbs(setup)|ha2wxbs(setup)|va2wxbs(setup)
    );
    set_native_pointer(static_cast<wxButton*>(tmp));

    AUX::setup_first_last_callbacks(OnClick, 
        [this, tmp] { GET<wxButton>::from(this)->Bind(wxEVT_BUTTON, &wrButton::evtButton, tmp, this->get_id()); },
        [this, tmp] { GET<wxButton>::from(this)->Unbind(wxEVT_BUTTON, &wrButton::evtButton, tmp, this->get_id()); }
    );
}

void button_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxControl>::from(this));
}

void button_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
