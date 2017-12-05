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
} // namespace <anonymous>

namespace AUX
{
template<> struct COGET<button_t> { typedef wxButton TYPE; };
} // namespace AUX

button_t::button_t(contentHolder_t& owner, buttonOptions_t setup)
    : parent_t(owner), native_t(*this)
{
    wrButton* tmp = new wrButton(*this, GET<wxWindow>::from(&owner), get_id(), setup.Title, wxPoint(setup.Position.Left, setup.Position.Top), wxSize(setup.Size.Width, setup.Size.Height), setup.Style);
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
