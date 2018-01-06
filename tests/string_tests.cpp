#include "jj/test/test.h"
#include "jj/string.h"

JJ_TEST_CLASS(strcvtTests_t)
static const char* long_string;

JJ_TEST_CASE_VARIANTS(charp2string,(const char* in, const char* out), ("", ""), ("a", "a"), (long_string,long_string),(nullptr,""))
{
    JJ_TEST(jj::strcvt::to_string(in) == out);
}

JJ_TEST_CASE_VARIANTS(string2string,(const std::string& in, const char* out), ("", ""), ("a", "a"), (long_string,long_string))
{
    JJ_TEST(jj::strcvt::to_string(in) == out);
}

JJ_TEST_CLASS_END(strcvtTests_t, charp2string, string2string)
const char* strcvtTests_t::long_string = "AbC023as;lk/./.as2190";



JJ_TEST_CLASS(wstrcvtTests_t)
static const wchar_t* long_wstring;

JJ_TEST_CASE_VARIANTS(wcharp2wstring,(const wchar_t* in, const wchar_t* out), (L"", L""), (L"a", L"a"), (long_wstring,long_wstring),(nullptr,L""))
{
    JJ_TEST(jj::strcvt::to_wstring(in) == out);
}

JJ_TEST_CASE_VARIANTS(wstring2wstring,(const std::wstring& in, const wchar_t* out), (L"", L""), (L"a", L"a"), (long_wstring,long_wstring))
{
    JJ_TEST(jj::strcvt::to_wstring(in) == out);
}

JJ_TEST_CLASS_END(wstrcvtTests_t, wcharp2wstring, wstring2wstring)
const wchar_t* wstrcvtTests_t::long_wstring = L"AbC023as;lk/./.as2190";


#if defined(_WINDOWS) || defined(_WIN32) || ( __GNUC__ > 5 ) || (__GNUC__ == 5 && (__GNUC_MINOR__ > 1 ) )
JJ_TEST_CLASS(xstrcvtTests_t)
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

JJ_TEST_CLASS_END(xstrcvtTests_t, wcharp2string, wstring2string, charp2wstring, string2wstring)
const char* xstrcvtTests_t::long_string = "AbC023as;lk/./.as2190";
const wchar_t* xstrcvtTests_t::long_wstring = L"AbC023as;lk/./.as2190";
#endif // defined(...

JJ_TEST_CLASS(str_starts_withTests_t)

JJ_TEST_CASE_VARIANTS(starts_with,(const char* a, const char* b, bool result),\
    ("ABC","A", true),("ABC","AB", true),("ABC","ABC",true),\
    ("ABC","B", false),("ABC","AC",false),("ABC","BC",false),("ABC","X",false),("ABC","ABX",false),\
    ("ABC","a", false),("ABC","abc",false),\
    ("","X",false),("X","",true),("","",true),\
    (nullptr,"X",false),("X",nullptr,true),(nullptr,nullptr,true))

{
    JJ_TEST(jj::str::starts_with(a,b)==result);
}

JJ_TEST_CASE_VARIANTS(starts_with_s_c,(const std::string& a, const char* b, bool result),("012","0",true),("0","012",false))
{
    JJ_TEST(jj::str::starts_with(a, b)== result);
}

JJ_TEST_CASE_VARIANTS(starts_with_c_s,(const char* a, const std::string& b, bool result),("012","0",true),("0","012",false))
{
    JJ_TEST(jj::str::starts_with(a, b)== result);
}

JJ_TEST_CASE_VARIANTS(starts_with_s_s,(const std::string& a, const std::string& b, bool result),("012","0",true),("0","012",false))
{
    JJ_TEST(jj::str::starts_with(a, b)== result);
}

JJ_TEST_CASE_VARIANTS(wstarts_with,(const wchar_t* a, const wchar_t* b, bool result),\
    (L"ABC",L"A", true),(L"ABC",L"AB", true),(L"ABC",L"ABC",true),\
    (L"ABC",L"B", false),(L"ABC",L"AC",false),(L"ABC",L"BC",false),(L"ABC",L"X",false),(L"ABC",L"ABX",false),\
    (L"ABC",L"a", false),(L"ABC",L"abc",false),\
    (L"",L"X",false),(L"X",L"",true),(L"",L"",true),\
    (nullptr,L"X",false),(L"X",nullptr,true),(nullptr,nullptr,true))

{
    JJ_TEST(jj::str::starts_with(a,b)==result);
}

JJ_TEST_CASE_VARIANTS(starts_with_ws_wc,(const std::wstring& a, const wchar_t* b, bool result),(L"012",L"0",true),(L"0",L"012",false))
{
    JJ_TEST(jj::str::starts_with(a, b)== result);
}

JJ_TEST_CASE_VARIANTS(starts_with_wc_ws,(const wchar_t* a, const std::wstring& b, bool result),(L"012",L"0",true),(L"0",L"012",false))
{
    JJ_TEST(jj::str::starts_with(a, b)== result);
}

JJ_TEST_CASE_VARIANTS(starts_with_ws_ws,(const std::wstring& a, const std::wstring& b, bool result),(L"012",L"0",true),(L"0",L"012",false))
{
    JJ_TEST(jj::str::starts_with(a, b)== result);
}

JJ_TEST_CLASS_END(str_starts_withTests_t, starts_with, starts_with_s_c, starts_with_c_s, starts_with_s_s, wstarts_with, starts_with_ws_wc, starts_with_wc_ws, starts_with_ws_ws)
