#ifndef JJ_GUI_WINDOW_H
#define JJ_GUI_WINDOW_H

#include "jj/gui/common.h"

#if (JJ_USE_GUI)

#include "jj/gui/menu.h"
#include "jj/functionBag.h"
#include "jj/gui/stock.h"

namespace jj
{
namespace opt
{
/*! File dialogs' specific option. */
struct file { string_t File; file() {} file(string_t afile) : File(afile) {} };
/*! Directory dialogs' specific option. */
struct directory { string_t Directory; directory() {} directory(string_t adir) : Directory(adir) {} };
/*! File dialogs' specific option. */
struct wildcard { string_t Wildcard; wildcard() {} wildcard(string_t awildcard) : Wildcard(awildcard) {} };
} // namespace opt

namespace gui
{

// forward declaration
class application_t;
class frame_t;

/*! Base class for a window that runs on its own (not within other windows) - typically a frame or dialog. */
class topLevelWindow_t : public AUX::nativePointerWrapper_t<topLevelWindow_t>, public contentHolder_t
{
    typedef contentHolder_t parent_t;

    application_t& app_; //!< the application in which the window is running
    topLevelWindow_t* owner_; //!< "master" top level window (if any)

public:
    /*! Ctor - provide the application instance and a "master" window (if any) */
    topLevelWindow_t(application_t& app, topLevelWindow_t* owner) : app_(app), owner_(owner) {}
    /*! Dtor */
    virtual ~topLevelWindow_t() {}

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<topLevelWindow_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Returns application associated with the window. */
    application_t& app() const { return app_; }

    /*! Returns the window's title text. */
    string_t title() const;
    /*! Sets the window's title text. */
    void title(const string_t& v);

    /*! Returns whether the window is currently in visible state; ie. not hidden (but this really means hidden not minimized!) */
    bool isShown() const;
    /*! Changes the window to visible state. */
    void show();
    /*! Changes the window to hidden state. */
    void hide();
    /*! Changes the window to minimized state. */
    void minimize();
    /*! Changes the window to maximized state. */
    void maximize();
    /*! Restores the window from minimized/maximized state. */
    void restore();

    /*! Closes the window or actually invokes the OnClose which determines whether the window will really be closed.
    Given force true overrides the OnClose result. */
    bool close(bool force = false);

    /*! Called when the window is about to be closed (and destroyed), return true to allow the action, false to veto it.
    Veto can be overriden in close(). */
    functionBag_t<bool, topLevelWindow_t&> OnClose;
};

/*! Represents frame's main menu bar.
Note that only a frame can have a bar, it's not possible in a dialog. */
class menuBar_t : public AUX::nativePointerWrapper_t<menuBar_t>, public menu_t
{
    typedef menu_t parent_t;
    frame_t& owner_; //!< the parent frame

public:
    /*! Ctor - needs a valid owning frame. */
    menuBar_t(frame_t& owner);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<menuBar_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

/*! Represents frame's status bar. */
class statusBar_t : public AUX::nativePointerWrapper_t<statusBar_t>
{
public:
    /*! Visual style of bar' field*/
    enum style_t
    {
        DEFAULT, //!< do not care, use what's usual for system
        FLAT, //!< no "3D"
        RAISED, //!< raised
        SUNKEN //!< sunken
    };
    /*! Abstracts a status bar field. */
    struct field_t
    {
        string_t Text; //!< text to be shown in field
        int Width; //!< width of the field; positive number gives absolute width in pixels, negative gives relative proportion to other fields with negative width
        style_t Style; //!< visual style of field

        /*! Ctor - empty default-styled field */
        field_t();
        /*! Ctor - no text */
        field_t(int width, style_t style = DEFAULT) : Width(width), Style(style) {}
        /*! Ctor */
        field_t(const string_t& text, style_t style = DEFAULT);
        /*! Ctor */
        field_t(const string_t& text, int width, style_t style = DEFAULT) : Text(text), Width(width), Style(style) {}
    };
    typedef std::vector<field_t> fields_t; //!< denotes a type holding all fields in a bar

private:
    frame_t& owner_; //!< the parent frame

