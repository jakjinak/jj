#ifndef JJ_CONFIGURATION_STORAGE_H
#define JJ_CONFIGURATION_STORAGE_H

#include "jj/configuration.h"
#include "jj/stream.h"
#include "jj/stringLiterals.h"
#include <map>
#include <functional>
#include <iomanip>
#include <limits>

namespace jj
{

namespace conf
{

//=== helpers ============

namespace AUX
{

/*! Defines text literals in proper character width. */
template<typename T>
struct bool_texts_t;

template<>
struct bool_texts_t<char>
{
    static const char* const alse;
    static const char* const rue;
    static const char* const es;
    static const char* const o;
};

template<>
struct bool_texts_t<wchar_t>
{
    static const wchar_t* const alse;
    static const wchar_t* const rue;
    static const wchar_t* const es;
    static const wchar_t* const o;
};

} // namespace AUX


  /*! Base class for other storage classes. Currently no use. */
class storage_base_t
{
public:
};

//=== base classes ============

/*! A base class for all streamed storage loaders. */
template<typename CH, typename TR>
class istreamStorage_base_T : public storage_base_t
{
    std::basic_istream<CH, TR>& s_; //!< the actual stream being loaded from

    /*! Reads given characters (con) from stream and throws if not matching.
    In fact returns if the first character does not match. */
    void check_bool_continuation(const CH* con)
    {
        CH ch = s_.peek();
        if (!jj::str::equali(*con, ch))
            return; // fine with single character, no more match

        // if second matches (here first) then all have to match
        while (*con != 0 && s_.good())
        {
            s_.get(ch);
            if (!jj::str::equali(*con, ch))
                throw std::runtime_error("Invalid bool value");
            ++con;
        }
        if (*con != 0)
            throw std::runtime_error("Invalid bool value");
    }

protected:
    /*! Constructor - takes given stream reference to operate on. */
    istreamStorage_base_T(std::basic_istream<CH, TR>& s) : s_(s) {}

    /*! Reads from stream as long as it sees whitespace (stops at newline). */
    void skip_spaces()
    {
        while (s_.good())
        {
            CH ch = s_.peek();
            if (!jj::str::isspace(ch) || ch == jj::str::literals_t<CH>::NL)
                return;
            s_.get(ch);
        }
    }
    /*! Reads spaces until it sees newline and consumes also the newline. */
    void skip_past_eol()
    {
        while (s_.good())
        {
            CH ch = s_.peek();
            if (!jj::str::isspace(ch))
                return;
            s_.get(ch);
            if (ch == jj::str::literals_t<CH>::NL)
                return;
        }
    }

    /*! Checks if the next character is the given one, throws otherwise.
    Reads whitespace (except newline). */
    void skip_to(CH ch)
    {
        skip_spaces();
        CH tmp;
        s_.get(tmp);
        if (ch == tmp)
            return;
        throw std::runtime_error("Unexpected character.");
    }

    /*! Reads the stream (skips whitespace except newline first) and stores as many characters as it can find into name.
    The allowed characters for name are alphanumeric (a-zA-z0-9) and also underscore (_) and dash (-). */
    void load_name(std::basic_string<CH, TR>& name)
    {
        name.clear();
        skip_spaces();
        while (s_.good())
        {
            CH ch = s_.peek();
            if (jj::str::isalnum(ch))
                ;
            else if (ch == jj::str::literals_t<CH>::UNDERSCORE || ch == jj::str::literals_t<CH>::MINUS)
                ;
            else
                break;
            s_.get(ch);
            name += ch;
        }
        if (name.empty())
            throw std::runtime_error("Empty name");
        skip_to(jj::str::literals_t<CH>::EQUAL);
    }

public:
    /*! Returns the internal stream reference.
    \warn To be only used in custom defined load methods. */
    std::basic_istream<CH, TR>& stream() { return s_; }

    typedef std::basic_string<CH, TR> string_type;
    typedef void (loader_type)(const string_type&, const string_type&);
    typedef std::map<string_type, std::function<loader_type>> loaders_type;

