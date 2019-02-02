#ifndef JJ_GUI_BUTTON_H
#define JJ_GUI_BUTTON_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

#include "jj/functionBag.h"

namespace jj
{
namespace gui
{

/*! A simple clickable button. */
class button_t : public AUX::nativePointerWrapper_t<button_t>, public control_t
{
public:
    /*! Flags available for tweaking the button style. */
    enum flags_t
    {
        EXACT_FIT, //!< enables auto-adjusting button's size to it's label's size
        NO_BORDER, //!< disable drawing border of the button
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t; //! all style modifiers
    typedef options_T<opt::text, opt::position, opt::size, flags1_t, opt::e<align_t>, opt::e<alignv_t>> options_t; //!< defines all available button options
    /*! Returns default button options. */
    static options_t options() { return options_t() << align_t::CENTER << alignv_t::MIDDLE; }

private:
    typedef control_t parent_t;

public:
    /*! Ctor */
    button_t(contentHolder_t& owner, options_t setup);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<button_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Returns the button's label. */
    string_t text() const;
    /*! Sets the button's label */
    void text(const string_t& v);

    /*! Invoked when the button is clicked. */
    functionBag_t<void, button_t&> OnClick;
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_BUTTON_H
