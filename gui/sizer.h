#ifndef JJ_GUI_SIZER_H
#define JJ_GUI_SIZER_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

namespace jj
{
namespace gui
{

struct sizerFlags_t
{
    enum align_t { LEFT, CENTER, RIGHT };
    align_t Align;
    enum alignv_t { TOP, MIDDLE, BOTTOM };
    alignv_t AlignV;
    enum expand_t { DONT_EXPAND, EXPAND };
    expand_t Expand;
    
    int Border, BorderV;
    int Proportion;

    sizerFlags_t();

    sizerFlags_t& set(align_t v) { Align = v; return *this; }
    sizerFlags_t& set(alignv_t v) { AlignV = v; return *this; }
    sizerFlags_t& set(expand_t v) { Expand = v; return *this; }
    sizerFlags_t& expand() { return set(EXPAND); return *this; }
    sizerFlags_t& border(int v) { Border = v; BorderV = v; return *this; }
    sizerFlags_t& borderH(int v) { Border = v; return *this; }
    sizerFlags_t& borderV(int v) { BorderV = v; return *this; }
    sizerFlags_t& proportion(int v) { Proportion = v; return *this; }
};

class sizer_t : public content_t, public nativePointerWrapper_t<sizer_t>
{
    typedef nativePointerWrapper_t<sizer_t> native_t;
    typedef content_t parent_t;

public:
    sizer_t(contentHolder_t& owner);

    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    void add(content_t& content, sizerFlags_t flags);
};

class boxSizer_t : public sizer_t, public nativePointerWrapper_t<boxSizer_t>
{
    typedef nativePointerWrapper_t<boxSizer_t> native_t;
    typedef sizer_t parent_t;

public:
    enum orientation_t { VERTICAL, HORIZONTAL };
    boxSizer_t(contentHolder_t& owner, orientation_t o);

    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_SIZER_H
