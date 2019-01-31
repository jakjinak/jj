#ifndef JJ_FUNCTION_BAG_H
#define JJ_FUNCTION_BAG_H

#include <functional>
#include <vector>
#include <map>
#include "jj/idGenerator.h"

namespace jj
{

/*! Base class for the functionBag_t, do not instantiate directly.
Allows to store functions, methods, functors... inside and invoke.

Note that every such function registered through add() is assigned an unique id, the same
function can be added multiple times receiving different id every time. The idGenerator_t
is used to generate the ids.

Provides FirstAdded and LastRemoved which (if set) are called when the first function
is registered in the instance, last function is removed from the instance resp. */
template<typename FUNC>
class functionBag_base_t : idGenerator_t
{
public:
    typedef FUNC func_t; //!< (signature) type of the stored function
    typedef std::function<void()> ctrl_t; //!< FirstAdded/LastRemoved method types 
    ctrl_t FirstAdded, //!< called when the first function is added to the instance (if set)
        LastRemoved; //!< called when the last function is removed from the instance (if set)

protected:
    typedef std::map<int, func_t> funcs_t; //!< type holding all the functions
    funcs_t funcs_; //!< container holding all added functions

public:
    /*! Adds a function into the internal container and returns a new unique id.
    Invokes the FirstAdded if this is the first item in the container. */
    template<typename FN>
    int add(FN fn)
    {
        int ret = funcs_.insert(typename funcs_t::value_type(get_an_id(), fn)).first->first;
        if (funcs_.size() == 1 && FirstAdded)
            FirstAdded();
        return ret;
    }
    /*! If id already exists in the internal container then it is updated with fn and same id
    is returned, otherwise fn is added as a new item and a new unique id is returned.
    Invokes the FirstAdded if this is the first item in the container. */
    template<typename FN>
    int add(int id, FN fn)
    {
        typename funcs_t::iterator fnd = funcs_.find(id);
        if (fnd == funcs_.end())
            return add(fn);
        fnd->second = fn;
        return id;
    }
    /*! Update existing id with fn and returns id. But if id does not exist in the internal
    container then nothing is done and -1 is returned. */
    template<typename FN>
    int update(int id, FN fn)
    {
        typename funcs_t::iterator fnd = funcs_.find(id);
        if (fnd == funcs_.end())
            return -1;
        fnd->second = fn;
        return id;
    }
    /*! Removes given id from the internal container, invokes LastRemoved if id was the last
    item. Does nothing if the id was not in the container.
    Returns whether id was removed. */
    bool remove(int id)
    {
        bool ret = funcs_.erase(id) > 0;
        if (funcs_.size() == 0 && LastRemoved)
            LastRemoved();
        return ret;
    }
};

// TODO handle additions/removals within call
// TODO handle call within call
/*! Use this to allow registering one or more callbacks as functions/methods/functors/...
and then call them using the () operator. */
template<typename R, typename ... Ps>
class functionBag_t : public functionBag_base_t<std::function<R(Ps...)>>
{
    typedef functionBag_base_t<std::function<R(Ps...)>> parent_t; //!< base class
public:
    /*! Adds a member method fn of instance i into the internal container, returns a new unique id. */
    template<class INST, typename FN>
    int add(INST& i, FN fn)
    {
        return parent_t::add(
            [&i, fn](Ps... ps) {
                return (i.*fn)(ps...);
            });
    }
    using parent_t::add;

    /*! Invokes all registered methods (in the order in which they were added) with given parameters
    and returns their return values in a vector. */
    std::vector<R> operator()(Ps... ps)
    {
        std::vector<R> rs;
        for (auto x : parent_t::funcs_)
            rs.push_back(x.second(ps...));
        return rs;
    }
    /*! Invokes all registered methods (in the order in which they were added) with given parameters
    and for each return value calls the query function.
    Ends after all functions are called or after query first returns false. */
    void call_individual(std::function<bool (R)> query, Ps...ps)
    {
        for (auto x : parent_t::funcs_)
            if (!query(x.second(ps...)))
                break;
    }
};

/*! Specialization for void return type. */
template<typename ... Ps>
class functionBag_t<void, Ps...> : public functionBag_base_t<std::function<void(Ps...)>>
{
    typedef functionBag_base_t<std::function<void(Ps...)>> parent_t; //!< the base class
public:
    /*! Adds a member method fn of instance i into the internal container, returns a new unique id. */
    template<typename INST, typename FN>
    int add(INST& i, FN fn)
    {
        return parent_t::add(
            [&i, fn](Ps... ps) {
                (i.*fn)(ps...);
            });
    }
    using parent_t::add;

    /*! Invokes all registered methods (in the order in which they were added) with given parameters. */
    void operator()(Ps... ps)
    {
        for (auto x : parent_t::funcs_)
            x.second(ps...);
    }
};

namespace AUX
{
/*! A helper to setup functionBag_t's FirstAdded and LastRemoved at once. */
template<typename T>
void setup_first_last_callbacks(T& ecb, std::function<void()> first, std::function<void()> last)
{
    ecb.FirstAdded = first;
    ecb.LastRemoved = last;
}
} // namespace AUX

} // namespace jj


#endif // JJ_FUNCTION_BAG_H
