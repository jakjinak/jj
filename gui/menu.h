#ifndef JJ_GUI_MENU_H
#define JJ_GUI_MENU_H

#include "jj/gui/common.h"

#if (JJ_USE_GUI)

#include "jj/gui/keys.h"
#include "jj/gui/eventCallback.h"

#include <memory>

namespace jj
{
namespace gui
{

class menu_t;
class menuBar_t;

class menuItem_t : protected idHolder_t
{
    menu_t& owner_;

protected:
    menuItem_t(menu_t& owner, int id);

    friend class menu_t;
public:
    enum kind_t { NORMAL, RADIO, CHECK };
    enum separator_t { SEPARATOR };

    typedef creationOptions_t<opt::text, opt::accelerator, opt::e<kind_t>> options_t;
    typedef creationOptions_t<opt::text> subOptions_t;

    eventCallback_t<void, menuItem_t&> OnClick;
};

class menu_t : public nativePointerWrapper_t<menu_t>, public idGenerator_t
{
    enum type_t { MENU, BAR };
    type_t type_;

    typedef std::shared_ptr<menuItem_t> item_t;
    typedef std::vector<item_t> items_t;
    items_t items_;

    menu_t(const menu_t&) = delete;
    menu_t(menu_t&&) = delete;
    menu_t& operator=(const menu_t&) = delete;
    menu_t& operator=(menu_t&&) = delete;

    friend class menuItem_t;
    friend class menuBar_t;
    menu_t(menuBar_t&);
public:
    menu_t();
    menu_t(const string_t& title);

    typedef nativePointerWrapper_t<menu_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    std::weak_ptr<menuItem_t> append(menuItem_t::options_t setup);
    std::weak_ptr<menuItem_t> append(menuItem_t::separator_t);
    std::weak_ptr<menuItem_t> append(menu_t* sub, menuItem_t::subOptions_t setup);
    //std::weak_ptr<menuItem_t> append(stockItem_t v);
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_MENU_H
