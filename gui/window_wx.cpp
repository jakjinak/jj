#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "jj/gui/common_wx.h"
#include "jj/gui/window.h"
#include <array>

/* DESTRUCTION MECHANISMS
1) when delete on jj object called:
      jj                       wx
-> delete <jj>    ->    <wr>->disown()
                        <wx>->Destroy() ->
                                        ...
                  X-        delete <wx> <-

2) when Destroy called on wx object:
                     -> <wx>->Destroy() ->
                                        ...
   <jj>->resetNP()<-        delete <wx>
   delete <jj>    <-
   (part of which is
    disown() on content
    children)
*/

namespace jj
{
namespace gui
{

namespace AUX
{

template<> struct COGET<topLevelWindow_t> { typedef wxTopLevelWindow TYPE; };
template<> struct COGET<frame_t> { typedef wxFrame TYPE; };
template<> struct COGET<menuBar_t> { typedef wxMenuBar TYPE; };
template<> struct COGET<statusBar_t> { typedef wxStatusBar TYPE; };

} // namespace AUX

void topLevelWindow_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxWindow>::from(this));
}

void topLevelWindow_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

string_t topLevelWindow_t::title() const
{
    return wxs2s<string_t>::cvt(GET<wxTopLevelWindow>::from(this)->GetTitle());
}

void topLevelWindow_t::title(const string_t& v)
{
    GET<wxTopLevelWindow>::from(this)->SetTitle(v);
}

bool topLevelWindow_t::isShown() const
{
    return GET<wxTopLevelWindow>::from(this)->IsShown();
}

void topLevelWindow_t::show()
{
    GET<wxTopLevelWindow>::from(this)->Show();
}

void topLevelWindow_t::hide()
{
    GET<wxTopLevelWindow>::from(this)->Hide();
}

void topLevelWindow_t::minimize()
{
    GET<wxTopLevelWindow>::from(this)->Iconize();
}

void topLevelWindow_t::maximize()
{
    GET<wxTopLevelWindow>::from(this)->Maximize();
}

void topLevelWindow_t::restore()
{
    GET<wxTopLevelWindow>::from(this)->Restore();
}

bool topLevelWindow_t::close(bool force)
{
    return GET<wxTopLevelWindow>::from(this)->Close(force);
}

namespace // <anonymous>
{
struct wrMenuBar : public nativeWrapper_t<menuBar_t, wxMenuBar>
{
    template<typename ... Ps>
    wrMenuBar(menuBar_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }

    void evtMenu(wxCommandEvent& evt)
    {
    }
};
struct wrStatusBar : public nativeWrapper_t<statusBar_t, wxStatusBar>
{
    template<typename ... Ps>
    wrStatusBar(statusBar_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }
};
struct wrFrame : public nativeWrapper_t<frame_t, wxFrame>
{
    template<typename ... Ps>
    wrFrame(frame_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
        Bind(wxEVT_CLOSE_WINDOW, &wrFrame::evtClose, this, this->GetId());
    }
    void evtClose(wxCloseEvent& evt)
    {
        owner_->OnClose.call_individual(
            [&evt](bool result) { 
                if (!result && evt.CanVeto()) 
                    evt.Veto(); 
                return true; 
            }, 
            *owner_);
        if (evt.GetVeto())
            ;
        else
            Destroy();
    }
};
} // namespace <anonymous>

menuBar_t::menuBar_t(frame_t& owner)
    : parent_t(owner, *this), native_t(*this), owner_(owner)
{
    wrMenuBar* tmp = new wrMenuBar(*this);
    set_native_pointer(static_cast<wxMenuBar*>(tmp));
}

void menuBar_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(ptr); // also the menu_t will contain a wxMenuBar despite normally it would be wxMenu
}

void menuBar_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

statusBar_t::field_t::field_t()
    : text_(), width_(0), style_(DEFAULT)
{
}

statusBar_t::field_t::field_t(const string_t& text, style_t style)
    : text_(text), width_(0), style_(DEFAULT)
{
}

