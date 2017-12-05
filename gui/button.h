#ifndef JJ_GUI_BUTTON_H
#define JJ_GUI_BUTTON_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

#include "jj/gui/eventCallback.h"

namespace jj
{
namespace gui
{

struct buttonStyle_t
{
    long Style;

    buttonStyle_t() : Style(0) {}
};
typedef creationOptions_t<opt::title, opt::position, opt::size, buttonStyle_t> buttonOptions_t;

class button_t : public nativePointerWrapper_t<button_t>, public control_t
{
    typedef nativePointerWrapper_t<button_t> native_t;
    typedef control_t parent_t;

public:
    button_t(contentHolder_t& owner, buttonOptions_t setup);

    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    eventCallback_t<void, button_t&> OnClick;
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_BUTTON_H