    /*! Returns a bool value read from stream.

    Checks if the stream contains any of the allowed bool values (0/1/true/false/yes/no).
    Throws otherwise. */
    bool load_logical()
    {
        if (!s_.good())
            throw std::runtime_error("Cannot read bool.");
        CH ch;
        s_.get(ch);
        if (ch == jj::str::literals_t<CH>::d0)
            return false;
        else if (ch == jj::str::literals_t<CH>::d1)
            return true;
        else if (jj::str::equali(ch, jj::str::literals_t<CH>::f))
        {
            check_bool_continuation(AUX::bool_texts_t<CH>::alse);
            return false;
        }
        else if (jj::str::equali(ch, jj::str::literals_t<CH>::t))
        {
            check_bool_continuation(AUX::bool_texts_t<CH>::rue);
            return true;
        }
        else if (jj::str::equali(ch, jj::str::literals_t<CH>::n))
        {
            check_bool_continuation(AUX::bool_texts_t<CH>::o);
            return false;
        }
        else if (jj::str::equali(ch, jj::str::literals_t<CH>::y))
        {
            check_bool_continuation(AUX::bool_texts_t<CH>::es);
            return true;
        }
        else
            throw std::runtime_error("Invalid bool value");
    }

    /*! Reads from stream an integer value. Treated as hexadecimal when starting with '\x'. */
    template<typename T>
    T load_numeric(T dflt)
    {
        T v = dflt;
        do
        {
            skip_spaces();
            CH ch = s_.peek();
            if (ch == jj::str::literals_t<CH>::BSLASH)
            {
                s_.get(ch);
                ch = s_.peek();
                if (ch != jj::str::literals_t<CH>::x)
                    break;
                s_.get(ch);
                s_ >> std::hex;
                bool r = (s_ >> v);
                s_ >> std::dec;
                if (!r)
                    v = dflt;
            }
            else
            {
                if (!(s_ >> v))
                    v = dflt;
            }
        } while (false);
        return v;
    }

    /*! Reads from stream a floating point value. */
    template<typename T>
    T load_fpoint(T dflt)
    {
        T v = dflt;
        if (!(s_ >> v))
            return dflt;
        return v;
    }
    /*! Reads from stream a text value. */
    void load_text(std::basic_string<CH, TR>& v)
    {
        v.clear();
        while (s_.good() && s_.peek() != jj::str::literals_t<CH>::NL)
        {
            CH ch;
            s_.get(ch);
            if (ch == jj::str::literals_t<CH>::BSLASH)
            {
                if (s_.good())
                {
                    ch = s_.peek();
                    if (ch == jj::str::literals_t<CH>::BSLASH)
                    {
                        s_.get(ch);
                        v += ch;
                    }
                    else if (ch == jj::str::literals_t<CH>::n)
                    {
                        s_.get(ch);
                        v += jj::str::literals_t<CH>::NL;
                    }
                    else
                        v += jj::str::literals_t<CH>::BSLASH;
                }
                else
                    v += jj::str::literals_t<CH>::BSLASH;
            }
            else
                v += ch;
        }
    }

