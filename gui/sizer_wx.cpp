#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "jj/gui/common_wx.h"
#include "jj/gui/sizer_wx.h"

namespace jj
{
namespace gui
{
namespace // <anonymous>
{
struct wrBoxSizer : public nativeWrapper_t<boxSizer_t, wxBoxSizer>
{
    template<typename ... Ps>
    wrBoxSizer(boxSizer_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }
};
} // namespace <anonymous>

sizerFlags_t::sizerFlags_t()
    : Align(align_t::LEFT), AlignV(alignv_t::MIDDLE), Expand(DONT_EXPAND), Border(wxSizerFlags::GetDefaultBorder()), BorderV(wxSizerFlags::GetDefaultBorder()), Proportion(0)
{
}

static wxSizerFlags f2f(const sizerFlags_t& f)
{
    wxSizerFlags t;
    int flag = 0;
    switch (f.Align)
    {
    case align_t::LEFT: flag |= wxALIGN_TOP; break;
    case align_t::CENTER: flag |= wxALIGN_CENTER_HORIZONTAL; break;
    case align_t::RIGHT: flag |= wxALIGN_RIGHT; break;
    }
    switch (f.AlignV)
    {
    case alignv_t::TOP: flag |= wxALIGN_TOP; break;
    case alignv_t::MIDDLE: flag |= wxALIGN_CENTER_VERTICAL; break;
    case alignv_t::BOTTOM: flag |= wxALIGN_BOTTOM; break;
    }
    t.Align(flag);

    // wx can only store one border value
    if (f.Border == f.BorderV)
        t.Border(wxALL, f.Border);
    else if (f.Border > f.BorderV)
        t.Border(wxHORIZONTAL, f.Border);
    else
        t.Border(wxVERTICAL, f.BorderV);

    t.Proportion(f.Proportion);

    if (f.Expand == sizerFlags_t::EXPAND)
        t.Expand();

    return t;
}

sizer_t::sizer_t(contentHolder_t& owner)
    : parent_t(owner, SIZER), native_t(*this)
{
}

void sizer_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxSizer>::from(this));
}

void sizer_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

void sizer_t::add(content_t& content, sizerFlags_t flags)
{
    switch (content.type())
    {
    case CONTROL:
    {
        wxWindow* w = reinterpret_cast<wxWindow*>(content.native_pointer());
        GET<wxSizer>::from(this)->Add(w, f2f(flags));
        break;
    }
    case SIZER:
        wxSizer* s = reinterpret_cast<wxSizer*>(content.native_pointer());
        GET<wxSizer>::from(this)->Add(s, f2f(flags));
        break;
    }
}

boxSizer_t::boxSizer_t(contentHolder_t& owner, orientation_t o)
    : parent_t(owner), native_t(*this)
{
    wrBoxSizer* tmp = new wrBoxSizer(*this, o == HORIZONTAL ? wxHORIZONTAL : wxVERTICAL);
    set_native_pointer(static_cast<wxBoxSizer*>(tmp));
}

void boxSizer_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxSizer>::from(this));
}

void boxSizer_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