    statusBar_t(const statusBar_t&) = delete;
    statusBar_t(statusBar_t&&) = delete;
    statusBar_t& operator=(const statusBar_t&) = delete;
    statusBar_t& operator=(statusBar_t&&) = delete;
public:
    /*! Ctor*/
    statusBar_t(frame_t& owner);
    /*! Ctor*/
    statusBar_t(frame_t& owner, const fields_t& fields) : statusBar_t(owner) { set(fields); }

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<statusBar_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Returns number of fields in the bar. */
    size_t count() const;
    /*! Change the number of fields in bar; extraneous fields are removed, missing fields are default created */
    void resize(size_t size);
    /*! Change all fields at once. */
    void set(const fields_t& fields);
    /*! Get all fields' data. */
    fields_t get() const;
    
    /*! Get label of the i-th field. The i must fit into count(). */
    string_t text(size_t i = 0);
    /*! Set label of the 1st field. */
    void text(const string_t& value) { text(0, value); }
    /*! Set label of the i-th field. The i must fit into count(). */
    void text(size_t i, const string_t& value);
    /*! Temporarily set label of the 1st field, revert with pop(). */
    void push(const string_t& value) { push(0, value); }
    /*! Temporarily set label of the i-th field, revert with pop(). The i must fit into count(). */
    void push(size_t i, const string_t& value);
    /*! Revert label of the i-th field temporarily set by push() to its previous value. The i must fit into count(). */
    void pop(size_t i=0);

    /*! Get width of the i-th field. The i must fit into count(). */
    int width(size_t i = 0);
    /*! Set width of the i-th field. The i must fit into count(). */
    void width(size_t i, int value);

    /*! Get style of the i-th field. The i must fit into count(). */
    style_t style(size_t i = 0);
    /*! Set style of the i-th field. The i must fit into count(). */
    void style(size_t i, style_t value);
};

/*! Frame usually represents the main window of an application. */
class frame_t : public AUX::nativePointerWrapper_t<frame_t>, public topLevelWindow_t
{
public:
    /*! Available styles for a frame. */
    enum flags_t
    {
        NO_RESIZE, //!< prevent the border to be resizable
        NO_MINIMIZE, //!< disable the minimize button
        NO_MAXIMIZE, //!< disable the maximize button
        NO_CLOSE, //!< disable the close button
        NO_SYSMENU, //!< disable the system menu
        NO_TITLEBAR, //!< remove the title bar from the window
        NO_TASKBAR, //!< hide the frame from taskbar (where applicable)
        TOOL_WINDOW, //!< changes the visuals of the window (to a smaller title bar, different buttons, ...), implies NO_TASKBAR
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t; //!< type holding frame's flags
    typedef options_T<opt::text, opt::position, opt::size, flags1_t> options_t; //!< defines all available frame options
    /*! Returns default frame options. */
    static options_t options() { return options_t(); }

private:
    typedef topLevelWindow_t parent_t;

    menuBar_t* menuBar_; //!< the main menu (as soon as created)
    /*! Helper - installs the menu bar into the frame. */
    void set_menu_bar();
    statusBar_t* statusBar_; //!< the status bar (as soon as created)
    /*! Helper - installs the status bar into the frame. */
    void set_status_bar();

protected:
    /*! Creates the menu bar. Invoked when the frame's menubar is first requested by menu_bar(), set in derived class to provide customized bar. */
    std::function<menuBar_t* ()> OnCreateMenuBar;
    /*! Creates the status bar. Invoked when the frame's status bar is first requested by status_bar(), set in derived class to provide customized bar. */
    std::function<statusBar_t* ()> OnCreateStatusBar; //

public:
    /*! Ctor - the "master" window */
    frame_t(application_t& app, options_t setup);
    /*! Ctor - a child (but still top-level) window */
    frame_t(application_t& app, topLevelWindow_t& owner, options_t setup);
    /*! Dtor */
    ~frame_t();

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<frame_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Return the frame's menu bar.
    There can be only one in the frame which is auto-created in OnCreateMenuBar, override that to provide a customized one. */
    menuBar_t& menu_bar()
    {
        if (menuBar_ == nullptr)
        {
            menuBar_ = OnCreateMenuBar();
            set_menu_bar();
        }
        return *menuBar_;
    }