    /*! Reads the top level info from a serizalized storage in a stream (ie. the initial "name=") */
    void load_root(string_type& name)
    {
        load_name(name);
    }
    /*! Takes given map of name->method and calls method everytime it encounters name as a field as it reads a serialized structure from the stream.
    Ignores unknown names (skips till end of line). */
    void load_structure(loaders_type loaders)
    {
        skip_to(jj::str::literals_t<CH>::LCB);
        while (s_.good())
        {
            skip_spaces();
            CH ch = s_.peek();
            if (ch == jj::str::literals_t<CH>::RCB)
            {
                skip_to(jj::str::literals_t<CH>::RCB);
                break;
            }
            else if (ch == jj::str::literals_t<CH>::NL)
            {
                skip_past_eol();
                continue; // retry searching name on next row
            }

            string_type name;
            load_name(name);
            typename loaders_type::const_iterator fnd = loaders.find(name);
            if (fnd == loaders.end() || fnd->second == nullptr)
            {
                skip_spaces();
                CH ch = s_.peek();
                if (ch == jj::str::literals_t<CH>::LCB)
                    throw std::runtime_error("Cannot skip structures."); // TODO stack automaton to skip
                skip_past_eol();
                // TODO log info
                continue;
            }
            (fnd->second)(jjT(""), name);
            skip_past_eol(); // TODO log warn if not at eol
        }

    }
};

/*! Reads a binary value from stream (recognizes 1,t,true,y,yes as true, 0,f,false,n,no as false (case insensitive)) and stores to given storage. */
template<typename CH, typename TR, bool DFLT>
void load(istreamStorage_base_T<CH, TR>& s, logical_t<DFLT>& x)
{
    x.set(s.load_logical());
}
/*! Reads an integer value from stream (allows hexadecimal if it starts with "\x") and stores to given storage. */
template<typename CH, typename TR, typename T, T DFLT>
void load(istreamStorage_base_T<CH, TR>& s, numeric_T<T, DFLT>& x)
{
    x.set(s.load_numeric(DFLT));
}
/*! Reads an integer value from stream (allows hexadecimal if it starts with "\x") and stores to given storage. */
template<typename CH, typename TR, typename T, T MIN, T MAX, T DFLT>
void load(istreamStorage_base_T<CH, TR>& s, numericRange_T<T, MIN, MAX, DFLT>& x)
{
    x.set(s.load_numeric(DFLT));
}
/*! Reads a floating point value from stream and stores to given storage. */
template<typename CH, typename TR, typename T>
void load(istreamStorage_base_T<CH, TR>& s, floatingPoint_t<T>& x)
{
    x.set(s.load_fpoint(x.getDefault()));
}
/*! Reads a floating point value from stream and stores to given storage. */
template<typename CH, typename TR, typename T>
void load(istreamStorage_base_T<CH, TR>& s, floatingPointRange_t<T>& x)
{
    x.set(s.load_fpoint(x.getDefault()));
}
/*! Reads a textual value from stream and stores to given storage (backslash has to be escaped as \\, newline as \n). */
template<typename CH, typename TR>
void load(istreamStorage_base_T<CH, TR>& s, text_base_T<CH, TR>& x)
{
    std::basic_string<CH, TR> tmp;
    s.load_text(tmp);
    x.set(tmp);
}

/*! Base class for stream based storage savers. */
template<typename CH, typename TR>
class ostreamStorage_base_T : public storage_base_t
{
    std::basic_ostream<CH, TR>& s_; //!< the actual stream being stored to
protected:
    /*! Constructor - takes given stream reference to operate on. */
    ostreamStorage_base_T(std::basic_ostream<CH, TR>& s) : s_(s) {}

    /*! Make sure that field name contains supported characters only. */
    inline bool check_name_internal(const CH* name)
    {
        while (*name != 0)
        {
            if (jj::str::isalnum(*name))
                ;
            else if (*name == jj::str::literals_t<CH>::UNDERSCORE || *name == jj::str::literals_t<CH>::MINUS)
                ;
            else
                return false;
            ++name;
        }
        return true;
    }
    /*! Make sure that field name is not empty and that it contains supported characters only. */
    inline void check_name(const CH* name)
    {
        if (name == nullptr || *name == 0 || !check_name_internal(name))
            throw std::runtime_error("Unsupported field name!");
    }

public:
    /*! Returns the internal stream reference.
    \warn To be only used in custom defined load methods. */
    std::basic_ostream<CH, TR>& stream() { return s_; }

    /*! Stores a bool value into the stream. */
    void save_logical(bool v)
    {
        s_ << (v ? str::literals_t<CH>::TRUE : str::literals_t<CH>::FALSE);
    }

    /*! Stores an integer value into the stream. */
    template<typename T>
    void save_numeric(T v)
    {
        s_ << v;
    }

