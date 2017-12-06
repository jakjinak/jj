#ifndef JJ_GUI_COMMON_H
#define JJ_GUI_COMMON_H

#include "jj/defines.h"

#if (JJ_USE_GUI)

#include "jj/string.h"
#include "jj/idGenerator.h"
#include "jj/geometry.h"

#include <set>
#include <functional>

namespace jj
{
namespace gui
{

namespace opt
{
struct title { string_t Title; title() {} title(string_t atitle) : Title(atitle) {} };
struct text { string_t Text; text() {} text(string_t atext) : Text(atext) {} };
struct position { screen_point_t Position; position(); position(screen_point_t p) : Position(p) {} position(int left, int top) : Position(left, top) {} };
struct size { screen_point_t Size; size(); size(screen_point_t s) : Size(s) {} size(int width, int height) : Size(width, height) {} };
template<typename T> struct e { T Value; e() : Value() {} e(T v) : Value(v) {} };
} // namespace opt

template<typename ... Ts>
class creationOptions_t : public Ts...
{
    template<typename T, bool X = std::is_convertible<creationOptions_t&, opt::e<T>&>::value>
    struct HELPER { typedef T type; };
    template<typename T>
    struct HELPER<T, true> { typedef opt::e<T> type; };

public:
    template<typename T>
    creationOptions_t& operator<<(const T v)
    {
        typename HELPER<T>::type& t = *this;
        t = v;
        return *this;
    }
};

template<typename T>
class nativePointerWrapper_t
{
    void* ptr_;
protected:
    nativePointerWrapper_t() : ptr_(nullptr) {}
public:
    void set_native_pointer(void* ptr) { ptr_ = ptr; }
    void reset_native_pointer() { ptr_ = nullptr; }
    void* native_pointer() const { return ptr_; }
};

template<typename T>
struct itemContainer_t
{
    typedef std::set<T*> items_t;

    void reg(T* item) { items_.insert(item); }
    void unreg(T* item) { items_.erase(item); }

    const items_t& items() const { return items_; }

private:
    items_t items_;
};

class content_t;
class sizer_t;

class contentHolder_t : public nativePointerWrapper_t<contentHolder_t>, public idGenerator_t, public itemContainer_t<content_t>
{
    sizer_t* sizer_;
    void set_sizer();

    contentHolder_t(const contentHolder_t&) = delete;
    contentHolder_t(contentHolder_t&&) = delete;
    contentHolder_t& operator=(const contentHolder_t&) = delete;
public:
    std::function<sizer_t* ()> onCreateSizer;

    contentHolder_t();
    virtual ~contentHolder_t();

    typedef nativePointerWrapper_t<contentHolder_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    sizer_t& sizer() {
        if (sizer_ == nullptr)
        {
            sizer_ = onCreateSizer();
            set_sizer();
        }
        return *sizer_;
    }
    void layout();
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_COMMON_H
