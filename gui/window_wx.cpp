#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "jj/gui/common_wx.h"
#include "jj/gui/window.h"
#include "jj/gui/stock_wx.h"
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
template<> struct COGET<dialog_t> { typedef wxDialog TYPE; };
template<> struct COGET<dlg::simple_t> { typedef wxMessageDialog TYPE; };
template<> struct COGET<dlg::openFile_t> { typedef wxFileDialog TYPE; };
template<> struct COGET<dlg::saveFile_t> { typedef wxFileDialog TYPE; };
template<> struct COGET<dlg::selectDir_t> { typedef wxDirDialog TYPE; };

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
    GET<wxTopLevelWindow>::from(this)->SetTitle(s2wxs<string_t>::cvt(v));
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
        sb->SetStatusText(s2wxs<string_t>::cvt(f.text_), i);
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
    GET<wxStatusBar>::from(this)->SetStatusText(s2wxs<string_t>::cvt(value), int(i));
}

void statusBar_t::push(size_t i, const string_t& value)
{
    GET<wxStatusBar>::from(this)->PushStatusText(s2wxs<string_t>::cvt(value), int(i));
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
    wrFrame* tmp = new wrFrame(*this, nullptr, wxID_ANY, s2wxs<string_t>::cvt(setup.Text), wxPoint(setup.Position.Column, setup.Position.Row), wxSize(setup.Size.Width, setup.Size.Height), ff2wxfs(setup));
    set_native_pointer(static_cast<wxFrame*>(tmp));
}

frame_t::frame_t(application_t& app, topLevelWindow_t& owner, options_t setup)
    : parent_t(app, &owner), native_t(*this)
    , menuBar_(nullptr), statusBar_(nullptr)
    , onCreateMenuBar([this] { return new menuBar_t(*this); }), onCreateStatusBar([this] { return new statusBar_t(*this); })
{
    wrFrame* tmp = new wrFrame(*this, GET<wxWindow>::from(&owner), wxID_ANY, s2wxs<string_t>::cvt(setup.Text), wxPoint(setup.Position.Column, setup.Position.Row), wxSize(setup.Size.Width, setup.Size.Height), ff2wxfs(setup));
    set_native_pointer(static_cast<wxFrame*>(tmp));
}

frame_t::~frame_t()
{
    if (!native_t::native_pointer())
        return;
    
    wxFrame* tmp = GET<wxFrame>::from(this);
    nativeWrapper_base_t* twr = dynamic_cast<nativeWrapper_base_t*>(tmp);
    if (twr)
        twr->disown();
    tmp->Destroy();
    reset_native_pointer();
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

namespace // <anonymous>
{
struct wrDialog : public nativeWrapper_t<dialog_t, wxDialog>
{
    template<typename ... Ps>
    wrDialog(dialog_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
        Bind(wxEVT_CLOSE_WINDOW, &wrDialog::evtClose, this, this->GetId());
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
            owner_->hide()/*Destroy()*/;
    }
};

static long df2wxds(dialog_t::flags1_t v)
{
    long ret = wxDEFAULT_DIALOG_STYLE;
    if (v*dialog_t::RESIZEABLE) ret |= wxRESIZE_BORDER;
    if (v*dialog_t::MINIMIZEABLE) ret |= wxMINIMIZE_BOX;
    if (v*dialog_t::MAXIMIZEABLE) ret |= wxMAXIMIZE_BOX;
    if (v*dialog_t::NO_CLOSE) ret &= (~(wxCLOSE_BOX));
    if (v*dialog_t::NO_SYSMENU) ret &= (~(wxSYSTEM_MENU));
    if (v*dialog_t::NO_CAPTION) ret &= (~(wxCAPTION));
    return ret;
}
} // namespace <anonymous>

dialog_t::dialog_t(topLevelWindow_t& owner, derived_t)
    : parent_t(owner.app(), &owner)
{
    // all set in derived class
}

dialog_t::dialog_t(topLevelWindow_t& owner, options_t setup)
    : parent_t(owner.app(), &owner)
{
    wrDialog* tmp = new wrDialog(*this, GET<wxTopLevelWindow>::from(&owner), wxID_ANY, s2wxs<string_t>::cvt(setup.Text), wxPoint(setup.Position.Column, setup.Position.Row), wxSize(setup.Size.Width, setup.Size.Height), df2wxds(setup));
    set_native_pointer(static_cast<wxDialog*>(tmp));
}

dialog_t::dialog_t(application_t& app, options_t setup)
    : parent_t(app, nullptr)
{
    wrDialog* tmp = new wrDialog(*this, nullptr, wxID_ANY, s2wxs<string_t>::cvt(setup.Text), wxPoint(setup.Position.Column, setup.Position.Row), wxSize(setup.Size.Width, setup.Size.Height), df2wxds(setup));
    set_native_pointer(static_cast<wxDialog*>(tmp));
}

dialog_t::~dialog_t()
{
    if (!native_t::native_pointer())
        return;

    wxDialog* tmp = GET<wxDialog>::from(this);
    nativeWrapper_base_t* twr = dynamic_cast<nativeWrapper_base_t*>(tmp);
    if (twr)
        twr->disown();
    tmp->Destroy();
    reset_native_pointer();
}

void dialog_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxTopLevelWindow>::from(this));
}

