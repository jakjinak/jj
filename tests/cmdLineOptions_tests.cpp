#include "cmdLine_tests.h"

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

JJ_TEST_CLASS_DERIVED(cmdLineOptionsTests_t, public cmdLineOptionsCommon_t)

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
    ({jjT("--bb=1"),jjT("2"),jjT("3")},true,{0,1,0},{{},{jjT("1"),jjT("2"),jjT("3")},{}}),\
    ({jjT("--aa=X=Y")},true,{1,0,0},{{jjT("X=Y")},{},{}}))
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
    ({jjT("-a=")},true,{1,0,0},{{jjT("")},{},{}}),\
    ({jjT("-a=X=Y")},true,{1,0,0},{{jjT("X=Y")},{},{}}))
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

JJ_TEST_CASE_VARIANTS(disabledStacksButStackedValues, (const std::initializer_list<jj::string_t>& argv, bool ok, bool allowStackedValues, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-ab")},true,true,{1,0,0},{{jjT("b")},{},{}}),\
    ({jjT("-ab"),jjT("1"),jjT("2"),jjT("3")},false,false,{},{}),\
    ({jjT("-abc")},true,true,{1,0,0},{{jjT("bc")},{},{}}),\
    ({jjT("-a=bc")},true,true,{1,0,0},{{jjT("bc")},{},{}}),\
    ({jjT("-aa")},true,true,{1,0,0},{{jjT("a")},{},{}}),\
    ({jjT("-ba")},false,true,{},{}),\
    ({jjT("-babc"),jjT("-b"),jjT("-c")},true,true,{0,1,0},{{},{jjT("abc"),jjT("-b"),jjT("-c")},{}}),\
    ({jjT("-ca"),jjT("X")},false,true,{},{}),\
    ({jjT("-cc")},false,true,{},{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("a")) }, 1u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("b")) }, 3u, multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT("c")) }, 0u, multiple_t::OVERRIDE);
    arguments_t args;
    args.ParserOptions >> flags_t::ALLOW_STACKS;
    if (allowStackedValues)
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

JJ_TEST_CASE_VARIANTS(valueModes, (const std::initializer_list<jj::string_t>& argv, const std::initializer_list<flags_t>& flags, multiple_t valueMode, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals),\
    ({jjT("-a"),jjT("X"),jjT("-a"),jjT("Y"),jjT("-a"),jjT("Z")},{},multiple_t::OVERRIDE,true,{3,0,0,0},{{jjT("Z")},{},{},{}}),\
    ({jjT("-a"),jjT("X"),jjT("-a"),jjT("Y"),jjT("-a"),jjT("Z")},{},multiple_t::PRESERVE,true,{3,0,0,0},{{jjT("X")},{},{},{}}),\
    ({jjT("-a"),jjT("X"),jjT("-a"),jjT("Y"),jjT("-a"),jjT("Z")},{},multiple_t::JOIN,true,{3,0,0,0},{{jjT("X"),jjT("Y"),jjT("Z")},{},{},{}}),\
    ({jjT("-a"),jjT("X"),jjT("-a"),jjT("Y"),jjT("-a"),jjT("Z")},{},multiple_t::ERROR,false,{},{}),\
    ({jjT("-b"),jjT("1"),jjT("2"),jjT("3"),jjT("-b"),jjT("4"),jjT("5"),jjT("6")},{},multiple_t::OVERRIDE,true,{0,2,0,0},{{},{jjT("4"),jjT("5"),jjT("6")},{},{}}),\
    ({jjT("-b"),jjT("1"),jjT("2"),jjT("3"),jjT("-b"),jjT("4"),jjT("5"),jjT("6")},{},multiple_t::PRESERVE,true,{0,2,0,0},{{},{jjT("1"),jjT("2"),jjT("3")},{},{}}),\
    ({jjT("-b"),jjT("1"),jjT("2"),jjT("3"),jjT("-b"),jjT("4"),jjT("5"),jjT("6")},{},multiple_t::JOIN,true,{0,2,0,0},{{},{jjT("1"),jjT("2"),jjT("3"),jjT("4"),jjT("5"),jjT("6")},{},{}}),\
    ({jjT("-b"),jjT("1"),jjT("2"),jjT("3"),jjT("-b"),jjT("4"),jjT("5"),jjT("6")},{},multiple_t::ERROR,false,{},{}),
    ({jjT("-cc")},{},multiple_t::OVERRIDE,true,{0,0,2,0},{{},{},{},{}}),\
    ({jjT("-cc")},{},multiple_t::PRESERVE,true,{0,0,2,0},{{},{},{},{}}),\
    ({jjT("-cc")},{},multiple_t::JOIN,true,{0,0,2,0},{{},{},{},{}}),\
    ({jjT("-cc")},{},multiple_t::ERROR,false,{},{}),\
    ({jjT("--long"),jjT("X"),jjT("--long"),jjT("Y")},{},multiple_t::OVERRIDE,true,{0,0,0,2},{{},{},{},{jjT("Y")}}),\
    ({jjT("--long"),jjT("X"),jjT("--long"),jjT("Y")},{},multiple_t::PRESERVE,true,{0,0,0,2},{{},{},{},{jjT("X")}}),\
    ({jjT("--long"),jjT("X"),jjT("--long"),jjT("Y")},{},multiple_t::JOIN,true,{0,0,0,2},{{},{},{},{jjT("X"),jjT("Y")}}),\
    ({jjT("--long"),jjT("X"),jjT("--long"),jjT("Y")},{},multiple_t::ERROR,false,{},{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("a")) }, 1u, valueMode);
    setup_single_option(defs, infos, { name_t(jjT("b")) }, 3u, valueMode);
    setup_single_option(defs, infos, { name_t(jjT("c")) }, 0u, valueMode);
    setup_single_option(defs, infos, { name_t(jjT("long")) }, 1u, valueMode);
    arguments_t args;
    jj::opt::f<flags_t, flags_t::MAX_FLAGS>& fs = args.ParserOptions;
    for (auto f : flags)
        fs.toggle(f);
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CLASS_END(cmdLineOptionsTests_t, parseShortOptions, parseLongOptions, redefinedPrefixes, stackedOptions, valuedLongOptions, valuedShortOptions,\
    looseStackValues, disabledStacks, stackedValues, disabledStacksButStackedValues, disabledAssigns, valueModes)

//================================================

JJ_TEST_CLASS_DERIVED(cmdLineListsTests_t, public cmdLineOptionsCommon_t)

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
    ({jjT("-c=")},true,true,{0,0,1},{{},{},{}}),\
    ({jjT("-a=X=Y")},false,true,{1,0,0},{{jjT("X=Y")},{},{}}))
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
    ({jjT("--aa=END")},true,true,{1,0,0},{{},{},{}}),\
    ({jjT("--aa=X=Y")},false,true,{1,0,0},{{jjT("X=Y")},{},{}}))
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

