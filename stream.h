#ifndef JJ_STREAM_H
#define JJ_STREAM_H

#include "jj/string.h"
#include <iosfwd>

namespace jj
{
#if defined(JJ_USE_WSTRING)
typedef std::wostream ostream_t;
typedef std::wistream istream_t;

typedef std::wofstream ofstream_t;
typedef std::wifstream ifstream_t;
typedef std::wfstream fstream_t;

typedef std::wostringstream osstream_t;
typedef std::wistringstream isstream_t;
typedef std::wstringstream sstreamt_t;
#else
typedef std::ostream ostream_t;
typedef std::istream istream_t;

typedef std::ofstream ofstream_t;
typedef std::ifstream ifstream_t;
typedef std::fstream fstream_t;

typedef std::ostringstream osstream_t;
typedef std::istringstream isstream_t;
typedef std::stringstream sstreamt_t;
#endif // defined(JJ_USE_WSTRING)

extern istream_t& cin;
extern ostream_t& cout;
extern ostream_t& cerr;

namespace AUX
{
template<typename T>
class SStreamWrap
{
    T stream_;
public:
    template<typename V>
    SStreamWrap& operator<<(const V& v)
    {
        stream_ << v;
        return *this;
    }
    string_t str() const
    {
        return stream_.str();
    }
};
} // namespace AUX
} // namespace jj

#define jjS(text) (jj::AUX::SStreamWrap<jj::sstreamt_t>() << text).str()

#endif // JJ_STREAM_H