void dialog_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

modal_result_t dialog_t::show_modal()
{
    int ret = GET<wxDialog>::from(this)->ShowModal();
    if (ret > wxID_LOWEST)
        return modal_result_t(wx2stock(ret));
    else
        return modal_result_t(ret);
}

void dialog_t::end_modal(modal_result_t ret)
{
    int v = (ret.isRegular() ? ret.regular() : stock2wx(ret.stock()));
    GET<wxDialog>::from(this)->EndModal(v);
}

namespace dlg
{
namespace // <anonymous>
{
struct wrMessageDialog : public nativeWrapper_t<simple_t, wxMessageDialog>
{
    template<typename ... Ps>
    wrMessageDialog(simple_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }
};

long i2wxmbs(const simple_t::icon1_t& v)
{
    switch (v.Value)
    {
    default:
    case stock::icon_t::NONE: return wxICON_NONE;
    case stock::icon_t::INFO: return wxICON_INFORMATION;
    case stock::icon_t::QUESTION: return wxICON_QUESTION;
    case stock::icon_t::EXCLAMATION: return wxICON_EXCLAMATION;
    case stock::icon_t::ERR: return wxICON_ERROR;
    case stock::icon_t::NEED_AUTH: return wxICON_AUTH_NEEDED;
    }
}
long b2wxmbs(const std::initializer_list<stock::item_t>& v)
{
    long ret = 0;
    for (auto i : v)
    {
        if (i == stock::item_t::OK) { ret |= wxOK; continue; }
        if (i == stock::item_t::CANCEL) { ret |= wxCANCEL; continue; }
        if (i == stock::item_t::YES) { ret |= wxYES_NO; continue; }
        if (i == stock::item_t::NO) { ret |= wxYES_NO; continue; }
    }
    return ret;
}
long d2wxmbs(const simple_t::default1_t& v)
{
    switch (v.Value)
    {
    default:
    case stock::item_t::OK: return wxOK_DEFAULT;
    case stock::item_t::CANCEL: return wxCANCEL_DEFAULT;
    case stock::item_t::YES: return wxYES_DEFAULT;
    case stock::item_t::NO: return wxNO_DEFAULT;
    }
}

} // namespace <anonymous>

simple_t::simple_t(topLevelWindow_t& parent, const string_t& message, const string_t& title, options_t setup, std::initializer_list<stock::item_t> buttons)
    : parent_t(parent, DERIVED)
{
    wrMessageDialog* tmp = new wrMessageDialog(*this, GET<wxTopLevelWindow>::from(&parent), s2wxs<string_t>::cvt(message), s2wxs<string_t>::cvt(title), i2wxmbs(setup) | b2wxmbs(buttons) | d2wxmbs(setup), wxPoint(setup.Position.Column, setup.Position.Row));
    set_native_pointer(static_cast<wxMessageDialog*>(tmp));
}

void simple_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxDialog>::from(this));
}

