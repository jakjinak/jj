#ifndef JJ_DEFINES_AUTO_H
#define JJ_DEFINES_AUTO_H

// the below definitions based on:
// https://sourceforge.net/p/predef/wiki/Compilers/
// https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
// https://msdn.microsoft.com/en-us/library/b0084kay.aspx

#if defined(__GNUC__)
#define JJ_COMPILER_GCC
#define JJ_COMPILER_VERSION_MAJOR __GNUC__
#define JJ_COMPILER_VERSION_MINOR __GNUC_MINOR__
#define JJ_COMPILER_VERSION_BUILD __GNUC_PATCHLEVEL__

#define JJ_FUNC __PRETTY_FUNCTION__
#elif defined (_MSC_VER)
#define JJ_COMPILER_MSVC
#define JJ_COMPILER_VERSION_MAJOR (_MSC_VER/100)
#define JJ_COMPILER_VERSION_MINOR (_MSC_VER%100)
#define JJ_COMPILER_VERSION_BUILD (_MSC_FULL_VER%10000)
// BUILD works only in VS2005(8.0) and newer

#define JJ_FUNC __FUNCSIG__
#endif

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

#endif // JJ_DEFINES_AUTO_H