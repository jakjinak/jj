#include "jj/defines.h"

#if (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
#include "wx/wx.h"
#include "wx/cmdline.h"

#include "jj/gui/application.h"
#include "jj/stream.h"

#include <exception>

namespace jj
{
namespace gui
{
namespace // <anonymous>
{
struct registrar_info
{
    application_t* inst;
    bool in_run;
    registrar_info() : inst(nullptr), in_run(false) {}
};

static registrar_info& registrar()
{
    static registrar_info inst;
    return inst;
}
} // namespace <anonymous>

application_t::application_t()
    : argc(0), argv(nullptr)
{
    registrar_info& r = registrar();
    if (r.inst)
    {
        if (r.in_run)
            return; // ignore apps defined after OnInit

        // but if multiple (global) application objects a created before OnInit then it's definitely a bug
        // TODO replace with kind of jjINITLOG
        jj::cerr << jjT("Multiple application instance!\n");
        exit(1);
    }
    r.inst = this;
}

application_t::~application_t()
{
    registrar().inst = nullptr;
}

namespace // <anonymous>
{
class theApp : public wxApp
{
    virtual void OnInitCmdLine(wxCmdLineParser& parser)
    {
        parser.SetCmdLine("");
    }
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser)
    {
        return true;
    }
    virtual bool OnInit()
    {
        registrar_info& r = registrar();
        if (!r.inst)
        {
            // TODO replace with kind of jjINITLOG
            jj::cerr << jjT("No application instance!\n");
            return false; // no global instance of jj::gui::application_t found
        }
        r.in_run = true;

        if (!wxApp::OnInit())
            return false;

        r.inst->argc = argc;
        r.inst->argv = argv;
        return r.inst->onInit();
    }
    virtual bool OnExceptionInMainLoop()
    {
        bool end = false;
        try
        {
            throw;
        }
        // TODO: add jj::exception
        catch (const std::exception& ex)
        {
            end = showError(ex.what());
        }
        catch (const wxString& ex)
        {
            end = showError(ex);
        }
        catch (const std::string& ex)
        {
            end = showError(ex);
        }
        catch (const std::wstring& ex)
        {
            end = showError(ex);
        }
        catch (const char* ex)
        {
            end = showError(ex);
        }
        catch (...)
        {
            end = showError("Unknown exception.");
        }
        return !end;
    }
private:
    bool showError(wxString err)
    {
        int result = ::wxMessageBox("The following error occurred:\n" + err + "\nThe program will terminate now. Ok?", "Unhandled exception", wxOK | wxCANCEL);
        return result != wxOK;
    }
};

} // namespace <anonymous>

} // namespace gui
} // namespace jj

IMPLEMENT_APP(jj::gui::theApp)

#endif // (JJ_USE_GUI == JJ_DEFINED_VALUE_GUI_WX)