    /*! Stores a floating point value into the stream. */
    template<typename T>
    void save_fpoint(T v)
    {
        s_ << std::setprecision(std::numeric_limits<T>::max_digits10) << v;
    }
    /*! Stores a textual value into the stream. Throws if control character other than newline or tab are encountered.
    Escapes (with a backslash) a backslash and newline character. */
    void save_text(const std::basic_string<CH, TR>& v)
    {
        for (CH ch : v)
        {
            if (ch == jj::str::literals_t<CH>::BSLASH)
                s_ << jj::str::literals_t<CH>::BSLASH << jj::str::literals_t<CH>::BSLASH;
            else if (ch == jj::str::literals_t<CH>::NL)
                s_ << jj::str::literals_t<CH>::BSLASH << jj::str::literals_t<CH>::n;
            else if (ch == jj::str::literals_t<CH>::TAB)
                s_ << ch;
            else if (jj::str::iscntrl(ch))
                throw std::runtime_error("Control characters are not supported.");
            else
                s_ << ch;
        }
    }

    /*! Wrapper to store structure members - stores given config value x under name.
    \note Path is currently ignored. */
    template<typename T>
    void save(const std::basic_string<CH, TR>& path, const std::basic_string<CH, TR>& name, const T& x)
    {
        save(path, name.c_str(), x);
    }
    /*! Wrapper to store structure members - stores given config value x under name.
    \note Path is currently ignored. */
    template<typename T>
    void save(const std::basic_string<CH, TR>& path, const CH* name, const T& x);

    /*! Writes the initial bit of the structure into the stream.
    \note This might need re-design in future (the containing structure should write the name=). */
    void begin_structure(const std::basic_string<CH, TR>& name)
    {
        check_name(name.c_str());
        s_ << name << str::literals_t<CH>::EQUAL << str::literals_t<CH>::LCB << str::literals_t<CH>::NL;
    }
    /*! Finishes the structure by writing the terminating sequence into the stream. */
    void end_structure()
    {
        s_ << str::literals_t<CH>::RCB << str::literals_t<CH>::NL;
    }
};

/*! Writes a binary value into stream. */
template<typename CH, typename TR, bool DFLT>
void save(ostreamStorage_base_T<CH, TR>& s, const logical_t<DFLT>& x)
{
    s.save_logical(x.get());
}
/*! Writes an integer value into stream. */
template<typename CH, typename TR, typename T, T DFLT>
void save(ostreamStorage_base_T<CH, TR>& s, const numeric_T<T, DFLT>& x)
{
    s.save_numeric(x.get());
}
/*! Writes a floating point value into stream. */
template<typename CH, typename TR, typename T>
void save(ostreamStorage_base_T<CH, TR>& s, const floatingPoint_t<T>& x)
{
    s.save_fpoint(x.get());
}
/*! Writes a textual value into stream. */
template<typename CH, typename TR>
void save(ostreamStorage_base_T<CH, TR>& s, const text_base_T<CH, TR>& x)
{
    s.save_text(x.get());
}

template<typename CH, typename TR>
template<typename T>
void ostreamStorage_base_T<CH, TR>::save(const std::basic_string<CH, TR>& path, const CH* name, const T& x)
{
    if (x.is_default())
        return; // don't bother saving default values
    check_name(name);
    s_ << name << str::literals_t<CH>::EQUAL;
    jj::conf::save(*this, x);
    s_ << str::literals_t<CH>::NL;
}


//=== specialized classes ============

/*! String stream storage source. */
template<typename CH, typename TR>
class isstreamStorage_T : public istreamStorage_base_T<CH, TR>
{
    std::basic_istringstream<CH, TR> stream_; //!< the actual string stream
public:
    /*! Constructor - takes the actual serialized value as parameter. */
    explicit isstreamStorage_T(const std::basic_string<CH, TR>& v) : istreamStorage_base_T<CH, TR>(stream_) { stream_.str(v); }
};

/*! System-preferred type of string stream storage. */
typedef isstreamStorage_T<jj::char_t, std::char_traits<jj::char_t>> isstreamStorage_t;

/*! String stream storage. */
template<typename CH, typename TR>
class osstreamStorage_T : public ostreamStorage_base_T<CH, TR>
{
    std::basic_ostringstream<CH, TR> stream_; //!< the actual string stream
public:
    /*! Constructor - creates an emtpy storage. */
    osstreamStorage_T() : ostreamStorage_base_T<CH, TR>(stream_) {}
    /*! Returns the stored (serialized) value. */
    std::basic_string<CH, TR> str() const { return stream_.str(); }
};

/*! System-preferred type of string stream storage. */
typedef osstreamStorage_T<jj::char_t, std::char_traits<jj::char_t>> osstreamStorage_t;

/*! File stream storage source. */
template<typename CH, typename TR>
class ifstreamStorage_T : public istreamStorage_base_T<CH, TR>
{
    std::basic_ifstream<CH, TR> stream_; //!< the actual file stream
public:
    /*! Constructor - default, use open() */
    ifstreamStorage_T() : istreamStorage_base_T<CH, TR>(stream_) {}
    /*! Constructor - opens given file in given mode */
    explicit ifstreamStorage_T(const CH* path, std::ios_base::openmode mode = std::ios_base::in) : istreamStorage_base_T<CH, TR>(stream_), stream_(path, mode) {}
    /*! Constructor - opens given file in given mode */
    explicit ifstreamStorage_T(const std::basic_string<CH, TR>& path, std::ios_base::openmode mode = std::ios_base::in) : istreamStorage_base_T<CH, TR>(stream_), stream_(path, mode) {}