void simple_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

namespace // <anonymous>
{
struct wrTextEntryDialog : public nativeWrapper_t<input_t, wxTextEntryDialog>
{
    template<typename ... Ps>
    wrTextEntryDialog(input_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }
};
struct wrPasswordEntryDialog : public nativeWrapper_t<input_t, wxPasswordEntryDialog>
{
    template<typename ... Ps>
    wrPasswordEntryDialog(input_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }
};
} // namespace <anonymous>

input_t::input_t(topLevelWindow_t& parent, const string_t& message, options_t setup)
    : parent_t(parent, DERIVED)
{
    const flags1_t& flags = setup;
    if (flags*PASSWORD)
    {
        isPsw_ = true;
        wrPasswordEntryDialog* tmp = new wrPasswordEntryDialog(
            *this, GET<wxTopLevelWindow>::from(&parent),
            s2wxs<string_t>::cvt(message), (setup.Title.empty() ? wxGetPasswordFromUserPromptStr : s2wxs<string_t>::cvt(setup.Title)),
            s2wxs<string_t>::cvt(setup.Text), wxTextEntryDialogStyle,
            wxPoint(setup.Position.Column, setup.Position.Row)
        );
        set_native_pointer(static_cast<wxPasswordEntryDialog*>(tmp));
    }
    else
    {
        isPsw_ = false;
        wrTextEntryDialog* tmp = new wrTextEntryDialog(
            *this, GET<wxTopLevelWindow>::from(&parent),
            s2wxs<string_t>::cvt(message), (setup.Title.empty() ? wxGetTextFromUserPromptStr : s2wxs<string_t>::cvt(setup.Title)),
            s2wxs<string_t>::cvt(setup.Text), wxTextEntryDialogStyle,
            wxPoint(setup.Position.Column, setup.Position.Row)
        );
        set_native_pointer(static_cast<wxTextEntryDialog*>(tmp));
    }
}

void input_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    if (isPsw_)
        parent_t::set_native_pointer(GET<wxDialog>::AS<wxPasswordEntryDialog>::from(this));
    else
        parent_t::set_native_pointer(GET<wxDialog>::AS<wxTextEntryDialog>::from(this));
}

void input_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

string_t input_t::text() const
{
    wxTextEntryDialog* d = (isPsw_ ? GET<wxTextEntryDialog>::AS<wxPasswordEntryDialog>::from(this) : GET<wxTextEntryDialog>::AS<wxTextEntryDialog>::from(this));
    return wxs2s<string_t>::cvt(d->GetValue());
}

namespace // <anonymous>
{
template<typename T>
struct wrFileDialog : public nativeWrapper_t<T, wxFileDialog>
{
    template<typename ... Ps>
    wrFileDialog(T& owner, Ps... ps)
        : nativeWrapper_t<T, wxFileDialog>(owner, ps...)
    {
    }
};
struct wrDirDialog : public nativeWrapper_t<selectDir_t, wxDirDialog>
{
    template<typename ... Ps>
    wrDirDialog(selectDir_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }
};
long off2wxfds(const openFile_t::flags1_t& v)
{
    long ret = 0;
    if (v*openFile_t::MUST_EXIST) ret |= wxFD_FILE_MUST_EXIST;
    if (v*openFile_t::MULTIPLE) ret |= wxFD_MULTIPLE;
    if (v*openFile_t::CHANGE_DIR) ret |= wxFD_CHANGE_DIR;
    if (v*openFile_t::PREVIEW) ret |= wxFD_PREVIEW;
    return ret;
}
long sff2wxfds(const saveFile_t::flags1_t& v)
{
    long ret = 0;
    if (v*saveFile_t::OVERWRITE) ret |= wxFD_OVERWRITE_PROMPT;
    if (v*saveFile_t::CHANGE_DIR) ret |= wxFD_CHANGE_DIR;
    if (v*saveFile_t::PREVIEW) ret |= wxFD_PREVIEW;
    return ret;
}
long sdf2wxdds(const selectDir_t::flags1_t& v)
{
    long ret = wxDD_DEFAULT_STYLE;
    if (v*selectDir_t::MUST_EXIST) ret |= wxDD_DIR_MUST_EXIST;
    if (v*selectDir_t::CHANGE_DIR) ret |= wxDD_CHANGE_DIR;
    return ret;
}
} // namespace <anonymous>

