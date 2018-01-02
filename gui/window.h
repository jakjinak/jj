#ifndef JJ_GUI_WINDOW_H
#define JJ_GUI_WINDOW_H

#include "jj/gui/common.h"

#if (JJ_USE_GUI)

#include "jj/gui/menu.h"
#include "jj/gui/eventCallback.h"
#include "jj/gui/stock.h"

namespace jj
{
namespace opt
{
struct file { string_t File; file() {} file(string_t afile) : File(afile) {} };
struct directory { string_t Directory; directory() {} directory(string_t adir) : Directory(adir) {} };
struct wildcard { string_t Wildcard; wildcard() {} wildcard(string_t awildcard) : Wildcard(awildcard) {} };
} // namespace opt

namespace gui
{

class application_t;

class topLevelWindow_t : public nativePointerWrapper_t<topLevelWindow_t>, public contentHolder_t
{
    typedef contentHolder_t parent_t;

    application_t& app_;
    topLevelWindow_t* owner_;

public:
    topLevelWindow_t(application_t& app, topLevelWindow_t* owner) : app_(app), owner_(owner) {}
    virtual ~topLevelWindow_t() {}

    typedef nativePointerWrapper_t<topLevelWindow_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    application_t& app() const { return app_; }

    string_t title() const;
    void title(const string_t& v);

    bool isShown() const;
    void show();
    void hide();
    void minimize();
    void maximize();
    void restore();

    bool close(bool force = false);

    eventCallback_t<bool, topLevelWindow_t&> OnClose;
};

class frame_t;

class menuBar_t : public nativePointerWrapper_t<menuBar_t>, public menu_t
{
    typedef menu_t parent_t;
    frame_t& owner_;

public:
    menuBar_t(frame_t& owner);

    typedef nativePointerWrapper_t<menuBar_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

class statusBar_t : public nativePointerWrapper_t<statusBar_t>
{
public:
    enum style_t { DEFAULT, FLAT, RAISED, SUNKEN};
    struct field_t
    {
        string_t text_;
        int width_;
        style_t style_;
        field_t();
        field_t(int width, style_t style = DEFAULT) : width_(width), style_(style) {}
        field_t(const string_t& text, style_t style = DEFAULT);
        field_t(const string_t& text, int width, style_t style = DEFAULT) : text_(text), width_(width), style_(style) {}
    };
    typedef std::vector<field_t> fields_t;

private:
    frame_t& owner_;

    statusBar_t(const statusBar_t&) = delete;
    statusBar_t(statusBar_t&&) = delete;
    statusBar_t& operator=(const statusBar_t&) = delete;
    statusBar_t& operator=(statusBar_t&&) = delete;
public:
    statusBar_t(frame_t& owner);
    statusBar_t(frame_t& owner, const fields_t& fields) : statusBar_t(owner) { set(fields); }

    typedef nativePointerWrapper_t<statusBar_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    size_t count() const;
    void resize(size_t size);
    void set(const fields_t& fields);
    fields_t get() const;
    
    string_t text(size_t i = 0);
    void text(const string_t& value) { text(0, value); }
    void text(size_t i, const string_t& value);
    void push(const string_t& value) { push(0, value); }
    void push(size_t i, const string_t& value);
    void pop(size_t i=0);

    int width(size_t i = 0);
    void width(size_t i, int value);

    style_t style(size_t i = 0);
    void style(size_t i, style_t value);
};


class frame_t : public nativePointerWrapper_t<frame_t>, public topLevelWindow_t
{
public:
    enum flags_t
    {
        NO_RESIZE,
        NO_MINIMIZE,
        NO_MAXIMIZE,
        NO_CLOSE,
        NO_SYSMENU,
        NO_CAPTION,
        NO_TASKBAR,
        TOOL_WINDOW,
        SHAPED,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef options_T<opt::text, opt::position, opt::size, flags1_t> options_t;
    static options_t options() { return options_t(); }

private:
    typedef topLevelWindow_t parent_t;

    menuBar_t* menuBar_;
    void set_menu_bar();
    statusBar_t* statusBar_;
    void set_status_bar();

protected:
    std::function<menuBar_t* ()> onCreateMenuBar;
    std::function<statusBar_t* ()> onCreateStatusBar;

public:
    frame_t(application_t& app, options_t setup);
    frame_t(application_t& app, topLevelWindow_t& owner, options_t setup);
    ~frame_t();

    typedef nativePointerWrapper_t<frame_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    menuBar_t& menu_bar()
    {
        if (menuBar_ == nullptr)
        {
            menuBar_ = onCreateMenuBar();
            set_menu_bar();
        }
        return *menuBar_;
    }

