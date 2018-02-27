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
