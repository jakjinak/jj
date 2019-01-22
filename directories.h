#ifndef JJ_DIRECTORIES_H
#define JJ_DIRECTORIES_H

#include "jj/string.h"

namespace jj
{
namespace directories
{
/*! Returns the full path of the current directory.
Note: In a multithreaded environment this is not thread-safe. */
jj::string_t current();
/*! Returns the full path to the directory in which the binary of the program is running (if given
parameter is false) or full path including the binary name (otherwise). */
jj::string_t program(bool includeProgramName = false);
/*! Returns the full path to the home directory of the current user (ignoring setuid) - this will be
usually equivalent to "$HOME" on Linux, resp. "%USERPROFILE%\Documents" on Windows if the given
parameter is false, if given parameter is true it will be no difference on Linux, on Windows this
will lead to user's local appdata folder, usually "%USERPROFILE%\AppData\Local". */
jj::string_t personal(bool hidden = false);
} // namespace directories
} // namespace jj

#endif // JJ_DIRECTORIES_H
