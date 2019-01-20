#ifndef JJ_PROPS_H
#define JJ_PROPS_H

#include "jj/string.h"
#include "jj/stringLiterals.h"
#include "jj/stream.h"
#include "jj/exception.h"
#include <sstream>
#include <map>
#include <list>

namespace jj
{
namespace props
{

namespace setup
{
/*! Allows defining props based on a const char_type* where the
key is compared case sensitive. */
template<typename CHAR = jj::char_t>
struct cstrkey_t
{
    typedef const CHAR* key_type;
    typedef jj::str::lessPred<key_type> comp_type;
};

/*! Allows defining props based on a const char_type* where the
key is compared case insensitive. */
template<typename CHAR = jj::char_t>
struct icstrkey_t
{
    typedef const CHAR* key_type;
    typedef jj::str::lessiPred<key_type> comp_type;
};

} // namespace setup

namespace conv
{
/*! Used to convert keys to (narrow) string values when passing to exceptions. */
inline const char* key2string(const char* key) { return key; }
/*! Used to convert keys to (narrow) string values when passing to exceptions. */
inline std::string key2string(const wchar_t* key) { return jj::strcvt::to_string(key); }

/* Convertor used in pathWalker_t. */
struct string2cstr_t
{
    typedef const jj::char_t* key_type;
    key_type convert(const jj::string_t& key) const { return key.c_str(); }
};
} // namespace conv

namespace exception
{
/*! Exception thrown when adding key to props that already exist. */
class duplicateKey : public jj::exception::base
{
public:
    template<typename KEY>
    duplicateKey(KEY key)
        : base(jjS2(std::ostringstream, "Duplicate key. Key [" << conv::key2string(key) << "] already exists."))
    {
    }
};

/*! Exception thrown when requesting key from props that is not there. */
class keyNotFound : public jj::exception::base
{
public:
    template<typename KEY>
    keyNotFound(KEY key)
        : base(jjS2(std::ostringstream, "Key not found. No [" << conv::key2string(key) << "] exists."))
    {
    }
};

} // namespace exception

// forward declare
template<typename SETUP, typename ... Ts>
class props;

/*! Helper class allowing to track the progress while iterating through props. */
template<typename KEY>
struct traversalContext_t
{
    std::list<KEY> Dive; //!< holds all the nested props that have already been visited
};

namespace aux
{
/*! Helper to auto-add const to T if X is const. */
template<typename X, typename T>
struct constnessHandler_t
{
    typedef T TYPE;
};
template<typename X, typename T>
struct constnessHandler_t<const X, T>
{
    typedef const T TYPE;
};

/*! The real values are taken by reference into the props. This class allows the storage of the references. */
template<typename T>
class propwrap
{
    T* prop_; //!< the actual prop value
public:
    /*! Ctor - just for the map container, shall never be used directly. */
    propwrap() : prop_(nullptr) {}
    /*! Ctor - the actual one to be used. Pass the reference to the actual variable that needs to be managed by props. */
    explicit propwrap(T& p) : prop_(&p) {}

    /*! Assign operator - just for the map container, shall never be used directly. */
    propwrap& operator=(T& p) { prop_ = &p; return *this; }

    /*! Returns the original value reference. */
    const T& get() const { return *prop_; }
    /*! Returns the original value reference. */
    T& get() { return *prop_; }
};

/*! Used when iterating nested props - automatically modifies the traversalContext. */
template<typename KEY>
struct traversalContextDiver_t
{
    /*! Ctor - inserts level key to ctx. */
    traversalContextDiver_t(traversalContext_t<KEY>& ctx, KEY key) : ctx_(ctx) { ctx_.Dive.push_back(key); }
    /*! Dtor - auto-pops the key from ctx. */
    ~traversalContextDiver_t() { ctx_.Dive.pop_back(); }
private:
    traversalContext_t<KEY>& ctx_; //!< the actual context tracking the depth
};

/*! Splits a textual path by separators (by next() calls). */
template<typename CH = jj::char_t, CH SEP = jj::str::literals_t<CH>::SLASH>
class pathIterator_t
{
public:
    typedef CH char_type; //!< characters in string
    typedef std::basic_string<char_type> key_type; //!< input string type (and type of keys as in path)

private:
    const key_type& in_; //!< the input path string
    typename key_type::const_iterator pos_; //!< current position in the input (changes with next() calls)

public:
    /*! Ctor - takes a reference of the input. */
    pathIterator_t(const key_type& input)
        : in_(input), pos_(input.begin())
    {
    }

