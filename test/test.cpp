#include "jj/test/test.h"
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
            (v.second)();
    }
}

} // namespace test
} // namespace jj
