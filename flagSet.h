#ifndef JJ_FLAGSET_H
#define JJ_FLAGSET_H

#include <bitset>

namespace jj
{

/*! Provides a facility to effectively work with a set of indipendent binary flags.
Ideally initialized by initializer list in constructor and checked by * operator.
The template parameters provide an enum type in which the flag names are defined,
the COUNT is a sentinel - assumed to be the last item (not counted into the flags) in the enum.
\note Wrapper over std::bitset. */
template<typename T, T COUNT>
class flagSet_t
{
    std::bitset<size_t(COUNT)> bits_; //!< internal storage
public:
    /*! Constructor - initializes to no flag set. */
    flagSet_t() {}
    /*! Constructor - initializes flags given in parameter to set, remaining to not set. */
    flagSet_t(std::initializer_list<T> init) { for (T i : init) bits_.set(i); }

    /*! Returns whether given flag is set. */
    bool test(T v) const { return bits_.test(size_t(v)); }
    /*! Sets given flag. */
    void set(T v) { bits_.set(size_t(v)); }
    /*! Unsets given flag. */
    void reset(T v) { bits_.reset(size_t(v)); }
    /* Toggles given flag. */
    void flip(T v) { bits_.flip(size_t(v)); }
    /* Toggles given flag. */
    void toggle(T v) { flip(v); }

    /* Sets given flag in current instance. */
    flagSet_t& operator+=(T v) { set(v); return *this; }
    /* Sets given flag in current instance. */
    flagSet_t& operator|=(T v) { set(v); return *this; }

    /* Unsets given flag in current instance. */
    flagSet_t& operator-=(T v) { reset(v); return *this; }
    /* Unsets given flag in current instance. */
    flagSet_t& operator&=(T v) { reset(v); return *this; }

    /* Toggles given flag in current instance. */
    flagSet_t& operator^=(T v) { flip(v); return *this; }

    /* Returns a new instance with all flags in the original instance and the given flag set. */
    friend flagSet_t operator+(flagSet_t x, T v) { flagSet_t n(x); n+=v; return n; }
    /* Returns a new instance with all flags in the original instance and the given flag set. */
    friend flagSet_t operator|(flagSet_t x, T v) { flagSet_t n(x); n |= v; return n; }
    /* Returns a new instance with all flags in the original instance set except for the given flag. */
    friend flagSet_t operator-(flagSet_t x, T v) { flagSet_t n(x); n -= v; return n; }
    /* Returns a new instance with all flags in the original instance set except for the given flag. */
    friend flagSet_t operator&(flagSet_t x, T v) { flagSet_t n(x); n &= v; return n; }
    /* Returns a new instance with flags as in the original instance, but the given flag toggled. */
    friend flagSet_t operator^(flagSet_t x, T v) { flagSet_t n(x); n ^= v; return n; }

    /*! Returns whether given flag is set. */
    bool operator*(T v) const { return test(v); }
};

} // namespace jj

#endif // JJ_FLAGSET_H
