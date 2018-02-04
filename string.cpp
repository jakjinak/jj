#include "jj/string.h"
#include "jj/exception.h"
#include <cstring>
#include <cctype>
#include <cwctype>

#if defined(_WINDOWS) || defined(_WIN32) || ( __GNUC__ > 5 ) || (__GNUC__ == 5 && (__GNUC_MINOR__ > 1 ) )
// this is only supported on windows (vs2017) or with g++ newer than
#include <locale>
#include <codecvt>

namespace jj
{
namespace strcvt
{

std::string to_string(const wchar_t* str)
{
    if (str == nullptr)
        return "";
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
    if (str == nullptr)
        return L"";
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::wstring to_wstring(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

} // namespace strcvt
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

} // namespace jj

#endif // defined(_WINDOWS)...

namespace jj
{

namespace str
{

bool isspace(char ch)
{
    return std::isspace(ch);
}

bool isspace(wchar_t ch)
{
    return std::iswspace(ch);
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
    if (pos>0)
        return nullptr;
    return std::strchr(str, what);
}

const char* find(const char* str, const char* what, size_t pos)
{
    if (what == nullptr)
        return str;
    if (str == nullptr)
        return nullptr;
    while (pos>0 && *str!=0)
    {
        --pos;
        ++str;
    }
    if (pos>0)
        return nullptr;
    return std::strstr(str, what);
}

const wchar_t* find(const wchar_t* str, wchar_t what, size_t pos)
{
    if (str == nullptr)
        return nullptr;
    while (pos>0 && *str!=0)
    {
        --pos;
        ++str;
    }
    if (pos>0)
        return nullptr;
    return std::wcschr(str, what);
}

const wchar_t* find(const wchar_t* str, const wchar_t* what, size_t pos)
{
    if (what == nullptr)
        return str;
    if (str == nullptr)
        return nullptr;
    while (pos>0 && *str!=0)
    {
        --pos;
        ++str;
    }
    if (pos>0)
        return nullptr;
    return std::wcsstr(str, what);
}

} // namespace str

} // namespace jj
