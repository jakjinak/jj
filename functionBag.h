#ifndef JJ_FUNCTION_BAG_H
#define JJ_FUNCTION_BAG_H

#include <functional>
#include <vector>
#include <map>
#include "idGenerator.h"

namespace jj
{

template<typename FUNC>
class functionBag_base_t : idGenerator_t
{
protected:
    typedef FUNC func_t;
    typedef std::map<int, func_t> funcs_t;
    funcs_t funcs_;

    typedef std::function<void()> ctrl_t;
    ctrl_t FirstAdded, LastRemoved;

public:
    template<typename FN>
    int add(FN fn)
    {
        int ret = funcs_.insert(typename funcs_t::value_type(get_an_id(), fn)).first->first;
        if (funcs_.size() == 1 && FirstAdded)
            FirstAdded();
        return ret;
    }
    template<typename FN>
    int add(int id, FN fn)
    {
        typename funcs_t::iterator fnd = funcs_.find(id);
        if (fnd == funcs_.end())
            return add(fn);
        fnd->second = fn;
        return id;
    }
    template<typename FN>
    int update(int id, FN fn)
    {
        typename funcs_t::iterator fnd = funcs_.find(id);
        if (fnd == funcs_.end())
            return -1;
        fnd->second = fn;
        return id;
    }
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
template<typename R, typename ... Ps>
class functionBag_t : public functionBag_base_t<std::function<R(Ps...)>>
{
    typedef functionBag_base_t<std::function<R(Ps...)>> parent_t;
public:
    template<typename INST, typename FN>
    int add(INST& i, FN fn)
    {
        return parent_t::add(
            [&i, fn](Ps... ps) {
                return (i.*fn)(ps...);
            });
    }
    using parent_t::add;

    std::vector<R> operator()(Ps... ps)
    {
        std::vector<R> rs;
        for (auto x : parent_t::funcs_)
            rs.push_back(x.second(ps...));
        return rs;
    }
    void call_individual(std::function<bool (R)> query, Ps...ps)
    {
        for (auto x : parent_t::funcs_)
            if (!query(x.second(ps...)))
                break;
    }
};

template<typename ... Ps>
class functionBag_t<void, Ps...> : public functionBag_base_t<std::function<void(Ps...)>>
{
    typedef functionBag_base_t<std::function<void(Ps...)>> parent_t;
public:
    template<typename INST, typename FN>
    int add(INST& i, FN fn)
    {
        return parent_t::add(
            [&i, fn](Ps... ps) {
                (i.*fn)(ps...);
            });
    }
    using parent_t::add;

    void operator()(Ps... ps)
    {
        for (auto x : parent_t::funcs_)
            x.second(ps...);
    }
};

} // namespace jj


#endif // JJ_FUNCTION_BAG_H
