#ifndef JJ_GUI_KEYS_H
#define JJ_GUI_KEYS_H

#include "jj/defines.h"

#if (JJ_USE_GUI)

#include "jj/string.h"

namespace jj
{
namespace gui
{
namespace keys
{

/*! Defines available modifier keys (and their combinations). */
enum mod_t
{
    NO_ACCELERATOR = 0,

    SHIFT = 1,
    ALT = 2,
    CTRL = 4,

    SHIFT_ALT = 3,
    SHIFT_CTRL = 5,
    CTRL_ALT = 6,
    SHIFT_CTRL_ALT = 7
};

// TODO enum modex_t { };

/*! Special keys. */
enum special_t { NOKEY, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12 };

/*! Defines a key combination (one key and modifiers). */
struct accelerator_t
{
    /*! Ctor */
    accelerator_t() : type_(SPECIAL), mod_(NO_ACCELERATOR), skey_(NOKEY) {}
    /*! Ctor */
    accelerator_t(char_t ch) : type_(REGULAR), mod_(NO_ACCELERATOR), rkey_(ch) {}
    /*! Ctor */
    accelerator_t(mod_t mod, char_t ch) : type_(REGULAR), mod_(mod), rkey_(ch) {}
    /*! Ctor */
    accelerator_t(special_t sk) : type_(SPECIAL), mod_(NO_ACCELERATOR), skey_(sk) {}
    /*! Ctor */
    accelerator_t(mod_t mod, special_t sk) : type_(SPECIAL), mod_(mod), skey_(sk) {}

    /*! Returns true if key is not set. */
    bool empty() const { return type_ == SPECIAL && skey_ == NOKEY; }
    /*! Returns true if key is a regular character. */
    bool regular() const { return type_ == REGULAR; }
    /*! Returns the modifier. */
    mod_t modifier() const { return mod_; }
    /*! Returns the key code if it is a regular key. */
    char_t key() const { return rkey_; }
    /*! Returns the key code if it is a special key. */
    special_t special_key() const { return skey_; }

private:
    enum type_t { REGULAR, SPECIAL };
    type_t type_;
    mod_t mod_;
    union {
        char_t rkey_;
        special_t skey_;
    };
};

} // namespace keys
} // namespace gui

namespace opt
{
/*! An option defining a keyboard shortcut. */
struct accelerator { gui::keys::accelerator_t Accelerator; accelerator() {} accelerator(gui::keys::accelerator_t v) : Accelerator(v) {} };
} // namespace opt
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_KEYS_H
