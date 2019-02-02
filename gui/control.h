#ifndef JJ_GUI_CONTROL_H
#define JJ_GUI_CONTROL_H

#include "jj/gui/common.h"

#if (JJ_USE_GUI)

namespace jj
{
namespace gui
{

/*! Abstracts (window) content - a control or a sizer. */
class content_t : public AUX::nativePointerWrapper_t<content_t>, protected idHolder_t
{
public:
    /*! Defines type of content. */
    enum type_t { CONTROL, SIZER };
private:
    contentHolder_t* owner_; //!< owner of the content
    type_t type_; //!< type of the content

    /*! Remove owner from the content, used (internally) when owner (window) is being destroyed. */
    void disown() { owner_ = nullptr; }
    friend class contentHolder_t;
public:
    /*! Ctor */
    content_t(contentHolder_t& owner, type_t type);
    /*! Dtor */
    virtual ~content_t();
    /*! Returns type of content. */
    type_t type() const { return type_; }

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<content_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

/*! Base class for all visible (non-sizer) content. */
class control_t : public AUX::nativePointerWrapper_t<control_t>, public content_t
{
    typedef content_t parent_t;

public:
    /*! Ctor */
    control_t(contentHolder_t& owner);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<control_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_CONTROL_H
