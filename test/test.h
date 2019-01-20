#ifndef JJ_TEST_H
#define JJ_TEST_H

#include "jj/string.h"
#include "jj/stream.h"
#include "jj/exception.h"
#include "jj/macros.h"
#include <map>
#include <list>
#include <memory>
#include <sstream>
#include <functional>

namespace jj
{

namespace cmdLine
{
struct definitions_t;
struct arguments_t;
} // namespace cmdLine

namespace test
{
class db_t;

/*! A simple wrapper over logic error.
Thrown if a ENSURE check fails. Makes framework to skip to the next testcase. */
struct testFailed_t : public std::logic_error
{
    testFailed_t() : std::logic_error("Test failed!") {}
};

/*! A simple wrapper over logic error.
Thrown if a MUSTBE check fails. Causes the whole program to finish immediatelly. */
struct testingFailed_t : public std::logic_error
{
    testingFailed_t() : std::logic_error("Testing failed!") {}
};

/*! Counts of testcases. */
struct statistics_t
{
    size_t Passed, //!< number of passed testcases
        Failed; //!< number of failed testcases

    statistics_t() : Passed(0), Failed(0) {}
    /*! total number of testcases */
    size_t total() const { return Passed + Failed; }
    /*! resets all counts to 0. */
    void reset() { Passed = Failed = 0; }
    /*! adds counts from other instance to this instance. */
    statistics_t& operator+=(const statistics_t& other) { if (&other == this) return *this; Passed+=other.Passed; Failed+=other.Failed; return *this; }
};

/*! Contains all members important for test classes. */
class testclass_base_t
{
public:
    statistics_t JJ_TEST_CASE_Statistics; //!< the per-class-variant statistics
};

/*! Encapsulates all the possible options available for the testing framework. 

Note that the outputs (and related settings) are directly in the db_t so that these options can apply to them as well independently. */
class options_t
{
public:
    /*! What is the desired format of presenting testcases being run. */
    enum class caseNames_t
    {
        OFF, //!< no information desired
        ENTER, //!< name of test being run; implied by the --case-names argument
        ENTERLEAVE //!< showing when test started and ended; implied by the --full-case-names
    };
    /*! How are results of TEST/ENSURE/MUSTBE conditions presented. */
    enum class testResults_t
    {
        NONE, //!< no information is shown even if tests fail
        FAILS, //!< only failed conditions are shown
        ALL //!< all results are shown
    };
    /*! If and how final statistics are printed. */
    enum class finalStatistics_t
    {
        NONE, //!< nothing printed (only result code will determine if any test failed)
        DEFAULT, //!< usual format with passed/failed/total in individual rows; this is the default if the -s option is not given
        SHORT //!< only passed/failed is printed as last row
    };

    bool ClassNames; //!< whether test classes should be shown; implied by the --class-names argument
    caseNames_t CaseNames; //!< whether test cases should be shown
    bool Colors; //!< whether output shall be in colors; implied by the --in-color argument
    testResults_t Tests; //!< how test condition results are presented; set using the --results=(none|fails|all) argument
    finalStatistics_t FinalStatistics; //!< if and how final statistics are printed

    /*! Ctor */
    options_t() : ClassNames(false), CaseNames(caseNames_t::OFF), Colors(false), Tests(testResults_t::FAILS), FinalStatistics(finalStatistics_t::DEFAULT) {}
};

/*! Abstracts a class that is called to initialize the db_t (and whatever else needs to be initialized).

Note that by default db_t sets up a defaultInitializer_t to perform the default parsing of cmdline args.
Override by modifying db_t::instance().Initializers (adding your own and/or removing the default one)
usually as a side effect of a global variable initialization.
If you want to modify the commandline args, just add your initializer after the default one and in on_init()
update its argument definitions. */
class initializer_t
{
public:
    virtual ~initializer_t() {}
    /*! Called to setup the initializers themselves. Do not call db_t::instance() here, rather use the given parameter. */
    virtual void on_init(db_t& DB) =0;
    /*! Called to parse the command line arguments. (Calling db_t::instance() is not limited here.) */
    virtual void on_setup(int argc, const char_t** argv) =0;
};

/*! Abstracts the interface a class has to implement to act as an outputter during the db_t::run().

Note that by default the db_t instantiates a defaultOutput_t to print to the terminal. */
class output_t
{
public:
    /*! Dtor */
    virtual ~output_t() {}

    enum test_result_t
    {
        PASSED, //!< the condition evaluated to true (=test passed)
        WARNING, //!< the condition evaluated to false (in JJ_WARN*)
        FAILED, //!< the condition evaluated to false (in JJ_TEST*, JJ_ENSURE* or JJ_MUSTBE*)
        FAILINFO //!< a subsequent info for a failure (when testFailed_t or testingFailed_t is caught)
    };

    // called in list mode
    /*! If db_t::ListClassVariants is false then called once for each class. */
    virtual void list_class(const string_t& name) =0;
    /*! If db_t::ListClassVariants is true then called once for each class variant. */
    virtual void list_class(const string_t& name, const string_t& variant) =0;
    /*! If db_t::ListCaseVariants is false then called once for each case in class. */
    virtual void list_case(const string_t& name) =0;
    /*! If db_t::ListCaseVariants is true then called once for each case variant in class. */
    virtual void list_case(const string_t& name, const string_t& variant) =0;

    // called in run mode
    /*! Called right before a new test class (or it's variant) is instantiated. */
    virtual void enter_class(const string_t& name, const string_t& variant) =0;
    /*! Called right after a test class (or it's variant) is destroyed. */
    virtual void leave_class(const string_t& name, const string_t& variant, const statistics_t& stats) =0;
    /*! Called right before a test case (or it's variant) is run. */
    virtual void enter_case(const string_t& name, const string_t& variant) =0;
    /*! Called right after a test case (or it's variant) finishes. */
    virtual void leave_case(const string_t& name, const string_t& variant) =0;
    /*! Called from within a TEST/ENSURE/MUSTBE checks, the first argument depends on the result of the condition. */
    virtual void test_result(test_result_t result, const string_t& text) =0;
    /*! Called at the end of testing to present the final counts for testcases. */
    virtual void statistics(const statistics_t& stats) =0;
};

namespace AUX
{
/*! No other meaning than just defining an empty struct type. */
struct nothing_t {};

/*! Parses the default command line arguments and sets up options_t accordingly. */
struct defaultInitializer_t : public initializer_t
{
    defaultInitializer_t();
    virtual void on_init(db_t& DB);
    virtual void on_setup(int argc, const char_t** argv);
    std::shared_ptr<cmdLine::definitions_t> ArgumentDefinitions;
    std::shared_ptr<cmdLine::arguments_t> Arguments;
};

/*! Prints to stdout. Honours the values in options_t. When using colors it uses the ANSI color codes. */
struct defaultOutput_t : public output_t
{
    defaultOutput_t(options_t& opt) : opt_(opt) {}

