#include "jj/test/test.h"
#include "jj/cmdLine.h"
#include <iostream>

namespace jj
{
namespace test
{

namespace AUX
{
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
            std::cout << "class " << i.first << v.first << " | entering\n";
            (v.second)();
            std::cout << "class " << i.first << v.first << " | leaving\n";
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
    using namespace jj::cmdLine;
    definitions_t argdefs;
    argdefs.Options.push_back({{name_t(jjT('t')), name_t(jjT("run")), name_t(jjT("run-tests"))}, jjT("Give any number of these to specify which tests shall run."), 1u, multiple_t::JOIN, nullptr});
    argdefs.Options.push_back({{name_t(jjT("class-names"))}, jjT("Prints information about entering/leaving testclass."), 0u, multiple_t::OVERRIDE, nullptr});
    argdefs.Options.push_back({{name_t(jjT("case-names"))}, jjT("Prints information about entering/leaving testcase."), 0u, multiple_t::OVERRIDE, nullptr});
    arguments_t args;
    try
    {
        args.parse(argdefs, argc, argv);
        DB.run();
    }
    catch (const jj::test::testFailed_t& ex)
    {
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
