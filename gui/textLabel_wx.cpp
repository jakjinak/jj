#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "common_wx.h"
#include "textLabel.h"

namespace jj
{
namespace gui
{

namespace // <anonymous>
{
struct wrTextLabel : public nativeWrapper_t<textLabel_t, wxStaticText>
{
    template<typename ... Ps>
    wrTextLabel(textLabel_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }
};

static long tlf2wxsts(const textLabel_t::flags1_t& v)
{
    long ret = 0;
    if (v*textLabel_t::NO_RESIZE) ret |= wxST_NO_AUTORESIZE;
    return ret;
}
static long ha2wxsts(const opt::e<align_t>& v)
{
    switch (v.Value)
    {
    case align_t::CENTER: return wxALIGN_CENTER;
    case align_t::RIGHT: return wxALIGN_RIGHT;
    case align_t::LEFT:
    default: return wxALIGN_LEFT;
    }
}

} // namespace <anonymous>

namespace AUX
{
template<> struct COGET<textLabel_t> { typedef wxStaticText TYPE; };
} // namespace AUX

textLabel_t::textLabel_t(contentHolder_t& owner, options_t setup)
    : parent_t(owner), native_t(*this)
{
    wrTextLabel* tmp = new wrTextLabel(*this, GET<wxWindow>::from(&owner),
        get_id(), setup.Text,
        wxPoint(setup.Position.Left, setup.Position.Top),
        wxSize(setup.Size.Width, setup.Size.Height),
        tlf2wxsts(setup) | ha2wxsts(setup)
    );
    set_native_pointer(static_cast<wxStaticText*>(tmp));
}

void textLabel_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxControl>::from(this));
}

void textLabel_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

string_t textLabel_t::text() const
{
    return wxs2s<string_t>::cvt(GET<wxStaticText>::from(this)->GetLabel());
}

void textLabel_t::text(const string_t& v)
{
    GET<wxStaticText>::from(this)->SetLabel(v);
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