    virtual void list_class(const string_t& name);
    virtual void list_class(const string_t& name, const string_t& variant);
    virtual void list_case(const string_t& name);
    virtual void list_case(const string_t& name, const string_t& variant);

    virtual void enter_class(const string_t& name, const string_t& variant);
    virtual void leave_class(const string_t& name, const string_t& variant, const statistics_t& stats);
    virtual void enter_case(const string_t& name, const string_t& variant);
    virtual void leave_case(const string_t& name, const string_t& variant);
    virtual void test_result(test_result_t result, const string_t& text);
    virtual void statistics(const statistics_t& stats);

private:
    options_t& opt_;
};

/*! Helper that only proxies the db_t (which derives from this) output calls to the individual registered outputs. */
struct db_output_t : public output_t
{
    typedef std::shared_ptr<output_t> outptr_t;
    typedef std::list<outptr_t> outlist_t;
    outlist_t Outputs;

    virtual void list_class(const string_t& name)
    {
        for (auto& o : Outputs)
            o->list_class(name);
    }
    virtual void list_class(const string_t& name, const string_t& variant)
    {
        for (auto& o : Outputs)
            o->list_class(name, variant);
    }
    virtual void list_case(const string_t& name)
    {
        for (auto& o : Outputs)
            o->list_case(name);
    }
    virtual void list_case(const string_t& name, const string_t& variant)
    {
        for (auto& o : Outputs)
            o->list_case(name, variant);
    }

    virtual void enter_class(const string_t& name, const string_t& variant)
    {
        for (auto& o : Outputs)
            o->enter_class(name, variant);
    }
    virtual void leave_class(const string_t& name, const string_t& variant, const statistics_t& stats)
    {
        for (auto& o : Outputs)
            o->leave_class(name, variant, stats);
    }
    virtual void enter_case(const string_t& name, const string_t& variant)
    {
        for (auto& o : Outputs)
            o->enter_case(name, variant);
    }
    virtual void leave_case(const string_t& name, const string_t& variant)
    {
        for (auto& o : Outputs)
            o->leave_case(name, variant);
    }
    virtual void test_result(test_result_t result, const string_t& text)
    {
        for (auto& o : Outputs)
            o->test_result(result, text);
    }
    virtual void statistics(const statistics_t& stats)
    {
        for (auto& o : Outputs)
            o->statistics(stats);
    }
};

/*! A helper class providing interface and common functionality for storing individual tests within the class. */
class holder_base_t
{
public:
    /*! Prints all testcases in the testclass, prints names only or names and variants depending on the variants parameter. */
    virtual void list(bool variants) const =0;
};

/*! Stores information about a filter (the value of --run/--skip argument). */
struct filter_t
{
    /*! Type of filter. */
    enum filterType_t
    {
        ADD, //!< adds to class/case selection
        REMOVE //!< removes from class/case selection
    };
    filterType_t Type; //!< type of filter
    string_t
        Class, //!< parsed class name (can be empty if Case is given, then all classes match unless some previous filter specified class)
        ClassVariant, //!< parsed class variant (portion in brackets including brackets), can only be specified if Class is specified
        Case, //!< parsed case name (can be empty if Class is given, the all cases in class/all classes/selected classes match)
        CaseVariant; //!< parsed case variant (portion in brackets including brackets), can only be specified if Case is specified
    /*! Ctor - sets type and parses given filter into Class, ClassVariant, Case, CaseVariant; throws runtime_error if parsing fails. */
    filter_t(filterType_t type, const string_t& filter);
};
/*! Represents all filters parsed from cmdline arguments. */
typedef std::list<filter_t> filters_t;

/*! Filtering helper class. References to actual filter_t instances.

List of these produced in db_t::check_class_filters(), contains only filters relevant for class. */
class filter_ref_t
{
    const filter_t& f_; //!< actual filter instance reference
public:
    /*! Ctor */
    filter_ref_t(const filter_t& f) : f_(f) {}
    /*! Indirection. */
    const filter_t& operator*() const { return f_; }
    /*! Indirection. */
    const filter_t* operator->() const { return &f_; }
};
/*! List of filter references. Produced in db_t::check_class_filter(), used in db_t::check_case_filters(). */
typedef std::list<filter_ref_t> filter_refs_t;

/*! Contains interface for actual test class, the actual class derives from this (the actual class is the template parameter).
Through the helper class JJ_TESTCASE_holder_t contains all the test cases. For every class variant a new instance of the class
is created but the JJ_TESTCASE_holder_t is always the same (=same list of testcases for each variant). */
template<typename T>
class testclass_base_T : public testclass_base_t
{
    typedef T parent_t; //!< denotes the actual test class type

public:
    //! the type of method that is used to run individual test cases(/variants)
    typedef void(*JJ_TESTCASE_runner_fn)(parent_t&, statistics_t&);
    //!< name of case variant (the part in brackets) and the method to run the variant
    typedef std::pair<string_t, JJ_TESTCASE_runner_fn> JJ_TESTCASE_variant_t;
    //!< type used to store all variants of a single test case
    typedef std::list<JJ_TESTCASE_variant_t> JJ_TESTCASE_variants_t;
    //!< name of case and list of all its variants (and runners)
    typedef std::pair<string_t, JJ_TESTCASE_variants_t> JJ_TESTCASE_t;
    //!< type denoting the list of all test cases in the test class
    typedef std::list<JJ_TESTCASE_t> JJ_TESTCASE_list_t;

    /*! Helper class to actually store the testcases in the test class. It is a singleton.

    It is a separate class as the list of testcases is independent of the individual class variants,
    ie. same test cases for each class variant. */
    struct JJ_TESTCASE_holder_t : public holder_base_t
    {
        JJ_TESTCASE_list_t list_; //!< all test cases (names and variants and runners)

        /*! Saves a new test case variant into the list. The fn/name/args specify the method to call to run the test/the name of the test/the variant of the test. */
        void register_testcase(JJ_TESTCASE_runner_fn fn, const char_t* name, const char_t* args)
        {
            for (typename JJ_TESTCASE_list_t::value_type& i : list_)
            {
                if (i.first == name)
                {
                    i.second.push_back(JJ_TESTCASE_variant_t(args, fn));
                    return;
                }
            }
            list_.push_back(JJ_TESTCASE_t(name, JJ_TESTCASE_variants_t()));
            list_.back().second.push_back(JJ_TESTCASE_variant_t(args, fn));
        }
        /*! Returns the instance of this singleton. */
        static JJ_TESTCASE_holder_t& instance() { static JJ_TESTCASE_holder_t inst; return inst; }

