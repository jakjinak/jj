#include "jj/test/test.h"
#include "cmdLine_tests.h"
#include "jj/cmdLine.h"
#include <vector>

using namespace jj::cmdLine;

nameCompare_less_t g_CASE_SENSITIVE = nameCompare_less_t(case_t::SENSITIVE), g_CASE_INSENSITIVE = nameCompare_less_t(case_t::INSENSITIVE);

JJ_TEST_CLASS(cmdLineOptionDefinitionsTests_t)

JJ_TEST_CASE(EmptyOptionName_Throws)
{
    definitions_t defs;
    defs.Options.push_back({{ name_t(jjT("")) }, jjT(""), 0u, multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
}

JJ_TEST_CASE(EmptyOptionPrefix_Throws)
{
    definitions_t defs;
    defs.Options.push_back({{ name_t(jjT(""), jjT("x")) }, jjT(""), 0u, multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
}

JJ_TEST_CASE_VARIANTS(OptionsNames_DuplicateThrow,(const optionDefinition_t::names_t& names, bool ok),\
    ({name_t(jjT("a")), name_t(jjT("--"), jjT("a"))},true),\
    ({name_t(jjT("a")), name_t(jjT("a"))},false),\
    ({name_t(jjT("A")), name_t(jjT('a'))},true),\
    ({name_t(jjT("ABC")), name_t(jjT("Abc")), name_t(jjT("abc"))},true),\
    ({name_t(jjT('-'),jjT('a')), name_t(jjT('+'),jjT('a'))},true))
{
    definitions_t defs;
    defs.Options.push_back({names, jjT(""), 0u, multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    if (ok)
    {
        args.parse(defs);
        JJ_TEST(true, jjT("parsing does not throw"));
    }
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
    }
}

JJ_TEST_CASE_VARIANTS(OptionsNamesICase_DuplicateThrow,(const optionDefinition_t::names_t& names, bool ok),\
    ({name_t(jjT("a")), name_t(jjT("--"), jjT("a"))},true),\
    ({name_t(jjT("a")), name_t(jjT("a"))},false),\
    ({name_t(jjT("A")), name_t(jjT('a'))},false),\
    ({name_t(jjT("ABC")), name_t(jjT("Abc")), name_t(jjT("abc"))},false),\
    ({name_t(jjT('-'),jjT('a')), name_t(jjT('+'),jjT('a'))},true))
{
    definitions_t defs;
    defs.Options.push_back({names, jjT(""), 0u, multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    args.OptionCase = case_t::INSENSITIVE;
    if (ok)
    {
        args.parse(defs);
        JJ_TEST(true, jjT("parsing does not throw"));
    }
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
    }
}

JJ_TEST_CASE_VARIANTS(LongOptionNameWithShortOptionPrefix_Throws,(const name_t& n, const std::list<std::pair<jj::string_t,type_t>>& pi),\
    (name_t(jjT('-'), jjT("name")),{}),\
    (name_t(jjT("--"),jjT("name")),{{jjT("--"),type_t::SHORT_OPTION}}),\
    (name_t(jjT('+'), jjT("name")),{}),\
    (name_t(jjT("++"),jjT("name")),{{jjT("++"),type_t::SHORT_OPTION}}))
{
    definitions_t defs;
    defs.Options.push_back({{n}, jjT(""), 0u, multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    for (auto& i : pi)
        args.PrefixInfo[i.first].Type = i.second;
    JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
}

JJ_TEST_CASE(EmptyListName_Throws)
{
    definitions_t defs;
    defs.ListOptions.push_back({{ name_t(jjT("")) }, jjT(""), jjT(""), multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
}

JJ_TEST_CASE(EmptyListPrefix_Throws)
{
    definitions_t defs;
    defs.ListOptions.push_back({{ name_t(jjT(""), jjT("x")) }, jjT(""), jjT(""), multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
}

JJ_TEST_CASE_VARIANTS(ListNames_DuplicateThrow,(const listDefinition_t::names_t& names, bool ok),\
    ({name_t(jjT("a")), name_t(jjT("--"), jjT("a"))},true),\
    ({name_t(jjT("a")), name_t(jjT("a"))},false),\
    ({name_t(jjT("A")), name_t(jjT('a'))},true),\
    ({name_t(jjT("ABC")), name_t(jjT("Abc")), name_t(jjT("abc"))},true),\
    ({name_t(jjT('-'),jjT('a')), name_t(jjT('+'),jjT('a'))},true))
{
    definitions_t defs;
    defs.ListOptions.push_back({names, jjT(""), jjT(""), multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    if (ok)
    {
        args.parse(defs);
        JJ_TEST(true, jjT("parsing does not throw"));
    }
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
    }
}

JJ_TEST_CASE_VARIANTS(ListNamesICase_DuplicateThrow,(const listDefinition_t::names_t& names, bool ok),\
    ({name_t(jjT("a")), name_t(jjT("--"), jjT("a"))},true),\
    ({name_t(jjT("a")), name_t(jjT("a"))},false),\
    ({name_t(jjT("A")), name_t(jjT('a'))},false),\
    ({name_t(jjT("ABC")), name_t(jjT("Abc")), name_t(jjT("abc"))},false),\
    ({name_t(jjT('-'),jjT('a')), name_t(jjT('+'),jjT('a'))},true))
{
    definitions_t defs;
    defs.ListOptions.push_back({names, jjT(""), jjT(""), multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    args.OptionCase = case_t::INSENSITIVE;
    if (ok)
    {
        args.parse(defs);
        JJ_TEST(true, jjT("parsing does not throw"));
    }
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
    }
}

JJ_TEST_CASE_VARIANTS(LongListOptionNameWithShortOptionPrefix_Throws,(const name_t& n, const std::list<std::pair<jj::string_t,type_t>>& pi),\
    (name_t(jjT('-'), jjT("name")),{}),\
    (name_t(jjT("--"),jjT("name")),{{jjT("--"),type_t::SHORT_OPTION}}),\
    (name_t(jjT('+'), jjT("name")),{}),\
    (name_t(jjT("++"),jjT("name")),{{jjT("++"),type_t::SHORT_OPTION}}))
{
    definitions_t defs;
    defs.ListOptions.push_back({{n}, jjT(""), jjT(""), multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    for (auto& i : pi)
        args.PrefixInfo[i.first].Type = i.second;
    JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
}

JJ_TEST_CASE_VARIANTS(RegularAndListOptions_DuplicateThrow,(const optionDefinition_t::names_t& opts, const listDefinition_t::names_t& lsts, bool icase, bool ok),\
    ({name_t(jjT("a"))},{name_t(jjT("a"))},false,false),\
    ({name_t(jjT("a"))},{name_t(jjT("A"))},false,true),\
    ({name_t(jjT("a"))},{name_t(jjT("A"))},true,false),\
    ({name_t(jjT("abc")),name_t(jjT("Abc"))},{name_t(jjT("ABC"))},false,true),\
    ({name_t(jjT("abc"))},{name_t(jjT("ABC")),name_t(jjT("Abc"))},true,false),\
    ({name_t(jjT("abc"))},{name_t(jjT("ABC"))},true,false),\
    ({name_t(jjT("abc")),name_t(jjT("Abc"))},{name_t(jjT("ABC"))},false,true),\
    ({name_t(jjT("abc"))},{name_t(jjT("ABC")),name_t(jjT("Abc"))},true,false),\
    ({name_t(jjT("abc"))},{name_t(jjT("Abc"))},true,false),\
    ({name_t(jjT("a"))},{name_t(jjT("--"),jjT("a"))},false,true))
{
    definitions_t defs;
    defs.Options.push_back({{opts},jjT(""),0u,multiple_t::OVERRIDE, nullptr});
    defs.ListOptions.push_back({{lsts},jjT(""),jjT(""),multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    if (icase)
        args.OptionCase = case_t::INSENSITIVE;
    if (ok)
    {
        args.parse(defs);
        JJ_TEST(true, jjT("parsing does not throw"));
    }
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs), std::runtime_error);
    }
}

JJ_TEST_CLASS_END(cmdLineOptionDefinitionsTests_t,\
    EmptyOptionName_Throws, EmptyOptionPrefix_Throws, OptionsNames_DuplicateThrow, OptionsNamesICase_DuplicateThrow, LongOptionNameWithShortOptionPrefix_Throws,\
    EmptyListName_Throws, EmptyListPrefix_Throws, ListNames_DuplicateThrow, ListNamesICase_DuplicateThrow, LongListOptionNameWithShortOptionPrefix_Throws,\
    RegularAndListOptions_DuplicateThrow)

//================================================

struct cmdLineOptionsCommon_t
{
    cmdLineOptionsCommon_t(jj::test::testclass_base_t& base) : base_(base) {}
#undef JJ_TEST_CLASS_ACCESSOR
#define JJ_TEST_CLASS_ACCESSOR base_.

    struct optinfo_t
    {
        bool Known;
        arguments_t::optionType_t Type;
        size_t Count;
        optinfo_t() : Known(false), Type(arguments_t::TREG), Count(0) {}
    };
    struct optinfos_t
    {
        typedef std::map<name_t, optinfo_t, nameCompare_less_t> infomap_t;
        typedef std::vector<name_t> namelists_t;

        infomap_t infos;
        namelists_t names;
        arg_info_t argv;

        optinfos_t(const std::initializer_list<jj::string_t>& args) : infos(g_CASE_SENSITIVE), argv(args) {}
    };

    const arguments_t::option_t& checkOption(const arguments_t& args, const definitions_t& defs, const name_t& name, arguments_t::optionType_t type)
    {
        arguments_t::options_t::const_iterator fnd = args.Options.find(name);
        JJ_ENSURE(fnd!=args.Options.end());
        const arguments_t::option_t& opt = fnd->second;
        JJ_ENSURE(opt.first.Type==type);
        if (type==arguments_t::TREG)
        {
            JJ_ENSURE(opt.first.u.Opt!=nullptr);
            nameCompare_less_t cmp(args.OptionCase);
            definitions_t::opts_t::const_iterator fndd=defs.Options.begin();
            for (; fndd!=defs.Options.end();++fndd)
            {
                for (auto& n : fndd->Names)
                {
                    if (!cmp(n,name)&&!cmp(name,n))
                    {
                        JJ_TEST(opt.first.u.Opt==&*fndd);
                        JJ_TEST(opt.second.Values.size() == fndd->ValueCount);
                        return opt;
                    }
                }
            }
        }
        else if (type==arguments_t::TLIST)
        {
            JJ_ENSURE(opt.first.u.List!=nullptr);
            nameCompare_less_t cmp(args.OptionCase);
            definitions_t::lists_t::const_iterator fndd=defs.ListOptions.begin();
            for (; fndd!=defs.ListOptions.end();++fndd)
            {
                for (auto& n : fndd->Names)
                {
                    if (!cmp(n,name)&&!cmp(name,n))
                    {
                        JJ_TEST(opt.first.u.List==&*fndd);
                        return opt;
                    }
                }
            }
        }
        JJ_ENSURE(false, jjT("Option definition found."));
    }

    void checkValues(const arguments_t::option_t& opt, const std::list<jj::string_t>& vals)
    {
        JJ_ENSURE(opt.second.Values.size() == vals.size());
        std::list<jj::string_t>::const_iterator it = vals.begin();
        for (const jj::string_t& s : opt.second.Values)
        {
            JJ_TEST(s == *it);
            ++it;
        }
    }

    void perform_test(optinfos_t& infos, definitions_t defs, arguments_t args, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals)
    {
        if (ok)
        {
            args.parse(defs, infos.argv.argc, infos.argv.argv);
        }
        else
        {
            JJ_TEST_THAT_THROWS(args.parse(defs, infos.argv.argc, infos.argv.argv), std::runtime_error);
            return;
        }
        size_t cnt = infos.names.size();
        JJ_ENSURE(cnt == count.size());
        JJ_ENSURE(cnt == pvals.size());
        for (size_t i = 0; i < cnt; ++i)
        {
            if (count[i] == 0)
                continue; // don't care about
            optinfos_t::infomap_t::const_iterator fnd = infos.infos.find(infos.names[i]);
            JJ_ENSURE(fnd != infos.infos.end(), jjT("is a known argument"));
            const arguments_t::option_t& o = checkOption(args, defs, infos.names[i], fnd->second.Type);
            checkValues(o, pvals[i]);
        }
        for (auto& info : infos.infos)
        {
            JJ_TEST(info.second.Known);
        }
    }
#undef JJ_TEST_CLASS_ACCESSOR
#define JJ_TEST_CLASS_ACCESSOR
private:
    jj::test::testclass_base_t& base_;
};

JJ_TEST_CLASS_DERIVED(cmdLineOptionsTests_t, public cmdLineOptionsCommon_t)

void setup_single_option(definitions_t& defs, optinfos_t& infos, const std::list<name_t>& names, size_t count, multiple_t multi)
{
    defs.Options.push_back({ names, jjT(""), count, multi,
        [this, &infos, names, count](const optionDefinition_t&, values_t& v) {
        ++infos.infos[names.front()].Count;
        JJ_TEST(v.Values.size() == count);
        return true;
    } });
    infos.names.push_back(names.front());
    infos.infos[names.front()].Known = true;
    infos.infos[names.front()].Type = arguments_t::TREG;
}

cmdLineOptionsTests_t() : cmdLineOptionsCommon_t(static_cast<jj::test::testclass_base_t&>(*this)) {}

JJ_TEST_CASE_VARIANTS(parseShortOptions,(const std::initializer_list<jj::string_t>& argv, const std::initializer_list<int>& parsed),\
    ({}, {0,0,0,0}),({jjT("-a")}, {1,0,0,0}),({jjT("-A"),jjT("-c"),jjT("-1")}, {1,0,1,1}),({jjT("-a"),jjT("-A"),jjT("-a"),jjT("-a")},{4,0,0,0}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT('a')),name_t(jjT('A')) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('C')) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('c')) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('1')) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    perform_test(infos, defs, args, true, parsed, { {},{},{},{} });
}

JJ_TEST_CASE_VARIANTS(parseLongOptions,(const std::initializer_list<jj::string_t>& argv, const std::initializer_list<int>& parsed),\
    ({jjT("--first")}, {1,0,0}),({jjT("--FIRST"),jjT("-1")},{2,0,0}),({jjT("--3rd"),jjT("--first"),jjT("--second")},{1,1,1}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("first")),name_t(jjT("FIRST")),name_t(jjT('1')) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("second")) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("3rd")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    perform_test(infos, defs, args, true, parsed, { {},{},{} });
}

JJ_TEST_CASE_VARIANTS(redefinedPrefixes,(const std::initializer_list<jj::string_t>& argv, const std::initializer_list<int>& parsed),\
    ({},{0,0,0}),({jjT("-argument")},{1,0,0}),({jjT("+-X")},{0,0,1}),({jjT("--A"),jjT("+-X"),jjT("--b"),jjT("-argument")},{2,1,1}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("-"),jjT("argument")),name_t(jjT("--"),jjT('A')) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("--"),jjT("b")) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("+-"),jjT("X")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    args.PrefixInfo[jjT("-")].Type = LONG_OPTION;
    args.PrefixInfo[jjT("--")].Type = SHORT_OPTION;
    args.PrefixInfo[jjT("+-")].Type = LONG_OPTION;
    perform_test(infos, defs, args, true, parsed, { {},{},{} });
}

JJ_TEST_CASE_VARIANTS(stackedOptions,(const std::initializer_list<jj::string_t>& argv, const std::initializer_list<int>& parsed),\
    ({jjT("-xy")},{1,1,0,0,0}),({jjT("-xyz")},{1,1,1,0,0}),({jjT("-x"),jjT("--x")},{1,0,0,1,0}),\
    ({jjT("-yzx"),jjT("-zxy"),jjT("-z"),jjT("-zyx"),jjT("-x"),jjT("-zz")},{4,3,6,0,0}),\
    ({jjT("--x"),jjT("--xyz")},{0,0,0,1,1}),({jjT("-xyz"),jjT("--xyz")},{1,1,1,0,1}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT('x')) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('y')) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('z')) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("--"),jjT('x')) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("xyz")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    perform_test(infos, defs, args, true, parsed, { {},{},{},{},{} });
}

JJ_TEST_CASE_VARIANTS(valuedLongOptions,(const std::initializer_list<jj::string_t>& argv, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("--aa"),jjT("v1"),jjT("--cc"),jjT("--bb"),jjT("1"),jjT("2"),jjT("3")},true,{1,1,1},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("--aa"),jjT("--cc")},true,{1,0,0},{{jjT("--cc")},{},{}}),\
    ({jjT("--bb"),jjT("--aa"),jjT("v1"),jjT("--cc")},true,{0,1,0},{{},{jjT("--aa"),jjT("v1"),jjT("--cc")},{}}),\
    ({jjT("--aa")},false,{},{}),\
    ({jjT("--bb"),jjT("1"),jjT("2")},false,{},{}),\
    ({jjT("--bb"),jjT("1")},false,{},{}),\
    ({jjT("--bb")},false,{},{}),\
    ({jjT("--aa"),jjT("v1"),jjT("--cc"),jjT("--bb"),jjT("1"),jjT("2")},false,{},{}),\
    ({jjT("--aa=v1")},true,{1,0,0},{{jjT("v1")},{},{}}),\
    ({jjT("--bb=1"),jjT("2"),jjT("3")},true,{0,1,0},{{},{jjT("1"),jjT("2"),jjT("3")},{}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("aa")) }, 1u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("bb")) }, 3u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("cc")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CASE_VARIANTS(valuedShortOptions,(const std::initializer_list<jj::string_t>& argv, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-a"),jjT("v1"),jjT("-c"),jjT("-b"),jjT("1"),jjT("2"),jjT("3")},true,{1,1,1},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-a"),jjT("-c")},true,{1,0,0},{{jjT("-c")},{},{}}),\
    ({jjT("-b"),jjT("-a"),jjT("v1"),jjT("-c")},true,{0,1,0},{{},{jjT("-a"),jjT("v1"),jjT("-c")},{}}),\
    ({jjT("-a")},false,{},{}),\
    ({jjT("-b"),jjT("1"),jjT("2")},false,{},{}),\
    ({jjT("-b"),jjT("1")},false,{},{}),\
    ({jjT("-b")},false,{},{}),\
    ({jjT("-a"),jjT("v1"),jjT("-c"),jjT("-b"),jjT("1"),jjT("2")},false,{},{}),\
    ({jjT("-ca"),jjT("v1")},true,{1,0,1},{{jjT("v1")},{},{}}),\
    ({jjT("-cb"),jjT("1"),jjT("2"),jjT("3")},true,{0,1,1},{{},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-ccccca"),jjT("v1")},true,{1,0,5},{{jjT("v1")},{},{}}),\
    ({jjT("-ac"),jjT("v1")},false,{},{}),\
    ({jjT("-ac")},false,{},{}),\
    ({jjT("-bc")},false,{},{}),\
    ({jjT("-ab"),jjT("v1"),jjT("1"),jjT("2"),jjT("3")},false,{},{}),\
    ({jjT("-ba"),jjT("1"),jjT("2"),jjT("3"),jjT("v1")},false,{},{}),\
    ({jjT("-ca=v1")},true,{1,0,1},{{jjT("v1")},{},{}}),\
    ({jjT("-a=c")},true,{1,0,0},{{jjT("c")},{},{}}),\
    ({jjT("-cb=1"),jjT("2"),jjT("3")},true,{0,1,1},{{},{jjT("1"),jjT("2"),jjT("3")},{}}),
    ({jjT("-c=1")},false,{},{}),\
    ({jjT("-a=")},true,{1,0,0},{{jjT("")},{},{}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("a")) }, 1u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("b")) }, 3u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("c")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CASE_VARIANTS(looseStackValues,(const std::initializer_list<jj::string_t>& argv, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-abc"),jjT("v1"),jjT("1"),jjT("2"),jjT("3")},true,{1,1,1},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-cab"),jjT("v1"),jjT("1"),jjT("2"),jjT("3")},true,{1,1,1},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-acb"),jjT("v1"),jjT("1"),jjT("2"),jjT("3")},true,{1,1,1},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-acb"),jjT("v1"),jjT("1"),jjT("2")},false,{},{}),\
    ({jjT("-cb"),jjT("1"),jjT("2")},false,{},{}),\
    ({jjT("-a")},false,{},{}),\
    ({jjT("-ba"),jjT("1"),jjT("2"),jjT("3")},false,{},{}),\
    ({jjT("-bccca"),jjT("1"),jjT("2"),jjT("3"),jjT("v1")},true,{1,1,3},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-ca")},false,{},{}),\
    ({jjT("-aa"),jjT("v1"),jjT("v2")},true,{2,0,0},{{jjT("v2")},{},{}}),\
    ({jjT("-bb"),jjT("1"),jjT("2"),jjT("3"),jjT("A"),jjT("B"),jjT("C")},true,{0,2,0},{{},{jjT("A"),jjT("B"),jjT("C")},{}}),\
    ({jjT("-c=1")},false,{},{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("a")) }, 1u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("b")) }, 3u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("c")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    args.ParserOptions << stackOptionValues_t::LOOSE;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CASE_VARIANTS(disabledStacks, (const std::initializer_list<jj::string_t>& argv, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-a"),jjT("X"),jjT("-b"),jjT("1"),jjT("2"),jjT("3"),jjT("-c")},true,{1,1,1},{{jjT("X")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-c"),jjT("-a"),jjT("X")},true,{1,0,0},{{jjT("X")},{},{}}),\
    ({jjT("-ca"),jjT("X")},false,{},{}),\
    ({jjT("-cc")},false,{},{}),\
    ({jjT("-a"),jjT("X")},true,{1,0,0},{{jjT("X")},{},{}}),\
    ({jjT("-a=X")},true,{1,0,0},{{jjT("X")},{},{}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("a")) }, 1u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("b")) }, 3u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("c")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    args.ParserOptions >> flags_t::ALLOW_STACKS;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CASE_VARIANTS(stackedValues, (const std::initializer_list<jj::string_t>& argv, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-aX"),jjT("-b1"),jjT("2"),jjT("3"),jjT("-c")},true,{1,1,1},{{jjT("X")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-ab")},true,{1,0,0},{{jjT("b")},{},{}}),\
    ({jjT("-cbccc")},false,{},{}),\
    ({jjT("-cbccc"),jjT("-c"),jjT("-c"),jjT("-ac")},true,{1,1,1},{{jjT("c")},{jjT("ccc"),jjT("-c"),jjT("-c")},{}}),\
    ({jjT("-a-c")},true,{1,0,0},{{jjT("-c")},{},{}}),\
    ({jjT("-a=c")},true,{1,0,0},{{jjT("c")},{},{}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("a")) }, 1u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("b")) }, 3u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("c")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    args.ParserOptions << flags_t::ALLOW_STACK_VALUES;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CASE_VARIANTS(disabledAssigns, (const std::initializer_list<jj::string_t>& argv, bool ok, bool allowStackedValues, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-a"),jjT("X"),jjT("-cb"),jjT("1"),jjT("2"),jjT("3"),jjT("--param"),jjT("A"),jjT("--option")},true,false,{1,1,1,1,1},{{jjT("X")},{jjT("1"),jjT("2"),jjT("3")},{},{jjT("A")},{}}),\
    ({jjT("-a=X")},false,false,{},{}),\
    ({jjT("-a=X")},true,true,{1,0,0,0,0},{{jjT("=X")},{},{},{},{}}), /*!!! this is weird but correct*/ \
    ({jjT("-aX")},true,true,{1,0,0,0,0},{{jjT("X")},{},{},{},{}}),\
    ({jjT("-cb=1"),jjT("2"),jjT("3")},false,false,{},{}),\
    ({jjT("-cb=1"),jjT("2"),jjT("3")},true,true,{0,1,1,0,0},{{},{jjT("=1"),jjT("2"),jjT("3")},{},{},{}}), /*!!! this is weird but correct*/ \
    ({jjT("-cb1"),jjT("2"),jjT("3")},true,true,{0,1,1,0,0},{{},{jjT("1"),jjT("2"),jjT("3")},{},{},{}}),\
    ({jjT("--param=A")},false,false,{},{}),\
    ({jjT("--paramA")},false,true,{},{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("a")) }, 1u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("b")) }, 3u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("c")) }, 0u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("param")) }, 1u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("option")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    args.ParserOptions >> flags_t::ALLOW_SHORT_ASSIGN >> flags_t::ALLOW_LONG_ASSIGN;
    if (allowStackedValues) args.ParserOptions << flags_t::ALLOW_STACK_VALUES;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CLASS_END(cmdLineOptionsTests_t, parseShortOptions, parseLongOptions, redefinedPrefixes, stackedOptions, valuedLongOptions, valuedShortOptions, looseStackValues, disabledStacks, stackedValues, disabledAssigns)