    /*! Reads the next segment of path and sets it as output parameter.
    Returns true if this is the last segment, false otherwise.
    Once true is returned this method shall not be called any more! */
    bool next(key_type& nextKey)
    {
        typename key_type::const_iterator end = in_.end();
        if (pos_ == end)
            return true;
        typename key_type::const_iterator start = pos_;
        while (pos_ != end && *pos_ != SEP)
            ++pos_;
        nextKey = key_type(start, pos_);
        if (pos_ == end)
            return true; // at the end
        else
        {
            ++pos_; // skip past the separator
            return false; // not at the end
        }
    }
};

/*! Holds values of one type (actually the propwraps). */
template<typename SETUP, typename T>
class holder_t
{
    typedef typename SETUP::key_type key_type; //!< key of the internal container
    typedef typename SETUP::comp_type comp_type; //!< predicate of the internal container (comparing the keys)
    typedef T value_type; //!< type of the actual values
    typedef propwrap<value_type> wrap_type; //!< helper type (actually stored in the container) wrapping actual value

    typedef std::map<key_type, wrap_type, comp_type> holder_type; //!< container type holding all the values
    holder_type props_; //!< container holding all the values

protected:
    /*! Inserts a new value to be managed by props to the container.
    Throws duplicateKey if key already exists in the container (duplicates not allowed). */
    void addProp(key_type key, value_type& prop)
    {
        auto ret = props_.insert(typename holder_type::value_type(key, wrap_type(prop)));
        if (!ret.second)
            throw exception::duplicateKey(key);
    }

public:
    /*! Iterates through all values calling the visitor for each of them. */
    template<typename VISITOR, typename CTX>
    void traverse(VISITOR& visitor, CTX& ctx) const
    {
        for (auto& x : props_)
            visitor.onValue(ctx, x.first, x.second.get());
    }

    /*! Searches the container and returns value stored under given key,
    nullptr if no such key exists. */
    const value_type* find(key_type key) const
    {
        typename holder_type::const_iterator it = props_.find(key);
        if (it == props_.end())
            return nullptr;
        return &(it->second.get());
    }
    /*! Searches the container and returns value stored under given key,
    nullptr if no such key exists. */
    value_type* find(key_type key)
    {
        typename holder_type::iterator it = props_.find(key);
        if (it == props_.end())
            return nullptr;
        return &(it->second.get());
    }
    /*! Searches the container and returns value stored under given key.
    Throws keyNotFound if no such key exists. */
    const value_type& get(key_type key) const
    {
        typename holder_type::const_iterator it = props_.find(key);
        if (it == props_.end())
            throw exception::keyNotFound(key);
        return it->second.get();
    }
    /*! Searches the container and returns value stored under given key.
    Throws keyNotFound if no such key exists. */
    value_type& get(key_type key)
    {
        typename holder_type::iterator it = props_.find(key);
        if (it == props_.end())
            throw exception::keyNotFound(key);
        return it->second.get();
    }

    /*! Searches the container and updates the value under given key.
    Throws keyNotFound if no such key exists. */
    void set(key_type key, const T& v)
    {
        typename holder_type::iterator it = props_.find(key);
        if (it == props_.end())
            throw exception::keyNotFound(key);
        it->second.get() = v;
    }
};

/*! Puts together the holders of individual types in the props.
This generic specialization takes the first type, makes a holder of it and descends recursively to the remaining types.
Note: It is possible to add same key but different type, props won't prohibit this. */
template<typename SETUP, typename T, typename ... Ts>
class typelist_t : public holder_t<SETUP, T>, public typelist_t<SETUP, Ts...>
{
    typedef holder_t<SETUP, T> prop_type; //!< holds values of the first type
    typedef typelist_t<SETUP, Ts...> next_type; //!< the remaining types

protected:
    // import methods from nested classes
    using prop_type::addProp;
    using next_type::addProp;

public:
    /*! Iterates through all values calling the visitor for each of them (for this and all the remaining types). */
    template<typename VISITOR, typename CTX>
    void traverse(VISITOR& str, CTX& ctx)
    {
        prop_type::traverse(str, ctx);
        next_type::traverse(str, ctx);
    }

    // import methods from nested classes
    using prop_type::set;
    using next_type::set;

