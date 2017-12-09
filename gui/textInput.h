#ifndef JJ_GUI_TEXT_INPUT_H
#define JJ_GUI_TEXT_INPUT_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

#include "jj/gui/eventCallback.h"

namespace jj
{
namespace gui
{

class textInput_t : public control_t, public nativePointerWrapper_t<textInput_t>
{
public:
    enum flags_t
    {
        MULTILINE,
        READONLY,
        PASSWORD,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef creationOptions_t<opt::text, opt::position, opt::size, flags1_t, opt::e<align_t>> options_t;
    static options_t options() { return options_t(); }

private:
    typedef control_t parent_t;

public:
    textInput_t(contentHolder_t& owner, options_t setup);

    typedef nativePointerWrapper_t<textInput_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    string_t value() const;
    void setValue(const string_t& v);
    void changeValue(const string_t& v);

    eventCallback_t<void, textInput_t&> OnTextChange;
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_TEXT_INPUT_H