//================================================

JJ_TEST_CLASS_DERIVED(cmdLineListsTests_t, public cmdLineOptionsCommon_t)

void setup_single_option(definitions_t& defs, optinfos_t& infos, const std::list<name_t>& names, jj::string_t end, multiple_t multi)
{
    defs.ListOptions.push_back({ names, end, jjT(""), multi,
        [this, &infos, names](const listDefinition_t&, values_t& v) {
        ++infos.infos[names.front()].Count;
        return true;
    } });
    infos.names.push_back(names.front());
    infos.infos[names.front()].Known = true;
    infos.infos[names.front()].Type = arguments_t::TLIST;
}

cmdLineListsTests_t() : cmdLineOptionsCommon_t(static_cast<jj::test::testclass_base_t&>(*this)) {}

JJ_TEST_CASE_VARIANTS(parseShortOptions,(const std::initializer_list<jj::string_t>& argv, bool mustterm, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-a"),jjT("v1"),jjT("END"),jjT("-b"),jjT("end"),jjT("-c"),jjT("A"),jjT("B"),jjT("C"), jjT("")},false,true,{1,1,1},{{jjT("v1")},{},{jjT("A"),jjT("B"),jjT("C")}}),\
    ({jjT("-a"),jjT("v1"),jjT("END"),jjT("-b"),jjT("end"),jjT("-c"),jjT("A"),jjT("B"),jjT("C")},false,true,{1,1,1},{{jjT("v1")},{},{jjT("A"),jjT("B"),jjT("C")}}),\
    ({jjT("-a"),jjT("v1"),jjT("END"),jjT("-b"),jjT("end"),jjT("-c"),jjT("A"),jjT("B"),jjT("C")},true,false,{},{}),\
    ({jjT("-a"),jjT("-b"),jjT("end"),jjT("-c"),jjT("END")},false,true,{1,0,0},{{jjT("-b"),jjT("end"),jjT("-c")},{},{}}),\
    ({jjT("-a"),jjT("-b"),jjT("end"),jjT("-c")},false,true,{1,0,0},{{jjT("-b"),jjT("end"),jjT("-c")},{},{}}),\
    ({jjT("-a"),jjT("-b"),jjT("end"),jjT("-c")},true,false,{},{}),\
    ({jjT("-abc"),jjT("A"),jjT("END"),jjT("B"),jjT("end"),jjT("C"),jjT("")},true,false,{},{}),\
    ({jjT("-ab"),jjT("A"),jjT("END"),jjT("B"),jjT("end")},true,false,{},{}),\
    ({jjT("-a=v1"),jjT("END"),jjT("-b=1"),jjT("2"),jjT("end")},true,true,{1,1,0},{{jjT("v1")},{jjT("1"),jjT("2")},{}}),\
    ({jjT("-a=v1")},false,true,{1,0,0},{{jjT("v1")},{},{}}),\
    ({jjT("-a=v1")},true,false,{},{}),\
    ({jjT("-a=END")},true,true,{1,0,0},{{},{},{}}),\
    ({jjT("-c=")},true,true,{0,0,1},{{},{},{}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT('a')) }, jjT("END"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('b')) }, jjT("end"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('c')) }, jjT(""), multiple_t::OVERRIDE);
    arguments_t args;
    if (mustterm) args.ParserOptions << flags_t::LIST_MUST_TERMINATE;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CASE_VARIANTS(parseLongOptions,(const std::initializer_list<jj::string_t>& argv, bool mustterm, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("--aa"),jjT("v1"),jjT("END"),jjT("--bb"),jjT("end"),jjT("--cc"),jjT("A"),jjT("B"),jjT("C"), jjT("")},false,true,{1,1,1},{{jjT("v1")},{},{jjT("A"),jjT("B"),jjT("C")}}),\
    ({jjT("--aa"),jjT("v1"),jjT("END"),jjT("--bb"),jjT("end"),jjT("--cc"),jjT("A"),jjT("B"),jjT("C")},false,true,{1,1,1},{{jjT("v1")},{},{jjT("A"),jjT("B"),jjT("C")}}),\
    ({jjT("--aa"),jjT("v1"),jjT("END"),jjT("--bb"),jjT("end"),jjT("--cc"),jjT("A"),jjT("B"),jjT("C")},true,false,{},{}),\
    ({jjT("--aa"),jjT("--bb"),jjT("end"),jjT("--cc"),jjT("END")},false,true,{1,0,0},{{jjT("--bb"),jjT("end"),jjT("--cc")},{},{}}),\
    ({jjT("--aa"),jjT("--bb"),jjT("end"),jjT("--cc")},false,true,{1,0,0},{{jjT("--bb"),jjT("end"),jjT("--cc")},{},{}}),\
    ({jjT("--aa"),jjT("--bb"),jjT("end"),jjT("--cc")},true,false,{},{}),\
    ({jjT("--aa=v1"),jjT("END"),jjT("--bb=1"),jjT("2"),jjT("end")},true,true,{1,1,0},{{jjT("v1")},{jjT("1"),jjT("2")},{}}),\
    ({jjT("--aa=v1")},false,true,{1,0,0},{{jjT("v1")},{},{}}),\
    ({jjT("--aa=v1")},true,false,{},{}),\
    ({jjT("--aa=END")},true,true,{1,0,0},{{},{},{}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("aa")) }, jjT("END"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("bb")) }, jjT("end"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("cc")) }, jjT(""), multiple_t::OVERRIDE);
    arguments_t args;
    if (mustterm) args.ParserOptions << flags_t::LIST_MUST_TERMINATE;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CASE_VARIANTS(looseStackValues,(const std::initializer_list<jj::string_t>& argv, bool mustterm, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-abc"),jjT("A"),jjT("END"),jjT("B"),jjT("end"),jjT("C"),jjT("")},true,true,{1,1,1},{{jjT("A")},{jjT("B")},{jjT("C")}}),\
    ({jjT("-abc"),jjT("A"),jjT("END"),jjT("B"),jjT("end")},false,true,{1,1,1},{{jjT("A")},{jjT("B")},{}}),\
    ({jjT("-abc"),jjT("A"),jjT("END"),jjT("B"),jjT("end")},true,false,{},{}),\
    ({jjT("-abc"),jjT("END"),jjT("end"),jjT("")},true,true,{1,1,1},{{},{},{}}),\
    ({jjT("-ab=A"),jjT("END"),jjT("B"),jjT("end")},false,true,{1,1,0},{{jjT("A")},{jjT("B")},{}}),\
    ({jjT("-ab=END"),jjT("B"),jjT("end")},false,true,{1,1,0},{{},{jjT("B")},{}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT('a')) }, jjT("END"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('b')) }, jjT("end"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('c')) }, jjT(""), multiple_t::OVERRIDE);
    arguments_t args;
    args.ParserOptions << stackOptionValues_t::LOOSE;
    if (mustterm) args.ParserOptions << flags_t::LIST_MUST_TERMINATE;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CASE_VARIANTS(stackedValues,(const std::initializer_list<jj::string_t>& argv, bool mustterm, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-aX"),jjT("END"),jjT("-b1"),jjT("2"),jjT("end"),jjT("-c"),jjT("")},false,true,{1,1,1},{{jjT("X")},{jjT("1"),jjT("2")},{}}),\
    ({jjT("-aX"),jjT("END"),jjT("-b1"),jjT("2"),jjT("end"),jjT("-c")},false,true,{1,1,1},{{jjT("X")},{jjT("1"),jjT("2")},{}}),\
    ({jjT("-aX"),jjT("END"),jjT("-b1"),jjT("2"),jjT("end"),jjT("-c")},true,false,{},{}),\
    ({jjT("-ab"),jjT("END")},false,true,{1,0,0},{{jjT("b")},{},{}}),\
    ({jjT("-ab")},false,true,{1,0,0},{{jjT("b")},{},{}}),\
    ({jjT("-ab")},true,false,{},{}),\
    ({jjT("-aEND"),jjT("-b"),jjT("X"),jjT("end")},true,true,{1,1,0},{{},{jjT("X")},{}}),\
    ({jjT("-a=X"),jjT("END")},true,true,{1,0,0},{{jjT("X")},{},{}}),\
    ({jjT("-a=X")},false,true,{1,0,0},{{jjT("X")},{},{}}),\
    ({jjT("-a=X")},true,false,{},{}),\
    ({jjT("-c"),jjT("")},true,true,{0,0,1},{{},{},{}}),\
    ({jjT("-c")},true,false,{},{}),\
    ({jjT("-cX"),jjT("")},true,true,{0,0,1},{{},{},{jjT("X")}}),\
    ({jjT("-cX")},false,true,{0,0,1},{{},{},{jjT("X")}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT('a')) }, jjT("END"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('b')) }, jjT("end"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('c')) }, jjT(""), multiple_t::OVERRIDE);
    arguments_t args;
    args.ParserOptions << flags_t::ALLOW_STACK_VALUES;
    if (mustterm) args.ParserOptions << flags_t::LIST_MUST_TERMINATE;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CASE_VARIANTS(disabledAssigns,(const std::initializer_list<jj::string_t>& argv, bool mustterm, bool allowStackValues, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-a"),jjT("X"),jjT("END"),jjT("-c"),jjT(""),jjT("--param"),jjT("1"),jjT("2"),jjT("3"),jjT("end"),jjT("--option"),jjT("")},true,false,true,{1,1,1,1},{{jjT("X")},{},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-a=X"),jjT("END")},true,false,false,{},{}),\
    ({jjT("-a=X"),jjT("END")},true,true,true,{1,0,0,0},{{jjT("=X")},{},{},{}}),\
    ({jjT("-aX"),jjT("END")},true,true,true,{1,0,0,0},{{jjT("X")},{},{},{}}),\
    ({jjT("-aX")},true,true,false,{},{}),\
    ({jjT("-aX")},false,true,true,{1,0,0,0},{{jjT("X")},{},{},{}}),\
    ({jjT("-c")},true,true,false,{},{}),\
    ({jjT("-c")},false,true,true,{0,1,0,0},{{},{},{},{}}),\
    ({jjT("-c"), jjT("")},true,true,true,{0,1,0,0},{{},{},{},{}}),\
    ({jjT("-c")},false,true,true,{0,1,0,0},{{},{},{},{}}),\
    ({jjT("-cX"),jjT("")},false,true,true,{0,1,0,0},{{},{jjT("X")},{},{}}),\
    ({jjT("-c=")},false,true,true,{0,1,0,0},{{},{jjT("=")},{},{}}),\
    ({jjT("-c=")},true,true,false,{},{}),\
    ({jjT("--param=X"),jjT("END")},true,false,false,{},{}),\
    ({jjT("--paramX"),jjT("END")},true,true,false,{},{}),\
    ({jjT("--option")},true,true,false,{},{}),\
    ({jjT("--option=")},true,true,false,{},{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("a")) }, jjT("END"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("c")) }, jjT(""), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("param")) }, jjT("end"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("option")) }, jjT(""), multiple_t::OVERRIDE);
    arguments_t args;
    args.ParserOptions >> flags_t::ALLOW_SHORT_ASSIGN >> flags_t::ALLOW_LONG_ASSIGN;
    if (allowStackValues) args.ParserOptions << flags_t::ALLOW_STACK_VALUES;
    if (mustterm) args.ParserOptions << flags_t::LIST_MUST_TERMINATE;
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CLASS_END(cmdLineListsTests_t, parseShortOptions, parseLongOptions, looseStackValues, stackedValues, disabledAssigns)