    /*! Calls action for each type in the typelist. */
    template<typename ACTION>
    void apply(ACTION& a) const
    {
        const prop_type& p = *this;
        a(p);
        next_type::apply(a);
    }
    /*! Calls action for each type in the typelist. */
    template<typename ACTION>
    void apply(ACTION& a)
    {
        prop_type& p = *this;
        a(p);
        next_type::apply(a);
    }
    /*! Calls action for key in each type in the typelist. */
    template<typename ACTION>
    void apply(ACTION& a, typename SETUP::key_type key) const
    {
        const prop_type& ph = *this;
        const T* p = ph.find(key);
        if (p != nullptr)
            a(*p);
        next_type::apply(a, key);
    }
    /*! Calls action for key in each type in the typelist. */
    template<typename ACTION>
    void apply(ACTION& a, typename SETUP::key_type key)
    {
        prop_type& ph = *this;
        T* p = ph.find(key);
        if (p != nullptr)
            a(*p);
        next_type::apply(a, key);
    }
};

/*! Sentinel. This specialization is used when the last type is encountered. */
template<typename SETUP, typename T>
class typelist_t<SETUP, T, void> : public holder_t<SETUP, T>
{
    typedef holder_t<SETUP, T> prop_type; //!< holds values of the first type
protected:
    // import methods from nested classes
    using prop_type::addProp;
public:
    // import methods from nested classes
    using prop_type::traverse;
    using prop_type::set;

    /*! Calls action for each type in the typelist. */
    template<typename ACTION>
    void apply(ACTION& a) const
    {
        const prop_type& p = *this;
        a(p);
    }
    /*! Calls action for each type in the typelist. */
    template<typename ACTION>
    void apply(ACTION& a)
    {
        prop_type& p = *this;
        a(p);
    }
    /*! Calls action for key in each type in the typelist. */
    template<typename ACTION>
    void apply(ACTION& a, typename SETUP::key_type key) const
    {
        const prop_type& ph = *this;
        const T* p = ph.find(key);
        if (p != nullptr)
            a(*p);
    }
    /*! Calls action for key in each type in the typelist. */
    template<typename ACTION>
    void apply(ACTION& a, typename SETUP::key_type key)
    {
        prop_type& ph = *this;
        T* p = ph.find(key);
        if (p != nullptr)
            a(*p);
    }
};

/*! Used for nesting individual props classes.
The individual "childs" are stored in a container under keys just like real values. */
template<typename SETUP, typename ... Ts>
class nested_t
{
    typedef typename SETUP::key_type key_type; //!< key of the internal container
    typedef typename SETUP::comp_type comp_type; //!< predicate of the internal container (comparing the keys)
    typedef props<SETUP, Ts...> value_type; //!< the value of the container - unlike real values when nesting the type is props itself
    typedef propwrap<value_type> wrap_type; //!< helper type (actually stored in the container) wrapping actual props

    typedef std::map<key_type, wrap_type, comp_type> holder_type; //!< container type holding all the values
    holder_type props_; //!< container holding all the nested props

protected:
    /*! Inserts a new "child" props to the container.
    Throws duplicateKey if key already exists in the container (duplicates not allowed). */
    void addNested(key_type key, value_type& prop)
    {
        auto ret = props_.insert(typename holder_type::value_type(key, wrap_type(prop)));
        if (!ret.second)
            throw exception::duplicateKey(key);
    }

public:
    /*! Iterates through all nested value calling the visitor for each of them (it will recurse for each of them). */
    template<typename VISITOR, typename CTX>
    void traverse(VISITOR& str, CTX& ctx)
    {
        for (auto& i : props_)
        {
            traversalContextDiver_t<key_type> dive(ctx, i.first);
            str.onNestedBegin(ctx);
            i.second.get().traverse(str, ctx);
            str.onNestedEnd(ctx);
        }
    }