JJ_TEST_CASE_VARIANTS(disabledStacksButStackedValues, (const std::initializer_list<jj::string_t>& argv, bool mustterm, bool allowStackedValues, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals), \
    ({jjT("-ab")},false,true,true,{1,0,0},{{jjT("b")},{},{}}),\
    ({jjT("-ab"),jjT("1"),jjT("2"),jjT("3")},false,false,false,{},{}),\
    ({jjT("-abc")},false,true,true,{1,0,0},{{jjT("bc")},{},{}}),\
    ({jjT("-a=bc")},false,true,true,{1,0,0},{{jjT("bc")},{},{}}),\
    ({jjT("-aa")},false,true,true,{1,0,0},{{jjT("a")},{},{}}),\
    ({jjT("-babc"),jjT("-b"),jjT("-c")},false,true,true,{0,1,0},{{},{jjT("abc"),jjT("-b"),jjT("-c")},{}}),\
    ({jjT("-ca"),jjT("END")},true,true,false,{},{}),\
    ({jjT("-ca"),jjT("END")},false,true,true,{0,0,1},{{},{},{jjT("a"),jjT("END")}}),\
    ({jjT("-ca"),jjT("")},true,true,true,{0,0,1},{{},{},{jjT("a")}}),\
    ({jjT("-cc")},false,true,true,{0,0,1},{{},{},{jjT("c")}}),\
    ({jjT("-cc")},true,true,false,{},{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT('a')) }, jjT("END"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('b')) }, jjT("end"), multiple_t::OVERRIDE);
    setup_single_option(defs, infos, { name_t(jjT('c')) }, jjT(""), multiple_t::OVERRIDE);
    arguments_t args;
    args.ParserOptions >> flags_t::ALLOW_STACKS;
    if (allowStackedValues) args.ParserOptions << flags_t::ALLOW_STACK_VALUES;
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

