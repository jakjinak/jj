#include "jj/stream.h"

#include <iostream>

namespace jj
{
#if defined(JJ_USE_WSTRING)
istream_t& cin = std::wcin;
ostream_t& cout = std::wcout;
ostream_t& cerr = std::wcerr;
#else
istream_t& cin = std::cin;
ostream_t& cout = std::cout;
ostream_t& cerr = std::cerr;
#endif // defined(JJ_USE_WSTRING)
} // namespace jj
