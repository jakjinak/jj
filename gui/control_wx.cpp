#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "jj/gui/common_wx.h"
#include "jj/gui/control.h"

namespace jj
{
namespace gui
{

namespace AUX
{
template<> struct COGET<content_t> { typedef wxWindow TYPE; };
template<> struct COGET<control_t> { typedef wxControl TYPE; };
} // namespace AUX

content_t::content_t(contentHolder_t& owner, type_t type)
    : owner_(&owner), type_(type), idHolder_t(wxID_ANY)
{
    set_id(owner.get_an_id());
    owner_->reg(this);
}

content_t::~content_t()
{
    if (owner_)
        owner_->unreg(this);
}

void content_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
}

void content_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
}


control_t::control_t(contentHolder_t& owner)
    : parent_t(owner, CONTROL), native_t(*this)
{
}

void control_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxWindow>::from(this));
}

void control_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
