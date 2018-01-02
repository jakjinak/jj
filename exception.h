#include <stdexcept>

namespace jj
{

/*! A simple wrapper over logic error. */
struct not_implemented_t : public std::logic_error
{
    not_implemented_t() : std::logic_error("Not implemented!") {}
};

} // namespace jj