statusBar_t::statusBar_t(frame_t& owner)
    : owner_(owner)
{
    wrStatusBar* tmp = new wrStatusBar(*this, GET<wxWindow>::from(&owner), owner.get_an_id(), wxSTB_DEFAULT_STYLE);
    set_native_pointer(static_cast<wxStatusBar*>(tmp));
}

void statusBar_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
}

void statusBar_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
}

size_t statusBar_t::count() const
{
    int i = GET<wxStatusBar>::from(this)->GetFieldsCount();
    if (i < 0)
        throw std::length_error("status bar field count");
    return (size_t)i;
}

void statusBar_t::resize(size_t size)
{
    GET<wxStatusBar>::from(this)->SetFieldsCount(int(size));
}

static statusBar_t::style_t sbs2s(int s)
{
    switch (s)
    {
    case wxSB_FLAT:
        return statusBar_t::FLAT;
    case wxSB_RAISED:
        return statusBar_t::RAISED;
    case wxSB_SUNKEN:
        return statusBar_t::SUNKEN;
    default:
        return statusBar_t::DEFAULT;
    }
}

static int s2sbs(statusBar_t::style_t s)
{
    switch (s)
    {
    case statusBar_t::FLAT:
        return wxSB_FLAT;
    case statusBar_t::RAISED:
        return wxSB_RAISED;
    case statusBar_t::SUNKEN:
        return wxSB_SUNKEN;
    default:
        return wxSB_NORMAL;
    }
}

void statusBar_t::set(const fields_t& fields)
{
    wxStatusBar* sb = GET<wxStatusBar>::from(this);
    int fs = int(fields.size());
    sb->SetFieldsCount(fs);
    int i = 0;
    int* arr = nullptr;
    try
    {
        // allocate
        arr = new int[fields.size()];

        // set styles
        i = 0;
        for (auto& f : fields)
        {
            arr[i] = s2sbs(f.style_);
            ++i;
        }
        sb->SetStatusStyles(fs, arr);

        // set widths
        i = 0;
        for (auto& f : fields)
        {
            arr[i] = f.width_;
            ++i;
        }
        sb->SetStatusWidths(fs, arr);

        // finalize
        delete[] arr;
    }
    catch (...)
    {
        delete[] arr;
        throw;
    }
    i = 0;
    for (auto& f : fields)
    {
        sb->SetStatusText(f.text_, i);
        ++i;
    }
}

statusBar_t::fields_t statusBar_t::get() const
{
    wxStatusBar* sb = GET<wxStatusBar>::from(this);
    std::vector<field_t> tmp;
    int si = sb->GetFieldsCount();
    if (si<0)
        throw std::length_error("status bar field count");

    tmp.reserve(size_t(si));
    for (int i = 0; i < si; ++i)
        tmp.push_back(field_t(
            wxs2s<string_t>::cvt(sb->GetStatusText(i)),
            sb->GetStatusWidth(i),
            sbs2s(sb->GetStatusStyle(i))
        ));
    return tmp;
}

string_t statusBar_t::text(size_t i)
{
    return wxs2s<string_t>::cvt(GET<wxStatusBar>::from(this)->GetStatusText(int(i)));
}

void statusBar_t::text(size_t i, const string_t& value)
{
    GET<wxStatusBar>::from(this)->SetStatusText(value, int(i));
}

void statusBar_t::push(size_t i, const string_t& value)
{
    GET<wxStatusBar>::from(this)->PushStatusText(value, int(i));
}

void statusBar_t::pop(size_t i)
{
    GET<wxStatusBar>::from(this)->PopStatusText(int(i));
}

int statusBar_t::width(size_t i)
{
    return GET<wxStatusBar>::from(this)->GetStatusWidth(int(i));
}

void statusBar_t::width(size_t i, int value)
{
    wxStatusBar* sb = GET<wxStatusBar>::from(this);
    int cnt = sb->GetFieldsCount();
    if (cnt < 0)
        throw std::length_error("status bar field count");
    int* arr = nullptr;
    try
    {
        arr = new int[cnt];
        for (int x = 0; x < cnt; ++x)
            arr[x] = sb->GetStatusWidth(x);
        arr[i] = value;
        sb->SetStatusWidths(cnt, arr);
        delete[] arr;
    }
    catch (...)
    {
        delete[] arr;
        throw;
    }
}

