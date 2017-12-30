#ifndef JJ_GUI_BUTTON_H
#define JJ_GUI_BUTTON_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

#include "jj/gui/eventCallback.h"

namespace jj
{
namespace gui
{

class button_t : public nativePointerWrapper_t<button_t>, public control_t
{
public:
    enum flags_t
    {
        EXACT_FIT,
        NO_BORDER,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef options_t<opt::text, opt::position, opt::size, flags1_t, opt::e<align_t>, opt::e<alignv_t>> options_t;
    static options_t options() { return options_t() << align_t::CENTER << alignv_t::MIDDLE; }

private:
    typedef control_t parent_t;

public:
    button_t(contentHolder_t& owner, options_t setup);

    typedef nativePointerWrapper_t<button_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    string_t text() const;
    void text(const string_t& v);

    eventCallback_t<void, button_t&> OnClick;
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_BUTTON_H
