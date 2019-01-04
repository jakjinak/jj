#include "jj/source.h"
#include "jj/test/test.h"
#include <sstream>

JJ_TEST_CLASS(streamSourceTests_t)

JJ_TEST_CASE(emptystream_alwaysreturnfalse)
{
    std::stringstream str("");
    jj::streamSource_t<std::stringstream> src(str);
    char ch;
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.get(ch));
    JJ_TEST(!src.get(ch));
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.get(ch));
    src.unget();
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.get(ch));
}

JJ_TEST_CASE(wemptystream_alwaysreturnfalse)
{
    std::wstringstream str(L"");
    jj::streamSource_t<std::wstringstream> src(str);
    wchar_t ch;
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.get(ch));
    JJ_TEST(!src.get(ch));
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.get(ch));
    src.unget();
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.get(ch));
}

JJ_TEST_CASE(simplepeekgetunget)
{
    std::stringstream str("123");
    jj::streamSource_t<std::stringstream> src(str);
    char ch;
    JJ_TEST(src.peek(ch) && ch == '1');
    JJ_TEST(src.get(ch) && ch == '1');
    src.unget();
    JJ_TEST(src.get(ch) && ch == '1');
    JJ_TEST(src.peek(ch) && ch == '2');
    JJ_TEST(src.get(ch) && ch == '2');
    JJ_TEST(src.peek(ch) && ch == '3');
    JJ_TEST(src.get(ch) && ch == '3');
    src.unget();
    JJ_TEST(src.peek(ch) && ch == '3');
    JJ_TEST(src.get(ch) && ch == '3');
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.get(ch));
}

JJ_TEST_CASE(wsimplepeekgetunget)
{
    std::wstringstream str(L"123");
    jj::streamSource_t<std::wstringstream> src(str);
    wchar_t ch;
    JJ_TEST(src.peek(ch) && ch == L'1');
    JJ_TEST(src.get(ch) && ch == L'1');
    src.unget();
    JJ_TEST(src.get(ch) && ch == L'1');
    JJ_TEST(src.peek(ch) && ch == L'2');
    JJ_TEST(src.get(ch) && ch == L'2');
    JJ_TEST(src.peek(ch) && ch == L'3');
    JJ_TEST(src.get(ch) && ch == L'3');
    src.unget();
    JJ_TEST(src.peek(ch) && ch == L'3');
    JJ_TEST(src.get(ch) && ch == L'3');
    JJ_TEST(!src.peek(ch));
    JJ_TEST(!src.get(ch));
}

JJ_TEST_CASE(ungetatend_staysend)
{
    std::stringstream str("123");
    jj::streamSource_t<std::stringstream> src(str);
    char ch;
    JJ_TEST(src.get(ch) && ch == '1');
    JJ_TEST(src.get(ch) && ch == '2');
    JJ_TEST(src.get(ch) && ch == '3');
    JJ_TEST(!src.get(ch));
    src.unget();
    JJ_TEST(!src.get(ch));
}

JJ_TEST_CASE(wungetatend_staysend)
{
    std::wstringstream str(L"123");
    jj::streamSource_t<std::wstringstream> src(str);
    wchar_t ch;
    JJ_TEST(src.get(ch) && ch == L'1');
    JJ_TEST(src.get(ch) && ch == L'2');
    JJ_TEST(src.get(ch) && ch == L'3');
    JJ_TEST(!src.get(ch));
    src.unget();
    JJ_TEST(!src.get(ch));
}

JJ_TEST_CASE(ungetatstart_noeffect)
{
    std::stringstream str("123");
    jj::streamSource_t<std::stringstream> src(str);
    char ch;
    src.unget();
    src.unget();
    src.unget();
    src.unget();
    JJ_TEST(src.get(ch) && ch == '1');
}

JJ_TEST_CASE(wungetatstart_noeffect)
{
    std::wstringstream str(L"123");
    jj::streamSource_t<std::wstringstream> src(str);
    wchar_t ch;
    src.unget();
    src.unget();
    src.unget();
    src.unget();
    JJ_TEST(src.get(ch) && ch == L'1');
}

JJ_TEST_CASE(stream_takenbyreference)
{
    std::stringstream str("ABC");
    jj::streamSource_t<std::stringstream> src(str);
    JJ_TEST(src.stream().str() == "ABC");
    str.str("123");
    JJ_TEST(src.stream().str() == "123");
}

JJ_TEST_CASE(wstream_takenbyreference)
{
    std::wstringstream str(L"ABC");
    jj::streamSource_t<std::wstringstream> src(str);
    JJ_TEST(src.stream().str() == L"ABC");
    str.str(L"123");
    JJ_TEST(src.stream().str() == L"123");
}

JJ_TEST_CLASS_END(streamSourceTests_t, emptystream_alwaysreturnfalse, wemptystream_alwaysreturnfalse, simplepeekgetunget, wsimplepeekgetunget, ungetatend_staysend, wungetatend_staysend, \
    ungetatstart_noeffect, wungetatstart_noeffect, stream_takenbyreference, wstream_takenbyreference)
