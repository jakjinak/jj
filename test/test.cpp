#include "jj/test/test.h"
#include "jj/cmdLine.h"
#include <iostream>

namespace jj
{
namespace test
{

namespace AUX
{
defaultInitializer_t::defaultInitializer_t()
    : ArgumentDefinitions(new cmdLine::definitions_t), Arguments(new cmdLine::arguments_t)
{
}

void defaultInitializer_t::on_init(db_t& DB)
{
    using namespace jj::cmdLine;
    ArgumentDefinitions->Options.push_back({{name_t(jjT('t')), name_t(jjT("run")), name_t(jjT("run-tests"))}, jjT("Give any number of these to specify which tests shall run."), 1u, multiple_t::JOIN, nullptr});
    ArgumentDefinitions->Options.push_back({{name_t(jjT("class-names"))}, jjT("Prints information about entering/leaving testclass."), 0u, multiple_t::OVERRIDE, [&DB] (const optionDefinition_t&, values_t&) { DB.ClassNames = true; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("case-names"))}, jjT("Prints information about testcase being run."), 0u, multiple_t::OVERRIDE, [&DB] (const optionDefinition_t&, values_t&) { DB.CaseNames = jj::test::options_t::caseNames_t::ENTER; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("full-case-names"))}, jjT("Prints information about entering/leaving testcase."), 0u, multiple_t::OVERRIDE, [&DB] (const optionDefinition_t&, values_t&) { DB.CaseNames = jj::test::options_t::caseNames_t::ENTERLEAVE; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("in-color"))}, jjT("Prints output in colors."), 0u, multiple_t::OVERRIDE, [&DB] (const optionDefinition_t&, values_t&) { DB.Colors = true; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("results"))}, jjT("Based on provided value prints results of individual tests within testcases; none means no results shown, fails shows only failed tests, all shows failed and passed conditions."), 1u, multiple_t::OVERRIDE,
        [&DB](const optionDefinition_t&, values_t& v) {
            if (v.Values.size()==0) throw std::runtime_error("Invalid number of arg values.");
            jj::string_t& s = v.Values.front();
            if (s==jjT("none")) DB.Tests = jj::test::options_t::testResults_t::NONE;
            else if (s==jjT("fails")) DB.Tests = jj::test::options_t::testResults_t::FAILS;
            else if (s==jjT("all")) DB.Tests = jj::test::options_t::testResults_t::ALL;
            else throw std::runtime_error("Value in --results can be one of none|fails|all.");
            return true; } });
}

void defaultInitializer_t::on_setup(int argc, const char_t** argv)
{
    try
    {
        Arguments->parse(*ArgumentDefinitions, argc, argv);
    }
    catch (const std::exception& ex)
    {
        jj:cout << jjT("Exception caught: ") << jj::strcvt::to_string(ex.what()) << jjT("\n");
        // TODO print help
        exit(1);
    }
    catch (...)
    {
        jj::cout << jjT("Unknown exception caught!\n");
        exit(2);
    }
}

void defaultOutput_t::enter_class(const string_t& name, const string_t& variant)
{
    if (!opt_.ClassNames)
        return;
    if (opt_.Colors)
        jj::cout << jjT("\033[36m");
    jj::cout << jjT("class ");
    if (opt_.Colors)
        jj::cout << jjT("\033[1m");
    jj::cout << name << variant;
    if (opt_.Colors)
        jj::cout << jjT("\033[22m");
    jj::cout << jjT(" | entering");
    if (opt_.Colors)
        jj::cout << jjT("\033[0m");
    jj::cout << jjT('\n');
}

void defaultOutput_t::leave_class(const string_t& name, const string_t& variant)
{
    if (!opt_.ClassNames)
        return;
    if (opt_.Colors)
        jj::cout << jjT("\033[36m");
    jj::cout << jjT("class ");
    if (opt_.Colors)
        jj::cout << jjT("\033[1m");
    jj::cout << name << variant;
    if (opt_.Colors)
        jj::cout << jjT("\033[22m");
    jj::cout << jjT(" | leaving");
    if (opt_.Colors)
        jj::cout << jjT("\033[0m");
    jj::cout << jjT('\n');
}

void defaultOutput_t::enter_case(const string_t& name, const string_t& variant)
{
    if (opt_.CaseNames == options_t::caseNames_t::OFF)
        return;
    if (opt_.Colors)
        jj::cout << jjT("\033[34;1m");
    jj::cout << name << variant;
    if (opt_.Colors)
        jj::cout << jjT("\033[22m");
    if (opt_.CaseNames == options_t::caseNames_t::ENTERLEAVE)
        jj::cout << jjT(" | entering");
    if (opt_.Colors)
        jj::cout << jjT("\033[0m");
    jj::cout << jjT('\n');
}

