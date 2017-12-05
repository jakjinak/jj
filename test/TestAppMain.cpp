#include "jj/gui/application.h"
#include "jj/gui/window.h"
#include "jj/gui/textInput.h"
#include "jj/gui/button.h"
#include "jj/gui/sizer.h"

class wnd : public jj::gui::frame_t
{
    jj::gui::textInput_t* t1;
    jj::gui::button_t* b1, *b2, *b3;

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
    void ontxt(jj::gui::textInput_t& x) {
        status_bar().text(x.value());
        if (!o)
            return;
        o->title(x.value());
    }

public:
    wnd(jj::gui::application_t& app)
        : jj::gui::frame_t(app, jj::gui::frame_t::options_t() << jj::gui::opt::title(jjT("First One")) << jj::gui::opt::size(800, 120))
        , o(nullptr)
    {
        using namespace jj::gui;

        onCreateSizer = [this] { return new boxSizer_t(*this, boxSizer_t::HORIZONTAL); };

        b1 = new button_t(*this, buttonOptions_t() << opt::title(jjT("B1")));
        b1->OnClick.add(*this, &wnd::onb1);
        b2 = new button_t(*this, buttonOptions_t() << opt::title(jjT("B2")));
        b2->OnClick.add(*this, &wnd::onb2);
        b3 = new button_t(*this, buttonOptions_t() << opt::title(jjT("B3")));
        b3->OnClick.add(*this, &wnd::onb3);
        t1 = new textInput_t(*this, textInputOptions_t() << opt::text(jjT("Child")));
        t1->OnTextChange.add(*this, &wnd::ontxt);
        sizer().add(*b1, sizerFlags_t().set(sizerFlags_t::CENTER));
        sizer().add(*b2, sizerFlags_t().set(sizerFlags_t::CENTER));
        sizer().add(*b3, sizerFlags_t().set(sizerFlags_t::CENTER));
        sizer().add(*t1, sizerFlags_t().set(sizerFlags_t::CENTER).proportion(1));

        menu_t* m1, *m2, *m3;
        menu_bar().append(m1 = new menu_t(), menuItem_t::subOptions_t() << opt::text(jjT("TEST")));
        auto me = m1->append(menuItem_t::options_t() << opt::text(jjT("Exit")) << opt::accelerator(keys::accelerator_t(keys::ALT, keys::F4)));
        me.lock()->OnClick.add([this](menuItem_t&) { this->close(); });
        menu_bar().append(m2 = new menu_t(), menuItem_t::subOptions_t() << opt::text(jjT("actions")));
        m2->append(menuItem_t::options_t() << opt::text(jjT("M1")));
        m2->append(m3 = new menu_t(), menuItem_t::subOptions_t() << opt::text(jjT("sub")));
        m2->append(menuItem_t::options_t() << opt::text(jjT("M2")) << menuItem_t::CHECK);
        m2->append(menuItem_t::options_t() << opt::text(jjT("M3")) << menuItem_t::CHECK);
        m2->append(menuItem_t::SEPARATOR);
        m2->append(menuItem_t::options_t() << opt::text(jjT("M4")));
        m3->append(menuItem_t::options_t() << opt::text(jjT("S1")) << menuItem_t::RADIO);
        m3->append(menuItem_t::options_t() << opt::text(jjT("S2")) << menuItem_t::RADIO);
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

        f->o = new jj::gui::frame_t(*this, jj::gui::frame_t::options_t() << jj::gui::opt::title(jjT("Child")));
        f->o->OnClose.add(*this, &app::ono);
        f->o->show();

		return true;
	}
};

app g_app;
