#ifndef JJ_GUI_CONTROL_H
#define JJ_GUI_CONTROL_H

#include "jj/gui/common.h"

#if (JJ_USE_GUI)

namespace jj
{
namespace gui
{

class content_t : public nativePointerWrapper_t<content_t>, protected idHolder_t
{
public:
    enum type_t { CONTROL, SIZER };
private:
    contentHolder_t* owner_;
    type_t type_;

    void disown() { owner_ = nullptr; }
    friend class contentHolder_t;
public:
    content_t(contentHolder_t& owner, type_t type);
    virtual ~content_t();
    type_t type() const { return type_; }

    typedef nativePointerWrapper_t<content_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

class control_t : public nativePointerWrapper_t<control_t>, public content_t
{
    typedef content_t parent_t;

public:
    control_t(contentHolder_t& owner);

    typedef nativePointerWrapper_t<control_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_CONTROL_H
