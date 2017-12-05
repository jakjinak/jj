#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "jj/gui/common_wx.h"
#include "jj/gui/textInput.h"

namespace jj
{
namespace gui
{

namespace // <anonymous>
{

struct wrTextInput : public nativeWrapper_t<textInput_t, wxTextCtrl>
{
    template<typename ... Ps>
    wrTextInput(textInput_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }

    void evtText(wxCommandEvent& evt)
    {
        owner_->OnTextChange(*owner_);
    }
};

} // namespace <anonymous>

namespace AUX
{
template<> struct COGET<textInput_t> { typedef wxTextCtrl TYPE; };
} // namespace AUX

textInput_t::textInput_t(contentHolder_t& owner, textInputOptions_t setup)
    : parent_t(owner), native_t(*this)
{
    wrTextInput* tmp = new wrTextInput(*this, GET<wxWindow>::from(&owner), get_id(), setup.Text, wxPoint(setup.Position.Left, setup.Position.Top), wxSize(setup.Size.Width, setup.Size.Height), setup.Style);
    set_native_pointer(static_cast<wxTextCtrl*>(tmp));

    AUX::setup_first_last_callbacks(OnTextChange,
        [this, tmp] { GET<wxTextCtrl>::from(this)->Bind(wxEVT_TEXT, &wrTextInput::evtText, tmp, this->get_id()); },
        [this, tmp] { GET<wxTextCtrl>::from(this)->Unbind(wxEVT_TEXT, &wrTextInput::evtText, tmp, this->get_id()); }
    );
}

void textInput_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxControl>::from(this));
}

void textInput_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

string_t textInput_t::value() const
{
    return wxs2s<string_t>::cvt(GET<wxTextCtrl>::from(this)->GetValue());
}

void textInput_t::setValue(const string_t& v)
{
    GET<wxTextCtrl>::from(this)->SetValue(v);
}

void textInput_t::changeValue(const string_t& v)
{
    GET<wxTextCtrl>::from(this)->ChangeValue(v);
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