    /*! Opens file */
    void open(const CH* path, std::ios_base::openmode mode = std::ios_base::in) { stream_.open(path, mode); }
    /*! Opens file */
    void open(const std::basic_string<CH, TR>& path, std::ios_base::openmode mode = std::ios_base::in) { stream_.open(path, mode); }
    /*! Returns if file is opened. */
    bool is_open() const { return stream_.is_open(); }
    /*! Closes the file. */
    void close() { stream_.close(); }
};

/*! System-preferred type of string stream storage. */
typedef ifstreamStorage_T<jj::char_t, std::char_traits<jj::char_t>> ifstreamStorage_t;

/*! File stream storage. */
template<typename CH, typename TR>
class ofstreamStorage_T : public ostreamStorage_base_T<CH, TR>
{
    std::basic_ofstream<CH, TR> stream_; //!< the actual file stream
public:
    /*! Constructor - default, use open() */
    ofstreamStorage_T() : ostreamStorage_base_T<CH, TR>(stream_) {}
    /*! Constructor - opens given file in given mode */
    explicit ofstreamStorage_T(const CH* path, std::ios_base::openmode mode = std::ios_base::out) : ostreamStorage_base_T<CH, TR>(stream_), stream_(path, mode) {}
    /*! Constructor - opens given file in given mode */
    explicit ofstreamStorage_T(const std::basic_string<CH, TR>& path, std::ios_base::openmode mode = std::ios_base::out) : ostreamStorage_base_T<CH, TR>(stream_), stream_(path, mode) {}

    /*! Opens file */
    void open(const CH* path, std::ios_base::openmode mode = std::ios_base::out) { stream_.open(path, mode); }
    /*! Opens file */
    void open(const std::basic_string<CH, TR>& path, std::ios_base::openmode mode = std::ios_base::out) { stream_.open(path, mode); }
    /*! Returns if file is opened. */
    bool is_open() const { return stream_.is_open(); }
    /*! Closes the file. */
    void close() { stream_.close(); }
};

/*! System-preferred type of string stream storage. */
typedef ofstreamStorage_T<jj::char_t, std::char_traits<jj::char_t>> ofstreamStorage_t;

} // namespace conf

} // namespace jj

#endif // JJ_CONFIGURATION_STORAGE_H
