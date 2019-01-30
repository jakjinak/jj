#ifndef JJ_EXCEPTION_TYPES_H
#define JJ_EXCEPTION_TYPES_H

#include "jj/exception.h"
#include "jj/stream.h"
#include <sstream>

namespace jj
{

namespace exception
{

/*! Thrown with an invalid index or other similar input parameter. */
template<typename T>
class out_of_range : public base
{
public:
    /*! Ctor - takes only the invalid value. */
    out_of_range(T index) : base(jjS2(std::ostringstream, "Index [" << index << "] out of range.")) {}
    /*! Ctor - takes the invalid value and the upper bound (exclusive) to the range.
    Note that lower bound is assumed to be 0 (inclusive). */
    out_of_range(T index, T max) : base(jjS2(std::ostringstream, "Index [" << index << "] out of range, maximum index is one less than [" << max << "]")) {}
};

} // namespace exception

} // namespace jj

#endif // JJ_EXCEPTION_TYPES_H
