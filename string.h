#ifndef JJ_STRING_H
#define JJ_STRING_H

#include <string>

#if defined(_WINDOWS) || defined(_WIN32)
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
#endif // defined(_WINDOWS) || defined(_WIN32)

namespace jj
{
namespace strcvt
{
/*! Returns a std::string object constructed from input parameter, does conversions when necessary. */
inline std::string to_string(const char* str) { if (str==nullptr) return ""; return str; }
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
inline std::wstring to_wstring(const wchar_t* str) { if (str==nullptr) return L""; return str; }
/*! Returns a std::wstring object constructed from input parameter, does conversions when necessary. */
inline std::wstring to_wstring(const std::wstring& str) { return str; }

#if defined(_WINDOWS) || defined(_WIN32)
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
#endif // defined(_WINDOWS) || defined(_WIN32)
} // namespace strcvt

namespace str
{

// TODO locale enabled variants

/*! Returns whether given character can be considered a whitespace. */
bool isspace(char ch);
/*! Returns whether given character can be considered a whitespace. */
bool isspace(wchar_t ch);
// TODO inline these

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
inline bool less(const std::wstring& a, const std::wstring& b) { return a < b; }
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(char a, char b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(const char* a, const char* b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(const std::string& a, const std::string& b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(wchar_t a, wchar_t b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(const wchar_t* a, const wchar_t* b);
/*! Returns whether given a is lexicographically less than b without taking care about character case. */
bool lessi(const std::wstring& a, const std::wstring& b);

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
} // namespace str

} // namespace jj

#endif // JJ_STRING_H
