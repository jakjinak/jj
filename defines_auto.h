#ifndef JJ_DEFINES_AUTO_H
#define JJ_DEFINES_AUTO_H

// the below definitions based on:
// https://sourceforge.net/p/predef/wiki/Compilers/
// https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
// https://msdn.microsoft.com/en-us/library/b0084kay.aspx

/*! JJ_COMPILER_GCC
Defined if building with GCC/G++. */
/*! JJ_COMPILER_MSVC
Defined if building with MS Visual Studio. */
/*! JJ_COMPILER_VERSION_MAJOR
Expands to compilers major version. */
/*! JJ_COMPILER_VERSION_MINOR
Expands to compilers minor version. */
/*! JJ_COMPILER_VERSION_BUILD
Expands to compilers build revision. */

/*! JJ_FUNC 
Expands to the compilers symbol for "current function decorated". */

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

/*! JJ_OS_WINDOWS
Defined if building on native Windows. */
/*! JJ_OS_CYGWIN
Defined if building under Cygwin. */
/*! JJ_OS_LINUX
Defined if building under Linux. */
#if defined(_WIN32)
#define JJ_OS_WINDOWS
#define JJ_USE_WSTRING
#elif defined(__CYGWIN__)
#define JJ_OS_CYGWIN
#define JJ_USE_STRING
#else
#define JJ_OS_LINUX
#define JJ_USE_STRING
#endif

/*! JJ_USE_CODECVT
Defined if string conversion functions are available (narrow<->wide strings). */
#if defined(JJ_COMPILER_MSVC) || ( __GNUC__ > 5 ) || (__GNUC__ == 5 && (__GNUC_MINOR__ > 1 ) )
// this is only supported on windows (vs2017) or with g++ newer than 5.1
#define JJ_USE_CODECVT
#else
#endif

#endif // JJ_DEFINES_AUTO_H
