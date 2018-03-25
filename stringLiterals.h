#ifndef JJ_STRING_LITERALS_H
#define JJ_STRING_LITERALS_H

namespace jj
{

namespace str
{

template<typename CH>
struct literals_t;

template<>
struct literals_t<char>
{
    static const char NL = '\n';
    static const char BSLASH = '\\';
    static const char TAB = '\t';
    static const char SPACE = ' ';
    static const char EQUAL = '=';
    static const char LCB = '{';
    static const char RCB = '}';
    static const char UNDERSCORE = '_';
    static const char MINUS = '-';
    static const char f = 'f';
    static const char n = 'n';
    static const char t = 't';
    static const char x = 'x';
    static const char y = 'y';
    static const char d0 = '0';
    static const char d1 = '1';
    static const char* const TRUE;
    static const char* const FALSE;
};

template<>
struct literals_t<wchar_t>
{
    static const wchar_t NL = L'\n';
    static const wchar_t BSLASH = L'\\';
    static const wchar_t TAB = L'\t';
    static const wchar_t SPACE = L' ';
    static const wchar_t EQUAL = L'=';
    static const wchar_t LCB = L'{';
    static const wchar_t RCB = L'}';
    static const wchar_t UNDERSCORE = L'_';
    static const wchar_t MINUS = L'-';
    static const wchar_t f = L'f';
    static const wchar_t n = L'n';
    static const wchar_t t = L't';
    static const wchar_t x = L'x';
    static const wchar_t y = L'y';
    static const wchar_t d0 = L'0';
    static const wchar_t d1 = L'1';
    static const wchar_t* const TRUE;
    static const wchar_t* const FALSE;
};

} // namespace str

} // namespace jj

#endif // JJ_STRING_LITERALS_H
