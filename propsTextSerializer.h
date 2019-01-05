#ifndef JJ_PROPS_TEXT_SERIALIZER_H
#define JJ_PROPS_TEXT_SERIALIZER_H

#include "jj/props.h"
#include <type_traits>

namespace jj
{
namespace props
{

/*! Converts a props class with its values into a textual representation.
Nested props are in subsections by path.

It operates on top of an actual stream and uses the traverse() method to process all the values.

To support custom types write a operator<< like this:
template<typename S>
jj::props::textSerializer_t<S>& operator<<(jj::props::textSerializer_t<S>& s, yourtype v)
{
    ...
    return s;
}
or a regular stream operator<< like
template<typename CH, typename TR>
std::basic_ostream<CH,TR>& operator<<(std::basic_ostream<CH,TR>& s, yourtype v)
{
    ...
    return s;
}
Of course, it does not matter if v is given by value, reference (or even pointer).

Example:
number=5
textual="A\nB C"
[sub]
var="value"
[sub/subsub]
X=1
Y=2
*/
template<typename STREAM>
struct textSerializer_t
{
    typedef STREAM stream_type; //!< type of stream to write to
    typedef typename stream_type::char_type char_type; //!< character type of the stream

    /*! Ctor - no indentation. */
    textSerializer_t(stream_type& s) : s_(s), inds_(0), indch_('\t') {}
    /*! Ctor - with indentation specification */
    textSerializer_t(stream_type& s, size_t n = 1, char ch = '\t') : s_(s), inds_(n), indch_(ch) {}

    /*! Return the internally stored stream (note it's taken by reference in ctor and that reference is stored). */
    stream_type& stream() { return s_; }

    /*! Helper to write indentation (as specified in ctor) of given depth. */
    inline void indent(size_t depth)
    {
        if (inds_ == 0)
            return;

        for (size_t i = 0; i< depth; ++i)
            for (size_t cnt = 0; cnt < inds_; ++cnt)
                s_ << indch_;
    }

    /*! Invoked by the traverse(), do not call directly. */
    template<typename CTX, typename KEY, typename T>
    void onValue(CTX& ctx, KEY key, const T& v);

    /*! Invoked by the traverse(), do not call directly. */
    template<typename CTX>
    void onNestedBegin(CTX& ctx)
    {
        s_ << jj::str::literals_t<char_type>::LSB;
        bool first = true;
        for (const auto& key : ctx.Dive)
        {
            if (!first)
                s_ << jj::str::literals_t<char_type>::SLASH;
            s_ << key;
            first = false;
        }
        s_ << jj::str::literals_t<char_type>::RSB << jj::str::literals_t<char_type>::NL;
    }
    /*! Invoked by the traverse(), do not call directly. */
    template<typename CTX>
    void onNestedEnd(CTX& ctx)
    {
    }

private:
    stream_type& s_; //!< stored stream reference
    const char indch_; //!< number of characters for one level of depth
    const size_t inds_; //!< character for indenting
};

} // namespace props
} // namespace jj

template<typename S, typename T>
jj::props::textSerializer_t<S>& operator<<(jj::props::textSerializer_t<S>& s, const T& v)
{
    s.stream() << v;
    return s;
}

namespace jj
{
namespace props
{
template<typename STREAM>
template<typename CTX, typename KEY, typename T>
void textSerializer_t<STREAM>::onValue(CTX& ctx, KEY key, const T& v)
{
    indent(ctx.Dive.size());
    s_ << key << jj::str::literals_t<char_type>::EQUAL;
    *this << v;
    s_ << jj::str::literals_t<char_type>::NL;
}

} // namespace props
} // namespace jj

template<typename S>
jj::props::textSerializer_t<S>& operator<<(jj::props::textSerializer_t<S>& s, bool v)
{
    // TODO provide means for configuring boolalpha or maybe other "styles"
    s.stream() << v;
    return s;
}

template<typename S, typename CH, typename TR, typename AL>
jj::props::textSerializer_t<S>& operator<<(jj::props::textSerializer_t<S>& s, const std::basic_string<CH, TR, AL>& v)
{
    typedef typename jj::props::textSerializer_t<S>::stream_type stream_type;
    typedef typename stream_type::char_type char_type;
    static_assert(std::is_same<char_type, CH>::value, "Must be of same character type!");
    stream_type& sr = s.stream();
    sr << jj::str::literals_t<CH>::QUOTE;
    for (CH ch : v)
    {
        if (ch == jj::str::literals_t<CH>::BSLASH || ch == jj::str::literals_t<CH>::QUOTE)
            sr << jj::str::literals_t<CH>::BSLASH;
        else if (ch == jj::str::literals_t<CH>::NL)
        {
            sr << jj::str::literals_t<CH>::BSLASH;
            ch = jj::str::literals_t<CH>::n;
        }
        sr << ch;
    }
    sr << jj::str::literals_t<CH>::QUOTE;
    return s;
}

template<typename S, typename T>
jj::props::textSerializer_t<S>& operator<<(jj::props::textSerializer_t<S>& s, const std::list<T>& v)
{
    typedef typename jj::props::textSerializer_t<S>::stream_type stream_type;
    typedef typename stream_type::char_type char_type;
    stream_type& sr = s.stream();
    bool first = true;
    for (const T& x : v)
    {
        if (!first)
            sr << jj::str::literals_t<char_type>::COMMA;
        s << x;
        first = false;
    }
    return s;
}

#endif // JJ_PROPS_TEXT_SERIALIZER_H
