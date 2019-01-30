#include "jj/gui/application.h"
#include "jj/gui/window.h"
#include "jj/gui/textLabel.h"
#include "jj/gui/textInput.h"
#include "jj/gui/comboBox.h"
#include "jj/gui/tree.h"
#include "jj/gui/button.h"
#include "jj/gui/sizer.h"
#include "jj/stream.h"
#include <sstream>
#include "jj/directories.h"

class myTreeThing : public jj::gui::treeItem_T<myTreeThing>
{
    typedef jj::gui::treeItem_T<myTreeThing> base_t;
    typedef jj::gui::tree_T<myTreeThing> CTRL;
public:
    myTreeThing(CTRL& parent, id_t id) : base_t(parent, id) {}
};

class treetest : public jj::gui::dialog_t
{
    jj::gui::tree_T<myTreeThing>* tree;
    jj::gui::textInput_t* name, *log;
    jj::gui::button_t* add, *remove, *dump;
    jj::gui::boxSizer_t* right, *sadd, *srem;

    void onAdd(jj::gui::button_t&) {
        myTreeThing* i = tree->selected();
        if (i == nullptr)
            dolog(jjT("Nothing selected, nothing added."));
        i->add(name->text());
        dolog(jjT("Added [") + name->text() + jjT("]"));
    }
    void onRemove(jj::gui::button_t&) {
        myTreeThing* i = tree->selected();
        if (i == nullptr)
        {
            dolog(jjT("Nothing selected, nothing removed."));
            return;
        }
        if (i->id() == tree->root().id())
        {
            dolog(jjT("Root selected, nothing removed."));
            return;
        }
        myTreeThing* p = i->parent_item();
        if (p == nullptr)
        {
            dolog(jjT("Something weird happened!"));
            return;
        }
        p->remove(*i);
    }
    void doDump(myTreeThing& t, jj::string_t& buf) {
        buf += t.text();
        buf += jjT("{");
        for (size_t i = 0; i < t.count(); ++i)
            doDump(t.get(i), buf);
        buf += jjT("}");
    }
    void onDump(jj::gui::button_t&) {
        jj::string_t buf;
        doDump(tree->root(), buf);
        dolog(buf);
    }

    bool beforeSelect(myTreeThing& x, myTreeThing* o) {
        dolog(jjT("Querying selection of [") + x.text() + jjT("], previous was [") + (o == nullptr ? jj::string_t(jjT("<NONE>")) : o->text()) + jjT("]"));
        if (x.text() == jjT("NEE"))
        {
            dolog(jjT("VETO!"));
            return false;
        }
        return true;
    }
    void onSelect(myTreeThing& x, myTreeThing* o) {
        dolog(jjT("Selected [") + x.text() + jjT("], previous was [") + (o == nullptr ? jj::string_t(jjT("<NONE>")) : o->text()) + jjT("]"));
    }
    void onActivate(myTreeThing& x) {
        x.bold(!x.bold());
        x.bold() ? x.expand() : x.collapse();
    }

public:
    treetest(jj::gui::topLevelWindow_t& main)
        : jj::gui::dialog_t(main, jj::gui::dialog_t::options() << jj::opt::text(jjT("GUI Tree test")) << jj::opt::size(400, 800) << jj::gui::dialog_t::RESIZEABLE)
    {
        using namespace jj;
        using namespace jj::gui;

        onCreateSizer = [this] { return new boxSizer_t(*this, boxSizer_t::HORIZONTAL); };

        tree = new tree_T<myTreeThing>(*this, jj::gui::tree_T<myTreeThing>::options() << opt::text(jjT("ROOT")));
        tree->BeforeSelect.add(*this, &treetest::beforeSelect);
        tree->OnSelect.add(*this, &treetest::onSelect);
        tree->OnActivate.add(*this, &treetest::onActivate);
        name = new textInput_t(*this, textInput_t::options());
        log = new textInput_t(*this, textInput_t::options() << textInput_t::MULTILINE);
        add = new button_t(*this, button_t::options() << opt::text(jjT("Add")));
        add->OnClick.add(*this, &treetest::onAdd);
        remove = new button_t(*this, button_t::options() << opt::text(jjT("Remove")));
        remove->OnClick.add(*this, &treetest::onRemove);
        dump = new button_t(*this, button_t::options() << opt::text(jjT("Dump")));
        dump->OnClick.add(*this, &treetest::onDump);

        sizer().add(*tree, sizerFlags_t().proportion(2).expand());
        sizer().add(*(right = new jj::gui::boxSizer_t(*this, boxSizer_t::VERTICAL)), sizerFlags_t().proportion(1).expand());
        right->add(*(sadd = new jj::gui::boxSizer_t(*this, boxSizer_t::HORIZONTAL)), sizerFlags_t().expand());
        right->add(*(srem = new jj::gui::boxSizer_t(*this, boxSizer_t::HORIZONTAL)), sizerFlags_t().expand());
        sadd->add(*name, sizerFlags_t().expand());
        sadd->add(*add, sizerFlags_t());
        srem->add(*remove, sizerFlags_t());
        srem->add(*dump, sizerFlags_t());
        right->add(*log, sizerFlags_t().expand().proportion(1));
    }