openFile_t::openFile_t(topLevelWindow_t& parent, options_t setup)
    : parent_t(parent, DERIVED)
{
    wrFileDialog<openFile_t>* tmp = new wrFileDialog<openFile_t>(
        *this, GET<wxTopLevelWindow>::from(&parent),
        setup.Title.empty() ? wxFileSelectorPromptStr : s2wxs<string_t>::cvt(setup.Title),
        s2wxs<string_t>::cvt(setup.Directory), s2wxs<string_t>::cvt(setup.File),
        setup.Wildcard.empty() ? wxFileSelectorDefaultWildcardStr : s2wxs<string_t>::cvt(setup.Wildcard),
        wxFD_OPEN | off2wxfds(setup),
        wxPoint(setup.Position.Column, setup.Position.Row),
        wxSize(setup.Size.Width, setup.Size.Height)
        );
    set_native_pointer(static_cast<wxFileDialog*>(tmp));
}

void openFile_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxDialog>::from(this));
}

void openFile_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

string_t openFile_t::file() const
{
    return wxs2s<string_t>::cvt(GET<wxFileDialog>::from(this)->GetPath());
}

std::vector<string_t> openFile_t::files() const
{
    wxArrayString fns;
    GET<wxFileDialog>::from(this)->GetPaths(fns);
    std::vector<string_t> ret;
    for (const wxString& fn : fns)
        ret.push_back(wxs2s<string_t>::cvt(fn));
    return ret;
}

saveFile_t::saveFile_t(topLevelWindow_t& parent, options_t setup)
    : parent_t(parent, DERIVED)
{
    wrFileDialog<saveFile_t>* tmp = new wrFileDialog<saveFile_t>(
        *this, GET<wxTopLevelWindow>::from(&parent),
        setup.Title.empty() ? wxFileSelectorPromptStr : s2wxs<string_t>::cvt(setup.Title),
        s2wxs<string_t>::cvt(setup.Directory), s2wxs<string_t>::cvt(setup.File),
        setup.Wildcard.empty() ? wxFileSelectorDefaultWildcardStr : s2wxs<string_t>::cvt(setup.Wildcard),
        wxFD_SAVE | sff2wxfds(setup),
        wxPoint(setup.Position.Column, setup.Position.Row),
        wxSize(setup.Size.Width, setup.Size.Height)
        );
    set_native_pointer(static_cast<wxFileDialog*>(tmp));
}

void saveFile_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxDialog>::from(this));
}

void saveFile_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

string_t saveFile_t::file() const
{
    return wxs2s<string_t>::cvt(GET<wxFileDialog>::from(this)->GetPath());
}

selectDir_t::selectDir_t(topLevelWindow_t& parent, options_t setup)
    : parent_t(parent, DERIVED)
{
    wrDirDialog* tmp = new wrDirDialog(
        *this, GET<wxTopLevelWindow>::from(&parent),
        setup.Title.empty() ? wxDirSelectorPromptStr : s2wxs<string_t>::cvt(setup.Title),
        s2wxs<string_t>::cvt(setup.Directory), sdf2wxdds(setup),
        wxPoint(setup.Position.Column, setup.Position.Row),
        wxSize(setup.Size.Width, setup.Size.Height)
        );
    set_native_pointer(static_cast<wxDirDialog*>(tmp));
}

void selectDir_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxDialog>::from(this));
}
void selectDir_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

string_t selectDir_t::dir() const
{
    return wxs2s<string_t>::cvt(GET<wxDirDialog>::from(this)->GetPath());
}

} // namespace dlg
} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
