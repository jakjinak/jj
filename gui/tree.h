#ifndef JJ_GUI_TREE_H
#define JJ_GUI_TREE_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

#include <list>
#include <memory>
#include "jj/exception.h"
#include "jj/functionBag.h"

namespace jj
{
namespace gui
{

class tree_base_t;
template<typename T>
class tree_T;

class treeItem_base_t
{
    typedef tree_base_t control_t;
    control_t& parent_;

public:
#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
    typedef void* id_t;
#endif
    treeItem_base_t(control_t& parent, id_t id) : parent_(parent), id_(id) {}
    virtual ~treeItem_base_t() =0;

    id_t id() const { return id_; }
    id_t parent() const;

    void text(const jj::string_t& t);
    jj::string_t text() const;

    void bold(bool b);
    bool bold() const;

    bool expanded() const;
    void collapse();
    void expand();
    void collapse_recursively();
    void expand_recursively();

private:
    id_t id_;

    template<typename T>
    friend class treeItem_T;
    id_t new_item(const jj::string_t& text);
    id_t new_item(size_t index, const jj::string_t& text);
    void remove_item(id_t id);

    void throw_invalid_index(size_t index, size_t max) const;
};

template<typename T>
class treeItem_T : public treeItem_base_t
{
    typedef treeItem_base_t base_t;
    typedef T item_t;
    typedef tree_T<T> control_t;
    control_t& parent_;

    typedef std::list<item_t> children_t;
    children_t children_;

    typename children_t::iterator locate(const item_t& item)
    {
        for (typename children_t::iterator it = children_.begin(); it != children_.end(); ++it)
            if (item.id_ == it->id_)
                return it;
        throw jj::exception::base("Item does not belong here!");
    }
    typename children_t::const_iterator locate(size_t index) const
    {
        if (index == children_.size())
            return children_.end();
        typename children_t::const_iterator it = children_.begin();
        while (index > 0 && it != children_.end())
        {
            ++it;
            --index;
        }
        return it;
    }
    typename children_t::iterator locate(size_t index)
    {
        if (index == children_.size())
            return children_.end();
        typename children_t::iterator it = children_.begin();
        while (index > 0 && it != children_.end())
        {
            ++it;
            --index;
        }
        return it;
    }

public:
    treeItem_T(control_t& parent, id_t id) : base_t(parent, id), parent_(parent) {}

    const item_t* parent_item() const
    {
        id_t p = parent();
        if (parent_.root().id() == p)
            return &parent_.root();
        return parent_.root().find(p);
    }

    item_t* parent_item()
    {
        id_t p = parent();
        if (parent_.root().id() == p)
            return &parent_.root();
        return parent_.root().find(p);
    }

    item_t& add(const jj::string_t& text)
    {
        id_t id = new_item(text);
        try
        {
            children_.emplace_back(parent_, id);
            return children_.back();
        }
        catch (...)
        {
            remove_item(id);
            throw;
        }
    }
    item_t& add(const jj::string_t& text, const item_t& infrontof)
    {
        typename children_t::iterator fnd = locate(infrontof);
        size_t idx = 0;
        for (auto& i : children_)
        {
            if (i.id_ == fnd->id_)
                break;
            ++idx;
        }
        id_t id = new_item(idx, text);
        try
        {
            return *children_.emplace(fnd, parent_, id);
        }
        catch (...)
        {
            remove_item(id);
            throw;
        }
    }
    item_t& add(const jj::string_t& text, size_t index)
    {
        if (index > children_.size())
            return add(text);

        typename children_t::iterator it = locate(index);
        id_t id = new_item(index, text);
        try
        {
            return *children_.emplace(it, parent_, id);
        }
        catch (...)
        {
            remove_item(id);
            throw;
        }
    }
    void remove(item_t& item)
    {
        typename children_t::iterator fnd = locate(item); // note: throws if not found
        remove_item(fnd->id_);
        children_.erase(fnd);
    }
    size_t count() const { return children_.size(); }
    const item_t& get(size_t index) const
    {
        if (index >= children_.size())
            throw_invalid_index(index, children_.size());
        return *locate(index);
    }
    item_t& get(size_t index)
    {
        if (index >= children_.size())
            throw_invalid_index(index, children_.size());
        return *locate(index);
    }
    const item_t* find(id_t id) const
    {
        for (const item_t& i : children_)
        {
            if (i.id_ == id)
                return &i;
            const item_t* fnd = i.find(id);
            if (fnd != nullptr)
                return fnd;
        }
        return nullptr;
    }
    item_t* find(id_t id)
    {
        for (item_t& i : children_)
        {
            if (i.id_ == id)
                return &i;
            item_t* fnd = i.find(id);
            if (fnd != nullptr)
                return fnd;
        }
        return nullptr;
    }
};

class tree_base_t : public nativePointerWrapper_t<tree_base_t>, public control_t
{
    typedef treeItem_base_t::id_t id_t;
    id_t current_item() const;
    bool is_valid_item(id_t id) const;

public:
    enum flags_t
    {
        EDITABLE, //!< labels of tree items are editable, see OnEditStart, OnEditEnd
        ROOT_IS_HIDDEN,
        LINES,
        LINES_AT_ROOT,
        FULL_ROW_HIGHLIGHT,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;