statusBar_t::style_t statusBar_t::style(size_t i)
{
    return sbs2s(GET<wxStatusBar>::from(this)->GetStatusStyle(int(i)));
}

void statusBar_t::style(size_t i, style_t value)
{
    wxStatusBar* sb = GET<wxStatusBar>::from(this);
    int cnt = sb->GetFieldsCount();
    if (cnt < 0)
        throw std::length_error("status bar field count");
    int* arr = nullptr;
    try
    {
        arr = new int[cnt];
        for (int x = 0; x < cnt; ++x)
            arr[x] = sb->GetStatusStyle(x);
        arr[i] = s2sbs(value);
        sb->SetStatusStyles(cnt, arr);
        delete[] arr;
    }
    catch (...)
    {
        delete[] arr;
        throw;
    }
}

namespace // <anonymous>
{
static long ff2wxfs(frame_t::flags1_t v)
{
    long ret = wxDEFAULT_FRAME_STYLE;
    if (v*frame_t::NO_RESIZE) ret &= (~(wxRESIZE_BORDER));
    if (v*frame_t::NO_MINIMIZE) ret &= (~(wxMINIMIZE_BOX));
    if (v*frame_t::NO_MAXIMIZE) ret &= (~(wxMAXIMIZE_BOX));
    if (v*frame_t::NO_CLOSE) ret &= (~(wxCLOSE_BOX));
    if (v*frame_t::NO_SYSMENU) ret &= (~(wxSYSTEM_MENU));
    if (v*frame_t::NO_CAPTION) ret &= (~(wxCAPTION));
    if (v*frame_t::NO_TASKBAR) ret |= wxFRAME_NO_TASKBAR;
    if (v*frame_t::TOOL_WINDOW) ret |= wxFRAME_TOOL_WINDOW;
    if (v*frame_t::SHAPED) ret |= wxFRAME_SHAPED;
    return ret;
}
} // namespace <anonymous>

frame_t::frame_t(application_t& app, options_t setup)
    : parent_t(app, nullptr), native_t(*this)
    , menuBar_(nullptr), statusBar_(nullptr)
    , onCreateMenuBar([this] { return new menuBar_t(*this); }), onCreateStatusBar([this] { return new statusBar_t(*this); })
{
    wrFrame* tmp = new wrFrame(*this, nullptr, wxID_ANY, setup.Text, wxPoint(setup.Position.Column, setup.Position.Row), wxSize(setup.Size.Width, setup.Size.Height), ff2wxfs(setup));
    set_native_pointer(static_cast<wxFrame*>(tmp));
}

frame_t::frame_t(application_t& app, topLevelWindow_t& owner, options_t setup)
    : parent_t(app, &owner), native_t(*this)
    , menuBar_(nullptr), statusBar_(nullptr)
    , onCreateMenuBar([this] { return new menuBar_t(*this); }), onCreateStatusBar([this] { return new statusBar_t(*this); })
{
    wrFrame* tmp = new wrFrame(*this, GET<wxWindow>::from(&owner), wxID_ANY, setup.Text, wxPoint(setup.Position.Column, setup.Position.Row), wxSize(setup.Size.Width, setup.Size.Height), ff2wxfs(setup));
    set_native_pointer(static_cast<wxFrame*>(tmp));
}

frame_t::~frame_t()
{
    if (!native_t::native_pointer())
        return;
    
    wxFrame* tmp = GET<wxFrame>::from(this);
    wrFrame* twr = dynamic_cast<wrFrame*>(tmp);
    if (twr)
        twr->disown();
    tmp->Destroy();
    native_t::reset_native_pointer();
}

void frame_t::set_menu_bar()
{
    GET<wxFrame>::from(this)->SetMenuBar(GET<wxMenuBar>::from(menuBar_));
}

void frame_t::set_status_bar()
{
    GET<wxFrame>::from(this)->SetStatusBar(GET<wxStatusBar>::from(statusBar_));
    GET<wxFrame>::from(this)->SetStatusBarPane(-1);
}

void frame_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxTopLevelWindow>::from(this));
}

void frame_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
