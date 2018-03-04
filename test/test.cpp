#include "jj/test/test.h"
#include "jj/cmdLine.h"
#include <iostream>
#include <deque>
#if defined(_WINDOWS) || defined(_WIN32)
#include <tchar.h>
#endif

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
    Arguments->add_default_help(*ArgumentDefinitions);
    ArgumentDefinitions->Options.push_back({{name_t(jjT('l')), name_t(jjT("list"))}, jjT("Lists all testcases known in the program and exits."), 0u, multiple_t::JOIN,
        [&DB] (const optionDefinition_t&, values_t&) { DB.Mode = db_t::LIST; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT('L')), name_t(jjT("list-classes"))}, jjT("Lists all testclasses known in the program and exits."), 0u, multiple_t::JOIN,
        [&DB] (const optionDefinition_t&, values_t&) { DB.Mode = db_t::LIST_CLASSES; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("class"))}, jjT("With --list specifies that only the cases in given class should be listed."), 1u, multiple_t::JOIN,
        [&DB] (const optionDefinition_t&, values_t& v) { DB.ListClass = v.Values.front(); return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT('V')), name_t(jjT("class-variants"))}, jjT("In list mode prints all test class variants."), 0u, multiple_t::JOIN,
        [&DB] (const optionDefinition_t&, values_t&) { DB.ListClassVariants = true; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT('v')), name_t(jjT("case-variants"))}, jjT("In list mode prints all test case variants."), 0u, multiple_t::JOIN,
        [&DB] (const optionDefinition_t&, values_t&) { DB.ListCaseVariants = true; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT('r')), name_t(jjT("run")), name_t(jjT('+'), jjT('t'))}, jjT("Give any number of these to specify which tests shall run. See"), 1u, multiple_t::JOIN,
        [&DB] (const optionDefinition_t&, values_t& v) { DB.Filters.push_back(filter_t(filter_t::ADD, v.Values.front())); return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT('s')), name_t(jjT("skip")), name_t(jjT('t'))}, jjT("Give any number of these to specify which tests shall be skipped."), 1u, multiple_t::JOIN,
        [&DB] (const optionDefinition_t&, values_t& v) { DB.Filters.push_back(filter_t(filter_t::REMOVE, v.Values.front())); return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("c")), name_t(jjT("class-names"))}, jjT("Prints information about entering/leaving testclass."), 0u, multiple_t::OVERRIDE,
        [&DB] (const optionDefinition_t&, values_t&) { DB.ClassNames = true; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("n")), name_t(jjT("case-names"))}, jjT("Prints information about testcase being run."), 0u, multiple_t::OVERRIDE,
        [&DB] (const optionDefinition_t&, values_t&) { DB.CaseNames = jj::test::options_t::caseNames_t::ENTER; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("N")), name_t(jjT("full-case-names"))}, jjT("Prints information about entering/leaving testcase."), 0u, multiple_t::OVERRIDE,
        [&DB] (const optionDefinition_t&, values_t&) { DB.CaseNames = jj::test::options_t::caseNames_t::ENTERLEAVE; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("C")), name_t(jjT("in-color"))}, jjT("Prints output in colors."), 0u, multiple_t::OVERRIDE,
        [&DB] (const optionDefinition_t&, values_t&) { DB.Colors = true; return true; } });
    ArgumentDefinitions->Options.push_back({{name_t(jjT("results"))}, jjT("Based on provided value prints results of individual tests within testcases; none means no results shown, fails shows only failed tests, all shows failed and passed conditions."), 1u, multiple_t::OVERRIDE,
        [&DB](const optionDefinition_t&, values_t& v) {
            if (v.Values.size()==0) throw std::runtime_error("Invalid number of arg values.");
            jj::string_t& s = v.Values.front();
            if (s==jjT("none")) DB.Tests = jj::test::options_t::testResults_t::NONE;
            else if (s==jjT("fails")) DB.Tests = jj::test::options_t::testResults_t::FAILS;
            else if (s==jjT("all")) DB.Tests = jj::test::options_t::testResults_t::ALL;
            else throw std::runtime_error("Value in --results can be one of none|fails|all.");
            return true; } });
    ArgumentDefinitions->Options.push_back({ {name_t(jjT("S")),name_t(jjT("summary"))}, jjT("Determines if and how the summary of tests is printed at the end of the program; none means not printed at all, default show the default (passed/failed/total) in rows, short prints only passed/failed numbers in last row."), 1u, multiple_t::OVERRIDE,
        [&DB](const optionDefinition_t&, values_t& v) {
            if (v.Values.size() == 0) throw std::runtime_error("Invalid number of arg values.");
            jj::string_t& s = v.Values.front();
            if (s == jjT("none")) DB.FinalStatistics = jj::test::options_t::finalStatistics_t::NONE;
            else if (s == jjT("default")) DB.FinalStatistics = jj::test::options_t::finalStatistics_t::DEFAULT;
            else if (s == jjT("short")) DB.FinalStatistics = jj::test::options_t::finalStatistics_t::SHORT;
            else throw std::runtime_error("Value in --summary can be one of none|default|short.");
            return true; } });
    ArgumentDefinitions->Sections.push_back({
        jjT("SELECTING TESTS"),
        jjT("If no --run/--skip arguments are given then all tests are run.\n")
        jjT("But any number of --run/--skip can be given - they will be processed sequentially to identify the matching testcases.\n")\
        jjT("If --run is the first provided then initially no test cases match and the provided value selects the ones to run.\n")
        jjT("If --skip is the first one then initially all tests match except those matching the provided value.\n")
        jjT("Further --run/--skip add/remove matching testcases to/from the list to be run.\n")
        jjT("\n")
        jjT("The value of --run/--skip (or their equivalents) is one of:\n")
        jjT("  1) classname(variant)/casename(variant)\n")
        jjT("  2) classname/casename(variant)\n")
        jjT("  3) classname(variant)/casename\n")
        jjT("  4) classname/casename\n")
        jjT("  5) casename(variant)\n")
        jjT("  6) casename\n")
        jjT("  7) classname(variant)/\n")
        jjT("  8) classname/\n")
        jjT("where classname stands for the name of test class, casename for the test case name and variant for the exact string as printed in --list.\n")
        jjT("If classname or casename given without variant then all variants are matching.\n")
        jjT("If no class name is given then 2 situations can occur depending whether there already were --run arguments specifying a classname or not.\n")
        jjT(" - if yes then only the test cases in these classes match\n")
        jjT(" - if no then all classes are evaluated for matching test cases\n")
        jjT("That effectively means that unless you specify at least one --run argument with classname then all classes are evaluated, first such argument changes the behavior.\n")
        jjT("If only class name is given then all test cases in that class match.\n")
        jjT("Whitespace around the / character as well as whitespace at the beginning/end is ignored.\n")
        jjT("Whitespace inside the (variant) is used \"as is\".\n")});
}

