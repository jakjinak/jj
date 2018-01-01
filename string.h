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

namespace jj
{
namespace strcvt
{
inline std::string to_string(const char* str) { return str; }
inline std::string to_string(const std::string& str) { return str; }
std::string to_string(const wchar_t* str);
std::string to_string(const std::wstring& str);
std::wstring to_wstring(const char* str);
std::wstring to_wstring(const std::string& str);
inline std::wstring to_wstring(const wchar_t* str) { return str; }
inline std::wstring to_wstring(const std::wstring& str) { return str; }

#if defined(_WINDOWS) || defined(_WIN32)
inline string_t to_string_t(const char* str) { return to_wstring(str); }
inline string_t to_string_t(const std::string& str) { return to_wstring(str); }
inline string_t to_string_t(const wchar_t* str) { return str; }
inline string_t to_string_t(const std::wstring& str) { return str; }
#else
inline string_t to_string_t(const char* str) { return str; }
inline string_t to_string_t(const std::string& str) { return str; }
inline string_t to_string_t(const wchar_t* str) { return to_string(str); }
inline string_t to_string_t(const std::wstring& str) { return to_string(str); }
#endif // defined(_WINDOWS) || defined(_WIN32)
} // namespace strcvt

namespace str
{
bool starts_with(const char* str, const char* with);
inline bool starts_with(const std::string& str, const char* with) { return starts_with(str.c_str(), with); }
inline bool starts_with(const char* str, const std::string& with) { return starts_with(str, with.c_str()); }
inline bool starts_with(const std::string& str, const std::string& with) { return starts_with(str.c_str(), with.c_str()); }
bool starts_with(const wchar_t* str, const wchar_t* with);
inline bool starts_with(const std::wstring& str, const wchar_t* with) { return starts_with(str.c_str(), with); }
inline bool starts_with(const wchar_t* str, const std::wstring& with) { return starts_with(str, with.c_str()); }
inline bool starts_with(const std::wstring& str, const std::wstring& with) { return starts_with(str.c_str(), with.c_str()); }

const char* find(const char* str, char what, size_t pos = 0);
const char* find(const char* str, const char* what, size_t pos = 0);
const char* find(const char* str, const std::string& what, size_t pos = 0);
size_t find(const std::string& str, char what, size_t pos = 0);
size_t find(const std::string& str, const char* what, size_t pos = 0);
size_t find(const std::string& str, const std::string& what, size_t pos = 0);
const wchar_t* find(const wchar_t* str, wchar_t what, size_t pos = 0);
const wchar_t* find(const wchar_t* str, const wchar_t* what, size_t pos = 0);
const wchar_t* find(const wchar_t* str, const std::wstring& what, size_t pos = 0);
size_t find(const std::wstring& str, wchar_t what, size_t pos = 0);
size_t find(const std::wstring& str, const wchar_t* what, size_t pos = 0);
size_t find(const std::wstring& str, const std::wstring& what, size_t pos = 0);
} // namespace str

} // namespace jj

#endif // JJ_STRING_H
