#include "cmdLine_tests.h"

using namespace jj::cmdLine;

nameCompare_less_t g_CASE_SENSITIVE = nameCompare_less_t(case_t::SENSITIVE), g_CASE_INSENSITIVE = nameCompare_less_t(case_t::INSENSITIVE);
const jj::char_t* arg_info_t::DefaultProgramName(jjT("ProgramName"));

void arg_info_t::print()
{
    jj::cout << jjT("ARGS[") << args.size() << jjT("]\n");
    for (auto& s : args)
        jj::cout << jjT("\t'") << s << jjT("'\n");
    jj::cout << jjT("\nARGV[") << argc << jjT("]\n");
    for (int i = 0; i < argc; ++i)
        jj::cout << jjT("\t'") << argv[i] << jjT("'\n");
}

//================================================

JJ_TEST_CLASS(cmdLineTests_t)

#if defined(_WINDOWS) || defined(_WIN32)
#define SEP L"\\"
#else
#define SEP "/"
#endif // defined(_WINDOWS) || defined(_WIN32)

JJ_TEST_CASE_VARIANTS(ProgramNameWrong_Throws,(const jj::char_t* pn),(jjT("")),(SEP jjT("bla") SEP),(jjT("bla") SEP),(SEP jjT("bla") SEP jjT("bla") SEP),(jjT("bla") SEP jjT("bla") SEP))
{
    definitions_t defs;
    arguments_t args;
    arg_info_t a(pn, {});
    JJ_TEST_THAT_THROWS(args.parse(defs, a.argc, a.argv), std::runtime_error);
}

JJ_TEST_CASE(ProgramNameNull_Throws)
{
    definitions_t defs;
    arguments_t args;
    arg_info_t a;
    a.argc = 1;
    a.argv[0] = nullptr;
    JJ_TEST_THAT_THROWS(args.parse(defs, a.argc, a.argv), std::runtime_error);
}

JJ_TEST_CASE_VARIANTS(ProgramNamePath_Parsed,(const jj::char_t* pn),(SEP jjT("pgm")),(jjT("pgm")),(SEP jjT("bla") SEP jjT("pgm")),(jjT("bla") SEP jjT("pgm")),(SEP jjT("bla") SEP jjT("bla") SEP jjT("bla") SEP jjT("pgm")))
{
    definitions_t defs;
    arguments_t args;
    arg_info_t a(pn, {});
    args.parse(defs, a.argc, a.argv);
    JJ_TEST(args.ProgramName == jjT("pgm"));
}

JJ_TEST_CLASS_END(cmdLineTests_t, ProgramNameWrong_Throws, ProgramNameNull_Throws, ProgramNamePath_Parsed)

//================================================

JJ_TEST_CLASS_DERIVED(cmdLinePositionalTests_t, public cmdLineOptionsCommon_t)

cmdLinePositionalTests_t() : cmdLineOptionsCommon_t(static_cast<jj::test::testclass_base_t&>(*this)) {}

