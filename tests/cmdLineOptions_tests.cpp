#include "jj/test/test.h"
#include "cmdLine_tests.h"
#include "jj/cmdLine.h"
#include <vector>

using namespace jj::cmdLine;

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

    const arguments_t::option_t& checkOption(const arguments_t& args, const definitions_t& defs, const name_t& name, arguments_t::optionType_t type)
    {
        arguments_t::options_t::const_iterator fnd = args.Options.find(name);
        JJ_ENSURE(fnd!=args.Options.end());
        const arguments_t::option_t& opt = fnd->second;
        JJ_ENSURE(opt.first.Type==type);
        if (type==arguments_t::TREG)
        {
            JJ_ENSURE(opt.first.u.Opt!=nullptr);
            nameCompare_t cmp(args.OptionCase);
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
            nameCompare_t cmp(args.OptionCase);
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

#undef JJ_TEST_CLASS_ACCESSOR
#define JJ_TEST_CLASS_ACCESSOR
private:
    jj::test::testclass_base_t& base_;
};

JJ_TEST_CLASS_DERIVED(cmdLineOptionsTests_t, public cmdLineOptionsCommon_t)

cmdLineOptionsTests_t() : cmdLineOptionsCommon_t(static_cast<jj::test::testclass_base_t&>(*this)) {}

JJ_TEST_CASE_VARIANTS(parseShortOptions,(const std::initializer_list<jj::string_t>& argv, const std::initializer_list<int>& parsed),\
    ({}, {0,0,0,0}),({jjT("-a")}, {1,0,0,0}),({jjT("-A"),jjT("-c"),jjT("-1")}, {1,0,1,1}),({jjT("-a"),jjT("-A"),jjT("-a"),jjT("-a")},{4,0,0,0}))
{
    std::map<int,int> cnt;
    definitions_t defs;
    defs.Options.push_back({{name_t(jjT('a')),name_t(jjT('A'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[0]; return true; } });
    defs.Options.push_back({{name_t(jjT('C'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[1]; return true; } });
    defs.Options.push_back({{name_t(jjT('c'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[2]; return true; } });
    defs.Options.push_back({{name_t(jjT('1'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[3]; return true; } });
    arguments_t args;
    arg_info_t cvt(argv);
    args.parse(defs, cvt.argc, cvt.argv);
    size_t i = 0;
    for (int c : parsed)
    {
        JJ_TEST(cnt[int(i)]==c, i << jjT("th count matches; ") << c << jjT(" expected, have ") << cnt[int(i)]);
        if (c>0)
        {
            if (i==0) checkOption(args, defs, name_t(jjT('a')), arguments_t::TREG);
            if (i==1) checkOption(args, defs, name_t(jjT('C')), arguments_t::TREG);
            if (i==2) checkOption(args, defs, name_t(jjT('c')), arguments_t::TREG);
            if (i==3) checkOption(args, defs, name_t(jjT('1')), arguments_t::TREG);
        }
        ++i;
    }
}

JJ_TEST_CASE_VARIANTS(parseLongOptions,(const std::initializer_list<jj::string_t>& argv, const std::initializer_list<int>& parsed),\
    ({jjT("--first")}, {1,0,0}),({jjT("--FIRST"),jjT("-1")},{2,0,0}),({jjT("--3rd"),jjT("--first"),jjT("--second")},{1,1,1}))
{
    std::map<int,int> cnt;
    definitions_t defs;
    defs.Options.push_back({{name_t(jjT("first")),name_t(jjT("FIRST")),name_t(jjT('1'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[0]; return true; } });
    defs.Options.push_back({{name_t(jjT("second"))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[1]; return true; } });
    defs.Options.push_back({{name_t(jjT("3rd"))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[2]; return true; } });
    arguments_t args;
    arg_info_t cvt(argv);
    args.parse(defs, cvt.argc, cvt.argv);
    size_t i = 0;
    for (int c : parsed)
    {
        JJ_TEST(cnt[int(i)]==c, i << jjT("th count matches; ") << c << jjT(" expected, have ") << cnt[int(i)]);
        if (c>0)
        {
            if (i==0) checkOption(args, defs, name_t(jjT("first")), arguments_t::TREG);
            if (i==1) checkOption(args, defs, name_t(jjT("second")), arguments_t::TREG);
            if (i==2) checkOption(args, defs, name_t(jjT("3rd")), arguments_t::TREG);
        }
        ++i;
    }
}

JJ_TEST_CASE_VARIANTS(redefinedPrefixes,(const std::initializer_list<jj::string_t>& argv, const std::initializer_list<int>& parsed),\
    ({},{0,0,0}),({jjT("-argument")},{1,0,0}),({jjT("+-X")},{0,0,1}),({jjT("--A"),jjT("+-X"),jjT("--b"),jjT("-argument")},{2,1,1}))
{
    std::map<int,int> cnt;
    definitions_t defs;
    defs.Options.push_back({{name_t(jjT("-"),jjT("argument")),name_t(jjT("--"),jjT('A'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[0]; return true; } });
    defs.Options.push_back({{name_t(jjT("--"),jjT("b"))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[1]; return true; } });
    defs.Options.push_back({{name_t(jjT("+-"),jjT("X"))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[2]; return true; } });
    arguments_t args;
    args.PrefixInfo[jjT("-")].Type = LONG_OPTION;
    args.PrefixInfo[jjT("--")].Type = SHORT_OPTION;
    args.PrefixInfo[jjT("+-")].Type = LONG_OPTION;
    arg_info_t cvt(argv);
    args.parse(defs, cvt.argc, cvt.argv);
    size_t i = 0;
    for (int c : parsed)
    {
        JJ_TEST(cnt[int(i)]==c, i << jjT("th count matches; ") << c << jjT(" expected, have ") << cnt[int(i)]);
        if (c>0)
        {
            if (i==0) checkOption(args, defs, name_t(jjT('-'),jjT("argument")), arguments_t::TREG);
            if (i==1) checkOption(args, defs, name_t(jjT("--"),jjT('b')), arguments_t::TREG);
            if (i==2) checkOption(args, defs, name_t(jjT("+-"),jjT('X')), arguments_t::TREG);
        }
        ++i;
    }
}

JJ_TEST_CASE_VARIANTS(stackedOptions,(const std::initializer_list<jj::string_t>& argv, const std::initializer_list<int>& parsed),\
    ({jjT("-xy")},{1,1,0,0,0}),({jjT("-xyz")},{1,1,1,0,0}),({jjT("-x"),jjT("--x")},{1,0,0,1,0}),\
    ({jjT("-yzx"),jjT("-zxy"),jjT("-z"),jjT("-zyx"),jjT("-x"),jjT("-zz")},{4,3,6,0,0}),\
    ({jjT("--x"),jjT("--xyz")},{0,0,0,1,1}),({jjT("-xyz"),jjT("--xyz")},{1,1,1,0,1}))
{
    std::map<int,int> cnt;
    definitions_t defs;
    defs.Options.push_back({{name_t(jjT('x'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[0]; return true; } });
    defs.Options.push_back({{name_t(jjT('y'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[1]; return true; } });
    defs.Options.push_back({{name_t(jjT('z'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[2]; return true; } });
    defs.Options.push_back({{name_t(jjT("--"),jjT('x'))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[3]; return true; } });
    defs.Options.push_back({{name_t(jjT("xyz"))}, jjT(""), 0u, multiple_t::OVERRIDE, [&cnt](const optionDefinition_t&, values_t&) { ++cnt[4]; return true; } });
    arguments_t args;
    arg_info_t cvt(argv);
    args.parse(defs, cvt.argc, cvt.argv);
    size_t i = 0;
    for (int c : parsed)
    {
        JJ_TEST(cnt[int(i)]==c, i << jjT("th count matches; ") << c << jjT(" expected, have ") << cnt[int(i)]);
        if (c>0)
        {
            if (i==0) checkOption(args, defs, name_t(jjT('x')), arguments_t::TREG);
            if (i==1) checkOption(args, defs, name_t(jjT('y')), arguments_t::TREG);
            if (i==2) checkOption(args, defs, name_t(jjT('z')), arguments_t::TREG);
            if (i==3) checkOption(args, defs, name_t(jjT("--"),jjT('x')), arguments_t::TREG);
            if (i==4) checkOption(args, defs, name_t(jjT("xyz")), arguments_t::TREG);
        }
        ++i;
    }
}

JJ_TEST_CASE_VARIANTS(valuedLongOptions,(const std::initializer_list<jj::string_t>& argv, bool ok, const std::vector<int>& count, const std::list<std::list<jj::string_t>>& pvals),\
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
    definitions_t defs;
    defs.Options.push_back({{name_t(jjT("aa"))}, jjT(""), 1u, multiple_t::OVERRIDE, [this](const optionDefinition_t&, values_t& v) { JJ_TEST(v.Values.size()==1u); return true; } });
    defs.Options.push_back({{name_t(jjT("bb"))}, jjT(""), 3u, multiple_t::OVERRIDE, [this](const optionDefinition_t&, values_t& v) { JJ_TEST(v.Values.size()==3u); return true; } });
    defs.Options.push_back({{name_t(jjT("cc"))}, jjT(""), 0u, multiple_t::OVERRIDE, [this](const optionDefinition_t&, values_t& v) { JJ_TEST(v.Values.size()==0u); return true; } });
    arguments_t args;
    arg_info_t cvt(argv);
    if (ok)
        args.parse(defs, cvt.argc, cvt.argv);
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs, cvt.argc, cvt.argv), std::runtime_error);
        return;
    }

    name_t names[3] = { name_t(jjT("aa")), name_t(jjT("bb")), name_t(jjT("cc")) };
    size_t i=0;
    for (auto& sl : pvals)
    {
        if (count[i]==0)
        {
            ++i;
            continue;
        }
        const arguments_t::option_t& o = checkOption(args, defs, names[i], arguments_t::TREG);
        checkValues(o, sl);
        ++i;
    }
}

JJ_TEST_CASE_VARIANTS(valuedShortOptions,(const std::initializer_list<jj::string_t>& argv, bool ok, const std::vector<int>& count, const std::list<std::list<jj::string_t>>& pvals),\
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
    ({jjT("-c=1")},false,{},{}))
{
    definitions_t defs;
    defs.Options.push_back({{name_t(jjT("a"))}, jjT(""), 1u, multiple_t::OVERRIDE, [this](const optionDefinition_t&, values_t& v) { JJ_TEST(v.Values.size()==1u); return true; } });
    defs.Options.push_back({{name_t(jjT("b"))}, jjT(""), 3u, multiple_t::OVERRIDE, [this](const optionDefinition_t&, values_t& v) { JJ_TEST(v.Values.size()==3u); return true; } });
    defs.Options.push_back({{name_t(jjT("c"))}, jjT(""), 0u, multiple_t::OVERRIDE, [this](const optionDefinition_t&, values_t& v) { JJ_TEST(v.Values.size()==0u); return true; } });
    arguments_t args;
    arg_info_t cvt(argv);
    if (ok)
        args.parse(defs, cvt.argc, cvt.argv);
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs, cvt.argc, cvt.argv), std::runtime_error);
        return;
    }

    name_t names[3] = { name_t(jjT("a")), name_t(jjT("b")), name_t(jjT("c")) };
    size_t i=0;
    for (auto& sl : pvals)
    {
        if (count[i]==0)
        {
            ++i;
            continue;
        }
        const arguments_t::option_t& o = checkOption(args, defs, names[i], arguments_t::TREG);
        checkValues(o, sl);
        ++i;
    }
}

JJ_TEST_CASE_VARIANTS(looseStackValues,(const std::initializer_list<jj::string_t>& argv, bool ok, const std::vector<int>& count, const std::list<std::list<jj::string_t>>& pvals),\
    ({jjT("-abc"),jjT("v1"),jjT("1"),jjT("2"),jjT("3")},true,{1,1,1},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-cab"),jjT("v1"),jjT("1"),jjT("2"),jjT("3")},true,{1,1,1},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-acb"),jjT("v1"),jjT("1"),jjT("2"),jjT("3")},true,{1,1,1},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("-acb"),jjT("v1"),jjT("1"),jjT("2")},false,{},{}),\
    ({jjT("-cb"),jjT("1"),jjT("2")},false,{},{}),\
    ({jjT("-a")},false,{},{}),\
    ({jjT("-ba"),jjT("1"),jjT("2"),jjT("3")},false,{},{}),\
    ({jjT("-bccca"),jjT("1"),jjT("2"),jjT("3"),jjT("v1")},true,{1,1,3},{{jjT("v1")},{jjT("1"),jjT("2"),jjT("3")}}),\
    ({jjT("-ca")},false,{},{}),\
    ({jjT("-aa"),jjT("v1"),jjT("v2")},true,{2,0,0},{{jjT("v2")},{},{}}),\
    ({jjT("-bb"),jjT("1"),jjT("2"),jjT("3"),jjT("A"),jjT("B"),jjT("C")},true,{0,2,0},{{},{jjT("A"),jjT("B"),jjT("C")},{}}),\
    ({jjT("-c=1")},false,{},{}))
{
    definitions_t defs;
    defs.Options.push_back({{name_t(jjT("a"))}, jjT(""), 1u, multiple_t::OVERRIDE, [this](const optionDefinition_t&, values_t& v) { JJ_TEST(v.Values.size()==1u); return true; } });
    defs.Options.push_back({{name_t(jjT("b"))}, jjT(""), 3u, multiple_t::OVERRIDE, [this](const optionDefinition_t&, values_t& v) { JJ_TEST(v.Values.size()==3u); return true; } });
    defs.Options.push_back({{name_t(jjT("c"))}, jjT(""), 0u, multiple_t::OVERRIDE, [this](const optionDefinition_t&, values_t& v) { JJ_TEST(v.Values.size()==0u); return true; } });
    arguments_t args;
    args.ParserOptions << stackOptionValues_t::LOOSE;
    arg_info_t cvt(argv);
    if (ok)
        args.parse(defs, cvt.argc, cvt.argv);
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs, cvt.argc, cvt.argv), std::runtime_error);
        return;
    }

    name_t names[3] = { name_t(jjT("a")), name_t(jjT("b")), name_t(jjT("c")) };
    size_t i=0;
    for (auto& sl : pvals)
    {
        if (count[i]==0)
        {
            ++i;
            continue;
        }
        const arguments_t::option_t& o = checkOption(args, defs, names[i], arguments_t::TREG);
        checkValues(o, sl);
        ++i;
    }
}

JJ_TEST_CLASS_END(cmdLineOptionsTests_t, parseShortOptions, parseLongOptions, redefinedPrefixes, stackedOptions, valuedLongOptions, valuedShortOptions, looseStackValues)

//================================================

JJ_TEST_CLASS_DERIVED(cmdLineListsTests_t, public cmdLineOptionsCommon_t)

    cmdLineListsTests_t() : cmdLineOptionsCommon_t(static_cast<jj::test::testclass_base_t&>(*this)) {}

JJ_TEST_CASE_VARIANTS(parseShortOptions,(const std::initializer_list<jj::string_t>& argv, bool mustterm, bool ok, const std::vector<bool>& count, const std::list<std::list<jj::string_t>>& pvals),\
    ({jjT("-a"),jjT("v1"),jjT("END"),jjT("-b"),jjT("end"),jjT("-c"),jjT("A"),jjT("B"),jjT("C"), jjT("")},false,true,{true,true,true},{{jjT("v1")},{},{jjT("A"),jjT("B"),jjT("C")}}),\
    ({jjT("-a"),jjT("v1"),jjT("END"),jjT("-b"),jjT("end"),jjT("-c"),jjT("A"),jjT("B"),jjT("C")},false,true,{true,true,true},{{jjT("v1")},{},{jjT("A"),jjT("B"),jjT("C")}}),\
    ({jjT("-a"),jjT("v1"),jjT("END"),jjT("-b"),jjT("end"),jjT("-c"),jjT("A"),jjT("B"),jjT("C")},true,false,{},{}),\
    ({jjT("-a"),jjT("-b"),jjT("end"),jjT("-c"),jjT("END")},false,true,{true,false,false},{{jjT("-b"),jjT("end"),jjT("-c")}}),\
    ({jjT("-a"),jjT("-b"),jjT("end"),jjT("-c")},false,true,{true,false,false},{{jjT("-b"),jjT("end"),jjT("-c")}}),\
    ({jjT("-a"),jjT("-b"),jjT("end"),jjT("-c")},true,false,{},{}),\
    ({jjT("-abc"),jjT("A"),jjT("END"),jjT("B"),jjT("end"),jjT("C"),jjT("")},true,false,{},{}),\
    ({jjT("-ab"),jjT("A"),jjT("END"),jjT("B"),jjT("end")},true,false,{},{}),\
    ({jjT("-a=v1"),jjT("END"),jjT("-b=1"),jjT("2"),jjT("end")},true,true,{true,true,false},{{jjT("v1")},{jjT("1"),jjT("2")},{}}),\
    ({jjT("-a=v1")},false,true,{true,false,false},{{jjT("v1")},{},{}}),\
    ({jjT("-a=v1")},true,false,{},{}),\
    ({jjT("-a=END")},true,true,{true,false,false},{{},{},{}}))
{
    definitions_t defs;
    defs.ListOptions.push_back({{name_t(jjT('a'))}, jjT("END"), jjT(""), multiple_t::OVERRIDE, nullptr });
    defs.ListOptions.push_back({{name_t(jjT('b'))}, jjT("end"), jjT(""), multiple_t::OVERRIDE, nullptr });
    defs.ListOptions.push_back({{name_t(jjT('c'))}, jjT(""), jjT(""), multiple_t::OVERRIDE, nullptr });
    arguments_t args;
    if (mustterm) args.ParserOptions << flags_t::LIST_MUST_TERMINATE;
    arg_info_t cvt(argv);
    if (ok)
        args.parse(defs, cvt.argc, cvt.argv);
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs, cvt.argc, cvt.argv), std::runtime_error);
        return;
    }

    name_t names[3] = { name_t(jjT("a")), name_t(jjT("b")), name_t(jjT("c")) };
    size_t i=0;
    for (auto& sl : pvals)
    {
        if (!count[i])
        {
            ++i;
            continue;
        }
        const arguments_t::option_t& o = checkOption(args, defs, names[i], arguments_t::TLIST);
        checkValues(o, sl);
        ++i;
    }
}

JJ_TEST_CASE_VARIANTS(parseLongOptions,(const std::initializer_list<jj::string_t>& argv, bool mustterm, bool ok, const std::vector<bool>& count, const std::list<std::list<jj::string_t>>& pvals),\
    ({jjT("--aa"),jjT("v1"),jjT("END"),jjT("--bb"),jjT("end"),jjT("--cc"),jjT("A"),jjT("B"),jjT("C"), jjT("")},false,true,{true,true,true},{{jjT("v1")},{},{jjT("A"),jjT("B"),jjT("C")}}),\
    ({jjT("--aa"),jjT("v1"),jjT("END"),jjT("--bb"),jjT("end"),jjT("--cc"),jjT("A"),jjT("B"),jjT("C")},false,true,{true,true,true},{{jjT("v1")},{},{jjT("A"),jjT("B"),jjT("C")}}),\
    ({jjT("--aa"),jjT("v1"),jjT("END"),jjT("--bb"),jjT("end"),jjT("--cc"),jjT("A"),jjT("B"),jjT("C")},true,false,{},{}),\
    ({jjT("--aa"),jjT("--bb"),jjT("end"),jjT("--cc"),jjT("END")},false,true,{true,false,false},{{jjT("--bb"),jjT("end"),jjT("--cc")}}),\
    ({jjT("--aa"),jjT("--bb"),jjT("end"),jjT("--cc")},false,true,{true,false,false},{{jjT("--bb"),jjT("end"),jjT("--cc")}}),\
    ({jjT("--aa"),jjT("--bb"),jjT("end"),jjT("--cc")},true,false,{},{}),\
    ({jjT("--aa=v1"),jjT("END"),jjT("--bb=1"),jjT("2"),jjT("end")},true,true,{true,true,false},{{jjT("v1")},{jjT("1"),jjT("2")},{}}),\
    ({jjT("--aa=v1")},false,true,{true,false,false},{{jjT("v1")},{},{}}),\
    ({jjT("--aa=v1")},true,false,{},{}),\
    ({jjT("--aa=END")},true,true,{true,false,false},{{},{},{}}))
{
    definitions_t defs;
    defs.ListOptions.push_back({{name_t(jjT("aa"))}, jjT("END"), jjT(""), multiple_t::OVERRIDE, nullptr });
    defs.ListOptions.push_back({{name_t(jjT("bb"))}, jjT("end"), jjT(""), multiple_t::OVERRIDE, nullptr });
    defs.ListOptions.push_back({{name_t(jjT("cc"))}, jjT(""), jjT(""), multiple_t::OVERRIDE, nullptr });
    arguments_t args;
    if (mustterm) args.ParserOptions << flags_t::LIST_MUST_TERMINATE;
    arg_info_t cvt(argv);
    if (ok)
        args.parse(defs, cvt.argc, cvt.argv);
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs, cvt.argc, cvt.argv), std::runtime_error);
        return;
    }

    name_t names[3] = { name_t(jjT("aa")), name_t(jjT("bb")), name_t(jjT("cc")) };
    size_t i=0;
    for (auto& sl : pvals)
    {
        if (!count[i])
        {
            ++i;
            continue;
        }
        const arguments_t::option_t& o = checkOption(args, defs, names[i], arguments_t::TLIST);
        checkValues(o, sl);
        ++i;
    }
}

JJ_TEST_CASE_VARIANTS(looseStackValues,(const std::initializer_list<jj::string_t>& argv, bool mustterm, bool ok, const std::vector<bool>& count, const std::list<std::list<jj::string_t>>& pvals),\
    ({jjT("-abc"),jjT("A"),jjT("END"),jjT("B"),jjT("end"),jjT("C"),jjT("")},true,true,{true,true,true},{{jjT("A")},{jjT("B")},{jjT("C")}}),\
    ({jjT("-abc"),jjT("A"),jjT("END"),jjT("B"),jjT("end")},false,true,{true,true,true},{{jjT("A")},{jjT("B")},{}}),\
    ({jjT("-abc"),jjT("A"),jjT("END"),jjT("B"),jjT("end")},true,false,{},{}),\
    ({jjT("-abc"),jjT("END"),jjT("end"),jjT("")},true,true,{true,true,true},{{},{},{}}),\
    ({jjT("-ab=A"),jjT("END"),jjT("B"),jjT("end")},false,true,{true,true,false},{{jjT("A")},{jjT("B")},{}}),\
    ({jjT("-ab=END"),jjT("B"),jjT("end")},false,true,{true,true,false},{{},{jjT("B")},{}}))
{
    definitions_t defs;
    defs.ListOptions.push_back({{name_t(jjT('a'))}, jjT("END"), jjT(""), multiple_t::OVERRIDE, nullptr });
    defs.ListOptions.push_back({{name_t(jjT('b'))}, jjT("end"), jjT(""), multiple_t::OVERRIDE, nullptr });
    defs.ListOptions.push_back({{name_t(jjT('c'))}, jjT(""), jjT(""), multiple_t::OVERRIDE, nullptr });
    arguments_t args;
    args.ParserOptions << stackOptionValues_t::LOOSE;
    if (mustterm) args.ParserOptions << flags_t::LIST_MUST_TERMINATE;
    arg_info_t cvt(argv);
    if (ok)
        args.parse(defs, cvt.argc, cvt.argv);
    else
    {
        JJ_TEST_THAT_THROWS(args.parse(defs, cvt.argc, cvt.argv), std::runtime_error);
        return;
    }

    name_t names[3] = { name_t(jjT("a")), name_t(jjT("b")), name_t(jjT("c")) };
    size_t i=0;
    for (auto& sl : pvals)
    {
        if (!count[i])
        {
            ++i;
            continue;
        }
        const arguments_t::option_t& o = checkOption(args, defs, names[i], arguments_t::TLIST);
        checkValues(o, sl);
        ++i;
    }
}

JJ_TEST_CLASS_END(cmdLineListsTests_t, parseShortOptions, parseLongOptions, looseStackValues)
