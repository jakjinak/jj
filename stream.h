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
    typedef typename T::char_type char_type;
    typedef typename T::traits_type traits_type;
    typedef typename T::allocator_type alloc_type;
    typedef std::basic_string<char_type, traits_type, alloc_type> string_type;
    T stream_;
public:
    template<typename V>
    SStreamWrap& operator<<(const V& v)
    {
        stream_ << v;
        return *this;
    }
    string_type str() const
    {
        return stream_.str();
    }
};
} // namespace AUX
} // namespace jj

#define jjS(text) (jj::AUX::SStreamWrap<jj::sstreamt_t>() << text).str()
#define jjS2(type,text) (jj::AUX::SStreamWrap<type>() << text).str()

#endif // JJ_STREAM_H
