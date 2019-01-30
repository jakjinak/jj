#ifndef JJ_GUI_APPLICATION_H
#define JJ_GUI_APPLICATION_H

#include "jj/defines.h"
#include "jj/string.h"

#if (JJ_USE_GUI)

namespace jj
{
namespace gui
{

/*! An instance of this class represents the running graphics program.

It implements internally the main (or WinMain or whatever required) so that function shall not be defined by user.
Instead the user shall implement the on_init where he can process the argc/argv. There must be exactly one global
instance of this class. */
class application_t
{
    application_t(const application_t&) = delete;
    application_t(application_t&&) = delete;
    application_t& operator=(const application_t&) = delete;
public:
    int argc; //!< count of arguments (as passed to main())
    char_t** argv; //!< program arguments (as passed to main())

    /*! Ctor */
    application_t();
    /*! Dtor */
	~application_t();

    /*! Invoked at the beginning of main() (after necessary initialization).
    
    User is assumed to process the program parameters here and create the main window. */
	virtual bool on_init() = 0;
    /*! Invoked at the end of main() after all windows destroyed (ie. the main loop ends).
    Tweak the returnCode in the function if you wish.
    Not invoked if on_init() failed. */
    virtual void on_exit(int& returnCode) {}
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_APPLICATION_H
