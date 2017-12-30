#ifndef JJ_GUI_TEXT_LABEL_H
#define JJ_GUI_TEXT_LABEL_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

namespace jj
{
namespace gui
{

class textLabel_t : public nativePointerWrapper_t<textLabel_t>, public control_t
{
public:
    enum flags_t
    {
        NO_RESIZE,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef options_t<opt::text, opt::position, opt::size, flags1_t, opt::e<align_t>> options_t;
    static options_t options() { return options_t(); }

private:
    typedef control_t parent_t;

public:
    textLabel_t(contentHolder_t& owner, options_t setup);

    typedef nativePointerWrapper_t<textLabel_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    string_t text() const;
    void text(const string_t& v);
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_TEXT_LABEL_H