JJ_TEST_CASE_VARIANTS(basic, (size_t mandatory, size_t optional, const std::initializer_list<jj::string_t>& argv, const std::initializer_list<parserSetupWrap_t>& flags, bool ok, const std::vector<jj::string_t>& pvals), \
    (1,1,{jjT("1"),jjT("2")},{},true,{jjT("1"),jjT("2")}),\
    (0,1,{jjT("1"),jjT("2")},{},true,{jjT("1"),jjT("2")}),\
    (1,0,{jjT("1"),jjT("2")},{},true,{jjT("1"),jjT("2")}),\
    (0,2,{jjT("1"),jjT("2")},{},true,{jjT("1"),jjT("2")}),\
    (2,0,{jjT("1"),jjT("2")},{},true,{jjT("1"),jjT("2")}),\
    (0,0,{jjT("1"),jjT("2")},{},true,{jjT("1"),jjT("2")}),\
    (0,0,{},{},true,{}),\
    (1,0,{},{},false,{}),\
    (0,1,{},{},true,{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_positionals(defs, mandatory, optional);
    arguments_t args;
    setup_parser(args, flags);
    perform_test(infos, defs, args, ok, pvals);
}

JJ_TEST_CASE_VARIANTS(noAdditional, (size_t mandatory, size_t optional, const std::initializer_list<jj::string_t>& argv, const std::initializer_list<parserSetupWrap_t>& flags, bool ok, const std::vector<jj::string_t>& pvals), \
    (1,1,{jjT("1"),jjT("2")},{},true,{jjT("1"),jjT("2")}),\
    (0,1,{jjT("1"),jjT("2")},{},false,{}),\
    (1,0,{jjT("1"),jjT("2")},{},false,{}),\
    (0,2,{jjT("1"),jjT("2")},{},true,{jjT("1"),jjT("2")}),\
    (2,0,{jjT("1"),jjT("2")},{},true,{jjT("1"),jjT("2")}),\
    (0,0,{jjT("1"),jjT("2")},{},false,{}),\
    (0,0,{},{},true,{}),\
    (1,0,{},{},false,{}),\
    (0,1,{},{},true,{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_positionals(defs, mandatory, optional);
    arguments_t args;
    args.ParserOptions << flags_t::DENY_ADDITIONAL;
    setup_parser(args, flags);
    perform_test(infos, defs, args, ok, pvals);
}

JJ_TEST_CASE_VARIANTS(delimiters, (size_t mandatory, const std::initializer_list<jj::string_t>& delimiters, const std::initializer_list<jj::string_t>& argv, const std::initializer_list<parserSetupWrap_t>& flags, bool ok, const std::vector<jj::string_t>& pvals), \
    (0,{},{jjT("-")},{},false,{}),\
    (0,{},{jjT("--")},{},false,{}),\
    (0,{},{jjT("-a")},{},false,{}),\
    (0,{jjT("--")},{jjT("--")},{},true,{}),\
    (0,{jjT("--")},{jjT("--"),jjT("-a")},{},true,{jjT("-a")}),\
    (0,{jjT("")},{jjT(""),jjT("-a")},{},true,{jjT("-a")}),\
    (0,{jjT("-a")},{jjT(""),jjT("-a")},{},true,{jjT("")}),\
    (0,{jjT("-a"),jjT("-a")},{ jjT("X"),jjT("-a"),jjT("-a"),jjT("Y")},{},true,{jjT("X"),jjT("Y")}),\
    (1,{jjT("--")},{jjT("--"),jjT("-a")},{},true,{jjT("-a")}),\
    (1,{jjT("--")},{jjT("--")},{},false,{}),
    (0,{jjT("--"),jjT("++")},{jjT("1"),jjT("--"),jjT("-a"),jjT("++"),jjT("2")},{},true,{jjT("1"),jjT("-a"),jjT("2")}),\
    (0,{jjT("-"),jjT("-")},{jjT("1"),jjT("-"),jjT("-a"),jjT("-"),jjT("2")},{},true,{jjT("1"),jjT("-a"),jjT("2")}),\
    (0,{jjT(""),jjT("")},{jjT("1"),jjT(""),jjT("-a"),jjT(""),jjT("2")},{},true,{jjT("1"),jjT("-a"),jjT("2")}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_positionals(defs, mandatory, 0);
    arguments_t args;
    std::initializer_list<jj::string_t>::const_iterator dit = delimiters.begin();
    if (dit != delimiters.end())
    {
        args.ParserOptions << flags_t::USE_POSITIONAL_DELIMITER;
        args.PositionalDelimiter = *dit;
        ++dit;
    }
    if (dit != delimiters.end())
    {
        args.ParserOptions << flags_t::USE_RETURN_DELIMITER;
        args.ReturnDelimiter = *dit;
        ++dit;
    }
    setup_parser(args, flags);
    perform_test(infos, defs, args, ok, pvals);
}

JJ_TEST_CLASS_END(cmdLinePositionalTests_t, basic, noAdditional, delimiters)

//================================================

JJ_TEST_CLASS_DERIVED(cmdLineVariableTests_t, public cmdLineOptionsCommon_t)

cmdLineVariableTests_t() : cmdLineOptionsCommon_t(static_cast<jj::test::testclass_base_t&>(*this)) {}

JJ_TEST_CASE_VARIANTS(basic, (const std::initializer_list<jj::string_t>& argv, const std::initializer_list<parserSetupWrap_t>& flags, bool ok, const std::map<jj::string_t, jj::string_t>& vars), \
    ({},{},true,{{jjT("var"),jjT("")},{jjT("Var"),jjT("1")},{jjT("VAR"),jjT(" ")}}),/*default values*/\
    ({jjT("var=val")},{},true,{{jjT("var"),jjT("val")},{jjT("Var"),jjT("1")},{jjT("VAR"),jjT(" ")}}),\
    ({jjT("Var=")},{},true,{{jjT("var"),jjT("")},{jjT("Var"),jjT("")},{jjT("VAR"),jjT(" ")}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_variable(defs, jjT("var"), jjT(""));
    setup_single_variable(defs, jjT("Var"), jjT("1"));
    setup_single_variable(defs, jjT("VAR"), jjT(" "));
    arguments_t args;
    setup_parser(args, flags);
    perform_test(infos, defs, args, ok, vars);
}

JJ_TEST_CASE_VARIANTS(duplicates, (const std::initializer_list<jj::string_t>& names, case_t varCase, bool ok),\
    ({jjT("")},case_t::SENSITIVE,false),\
    ({jjT("A"),jjT("a"),jjT("b")},case_t::SENSITIVE,true),\
    ({jjT("A"),jjT("a"),jjT("b")},case_t::INSENSITIVE,false),\
    ({jjT("A"),jjT("b")},case_t::INSENSITIVE,true),\
    ({jjT("blabla"),jjT("BLABLA")},case_t::INSENSITIVE,false),\
    ({jjT("blabla"),jjT("BLABLA")},case_t::SENSITIVE,true),\
    ({jjT("Bla-Bla"),jjT("BLA-BLA")},case_t::INSENSITIVE,false),\
    ({jjT("Bla-Bla"),jjT("BLA-BLA")},case_t::SENSITIVE,true))
{
    definitions_t defs;
    for (auto& x : names)
        setup_single_variable(defs, x, jjT(""));

    arguments_t args;
    args.VariableCase = varCase;

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

JJ_TEST_CASE_VARIANTS(caseinsensitive, (const std::initializer_list<jj::string_t>& argv, const std::initializer_list<parserSetupWrap_t>& flags, bool ok, const std::map<jj::string_t, jj::string_t>& vars), \
    ({jjT("var=10")},{},true,{{jjT("var"),jjT("10")}}),\
    ({jjT("Var=10")},{},true,{{jjT("var"),jjT("10")}}),\
    ({jjT("VAR=10")},{},true,{{jjT("var"),jjT("10")}}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_variable(defs, jjT("var"), jjT("5"));
    arguments_t args;
    args.VariableCase = case_t::INSENSITIVE;
    setup_parser(args, flags);
    perform_test(infos, defs, args, ok, vars);
}

JJ_TEST_CASE_VARIANTS(unknown, (const std::initializer_list<jj::string_t>& argv, const std::initializer_list<parserSetupWrap_t>& flags, bool ok, const std::map<jj::string_t, jj::string_t>& vars, const std::vector<jj::string_t>& pvals), \
    ({jjT("xxx=1")},{},false,{},{}),\
    ({jjT("xxx=1")},{unknownVariableBehavior_t::IS_POSITIONAL},true,{},{jjT("xxx=1")}),\
    ({jjT("xxx=1")},{unknownVariableBehavior_t::IS_VARIABLE},true,{{jjT("xxx"),jjT("1")}},{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    arguments_t args;
    setup_parser(args, flags);
    perform_test(infos, defs, args, ok, vars, pvals);
}

JJ_TEST_CASE_VARIANTS(unknown_sensitiveness, (const std::initializer_list<jj::string_t>& argv, const std::initializer_list<parserSetupWrap_t>& flags, case_t cs, bool ok, const std::map<jj::string_t, jj::string_t>& vars, const std::vector<jj::string_t>& pvals), \
    ({jjT("VAR=1")},{unknownVariableBehavior_t::IS_ERROR},case_t::SENSITIVE,false,{},{}),\
    ({jjT("VAR=1")},{unknownVariableBehavior_t::IS_ERROR},case_t::INSENSITIVE,true,{{jjT("var"),jjT("1")}},{}),\
    ({jjT("VAR=1")},{unknownVariableBehavior_t::IS_POSITIONAL},case_t::SENSITIVE,true,{},{jjT("VAR=1")}),\
    ({jjT("VAR=1")},{unknownVariableBehavior_t::IS_POSITIONAL},case_t::INSENSITIVE,true,{{jjT("var"),jjT("1")}},{}),\
    ({jjT("VAR=1")},{unknownVariableBehavior_t::IS_VARIABLE},case_t::SENSITIVE,true,{{jjT("var"),jjT("5")},{jjT("VAR"),jjT("1")}},{}),\
    ({jjT("VAR=1")},{unknownVariableBehavior_t::IS_VARIABLE},case_t::INSENSITIVE,true,{{jjT("var"),jjT("1")}},{}))
{
    optinfos_t infos(argv);
    definitions_t defs;
    setup_single_variable(defs, jjT("var"), jjT("5"));
    arguments_t args;
    args.VariableCase = cs;
    setup_parser(args, flags);
    perform_test(infos, defs, args, ok, vars, pvals);
}

JJ_TEST_CLASS_END(cmdLineVariableTests_t, basic, duplicates, caseinsensitive, unknown, unknown_sensitiveness)