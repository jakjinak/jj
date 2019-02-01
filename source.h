#ifndef JJ_SOURCE_H
#define JJ_SOURCE_H

#include "jj/string.h"

namespace jj
{

/*! Allows reading characters from a stream end of file is autodetected. */
template<typename STREAM>
class streamSource_t
{
    // TODO add position tracking policy (probably as derived source classes)
public:
    typedef STREAM stream_type; //!< underlying stream type
    typedef typename stream_type::char_type char_type; //!< character type in the underlying stream

private:
    stream_type& s_; //!< the underlying stream

public:
    /*! Ctor - takes stream by reference */
    streamSource_t(stream_type& s) : s_(s) {}

    /*! Returns the underlying stream for direct access. */
    stream_type& stream() { return s_; }

    /*! Reads next char, but does not skip past it.
    Returns true until EOF (before reading the char). */
    bool peek(char_type& ch)
    {
        bool ret = get(ch);
        if (ret)
            unget();
        return ret;
    }

    /*! Reads next char and skips past it.
    Returns true until EOF (before reading the char). */
    bool get(char_type& ch)
    {
        while (s_.good() && !s_.eof())
        {
            int tmp = s_.get();
            if (tmp == stream_type::traits_type::eof())
                return false;
            ch = char_type(tmp);
            return true;
        }
        return false;
    }

    /*! Goes back by one character in the stream - guaranteed to work only once and only if get() was called before calling unget().
    Does nothing if at the beginning. */
    void unget()
    {
        if (s_.tellg() <= typename stream_type::pos_type(0))
            return;
        s_.unget();
    }
};
} // namespace jj

#endif // JJ_SOURCE_H
