#ifndef JJ_GUI_STOCK_H
#define JJ_GUI_STOCK_H

#include "jj/defines.h"

#if (JJ_USE_GUI)

namespace jj
{
namespace gui
{
namespace stock
{
/*! Denotes defined stock ids for modal results. */
enum class item_t
{
    OK,
    CANCEL,
    YES,
    NO,
    MAX_STOCK
};

/*! Denotes defined stock ids for icons (in dialogs). */
enum class icon_t
{
    NONE,
    INFO,
    QUESTION,
    EXCLAMATION,
    ERR, // note: imposible to call it ERROR because such macro is defined in wingdi.h
    NEED_AUTH
};

} // namespace stock
} // namespace gui
} // namespace jj

template<typename T>
bool operator<(T x, jj::gui::stock::item_t y)
{
    return x < T(y);
}
template<typename T>
bool operator==(T x, jj::gui::stock::icon_t y)
{
    return x == T(y);
}

#endif // JJ_USE_GUI

#endif // JJ_GUI_STOCK_H
