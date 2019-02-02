#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "jj/gui/common_wx.h"
#include "jj/gui/sizer_wx.h"
#include "jj/gui/common.h"

namespace jj
{
namespace opt
{
position::position() : Position(gui::pt2p(wxDefaultPosition)) {}
size::size() : Size(gui::siz2p(wxDefaultSize)) {}
} // namespace opt

namespace gui
{

contentHolder_t::contentHolder_t()
    : sizer_(nullptr), OnCreateSizer([this] { return new boxSizer_t(*this, boxSizer_t::VERTICAL); })
{
}

contentHolder_t::~contentHolder_t()
{
    for (items_t::value_type item : items())
        item->disown();
}

void contentHolder_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
}

void contentHolder_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
}

void contentHolder_t::set_sizer()
{
    GET<wxWindow>::from(this)->SetSizer(GET<wxSizer>::from(sizer_));
}

void contentHolder_t::layout()
{
    wxWindow* w = GET<wxWindow>::from(this);
    wxSizer* s = w->GetSizer();
    if (s == nullptr)
        return;
    s->Layout();
    w->SendSizeEvent();
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
