#include "jj/test/test.h"
#include "jj/cmdLine.h"
#include <iostream>

namespace jj
{
namespace test
{

namespace AUX
{
void defaultInitializer_t::on_init(int argc, const char_t** argv)
{
    jj::test::db_t& DB = jj::test::db_t::instance();
    using namespace jj::cmdLine;
    definitions_t argdefs;
    argdefs.Options.push_back({{name_t(jjT('t')), name_t(jjT("run")), name_t(jjT("run-tests"))}, jjT("Give any number of these to specify which tests shall run."), 1u, multiple_t::JOIN, nullptr});
    argdefs.Options.push_back({{name_t(jjT("class-names"))}, jjT("Prints information about entering/leaving testclass."), 0u, multiple_t::OVERRIDE, [&DB] (const optionDefinition_t&, values_t&) { DB.ClassNames = true; return true; } });
    argdefs.Options.push_back({{name_t(jjT("case-names"))}, jjT("Prints information about testcase being run."), 0u, multiple_t::OVERRIDE, [&DB] (const optionDefinition_t&, values_t&) { DB.CaseNames = jj::test::options_t::caseNames_t::ENTER; return true; } });
    argdefs.Options.push_back({{name_t(jjT("full-case-names"))}, jjT("Prints information about entering/leaving testcase."), 0u, multiple_t::OVERRIDE, [&DB] (const optionDefinition_t&, values_t&) { DB.CaseNames = jj::test::options_t::caseNames_t::ENTERLEAVE; return true; } });
    argdefs.Options.push_back({{name_t(jjT("in-color"))}, jjT("Prints output in colors."), 0u, multiple_t::OVERRIDE, [&DB] (const optionDefinition_t&, values_t&) { DB.Colors = true; return true; } });
    argdefs.Options.push_back({{name_t(jjT("results"))}, jjT("Based on provided value prints results of individual tests within testcases; none means no results shown, fails shows only failed tests, all shows failed and passed conditions."), 1u, multiple_t::OVERRIDE,
        [&DB](const optionDefinition_t&, values_t& v) {
            if (v.Values.size()==0) throw std::runtime_error("Invalid number of arg values.");
            jj::string_t& s = v.Values.front();
            if (s==jjT("none")) DB.Tests = jj::test::options_t::testResults_t::NONE;
            else if (s==jjT("fails")) DB.Tests = jj::test::options_t::testResults_t::FAILS;
            else if (s==jjT("all")) DB.Tests = jj::test::options_t::testResults_t::ALL;
            else throw std::runtime_error("Value in --results can be one of none|fails|all.");
            return true; } });

    arguments_t args;
    try
    {
        args.parse(argdefs, argc, argv);
    }
    catch (const std::exception& ex)
    {
        std::cout << "Exception caught: " << ex.what() << "\n";
        // TODO print help
        exit(1);
    }
    catch (...)
    {
        std::cout << "Unknown exception caught!\n";
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

void defaultOutput_t::test_ok(const string_t& text)
{
    if (opt_.Colors)
        jj::cout << jjT("\033[32m");
    jj::cout << jjT("test '");
    if (opt_.Colors)
        jj::cout << jjT("\033[1m");
    jj::cout << text;
    if (opt_.Colors)
        jj::cout << jjT("\033[22m");
    jj::cout << jjT("' passed");
    if (opt_.Colors)
        jj::cout << jjT("\033[0m");
    jj::cout << jjT('\n');
}

void defaultOutput_t::test_fail(const string_t& text)
{
    if (opt_.Colors)
        jj::cout << jjT("\033[31m");
    jj::cout << jjT("test '");
    if (opt_.Colors)
        jj::cout << jjT("\033[1m");
    jj::cout << text;
    if (opt_.Colors)
        jj::cout << jjT("\033[22m");
    jj::cout << jjT("' failed");
    if (opt_.Colors)
        jj::cout << jjT("\033[0m");
    jj::cout << jjT('\n');
}

void holder_base_t::print(const string_t& name)
{
    std::cout << '\t' << name << '\n';
}
void holder_base_t::print(const string_t& name, const string_t& variant)
{
    std::cout << '\t' << name << variant << '\n';
}
} // namespace AUX

void db_t::do_list(const testclasses_t::value_type& testclass, bool classvariants, bool tests, bool variants) const
{
    if (testclass.second.first.size() == 1 || !classvariants)
        std::cout << testclass.first << jjT('\n');
    else for (const testclass_variants_t::value_type& v : testclass.second.first)
        std::cout << testclass.first << v.first << jjT('\n');
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
    try
    {
        jj::test::db_t& DB = jj::test::db_t::instance();
        for (auto i : DB.Initializers)
        {
            i->on_init(argc, argv);
        }
        DB.run();
    }
    catch (const jj::test::testingFailed_t& ex)
    {
        // TODO provide extra message (callback to output_t) about fatal failure
        return 1;
    }
    catch (const jj::test::testFailed_t& ex)
    {
        // TODO move to single test execution
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