void defaultInitializer_t::on_setup(int argc, const char_t** argv)
{
    try
    {
        Arguments->parse(*ArgumentDefinitions, argc, argv);
    }
    catch (const std::exception& ex)
    {
        jj::cout << jjT("Exception caught: ") << jj::strcvt::to_string_t(ex.what()) << jjT("\n");
        Arguments->print_default_help();
        exit(1);
    }
    catch (...)
    {
        jj::cout << jjT("Unknown exception caught!\n");
        exit(2);
    }
}

void defaultOutput_t::list_class(const string_t& name)
{
    jj::cout << name << jjT('\n');
}

void defaultOutput_t::list_class(const string_t& name, const string_t& variant)
{
    jj::cout << name << variant << jjT('\n');
}

void defaultOutput_t::list_case(const string_t& name)
{
    jj::cout << jjT('\t') << name << jjT('\n');
}

void defaultOutput_t::list_case(const string_t& name, const string_t& variant)
{
    jj::cout << jjT('\t') << name << variant << jjT('\n');
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

void defaultOutput_t::leave_class(const string_t& name, const string_t& variant, const statistics_t& stats)
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
        jj::cout << text;
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

void defaultOutput_t::statistics(const statistics_t& stats)
{
    if (opt_.FinalStatistics == jj::test::options_t::finalStatistics_t::NONE)
        return;
    if (opt_.Colors)
        jj::cout << jjT("\033[1m");
    if (opt_.FinalStatistics == jj::test::options_t::finalStatistics_t::DEFAULT)
    {
        jj::cout << jjT("SUMMARY\n");
        jj::cout << jjT("Successful tests:   ") << stats.Passed << jjT("\n");
        jj::cout << jjT("Failed tests:       ") << stats.Failed << jjT("\n");
        jj::cout << jjT("Tests run in total: ") << stats.total() << jjT("\n");
    }
    else if (opt_.FinalStatistics == jj::test::options_t::finalStatistics_t::SHORT)
    {
        jj::cout << stats.Passed << jjT('/') << stats.Failed;
    }
    if (opt_.Colors)
        jj::cout << jjT("\033[0m");
}

namespace // <anonymous>
{
static bool skip_space(const string_t& s, size_t& pos)
{
    while (pos < s.length() && jj::str::isspace(s[pos]))
        ++pos;
    return pos < s.length();
}
static bool skip_till(const string_t& s, size_t& pos, char_t c1)
{
    while (pos < s.length() && s[pos] != c1)
        ++pos;
    return pos < s.length();
}
static bool skip_till(const string_t& s, size_t& pos, char_t c1, char_t c2)
{
    while (pos < s.length() && s[pos] != c1 && s[pos] != c2)
        ++pos;
    return pos < s.length();
}
static bool skip_bracket(const string_t& s, size_t& pos)
{
    if (pos >= s.length())
        return false;
    if (s[pos]!=jjT('('))
        throw std::runtime_error("Internal error.");

    struct item_t
    {
        char_t ch;
        bool esc;
        item_t(char_t c, bool e) : ch(c), esc(e) {}
    };
    typedef std::deque<item_t> stack_t;
    stack_t st;
    bool esc = false;
    size_t len = s.length();

    // we are at a '(' as checked above, initalize stack
    st.push_back(item_t(jjT(')'), false));
    ++pos;

    for (; pos<len; ++pos)
    {
        if (esc)
        {
            // while waiting for pair character encountered a backslash, skipping the next char
            esc = false;
            continue;
        }
        // waiting for some pair character () or ""
        item_t& t = st.back();
        if (t.esc)
        {
            // within string
            if (s[pos] == jjT('\\'))
            {
                // encountered a backslash, skip this and next char
                esc = true;
                continue;
            }
            if (s[pos] == t.ch)
            {
                st.pop_back();
                if (st.empty())
                {
                    ++pos;
                    return pos < len;
                }
                continue;
            }
            continue; // within something escapable no more stack descent
        }
        if (s[pos] == t.ch)
        {
            st.pop_back();
            if (st.empty())
            {
                ++pos;
                return pos < len;
            }
            continue;
        }
        else switch (s[pos]) // next stack level (descending into next bracket or string or char
        {
        case jjT('('):
            st.push_back(item_t(jjT(')'), false));
            break;
        case jjT('"'):
            st.push_back(item_t(jjT('"'), true));
            break;
        case jjT('\''):
            st.push_back(item_t(jjT('\''), true));
            break;
        }
    }
    if (!st.empty())
        throw std::runtime_error("Invalid filter specification. Unterminated variant?");
    return pos < len;
}
static void construct(const string_t& s, size_t start, size_t end, string_t& os)
{
    if (end<=start)
        throw std::runtime_error("Empty string to be constructed.");
    if (end > s.length())
        throw std::runtime_error("Invalid string index.");
    while (end>start && jj::str::isspace(s[end-1]))
        --end;
    if (end == start)
        throw std::runtime_error("Empty string to be constructed.");
    os.assign(s, start, end-start);
}
} // namespace <anonymous>

filter_t::filter_t(filterType_t type, const string_t& filter)
    : Type(type)
{
    size_t i = 0;
    if (!skip_space(filter, i))
        return; // empty filter
    size_t s1 = i, e1 = 0, vs1 =0 , ve1 = 0;
    if (!skip_till(filter, i, jjT('('), jjT('/')))
    {
        // case name only was specified
        construct(filter, s1, i, Case);
        return;
    }
    if (filter[i] == jjT('('))
    {
        e1 = i;
        vs1 = i;
        skip_bracket(filter, i);
        ve1 = i;
        if (!skip_space(filter, i))
        {
            // it's a case name with variant
            construct(filter, s1, e1, Case);
            construct(filter, vs1, ve1, CaseVariant);
            return;
        }
    }
    else
        e1 = i;

    if (filter[i] != jjT('/'))
        throw std::runtime_error("Unexpected character where '/' was expected.");
    ++i; // skip the '/'
    // whatever we received before was a class specification
    construct(filter, s1, e1, Class);
    if (vs1 > s1) // check if there was a variant spec as well
        construct(filter, vs1, ve1, ClassVariant);

    if (!skip_space(filter, i))
        return; // after all it was a class spec only

    size_t s2 = i;
    if (!skip_till(filter, i, jjT('('), jjT('/')))
    {
        // case spec only
        construct(filter, s2, i, Case);
        return;
    }
    if (filter[i] == jjT('/'))
        throw std::runtime_error("Unexpected '/', one was already seen.");
    size_t vs2 = i;
    bool cont = skip_bracket(filter, i);
    // case spec with variant
    construct(filter, s2, vs2, Case);
    construct(filter, vs2, i, CaseVariant);
    if (!cont)
        return;
    if (!skip_space(filter, i))
        return;
    throw std::runtime_error("Unexpected characters after testcase variant specification.");
}


} // namespace AUX

