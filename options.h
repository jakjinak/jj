#ifndef JJ_OPTIONS_H
#define JJ_OPTIONS_H

#include "jj/flagSet.h"

namespace jj
{
namespace opt
{
/*! Provides "one of" abstraction on top of enum values given as (enum given as T). */
template<typename T> struct e { T Value; e() : Value() {} e(T v) : Value(v) {} };
/*! Provides "any of" abstraction on top of enum values given as (enum given as T). Effictively it is an vector<bool> inside. */
template<typename T, T COUNT> struct f : jj::flagSet_t<T, COUNT> { f() : flagSet_t<T, COUNT>() {} f(std::initializer_list<T> init) : flagSet_t<T, COUNT>(init) {} };
} // namespace opt

/*! Abstracts a set of different types and provides the << operator to set all of them inline.
As it is written there can be only one value per type in an instance!

Usual usage is one of:
  - make an instance of this as public member
  - pass an instance of this into ctor and provide a static member with the defaults
Note that there are opt::e and opt::f helpers for "one of"/"any of".
  
Eg. like this:
class myClass_t
{ ...
  typedef options_t<opt::pos, opt::size, myoption1, opt::e<myenum>, opt::f<myenum2> options_t;
  static options_t options() { return options_t() << myenum::value << opt::size(100,300) << myenum2::value; }
  ...
}; */
template<typename ... Ts>
class options_t : public Ts...
{
    template<typename T, typename S, typename ... Ss>
    struct SEARCHER : SEARCHER<T, Ss...>
    {
        // just forward to next type
    };
    // regular type specialization
    template<typename T, typename ... Ss>
    struct SEARCHER<T, T, Ss...>
    {
        typedef T type;
        static void adopt(type& t, const T v) { t = v; }
    };
    // "one of" specialization
    template<typename T, typename ... Ss>
    struct SEARCHER<T, opt::e<T>, Ss...>
    {
        typedef opt::e<T> type;
        static void adopt(type& t, const T v) { t = v; }
    };
    // "any of" specialization
    template<typename T, T COUNT, typename ... Ss>
    struct SEARCHER<T, opt::f<T, COUNT>, Ss...>
    {
        typedef opt::f<T, COUNT> type;
        static void adopt(type& t, const T v) { t |= v; }
    };
    // the sentinel if no match was found in the type list
    template<typename T>
    struct SEARCHER<T, void> : T::noMatchingTypeFoundInTemplateParameterList
    {
    };

public:
    /*! Sets value of given type.
    Note that for opt::f the flags can only be set, not unset. For other types any new value simply overwrites the previous one.
    */
    template<typename T>
    options_t& operator<<(const T v)
    {
        typename SEARCHER<T, Ts..., void>::type& t = *this;
        SEARCHER<T, Ts..., void>::adopt(t, v);
        return *this;
    }
};

} // namespace jj

#endif // !JJ_OPTIONS_H