    /*! Return the frame's status bar.
    There can be only one in the frame which is auto-created in OnCreateStatusBar, override that to provide a customized one. */
    statusBar_t& status_bar()
    {
        if (statusBar_ == nullptr)
        {
            statusBar_ = OnCreateStatusBar();
            set_status_bar();
        }
        return *statusBar_;
    }
};

/*! Represents a result returned by dialog_t::show_modal(), ie. (simplified) which button was pressed to close the dialog. */
class modal_result_t
{
    enum type_t
    {
        REGULAR, //!< the internal value refers to a custom integer value
        STOCK //!< the internal value refers to a stock result
    };
    type_t type_; //!< regular/stock value stored in union
    union
    {
        int r; //!< regular result
        stock::item_t s; //!< stock result
    };

public:
    /*! Ctor - regular result */
    modal_result_t(int v) : type_(REGULAR), r(v) {}
    /*! Ctor - stock result */
    modal_result_t(stock::item_t v) : type_(STOCK), s(v) {}

    /*! If this returns true then regular() shall be used to retrieve value, other stock() shall be used. */
    bool isRegular() const { return type_ == REGULAR; }
    /*! If this returns true then stock() shall be used to retrieve value, other regular() shall be used. */
    bool isStock() const { return type_ == STOCK; }
    /*! Returns the value if it is regular. */
    int regular() const { return r; }
    /*! Returns the value if it is stock. */
    stock::item_t stock() const { return s; }