        /*! Lists all testcases stored in this instance to standard output.
        Depending on the value of variants prints either only case names or names and variants. */
        void list(bool variants) const;
        /*! Checks given filters and runs all cases (and variants) stored here and matching given filters
        on top of given testclass. Updates given stats along the way. */
        void run(parent_t& testclass, statistics_t& stats, const filter_refs_t& filters);
    };
};

} // namespace AUX

/*! Represents the test framework and holds the list of all test classes (and variants) in the binary.

The implementation also provides a main() method so you only link against this lib. */
class db_t : public options_t, public AUX::db_output_t
{
    //!< type of function to instantiate test classes (in variants) and running test cases in them
    typedef void(*runner_fn)(statistics_t&, const AUX::filter_refs_t&);

    // TODO replace with const char_t*?
    /*! class variant name and runner of test cases */
    typedef std::pair<string_t, runner_fn> testclass_variant_t;
    /*! list of all variants (and runners) of a test class */
    typedef std::list<testclass_variant_t> testclass_variants_t;
    /*! list of all class variants and pointer to singleton holding the test classes' test cases (and runners). */
    typedef std::pair<testclass_variants_t, AUX::holder_base_t*> testclass_data_t;
    /*! storage for all testclasses (and variants and runners)

    Note: the variants are used when running the tests, the pointer when listing the tests. */
    typedef std::map<string_t, testclass_data_t> testclasses_t;
    testclasses_t testclasses_; //!< stores info about all individual test classes (and variants)

    /*! Internal helper to list all test cases in a test class. */
    void do_list(
        const testclasses_t::value_type& testclass, //!< class to be printed
        bool classvariants, //!< whether to print the variants or just the name
        bool tests, //!< whether to print the test cases
        bool testvariants=false //!< whether to print test case variants or just case names; ignored if tests is false
        );

    /*! Ctor, note this class is a singleton. */
    db_t()
        : Mode(RUN), ListClassVariants(false), ListCaseVariants(false)
    {
        Initializers.push_back(initptr_t(new AUX::defaultInitializer_t));
        Outputs.push_back(outptr_t(new AUX::defaultOutput_t(*this)));
    }

public:
    /*! Provides access to the test framework. */
    static db_t& instance() { static db_t inst; return inst; }

    //! Holds an initializer of the test framework. */
    typedef std::shared_ptr<initializer_t> initptr_t;
    /*! Type holding all initializer of the test framework. */
    typedef std::list<initptr_t> initlist_t;
    initlist_t Initializers; //!< holds all initializers, add your in a global variable initialization

    /*! What should the framework do - listing or running test cases. */
    enum mode_t { LIST, LIST_CLASSES, RUN };
    mode_t Mode; //!< what should the test framework do - list or run test cases
    bool
        ListClassVariants, //!< in list mode whether to list class variants, ignored in run mode
        ListCaseVariants; //!< in list mode whether to list case variants, ignored in run mode
    string_t ListClass; //!< in list mode list only test cases in this test class; ignored in run mode

    statistics_t Statistics; //!< the overall statistics of passed / failed tests

    void register_testclass(runner_fn fn, const char_t* name, const char_t* args)
    {
        testclasses_[name].first.push_back(testclass_variant_t(args, fn));
    }
    void register_testholder(const char_t* name, AUX::holder_base_t* hold)
    {
        testclasses_[name].second = hold;
    }

    /*! Prints all test classes to standard output. */
    void list_testclasses(bool classvariants=false);
    /*! Prints all test cases of all test classes to standard output. Depending on parameters it will either only print the class/test name or names and and all arg combinations. */
    void list_testcases(bool classvariants=false, bool testvariants=false);
    /*! Prints all test cases of a specific test class. */
    void list_testcases(const string_t& testclass, bool classvariants=false, bool testvariants=false);

    AUX::filters_t Filters; //!< holds filters for selecting tests
    /*! Helper called in run mode to check whether class matches the Filters. Returns whether matching (ie. whether test evaluation shall be done).

    Produces filters which is a list of references to the original filters, but only the relevant for current test class are contained. */
    bool check_class_filters(const string_t& c, const string_t& v, bool& startWith, AUX::filter_refs_t& filters);
    /*! Helper called in run mode to check whether particular case variant is matching given filters. Returns whether matching. */
    bool check_case_filters(const string_t& c, const string_t& v, const AUX::filter_refs_t& filters);

    /*! Runs a single testcase in its testclass instance taking care about exception handling and statistics. */
    void run_testcase(std::function<void(statistics_t&)> tc, statistics_t& stats);

    /*! Runs all testcases. */
    bool run();
};

namespace AUX
{
template<typename T>
void testclass_base_T<T>::JJ_TESTCASE_holder_t::list(bool variants) const
{
    db_t& DB = db_t::instance();
    for (const typename JJ_TESTCASE_list_t::value_type& i : list_)
    {
        if (variants)
        {
            for (const typename JJ_TESTCASE_variants_t::value_type& v : i.second)
                DB.list_case(i.first, v.first);
        }
        else
            DB.list_case(i.first);
    }
}

template<typename T>
void testclass_base_T<T>::JJ_TESTCASE_holder_t::run(parent_t& testclass, statistics_t& stats, const filter_refs_t& filters)
{
    db_t& db = db_t::instance();
    for (typename JJ_TESTCASE_list_t::value_type& i : list_)
    {
        for (typename JJ_TESTCASE_variants_t::value_type& v : i.second)
        {
            if (!db.check_case_filters(i.first, v.first, filters))
                continue;

            db.enter_case(i.first, v.first);
            db.run_testcase([&v, &testclass](statistics_t& stats){ (v.second)(testclass, stats); }, stats);
            db.leave_case(i.first, v.first);
        }
    }
}
} // namespace AUX
} // namespace test
} // namespace jj


