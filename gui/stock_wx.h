#ifndef JJ_GUI_STOCK_WX_H
#define JJ_GUI_STOCK_WX_H

#include "jj/defines.h"

#if (JJ_USE_GUI)

#include <wx/wx.h>
#include "jj/gui/stock.h"

namespace jj
{
namespace gui
{

inline int stock2wx(stock::item_t v)
{
    switch (v)
    {
    case stock::item_t::OK: return wxID_OK;
    case stock::item_t::CANCEL: return wxID_CANCEL;
    case stock::item_t::YES: return wxID_YES;
    case stock::item_t::NO: return wxID_NO;
    }
    return wxID_NONE;
}

inline stock::item_t wx2stock(int v)
{
    switch (v)
    {
    default:
    case wxID_OK: return stock::item_t::OK;
    case wxID_CANCEL: return stock::item_t::CANCEL;
    case wxID_YES: return stock::item_t::YES;
    case wxID_NO: return stock::item_t::NO;
    }
}

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_STOCK_WX_H
