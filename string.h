#ifndef JJ_STRING_H
#define JJ_STRING_H

#include "defines.h"
#include <string>
#include <cstring> // std::strcmp for gcc

#if defined(JJ_USE_WSTRING)
namespace jj
{
/*! Uniquely defines a character type in a platform independent fashion. */
typedef wchar_t char_t;
/*! Uniquely defines a string type in a platform independent fashion. */
typedef std::wstring string_t;
} // namespace jj
#define jjT(x) L##x
#else
namespace jj
{
/*! Uniquely defines a character type in a platform independent fashion. */
typedef char char_t;
/*! Uniquely defines a string type in a platform independent fashion. */
typedef std::string string_t;
} // namespace jj
#define jjT(x) x
#endif // defined(JJ_USE_WSTRING)

namespace jj
{
namespace str
{

extern const std::string EmptyString; //!< an empty "narrow" string instance
extern const std::wstring EmptyWString; //!< an empty "wide" string instance
extern const string_t& Empty; //!< reference to system native empty string instance

} // namespace str
} // namespace jj

// TODO locale enabled variants of all those below

namespace jj
{
namespace strcvt
{
/*! Returns a std::string object constructed from input parameter, does conversions when necessary. */
inline std::string to_string(const char* str) { if (str==nullptr) return jj::str::EmptyString; return str; }
/*! Returns a std::string object constructed from input parameter, does conversions when necessary. */
inline std::string to_string(const std::string& str) { return str; }
/*! Returns a std::string object constructed from input parameter, does conversions when necessary. */
std::string to_string(const wchar_t* str);
/*! Returns a std::string object constructed from input parameter, does conversions when necessary. */
std::string to_string(const std::wstring& str);
/*! Returns a std::wstring object constructed from input parameter, does conversions when necessary. */
std::wstring to_wstring(const char* str);
/*! Returns a std::wstring object constructed from input parameter, does conversions when necessary. */
std::wstring to_wstring(const std::string& str);
/*! Returns a std::wstring object constructed from input parameter, does conversions when necessary. */
inline std::wstring to_wstring(const wchar_t* str) { if (str==nullptr) return jj::str::EmptyWString; return str; }
/*! Returns a std::wstring object constructed from input parameter, does conversions when necessary. */
inline std::wstring to_wstring(const std::wstring& str) { return str; }

#if defined(JJ_USE_WSTRING)
/*! Returns platform specific string object constructed from input parameter, does conversions when necessary. */
inline string_t to_string_t(const char* str) { return to_wstring(str); }
/*! Returns platform specific string object constructed from input parameter, does conversions when necessary. */
inline string_t to_string_t(const std::string& str) { return to_wstring(str); }
/*! Returns platform specific string object constructed from input parameter, does conversions when necessary. */
inline string_t to_string_t(const wchar_t* str) { return to_wstring(str); }
/*! Returns platform specific string object constructed from input parameter, does conversions when necessary. */
inline string_t to_string_t(const std::wstring& str) { return str; }
#else
/*! Returns platform specific string object constructed from input parameter, does conversions when necessary. */
inline string_t to_string_t(const char* str) { return to_string(str); }
/*! Returns platform specific string object constructed from input parameter, does conversions when necessary. */
inline string_t to_string_t(const std::string& str) { return str; }
/*! Returns platform specific string object constructed from input parameter, does conversions when necessary. */
inline string_t to_string_t(const wchar_t* str) { return to_string(str); }
/*! Returns platform specific string object constructed from input parameter, does conversions when necessary. */
inline string_t to_string_t(const std::wstring& str) { return to_string(str); }
#endif // defined(JJ_USE_WSTRING)
} // namespace strcvt

namespace str
{

/*! Returns whether given character can be considered a whitespace. */
bool isspace(char ch);
/*! Returns whether given character can be considered a whitespace. */
bool isspace(wchar_t ch);
/*! Returns whether given character is an alphabetic character. */
bool isalpha(char ch);
/*! Returns whether given character is an alphabetic character. */
bool isalpha(wchar_t ch);
/*! Returns whether given character is an alphabetic or numeric character. */
bool isalnum(char ch);
/*! Returns whether given character is an alphabetic or numeric character. */
bool isalnum(wchar_t ch);
/*! Returns whether given character is a numeric character. */
bool isdigit(char ch);
/*! Returns whether given character is a numeric character. */
bool isdigit(wchar_t ch);
/*! Returns whether given character is a hexadecimal character. */
bool isxdigit(char ch);
/*! Returns whether given character is a hexadecimal character. */
bool isxdigit(wchar_t ch);
/*! Returns whether given character is a control character. */
bool iscntrl(char ch);
/*! Returns whether given character is a control character. */
bool iscntrl(wchar_t ch);

// TODO inline these

/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(char a, char b)
{
    if (a < b)
        return -1;
    if (b < a)
        return 1;
    return 0;
}
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(char a, char b, size_t pos, size_t len = 1)
{
    if (pos > 0 || len == 0)
        return 0;
    return cmp(a, b);
}
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
int cmp(const char* a, const char* b, size_t pos = 0, size_t len = std::string::npos);
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(const std::string& a, const char* b, size_t pos = 0, size_t len = std::string::npos) { return cmp(a.c_str(), b, pos, len); }
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(const char* a, const std::string& b, size_t pos = 0, size_t len = std::string::npos) { return cmp(a, b.c_str(), pos, len); }
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(const std::string& a, const std::string& b, size_t pos = 0, size_t len = std::string::npos) { return cmp(a.c_str(), b.c_str(), pos, len); }
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(wchar_t a, wchar_t b)
{
    if (a < b)
        return -1;
    if (b < a)
        return 1;
    return 0;
}
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(wchar_t a, wchar_t b, size_t pos, size_t len = 1)
{
    if (pos > 0 || len == 0)
        return 0;
    return cmp(a, b);
}
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
int cmp(const wchar_t* a, const wchar_t* b, size_t pos = 0, size_t len = std::wstring::npos);
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(const std::wstring& a, const wchar_t* b, size_t pos = 0, size_t len = std::wstring::npos) { return cmp(a.c_str(), b, pos, len); }
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(const wchar_t* a, const std::wstring& b, size_t pos = 0, size_t len = std::wstring::npos) { return cmp(a, b.c_str(), pos, len); }
/*! Returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmp(const std::wstring& a, const std::wstring& b, size_t pos = 0, size_t len = std::wstring::npos) { return cmp(a.c_str(), b.c_str(), pos, len); }

/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
int cmpi(char a, char b);
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmpi(char a, char b, size_t pos, size_t len = 1)
{
    if (pos > 0 || len == 0)
        return 0;
    return cmpi(a, b);
}
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
int cmpi(const char* a, const char* b, size_t pos = 0, size_t len = std::string::npos);
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmpi(const std::string& a, const char* b, size_t pos = 0, size_t len = std::string::npos) { return cmpi(a.c_str(), b, pos, len); }
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmpi(const char* a, const std::string& b, size_t pos = 0, size_t len = std::string::npos) { return cmpi(a, b.c_str(), pos, len); }
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmpi(const std::string& a, const std::string& b, size_t pos = 0, size_t len = std::string::npos) { return cmpi(a.c_str(), b.c_str(), pos, len); }
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
int cmpi(wchar_t a, wchar_t b);
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmpi(wchar_t a, wchar_t b, size_t pos, size_t len = 1)
{
    if (pos > 0 || len == 0)
        return 0;
    return cmpi(a, b);
}
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
int cmpi(const wchar_t* a, const wchar_t* b, size_t pos = 0, size_t len = std::wstring::npos);
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmpi(const std::wstring& a, const wchar_t* b, size_t pos = 0, size_t len = std::wstring::npos) { return cmpi(a.c_str(), b, pos, len); }
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmpi(const wchar_t* a, const std::wstring& b, size_t pos = 0, size_t len = std::wstring::npos) { return cmpi(a, b.c_str(), pos, len); }
/*! Ignoring character case, this returns -1 if a is lexicographically lower than b, 0 if they are equal or 1. */
inline int cmpi(const std::wstring& a, const std::wstring& b, size_t pos = 0, size_t len = std::wstring::npos) { return cmpi(a.c_str(), b.c_str(), pos, len); }

//=====================================

/*! Returns whether a and b do have same value. */
inline bool equal(char a, char b) { return a == b; }
/*! Returns whether a and b do have same value. */
inline bool equal(const char* a, const char* b) { return strcmp(a == nullptr ? "" : a, b == nullptr ? "" : b) == 0; }
/*! Returns whether a and b do have same value. */
inline bool equal(const std::string& a, const char* b) { return equal(a.c_str(), b); }
/*! Returns whether a and b do have same value. */
inline bool equal(const char* a, const std::string& b) { return equal(a, b.c_str()); }
/*! Returns whether a and b do have same value. */
inline bool equal(const std::string& a, const std::string& b) { return a == b; }
/*! Returns whether a and b do have same value. */
inline bool equal(wchar_t a, wchar_t b) { return a == b; }
/*! Returns whether a and b do have same value. */
inline bool equal(const wchar_t* a, const wchar_t* b) { return wcscmp(a == nullptr ? L"" : a, b == nullptr ? L"" : b) == 0; }
/*! Returns whether a and b do have same value. */
inline bool equal(const std::wstring& a, const wchar_t* b) { return equal(a.c_str(), b); }
/*! Returns whether a and b do have same value. */
inline bool equal(const wchar_t* a, const std::wstring& b) { return equal(a, b.c_str()); }
/*! Returns whether a and b do have same value. */
inline bool equal(const std::wstring& a, const std::wstring& b) { return a == b; }
/*! Returns whether a and b do have same value when ignoring character case. */
bool equali(char a, char b);
/*! Returns whether a and b do have same value when ignoring character case. */
bool equali(const char* a, const char* b);
/*! Returns whether a and b do have same value when ignoring character case. */
inline bool equali(const std::string& a, const char* b) { return equali(a.c_str(), b); }
/*! Returns whether a and b do have same value when ignoring character case. */
inline bool equali(const char* a, const std::string& b) { return equali(a, b.c_str()); }
/*! Returns whether a and b do have same value when ignoring character case. */
inline bool equali(const std::string& a, const std::string& b) { return equali(a.c_str(), b.c_str()); }
/*! Returns whether a and b do have same value when ignoring character case. */
bool equali(wchar_t a, wchar_t b);
/*! Returns whether a and b do have same value when ignoring character case. */
bool equali(const wchar_t* a, const wchar_t* b);
/*! Returns whether a and b do have same value when ignoring character case. */
inline bool equali(const std::wstring& a, const wchar_t* b) { return equali(a.c_str(), b); }
/*! Returns whether a and b do have same value when ignoring character case. */
inline bool equali(const wchar_t* a, const std::wstring& b) { return equali(a, b.c_str()); }
/*! Returns whether a and b do have same value when ignoring character case. */
inline bool equali(const std::wstring& a, const std::wstring& b) { return equali(a.c_str(), b.c_str()); }

//=====================================

/*! Returns whether given a is lexicographically less than b. */
inline bool less(char a, char b) { return a < b; }
/*! Returns whether given a is lexicographically less than b. */
inline bool less(const char* a, const char* b)
{
    if (b == nullptr || *b == 0)
        return false;
    if (a == nullptr)
        return true;
    return std::strcmp(a, b) < 0;
}
/*! Returns whether given a is lexicographically less than b. */
inline bool less(const std::string& a, const char* b) { return less(a.c_str(), b); }
/*! Returns whether given a is lexicographically less than b. */
inline bool less(const char* a, const std::string& b) { return less(a, b.c_str()); }
/*! Returns whether given a is lexicographically less than b. */
inline bool less(const std::string& a, const std::string& b) { return a < b; }
/*! Returns whether given a is lexicographically less than b. */
inline bool less(wchar_t a, wchar_t b) { return a < b; }
/*! Returns whether given a is lexicographically less than b. */
inline bool less(const wchar_t* a, const wchar_t* b)
{
    if (b == nullptr || *b == 0)
        return false;
    if (a == nullptr)
        return true;
    return std::wcscmp(a, b) < 0;
}
/*! Returns whether given a is lexicographically less than b. */
inline bool less(const std::wstring& a, const wchar_t* b) { return less(a.c_str(), b); }
/*! Returns whether given a is lexicographically less than b. */
inline bool less(const wchar_t* a, const std::wstring& b) { return less(a, b.c_str()); }
/*! Returns whether given a is lexicographically less than b. */
inline bool less(const std::wstring& a, const std::wstring& b) { return a < b; }
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(char a, char b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(const char* a, const char* b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
inline bool lessi(const std::string& a, const char* b) { return lessi(a.c_str(), b); }
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
inline bool lessi(const char* a, const std::string& b) { return lessi(a, b.c_str()); }
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(const std::string& a, const std::string& b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(wchar_t a, wchar_t b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(const wchar_t* a, const wchar_t* b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
inline bool lessi(const std::wstring& a, const wchar_t* b) { return lessi(a.c_str(), b); }
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
inline bool lessi(const wchar_t* a, const std::wstring& b) { return lessi(a, b.c_str()); }
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(const std::wstring& a, const std::wstring& b);

//=====================================

/*! Returns whether given string str begins with given string with. The NUL characters are not compared. */
template<typename CH>
bool starts_with_T(const CH* str, const CH* with)
{
    if (with == nullptr)
        return true;
    if (str == nullptr)
        return false;
    for (; *str == *with; ++str, ++with)
    {
        if (*str == CH() || *with == CH())
            break;
    }
    return *with == CH() || *str == *with;
}
/*! Returns whether given string str begins with given string with. */
inline bool starts_with(const char* str, const char* with) { return starts_with_T<char>(str, with); }
/*! Returns whether given string str begins with given string with. */
inline bool starts_with(const std::string& str, const char* with) { return starts_with(str.c_str(), with); }
/*! Returns whether given string str begins with given string with. */
inline bool starts_with(const char* str, const std::string& with) { return starts_with(str, with.c_str()); }
/*! Returns whether given string str begins with given string with. */
inline bool starts_with(const std::string& str, const std::string& with) { return starts_with(str.c_str(), with.c_str()); }
/*! Returns whether given string str begins with given string with. */
inline bool starts_with(const wchar_t* str, const wchar_t* with) { return starts_with_T<wchar_t>(str, with); }
/*! Returns whether given string str begins with given string with. */
inline bool starts_with(const std::wstring& str, const wchar_t* with) { return starts_with(str.c_str(), with); }
/*! Returns whether given string str begins with given string with. */
inline bool starts_with(const wchar_t* str, const std::wstring& with) { return starts_with(str, with.c_str()); }
/*! Returns whether given string str begins with given string with. */
inline bool starts_with(const std::wstring& str, const std::wstring& with) { return starts_with(str.c_str(), with.c_str()); }

//=====================================

/*! Returns pointer to first occurrence of what within str since position pos or nullptr if not found. */
const char* find(const char* str, char what, size_t pos = 0);
/*! Returns pointer to first occurrence of what within str since position pos or nullptr if not found. */
const char* find(const char* str, const char* what, size_t pos = 0);
/*! Returns pointer to first occurrence of what within str since position pos or nullptr if not found. */
inline const char* find(const char* str, const std::string& what, size_t pos = 0) { return find(str, what.c_str(), pos); }
/*! Returns position of first occurrence of what within str since position pos or npos if not found. */
inline size_t find(const std::string& str, char what, size_t pos = 0) { return str.find(what, pos); }
/*! Returns position of first occurrence of what within str since position pos or npos if not found. */
inline size_t find(const std::string& str, const char* what, size_t pos = 0) { if (what==nullptr) return 0u; return str.find(what, pos); }
/*! Returns position of first occurrence of what within str since position pos or npos if not found. */
inline size_t find(const std::string& str, const std::string& what, size_t pos = 0) { return str.find(what, pos); }
/*! Returns pointer to first occurrence of what within str since position pos or nullptr if not found. */
const wchar_t* find(const wchar_t* str, wchar_t what, size_t pos = 0);
/*! Returns pointer to first occurrence of what within str since position pos or nullptr if not found. */
const wchar_t* find(const wchar_t* str, const wchar_t* what, size_t pos = 0);
/*! Returns pointer to first occurrence of what within str since position pos or nullptr if not found. */
inline const wchar_t* find(const wchar_t* str, const std::wstring& what, size_t pos = 0) { return find(str, what.c_str(), pos); }
/*! Returns position of first occurrence of what within str since position pos or npos if not found. */
inline size_t find(const std::wstring& str, wchar_t what, size_t pos = 0) { return str.find(what, pos); }
/*! Returns position of first occurrence of what within str since position pos or npos if not found. */
inline size_t find(const std::wstring& str, const wchar_t* what, size_t pos = 0) { if (what==nullptr) return 0u; return str.find(what, pos); }
/*! Returns position of first occurrence of what within str since position pos or npos if not found. */
inline size_t find(const std::wstring& str, const std::wstring& what, size_t pos = 0) { return str.find(what, pos); }

//=====================================

/*! A functor wrapping the case sensitive string comparison. */
template<typename CHAR>
struct lessPred
{
    template<typename T1, typename T2>
    bool operator()(T1& a, T2& b) const
    {
        return jj::str::less(a, b);
    }
};

/*! A functor wrapping the case insensitive string comparison. */
template<typename CHAR>
struct lessiPred
{
    template<typename T1, typename T2>
    bool operator()(T1& a, T2& b) const
    {
        return jj::str::lessi(a, b);
    }
};

} // namespace str

} // namespace jj

#endif // JJ_STRING_H
