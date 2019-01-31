#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "common_wx.h"
#include "comboBox.h"

namespace jj
{
namespace gui
{

namespace // <anonymous>
{
struct wrComboBox : public nativeWrapper_t<comboBox_t, wxComboBox>
{
    template<typename ... Ps>
    wrComboBox(comboBox_t& owner, Ps... ps)
        : nativeWrapper_t(owner, ps...)
    {
    }

    void evtSelect(wxCommandEvent& evt)
    {
        owner_->OnSelect(*owner_);
    }
    void evtChange(wxCommandEvent& evt)
    {
        owner_->OnChange(*owner_);
    }
};

static long cbf2wxcbs(const comboBox_t::flags1_t& v)
{
    long ret = wxCB_READONLY | wxCB_DROPDOWN;
    if (v*comboBox_t::EDITABLE) ret &= (~(wxCB_READONLY));
    return ret;
}

} // namespace <anonymous>

namespace AUX
{
template<> struct COGET<comboBox_t> { typedef wxComboBox TYPE; };
} // namespace AUX

comboBox_t::comboBox_t(contentHolder_t& owner, options_t setup, const std::initializer_list<string_t>& items)
    : parent_t(owner), native_t(*this)
{
    wxArrayString strs;
    for (auto& s : items)
        strs.Add(s2wxs<string_t>::cvt(s));

    wrComboBox* tmp = new wrComboBox(*this, GET<wxWindow>::from(&owner),
        get_id(), s2wxs<string_t>::cvt(setup.Text),
        wxPoint(setup.Position.Left, setup.Position.Top),
        wxSize(setup.Size.Width, setup.Size.Height),
        strs,
        cbf2wxcbs(setup)
    );
    set_native_pointer(static_cast<wxComboBox*>(tmp));

    jj::AUX::setup_first_last_callbacks(OnSelect,
        [this, tmp] { GET<wxComboBox>::from(this)->Bind(wxEVT_COMBOBOX, &wrComboBox::evtSelect, tmp, this->get_id()); },
        [this, tmp] { GET<wxComboBox>::from(this)->Unbind(wxEVT_COMBOBOX, &wrComboBox::evtSelect, tmp, this->get_id()); }
    );
    jj::AUX::setup_first_last_callbacks(OnChange,
        [this, tmp] { GET<wxComboBox>::from(this)->Bind(wxEVT_TEXT, &wrComboBox::evtChange, tmp, this->get_id()); },
        [this, tmp] { GET<wxComboBox>::from(this)->Unbind(wxEVT_TEXT, &wrComboBox::evtChange, tmp, this->get_id()); }
    );
}

void comboBox_t::set_native_pointer(void* ptr)
{
    native_t::set_native_pointer(ptr);
    parent_t::set_native_pointer(GET<wxControl>::from(this));
}

void comboBox_t::reset_native_pointer()
{
    native_t::reset_native_pointer();
    parent_t::reset_native_pointer();
}

size_t comboBox_t::npos = std::numeric_limits<size_t>::max();

size_t comboBox_t::count() const
{
    return GET<wxComboBox>::from(this)->GetCount();
}

bool comboBox_t::is_selected() const
{
    return GET<wxComboBox>::from(this)->GetSelection() != wxNOT_FOUND;
}

size_t comboBox_t::selected() const
{
    int pos = GET<wxComboBox>::from(this)->GetSelection();
    if (pos == wxNOT_FOUND)
        return npos;
    return size_t(pos);
}

void comboBox_t::select(size_t pos)
{
    GET<wxComboBox>::from(this)->SetSelection((pos == npos ? wxNOT_FOUND : int(pos)));
}

string_t comboBox_t::text() const
{
    return wxs2s<string_t>::cvt(GET<wxComboBox>::from(this)->GetValue());
}

void comboBox_t::text(const string_t& v)
{
    GET<wxComboBox>::from(this)->SetValue(s2wxs<string_t>::cvt(v));
}

void comboBox_t::changeText(const string_t& v)
{
    GET<wxComboBox>::from(this)->ChangeValue(s2wxs<string_t>::cvt(v));
}

void comboBox_t::append(const string_t& item)
{
    GET<wxComboBox>::from(this)->AppendString(s2wxs<string_t>::cvt(item));
}

void comboBox_t::insert(const string_t& item, size_t pos)
{
    GET<wxComboBox>::from(this)->Insert(s2wxs<string_t>::cvt(item), pos);
}

void comboBox_t::change(const string_t& item, size_t pos)
{
    GET<wxComboBox>::from(this)->SetString(pos, s2wxs<string_t>::cvt(item));
}

void comboBox_t::erase(size_t pos)
{
    GET<wxComboBox>::from(this)->Delete(pos);
}

} // namespace gui
} // namespace jj

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