JJ_TEST_CASE_VARIANTS(valueModes, (const std::initializer_list<jj::string_t>& argv, const std::initializer_list<flags_t>& flags, multiple_t valueMode, bool ok, const std::vector<int>& count, const std::vector<std::list<jj::string_t>>& pvals), \
    ({jjT("-a"),jjT("X"),jjT("END"),jjT("-a"),jjT("Y"),jjT("END")},{},multiple_t::OVERRIDE,true,{2,0,0,0},{{jjT("Y")},{},{},{}}),\
    ({jjT("-a"),jjT("X"),jjT("END"),jjT("-a"),jjT("Y"),jjT("END")},{},multiple_t::PRESERVE,true,{2,0,0,0},{{jjT("X")},{},{},{}}),\
    ({jjT("-a"),jjT("X"),jjT("END"),jjT("-a"),jjT("Y"),jjT("END")},{},multiple_t::JOIN,true,{2,0,0,0},{{jjT("X"),jjT("Y")},{},{},{}}),\
    ({jjT("-a"),jjT("X"),jjT("END"),jjT("-a"),jjT("Y"),jjT("END")},{},multiple_t::ERROR,false,{},{}),\
    ({jjT("-c"),jjT(""),jjT("-c"),jjT("Y")},{},multiple_t::OVERRIDE,true,{0,2,0,0},{{},{jjT("Y")},{},{}}),\
    ({jjT("-c"),jjT(""),jjT("-c"),jjT("Y")},{},multiple_t::PRESERVE,true,{0,2,0,0},{{},{},{},{}}),\
    ({jjT("-c"),jjT(""),jjT("-c"),jjT("Y")},{},multiple_t::JOIN,true,{0,2,0,0},{{},{jjT("Y")},{},{}}),\
    ({jjT("-c"),jjT(""),jjT("-c"),jjT("Y")},{},multiple_t::ERROR,false,{},{}),\
    ({jjT("-cc"),jjT(""),jjT("Y")},{},multiple_t::OVERRIDE,true,{0,2,0,0},{{},{jjT("Y")},{},{}}),\
    ({jjT("-cc"),jjT(""),jjT("Y")},{},multiple_t::PRESERVE,true,{0,2,0,0},{{},{},{},{}}),\
    ({jjT("-cc"),jjT(""),jjT("Y")},{},multiple_t::JOIN,true,{0,2,0,0},{{},{jjT("Y")},{},{}}),\
    ({jjT("-cc"),jjT(""),jjT("Y")},{},multiple_t::ERROR,false,{},{}),\
    ({jjT("--longa"),jjT("X"),jjT("END"),jjT("--longa"),jjT("END")},{},multiple_t::OVERRIDE,true,{0,0,2,0},{{},{},{},{}}),\
    ({jjT("--longa"),jjT("X"),jjT("END"),jjT("--longa"),jjT("END")},{},multiple_t::PRESERVE,true,{0,0,2,0},{{},{},{jjT("X")},{}}),\
    ({jjT("--longa"),jjT("X"),jjT("END"),jjT("--longa"),jjT("END")},{},multiple_t::JOIN,true,{0,0,2,0},{{},{},{jjT("X")},{}}),\
    ({jjT("--longa"),jjT("X"),jjT("END"),jjT("--longa"),jjT("END")},{},multiple_t::ERROR,false,{},{}),\
    ({jjT("--longc=1"),jjT("2"),jjT(""),jjT("--longc"),jjT(" ")},{},multiple_t::OVERRIDE,true,{0,0,0,2},{{},{},{},{jjT(" ")}}),\
    ({jjT("--longc=1"),jjT("2"),jjT(""),jjT("--longc"),jjT(" ")},{},multiple_t::PRESERVE,true,{0,0,0,2},{{},{},{},{jjT("1"),jjT("2")}}),\
    ({jjT("--longc=1"),jjT("2"),jjT(""),jjT("--longc"),jjT(" ")},{},multiple_t::JOIN,true,{0,0,0,2},{{},{},{},{jjT("1"),jjT("2"),jjT(" ")}}),\
    ({jjT("--longc=1"),jjT("2"),jjT(""),jjT("--longc"),jjT(" ")},{},multiple_t::ERROR,false,{},{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_option(defs, infos, { name_t(jjT("a")) }, jjT("END"), valueMode);
    setup_single_option(defs, infos, { name_t(jjT("c")) }, jjT(""), valueMode);
    setup_single_option(defs, infos, { name_t(jjT("longa")) }, jjT("END"), valueMode);
    setup_single_option(defs, infos, { name_t(jjT("longc")) }, jjT(""), valueMode);
    arguments_t args;
    args.ParserOptions << stackOptionValues_t::LOOSE;
    jj::opt::f<flags_t, flags_t::MAX_FLAGS>& fs = args.ParserOptions;
    for (auto f : flags)
        fs.toggle(f);
    perform_test(infos, defs, args, ok, count, pvals);
}

JJ_TEST_CLASS_END(cmdLineListsTests_t, parseShortOptions, parseLongOptions, looseStackValues, stackedValues, disabledStacksButStackedValues, disabledAssigns, valueModes)
