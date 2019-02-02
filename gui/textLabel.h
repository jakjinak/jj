#ifndef JJ_GUI_TEXT_LABEL_H
#define JJ_GUI_TEXT_LABEL_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

namespace jj
{
namespace gui
{

/*! A simple label shown in window without further functionality. */
class textLabel_t : public AUX::nativePointerWrapper_t<textLabel_t>, public control_t
{
public:
    /*! Flags available for tweaking the text label's style. */
    enum flags_t
    {
        NO_RESIZE, //!< by default the label size is adjusted to fit the text size, adding this disables that
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t; //!< all style modifiers
    typedef options_T<opt::text, opt::position, opt::size, flags1_t, opt::e<align_t>> options_t; //!< defines all available text label options
    /*! Returns the default text label options. */
    static options_t options() { return options_t(); }

private:
    typedef control_t parent_t;

public:
    /*! Ctor */
    textLabel_t(contentHolder_t& owner, options_t setup);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<textLabel_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Returns the text in label. */
    string_t text() const;
    /*! Sets the text in label. */
    void text(const string_t& v);
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_TEXT_LABEL_H
