#ifndef JJ_GUI_TEXT_INPUT_H
#define JJ_GUI_TEXT_INPUT_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

#include "jj/functionBag.h"

namespace jj
{
namespace gui
{

/*! Defines a simple text editor (single- or multi-line). */
class textInput_t : public control_t, public AUX::nativePointerWrapper_t<textInput_t>
{
public:
    /*! Flags available for tweaking the text input style. */
    enum flags_t
    {
        MULTILINE, //!< changes the input to be multiline
        READONLY, //!< user cannot edit the value
        PASSWORD, //!< hide real value, instead show (usually) '*'
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t; //!< all style modifiers
    typedef options_T<opt::text, opt::position, opt::size, flags1_t, opt::e<align_t>> options_t; //!< defines all available text input options
    /*! Returns the default text input options. */
    static options_t options() { return options_t(); }

private:
    typedef control_t parent_t;

public:
    /*! Ctor */
    textInput_t(contentHolder_t& owner, options_t setup);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<textInput_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Returns the current text in the input. */
    string_t text() const;
    /*! Sets the text in input, invokes the OnTextChange. */
    void text(const string_t& v);
    /*! Sets the text in input without invoking the OnTextChange. */
    void changeText(const string_t& v);

    /*! Invoked when the text in the input is changed by user or with text(). */
    functionBag_t<void, textInput_t&> OnTextChange;
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_TEXT_INPUT_H
