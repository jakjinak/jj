#ifndef JJ_GUI_COMMON_WX_H
#define JJ_GUI_COMMON_WX_H

#include <wx/wx.h>
#include "jj/gui/common.h"

namespace jj
{
namespace gui
{

inline screen_point_t pt2p(wxPoint pt) { return screen_point_t(pt.x, pt.y); }
inline screen_point_t siz2p(wxSize siz) { return screen_point_t(siz.x, siz.y); }

template<typename S> struct wxs2s;
template<> struct wxs2s<std::string> { inline static std::string cvt(const wxString& s) { return s.ToStdString(); } };
template<> struct wxs2s<std::wstring> { inline static std::wstring cvt(const wxString& s) { return s.ToStdWstring(); } };

namespace AUX
{

/** Wraps correct type conversion (which actually is automatic in the end), but needs correct source types provided. */
template<typename O, typename P> struct GETTER;
template<typename P> struct GETTER<wxWindow, P> { inline static P* get(wxWindow* p) { return p; } };

template<typename P> struct GETTER<wxControl, P> { inline static P* get(wxControl* p) { return p; } };
template<typename P> struct GETTER<wxStaticText, P> { inline static P* get(wxStaticText* p) { return p; } };
template<typename P> struct GETTER<wxTextCtrl, P> { inline static P* get(wxTextCtrl* p) { return p; } };
template<typename P> struct GETTER<wxComboBox, P> { inline static P* get(wxComboBox* p) { return p; } };
template<typename P> struct GETTER<wxButton, P> { inline static P* get(wxButton* p) { return p; } };

template<typename P> struct GETTER<wxSizer, P> { inline static P* get(wxSizer* p) { return p; } };
template<typename P> struct GETTER<wxBoxSizer, P> { inline static P* get(wxBoxSizer* p) { return p; } };

template<typename P> struct GETTER<wxMenu, P> { inline static P* get(wxMenu* p) { return p; } };
template<typename P> struct GETTER<wxMenuBar, P> { inline static P* get(wxMenuBar* p) { return p; } };
template<typename P> struct GETTER<wxStatusBar, P> { inline static P* get(wxStatusBar* p) { return p; } };

template<typename P> struct GETTER<wxTopLevelWindow, P> { inline static P* get(wxTopLevelWindow* p) { return p; } };
template<typename P> struct GETTER<wxFrame, P> { inline static P* get(wxFrame* p) { return p; } };
template<typename P> struct GETTER<wxDialog, P> { inline static P* get(wxDialog* p) { return p; } };
template<typename P> struct GETTER<wxMessageDialog, P> { inline static P* get(wxMessageDialog* p) { return p; } };
template<typename P> struct GETTER<wxTextEntryDialog, P> { inline static P* get(wxTextEntryDialog* p) { return p; } };
template<typename P> struct GETTER<wxPasswordEntryDialog, P> { inline static P* get(wxPasswordEntryDialog* p) { return p; } };
template<typename P> struct GETTER<wxFileDialog, P> { inline static P* get(wxFileDialog* p) { return p; } };
template<typename P> struct GETTER<wxDirDialog, P> { inline static P* get(wxDirDialog* p) { return p; } };

/** Converts given type T representing a class containing a void* to the native type actually meant.
Ie. ptr frame_t always holds a wxFrame pointer (even if it is void*), content_holder_t holds wxWindow* (even if void*), etc. */
template<typename T> struct COGET;
template<> struct COGET<contentHolder_t> { typedef wxWindow TYPE; };

} // namespace AUX

  /** Gets a ptr stored as void* in given object of type T and converts it to the correct native type as given by X. */
template<typename X> struct GET
{
    template<typename T> inline static X* from(const T* p) { return AUX::GETTER<typename AUX::COGET<T>::TYPE, X>::get(reinterpret_cast<typename AUX::COGET<T>::TYPE*>(p->T::native_t::native_pointer())); }

    template<typename Y> struct AS
    {
        template<typename T> inline static X* from(const T* p) { return AUX::GETTER<Y, X>::get(reinterpret_cast<Y*>(p->T::native_t::native_pointer())); }
    };
};

// TODO rename nativeWrapper_t to nativeWrapper_T and this to nativeWrapper_t
struct nativeWrapper_base_t
{
    virtual ~nativeWrapper_base_t() {}
    virtual void disown() = 0;
};

template<typename OWNER, typename NATIVE>
class nativeWrapper_t : public NATIVE, public nativeWrapper_base_t
{
    typedef OWNER owner_t;
    typedef NATIVE native_t;

protected:
    owner_t* owner_;

public:
    template<typename ... Ps>
    nativeWrapper_t(owner_t& owner, Ps... ps)
        : native_t(ps...), owner_(&owner)
    {
    }
    ~nativeWrapper_t()
    {
        if (owner_ == nullptr)
            return;
        owner_->reset_native_pointer();
        delete owner_;
    }

    virtual void disown() override
    {
        owner_ = nullptr;
    }
};

} // namespace gui
} // namespace jj

#endif // JJ_GUI_COMMON_WX_H