    void dolog(const jj::string_t& txt)
    {
        auto ct = log->text();
        if (ct.empty())
            log->changeText(txt);
        else
            log->changeText(txt + jjT('\n') + ct);
    }
};

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
        : jj::gui::frame_t(app, jj::gui::frame_t::options() << jj::opt::text(jjT("First One")) << jj::opt::size(600, 280) << jj::gui::frame_t::NO_MAXIMIZE)
        , o(nullptr), dlg(nullptr)
    {
        using namespace jj;
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

        menu_t* m1, *m2, *m3, *m4, *mt;
        menu_bar().append(m1 = new menu_t(*this), jjT("TEST"));
        m1->append(menuItem_t::options() << opt::text(jjT("Current Dir"))).lock()->OnClick.add([this](menuItem_t&) {
            t2->changeText(jj::directories::current());
        });
        m1->append(menuItem_t::options() << opt::text(jjT("Program Dir"))).lock()->OnClick.add([this](menuItem_t&) {
            t2->changeText(jj::directories::program());
        });
        m1->append(menuItem_t::options() << opt::text(jjT("Program Path"))).lock()->OnClick.add([this](menuItem_t&) {
            t2->changeText(jj::directories::program(true));
        });
        m1->append(menuItem_t::options() << opt::text(jjT("Home Dir"))).lock()->OnClick.add([this](menuItem_t&) {
            t2->changeText(jj::directories::personal());
        });
        m1->append(menuItem_t::options() << opt::text(jjT("Home Dir2"))).lock()->OnClick.add([this](menuItem_t&) {
            t2->changeText(jj::directories::personal(true));
        });
        m1->append(menuItem_t::SEPARATOR);
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
        m4->append(menuItem_t::SEPARATOR);
        m4->append(menuItem_t::options() << opt::text(jjT("open")))
            .lock()->OnClick.add([this](menuItem_t&) {
                dlg::openFile_t x(*this, dlg::openFile_t::options());
                if (x.show_modal() == stock::item_t::OK)
                    t2->text(jjS(jjT("[") << x.file() << jjT("] selected in dialog")));
            });
        m4->append(menuItem_t::options() << opt::text(jjT("open many")))
            .lock()->OnClick.add([this](menuItem_t&) {
            dlg::openFile_t x(*this, dlg::openFile_t::options() << dlg::openFile_t::MULTIPLE << dlg::openFile_t::MUST_EXIST);
            if (x.show_modal() == stock::item_t::OK)
            {
                jj::string_t t;
                for (const jj::string_t& s : x.files())
                    t = t + jjT('[') + s + jjT("] ");
                if (t.empty())
                    t = jjT("Nothing ");
                t += jjT("selected in dialog");
                t2->text(t);
            }
        });
        m4->append(menuItem_t::options() << opt::text(jjT("save")))
            .lock()->OnClick.add([this](menuItem_t&) {
            dlg::saveFile_t x(*this, dlg::saveFile_t::options() << dlg::saveFile_t::OVERWRITE);
            if (x.show_modal() == stock::item_t::OK)
                t2->text(jjS(jjT("[") << x.file() << jjT("] selected in dialog")));
        });
        m4->append(menuItem_t::options() << opt::text(jjT("get dir")))
            .lock()->OnClick.add([this](menuItem_t&) {
            dlg::selectDir_t x(*this, dlg::selectDir_t::options());
            if (x.show_modal() == stock::item_t::OK)
                t2->text(jjS(jjT("[") << x.dir() << jjT("] selected in dialog")));
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

        menu_bar().append(mt = new menu_t(*this), jjT("TESTS"));
        mt->append(menuItem_t::options() << opt::text(jjT("Tree test"))).lock()->OnClick.add([this](menuItem_t&) {
            treetest x(*this);
            x.show_modal();
        });
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

	virtual bool on_init()
	{
        //_CrtSetBreakAlloc(5684);
        f = new wnd(*this);
        f->show();

        f->o = new jj::gui::frame_t(*this, jj::gui::frame_t::options() << jj::opt::text(jjT("Child")) << jj::gui::frame_t::NO_MINIMIZE);
        f->o->OnClose.add(*this, &app::ono);
        f->o->show();

		return true;
	}
};

app g_app;