    /*! Compare with regular value. */
    bool operator==(int v) const
    {
        if (type_ == STOCK)
            return false;
        return r == v;
    }
    /*! Compare with stock value. */
    bool operator==(stock::item_t v) const
    {
        if (type_ == REGULAR)
            return false;
        return s == v;
    }
};

/*! Usually represents a "modal" window, which blocks the underlying caller window. */
class dialog_t : public AUX::nativePointerWrapper_t<dialog_t>, public topLevelWindow_t
{
public:
    /*! Available styles for a dialog. */
    enum flags_t
    {
        RESIZEABLE, //!< enables the border to be resizable
        MINIMIZEABLE, //!< allows the minimize button
        MAXIMIZEABLE, //!< allows the maximize button
        NO_CLOSE, //!< disable the close button
        NO_SYSMENU, //!< disable the system menu
        NO_TITLEBAR, //!< remove the title bar from the window
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t; //!< type holding dialog's flags
    typedef options_T<opt::text, opt::position, opt::size, flags1_t> options_t; //!< defines all available dialog options
    /*! Returns default dialog options. */
    static options_t options() { return options_t(); }

private:
    typedef topLevelWindow_t parent_t;

protected:
    /*! Helper used in special ctor used by derived classes. */
    enum derived_t { DERIVED };
    dialog_t(topLevelWindow_t& owner, derived_t);
public:
    /*! Ctor - child of another top level (usually frame) */
    dialog_t(topLevelWindow_t& owner, options_t setup);
    /*! Ctor - "master" window */
    dialog_t(application_t& app, options_t setup);
    /*! Dtor */
    ~dialog_t();

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<dialog_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Show the dialog so that it "blocks" (rather disables) the caller until dialog is closed.
    Returns which event caused the dialog to close. */
    modal_result_t show_modal();
    /*! Explicitly close the dialog opened by show_modal(), supply the return value of show_modal(). */
    void end_modal(modal_result_t ret);
};

namespace dlg
{

/*! A simple dialog with only a message (+icon) and stock buttons.
The buttons can be a combination of OK, CANCEL, YES, NO. OK is mutually exclusive with YES and NO;
both YES and NO implicitly add also the other one (it is enough to specify one of them). 
\note Call show_modal() to show the dialog. */
class simple_t : public AUX::nativePointerWrapper_t<simple_t>, public dialog_t
{
public:
    typedef opt::e<stock::icon_t> icon1_t;
    typedef opt::e<stock::item_t> default1_t;
    typedef jj::options_T<opt::position, icon1_t, default1_t> options_t; //!< defines all available dialog options
    /*! Returns default options of the dialog. */
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;

public:
    /*! Ctor */
    simple_t(topLevelWindow_t& parent, const string_t& message, const string_t& title, options_t setup, std::initializer_list<stock::item_t> buttons);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<simple_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();
};

/*! A dialog with OK, CANCEL and one text input field.
\note Call show_modal() to show the dialog. */
class input_t : public AUX::nativePointerWrapper_t<input_t>, public dialog_t
{
public:
    /*! Available flags modifying style of the dialog. */
    enum flags_t
    {
        PASSWORD, //!< print * (or system equivalent) instead of the characters typed in
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef jj::options_T<opt::position, opt::text, opt::title, flags1_t> options_t; //!< defines all available dialog options
    /*! Returns default options of the dialog. */
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;
    bool isPsw_;

public:
    input_t(topLevelWindow_t& parent, const string_t& message, options_t setup);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<input_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    string_t text() const;
};

/*! A dialog allowing to select one or more files on the filesystem. Meant to be used when files are "read".
\note Call show_modal() to show the dialog. */
class openFile_t : public AUX::nativePointerWrapper_t<openFile_t>, public dialog_t
{
public:
    /*! Available flags modifying style of the dialog. */
    enum flags_t
    {
        MUST_EXIST, //!< allow selecting only existing file
        MULTIPLE, //!< allow selection of multiple files
        CHANGE_DIR, //!< change the working directory to the one of the file(s) chosen after dialog is closed
        PREVIEW, //!< show preview pane
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef jj::options_T<opt::position, opt::size, opt::title, opt::file, opt::directory, opt::wildcard, flags1_t> options_t; //!< defines all available dialog options
    /*! Returns default options of the dialog. */
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;

public:
    /*! Ctor */
    openFile_t(topLevelWindow_t& parent, options_t setup);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<openFile_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Return the selected file (full path). */
    string_t file() const;
    /*! Return all files selected in dialog (full paths). */
    std::vector<string_t> files() const;
};

/*! A dialog allowing to select a file on the filesystem. Meant to be used when file is "written to". 
\note Call show_modal() to show the dialog. */
class saveFile_t : public AUX::nativePointerWrapper_t<saveFile_t>, public dialog_t
{
public:
    /*! Available flags modifying style of the dialog. */
    enum flags_t
    {
        OVERWRITE, //!< show "overwrite?" dialog if selected file exists
        CHANGE_DIR, //!< change the working directory to the one of the file chosen after dialog is closed
        PREVIEW, //!< show preview pane
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef jj::options_T<opt::position, opt::size, opt::title, opt::file, opt::directory, opt::wildcard, flags1_t> options_t; //!< defines all available dialog options
    /*! Returns default options of the dialog. */
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;

public:
    /*! Ctor */
    saveFile_t(topLevelWindow_t& parent, options_t setup);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<saveFile_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Return file selected in dialog (full path). */
    string_t file() const;
};

/*! A dialog allowing to select a directory on the file system. 
\note Call show_modal() to show the dialog. */
class selectDir_t : public AUX::nativePointerWrapper_t<selectDir_t>, public dialog_t
{
public:
    /*! Available flags modifying style of the dialog. */
    enum flags_t
    {
        MUST_EXIST, //!< only allow selecting existing directory
        CHANGE_DIR, //!< change the working directory to the one of the dir chosen after dialog is closed
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef jj::options_T<opt::position, opt::size, opt::title, opt::directory, flags1_t> options_t; //!< defines all available dialog options
    /*! Returns default options of the dialog. */
    static options_t options() { return options_t(); }

private:
    typedef dialog_t parent_t;

public:
    /*! Ctor */
    selectDir_t(topLevelWindow_t& parent, options_t setup);

    // IGNORE native pointer stuff, implementation specific
    typedef AUX::nativePointerWrapper_t<selectDir_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    /*! Return the directory selected in dialog (full path). */
    string_t dir() const;
};

} // namespace dlg

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_WINDOW_H
