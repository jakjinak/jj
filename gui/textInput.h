#ifndef JJ_GUI_TEXT_INPUT_H
#define JJ_GUI_TEXT_INPUT_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

#include "jj/gui/eventCallback.h"

namespace jj
{
namespace gui
{

struct textInputStyle_t
{
    long Style;

    textInputStyle_t() : Style(0) {}
};
typedef creationOptions_t<opt::text, opt::position, opt::size, textInputStyle_t> textInputOptions_t;

class textInput_t : public control_t, public nativePointerWrapper_t<textInput_t>
{
    typedef control_t parent_t;

public:
    textInput_t(contentHolder_t& owner, textInputOptions_t setup);

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
