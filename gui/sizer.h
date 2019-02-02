#ifndef JJ_GUI_SIZER_H
#define JJ_GUI_SIZER_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

namespace jj
{
namespace gui
{

/*! Definition structure for setting up how sizer affects added content. */
struct sizerFlags_t
{
    align_t Align; //!< horizontal alignment
    alignv_t AlignV; //!< vertical alignment
    /*! Controls expansion of sizer content (the other size than proportion). */
    enum expand_t
    {
        DONT_EXPAND, //!< keep size of the content
        EXPAND //!< expand the size of content to the size of sizer
    };
    expand_t Expand; //!< whether to expand the content
    
    int Border, //!< horizontal alignment of content
        BorderV; //!< vertical alignment of content
    int Proportion; //!< relative proportion of size of content compared to other content (with relative proportion)

    /*! Ctor */
    sizerFlags_t();

    /*! Sets horizontal alignment. */
    sizerFlags_t& set(align_t v) { Align = v; return *this; }
    /*! Sets vertical alignment. */
    sizerFlags_t& set(alignv_t v) { AlignV = v; return *this; }
    /*! Sets how to expand content. */
    sizerFlags_t& set(expand_t v) { Expand = v; return *this; }
    /*! Sets to expand content. */
    sizerFlags_t& expand() { return set(EXPAND); return *this; }
    /*! Sets size of both, horizontal and vertical, border. */
    sizerFlags_t& border(int v) { Border = v; BorderV = v; return *this; }
    /*! Sets size of horizontal border. */
    sizerFlags_t& borderH(int v) { Border = v; return *this; }
    /*! Sets size of vertical border. */
    sizerFlags_t& borderV(int v) { BorderV = v; return *this; }
    /*! Sets the relative proportion of content size. */
    sizerFlags_t& proportion(int v) { Proportion = v; return *this; }
};

/*! Base class for all sizers.
Note: You shall use the contentHolder_t::layout() to force recalculation how content is organized. */
class sizer_t : public content_t, public AUX::nativePointerWrapper_t<sizer_t>
{
    typedef content_t parent_t;

public:
    /*! Ctor */
    sizer_t(contentHolder_t& owner);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<sizer_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Add content to the sizer.
    Note that it does not layout(). */
    void add(content_t& content, sizerFlags_t flags);
};

/*! The straightforwardest (and default) sizer. Allows organizing content
vertically or horizontally. */
class boxSizer_t : public sizer_t, public AUX::nativePointerWrapper_t<boxSizer_t>
{
    typedef sizer_t parent_t;

public:
    /*! Defines how sizer organizes the content. */
    enum orientation_t { VERTICAL, HORIZONTAL };
    /*! Ctor */
    boxSizer_t(contentHolder_t& owner, orientation_t o);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<boxSizer_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_SIZER_H
