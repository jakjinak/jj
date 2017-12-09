#ifndef JJ_GUI_WINDOW_H
#define JJ_GUI_WINDOW_H

#include "jj/gui/common.h"

#if (JJ_USE_GUI)

#include "jj/gui/menu.h"
#include "jj/gui/eventCallback.h"

namespace jj
{
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

class dialog_t : public topLevelWindow_t
{
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
    typedef creationOptions_t<opt::text, opt::position, opt::size, flags1_t> options_t;
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

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_WINDOW_H
