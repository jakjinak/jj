#ifndef JJ_GUI_MENU_H
#define JJ_GUI_MENU_H

#include "jj/gui/common.h"

#if (JJ_USE_GUI)

#include "jj/gui/keys.h"
#include "jj/functionBag.h"

#include <memory>

namespace jj
{
namespace gui
{

// forward declaration
class menu_t;
class menuBar_t;

/*! Abstracts an item within a menu. */
class menuItem_t : protected idHolder_t
{
    menu_t& owner_; //!< item belongs to this menu

    /*! Ctor - only the menu can create its items */
    menuItem_t(menu_t& owner, int id);

    friend class menu_t;
public:
    /*! Defines whether regular or with special features. */
    enum kind_t { NORMAL, RADIO, CHECK };
    /*! A special value to for adding separator fake items into menu. */
    enum separator_t { SEPARATOR };


    typedef jj::options_T<opt::text, opt::accelerator, opt::e<kind_t>> options_t; //!< defines all available item options
    /*! Returns default menu item options. */
    static options_t options() { return options_t(); }
    typedef jj::options_T<opt::text> submenuOptions_t; //!< defines all available item options for submenu items
    /*! Returns default menu item options for submenu items. */
    static submenuOptions_t submenu_options() { return submenuOptions_t(); }

    /*! Returns whether item is checked. */
    bool checked() const;
    /*! Sets the checked state. */
    void check(bool v = true);
    /*! Uncheks the item. */
    void uncheck() { check(false); }

    /*! Returns whether the item is enabled (can be selected). */
    bool enabled() const;
    /*! Sets the enabled state. */
    void enable(bool v = true);
    /* Disables the item. */
    void disable() { enable(false); }

    /*! Returns label of the item. */
    string_t text() const;
    /*! Sets label of the item. */
    void text(const string_t& v);

    /*! Invoked when the item is selected. */
    functionBag_t<void, menuItem_t&> OnClick;
};

/*! A menu of items. This can be a submenu or the main menu bar. */
class menu_t : public AUX::nativePointerWrapper_t<menu_t>
{
    idGenerator_t& idsrc_; //!< generates ids, actually comes from the parent window

    /*! Type of menu. */
    enum type_t { MENU, BAR };
    type_t type_; //!< type of menu

    typedef std::shared_ptr<menuItem_t> item_t; //!< a menu item - it is owned by the menu
    typedef std::vector<item_t> items_t; //!< type of container holding items
    items_t items_; //!< all menu items

    menu_t(const menu_t&) = delete;
    menu_t(menu_t&&) = delete;
    menu_t& operator=(const menu_t&) = delete;
    menu_t& operator=(menu_t&&) = delete;

    friend class menuItem_t;
    friend class menuBar_t;
    /*! Special ctor used when the main manu bar is being created. */
    menu_t(idGenerator_t& idsrc, menuBar_t&);
public:
    /*! Ctor - submenu */
    menu_t(idGenerator_t& idsrc);
    /*! Ctor - context menu (with a label, usually on the side) */
    menu_t(idGenerator_t& idsrc, const string_t& title);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<menu_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Adds an item into the menu and returns it. */
    std::weak_ptr<menuItem_t> append(menuItem_t::options_t setup);
    /*! Adds a separator into the menu and returns it. */
    std::weak_ptr<menuItem_t> append(menuItem_t::separator_t);
    /*! Adds a submenu item into the menu and returns it. */
    std::weak_ptr<menuItem_t> append(menu_t* sub, menuItem_t::submenuOptions_t setup);
    /*! Adds a submenu item into the menu and returns it. */
    std::weak_ptr<menuItem_t> append(menu_t* sub, const string_t& text) { return append(sub, menuItem_t::submenu_options() << opt::text(text)); }
    //std::weak_ptr<menuItem_t> append(stockItem_t v);
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_MENU_H
