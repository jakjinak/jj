#ifndef JJ_DEFINES_GEN_H
#define JJ_DEFINES_GEN_H

#if defined(_WIN32)
#define JJ_OS_WINDOWS
#define JJ_USE_WSTRING
#else
#define JJ_OS_UNIX_LINUX
#define JJ_OS_LINUX
#define JJ_USE_STRING
#endif

#if defined(_WIN32) || ( __GNUC__ > 5 ) || (__GNUC__ == 5 && (__GNUC_MINOR__ > 1 ) )
// this is only supported on windows (vs2017) or with g++ newer than 5.1
#define JJ_USE_CODECVT
#else
#endif

#endif // JJ_DEFINES_GEN_H
