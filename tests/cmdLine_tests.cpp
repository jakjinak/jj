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

JJ_TEST_CASE_VARIANTS(basic, (size_t mandatory, size_t optional, const std::initializer_list<jj::string_t>& argv, const std::initializer_list<flags_t>& flags, bool ok, const std::vector<jj::string_t>& pvals), \
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

JJ_TEST_CASE_VARIANTS(noAdditional, (size_t mandatory, size_t optional, const std::initializer_list<jj::string_t>& argv, const std::initializer_list<flags_t>& flags, bool ok, const std::vector<jj::string_t>& pvals), \
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

JJ_TEST_CASE_VARIANTS(delimiters, (size_t mandatory, const std::initializer_list<jj::string_t>& delimiters, const std::initializer_list<jj::string_t>& argv, const std::initializer_list<flags_t>& flags, bool ok, const std::vector<jj::string_t>& pvals), \
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
