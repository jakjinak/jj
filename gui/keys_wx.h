#ifndef JJ_GUI_KEYS_WX_H
#define JJ_GUI_KEYS_WX_H

#include "jj/gui/keys.h"

#if (JJ_USE_GUI)

#include "wx/wx.h"

namespace jj
{
namespace gui
{
namespace keys
{

inline int m2aef(mod_t m)
{
    int r = 0;
    if (m&ALT) r |= wxACCEL_ALT;
    if (m&CTRL) r |= wxACCEL_CTRL;
    if (m&SHIFT) r |= wxACCEL_SHIFT;
    return r;
}

inline wxAcceleratorEntry a2ae(accelerator_t a)
{
    if (a.empty())
        return wxAcceleratorEntry(wxACCEL_NORMAL, WXK_NONE, wxID_ANY);
    if (a.regular())
        return wxAcceleratorEntry(m2aef(a.modifier()), a.key(), wxID_ANY);

    int code = 0;
    switch (a.special_key())
    {
    case F1: code = WXK_F1; break;
    case F2: code = WXK_F2; break;
    case F3: code = WXK_F3; break;
    case F4: code = WXK_F4; break;
    case F5: code = WXK_F5; break;
    case F6: code = WXK_F6; break;
    case F7: code = WXK_F7; break;
    case F8: code = WXK_F8; break;
    case F9: code = WXK_F9; break;
    case F10: code = WXK_F10; break;
    case F11: code = WXK_F11; break;
    case F12: code = WXK_F12; break;
    }
    return wxAcceleratorEntry(m2aef(a.modifier()), code, wxID_ANY);
}

} // namespace keys
} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_KEYS_WX_H