#define JJ___TEST_CLASS_CALL(name, no, args) \
    static void jjM2(JJ_TEST_CLASS_VARIANT_,no)(jj::test::statistics_t& stats, const jj::test::AUX::filter_refs_t& filters){ name inst args; JJ_THIS_TESTCLASS::JJ_TESTCASE_holder_t::instance().run(inst, stats, filters); } \
    static int jjM2(JJ_TEST_CLASS_REGISTRAR_,no)() { jj::test::db_t::instance().register_testclass(&name::jjM2(JJ_TEST_CLASS_VARIANT_,no), jjT(#name), jjT(#args)); return no; }
#define JJ___TEST_CLASS_CALLS1(name, p1) JJ___TEST_CLASS_CALL(name, 1, p1)
#define JJ___TEST_CLASS_CALLS2(name, p1, p2) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2)
#define JJ___TEST_CLASS_CALLS3(name, p1, p2, p3) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3)
#define JJ___TEST_CLASS_CALLS4(name, p1, p2, p3, p4) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4)
#define JJ___TEST_CLASS_CALLS5(name, p1, p2, p3, p4, p5) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name,5, p5)
#define JJ___TEST_CLASS_CALLS6(name, p1, p2, p3, p4, p5, p6) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6)
#define JJ___TEST_CLASS_CALLS7(name, p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7)
#define JJ___TEST_CLASS_CALLS8(name, p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8)
#define JJ___TEST_CLASS_CALLS9(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9)
#define JJ___TEST_CLASS_CALLS10(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10)
#define JJ___TEST_CLASS_CALLS11(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4)JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11)
#define JJ___TEST_CLASS_CALLS12(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12)
#define JJ___TEST_CLASS_CALLS13(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13)
#define JJ___TEST_CLASS_CALLS14(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14)
#define JJ___TEST_CLASS_CALLS15(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15)
#define JJ___TEST_CLASS_CALLS16(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16)
#define JJ___TEST_CLASS_CALLS17(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16) JJ___TEST_CLASS_CALL(name, 17, p17)
#define JJ___TEST_CLASS_CALLS18(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16) JJ___TEST_CLASS_CALL(name, 17, p17) JJ___TEST_CLASS_CALL(name, 18, p18)
#define JJ___TEST_CLASS_CALLS19(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16) JJ___TEST_CLASS_CALL(name, 17, p17) JJ___TEST_CLASS_CALL(name, 18, p18) JJ___TEST_CLASS_CALL(name, 19, p19)
#define JJ___TEST_CLASS_CALLS20(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16) JJ___TEST_CLASS_CALL(name, 17, p17) JJ___TEST_CLASS_CALL(name, 18, p18) JJ___TEST_CLASS_CALL(name, 19, p19) JJ___TEST_CLASS_CALL(name, 20, p20)
#define JJ___TEST_CLASS_CALLS21(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9)JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16) JJ___TEST_CLASS_CALL(name, 17, p17) JJ___TEST_CLASS_CALL(name, 18, p18) JJ___TEST_CLASS_CALL(name, 19, p19) JJ___TEST_CLASS_CALL(name, 20, p20) JJ___TEST_CLASS_CALL(name, 21, p21)
#define JJ___TEST_CLASS_CALLS22(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16) JJ___TEST_CLASS_CALL(name, 17, p17) JJ___TEST_CLASS_CALL(name, 18, p18) JJ___TEST_CLASS_CALL(name, 19, p19) JJ___TEST_CLASS_CALL(name, 20, p20) JJ___TEST_CLASS_CALL(name, 21, p21) JJ___TEST_CLASS_CALL(name, 22, p22)
#define JJ___TEST_CLASS_CALLS23(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16) JJ___TEST_CLASS_CALL(name, 17, p17) JJ___TEST_CLASS_CALL(name, 18, p18) JJ___TEST_CLASS_CALL(name, 19, p19) JJ___TEST_CLASS_CALL(name, 20, p20) JJ___TEST_CLASS_CALL(name, 21, p21) JJ___TEST_CLASS_CALL(name, 22, p22) JJ___TEST_CLASS_CALL(name, 23, p23)
#define JJ___TEST_CLASS_CALLS24(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16) JJ___TEST_CLASS_CALL(name, 17, p17) JJ___TEST_CLASS_CALL(name, 18, p18) JJ___TEST_CLASS_CALL(name, 19, p19) JJ___TEST_CLASS_CALL(name, 20, p20) JJ___TEST_CLASS_CALL(name, 21, p21) JJ___TEST_CLASS_CALL(name, 22, p22) JJ___TEST_CLASS_CALL(name, 23, p23) JJ___TEST_CLASS_CALL(name, 24, p24)
#define JJ___TEST_CLASS_CALLS25(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9) JJ___TEST_CLASS_CALL(name, 10, p10) JJ___TEST_CLASS_CALL(name, 11, p11) JJ___TEST_CLASS_CALL(name, 12, p12) JJ___TEST_CLASS_CALL(name, 13, p13) JJ___TEST_CLASS_CALL(name, 14, p14) JJ___TEST_CLASS_CALL(name, 15, p15) JJ___TEST_CLASS_CALL(name, 16, p16) JJ___TEST_CLASS_CALL(name, 17, p17) JJ___TEST_CLASS_CALL(name, 18, p18) JJ___TEST_CLASS_CALL(name, 19, p19) JJ___TEST_CLASS_CALL(name, 20, p20) JJ___TEST_CLASS_CALL(name, 21, p21) JJ___TEST_CLASS_CALL(name, 22, p22) JJ___TEST_CLASS_CALL(name, 23, p23) JJ___TEST_CLASS_CALL(name, 24, p24) JJ___TEST_CLASS_CALL(name, 25, p25)
#define JJ___TEST_CLASS_CALLS(name, ...) JJ_VARG_N1(JJ___TEST_CLASS_CALLS, name, __VA_ARGS__)

#define JJ___TEST_CLASS_REG(no) jjM2(JJ_TEST_CLASS_REGISTRAR_,no)();
#define JJ___TEST_CLASS_REGS1(dummy, p1) JJ___TEST_CLASS_REG(1)
#define JJ___TEST_CLASS_REGS2(dummy, p1, p2) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2)
#define JJ___TEST_CLASS_REGS3(dummy, p1, p2, p3) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3)
#define JJ___TEST_CLASS_REGS4(dummy, p1, p2, p3, p4) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4)
#define JJ___TEST_CLASS_REGS5(dummy, p1, p2, p3, p4, p5) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5)
#define JJ___TEST_CLASS_REGS6(dummy, p1, p2, p3, p4, p5, p6) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6)
#define JJ___TEST_CLASS_REGS7(dummy, p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7)
#define JJ___TEST_CLASS_REGS8(dummy, p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8)
#define JJ___TEST_CLASS_REGS9(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9)
#define JJ___TEST_CLASS_REGS10(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10)
#define JJ___TEST_CLASS_REGS11(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11)
#define JJ___TEST_CLASS_REGS12(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12)
#define JJ___TEST_CLASS_REGS13(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13)
#define JJ___TEST_CLASS_REGS14(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14)
#define JJ___TEST_CLASS_REGS15(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15)
#define JJ___TEST_CLASS_REGS16(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16)
#define JJ___TEST_CLASS_REGS17(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16) JJ___TEST_CLASS_REG(17)
#define JJ___TEST_CLASS_REGS18(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16) JJ___TEST_CLASS_REG(17) JJ___TEST_CLASS_REG(18)
#define JJ___TEST_CLASS_REGS19(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16) JJ___TEST_CLASS_REG(17) JJ___TEST_CLASS_REG(18) JJ___TEST_CLASS_REG(19)
#define JJ___TEST_CLASS_REGS20(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16) JJ___TEST_CLASS_REG(17) JJ___TEST_CLASS_REG(18) JJ___TEST_CLASS_REG(19) JJ___TEST_CLASS_REG(20)
#define JJ___TEST_CLASS_REGS21(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16) JJ___TEST_CLASS_REG(17) JJ___TEST_CLASS_REG(18) JJ___TEST_CLASS_REG(19) JJ___TEST_CLASS_REG(20) JJ___TEST_CLASS_REG(21)
#define JJ___TEST_CLASS_REGS22(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16) JJ___TEST_CLASS_REG(17) JJ___TEST_CLASS_REG(18) JJ___TEST_CLASS_REG(19) JJ___TEST_CLASS_REG(20) JJ___TEST_CLASS_REG(21) JJ___TEST_CLASS_REG(22)
#define JJ___TEST_CLASS_REGS23(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16) JJ___TEST_CLASS_REG(17) JJ___TEST_CLASS_REG(18) JJ___TEST_CLASS_REG(19) JJ___TEST_CLASS_REG(20) JJ___TEST_CLASS_REG(21) JJ___TEST_CLASS_REG(22) JJ___TEST_CLASS_REG(23)
#define JJ___TEST_CLASS_REGS24(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16) JJ___TEST_CLASS_REG(17) JJ___TEST_CLASS_REG(18) JJ___TEST_CLASS_REG(19) JJ___TEST_CLASS_REG(20) JJ___TEST_CLASS_REG(21) JJ___TEST_CLASS_REG(22) JJ___TEST_CLASS_REG(23) JJ___TEST_CLASS_REG(24)
#define JJ___TEST_CLASS_REGS25(dummy, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9) JJ___TEST_CLASS_REG(10) JJ___TEST_CLASS_REG(11) JJ___TEST_CLASS_REG(12) JJ___TEST_CLASS_REG(13) JJ___TEST_CLASS_REG(14) JJ___TEST_CLASS_REG(15) JJ___TEST_CLASS_REG(16) JJ___TEST_CLASS_REG(17) JJ___TEST_CLASS_REG(18) JJ___TEST_CLASS_REG(19) JJ___TEST_CLASS_REG(20) JJ___TEST_CLASS_REG(21) JJ___TEST_CLASS_REG(22) JJ___TEST_CLASS_REG(23) JJ___TEST_CLASS_REG(24) JJ___TEST_CLASS_REG(25)
#define JJ___TEST_CLASS_REGS(dummy, ...) JJ_VARG_N1(JJ___TEST_CLASS_REGS, dummy, __VA_ARGS__)
// note: the dummy parameter above is not required except for MSVC which cannot have empty macro argument list ie, MACRO()

#define JJ___TEST_CASE_DEF(classname, testname) classname::jjM2(registrar_,testname)();
#define JJ___TEST_CASE_DEFS1(classname, p1) JJ___TEST_CASE_DEF(classname, p1)
#define JJ___TEST_CASE_DEFS2(classname, p1, p2) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2)
#define JJ___TEST_CASE_DEFS3(classname, p1, p2, p3) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3)
#define JJ___TEST_CASE_DEFS4(classname, p1, p2, p3, p4) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4)
#define JJ___TEST_CASE_DEFS5(classname, p1, p2, p3, p4, p5) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5)
#define JJ___TEST_CASE_DEFS6(classname, p1, p2, p3, p4, p5, p6) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6)
#define JJ___TEST_CASE_DEFS7(classname, p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7)
#define JJ___TEST_CASE_DEFS8(classname, p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8)
#define JJ___TEST_CASE_DEFS9(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9)
#define JJ___TEST_CASE_DEFS10(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10)
#define JJ___TEST_CASE_DEFS11(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11)
#define JJ___TEST_CASE_DEFS12(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12)
#define JJ___TEST_CASE_DEFS13(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13)
#define JJ___TEST_CASE_DEFS14(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14)
#define JJ___TEST_CASE_DEFS15(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15)
#define JJ___TEST_CASE_DEFS16(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16)
#define JJ___TEST_CASE_DEFS17(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16) JJ___TEST_CASE_DEF(classname, p17)
#define JJ___TEST_CASE_DEFS18(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname,p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16) JJ___TEST_CASE_DEF(classname, p17) JJ___TEST_CASE_DEF(classname, p18)
#define JJ___TEST_CASE_DEFS19(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16) JJ___TEST_CASE_DEF(classname, p17) JJ___TEST_CASE_DEF(classname, p18) JJ___TEST_CASE_DEF(classname, p19)
#define JJ___TEST_CASE_DEFS20(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16) JJ___TEST_CASE_DEF(classname, p17) JJ___TEST_CASE_DEF(classname, p18) JJ___TEST_CASE_DEF(classname, p19) JJ___TEST_CASE_DEF(classname, p20)
#define JJ___TEST_CASE_DEFS21(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16) JJ___TEST_CASE_DEF(classname, p17) JJ___TEST_CASE_DEF(classname, p18) JJ___TEST_CASE_DEF(classname, p19) JJ___TEST_CASE_DEF(classname, p20) JJ___TEST_CASE_DEF(classname, p21)
#define JJ___TEST_CASE_DEFS22(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16) JJ___TEST_CASE_DEF(classname, p17) JJ___TEST_CASE_DEF(classname, p18) JJ___TEST_CASE_DEF(classname, p19) JJ___TEST_CASE_DEF(classname, p20) JJ___TEST_CASE_DEF(classname, p21) JJ___TEST_CASE_DEF(classname, p22)
#define JJ___TEST_CASE_DEFS23(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16) JJ___TEST_CASE_DEF(classname, p17) JJ___TEST_CASE_DEF(classname, p18) JJ___TEST_CASE_DEF(classname, p19) JJ___TEST_CASE_DEF(classname, p20) JJ___TEST_CASE_DEF(classname, p21) JJ___TEST_CASE_DEF(classname, p22) JJ___TEST_CASE_DEF(classname, p23)
#define JJ___TEST_CASE_DEFS24(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2)JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16) JJ___TEST_CASE_DEF(classname, p17) JJ___TEST_CASE_DEF(classname, p18) JJ___TEST_CASE_DEF(classname, p19) JJ___TEST_CASE_DEF(classname, p20) JJ___TEST_CASE_DEF(classname, p21) JJ___TEST_CASE_DEF(classname, p22) JJ___TEST_CASE_DEF(classname, p23) JJ___TEST_CASE_DEF(classname, p24)
#define JJ___TEST_CASE_DEFS25(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9) JJ___TEST_CASE_DEF(classname, p10) JJ___TEST_CASE_DEF(classname, p11) JJ___TEST_CASE_DEF(classname, p12) JJ___TEST_CASE_DEF(classname, p13) JJ___TEST_CASE_DEF(classname, p14) JJ___TEST_CASE_DEF(classname, p15) JJ___TEST_CASE_DEF(classname, p16) JJ___TEST_CASE_DEF(classname, p17) JJ___TEST_CASE_DEF(classname, p18) JJ___TEST_CASE_DEF(classname, p19) JJ___TEST_CASE_DEF(classname, p20) JJ___TEST_CASE_DEF(classname, p21) JJ___TEST_CASE_DEF(classname, p22) JJ___TEST_CASE_DEF(classname, p23) JJ___TEST_CASE_DEF(classname, p24) JJ___TEST_CASE_DEF(classname, p25)
#define JJ___TEST_CASE_DEFS(classname, ...) JJ_VARG_N1(JJ___TEST_CASE_DEFS, classname, __VA_ARGS__)

#define JJ___TEST_CASE_CALL(name, no, args) \
    static void jjM3(runner,no,name)(JJ_THIS_TESTCLASS& inst, jj::test::statistics_t& stats) { inst.JJ_TEST_CASE_Statistics.reset(); (inst.*&JJ_THIS_TESTCLASS::name) args; stats = inst.JJ_TEST_CASE_Statistics; }
#define JJ___TEST_CASE_CALLS1(name, p1) JJ___TEST_CASE_CALL(name, 1, p1)
#define JJ___TEST_CASE_CALLS2(name, p1, p2) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2)
#define JJ___TEST_CASE_CALLS3(name, p1, p2, p3) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3)
#define JJ___TEST_CASE_CALLS4(name, p1, p2, p3, p4) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4)
#define JJ___TEST_CASE_CALLS5(name, p1, p2, p3, p4, p5) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5)
#define JJ___TEST_CASE_CALLS6(name, p1, p2, p3, p4, p5, p6) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6)
#define JJ___TEST_CASE_CALLS7(name, p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7)
#define JJ___TEST_CASE_CALLS8(name, p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8)
#define JJ___TEST_CASE_CALLS9(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9)
#define JJ___TEST_CASE_CALLS10(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10)
#define JJ___TEST_CASE_CALLS11(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11)
#define JJ___TEST_CASE_CALLS12(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4)JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12)
#define JJ___TEST_CASE_CALLS13(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13)
#define JJ___TEST_CASE_CALLS14(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14)
#define JJ___TEST_CASE_CALLS15(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15)
#define JJ___TEST_CASE_CALLS16(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10)JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15) JJ___TEST_CASE_CALL(name, 16, p16)
#define JJ___TEST_CASE_CALLS17(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10,p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15) JJ___TEST_CASE_CALL(name, 16, p16) JJ___TEST_CASE_CALL(name, 17, p17)
#define JJ___TEST_CASE_CALLS18(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15) JJ___TEST_CASE_CALL(name, 16, p16)JJ___TEST_CASE_CALL(name, 17, p17) JJ___TEST_CASE_CALL(name, 18, p18)
#define JJ___TEST_CASE_CALLS19(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15) JJ___TEST_CASE_CALL(name, 16,p16) JJ___TEST_CASE_CALL(name, 17, p17) JJ___TEST_CASE_CALL(name, 18, p18) JJ___TEST_CASE_CALL(name, 19, p19)
#define JJ___TEST_CASE_CALLS20(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name,3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15) JJ___TEST_CASE_CALL(name, 16, p16) JJ___TEST_CASE_CALL(name, 17, p17) JJ___TEST_CASE_CALL(name, 18, p18) JJ___TEST_CASE_CALL(name, 19, p19) JJ___TEST_CASE_CALL(name, 20, p20)
#define JJ___TEST_CASE_CALLS21(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15) JJ___TEST_CASE_CALL(name, 16, p16) JJ___TEST_CASE_CALL(name, 17, p17) JJ___TEST_CASE_CALL(name, 18, p18) JJ___TEST_CASE_CALL(name, 19, p19) JJ___TEST_CASE_CALL(name, 20, p20) JJ___TEST_CASE_CALL(name, 21, p21)
#define JJ___TEST_CASE_CALLS22(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15) JJ___TEST_CASE_CALL(name, 16, p16) JJ___TEST_CASE_CALL(name, 17, p17) JJ___TEST_CASE_CALL(name, 18, p18) JJ___TEST_CASE_CALL(name, 19, p19) JJ___TEST_CASE_CALL(name, 20, p20) JJ___TEST_CASE_CALL(name, 21, p21) JJ___TEST_CASE_CALL(name, 22, p22)
#define JJ___TEST_CASE_CALLS23(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9)JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15) JJ___TEST_CASE_CALL(name, 16, p16) JJ___TEST_CASE_CALL(name, 17, p17) JJ___TEST_CASE_CALL(name, 18, p18) JJ___TEST_CASE_CALL(name, 19, p19) JJ___TEST_CASE_CALL(name, 20, p20) JJ___TEST_CASE_CALL(name, 21, p21) JJ___TEST_CASE_CALL(name, 22, p22) JJ___TEST_CASE_CALL(name, 23, p23)
#define JJ___TEST_CASE_CALLS24(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15) JJ___TEST_CASE_CALL(name, 16, p16) JJ___TEST_CASE_CALL(name, 17, p17) JJ___TEST_CASE_CALL(name, 18, p18) JJ___TEST_CASE_CALL(name, 19, p19) JJ___TEST_CASE_CALL(name, 20, p20) JJ___TEST_CASE_CALL(name, 21, p21) JJ___TEST_CASE_CALL(name, 22, p22) JJ___TEST_CASE_CALL(name, 23, p23) JJ___TEST_CASE_CALL(name, 24, p24)
#define JJ___TEST_CASE_CALLS25(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9) JJ___TEST_CASE_CALL(name, 10, p10) JJ___TEST_CASE_CALL(name, 11, p11) JJ___TEST_CASE_CALL(name, 12, p12) JJ___TEST_CASE_CALL(name, 13, p13) JJ___TEST_CASE_CALL(name, 14, p14) JJ___TEST_CASE_CALL(name, 15, p15)JJ___TEST_CASE_CALL(name, 16, p16) JJ___TEST_CASE_CALL(name, 17, p17) JJ___TEST_CASE_CALL(name, 18, p18) JJ___TEST_CASE_CALL(name, 19, p19) JJ___TEST_CASE_CALL(name, 20, p20) JJ___TEST_CASE_CALL(name, 21, p21) JJ___TEST_CASE_CALL(name, 22, p22) JJ___TEST_CASE_CALL(name, 23, p23) JJ___TEST_CASE_CALL(name, 24, p24) JJ___TEST_CASE_CALL(name, 25, p25)
#define JJ___TEST_CASE_CALLS(name, ...) JJ_VARG_N1(JJ___TEST_CASE_CALLS, name, __VA_ARGS__)

#define JJ___TEST_CASE_REG(name, no, args) \
    JJ_THIS_TESTCLASS::JJ_TESTCASE_holder_t::instance().register_testcase(&JJ_THIS_TESTCLASS::jjM3(runner,no,name), jjT(#name), jjT(#args));
#define JJ___TEST_CASE_REGS1(name, p1) JJ___TEST_CASE_REG(name, 1, p1)
#define JJ___TEST_CASE_REGS2(name, p1, p2) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2)
#define JJ___TEST_CASE_REGS3(name, p1, p2, p3) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3)
#define JJ___TEST_CASE_REGS4(name, p1, p2, p3, p4) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4)
#define JJ___TEST_CASE_REGS5(name, p1, p2, p3, p4, p5) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5)
#define JJ___TEST_CASE_REGS6(name, p1, p2, p3, p4, p5, p6) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6)
#define JJ___TEST_CASE_REGS7(name, p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7)
#define JJ___TEST_CASE_REGS8(name, p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name,5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8)
#define JJ___TEST_CASE_REGS9(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9)
#define JJ___TEST_CASE_REGS10(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10)
#define JJ___TEST_CASE_REGS11(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11)
#define JJ___TEST_CASE_REGS12(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12)
#define JJ___TEST_CASE_REGS13(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4)JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13)
#define JJ___TEST_CASE_REGS14(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14)
#define JJ___TEST_CASE_REGS15(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15)
#define JJ___TEST_CASE_REGS16(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16)
#define JJ___TEST_CASE_REGS17(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16) JJ___TEST_CASE_REG(name, 17, p17)
#define JJ___TEST_CASE_REGS18(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16) JJ___TEST_CASE_REG(name, 17, p17) JJ___TEST_CASE_REG(name, 18, p18)
#define JJ___TEST_CASE_REGS19(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16) JJ___TEST_CASE_REG(name, 17, p17) JJ___TEST_CASE_REG(name, 18, p18) JJ___TEST_CASE_REG(name, 19, p19)
#define JJ___TEST_CASE_REGS20(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name,10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16) JJ___TEST_CASE_REG(name, 17, p17) JJ___TEST_CASE_REG(name, 18, p18) JJ___TEST_CASE_REG(name, 19, p19) JJ___TEST_CASE_REG(name, 20, p20)
#define JJ___TEST_CASE_REGS21(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16) JJ___TEST_CASE_REG(name, 17, p17) JJ___TEST_CASE_REG(name, 18, p18) JJ___TEST_CASE_REG(name, 19, p19) JJ___TEST_CASE_REG(name, 20, p20) JJ___TEST_CASE_REG(name, 21, p21)
#define JJ___TEST_CASE_REGS22(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16) JJ___TEST_CASE_REG(name, 17, p17) JJ___TEST_CASE_REG(name, 18, p18) JJ___TEST_CASE_REG(name, 19, p19) JJ___TEST_CASE_REG(name, 20, p20) JJ___TEST_CASE_REG(name, 21, p21) JJ___TEST_CASE_REG(name, 22, p22)
#define JJ___TEST_CASE_REGS23(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16) JJ___TEST_CASE_REG(name, 17, p17) JJ___TEST_CASE_REG(name, 18, p18) JJ___TEST_CASE_REG(name, 19, p19) JJ___TEST_CASE_REG(name, 20, p20) JJ___TEST_CASE_REG(name, 21, p21) JJ___TEST_CASE_REG(name, 22, p22) JJ___TEST_CASE_REG(name, 23, p23)
#define JJ___TEST_CASE_REGS24(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9) JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16) JJ___TEST_CASE_REG(name, 17, p17) JJ___TEST_CASE_REG(name, 18, p18) JJ___TEST_CASE_REG(name, 19, p19) JJ___TEST_CASE_REG(name, 20, p20) JJ___TEST_CASE_REG(name, 21, p21) JJ___TEST_CASE_REG(name, 22, p22) JJ___TEST_CASE_REG(name, 23, p23) JJ___TEST_CASE_REG(name, 24, p24)
#define JJ___TEST_CASE_REGS25(name, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9)JJ___TEST_CASE_REG(name, 10, p10) JJ___TEST_CASE_REG(name, 11, p11) JJ___TEST_CASE_REG(name, 12, p12) JJ___TEST_CASE_REG(name, 13, p13) JJ___TEST_CASE_REG(name, 14, p14) JJ___TEST_CASE_REG(name, 15, p15) JJ___TEST_CASE_REG(name, 16, p16) JJ___TEST_CASE_REG(name, 17, p17) JJ___TEST_CASE_REG(name, 18, p18) JJ___TEST_CASE_REG(name, 19, p19) JJ___TEST_CASE_REG(name, 20, p20) JJ___TEST_CASE_REG(name, 21, p21) JJ___TEST_CASE_REG(name, 22, p22) JJ___TEST_CASE_REG(name, 23, p23) JJ___TEST_CASE_REG(name, 24, p24) JJ___TEST_CASE_REG(name, 25, p25)
#define JJ___TEST_CASE_REGS(name, ...) JJ_VARG_N1(JJ___TEST_CASE_REGS, name, __VA_ARGS__)


/*! Opens a test class, inside it JJ_TEST_CASE shall be defined (and any other definitions that are required).
Note that internally this starts a struct (ie. something like struct name ... { */
#define JJ_TEST_CLASS(name) JJ_TEST_CLASS_DERIVED_VARIANTS(name,private jj::test::AUX::nothing_t,)
/*! Opens a test class. You are responsible to define all constructors to suit all the variants!
For all other see description of JJ_TEST_CLASS. */
#define JJ_TEST_CLASS_VARIANTS(name, ...) JJ_TEST_CLASS_DERIVED_VARIANTS(name,private jj::test::AUX::nothing_t,__VA_ARGS__)
/*! Same as JJ_TEST_CLASS but testclass derives from given list of classes. */
#define JJ_TEST_CLASS_DERIVED(name, from) JJ_TEST_CLASS_DERIVED_VARIANTS(name,from,)
/*! Same as JJ_TEST_CLASS_VARIANTS but testclass derives from given list of classes. */
#define JJ_TEST_CLASS_DERIVED_VARIANTS(name, from, ...) \
     struct name : from, public jj::test::AUX::testclass_base_T<name> \
     { \
         typedef name JJ_THIS_TESTCLASS; \
         const char* JJ_TEST_CLASS_get_name() const { return #name; } \
         JJ___TEST_CLASS_CALLS(name, __VA_ARGS__) \
         static void JJ_TEST_CLASS_register() { \
             JJ___TEST_CLASS_REGS(dummy, __VA_ARGS__) \
         }

/*! Finishes the defition of a testclass without defining the global variables. (Can be used in header.) */
#define JJ_TEST_CLASS_END_NODEF(name) \
     };
/*! Defines the global variables associated with testclass. (Shall be used in source file if JJ_TEST_CLASS_END_NODEF used in header.)
You have to name all the testcases as the additional parameters. */
#define JJ_TEST_CLASS_DEF(name, ...) \
     struct jjM2(name,__DEF) \
     { \
         jjM2(name,__DEF)() { \
             name::JJ_TEST_CLASS_register(); \
             jj::test::db_t::instance().register_testholder(jjT(#name), &name::JJ_TESTCASE_holder_t::instance()); \
             JJ___TEST_CASE_DEFS(name, __VA_ARGS__) \
         } \
     }; \
     jjM2(name,__DEF) jjM3(g_,name,__DEF);
/*! Finishes the definition of a testclass along with defining all the associated global variables.
You have to name all the testcases as the additional parameters. */
#define JJ_TEST_CLASS_END(name, ...) \
    JJ_TEST_CLASS_END_NODEF(name) \
    JJ_TEST_CLASS_DEF(name, __VA_ARGS__)

/*! Starts testcase implementation with a single variant without parameters. */
#define JJ_TEST_CASE(name) JJ_TEST_CASE_VARIANTS(name, (), ())
/*! Starts testcase implementation and defines all the variants per given parameters. */
#define JJ_TEST_CASE_VARIANTS(name, args, ...) \
    JJ___TEST_CASE_CALLS(name, __VA_ARGS__) \
    static void jjM2(registrar_,name)() { \
        JJ___TEST_CASE_REGS(name, __VA_ARGS__) \
    } \
    void name args


#define JJ___MEMBER_WARNING JJ_TEST_CASE_Statistics.Passed
#define JJ___MEMBER_FAILED JJ_TEST_CASE_Statistics.Failed

#define JJ___DO_TEST(cnd, msg, rt, exc) { \
    jj::test::db_t& DB = jj::test::db_t::instance(); \
    if (cnd) { \
        if (DB.Tests == jj::test::options_t::testResults_t::ALL) \
            DB.test_result(jj::test::output_t::PASSED, jjS(msg)); \
        ++ JJ_TEST_CLASS_ACCESSOR JJ_TEST_CASE_Statistics.Passed; \
    } else { \
        if (DB.Tests != jj::test::options_t::testResults_t::NONE) \
            DB.test_result(jj::test::output_t::rt, jjS(msg)); \
        ++ JJ_TEST_CLASS_ACCESSOR jjM2(JJ___MEMBER_,rt); \
        exc \
    }}

/*! By default the members of a testclass are accessed directly. However if any of the JJ_WARN/JJ_TEST/... shall be used in a class from which the testclass derives this macro
must be defined to a member name followed by the accessor which leads to the testclass_base_t class. Define JJ_TEST_CLASS_ACCESSOR at the beginning of such class and restore
empty definition of JJ_TEST_CLASS_ACCESSOR at the end of such class.

Example: 
class mycl_t
{ testclass_base_t bc_;
public:
  mycl_t(testclass_base_t& bc) : bc_(bc) {}

#undef JJ_TEST_CLASS_ACCESSOR
#define JJ_TEST_CLASS_ACCESSOR bc_.
  void method()
  { ...
    JJ_TEST(mycondition);
    ...
  }
#undef JJ_TEST_CLASS_ACCESSOR
#define JJ_TEST_CLASS_ACCESSOR
}

JJ_TEST_CLASS_DERIVED(mytestcl,mycl)
mytestcl() : mycl(static_cast<jj::test::testclass_base_t&>(*this)) {}
...
  */
#define JJ_TEST_CLASS_ACCESSOR

/*! Same as JJ__TEST1 but do not consider the test failed. */
#define JJ__WARN1(cond) JJ___DO_TEST(cond, #cond, WARNING,)
/*! Same as JJ__TEST2 but do not consider the test failed. */
#define JJ__WARN2(cond,msg) JJ___DO_TEST(cond, msg, WARNING,)
/*! Same as JJ_TEST but do not consider the test failed. */
#define JJ_WARN(...) JJ_VARG_N(JJ__WARN, __VA_ARGS__)

/*! Verifies a condition and logs it. */
#define JJ__TEST1(cond) JJ___DO_TEST(cond, #cond, FAILED,)
/*! Verifies a condition and logs the provided message instead. */
#define JJ__TEST2(cond,msg) JJ___DO_TEST(cond, msg, FAILED,)
/*! Verifies a condition and logs it or message provided as second parameter. */
#define JJ_TEST(...) JJ_VARG_N(JJ__TEST, __VA_ARGS__)

/*! Same as JJ__TEST1 but throws testFailed_t to skip to the end of test. */
#define JJ__ENSURE1(cond) JJ___DO_TEST(cond, #cond, FAILED, throw jj::test::testFailed_t();)
/*! Same as JJ__TEST2 but throws testFailed_t to skip to the end of test. */
#define JJ__ENSURE2(cond,msg) JJ___DO_TEST(cond,msg,FAILED,throw jj::test::testFailed_t();)
/*! Same as JJ_TEST but throws testFailed_t to skip to the end of test. */
#define JJ_ENSURE(...) JJ_VARG_N(JJ__ENSURE, __VA_ARGS__)

/*! Same as JJ__TEST1 but throws testingFailed_t to skip to the end of the test program. */
#define JJ__MUSTBE1(cond) JJ___DO_TEST(cond, #cond, FAILED, throw jj::test::testingFailed_t();)
/*! Same as JJ__TEST2 but throws testingFailed_t to skip to the end of the test program. */
#define JJ__MUSTBE2(cond,msg) JJ___DO_TEST(cond,msg,FAILED,throw jj::test::testingFailed_t();)
/*! Same as JJ_TEST but throws testingFailed_t to skip to the end of the test program. */
#define JJ_MUSTBE(...) JJ_VARG_N(JJ__MUSTBE, __VA_ARGS__)

/*! Evaluates given expression and checks that given exception was thrown. */
#define JJ_TEST_THAT_THROWS(expr, exc) \
    try { \
        expr; \
        JJ___DO_TEST(false, #expr << jjT(" throws ") << #exc, FAILED,) \
    } catch (const exc&) { \
        JJ___DO_TEST(true, #expr << jjT(" throws ") << #exc, FAILED,) \
    }

/*! Evaluates given expression and checks that given exception was thrown or skip to the end of test. */
#define JJ_ENSURE_THAT_THROWS(expr, exc) \
    try { \
        expr; \
        JJ___DO_TEST(false, #expr << jjT(" throws ") << #exc, FAILED, throw jj::test::testFailed_t();) \
    } catch (const exc&) { \
        JJ___DO_TEST(true, #expr << jjT(" throws ") << #exc, FAILED, throw jj::test::testFailed_t();) \
    }

/*! Evaluates given expression and checks that given exception was thrown or skip to the end of test program. */
#define JJ_MUSTBE_THAT_THROWS(expr, exc) \
    try { \
        expr; \
        JJ___DO_TEST(false, #expr << jjT(" throws ") << #exc, FAILED, throw jj::test::testingFailed_t();) \
    } catch (const exc&) { \
        JJ___DO_TEST(true, #expr << jjT(" throws ") << #exc, FAILED, throw jj::test::testingFailed_t();) \
    }

#endif // JJ_TEST_H
