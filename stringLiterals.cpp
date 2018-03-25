#include "jj/stringLiterals.h"

namespace jj
{

namespace str
{

const char* const literals_t<char>::TRUE = "true";
const char* const literals_t<char>::FALSE = "false";

const wchar_t* const literals_t<wchar_t>::TRUE = L"true";
const wchar_t* const literals_t<wchar_t>::FALSE = L"false";

} // namespace str

} // namespace jj
