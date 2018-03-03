#include "jj/string.h"
#include <list>
#include <vector>
#include "jj/test/test.h"
#include "jj/cmdLine.h"

extern jj::cmdLine::nameCompare_less_t g_CASE_SENSITIVE;
extern jj::cmdLine::nameCompare_less_t g_CASE_INSENSITIVE;

/*! Helper for commandline arguments testing containing the args and able to convert from initializer list.
\warning DONT EVER MODIFY args/argv/argc manually! */
struct arg_info_t
{
    static const int MAX_SIZE = 32; //!< maximum args to store
    static const jj::char_t* DefaultProgramName; // jjT("ProgramName")

    std::list<jj::string_t> args; //!< arguments in C++ string form (dummy program name auto-added)
    int argc; //!< the count of args (and argv)
    const jj::char_t* argv[MAX_SIZE]; //!< the regular argv (pointers to args)

    arg_info_t() : argc(0) {}
    arg_info_t(const std::initializer_list<jj::string_t>& x) { setup(x, *this); }
    arg_info_t(const jj::string_t& pn, const std::initializer_list<jj::string_t>& x) { setup(pn, x, *this); }

    static void setup(const jj::string_t& pn, const std::initializer_list<jj::string_t>& x, arg_info_t& info)
    {
        info.args.clear();
        info.args.push_back(pn);
        info.argv[0] = info.args.back().c_str();
        size_t i=1;
        for (auto& s : x)
        {
            info.args.push_back(s);
            info.argv[i] = info.args.back().c_str();
            ++i;
            if (i>=MAX_SIZE)
                break;
        }
        info.argc = int(i);
    }
    static void setup(const std::initializer_list<jj::string_t>& x, arg_info_t& info)
    {
        setup(DefaultProgramName, x, info);
    }

    void print();
};

struct cmdLineOptionsCommon_t
{
    cmdLineOptionsCommon_t(jj::test::testclass_base_t& base) : base_(base) {}
#undef JJ_TEST_CLASS_ACCESSOR
#define JJ_TEST_CLASS_ACCESSOR base_.

    struct optinfo_t
    {
        bool Known;
        jj::cmdLine::arguments_t::optionType_t Type;
        size_t Count;
        optinfo_t() : Known(false), Type(jj::cmdLine::arguments_t::TREG), Count(0) {}
    };
    struct optinfos_t
    {
        typedef std::map<jj::cmdLine::name_t, optinfo_t, jj::cmdLine::nameCompare_less_t> infomap_t;
        typedef std::vector<jj::cmdLine::name_t> namelists_t;

        infomap_t infos;
        namelists_t names;
        arg_info_t argv;

        optinfos_t(const std::initializer_list<jj::string_t>& args) : infos(g_CASE_SENSITIVE), argv(args) {}
    };

    void setup_single_option(jj::cmdLine::definitions_t& defs, optinfos_t& infos, const std::list<jj::cmdLine::name_t>& names, size_t count, jj::cmdLine::multiple_t multi)
    {
        defs.Options.push_back({ names, jjT(""), count, multi,
                [this, &infos, names, count](const jj::cmdLine::optionDefinition_t&, jj::cmdLine::values_t& v) {
                ++infos.infos[names.front()].Count;
                JJ_TEST(v.Values.size() == count);
                return true;
            } });
        infos.names.push_back(names.front());
        infos.infos[names.front()].Known = true;
        infos.infos[names.front()].Type = jj::cmdLine::arguments_t::TREG;
    }

    void setup_single_option(jj::cmdLine::definitions_t& defs, optinfos_t& infos, const std::list<jj::cmdLine::name_t>& names, jj::string_t end, jj::cmdLine::multiple_t multi)
    {
        defs.ListOptions.push_back({ names, end, jjT(""), multi,
                [this, &infos, names](const jj::cmdLine::listDefinition_t&, jj::cmdLine::values_t& v) {
                ++infos.infos[names.front()].Count;
                return true;
            } });
        infos.names.push_back(names.front());
        infos.infos[names.front()].Known = true;
        infos.infos[names.front()].Type = jj::cmdLine::arguments_t::TLIST;
    }

    void setup_positionals(jj::cmdLine::definitions_t& defs, size_t mandatory, size_t optional)
    {
        static jj::string_t shorts[] = { jjT("first"), jjT("second"), jjT("third"), jjT("fourth"), jjT("fifth"), jjT("sixth"), jjT("seventh"), jjT("eight"), jjT("nineth"), jjT("tenth") };
        JJ_ENSURE(mandatory + optional <= 10);
        size_t cnt = 0;
        for (size_t i = 0; i < mandatory; ++i, ++cnt)
            defs.Positionals.push_back({ shorts[cnt], jjT(""), true, [this](const jj::cmdLine::positionalDefinition_t, jj::string_t& value) { return true; } });
        for (size_t i = 0; i < optional; ++i, ++cnt)
            defs.Positionals.push_back({ shorts[cnt], jjT(""), false, [this](const jj::cmdLine::positionalDefinition_t, jj::string_t& value) { return true; } });
    }

