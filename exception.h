#ifndef JJ_EXCEPTION_H
#define JJ_EXCEPTION_H

#include <stdexcept>

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

/*! Thrown in a method that is yet to be implemented. */
struct not_implemented : public std::logic_error
{
    /*! Ctor */
    not_implemented() : std::logic_error("Not implemented!") {}
};

} // namespace exception
} // namespace jj

#endif // JJ_EXCEPTION_H
