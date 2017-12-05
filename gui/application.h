#ifndef JJ_GUI_APPLICATION_H
#define JJ_GUI_APPLICATION_H

#include "jj/defines.h"
#include "jj/string.h"

#if (JJ_USE_GUI)

namespace jj
{
namespace gui
{

class application_t
{
    application_t(const application_t&) = delete;
    application_t(application_t&&) = delete;
    application_t& operator=(const application_t&) = delete;
public:
    int argc;
    char_t** argv;

    application_t();
	~application_t();
	virtual bool onInit() = 0;
    virtual void onExit() {}
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_APPLICATION_H