void db_t::do_list(const testclasses_t::value_type& testclass, bool classvariants, bool tests, bool variants)
{
    if (!classvariants)
        list_class(testclass.first);
    else for (const testclass_variants_t::value_type& v : testclass.second.first)
        list_class(testclass.first, v.first);
    if (!tests)
        return;
    AUX::holder_base_t* h = testclass.second.second;
    h->list(variants);
}

void db_t::list_testclasses(bool classvariants)
{
    for (const testclasses_t::value_type& i : testclasses_)
        do_list(i, classvariants, false);
}

void db_t::list_testcases(bool classvariants, bool testvariants)
{
    for (const testclasses_t::value_type& i : testclasses_)
        do_list(i, classvariants, true, testvariants);
}

void db_t::list_testcases(const string_t& testclass, bool classvariants, bool testvariants)
{
    testclasses_t::const_iterator fnd = testclasses_.find(testclass);
    if (fnd == testclasses_.end())
        return;
    do_list(*fnd, classvariants, true, testvariants);
}

bool db_t::check_class_filters(const string_t& c, const string_t& v, bool& startWith, AUX::filter_refs_t& filters)
{
    filters.clear();
    if (Filters.empty())
    {
        startWith = true; // understand run everything
        return true;
    }
    startWith = Filters.front().Type != AUX::filter_t::ADD;
    bool cur = startWith;
    bool exact = false;
    AUX::filters_t::const_iterator it = Filters.begin(), e = Filters.end();
    for (; it!=e; ++it)
    {
        if (it->Type == AUX::filter_t::ADD && !it->Class.empty())
            exact = true;

        if (it->Class.empty())
        {
            filters.push_back(AUX::filter_ref_t(*it)); // empty matches every class
            if (!exact)
                cur = it->Type == AUX::filter_t::ADD;
        }
        else if (it->Class == c)
        {
            bool varmatch = false;
            if (v.empty())
                varmatch = it->ClassVariant.empty();
            else if (it->ClassVariant.empty())
                varmatch = true;
            else
                varmatch = it->ClassVariant == v;
            if (varmatch)
            {
                if (!it->Case.empty())
                    filters.push_back(AUX::filter_ref_t(*it));
                if (it->Type == AUX::filter_t::ADD)
                    cur = true;
                else if (it->Case.empty())
                    cur = false;
            }
        }
    }
    if (!exact) // no class names were given so there was no exact match for class so "cur" is not set properly
        return filters.size() > 0;
    return cur;
}

