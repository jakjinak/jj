#ifndef JJ_DEFINES_H
#define JJ_DEFINES_H

#include "defines_auto.h"

/*! JJ_CUSTOM_DEFINES_FILE
Define this to a include file name that contains your defines to be included as part of this header file. */
#ifdef JJ_CUSTOM_DEFINES_FILE
#undef JJ_CUSTOM_INCLUDER_HELPER
#define JJ_CUSTOM_INCLUDER_HELPER(x) #x
#include JJ_CUSTOM_INCLUDER_HELPER(JJ_CUSTOM_DEFINES_FILE)
#undef JJ_CUSTOM_INCLUDER_HELPER
#endif // JJ_CUSTOM_DEFINES_FILE

/*! JJ_LOGLIMIT_HARD
Define this to one of the log levels to override the default "hard" log level. */
#ifndef JJ_LOGLIMIT_HARD
#define JJ_LOGLIMIT_HARD JJ_LOGLEVEL_SCOPE
#endif
/*! JJ_LOG_STREAM_PROVIDER
Define this to an expression providing a "stream" to convert streamed values into a string. */
#ifndef JJ_LOG_STREAM_PROVIDER
#define JJ_LOG_STREAM_PROVIDER jj::log::simpleStreamProvider_t()
#endif

#define JJ_DEFINED_VALUE_GUI_NONE 0
#define JJ_DEFINED_VALUE_GUI_WX 1

#ifndef JJ_USE_GUI
#define JJ_USE_GUI JJ_DEFINED_VALUE_GUI_WX
#endif // JJ_USE_GUI

#endif // JJ_DEFINES_H
