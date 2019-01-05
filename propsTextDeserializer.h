#ifndef JJ_PROPS_TEXT_DESERIALIZER_H
#define JJ_PROPS_TEXT_DESERIALIZER_H

#include "jj/props.h"
#include <type_traits>

namespace jj
{
namespace props
{

namespace aux
{
/*! Helper - serves only to invoke the operator>> for the individual types. */
template<typename STREAM>
struct deserializeAction_t
{
    STREAM& s_; //!< the actual textDeserializer_t
    /*! Ctor */
    deserializeAction_t(STREAM& s) : s_(s) {}

    /*! Just invokes the operator>> for individual types and textDeserializer_t */
    template<typename T>
    void operator() (T& v)
    {
        s_ >> v;
    }
};

} // namespace aux

/*! Reads the textual representation made by textSerializer_t into real values represented by objects.
See textSerializer_t for the syntax of the textual representation. 

It uses the operator>> to deserialize the objects, for custom types you shall define one like
template<typename S, typename P>
jj::props::textDeserializer_t<S, P>& operator>>(jj::props::textDeserializer_t<S, P>& s, yourtype& v)
{
    ...
    return s;
}
(note the S is assumed to be a jj::streamSource_t instance)
or the regular stream operator>> like
template<typename CH, typename TR>
std::basic_istream<CH, TR>& operator>>(std::basic_istream<CH, TR>& s, yourtype& v)
{
    ...
    return s;
}
*/
template<typename SOURCE, typename PROPS>
class textDeserializer_t
{
    typedef typename SOURCE::char_type char_type; //!< character type of the underlying stream
    typedef std::basic_string<char_type> string_type; //!< the string type of character type equivalent to the stream's char_type
    typedef textDeserializer_t<SOURCE, PROPS> this_type; //!< refers to the type of this class
    SOURCE& src_; //!< the stored source stream
    PROPS& ps_; //!< the props into which the stream is read

    // the lexer part of this class

    /*! Types of input sequences. */
    enum token_t
    {
        INVALID, //!< unknown character
        ENDOFFILE, //!< end of source stream
        ENDOFLINE, //!< newline char
        EQUALSIGN, //!< =
        SBRACKETOPEN, //!< [
        SBRACKETCLOSE, //!< ]
        SLASH, //!< /
        NAME //!< a sequence like [A-Za-z0-9]([-_A-Za-z0-9]*)
    };
    string_type v_; //!< if NAME is read then this contains the actual value read

    /*! Check if ch can start a NAME. */
    bool isNameStart(char_type ch)
    {
        return jj::str::isalnum(ch);
    }
    /*! Check if ch can be a NAME continuation. */
    bool isNameContinue(char_type ch)
    {
        return jj::str::isalnum(ch) || ch == jj::str::literals_t<char_type>::UNDERSCORE || ch == jj::str::literals_t<char_type>::MINUS;
    }

    /*! Extracts the next token from the source stream and returns it. */
    token_t next()
    {
        while (true)
        {
            char_type ch;
            if (!src_.get(ch))
                return ENDOFFILE;
            if (ch == jj::str::literals_t<char_type>::NL)
                return ENDOFLINE;
            if (jj::str::isspace(ch))
                continue;
            if (ch == jj::str::literals_t<char_type>::SLASH)
                return SLASH;
            if (ch == jj::str::literals_t<char_type>::EQUAL)
                return EQUALSIGN;
            if (ch == jj::str::literals_t<char_type>::LSB)
                return SBRACKETOPEN;
            if (ch == jj::str::literals_t<char_type>::RSB)
                return SBRACKETCLOSE;
            if (isNameStart(ch))
            {
                v_.assign(1, ch);
                while (src_.get(ch))
                {
                    if (!isNameContinue(ch))
                    {
                        src_.unget();
                        break;
                    }
                    v_ += ch;
                }
                return NAME;
            }
            return INVALID;
        }
    }

    // the parser part of the class

    /*! Thrown if the input syntax does not conform with the grammar. */
    struct syntax_error : public jj::exception::base
    {
        syntax_error(token_t expected)
            : jj::exception::base(jjS2(std::ostringstream, "Syntax error. Unexpected " << expected << " read."))
        {
        }
        syntax_error(token_t actual, token_t expected)
            : jj::exception::base(jjS2(std::ostringstream, "Syntax error. Read " << actual << " but expected " << expected << "."))
        {
        }
        syntax_error(token_t actual, token_t exp1, token_t exp2)
            : jj::exception::base(jjS2(std::ostringstream, "Syntax error. Read " << actual << " but expected one of " << exp1 << ',' << exp2 << "."))
        {
        }
    };

    /*! Reads a token and verifies it is same as the given one, throws syntax_error otherwise. */
    token_t expect(token_t tok)
    {
        token_t t = next();
        if (t == tok)
            return tok;
        throw syntax_error(t, tok);
    }
    /*! Reads a token and verifies it is same as any of the given ones, throws syntax_error otherwise. */
    token_t expect(token_t tok1, token_t tok2)
    {
        token_t t = next();
        if (t == tok1)
            return t;
        if (t == tok2)
            return t;
        throw syntax_error(t, tok1, tok2);
    }

