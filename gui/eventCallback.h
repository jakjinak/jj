#ifndef JJ_GUI_EVENT_CALLBACK_H
#define JJ_GUI_EVENT_CALLBACK_H

#include "jj/defines.h"

#if (JJ_USE_GUI)

#include "jj/functionBag.h"

namespace jj
{
namespace gui
{

namespace AUX
{
template<typename T>
void setup_first_last_callbacks(T& ecb, std::function<void()> first, std::function<void()> last)
{
    ecb.FirstAdded = first;
    ecb.LastRemoved = last;
}
} // namespace AUX

template<typename R, typename ... Ps>
class eventCallback_t : public functionBag_t<R, Ps...>
{
    typedef functionBag_t<R, Ps...> parent_t;
    
    template<typename T>
    friend void AUX::setup_first_last_callbacks(T& ecb, std::function<void()> first, std::function<void()> last);
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_EVENT_CALLBACK_H