bool db_t::check_case_filters(const string_t& c, const string_t& v, const AUX::filter_refs_t& filters)
{
    if (filters.size() == 0)
        return true;
    bool cur = filters.front()->Type != AUX::filter_t::ADD;
    for (AUX::filter_refs_t::const_iterator it = filters.begin(); it!=filters.end(); ++it)
    {
        if ((*it)->Case.empty())
            cur = (*it)->Type == AUX::filter_t::ADD; // everything matches empty value
        else if ((*it)->Case == c)
        {
            if ((*it)->CaseVariant.empty() || (*it)->CaseVariant == v)
                cur = (*it)->Type == AUX::filter_t::ADD;
        }
    }
    return cur;
}

void db_t::run_testcase(std::function<void(statistics_t&)> tc, statistics_t& stats)
{
    try
    {
        statistics_t s;
        tc(s);
        if (s.Failed > 0)
            ++stats.Failed;
        else
            ++stats.Passed;
    }
    catch (const jj::test::testingFailed_t&)
    {
        Statistics += stats;
        throw; // propagate to main()
    }
    catch (const jj::test::testFailed_t& ex)
    {
        ++stats.Failed;
        test_result(output_t::FAILINFO, jjS(jjT("The previous failure was considered crutial for the test case. Skipping to the end of test case. Exception caught : ") << jj::strcvt::to_string_t(ex.what())));
    }
    catch (const std::exception& ex)
    {
        ++stats.Failed;
        test_result(output_t::FAILINFO, jjS(jjT("Exception caught: ") << jj::strcvt::to_string_t(ex.what())));
    }
    catch (const std::string& ex)
    {
        ++stats.Failed;
        test_result(output_t::FAILINFO, jjS(jjT("Exception caught: ") << jj::strcvt::to_string_t(ex)));
    }
    catch (...)
    {
        ++stats.Failed;
        test_result(output_t::FAILINFO, jjT("Unknown exception caught!"));
    }
}

