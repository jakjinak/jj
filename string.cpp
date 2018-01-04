#include "jj/string.h"
#include "jj/exception.h"
#include <cstring>

#if defined(_WINDOWS) || defined(_WIN32) || ( __GNUC__ > 5 ) || (__GNUC__ == 5 && (__GNUC_MINOR__ > 1 ) )
// this is only supported on windows (vs2017) or with g++ newer than
#include <locale>
#include <codecvt>

namespace jj
{
namespace stringcvt
{

std::string to_string(const wchar_t* str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(str);
}

std::string to_string(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(str);
}

std::wstring to_wstring(const char* str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::wstring to_wstring(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

} // namespace stringcvt
} // namespace jj

#else // defined(_WINDOWS)...

namespace jj
{
namespace strcvt
{

std::string to_string(const wchar_t* str)
{
    throw not_implemented_t();
}

std::string to_string(const std::wstring& str)
{
    throw not_implemented_t();
}

std::wstring to_wstring(const char* str)
{
    throw not_implemented_t();
}

std::wstring to_wstring(const std::string& str)
{
    throw not_implemented_t();
}

} // namespace strcvt

namespace str
{

bool starts_with(const char* str, const char* with)
{
    // TODO do better
    return std::strstr(str, with) == str;
}

const char* find(const char* str, char what, size_t pos)
{
    if (str == nullptr)
        return nullptr;
    while (pos>0 && *str!=0)
    {
        --pos;
        ++str;
    }
    return std::strchr(str, what);
}

} // namespace str

} // namespace jj

#endif // defined(_WINDOWS)...
