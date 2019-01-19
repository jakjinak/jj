#ifndef JJ_SINGLETON_H
#define JJ_SINGLETON_H

namespace jj
{

struct basicSingletonPolicy_t {};

/*! Provides the implementation of the singleton design pattern.
The exact behavior depends on the POLICY. */
template<typename T, typename POLICY = basicSingletonPolicy_t>
class singleton_t
{
    // this is the basicSingletonPolicy_t specialization
public:
    /*! Returns the instance. */
    static T& instance()
    {
        static T inst;
        return inst;
    }
};

} // namespace jj

#endif // JJ_SINGLETON_H
