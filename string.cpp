#include "jj/string.h"
#include "jj/exception.h"
#include <cstring>
#include <cctype>
#include <cwctype>

#if defined(_WINDOWS) || defined(_WIN32)
#include <string.h> // _strnicmp, _wcsnicmp
#else
#include <strings.h> // strncasecmp, wcsncasecmp
#endif

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
        return jj::str::EmptyString;
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
        return jj::str::EmptyWString;
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

const std::string EmptyString;
const std::wstring EmptyWString;
#if defined(_WINDOWS) || defined(_WIN32)
const string_t& Empty = EmptyWString;
#else
const string_t& Empty = EmptyString;
#endif

namespace // <anonymous>
{
template<typename CH> struct CHtrait;
template<> struct CHtrait<char>
{
    static inline const char* empty() { return ""; }
};
template<> struct CHtrait<wchar_t>
{
    static inline const wchar_t* empty() { return L""; }
};
} // namespace <anonymous>

template<typename CH>
inline void strprecheck(const CH*& a, const CH*& b, size_t& pos)
{
    if (a == nullptr)
        a = CHtrait<CH>::empty();
    if (b == nullptr)
        b = CHtrait<CH>::empty();
    while (pos > 0 && *a != 0 && *b != 0)
    {
        --pos;
        ++a;
        ++b;
    }
    while (pos > 0 && *a != 0)
    {
        --pos;
        ++a;
    }
    while (pos > 0 && *b != 0)
    {
        --pos;
        ++b;
    }
}

static inline int sgn(int n)
{
    if (n < 0)
        return -1;
    if (n > 0)
        return 1;
    return 0;
}

int cmp(const char* a, const char* b, size_t pos, size_t len)
{
    strprecheck(a, b, pos);
    return sgn(strncmp(a, b, len));
}
int cmp(const wchar_t* a, const wchar_t* b, size_t pos, size_t len)
{
    strprecheck(a, b, pos);
    return sgn(wcsncmp(a, b, len));
}

bool isspace(char ch)
{
    return std::isspace(ch);
}

bool isspace(wchar_t ch)
{
    return std::iswspace(ch);
}

int cmpi(char a, char b)
{
    a = std::tolower(a);
    b = std::tolower(b);

    if (a < b)
        return -1;
    if (b < a)
        return 1;
    return 0;
}
int cmpi(const char* a, const char* b, size_t pos, size_t len)
{
    strprecheck(a, b, pos);
#if defined(_WINDOWS) || defined(_WIN32)
    if (len == std::string::npos)
        return sgn(_strcmpi(a, b));
    return sgn(_strnicmp(a, b, len));
#else
    return sgn(strncasecmp(a, b, len));
#endif
}
int cmpi(wchar_t a, wchar_t b)
{
    a = std::towlower(a);
    b = std::towlower(b);

    if (a < b)
        return -1;
    if (b < a)
        return 1;
    return 0;
}
int cmpi(const wchar_t* a, const wchar_t* b, size_t pos, size_t len)
{
    strprecheck(a, b, pos);
#if defined(_WINDOWS) || defined(_WIN32)
    if (len == std::wstring::npos)
        return sgn(_wcsicmp(a, b));
    return sgn(_wcsnicmp(a, b, len));
#else
    return sgn(wcsncasecmp(a, b, len));
#endif
}

bool lessi(char a, char b)
{
    return std::tolower(a) < std::tolower(b);
}

bool lessi(const char* a, const char* b)
{
    if (b == nullptr || *b == 0)
        return false; // b is prefix of a or both empty
    if (a == nullptr)
        return true; // empty a is always prefix of b
    for (; *a != 0 && *b != 0 && std::tolower(*a) == std::tolower(*b); ++a, ++b)
        ;
    if (*b == 0)
        return false; // b is prefix of a, ie. b is less than a
    if (*a == 0)
        return true; // a is prefix of b, ie. a is less than b
    return std::tolower(*a) < std::tolower(*b); // found non-matching chars
}

bool lessi(const std::string& a, const std::string& b)
{
    std::string::size_type ai = 0, bi = 0, al = a.length(), bl = b.length();
    for (; ai < al && bi < bl && std::tolower(a[ai]) == std::tolower(b[bi]); ++ai, ++bi)
        ;
    if (bi == bl)
        return false; // b is prefix of a, ie. b is less than a
    if (ai == al)
        return true; // a is prefix of b, ie. a is less than b
    return std::tolower(a[ai]) < std::tolower(b[bi]); // found non-matching chars
}

bool lessi(wchar_t a, wchar_t b)
{
    return std::towlower(a) < std::towlower(b);
}

bool lessi(const wchar_t* a, const wchar_t* b)
{
    if (b == nullptr || *b == 0)
        return false; // b is prefix of a or both empty
    if (a == nullptr)
        return true; // empty a is always prefix of b
    for (; *a != 0 && *b != 0 && std::towlower(*a) == std::towlower(*b); ++a, ++b)
        ;
    if (*b == 0)
        return false; // b is prefix of a, ie. b is less than a
    if (*a == 0)
        return true; // a is prefix of b, ie. a is less than b
    return std::towlower(*a) < std::towlower(*b); // found non-matching chars
}

bool lessi(const std::wstring& a, const std::wstring& b)
{
    std::wstring::size_type ai = 0, bi = 0, al = a.length(), bl = b.length();
    for (; ai < al && bi < bl && std::towlower(a[ai]) == std::towlower(b[bi]); ++ai, ++bi)
        ;
    if (bi == bl)
        return false; // b is prefix of a, ie. b is less than a
    if (ai == al)
        return true; // a is prefix of b, ie. a is less than b
    return std::towlower(a[ai]) < std::towlower(b[bi]); // found non-matching chars
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