bool db_t::run()
{
    Statistics.reset();
    for (testclasses_t::value_type& i : testclasses_)
    {
        for (testclass_variants_t::value_type& v : i.second.first)
        {
            bool start = true;
            AUX::filter_refs_t refs;
            if (!check_class_filters(i.first, v.first, start, refs))
                continue;

            statistics_t stats;
            enter_class(i.first, v.first);
            (v.second)(stats, refs);
            leave_class(i.first, v.first, stats);
            Statistics += stats;
        }
    }
    statistics(Statistics);
    return Statistics.Failed == 0;
}

} // namespace test
} // namespace jj

int main(int argc, const char** argv)
{
#if defined(_WINDOWS) || defined(_WIN32)
    typedef std::list<std::wstring> args_t;
    args_t args;
    const wchar_t** argx = new const wchar_t*[argc];
    std::unique_ptr<const wchar_t*[]> argp(argx);
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(jj::strcvt::to_string_t(argv[i]));
        argx[i] = args.back().c_str();
    }
#else
    const jj::char_t** argx = argv;
#endif
    jj::test::db_t& DB = jj::test::db_t::instance();
    try
    {
        for (auto i : DB.Initializers)
            i->on_init(DB);
        for (auto i : DB.Initializers)
            i->on_setup(argc, argx);

        if (DB.Mode == jj::test::db_t::LIST)
        {
            if (DB.ListClass.empty())
                DB.list_testcases(DB.ListClassVariants, DB.ListCaseVariants);
            else
                DB.list_testcases(DB.ListClass, DB.ListClassVariants, DB.ListCaseVariants);
        }
        else if (DB.Mode == jj::test::db_t::LIST_CLASSES)
            DB.list_testclasses(DB.ListClassVariants);
        else if (DB.Mode == jj::test::db_t::RUN)
            return DB.run() ? 0 : 1;
    }
    catch (const jj::test::testingFailed_t& ex)
    {
        DB.test_result(jj::test::output_t::FAILINFO, jjS(jjT("The previous failure was considered crutial for the test suite. Skipping to the end of test suite.")));
        std::cout << "Exception caught: " << ex.what() << "\n";
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
