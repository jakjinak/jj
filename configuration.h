#ifndef JJ_CONFIGURATION_H
#define JJ_CONFIGURATION_H

#include "jj/string.h"
#include <stdexcept>

namespace jj
{

namespace conf
{

//=== base classes ============

/*! Is a base class for something passed by value with a default value (specifiable in type). */
template<typename T, T DFLT>
struct value_base_T
{
    /*! Ctor. */
    value_base_T() : value_(DFLT) {}

    /*! Returns the value stored in object. */
    T get() const { return value_; }
    /*! Changes the value of the object. */
    void set(T v) { value_ = v; }
    /*! Changes the value of the object to the default value. */
    void reset() { value_ = DFLT; }
    /*! Returns whether the current value in the object is the default value. */
    bool is_default() const { return value_ == DFLT; }
    /*! Returns the default value. */
    T getDefault() const { return DFLT; }

protected:
    T value_; //!< stores the current value
};

/*! Is a base class for something passed by value with a default value (not specifiable in type). */
template<typename T>
struct value2_base_T
{
    /*! Ctor. */
    value2_base_T(T v = T()) : value_(v), dflt_(v) {}

    /*! Returns the value stored in object. */
    T get() const { return value_; }
    /*! Changes the value of the object. */
    void set(T v) { value_ = v; }
    /*! Changes the value of the object to the default value. */
    void reset() { value_ = dflt_; }
    /*! Returns whether the current value in the object is the default value. */
    bool is_default() const { return value_ == dflt_; }
    /*! Returns the default value. */
    T getDefault() const { return dflt_; }

protected:
    T value_; //!< stores the current value
    const T dflt_; //!< stores the default value
};

/*! Is a base class for something passed by reference with a default value not specifiable in type. */
template<typename T>
struct valueRef_base_T
{
    /*! Default ctor. */
    valueRef_base_T() : value_(), dflt_() {}
    /*! Ctor. */
    valueRef_base_T(const T& v) : value_(v), dflt_(v) {}

    /*! Returns the value stored in object. */
    const T& get() const { return value_; }
    /*! Changes the value of the object. */
    void set(const T& v) { value_ = v; }
    /*! Changes the value of the object to the default value. */
    void reset() { return value_ = dflt_; }
    /*! Returns whether the current value in the object is the default value. */
    bool is_default() const { return value_ == dflt_; }
    /*! Returns the default value. */
    const T& getDefault() const { return dflt_; }

protected:
    T value_; //!< stores the current value
    const T dflt_; //!< stores the default value
};

//=== actual classes ============

/*! Represents a binary information true/false. */
template<bool DFLT = false> struct logical_t : public value_base_T<bool, DFLT> {};
/*! Represents any integer type information. */
template<typename T, T DFLT = T()> struct numeric_T : public value_base_T<T, DFLT> {};
/*! Represents int type information. */
template<int DFLT = 0> struct integer_t : public numeric_T<int, DFLT> {};
/*! Represents any integer type information, but allowed range is limited. */
template<typename T, T MIN, T MAX, T DFLT>
struct numericRange_T : public numeric_T<T, DFLT>
{
    /*! Just like regular set in parent class, but checks if new value fits into the allowed range.
    Value is changed only if it fits into the range.
    \return Whether value fits into range. */
    bool tryset(T v)
    {
        if (v < MIN || v > MAX)
            return false;
        numeric_T<T, DFLT>::value_ = v;
        return true;
    }
    /*! Just like regular set in parent class, but checks if new value fits into the allowed range.
    Throws out_of_range if value does not fit into the range (and does not change the value). */
    void set(T v)
    {
        if (v < MIN || v > MAX)
            throw std::out_of_range(jj::strcvt::to_string(jjS(jjT("Value ") << v << jjT(" is out of allowed range <") << MIN << jjT(',') << MAX << jjT(">."))));
        numeric_T<T, DFLT>::value_ = v;
    }
};

/*! Represents any floating point type information. */
template<typename T = double> struct floatingPoint_t : public value2_base_T<T>
{
    /*! Constructor */
    floatingPoint_t(T v = T()) : value2_base_T<T>(v) {}
};
/*! Represents any floating point type information, but allowed range is limited. */
template<typename T = double> struct floatingPointRange_t : public floatingPoint_t<T>
{
    /*! Constructor */
    floatingPointRange_t(T min, T max, T v = T()) : floatingPoint_t<T>(v), min_(min), max_(max) {}

    /*! Just like regular set in parent class, but checks if new value fits into the allowed range.
    Value is changed only if it fits into the range.
    \return Whether value fits into range. */
    bool tryset(T v)
    {
        if (v < min_ || v > max_)
            return false;
        floatingPoint_t<T>::value_ = v;
        return true;
    }
    /*! Just like regular set in parent class, but checks if new value fits into the allowed range.
    Throws out_of_range if value does not fit into the range (and does not change the value). */
    void set(T v)
    {
        if (v < min_ || v > max_)
            throw std::out_of_range(jj::strcvt::to_string(jjS(jjT("Value ") << v << jjT(" is out of allowed range <") << min_ << jjT(',') << max_ << jjT(">."))));
        floatingPoint_t<T>::value_ = v;
    }
private:
    const T min_; //!< holds minimum allowed value
    const T max_; //!< holds maximum allowed value
};

/*! Represents a textual information. */
template<typename CH, typename TR>
struct text_base_T : valueRef_base_T<std::basic_string<CH,TR>>
{
    /*! Default constructor */
    text_base_T() {}
    /*! Constructor */
    text_base_T(const std::basic_string<CH, TR>& v) : valueRef_base_T<std::basic_string<CH,TR>>(v) {}
};

/*! Represents a textual information in the system-preferred character width. */
typedef text_base_T<jj::char_t, std::char_traits<jj::char_t>> text_t;

} // namespace conf

} // namespace jj

#endif // JJ_CONFIGURATION_H
