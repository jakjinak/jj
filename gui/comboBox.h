#ifndef JJ_GUI_COMBO_BOX_H
#define JJ_GUI_COMBO_BOX_H

#include "jj/gui/control.h"

#if (JJ_USE_GUI)

#include "jj/gui/eventCallback.h"

namespace jj
{
namespace gui
{

class comboBox_t : public nativePointerWrapper_t<comboBox_t>, public control_t
{
public:
    enum flags_t
    {
        EDITABLE,
        MAX_FLAGS
    };
    typedef opt::f<flags_t, MAX_FLAGS> flags1_t;
    typedef creationOptions_t<opt::text, opt::position, opt::size, flags1_t> options_t;
    static options_t options() { return options_t(); }

private:
    typedef control_t parent_t;

public:
    comboBox_t(contentHolder_t& owner, options_t setup) : comboBox_t(owner, setup, {}) {}
    comboBox_t(contentHolder_t& owner, const std::initializer_list<string_t>& items) : comboBox_t(owner, options(), items) {}
    comboBox_t(contentHolder_t& owner, options_t setup, const std::initializer_list<string_t>& items);

    typedef nativePointerWrapper_t<comboBox_t> native_t;
    void set_native_pointer(void* ptr);
    void reset_native_pointer();

    static size_t npos;
    size_t count() const;
    bool is_selected() const;
    size_t selected() const;
    void select(size_t pos);

    string_t text() const;
    void text(const string_t& v);
    void changeText(const string_t& v);

    void append(const string_t& item);
    void insert(const string_t& item, size_t pos = 0);
    void change(const string_t& item, size_t pos);
    void erase(size_t pos);

    eventCallback_t<void, comboBox_t&> OnSelect;
    eventCallback_t<void, comboBox_t&> OnChange;
};

} // namespace gui
} // namespace jj

#endif // JJ_USE_GUI

#endif // JJ_GUI_COMBO_BOX_H