    /*! Searches the container and returns the props stored under given key or
    returns nullptr if no such key exists. */
    const value_type* findNested(key_type key) const
    {
        typename holder_type::const_iterator fnd = props_.find(key);
        if (fnd == props_.end())
            return nullptr;
        return &(fnd->second.get());
    }
    /*! Searches the container and returns the props stored under given key or
    returns nullptr if no such key exists. */
    value_type* findNested(key_type key)
    {
        typename holder_type::iterator fnd = props_.find(key);
        if (fnd == props_.end())
            return nullptr;
        return &(fnd->second.get());
    }
    /*! Searches the container and returns the props stored under given key.
    Throws keyNotFound if no such key exists. */
    const value_type& getNested(key_type key) const
    {
        typename holder_type::const_iterator fnd = props_.find(key);
        if (fnd == props_.end())
            throw exception::keyNotFound(key);
        return fnd->second.get();
    }
    /*! Searches the container and returns the props stored under given key.
    Throws keyNotFound if no such key exists. */
    value_type& getNested(key_type key)
    {
        typename holder_type::iterator fnd = props_.find(key);
        if (fnd == props_.end())
            throw exception::keyNotFound(key);
        return fnd->second.get();
    }
};

} // namespace aux

/*! An abstraction to help generalize structures holding values (such as configuration or so).
Derive your class from this (passing all required types as template parameters) and call addProp()
for each of the values to register it. Use addNested() to recurse into substructures
(also derived from this, same type as parent).
Later you can access the values in a generic way through get(), set() or getNested().
Or use any available infrastructure like props::pathWalker_t, textSerializer_t or textDeserializer_t.
Or define your own.
Note that it is possible to insert multiple same keys if each is for different value type (or value and nested),
props will not prohibit this. This will however lead to UNDEFINED BEHAVIOR with the textDeserializer_t!

The SETUP template parameter has to fullfil: 
* key_type - the type of the key in the map holding the props
* comp_type - the type defining the predicate for comparing keys
The predefined setup types (setup::cstrkey_t, setup::icstrkey_t) are using const char_t* as key - either 
case sensitive or insensitive.

Example:
using namespace jj::props;
typedef props<setup::icstrkey_t, int, jj::string_t> myprops;

struct nested : myprops
{
    int value;
    string_t details;

    nested()
    {
        addProp(jjT("value"), value);
        addProp(jjT("details"), details);
    }
};

struct master : myprops
{
    string_t title;
    nested A, B;
    master()
    {
        addProp(jjT("title"), title);
        addNested(jjT("A"), A);
        addNested(jjT("B"), B);
    }
};

int main()
{
    master X;
    X.A.value = 15;

    jj::ofstream_t f(...);
    text_serializer<jj::ofstream_t> ts(f);
    ts << X;

    // or 

    jj::ifstream_t f(...);
    text_deserializer<jj::ifstream_t> td(f);
    ts >> X;
} */
template<typename SETUP, typename ... Ts>
class props : public aux::typelist_t<SETUP, Ts..., void>, public aux::nested_t<SETUP, Ts...>
{
public:
    typedef SETUP setup_type; //!< describes the properties of the props
    typedef aux::typelist_t<SETUP, Ts..., void> list_type; //!< represents all the types available in props

private:
    typedef aux::nested_t<SETUP, Ts...> nested_type; //!< represents the holder of any "children"
    props(const props&); // disabled copy ctor
    props& operator=(const props&); // disable
protected:
    // import methods from nested classes
    // TODO remove these rows
    using list_type::addProp;
    using nested_type::addNested;

public:
    /*! Ctor */
    props() {}

    /*! Iterates through all values (and nested props and their values) calling the visitor for each of them (it will recurse for each of them).
    Note that the visitor has to fullful the following:
    * have onNestedBegin(CTX&) and onNestedEnd(CTX&) methods
    * have on_value(CTX&, SETUP::key_type, T&) for each T in the props */
    template<typename VISITOR, typename CTX>
    void traverse(VISITOR& str, CTX& ctx)
    {
        list_type::traverse(str, ctx);
        nested_type::traverse(str, ctx);
    }

    /*! Searches the container of given type T (one of those in props) for key and returns the associated value
    or nullptr if no such key exists. */
    template<typename T>
    const T* find(typename setup_type::key_type key) const
    {
        const aux::holder_t<setup_type, T>& tmp = *this;
        return tmp.find(key);
    }
    /*! Searches the container of given type T (one of those in props) for key and returns the associated value
    or nullptr if no such key exists. */
    template<typename T>
    T* find(typename setup_type::key_type key)
    {
        aux::holder_t<setup_type, T>& tmp = *this;
        return tmp.find(key);
    }
    /*! Searches the container of given type T (one of those in props) for key and returns the associated value.
    Throws keyNotFound if no such key exists. */
    template<typename T>
    const T& get(typename setup_type::key_type key) const
    {
        const aux::holder_t<setup_type, T>& tmp = *this;
        return tmp.get(key);
    }
    /*! Searches the container of given type T (one of those in props) for key and returns the associated value.
    Throws keyNotFound if no such key exists. */
    template<typename T>
    T& get(typename setup_type::key_type key)
    {
        aux::holder_t<setup_type, T>& tmp = *this;
        return tmp.get(key);
    }

