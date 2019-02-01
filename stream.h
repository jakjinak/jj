#ifndef JJ_STREAM_H
#define JJ_STREAM_H

#include "jj/string.h"
#include <iosfwd>

namespace jj
{
/*! ostream_t
Denotes the system native type of generic output stream. */
/*! istream_t
Denotes the system native type of generic input stream. */
/*! fstream_t, ofstream_t, ifstream_t
Denotes the system native types of inout/out/in file stream. */
/*! sstream_t, osstream_t, isstream_t
Denotes the system native types of inout/out/in string stream. */
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

extern istream_t& cin; //!< alias to the system native standard input
extern ostream_t& cout; //!< alias to the system native standard output
extern ostream_t& cerr; //!< alias to the system native standard error output

namespace AUX
{
/*! A helper for the jjS and jjS2 macros. */
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

/*! Expands to an expression which "in-line" uses default stringstream to convert
given streamed values into a string. Usage:
my_function_taking_string_as_parameter(jjS(jjT("the values are [") << a << jjT(',') << b << jjT(']'))) */
#define jjS(text) (jj::AUX::SStreamWrap<jj::sstreamt_t>() << text).str()
/*! Expands to an expression which "in-line" uses given stringstream type to convert
given streamed values into a string. Usage:
throw MyException(jjS2(std::ostringstream, jjT("invalid argument [") << a << jjT(']'))) */
#define jjS2(type,text) (jj::AUX::SStreamWrap<type>() << text).str()

#endif // JJ_STREAM_H
