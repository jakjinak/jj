#ifndef JJ_GUI_COMBO_BOX_H
#define JJ_GUI_COMBO_BOX_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

#include "jj/functionBag.h"

namespace jj
{
namespace gui
{

/*! Allows selecting text from a list of defined texts. */
class comboBox_t : public AUX::nativePointerWrapper_t<comboBox_t>, public control_t
{
public:
    /*! Flags available for tweaking the combo style. */
    enum flags_t
    {
        EDITABLE, //!< normally only items in list can be selected, with this user can type in custom value
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t; //!< all style modifiers
    typedef options_T<opt::text, opt::position, opt::size, flags1_t> options_t; //!< all available combo box options
    /*! Returns the default combo box options. */
    static options_t options() { return options_t(); }

private:
    typedef control_t parent_t;

public:
    /*! Ctor - default */
    comboBox_t(contentHolder_t& owner, options_t setup) : comboBox_t(owner, setup, {}) {}
    /*! Ctor - initialize the items to select from */
    comboBox_t(contentHolder_t& owner, const std::initializer_list<string_t>& items) : comboBox_t(owner, options(), items) {}
    /*! Ctor - initialize the items to select from */
    comboBox_t(contentHolder_t& owner, options_t setup, const std::initializer_list<string_t>& items);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<comboBox_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    static size_t npos; //!< a value identifying "no such item"
    /*! Returns the number of items in the list to select from. */
    size_t count() const;
    /*! Returns true if some item is selected. */
    bool is_selected() const;
    /*! Returns the index of selected item. */
    size_t selected() const;
    /*! Forces selection of item on given index. Just deselects with invalid index. */
    void select(size_t pos);

    /*! Returns the text of the selected item (or the custom-typed item). */
    string_t text() const;
    /*! Sets the text of selected item. Does nothing if not EDITABLE and text not in list. Invokes the respective callback. */
    void text(const string_t& v);
    /*! Like text(), but does not invoke the callbacks. */
    void changeText(const string_t& v);

    /*! Add new item to the end of list. */
    void append(const string_t& item);
    /* Insert new item to the defined position in list. */
    void insert(const string_t& item, size_t pos = 0);
    /*! Changes text of given item in the list. */
    void change(const string_t& item, size_t pos);
    /*! Removes given item from the list.*/
    void erase(size_t pos);

    /*! Invoked when an item is selected from the dropdown. */
    functionBag_t<void, comboBox_t&> OnSelect;
    /*! Invoked when the text in the edit part of combo changes. */
    functionBag_t<void, comboBox_t&> OnChange;
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_COMBO_BOX_H
