#include "jj/test/test.h"
#include "jj/string.h"

JJ_TEST_CLASS(stringTests_t)
static const char* long_string;

JJ_TEST_CASE_VARIANTS(charp2string,(const char* in, const char* out), ("", ""), ("a", "a"), (long_string,long_string),(nullptr,""))
{
    JJ_TEST(jj::strcvt::to_string(in) == out);
}

JJ_TEST_CASE_VARIANTS(string2string,(const std::string& in, const char* out), ("", ""), ("a", "a"), (long_string,long_string))
{
    JJ_TEST(jj::strcvt::to_string(in) == out);
}

JJ_TEST_CLASS_END(stringTests_t, charp2string, string2string)
const char* stringTests_t::long_string = "AbC023as;lk/./.as2190";



JJ_TEST_CLASS(wstringTests_t)
static const wchar_t* long_wstring;

JJ_TEST_CASE_VARIANTS(wcharp2wstring,(const wchar_t* in, const wchar_t* out), (L"", L""), (L"a", L"a"), (long_wstring,long_wstring),(nullptr,L""))
{
    JJ_TEST(jj::strcvt::to_wstring(in) == out);
}

JJ_TEST_CASE_VARIANTS(wstring2wstring,(const std::wstring& in, const wchar_t* out), (L"", L""), (L"a", L"a"), (long_wstring,long_wstring))
{
    JJ_TEST(jj::strcvt::to_wstring(in) == out);
}

JJ_TEST_CLASS_END(wstringTests_t, wcharp2wstring, wstring2wstring)
const wchar_t* wstringTests_t::long_wstring = L"AbC023as;lk/./.as2190";


#if defined(_WINDOWS) || defined(_WIN32) || ( __GNUC__ > 5 ) || (__GNUC__ == 5 && (__GNUC_MINOR__ > 1 ) )
JJ_TEST_CLASS(xstringTests_t)
static const char* long_string;
static const wchar_t* long_wstring;

JJ_TEST_CASE_VARIANTS(wcharp2string,(const wchar_t* in, const char* out), (L"", ""), (L"a", "a"), (long_wstring,long_string),(nullptr,""))
{
    JJ_TEST(jj::strcvt::to_string(in) == out);
}

JJ_TEST_CASE_VARIANTS(wstring2string,(const std::wstring& in, const char* out), (L"", ""), (L"a", "a"), (long_wstring,long_string))
{
    JJ_TEST(jj::strcvt::to_string(in) == out);
}

JJ_TEST_CASE_VARIANTS(charp2wstring,(const char* in, const wchar_t* out), ("", L""), ("a", L"a"), (long_string,long_wstring),(nullptr,L""))
{
    JJ_TEST(jj::strcvt::to_wstring(in) == out);
}

JJ_TEST_CASE_VARIANTS(string2wstring,(const std::string& in, const wchar_t* out), ("", L""), ("a", L"a"), (long_string,long_wstring))
{
    JJ_TEST(jj::strcvt::to_wstring(in) == out);
}

JJ_TEST_CLASS_END(xStringTests_t, wcharp2string, wstring2string, charp2wstring, string2wstring)
const char* xstringTests_t::long_string = "AbC023as;lk/./.as2190";
const wchar_t* xstringTests_t::long_wstring = L"AbC023as;lk/./.as2190";
#endif // defined(...