    statusBar_t& status_bar()
    {
        if (statusBar_ == nullptr)
        {
            statusBar_ = onCreateStatusBar();
            set_status_bar();
        }
        return *statusBar_;
    }
};

class modal_result_t
{
    enum type_t { REGULAR, STOCK };
    type_t type_;
    union
    {
        int r; //!< regular result
        stock::item_t s; //!< stock result
    };

public:
    modal_result_t(int v) : type_(REGULAR), r(v) {}
    modal_result_t(stock::item_t v) : type_(STOCK), s(v) {}

    bool isRegular() const { return type_ == REGULAR; }
    bool isStock() const { return type_ == STOCK; }
    int regular() const { return r; }
    stock::item_t stock() const { return s; }

    bool operator==(int v) const
    {
        if (type_ == STOCK)
            return false;
        return r == v;
    }
    bool operator==(stock::item_t v) const
    {
        if (type_ == REGULAR)
            return false;
        return s == v;
    }
};

class dialog_t : public nativePointerWrapper_t<dialog_t>, public topLevelWindow_t
{
public:
    enum flags_t
    {
        RESIZEABLE,
        MINIMIZEABLE,
        MAXIMIZEABLE,
        NO_CLOSE,
        NO_SYSMENU,
        NO_CAPTION,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef options_T<opt::text, opt::position, opt::size, flags1_t> options_t;
    static options_t options() { return options_t(); }

private:
    typedef topLevelWindow_t parent_t;

protected:
    enum derived_t { DERIVED };
    dialog_t(topLevelWindow_t& owner, derived_t);
public:
    dialog_t(topLevelWindow_t& owner, options_t setup);
    dialog_t(application_t& app, options_t setup);
    ~dialog_t();

    typedef nativePointerWrapper_t<dialog_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    modal_result_t show_modal();
    void end_modal(modal_result_t ret);
};

namespace dlg
{

/*! A simple dialog with only a message (+icon) and stock buttons.
The buttons can be a combination of OK, CANCEL, YES, NO. OK is mutually exclusive with YES and NO;
both YES and NO implicitly add also the other one (it is enough to specify one of them). 
\note Call show_modal() to show the dialog. */
class simple_t : public nativePointerWrapper_t<simple_t>, public dialog_t
{
public:
    typedef opt::e<stock::icon_t> icon1_t;
    typedef opt::e<stock::item_t> default1_t;
    typedef jj::options_T<opt::position, icon1_t, default1_t> options_t;
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;

public:
    simple_t(topLevelWindow_t& parent, const string_t& message, const string_t& title, options_t setup, std::initializer_list<stock::item_t> buttons);

    typedef nativePointerWrapper_t<simple_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

/*! A dialog with OK, CANCEL and one text input field.
\note Call show_modal() to show the dialog. */
class input_t : public nativePointerWrapper_t<input_t>, public dialog_t
{
public:
    enum flags_t
    {
        PASSWORD,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef jj::options_T<opt::position, opt::text, opt::title, flags1_t> options_t;
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;
    bool isPsw_;

public:
    input_t(topLevelWindow_t& parent, const string_t& message, options_t setup);

    typedef nativePointerWrapper_t<input_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    string_t text() const;
};

class openFile_t : public nativePointerWrapper_t<openFile_t>, public dialog_t
{
public:
    enum flags_t
    {
        MUST_EXIST,
        MULTIPLE,
        CHANGE_DIR,
        PREVIEW,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef jj::options_T<opt::position, opt::size, opt::title, opt::file, opt::directory, opt::wildcard, flags1_t> options_t;
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;

public:
    openFile_t(topLevelWindow_t& parent, options_t setup);

    typedef nativePointerWrapper_t<openFile_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    string_t file() const;
    std::vector<string_t> files() const;
};

class saveFile_t : public nativePointerWrapper_t<saveFile_t>, public dialog_t
{
public:
    enum flags_t
    {
        OVERWRITE,
        CHANGE_DIR,
        PREVIEW,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef jj::options_T<opt::position, opt::size, opt::title, opt::file, opt::directory, opt::wildcard, flags1_t> options_t;
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;

public:
    saveFile_t(topLevelWindow_t& parent, options_t setup);

    typedef nativePointerWrapper_t<saveFile_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    string_t file() const;
};

class selectDir_t : public nativePointerWrapper_t<selectDir_t>, public dialog_t
{
public:
    enum flags_t
    {
        MUST_EXIST,
        CHANGE_DIR,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef jj::options_T<opt::position, opt::size, opt::title, opt::directory, flags1_t> options_t;
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;

public:
    selectDir_t(topLevelWindow_t& parent, options_t setup);

    typedef nativePointerWrapper_t<selectDir_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    string_t dir() const;
};

} // namespace dlg

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_WINDOW_H
