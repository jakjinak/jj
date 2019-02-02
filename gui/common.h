#ifndef JJ_GUI_COMMON_H
#define JJ_GUI_COMMON_H

#include "jj/defines.h"

#if (JJ_USE_GUI)

#include "jj/string.h"
#include "jj/idGenerator.h"
#include "jj/point.h"
#include "jj/options.h"

#include <set>
#include <functional>

namespace jj
{

namespace opt
{
/*! Window's specific option - define the text in the title bar. */
struct title { string_t Title; title() {} title(string_t atitle) : Title(atitle) {} };
/*! Control's or window's option. */
struct text { string_t Text; text() {} text(string_t atext) : Text(atext) {} };
/*! Control's or window's option. */
struct position { screen_point_t Position; position(); position(screen_point_t p) : Position(p) {} position(int left, int top) : Position(left, top) {} position(const position& other) : Position(other.Position) {} };
/*! Control's or window's option. */
struct size { screen_point_t Size; size(); size(screen_point_t s) : Size(s) {} size(int width, int height) : Size(width, height) {} };
} // namespace opt

namespace gui
{

/*! Defines horizontal alignment. */
enum class align_t { LEFT, CENTER, RIGHT };
/*! Defines vertical alignment. */
enum class alignv_t { TOP, MIDDLE, BOTTOM };

namespace AUX
{
/*! A helper to hide the actual implementation specific pointer. */
template<typename T>
class nativePointerWrapper_t
{
    void* ptr_; //!< the stored pointer
protected:
    /*! Ctor */
    nativePointerWrapper_t() : ptr_(nullptr) {}
public:
    /*! Store the native pointer. */
    void set_native_pointer(void* ptr) { ptr_ = ptr; }
    /*! Clean the stored native pointer. */
    void reset_native_pointer() { ptr_ = nullptr; }
    /*! Return the stored native pointer. */
    void* native_pointer() const { return ptr_; }
};
} // namespace AUX

/*! Holds items of given type T. */
template<typename T>
struct itemContainer_t
{
    typedef std::set<T*> items_t; //!< the holder type

    /*! Adds item to container, does nothing if it's already there. */
    void reg(T* item) { items_.insert(item); }
    /*! Removes item from container, does nothing if it's not there. */
    void unreg(T* item) { items_.erase(item); }
    /*! Returns all items in container. */
    const items_t& items() const { return items_; }

private:
    items_t items_; //!< holder
};

// forward declaration
class content_t;
class sizer_t;

/*! A class that holds controls and sizers (of a window) and also wraps the creation of the main sizer, which is auto-created. */
class contentHolder_t : public AUX::nativePointerWrapper_t<contentHolder_t>, public idGenerator_t, public itemContainer_t<content_t>
{
    sizer_t* sizer_; //!< the main sizer
    /*! Physically inserts the main sizer into the native implementation (of the window). */
    void set_sizer();

    contentHolder_t(const contentHolder_t&) = delete;
    contentHolder_t(contentHolder_t&&) = delete;
    contentHolder_t& operator=(const contentHolder_t&) = delete;
public:
    /*! Creates the main sizer. Invoked when the sizer() is first called, set your own to customize behavior. */
    std::function<sizer_t* ()> OnCreateSizer;

    /*! Ctor */
    contentHolder_t();
    /*! Dtor */
    virtual ~contentHolder_t();

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<contentHolder_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Returns the main sizer (which contains everything else) for this holder (usually toplevel window).
    Customize the creation by overriding OnCreateSizer. */
    sizer_t& sizer()
    {
        if (sizer_ == nullptr)
        {
            sizer_ = OnCreateSizer();
            set_sizer();
        }
        return *sizer_;
    }
    /*! Force recalculation of the sizer geometry (and thus repositioning the controls). */
    void layout();
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_COMMON_H
