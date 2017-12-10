#include "jj/gui/application.h"
#include "jj/gui/window.h"
#include "jj/gui/textLabel.h"
#include "jj/gui/textInput.h"
#include "jj/gui/comboBox.h"
#include "jj/gui/button.h"
#include "jj/gui/sizer.h"
#include "jj/stream.h"
#include <sstream>

class wnd : public jj::gui::frame_t
{
    jj::gui::textInput_t* t1, *t2;
    jj::gui::button_t* b1, *b2, *b3, *b4;
    jj::gui::boxSizer_t* s1, *s2;
    jj::gui::dialog_t* dlg;

    void onb1(jj::gui::button_t&) {
        if (!o)
            return;

        if (o->isShown())
            o->hide();
        else
            o->show();
    }
    void onb2(jj::gui::button_t&) {
        if (!o)
            return;
        o->close();
        o = nullptr;
    }
    static jj::gui::statusBar_t::style_t cycleSB(jj::gui::statusBar_t::style_t style)
    {
        using namespace jj::gui;
        switch (style)
        {
        case statusBar_t::RAISED:
            return statusBar_t::FLAT;
        case statusBar_t::FLAT:
            return statusBar_t::SUNKEN;
        default:
            return statusBar_t::RAISED;
        }
    }
    void onb3(jj::gui::button_t&) {
        status_bar().style(2, cycleSB(status_bar().style(2)));
    }
    void onb4(jj::gui::button_t& b) {
        if (b.text() == jjT("B4 A"))
            b.text(jjT("B4 B"));
        else
            b.text(jjT("B4 A"));
    }
    void ontxt(jj::gui::textInput_t& x) {
        status_bar().text(x.text());
        if (!o)
            return;
        o->title(x.text());
    }
    void onradio(jj::gui::menuItem_t& x) {
        x.check();
    }

public:
    wnd(jj::gui::application_t& app)
        : jj::gui::frame_t(app, jj::gui::frame_t::options() << jj::gui::opt::text(jjT("First One")) << jj::gui::opt::size(600, 280) << jj::gui::frame_t::NO_MAXIMIZE)
        , o(nullptr), dlg(nullptr)
    {
        using namespace jj::gui;

        onCreateSizer = [this] { return new boxSizer_t(*this, boxSizer_t::VERTICAL); };

        textLabel_t* st = new textLabel_t(*this, textLabel_t::options() << opt::text(jjT("ST")));
        b1 = new button_t(*this, button_t::options() << opt::text(jjT("B1")));
        b1->OnClick.add(*this, &wnd::onb1);
        b2 = new button_t(*this, button_t::options() << opt::text(jjT("B2")) << button_t::EXACT_FIT);
        b2->OnClick.add(*this, &wnd::onb2);
        b3 = new button_t(*this, button_t::options() << opt::text(jjT("B3")) << align_t::LEFT << alignv_t::BOTTOM);
        b3->OnClick.add(*this, &wnd::onb3);
        b4 = new button_t(*this, button_t::options() << opt::text(jjT("B4 A")) << button_t::EXACT_FIT << button_t::NO_BORDER);
        b4->OnClick.add(*this, &wnd::onb4);
        t1 = new textInput_t(*this, textInput_t::options() << opt::text(jjT("Child")));
        t1->OnTextChange.add(*this, &wnd::ontxt);
        t2 = new textInput_t(*this, textInput_t::options() << textInput_t::MULTILINE);
        comboBox_t* cb = new comboBox_t(*this, { jjT("ABC"), jjT("XYZ"), jjT("123") });
        cb->OnSelect.add([this](comboBox_t& x) { t2->changeText(jjT("Selected [") + x.text() + jjT("]")); });
        sizer().add(*(s1 = new jj::gui::boxSizer_t(*this, boxSizer_t::HORIZONTAL)), sizerFlags_t().proportion(1).expand());
        sizer().add(*(s2 = new jj::gui::boxSizer_t(*this, boxSizer_t::HORIZONTAL)), sizerFlags_t().proportion(3).expand());
        s1->add(*st, sizerFlags_t().border(5));
        s1->add(*b1, sizerFlags_t().set(align_t::CENTER));
        s1->add(*b2, sizerFlags_t().set(align_t::CENTER));
        s1->add(*b3, sizerFlags_t().set(align_t::CENTER).set(sizerFlags_t::EXPAND));
        s1->add(*b4, sizerFlags_t().set(align_t::CENTER));
        s1->add(*t1, sizerFlags_t().set(align_t::CENTER).proportion(1));
        s1->add(*cb, sizerFlags_t().set(align_t::CENTER).proportion(1));
        s2->add(*t2, sizerFlags_t().proportion(1).expand());

        menu_t* m1, *m2, *m3, *m4;
        menu_bar().append(m1 = new menu_t(*this), jjT("TEST"));
        auto me = m1->append(menuItem_t::options() << opt::text(jjT("Exit")) << opt::accelerator(keys::accelerator_t(keys::ALT, keys::F4)));
        me.lock()->OnClick.add([this](menuItem_t&) { 
            this->close(); 
        });
        menu_bar().append(m2 = new menu_t(*this), menuItem_t::submenu_options() << opt::text(jjT("actions")));
        m2->append(m4 = new menu_t(*this), jjT("dialogs"));
        auto m41 = m4->append(menuItem_t::options() << menuItem_t::CHECK << opt::text(jjT("generic is modal")));
        auto m42 = m4->append(menuItem_t::options() << opt::text(jjT("generic")));
        m42.lock()->OnClick.add([this, m41](menuItem_t&) {
                this->dlg = new dialog_t(*this, dialog_t::options() << opt::text(jjT("The dialog")));
                if (m41.lock()->checked())
                    this->dlg->show_modal();
                else
                    this->dlg->show();
            });
        m4->append(menuItem_t::options() << opt::text(jjT("delete generic")))
            .lock()->OnClick.add([this](menuItem_t&) {
                delete this->dlg;
                this->dlg = nullptr;
            });
        m4->append(menuItem_t::SEPARATOR);
        m4->append(menuItem_t::options() << opt::text(jjT("simple")))
            .lock()->OnClick.add([this](menuItem_t&) {
                dlg::simple_t x(*this, jjT("So tell me now, what u want to do..."), jjT("Question for you"), dlg::simple_t::options() << stock::icon_t::ERR << stock::item_t::CANCEL, {/*stock::item_t::OK, */ stock::item_t::CANCEL, stock::item_t::YES });
                modal_result_t result = x.show_modal();
                if (result.isStock())
                    t2->text(jjS(jjT("The result was a stock result ") << int(result.stock())));
                else
                    t2->text(jjS(jjT("The result was a regular result ") << result.regular()));
            });
        m4->append(menuItem_t::options() << opt::text(jjT("text input")))
            .lock()->OnClick.add([this](menuItem_t&) {
                dlg::input_t x(*this, jjT("Give me text:"), dlg::input_t::options() << opt::text(jjT("default")));
                modal_result_t result = x.show_modal();
                if (result == stock::item_t::OK)
                    t2->text(jjS(jjT("The result was a stock OK: '") << x.text() << jjT("'")));
                else if (result == stock::item_t::CANCEL)
                    t2->text(jjT("The result was a stock CANCEL"));
                else if (result.isStock())
                    t2->text(jjS(jjT("The result was a stock result ") << int(result.stock())));
                else
                    t2->text(jjS(jjT("The result was a regular result ") << result.regular()));
            });
        m4->append(menuItem_t::options() << opt::text(jjT("password input")))
            .lock()->OnClick.add([this](menuItem_t&) {
            dlg::input_t x(*this, jjT("Give me password:"), dlg::input_t::options() << dlg::input_t::PASSWORD);
            modal_result_t result = x.show_modal();
            if (result == stock::item_t::OK)
                t2->text(jjS(jjT("The result was a stock OK: '") << x.text() << jjT("'")));
            else if (result == stock::item_t::CANCEL)
                t2->text(jjT("The result was a stock CANCEL"));
            else if (result.isStock())
                t2->text(jjS(jjT("The result was a stock result ") << int(result.stock())));
            else
                t2->text(jjS(jjT("The result was a regular result ") << result.regular()));
        });
        m2->append(m3 = new menu_t(*this), jjT("sub"));
        auto m22 = m2->append(menuItem_t::options() << opt::text(jjT("M2")) << menuItem_t::CHECK);
        m2->append(menuItem_t::options() << opt::text(jjT("M3")) << menuItem_t::CHECK)
            .lock()->OnClick.add([this, m22, st](menuItem_t& i) { 
                menuItem_t& mi = *m22.lock(); 
                mi.enable(i.checked());
                jj::string_t txt = mi.text();
                if (txt.length() < 15)
                {
                    mi.text(txt + jjT(" X"));
                    st->text(st->text() + jjT(" X"));
                }
                else
                {
                    mi.text(txt.substr(0, 2));
                    st->text(st->text().substr(0, 2));
                }
                this->layout();
            });
        m2->append(menuItem_t::SEPARATOR);
        m2->append(menuItem_t::options() << opt::text(jjT("M4")));
        auto m31 = m3->append(menuItem_t::options() << opt::text(jjT("S1")) << menuItem_t::RADIO);
        m31.lock()->OnClick.add(*this, &wnd::onradio);
        auto m32 = m3->append(menuItem_t::options() << opt::text(jjT("S2")) << menuItem_t::RADIO);
        m32.lock()->OnClick.add(*this, &wnd::onradio);
        status_bar().set({ {jjT("status"), -1, statusBar_t::RAISED}, {jjT("---"), -2, statusBar_t::FLAT}, {jjT("enjoy"), 60, statusBar_t::SUNKEN} });
        layout();
    }
    ~wnd()
    {
        if (o) o->close();
    }

    jj::gui::frame_t* o;
};

class app : public jj::gui::application_t
{
    wnd* f;

    bool ono(jj::gui::topLevelWindow_t&)
    {
        f->o = nullptr;
        return true;
    }

	virtual bool onInit()
	{
        //_CrtSetBreakAlloc(5684);
        f = new wnd(*this);
        f->show();

        f->o = new jj::gui::frame_t(*this, jj::gui::frame_t::options() << jj::gui::opt::text(jjT("Child")) << jj::gui::frame_t::NO_MINIMIZE);
        f->o->OnClose.add(*this, &app::ono);
        f->o->show();

		return true;
	}
};

app g_app;