    void setup_single_variable(jj::cmdLine::definitions_t& defs, const jj::string_t& name, const jj::string_t& deflt)
    {
        defs.Variables.push_back({ name, jjT(""), deflt, nullptr });
    }

    struct parserSetupWrap_t
    {
        enum t { FLAGS, SOVS, UVS } t_;
        union {
            jj::cmdLine::flags_t f;
            jj::cmdLine::stackOptionValues_t sov;
            jj::cmdLine::unknownVariableBehavior_t uv;
        } v_;
        parserSetupWrap_t(jj::cmdLine::flags_t v) : t_(FLAGS) { v_.f = v; }
        parserSetupWrap_t(jj::cmdLine::stackOptionValues_t v) : t_(SOVS) { v_.sov = v; }
        parserSetupWrap_t(jj::cmdLine::unknownVariableBehavior_t v) : t_(UVS) { v_.uv = v; }
    };

    void setup_parser(jj::cmdLine::arguments_t& args, const std::initializer_list<parserSetupWrap_t>& setup)
    {
        jj::opt::f<jj::cmdLine::flags_t, jj::cmdLine::flags_t::MAX_FLAGS>& fs = args.ParserOptions;
        for (auto s : setup)
        {
            switch (s.t_)
            {
            case parserSetupWrap_t::FLAGS:
                fs.toggle(s.v_.f);
                break;
            case parserSetupWrap_t::SOVS:
                args.ParserOptions << s.v_.sov;
                break;
            case parserSetupWrap_t::UVS:
                args.ParserOptions << s.v_.uv;
                break;
            }
        }
    }

    const jj::cmdLine::arguments_t::option_t& checkOption(const jj::cmdLine::arguments_t& args, const jj::cmdLine::definitions_t& defs, const jj::cmdLine::name_t& name, jj::cmdLine::arguments_t::optionType_t type)
    {
        jj::cmdLine::arguments_t::options_t::const_iterator fnd = args.Options.find(name);
        JJ_ENSURE(fnd != args.Options.end());
        const jj::cmdLine::arguments_t::option_t& opt = fnd->second;
        JJ_ENSURE(opt.first.Type == type);
        if (type == jj::cmdLine::arguments_t::TREG)
        {
            JJ_ENSURE(opt.first.u.Opt != nullptr);
            jj::cmdLine::nameCompare_less_t cmp(args.OptionCase);
            jj::cmdLine::definitions_t::opts_t::const_iterator fndd = defs.Options.begin();
            for (; fndd != defs.Options.end(); ++fndd)
            {
                for (auto& n : fndd->Names)
                {
                    if (!cmp(n, name) && !cmp(name, n))
                    {
                        JJ_TEST(opt.first.u.Opt == &*fndd);
                        if (opt.first.u.Opt->Multi == jj::cmdLine::multiple_t::JOIN)
                        {
                            if (fndd->ValueCount > 0)
                            {
                                JJ_TEST(opt.second.Values.size() % fndd->ValueCount == 0);
                            }
                        }
                        else
                        {
                            JJ_TEST(opt.second.Values.size() == fndd->ValueCount);
                        }
                        return opt;
                    }
                }
            }
        }
        else if (type == jj::cmdLine::arguments_t::TLIST)
        {
            JJ_ENSURE(opt.first.u.List != nullptr);
            jj::cmdLine::nameCompare_less_t cmp(args.OptionCase);
            jj::cmdLine::definitions_t::lists_t::const_iterator fndd = defs.ListOptions.begin();
            for (; fndd != defs.ListOptions.end(); ++fndd)
            {
                for (auto& n : fndd->Names)
                {
                    if (!cmp(n, name) && !cmp(name, n))
                    {
                        JJ_TEST(opt.first.u.List == &*fndd);
                        return opt;
                    }
                }
            }
        }
        JJ_ENSURE(false, jjT("Option definition found."));
    }

    void checkValues(const jj::cmdLine::arguments_t::option_t& opt, const std::list<jj::string_t>& vals)
    {
        JJ_ENSURE(opt.second.Values.size() == vals.size(), jjT("Expected ") << vals.size() << jjT(" values, received ") << opt.second.Values.size());
        std::list<jj::string_t>::const_iterator it = vals.begin();
        for (const jj::string_t& s : opt.second.Values)
        {
            JJ_TEST(s == *it, jjT("Expected \"") << *it << jjT("\", received \"") << s << jjT("\""));
            ++it;
        }
    }

    /* verifies whether parsing is successful or throws */
    bool perform_test(optinfos_t& infos, jj::cmdLine::definitions_t& defs, jj::cmdLine::arguments_t& args, bool ok)
    {
        if (ok)
        {
            args.parse(defs, infos.argv.argc, infos.argv.argv);
            return true;
        }
        else
        {
            JJ_TEST_THAT_THROWS(args.parse(defs, infos.argv.argc, infos.argv.argv), std::runtime_error);
            return false;
        }
    }

