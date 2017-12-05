#ifndef JJ_GUI_SIZER_WX_H
#define JJ_GUI_SIZER_WX_H

#include "jj/gui/common_wx.h"
#include "jj/gui/sizer.h"

namespace jj
{
namespace gui
{

namespace AUX
{
template<> struct COGET<sizer_t> { typedef wxSizer TYPE; };
template<> struct COGET<boxSizer_t> { typedef wxBoxSizer TYPE; };
} // namespace AUX

} // namespace gui
} // namespace jj

#endif // JJ_GUI_SIZER_WX_H
