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

//================================================

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

//================================================

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

//================================================

JJ_TEST_CLASS(cmpTests_t)

JJ_TEST_CASE_VARIANTS(cmpc, (char a, char b, int result), \
    ('A','a',-1),('a','A',1),('A','A',0),('a','a',0),\
    ('a','b',-1),('b','a',1),('a','a',0),('X','Y',-1),('Y','X',1),('X','X',0),\
    ('0','1',-1),('1','0',1),('0','0',0),('0','9',-1),('9','0',1),('9','9',0))
{
    JJ_TEST(jj::str::cmp(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(cmp0, (const char* a, const char* b, int result), \
    (nullptr,nullptr,0),(nullptr,"",0),("",nullptr,0),\
    (nullptr,"A",-1),("A",nullptr,1),(nullptr,"xyz",-1),("xyz",nullptr,1),\
    ("","",0),("","A",-1),("A","",1),("","a",-1),("a","",1),("","xyz",-1),("xyz","",1))
{
    JJ_TEST(jj::str::cmp(a, b) == result);
    JJ_TEST(jj::str::cmp(a, b, 10) == 0);
    JJ_TEST(jj::str::cmp(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::string(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), std::string(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmp,(const char*a, const char* b, int result),\
    ("A","a",-1),("a","A",1),("a","a",0),("abC","abc",-1),("aBc","abc",-1),("abc","aBc",1),("abc","abc",0),\
    ("a","b",-1),("b","a",1),("X","Y",-1),("Y","X",1),\
    ("ijk","pjk",-1),("pjk","ijk",1),("idk","ijk",-1),("ijk","idk",1),("   ","   ",0),\
    ("a","aa",-1),("aa","a",1),("abcde","abcdef",-1),("abcdef","abcde",1))
{
    JJ_TEST(jj::str::cmp(a, b) == result);
    JJ_TEST(jj::str::cmp(a, b, 10) == 0);
    JJ_TEST(jj::str::cmp(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::string(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), std::string(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpno, (const char*a, const char* b, int result), \
    ("0","1",-1),("1","0",1),("0","9",-1),("9","0",1),("123","1234",-1),("1234","123",1),("0123456789", "0123456789", 0))
{
    JJ_TEST(jj::str::cmp(a, b) == result);
    JJ_TEST(jj::str::cmp(a, b, 10) == 0);
    JJ_TEST(jj::str::cmp(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::string(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), std::string(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpp, (const char* a, const char* b, size_t pos, int result), \
    ("XBC","ABC",1,0),("XBC","Abc",1,-1),("Xbc","ABC",1,1),\
    ("Xa","Aaa",1,-1),("Xaa","Aa",1,1),("X","Ab",1,-1),("Xb","A",1,1),("X","Aabcde",4,-1),("A","Xabcde",4,-1),("Aabcde","X",4,1))
{
    JJ_TEST(jj::str::cmp(a, b, pos) == result);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), b, pos) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::string(b), pos) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), std::string(b), pos) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpn, (const char* a, const char* b, size_t len, int result), \
    ("abcbla","abcxyz",3,0),("Abc","Bbc",1,-1),("ab","abc",2,0))
{
    JJ_TEST(jj::str::cmp(a, b, 0, len) == result);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), b, 0, len) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::string(b), 0, len) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::string(a), std::string(b), 0, len) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpc_w, (wchar_t a, wchar_t b, int result), \
    (L'A',L'a',-1),(L'a',L'A',1),(L'A',L'A',0),(L'a',L'a',0),\
    (L'a',L'b',-1),(L'b',L'a',1),(L'a',L'a',0),(L'X',L'Y',-1),(L'Y',L'X',1),(L'X',L'X',0),\
    (L'0',L'1',-1),(L'1',L'0',1),(L'0',L'0',0),(L'0',L'9',-1),(L'9',L'0',1),(L'9',L'9',0))
{
    JJ_TEST(jj::str::cmp(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(cmp0_w, (const wchar_t* a, const wchar_t* b, int result), \
    (nullptr,nullptr,0),(nullptr,L"",0),(L"",nullptr,0),\
    (nullptr,L"A",-1),(L"A",nullptr,1),(nullptr,L"xyz",-1),(L"xyz",nullptr,1),\
    (L"",L"",0),(L"",L"A",-1),(L"A",L"",1),(L"",L"a",-1),(L"a",L"",1),(L"",L"xyz",-1),(L"xyz",L"",1))
{
    JJ_TEST(jj::str::cmp(a, b) == result);
    JJ_TEST(jj::str::cmp(a, b, 10) == 0);
    JJ_TEST(jj::str::cmp(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::wstring(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), std::wstring(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmp_w,(const wchar_t* a, const wchar_t* b, int result),\
    (L"A",L"a",-1),(L"a",L"A",1),(L"a",L"a",0),(L"abC",L"abc",-1),(L"aBc",L"abc",-1),(L"abc",L"aBc",1),(L"abc",L"abc",0),\
    (L"a",L"b",-1),(L"b",L"a",1),(L"X",L"Y",-1),(L"Y",L"X",1),\
    (L"ijk",L"pjk",-1),(L"pjk",L"ijk",1),(L"idk",L"ijk",-1),(L"ijk",L"idk",1),(L"   ",L"   ",0),\
    (L"a",L"aa",-1),(L"aa",L"a",1),(L"abcde",L"abcdef",-1),(L"abcdef",L"abcde",1))
{
    JJ_TEST(jj::str::cmp(a, b) == result);
    JJ_TEST(jj::str::cmp(a, b, 10) == 0);
    JJ_TEST(jj::str::cmp(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::wstring(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), std::wstring(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpno_w, (const wchar_t* a, const wchar_t* b, int result), \
    (L"0",L"1",-1),(L"1",L"0",1),(L"0",L"9",-1),(L"9",L"0",1),(L"123",L"1234",-1),(L"1234",L"123",1),(L"0123456789", L"0123456789", 0))
{
    JJ_TEST(jj::str::cmp(a, b) == result);
    JJ_TEST(jj::str::cmp(a, b, 10) == 0);
    JJ_TEST(jj::str::cmp(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::wstring(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), std::wstring(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpp_w, (const wchar_t* a, const wchar_t* b, size_t pos, int result), \
    (L"XBC",L"ABC",1,0),(L"XBC",L"Abc",1,-1),(L"Xbc",L"ABC",1,1),\
    (L"Xa",L"Aaa",1,-1),(L"Xaa",L"Aa",1,1),(L"X",L"Ab",1,-1),(L"Xb",L"A",1,1),(L"X",L"Aabcde",4,-1),(L"A",L"Xabcde",4,-1),(L"Aabcde",L"X",4,1))
{
    JJ_TEST(jj::str::cmp(a, b, pos) == result);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), b, pos) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::wstring(b), pos) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), std::wstring(b), pos) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpn_w, (const wchar_t* a, const wchar_t* b, size_t len, int result), \
    (L"abcbla",L"abcxyz",3,0),(L"Abc",L"Bbc",1,-1),(L"ab",L"abc",2,0))
{
    JJ_TEST(jj::str::cmp(a, b, 0, len) == result);
    if (a != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), b, 0, len) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmp(a, std::wstring(b), 0, len) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmp(std::wstring(a), std::wstring(b), 0, len) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpc_i, (char a, char b, int result), \
    ('A','a',0),('a','A',0),('A','A',0),('a','a',0),\
    ('a','b',-1),('b','a',1),('a','a',0),('X','Y',-1),('Y','X',1),('X','X',0),\
    ('0','1',-1),('1','0',1),('0','0',0),('0','9',-1),('9','0',1),('9','9',0))
{
    JJ_TEST(jj::str::cmpi(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(cmp0_i, (const char* a, const char* b, int result), \
    (nullptr,nullptr,0),(nullptr,"",0),("",nullptr,0),\
    (nullptr,"A",-1),("A",nullptr,1),(nullptr,"xyz",-1),("xyz",nullptr,1),\
    ("","",0),("","A",-1),("A","",1),("","a",-1),("a","",1),("","xyz",-1),("xyz","",1))
{
    JJ_TEST(jj::str::cmpi(a, b) == result);
    JJ_TEST(jj::str::cmpi(a, b, 10) == 0);
    JJ_TEST(jj::str::cmpi(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::string(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), std::string(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmp_i,(const char*a, const char* b, int result),\
    ("A","a",0),("a","A",0),("a","a",0),("abC","abc",0),("aBc","abc",0),("abc","aBc",0),("abc","abc",0),\
    ("a","b",-1),("b","a",1),("X","Y",-1),("Y","X",1),\
    ("ijk","pjk",-1),("pjk","ijk",1),("idk","ijk",-1),("ijk","idk",1),("   ","   ",0),\
    ("a","aa",-1),("aa","a",1),("abcde","abcdef",-1),("abcdef","abcde",1))
{
    JJ_TEST(jj::str::cmpi(a, b) == result);
    JJ_TEST(jj::str::cmpi(a, b, 10) == 0);
    JJ_TEST(jj::str::cmpi(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::string(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), std::string(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpno_i, (const char*a, const char* b, int result), \
    ("0","1",-1),("1","0",1),("0","9",-1),("9","0",1),("123","1234",-1),("1234","123",1),("0123456789", "0123456789", 0))
{
    JJ_TEST(jj::str::cmpi(a, b) == result);
    JJ_TEST(jj::str::cmpi(a, b, 10) == 0);
    JJ_TEST(jj::str::cmpi(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::string(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), std::string(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpp_i, (const char* a, const char* b, size_t pos, int result), \
    ("XBC","ABC",1,0),("XBC","Abc",1,0),("Xbc","ABC",1,0),\
    ("Xa","Aaa",1,-1),("Xaa","Aa",1,1),("X","Ab",1,-1),("Xb","A",1,1),("X","Aabcde",4,-1),("A","Xabcde",4,-1),("Aabcde","X",4,1))
{
    JJ_TEST(jj::str::cmpi(a, b, pos) == result);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), b, pos) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::string(b), pos) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), std::string(b), pos) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpn_i, (const char* a, const char* b, size_t len, int result), \
    ("abcbla","abcxyz",3,0),("Abc","Bbc",1,-1),("ab","abc",2,0))
{
    JJ_TEST(jj::str::cmpi(a, b, 0, len) == result);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), b, 0, len) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::string(b), 0, len) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::string(a), std::string(b), 0, len) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpc_iw, (wchar_t a, wchar_t b, int result), \
    (L'A',L'a',0),(L'a',L'A',0),(L'A',L'A',0),(L'a',L'a',0),\
    (L'a',L'b',-1),(L'b',L'a',1),(L'a',L'a',0),(L'X',L'Y',-1),(L'Y',L'X',1),(L'X',L'X',0),\
    (L'0',L'1',-1),(L'1',L'0',1),(L'0',L'0',0),(L'0',L'9',-1),(L'9',L'0',1),(L'9',L'9',0))
{
    JJ_TEST(jj::str::cmpi(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(cmp0_iw, (const wchar_t* a, const wchar_t* b, int result), \
    (nullptr,nullptr,0),(nullptr,L"",0),(L"",nullptr,0),\
    (nullptr,L"A",-1),(L"A",nullptr,1),(nullptr,L"xyz",-1),(L"xyz",nullptr,1),\
    (L"",L"",0),(L"",L"A",-1),(L"A",L"",1),(L"",L"a",-1),(L"a",L"",1),(L"",L"xyz",-1),(L"xyz",L"",1))
{
    JJ_TEST(jj::str::cmpi(a, b) == result);
    JJ_TEST(jj::str::cmpi(a, b, 10) == 0);
    JJ_TEST(jj::str::cmpi(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::wstring(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), std::wstring(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmp_iw,(const wchar_t* a, const wchar_t* b, int result),\
    (L"A",L"a",0),(L"a",L"A",0),(L"a",L"a",0),(L"abC",L"abc",0),(L"aBc",L"abc",0),(L"abc",L"aBc",0),(L"abc",L"abc",0),\
    (L"a",L"b",-1),(L"b",L"a",1),(L"X",L"Y",-1),(L"Y",L"X",1),\
    (L"ijk",L"pjk",-1),(L"pjk",L"ijk",1),(L"idk",L"ijk",-1),(L"ijk",L"idk",1),(L"   ",L"   ",0),\
    (L"a",L"aa",-1),(L"aa",L"a",1),(L"abcde",L"abcdef",-1),(L"abcdef",L"abcde",1))
{
    JJ_TEST(jj::str::cmpi(a, b) == result);
    JJ_TEST(jj::str::cmpi(a, b, 10) == 0);
    JJ_TEST(jj::str::cmpi(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::wstring(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), std::wstring(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpno_iw, (const wchar_t* a, const wchar_t* b, int result), \
    (L"0",L"1",-1),(L"1",L"0",1),(L"0",L"9",-1),(L"9",L"0",1),(L"123",L"1234",-1),(L"1234",L"123",1),(L"0123456789", L"0123456789", 0))
{
    JJ_TEST(jj::str::cmpi(a, b) == result);
    JJ_TEST(jj::str::cmpi(a, b, 10) == 0);
    JJ_TEST(jj::str::cmpi(a, b, 0, 0) == 0);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), b) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::wstring(b)) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), std::wstring(b)) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpp_iw, (const wchar_t* a, const wchar_t* b, size_t pos, int result), \
    (L"XBC",L"ABC",1,0),(L"XBC",L"Abc",1,0),(L"Xbc",L"ABC",1,0),\
    (L"Xa",L"Aaa",1,-1),(L"Xaa",L"Aa",1,1),(L"X",L"Ab",1,-1),(L"Xb",L"A",1,1),(L"X",L"Aabcde",4,-1),(L"A",L"Xabcde",4,-1),(L"Aabcde",L"X",4,1))
{
    JJ_TEST(jj::str::cmpi(a, b, pos) == result);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), b, pos) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::wstring(b), pos) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), std::wstring(b), pos) == result); }
}

JJ_TEST_CASE_VARIANTS(cmpn_iw, (const wchar_t* a, const wchar_t* b, size_t len, int result), \
    (L"abcbla",L"abcxyz",3,0),(L"Abc",L"Bbc",1,-1),(L"ab",L"abc",2,0))
{
    JJ_TEST(jj::str::cmpi(a, b, 0, len) == result);
    if (a != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), b, 0, len) == result); }
    if (b != nullptr) { JJ_TEST(jj::str::cmpi(a, std::wstring(b), 0, len) == result); }
    if (a != nullptr && b != nullptr) { JJ_TEST(jj::str::cmpi(std::wstring(a), std::wstring(b), 0, len) == result); }
}

JJ_TEST_CLASS_END(cmpTests_t, cmpc, cmp0, cmp, cmpno, cmpp, cmpn, cmpc_w, cmp0_w, cmp_w, cmpno_w, cmpp_w, cmpn_w, cmpc_i, cmp0_i, cmp_i, cmpno_i, cmpp_i, cmpn_i, cmpc_iw, cmp0_iw, cmp_iw, cmpno_iw, cmpp_iw, cmpn_iw)

//================================================

JJ_TEST_CLASS(str_lessTests_t)

JJ_TEST_CASE_VARIANTS(lessc, (char a, char b, bool result),\
    ('a','A',false), ('A','a',true),\
    ('a','b',true), ('b','a',false), ('X','Y',true), ('Y','X',false),\
    ('0','0',false), ('0','1',true), ('1','0',false), ('0','9',true), ('9','0',false))
{
    JJ_TEST(jj::str::less(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(less0,(const char* a, const char* b, bool result),\
    (nullptr,nullptr,false),(nullptr,"",false),("",nullptr,false),\
    (nullptr,"A",true),("A",nullptr,false),(nullptr,"xyz",true),("xyz",nullptr,false))
{
    JJ_TEST(jj::str::less(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(less,(const char* a, const char* b, bool result),\
    ("","",false), ("","A",true), ("A","",false), ("","a",true), ("a","",false), ("","abc",true), ("abc","", false),\
    ("a","A",false), ("A","a",true), ("abc","abC",false), ("abC","abc",true), ("abc","aBc",false), ("aBc","abc",true),\
    ("a","b",true), ("b","a",false), ("X","Y",true), ("Y","X",false),\
    ("ijk","pjk",true), ("pjk","ijk",false), ("idk","ijk",true), ("ijk","idk",false),\
    ("a","aa",true), ("aa","a",false), ("abcde","abcdef",true), ("abcdef","abcde",false))
{
    JJ_TEST(jj::str::less(a, b) == result);
    JJ_TEST(jj::str::less(std::string(a), std::string(b)) == result);
}

JJ_TEST_CASE_VARIANTS(lessn, (const char* a, const char* b, bool result),\
    ("0","0",false), ("0","1",true), ("1","0",false), ("0","9",true), ("9","0",false))
{
    JJ_TEST(jj::str::less(a, b) == result);
    JJ_TEST(jj::str::less(std::string(a), std::string(b)) == result);
}

JJ_TEST_CASE_VARIANTS(lesswc, (wchar_t a, wchar_t b, bool result),\
    (L'a',L'A',false), (L'A',L'a',true),\
    (L'a',L'b',true), (L'b',L'a',false), (L'X',L'Y',true), (L'Y',L'X',false),\
    (L'0',L'0',false), (L'0',L'1',true), (L'1',L'0',false), (L'0',L'9',true), (L'9',L'0',false))
{
    JJ_TEST(jj::str::less(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(lessw0,(const wchar_t* a, const wchar_t* b, bool result),\
    (nullptr,nullptr,false),(nullptr,L"",false),(L"",nullptr,false),\
    (nullptr,L"A",true),(L"A",nullptr,false),(nullptr,L"xyz",true),(L"xyz",nullptr,false))
{
    JJ_TEST(jj::str::less(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(lessw,(const wchar_t* a, const wchar_t* b, bool result),\
    (L"",L"",false), (L"",L"A",true), (L"A",L"",false), (L"",L"a",true), (L"a",L"",false), (L"",L"abc",true), (L"abc",L"", false),\
    (L"a",L"A",false), (L"A",L"a",true), (L"abc",L"abC",false), (L"abC",L"abc",true), (L"abc",L"aBc",false), (L"aBc",L"abc",true),\
    (L"a",L"b",true), (L"b",L"a",false), (L"X",L"Y",true), (L"Y",L"X",false),\
    (L"ijk",L"pjk",true), (L"pjk",L"ijk",false), (L"idk",L"ijk",true), (L"ijk",L"idk",false),\
    (L"a",L"aa",true), (L"aa",L"a",false), (L"abcde",L"abcdef",true), (L"abcdef",L"abcde",false))
{
    JJ_TEST(jj::str::less(a, b) == result);
    JJ_TEST(jj::str::less(std::wstring(a), std::wstring(b)) == result);
}

JJ_TEST_CASE_VARIANTS(lesswn, (const wchar_t* a, const wchar_t* b, bool result),\
    (L"0",L"0",false), (L"0",L"1",true), (L"1",L"0",false), (L"0",L"9",true), (L"9",L"0",false))
{
    JJ_TEST(jj::str::less(a, b) == result);
    JJ_TEST(jj::str::less(std::wstring(a), std::wstring(b)) == result);
}

JJ_TEST_CASE_VARIANTS(lessic, (char a, char b, bool result),\
    ('a','A',false), ('A','a',false),\
    ('a','b',true), ('b','a',false), ('X','Y',true), ('Y','X',false),\
    ('0','0',false), ('0','1',true), ('1','0',false), ('0','9',true), ('9','0',false))
{
    JJ_TEST(jj::str::lessi(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(lessi0,(const char* a, const char* b, bool result),\
    (nullptr,nullptr,false),(nullptr,"",false),("",nullptr,false),\
    (nullptr,"A",true),("A",nullptr,false),(nullptr,"xyz",true),("xyz",nullptr,false))
{
    JJ_TEST(jj::str::lessi(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(lessi, (const char* a, const char* b, bool result),\
    ("","",false), ("","A",true), ("A","", false), ("","a",true), ("a","",false), ("","abc",true), ("abc","",false),\
    ("a","A",false), ("A","a",false), ("abc","abC",false), ("abC","abc",false), ("abc","aBc",false), ("aBc","abc",false),\
    ("a","b",true), ("b","a",false), ("X","Y",true), ("Y","X",false),\
    ("ijk","pjk",true), ("pjk","ijk",false), ("idk","ijk",true), ("ijk","idk",false),\
    ("a","aa",true), ("aa","a",false), ("abcde","abcdef",true), ("abcdef","abcde",false))
{
    JJ_TEST(jj::str::lessi(a, b) == result);
    JJ_TEST(jj::str::lessi(std::string(a), std::string(b)) == result);
}

JJ_TEST_CASE_VARIANTS(lessin, (const char* a, const char* b, bool result),\
    ("0","0",false), ("0","1",true), ("1","0",false), ("0","9",true), ("9","0",false))
{
    JJ_TEST(jj::str::lessi(a, b) == result);
    JJ_TEST(jj::str::lessi(std::string(a), std::string(b)) == result);
}

JJ_TEST_CASE_VARIANTS(lesswic, (wchar_t a, wchar_t b, bool result),\
    (L'a',L'A',false), (L'A',L'a',false),\
    (L'a',L'b',true), (L'b',L'a',false), (L'X',L'Y',true), (L'Y',L'X',false),\
    (L'0',L'0',false), (L'0',L'1',true), (L'1',L'0',false), (L'0',L'9',true), (L'9',L'0',false))
{
    JJ_TEST(jj::str::lessi(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(lesswi0,(const wchar_t* a, const wchar_t* b, bool result),\
    (nullptr,nullptr,false),(nullptr,L"",false),(L"",nullptr,false),\
    (nullptr,L"A",true),(L"A",nullptr,false),(nullptr,L"xyz",true),(L"xyz",nullptr,false))
{
    JJ_TEST(jj::str::less(a, b) == result);
}

JJ_TEST_CASE_VARIANTS(lesswi, (const wchar_t* a, const wchar_t* b, bool result),\
    (L"",L"",false), (L"",L"A",true), (L"A",L"", false), (L"",L"a",true), (L"a",L"",false), (L"",L"abc",true), (L"abc",L"",false),\
    (L"a",L"A",false), (L"A",L"a",false), (L"abc",L"abC",false), (L"abC",L"abc",false), (L"abc",L"aBc",false), (L"aBc",L"abc",false),\
    (L"a",L"b",true), (L"b",L"a",false), (L"X",L"Y",true), (L"Y",L"X",false),\
    (L"ijk",L"pjk",true), (L"pjk",L"ijk",false), (L"idk",L"ijk",true), (L"ijk",L"idk",false),\
    (L"a",L"aa",true), (L"aa",L"a",false), (L"abcde",L"abcdef",true), (L"abcdef",L"abcde",false))
{
    JJ_TEST(jj::str::lessi(a, b) == result);
    JJ_TEST(jj::str::lessi(std::wstring(a), std::wstring(b)) == result);
}

JJ_TEST_CASE_VARIANTS(lesswin, (const wchar_t* a, const wchar_t* b, bool result),\
    (L"0",L"0",false), (L"0",L"1",true), (L"1",L"0",false), (L"0",L"9",true), (L"9",L"0",false))
{
    JJ_TEST(jj::str::lessi(a, b) == result);
    JJ_TEST(jj::str::lessi(std::wstring(a), std::wstring(b)) == result);
}

JJ_TEST_CLASS_END(str_lessTests_t, lessc, less0, less, lessn, lessw0, lesswc, lessw, lesswn, lessic, lessi0, lessi, lessin, lesswic, lesswi0, lesswi, lesswin)

//================================================

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

//================================================

JJ_TEST_CLASS(str_findTests_t)

static const char* ABC;
static const char* ABAABCBCC;

JJ_TEST_CASE_VARIANTS(char_in_charp,(const char* str, char what, const char* result),(ABC,'A',ABC),(ABC,'C',ABC+2),(ABC,'X',nullptr),(ABC,'a',nullptr),(ABC,'c',nullptr),("",'X',nullptr),(nullptr,'X',nullptr))
{
    JJ_TEST(jj::str::find(str, what)==result);
}

JJ_TEST_CASE_VARIANTS(char_in_charp_pos,(const char* str, char what, size_t pos, const char* result),(ABC,'A',0u,ABC),(ABC,'A',1u,nullptr),(ABC,'A',3u,nullptr),(ABC,'A',5u,nullptr),\
    (ABAABCBCC,'A',0u,ABAABCBCC),(ABAABCBCC,'A',1u,ABAABCBCC+2),(ABAABCBCC,'A',2u,ABAABCBCC+2),(ABAABCBCC,'A',3u,ABAABCBCC+3),(ABAABCBCC,'A',4u,nullptr),(ABAABCBCC,'C',4u,ABAABCBCC+5))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CASE_VARIANTS(charp_in_charp,(const char* str, const char* what, const char* result),(ABC,"A",ABC),(ABC,"C",ABC+2),(ABC,ABC,ABC),\
    (ABC,"",ABC),(ABC, nullptr, ABC),("","A",nullptr),(nullptr,"A",nullptr),(nullptr,nullptr,nullptr),\
    (ABAABCBCC,ABC,ABAABCBCC+3),(ABAABCBCC,"X",nullptr),(ABAABCBCC,"ABBC",nullptr))
{
    JJ_TEST(jj::str::find(str, what)==result);
}

JJ_TEST_CASE_VARIANTS(charp_in_charp_pos,(const char* str, const char* what, size_t pos, const char* result),(ABC,"A",0u,ABC),(ABC,"A",1u,nullptr),(ABC,"A",3u,nullptr),(ABC,"A",5u,nullptr),\
    (ABAABCBCC,"AB",0u,ABAABCBCC),(ABAABCBCC,"AB",1u,ABAABCBCC+3),(ABAABCBCC,"AB",2u,ABAABCBCC+3),(ABAABCBCC,"AB",3u,ABAABCBCC+3),(ABAABCBCC,"AB",4u,nullptr))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CASE_VARIANTS(str_in_charp,(const char* str, const std::string& what, size_t pos, const char* result),(ABC,"A",0u,ABC),(ABC,"A",1u,nullptr),(ABAABCBCC,"ABC",1u,ABAABCBCC+3),(ABAABCBCC,"ABA",1u,nullptr))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

static const size_t npos;

JJ_TEST_CASE_VARIANTS(char_in_str,(const std::string& str, char what, size_t pos, size_t result),("ABC",'A',0u,0u),("ABC",'A',1u,npos),("ABA",'A',1u,2u),("ABC",'a',0u,npos),("",'a',0u,npos))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CASE_VARIANTS(charp_in_str,(const std::string& str, const char* what, size_t pos, size_t result),("ABC","A",0u,0u),("ABC","A",1u,npos),("AABA","BA",1u,2u),("ABC","a",0u,npos),("ABC","",0u,0u),("","a",0u,npos),("ABC",nullptr,0u,0u))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CASE_VARIANTS(str_in_str,(const std::string& str, const std::string& what, size_t pos, size_t result),("ABC","A",0u,0u),("ABC","A",1u,npos),("AABA","BA",1u,2u),("ABC","a",0u,npos),("ABC","",0u,0u),("","a",0u,npos))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CLASS_END(str_findTests_t, char_in_charp, char_in_charp_pos, charp_in_charp, charp_in_charp_pos, str_in_charp, char_in_str, charp_in_str, str_in_str)
const char* str_findTests_t::ABC("ABC");
const char* str_findTests_t::ABAABCBCC("ABAABCBCC");
const size_t str_findTests_t::npos(std::string::npos);

//================================================

JJ_TEST_CLASS(wstr_findTests_t)

static const wchar_t* ABC;
static const wchar_t* ABAABCBCC;

JJ_TEST_CASE_VARIANTS(char_in_charp,(const wchar_t* str, char what, const wchar_t* result),(ABC,L'A',ABC),(ABC,L'C',ABC+2),(ABC,L'X',nullptr),(ABC,L'a',nullptr),(ABC,L'c',nullptr),(L"",L'X',nullptr),(nullptr,L'X',nullptr))
{
    JJ_TEST(jj::str::find(str, what)==result);
}

JJ_TEST_CASE_VARIANTS(char_in_charp_pos,(const wchar_t* str, char what, size_t pos, const wchar_t* result),(ABC,L'A',0u,ABC),(ABC,L'A',1u,nullptr),(ABC,L'A',3u,nullptr),(ABC,L'A',5u,nullptr),\
    (ABAABCBCC,L'A',0u,ABAABCBCC),(ABAABCBCC,L'A',1u,ABAABCBCC+2),(ABAABCBCC,L'A',2u,ABAABCBCC+2),(ABAABCBCC,L'A',3u,ABAABCBCC+3),(ABAABCBCC,L'A',4u,nullptr),(ABAABCBCC,L'C',4u,ABAABCBCC+5))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CASE_VARIANTS(charp_in_charp,(const wchar_t* str, const wchar_t* what, const wchar_t* result),(ABC,L"A",ABC),(ABC,L"C",ABC+2),(ABC,ABC,ABC),\
    (ABC,L"",ABC),(ABC, nullptr, ABC),(L"",L"A",nullptr),(nullptr,L"A",nullptr),(nullptr,nullptr,nullptr),\
    (ABAABCBCC,ABC,ABAABCBCC+3),(ABAABCBCC,L"X",nullptr),(ABAABCBCC,L"ABBC",nullptr))
{
    JJ_TEST(jj::str::find(str, what)==result);
}

JJ_TEST_CASE_VARIANTS(charp_in_charp_pos,(const wchar_t* str, const wchar_t* what, size_t pos, const wchar_t* result),(ABC,L"A",0u,ABC),(ABC,L"A",1u,nullptr),(ABC,L"A",3u,nullptr),(ABC,L"A",5u,nullptr),\
    (ABAABCBCC,L"AB",0u,ABAABCBCC),(ABAABCBCC,L"AB",1u,ABAABCBCC+3),(ABAABCBCC,L"AB",2u,ABAABCBCC+3),(ABAABCBCC,L"AB",3u,ABAABCBCC+3),(ABAABCBCC,L"AB",4u,nullptr))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CASE_VARIANTS(str_in_charp,(const wchar_t* str, const std::wstring& what, size_t pos, const wchar_t* result),(ABC,L"A",0u,ABC),(ABC,L"A",1u,nullptr),(ABAABCBCC,L"ABC",1u,ABAABCBCC+3),(ABAABCBCC,L"ABA",1u,nullptr))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

static const size_t npos;

JJ_TEST_CASE_VARIANTS(char_in_str,(const std::wstring& str, char what, size_t pos, size_t result),(L"ABC",L'A',0u,0u),(L"ABC",L'A',1u,npos),(L"ABA",L'A',1u,2u),(L"ABC",L'a',0u,npos),(L"",L'a',0u,npos))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CASE_VARIANTS(charp_in_str,(const std::wstring& str, const wchar_t* what, size_t pos, size_t result),(L"ABC",L"A",0u,0u),(L"ABC",L"A",1u,npos),(L"AABA",L"BA",1u,2u),(L"ABC",L"a",0u,npos),(L"ABC",L"",0u,0u),(L"",L"a",0u,npos),(L"ABC",nullptr,0u,0u))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CASE_VARIANTS(str_in_str,(const std::wstring& str, const std::wstring& what, size_t pos, size_t result),(L"ABC",L"A",0u,0u),(L"ABC",L"A",1u,npos),(L"AABA",L"BA",1u,2u),(L"ABC",L"a",0u,npos),(L"ABC",L"",0u,0u),(L"",L"a",0u,npos))
{
    JJ_TEST(jj::str::find(str, what, pos)==result);
}

JJ_TEST_CLASS_END(wstr_findTests_t, char_in_charp, char_in_charp_pos, charp_in_charp, charp_in_charp_pos, str_in_charp, char_in_str, charp_in_str, str_in_str)
const wchar_t* wstr_findTests_t::ABC(L"ABC");
const wchar_t* wstr_findTests_t::ABAABCBCC(L"ABAABCBCC");
const size_t wstr_findTests_t::npos = std::wstring::npos;