    /* verifies that options have given values and counts and so ... */
    void perform_test(optinfos_t& infos, jj::cmdLine::definitions_t& defs, jj::cmdLine::arguments_t& args, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals)
    {
        size_t cnt = infos.names.size();
        JJ_ENSURE(cnt == count.size());
        JJ_ENSURE(cnt == pvals.size());
        for (size_t i = 0; i < cnt; ++i)
        {
            if (count[i] == 0)
                continue; // don't care about
            optinfos_t::infomap_t::const_iterator fnd = infos.infos.find(infos.names[i]);
            JJ_ENSURE(fnd != infos.infos.end(), jjT("is a known argument"));
            const jj::cmdLine::arguments_t::option_t& o = checkOption(args, defs, infos.names[i], fnd->second.Type);
            checkValues(o, pvals[i]);
        }
        for (auto& info : infos.infos)
        {
            JJ_TEST(info.second.Known);
        }
    }

    /* verifies that positional arguments have the given values (and associated the correct definition) */
    void perform_test(jj::cmdLine::definitions_t& defs, jj::cmdLine::arguments_t& args, const std::vector<jj::string_t>& pvals)
    {
        JJ_ENSURE(args.Positionals.size() == pvals.size(), jjT("Expected ") << pvals.size() << jjT(", ") << args.Positionals.size() << jjT(" were parsed"));
        size_t i = 0;
        jj::cmdLine::arguments_t::positionals_t::const_iterator pit = args.Positionals.begin();
        for (; i < pvals.size(); ++i, ++pit)
        {
            JJ_TEST(pit->second == pvals[i]);
            if (i < defs.Positionals.size())
            {
                jj::cmdLine::definitions_t::poss_t::const_iterator def = defs.Positionals.begin();
                std::advance(def, i);
                JJ_TEST(pit->first == &*def);
            }
            else
            {
                JJ_TEST(pit->first == nullptr);
            }
        }
    }

    /* verifies that variables have values as given vars (and correctly associated definitions) */
    void perform_test(jj::cmdLine::definitions_t& defs, jj::cmdLine::arguments_t& args, const std::map<jj::string_t, jj::string_t>& vars)
    {
        for (auto& v : vars)
        {
            jj::cmdLine::arguments_t::varmap_t::const_iterator fnd = args.Variables.find(v.first);
            JJ_ENSURE_MSG(fnd != args.Variables.end(), jjT("Variable ") << v.first << jjT(" is defined"));
            JJ_TEST(v.second == fnd->second.Value);

            // check correct definition associated
            bool(*cmpfn)(const jj::string_t&, const jj::string_t&) = (args.VariableCase == jj::cmdLine::case_t::SENSITIVE
                ? static_cast<bool(*)(const jj::string_t&, const jj::string_t&)>(jj::str::less)
                : static_cast<bool(*)(const jj::string_t&, const jj::string_t&)>(jj::str::lessi));
            jj::cmdLine::variableDefinition_t* def = nullptr;
            for (auto& d : defs.Variables)
            {
                // means equal; TODO replace less/lessi above with equal/equali when available
                if (!cmpfn(d.Name, v.first) && !cmpfn(v.first, d.Name))
                {
                    def = &d;
                    break;
                }
            }
            JJ_TEST_MSG(fnd->second.Var == def, jjT("Variable has ") << (def == nullptr ? jjT("correctly no") : jjT("correct non-NULL")) << jjT(" definition associated."));
        }
    }

    void perform_test(optinfos_t& infos, jj::cmdLine::definitions_t& defs, jj::cmdLine::arguments_t& args, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals)
    {
        if (!perform_test(infos, defs, args, ok))
            return;
        perform_test(infos, defs, args, count, pvals);
    }

    void perform_test(optinfos_t& infos, jj::cmdLine::definitions_t& defs, jj::cmdLine::arguments_t& args, bool ok, const std::vector<jj::string_t>& pvals)
    {
        if (!perform_test(infos, defs, args, ok))
            return;
        perform_test(defs, args, pvals);
    }

    void perform_test(optinfos_t& infos, jj::cmdLine::definitions_t& defs, jj::cmdLine::arguments_t& args, bool ok, const std::map<jj::string_t, jj::string_t>& vars)
    {
        if (!perform_test(infos, defs, args, ok))
            return;
        perform_test(defs, args, vars);
    }

    void perform_test(optinfos_t& infos, jj::cmdLine::definitions_t& defs, jj::cmdLine::arguments_t& args, bool ok, const std::map<jj::string_t, jj::string_t>& vars, const std::vector<jj::string_t>& pvals)
    {
        if (!perform_test(infos, defs, args, ok))
            return;
        perform_test(defs, args, vars);
        perform_test(defs, args, pvals);
    }

    void perform_test(optinfos_t& infos, jj::cmdLine::definitions_t& defs, jj::cmdLine::arguments_t& args, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& optvals, const std::map<jj::string_t, jj::string_t>& vars, const std::vector<jj::string_t>& posvals)
    {
        if (!perform_test(infos, defs, args, ok))
            return;
        perform_test(infos, defs, args, count, optvals);
        perform_test(defs, args, vars);
        perform_test(defs, args, posvals);
    }
#undef JJ_TEST_CLASS_ACCESSOR
#define JJ_TEST_CLASS_ACCESSOR
private:
    jj::test::testclass_base_t& base_;
};
