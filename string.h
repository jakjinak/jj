#ifndef JJ_STRING_H
#define JJ_STRING_H

#include <string>

#if defined(_WINDOWS) || defined(_WIN32)
namespace jj
{
typedef wchar_t char_t;
typedef std::wstring string_t;
} // namespace jj
#define jjT(x) L##x
#else
namespace jj
{
typedef char char_t;
typedef std::string string_t;
} // namespace jj
#define jjT(x) x
#endif // defined(_WINDOWS) || defined(_WIN32)

#endif // JJ_STRING_H