    /*! Searches the container of given type T (one of those in props) for key and updates the associated value to v. */
    template<typename T>
    void set(typename setup_type::key_type key, const T& v)
    {
        aux::holder_t<setup_type, T>& tmp = *this;
        return tmp.set(key, v);
    }

    // import methods from nested classes
    using list_type::set;

    /*! Calls action for each type in the props (but not nested props).
    The ACTION has to fullfil:
    * operator()(const jj::props::aux::holder_t<SETUP,T>&) for each of the types
    - it is assumed that holder_t will not be named directly */
    template<typename ACTION>
    void apply(ACTION& a) const
    {
        list_type::apply(a);
    }
    /*! Calls action for each type in the props (but not nested props).
    The ACTION has to fullfil:
    * operator()(jj::props::aux::holder_t<SETUP,T>&) for each of the types
    - it is assumed that holder_t will not be named directly */
    template<typename ACTION>
    void apply(ACTION& a)
    {
        list_type::apply(a);
    }
    /*! Calls action for key of each type in the props (but not nested props) passing
    the value under the key if the key exists. Does not do anything for types where key is not present.
    The ACTION has to fullfil:
    * operator()(T&) for each of the types */
    template<typename ACTION>
    void apply(ACTION& a, typename SETUP::key_type key) const
    {
        list_type::apply(a, key);
    }
    /*! Calls action for key of each type in the props (but not nested props) passing
    the value under the key if the key exists. Does not do anything for types where key is not present.
    The ACTION has to fullfil:
    * operator()(T&) for each of the types */
    template<typename ACTION>
    void apply(ACTION& a, typename SETUP::key_type key)
    {
        list_type::apply(a, key);
    }
};

/*! Wrapper over props to enable search the hiararchy of props.

The SPLITTER is used to get individual segments of the path and has to fullfil:
- takes input path as ctor parameter
- has bool next() method, see pathIterator_t::next()

The CONVERTER converts keys (=path segments) as returned by SPLITTER to the actual key type as used in PROPS. It has to fullfil:
* have method PROPS::SETUP::key_type convert(SPLITTER::key_type) */
template<typename PROPS, typename SPLITTER = aux::pathIterator_t<>, typename CONVERTER = conv::string2cstr_t>
class pathWalker_t
{
    typedef typename SPLITTER::key_type source_key_type; //!< input type, also key type as read from input
    typedef typename CONVERTER::key_type key_type; //!< actual key type as used in PROPS

    PROPS& top_; //!< these are searched by path
    CONVERTER conv_; //!< key convertor

public:
    /*! Ctor */
    pathWalker_t(PROPS& top)
        : top_(top)
    {
    }

    /*! Searches props given to ctor if it contains the path, the out will contain the last segment
    of the path (=the key of the actual value), props representing the path (except the last segment)
    is returned. Throws keyNotFound if any path segment cannot be found. */
    PROPS& descend(const source_key_type& path, source_key_type& out)
    {
        SPLITTER s(path);
        PROPS* cp = &top_;
        while (!s.next(out))
            cp = &cp->getNested(conv_.convert(out));
        return *cp;
    }

    /*! Combines the descend() and PROPS::get<T>. Returns actual value reference or throws keyNotFound
    if any path segment could not be found. */
    template<typename T>
    typename aux::constnessHandler_t<PROPS, T>::TYPE& get(const source_key_type& path)
    {
        source_key_type finalKey{};
        PROPS& p = descend(path, finalKey);
        return p.template get<T>(conv_.convert(finalKey));
    }

    /*! Combines the descend() and PROPS::set<T>. Updates value at path or throws keyNotFound
    if any path segment could not be found. */
    template<typename T>
    void set(const source_key_type& path, const T& v)
    {
        source_key_type finalKey{};
        PROPS& p = descend(path, finalKey);
        p.template set<T>(conv_.convert(finalKey), v);
    }

    /*! Combines the descend() and PROPS::apply. Applies action on value by path or throws keyNotFound
    if any path segment could not be found. */
    template<typename ACTION>
    void apply(ACTION& a, const source_key_type& path)
    {
        source_key_type finalKey{};
        PROPS& p = descend(path, finalKey);
        p.apply(a, conv_.convert(finalKey));
    }

};

} // namespace props
} // namespace jj

#endif // JJ_PROPS_H
