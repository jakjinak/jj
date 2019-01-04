#ifndef JJ_EXCEPTION_H
#define JJ_EXCEPTION_H

#include <stdexcept>

#include "jj/stream.h"

namespace jj
{

namespace exception
{

/*! Base class of all exceptions. */
class base : public std::exception
{
    std::string what_; //!< exception message

public:
    /*! Ctor - just stores the message given by txt. */
    base(const std::string& txt)
        : what_(txt)
    {
    }
    /*! Returns the stored message. */
    const char* what() const noexcept
    {
        return what_.c_str();
    }
};

/*! A simple wrapper over logic error. */
struct not_implemented : public std::logic_error
{
    not_implemented() : std::logic_error("Not implemented!") {}
};

} // namespace exception
} // namespace jj

#endif // JJ_EXCEPTION_H