    string_type cpath_; //!< holds the current path (for nested props), empty if processing the top props

    /*! Parses the input and deserializes all values (stores into props) as part of the process. */
    void read()
    {
        while (true)
        {
            token_t tok = next();

            if (tok == ENDOFFILE)
                break;
            if (tok == ENDOFLINE)
                continue;
            if (tok == SBRACKETOPEN)
            {
                expect(NAME);
                cpath_.assign(v_);
                while (true)
                {
                    tok = expect(SBRACKETCLOSE, SLASH);
                    if (tok == SBRACKETCLOSE)
                    {
                        expect(ENDOFLINE, ENDOFFILE);
                        break; // read path
                    }
                    else
                    {
                        // must be a slash
                        expect(NAME);
                        cpath_ += jj::str::literals_t<char_type>::SLASH;
                        cpath_ += v_;
                    }
                }
                continue; // done reading section
            }
            if (tok == NAME)
            {
                string_type key(v_);
                expect(EQUALSIGN);

                // locate the actual property and invoke it's deserialization
                pathWalker_t<PROPS> walk(ps_);
                aux::deserializeAction_t<this_type> a(*this);
                walk.apply(a, cpath_.empty() ? key : cpath_ + jj::str::literals_t<char_type>::SLASH + key);

                expect(ENDOFLINE, ENDOFFILE);
            }
        }
    }

public:
    /*! Ctor - reads the given source stream s and stores contents into props p.
    Throws syntax_error (derivate of std::exception) in case of an syntax error. */
    textDeserializer_t(SOURCE& s, PROPS& p)
        : src_(s), ps_(p)
    {
        read();
    }

    /* Returns the source stream reference as given to ctor - can be used in the custom operator>> */
    SOURCE& source() { return src_; }
};

} // namespace props
} // namespace jj

template<typename S, typename P, typename T>
jj::props::textDeserializer_t<S, P>& operator>>(jj::props::textDeserializer_t<S, P>& s, T& v)
{
    s.source().stream() >> v;
    return s;
}

template<typename S, typename P>
jj::props::textDeserializer_t<S, P>& operator>>(jj::props::textDeserializer_t<S, P>& s, bool& v)
{
    s.source().stream() >> v;
    // TODO support boolalpha (and other styles)
    return s;
}

template<typename S, typename P, typename CH, typename TR, typename AL>
jj::props::textDeserializer_t<S, P>& operator>>(jj::props::textDeserializer_t<S, P>& s, std::basic_string<CH, TR, AL>& v)
{
    S& str = s.source();
    typedef typename S::char_type char_type;
    static_assert(std::is_same<char_type, CH>::value, "Must be of same character type!");
    char_type ch;
    if (!str.get(ch) || ch != jj::str::literals_t<char_type>::QUOTE)
        throw jj::exception::base("Expected a '\"'.");
    v.clear();
    bool escape = false;
    bool done = false;
    while (str.get(ch))
    {
        if (ch == jj::str::literals_t<char_type>::NL)
            throw jj::exception::base("Unexpected newline, a '\"' was expected.");
        if (escape)
        {
            if (ch == jj::str::literals_t<char_type>::BSLASH || ch == jj::str::literals_t<char_type>::QUOTE)
                v += ch;
            else if (ch == jj::str::literals_t<char_type>::n)
                v += jj::str::literals_t<char_type>::NL;
            else
            {
                // unknown escape, fallback
                v += jj::str::literals_t<char_type>::BSLASH;
                v += ch;
            }
            escape = false;
            continue;
        }
        if (ch == jj::str::literals_t<char_type>::QUOTE)
        {
            done = true;
            break;
        }
        if (ch == jj::str::literals_t<char_type>::BSLASH)
            escape = true;
        else
            v += ch;
    }
    if (!done)
        throw jj::exception::base("Expected a '\"'.");
    return s;
}

namespace jj
{
namespace props
{
namespace aux
{

template<typename S, typename P, typename C, typename T>
inline void readContainer(jj::props::textDeserializer_t<S, P>& s, C& c)
{
    S& src = s.source();
    typedef typename S::char_type char_type;
    c.clear();

    // check for empty container
    char_type ch;
    if (!src.get(ch))
        return;
    if (ch == jj::str::literals_t<char_type>::NL)
    {
        src.unget();
        return;
    }
    src.unget();

    // otherwise read the first element
    T x;
    s >> x;
    c.push_back(x);

    // now read all the remaining elements
    while (src.get(ch))
    {
        if (ch == jj::str::literals_t<char_type>::NL)
        {
            src.unget();
            return;
        }
        if (ch == jj::str::literals_t<char_type>::COMMA)
        {
            T x;
            s >> x;
            c.push_back(x);
        }
        else
            throw jj::exception::base("Syntax error. Expected ',' or newline.");
    }
}

} // namespace aux
} // namespace props
} // namespace jj

template<typename S, typename P, typename T>
jj::props::textDeserializer_t<S, P>& operator>>(jj::props::textDeserializer_t<S, P>& s, std::list<T>& v)
{
    jj::props::aux::readContainer<S, P, std::list<T>, T>(s, v);
    return s;
}

#endif // JJ_PROPS_TEXT_DESERIALIZER_H