    enum buttonType_t
    {
        NONE,
        REGULAR,
        TWIST
    };
    typedef opt::e<buttonType_t> buttonType1_t;
    enum selection_t
    {
        SINGLE,
        RANGE
    };
    typedef opt::e<selection_t> selection1_t;
    typedef options_T<opt::text, opt::position, opt::size, flags1_t, buttonType1_t, selection1_t> options_t;
    static options_t options() { return options_t() << LINES << SINGLE << REGULAR; }

private:
    typedef control_t parent_t;

    template<typename T>
    friend class tree_T;

    class treeImpl_t;
    std::shared_ptr<treeImpl_t> impl_;

    std::function<void()> startBeforeSelect, stopBeforeSelect, startOnSelect, stopOnSelect,
        startOnActivate, stopOnActivate;
    virtual bool on_before_select(id_t newid, id_t oldid) =0;
    virtual void on_select(id_t newid, id_t oldid) =0;
    virtual void on_activate(id_t id) =0;

    friend class treeImpl_t;

protected:
    id_t rootId_;

public:
    tree_base_t(contentHolder_t& owner, options_t setup);
    virtual ~tree_base_t() {}

    treeImpl_t& impl() { return *impl_; }

    typedef nativePointerWrapper_t<tree_base_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    void collapse_recursive();
    void expand_recursive();
};

template<typename T>
class tree_T : public tree_base_t
{
    typedef T item_t;
    typedef tree_base_t base_t;

    item_t root_;

    item_t* root_or_find(treeItem_base_t::id_t id)
    {
        if (root().id() == id)
            return &root();
        return root().find(id);
    }


    virtual bool on_before_select(id_t newid, id_t oldid) override
    {
        item_t* fndnew = root_or_find(newid);
        if (fndnew == nullptr)
            return true;
        item_t* fndold = root_or_find(oldid);
        auto ret = BeforeSelect(*fndnew, fndold);
        for (auto r : ret)
            if (!r)
                return false;
        return true;
    }
    virtual void on_select(id_t newid, id_t oldid) override
    {
        item_t* fndnew = root_or_find(newid);
        if (fndnew == nullptr)
            return;
        item_t* fndold = root_or_find(oldid);
        OnSelect(*fndnew, fndold);
    }
    virtual void on_activate(id_t id)
    {
        item_t* fnd = root_or_find(id);
        if (fnd == nullptr)
            return;
        OnActivate(*fnd);
    }

public:
    tree_T(contentHolder_t& owner, options_t setup)
        : base_t(owner, setup), root_(*this, rootId_)
    {
        jj::setup_functionBag_callbacks(BeforeSelect, startBeforeSelect, stopBeforeSelect);
        jj::setup_functionBag_callbacks(OnSelect, startOnSelect, stopOnSelect);
        jj::setup_functionBag_callbacks(OnActivate, startOnActivate, stopOnActivate);
    }

    const item_t& root() const { return root_; }
    item_t& root() { return root_; }

    const item_t* selected() const
    {
        id_t cid = current_item();
        if (!is_valid_item(cid))
            return nullptr;
        if (root_.id() == cid)
            return &root_;
        return root_.find(cid);
    }
    item_t* selected()
    {
        id_t cid = current_item();
        if (!is_valid_item(cid))
            return nullptr;
        if (root_.id() == cid)
            return &root_;
        return root_.find(cid);
    }

    functionBag_t<bool, item_t&, item_t*> BeforeSelect;
    functionBag_t<void, item_t&, item_t*> OnSelect;
    functionBag_t<void, item_t&> OnActivate;
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_TREE_H