void defaultOutput_t::leave_case(const string_t& name, const string_t& variant)
{
    if (opt_.CaseNames != options_t::caseNames_t::ENTERLEAVE)
        return;
    if (opt_.Colors)
        jj::cout << jjT("\033[34;1m");
    jj::cout << name << variant;
    if (opt_.Colors)
        jj::cout << jjT("\033[22m");
    jj::cout << jjT(" | leaving");
    if (opt_.Colors)
        jj::cout << jjT("\033[0m");
    jj::cout << jjT('\n');
}

void defaultOutput_t::test_result(test_result_t result, const string_t& text)
{
    const char_t* color = nullptr, *rtxt;
    switch (result)
    {
    case FAILINFO:
        if (opt_.Colors)
            jj::cout << jjT("\033[31;1m");
        std::cout << text;
        if (opt_.Colors)
            jj::cout << jjT("\033[0m");
        jj::cout << jjT('\n');
        return;
    case PASSED:
        color = jjT("\033[32m");
        rtxt = jjT("passed");
        break;
    case WARNING:
        color = jjT("\033[33m");
        rtxt = jjT("failed (considered warning)");
        break;
    case FAILED:
        color = jjT("\033[31m");
        rtxt = jjT("failed");
        break;
    default:
        return;
    }
    if (opt_.Colors)
        jj::cout << color;
    jj::cout << jjT("test '");
    if (opt_.Colors)
        jj::cout << jjT("\033[1m");
    jj::cout << text;
    if (opt_.Colors)
        jj::cout << jjT("\033[22m");
    jj::cout << jjT("' ") << rtxt;
    if (opt_.Colors)
        jj::cout << jjT("\033[0m");
    jj::cout << jjT('\n');
}

void holder_base_t::print(const string_t& name)
{
    jj::cout << jjT('\t') << name << jjT('\n');
}
void holder_base_t::print(const string_t& name, const string_t& variant)
{
    jj::cout << jjT('\t') << name << variant << jjT('\n');
}
} // namespace AUX

void db_t::do_list(const testclasses_t::value_type& testclass, bool classvariants, bool tests, bool variants) const
{
    if (testclass.second.first.size() == 1 || !classvariants)
        jj::cout << testclass.first << jjT('\n');
    else for (const testclass_variants_t::value_type& v : testclass.second.first)
        jj::cout << testclass.first << v.first << jjT('\n');
    if (!tests)
        return;
    AUX::holder_base_t* h = testclass.second.second;
    h->list(variants);
}

void db_t::list_testclasses(bool classvariants) const
{
    for (const testclasses_t::value_type& i : testclasses_)
        do_list(i, classvariants, false);
}

void db_t::list_testcases(bool classvariants, bool testvariants) const
{
    for (const testclasses_t::value_type& i : testclasses_)
        do_list(i, classvariants, true, testvariants);
}

void db_t::list_testcases(const string_t& testclass, bool classvariants, bool testvariants) const
{
    testclasses_t::const_iterator fnd = testclasses_.find(testclass);
    if (fnd == testclasses_.end())
        return;
    do_list(*fnd, classvariants, true, testvariants);
}

void db_t::run_testcase(std::function<void()> tc)
{
    try
    {
        tc();
    }
    catch (const jj::test::testingFailed_t& ex)
    {
        throw; // propagate to main()
    }
    catch (const jj::test::testFailed_t& ex)
    {
        test_result(output_t::FAILINFO, jjS(jjT("The previous failure was considered crutial for the test case. Skipping to the end of test case.")));
    }
    catch (const std::exception& ex)
    {
        test_result(output_t::FAILINFO, jjS(jjT("Exception caught: ") << jj::strcvt::to_string_t(ex.what())));
    }
    catch (const std::string& ex)
    {
        test_result(output_t::FAILINFO, jjS(jjT("Exception caught: ") << jj::strcvt::to_string_t(ex)));
    }
    catch (...)
    {
        test_result(output_t::FAILINFO, jjT("Unknown exception caught!"));
    }
}

void db_t::run()
{
    for (testclasses_t::value_type& i : testclasses_)
    {
        for (testclass_variants_t::value_type& v : i.second.first)
        {
            enter_class(i.first, v.first);
            (v.second)();
            leave_class(i.first, v.first);
        }
    }
}

} // namespace test
} // namespace jj

#if defined(_WINDOWS) || defined(_WIN32)
int wmain(int argc, const wchar_t** argv)
#else
int main(int argc, const char** argv)
#endif
{
    jj::test::db_t& DB = jj::test::db_t::instance();
    try
    {
        for (auto i : DB.Initializers)
            i->on_init(DB);
        for (auto i : DB.Initializers)
            i->on_setup(argc, argv);
        DB.run();
    }
    catch (const jj::test::testingFailed_t& ex)
    {
        DB.test_result(jj::test::output_t::FAILINFO, jjS(jjT("The previous failure was considered crutial for the test suite. Skipping to the end of test suite.")));
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cout << "Exception caught: " << ex.what() << "\n";
        return 2;
    }
    catch (const std::string& ex)
    {
        std::cout << "Exception caught: " << ex << "\n";
        return 2;
    }
    catch (...)
    {
        std::cout << "Unknown exception caught!\n";
        return 2;
    }
}
