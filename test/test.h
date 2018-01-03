#ifndef JJ_TEST_H
#define JJ_TEST_H

#include "jj/string.h"
#include <map>
#include <list>

namespace jj
{
namespace test
{

namespace AUX
{

class testclass_base_t;

class holder_base_t
{
public:
    static void print(const string_t& name);
    static void print(const string_t& name, const string_t& variant);
    virtual void list(bool variants) const =0;
};

class testclass_base_t
{
};


template<typename T>
class testclass_base_T : public testclass_base_t
{
    typedef T parent_t;

public:
    typedef void(*JJ_TESTCASE_runner_fn)(parent_t&);
    typedef std::pair<string_t, JJ_TESTCASE_runner_fn> JJ_TESTCASE_variant_t;
    typedef std::list<JJ_TESTCASE_variant_t> JJ_TESTCASE_variants_t;
    typedef std::pair<string_t, JJ_TESTCASE_variants_t> JJ_TESTCASE_t;
    typedef std::list<JJ_TESTCASE_t> JJ_TESTCASE_list_t;

    struct JJ_TESTCASE_holder_t : public holder_base_t
    {
        JJ_TESTCASE_list_t list_;
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
        static JJ_TESTCASE_holder_t& instance() { static JJ_TESTCASE_holder_t inst; return inst; }

        void list(bool variants) const
        {
            for (const typename JJ_TESTCASE_list_t::value_type& i : list_)
            {
                if (variants)
                {
                    for (const typename JJ_TESTCASE_variants_t::value_type& v : i.second)
                        print(i.first, v.first);
                }
                else
                    print(i.first);
            }
        }
        void run(parent_t& testclass)
        {
            for (typename JJ_TESTCASE_list_t::value_type& i : list_)
            {
                for (typename JJ_TESTCASE_variants_t::value_type& v : i.second)
                {
                    (v.second)(testclass);
                }
            }
        }
    };
};

} // namespace AUX

class db_t
{
    typedef void(*runner_fn)();

    // TODO replace with const char_t*
    typedef std::pair<string_t, runner_fn> testclass_variant_t;
    typedef std::list<testclass_variant_t> testclass_variants_t;
    typedef std::pair<testclass_variants_t, AUX::holder_base_t*> testclass_data_t;
    typedef std::map<string_t, testclass_data_t> testclasses_t;
    testclasses_t testclasses_;

    void do_list(const testclasses_t::value_type& testclass, bool classvariants, bool tests, bool testvariants=false) const;

public:
    static db_t& instance() { static db_t inst; return inst; }

    void register_testclass(runner_fn fn, const char_t* name, const char_t* args)
    {
        testclasses_[name].first.push_back(testclass_variant_t(args, fn));
    }
    void register_testholder(const char_t* name, AUX::holder_base_t* hold)
    {
        testclasses_[name].second = hold;
    }

    /*! Prints all test classes to standard output. */
    void list_testclasses(bool classvariants=false) const;
    /*! Prints all test cases of all test classes to standard output. Depending on parameters it will either only print the class/test name or names and and all arg combinations. */
    void list_testcases(bool classvariants=false, bool testvariants=false) const;
    /*! Prints all test cases of a specific test class. */
    void list_testcases(const string_t& testclass, bool classvariants=false, bool testvariants=false) const;

    /*! Runs all testcases. */
    void run(); // TODO take filters and other options into account
};

} // namespace test
} // namespace jj


#define JJ_PP_SELECTOR(p1, p2, p3, p4, p5, p6, p7, p8, p9, actual, ...) actual

#define jjM1(p1) p1
#define jjM2(p1,p2) p1##p2
#define jjM3(p1,p2,p3) p1##p2##p3
#define jjM4(p1,p2,p3,p4) p1##p2##p3##p4
#define jjM(...) JJ_PP_SELECTOR(__VA_ARGS__, jjM4, jjM3, jjM2, jjM1)

#define JJ___TEST_CLASS_CALL(name, no, args) \
    static void jjM2(JJ_TEST_CLASS_VARIANT_,no)(){ name inst args; JJ_THIS_TESTCLASS::JJ_TESTCASE_holder_t::instance().run(inst); } \
    static int jjM2(JJ_TEST_CLASS_REGISTRAR_,no)() { jj::test::db_t::instance().register_testclass(&name::jjM2(JJ_TEST_CLASS_VARIANT_,no), #name, #args); return no; }
#define JJ___TEST_CLASS_1CALLS(name, p1) JJ___TEST_CLASS_CALL(name, 1, p1)
#define JJ___TEST_CLASS_2CALLS(name, p1, p2) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2)
#define JJ___TEST_CLASS_3CALLS(name, p1, p2, p3) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3)
#define JJ___TEST_CLASS_4CALLS(name, p1, p2, p3, p4) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4)
#define JJ___TEST_CLASS_5CALLS(name, p1, p2, p3, p4, p5) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5)
#define JJ___TEST_CLASS_6CALLS(name, p1, p2, p3, p4, p5, p6) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6)
#define JJ___TEST_CLASS_7CALLS(name, p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7)
#define JJ___TEST_CLASS_8CALLS(name, p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8)
#define JJ___TEST_CLASS_9CALLS(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CLASS_CALL(name, 1, p1) JJ___TEST_CLASS_CALL(name, 2, p2) JJ___TEST_CLASS_CALL(name, 3, p3) JJ___TEST_CLASS_CALL(name, 4, p4) JJ___TEST_CLASS_CALL(name, 5, p5) JJ___TEST_CLASS_CALL(name, 6, p6) JJ___TEST_CLASS_CALL(name, 7, p7) JJ___TEST_CLASS_CALL(name, 8, p8) JJ___TEST_CLASS_CALL(name, 9, p9)
#define JJ___TEST_CLASS_CALLS(name, ...) JJ_PP_SELECTOR(__VA_ARGS__, JJ___TEST_CLASS_9CALLS, JJ___TEST_CLASS_8CALLS, JJ___TEST_CLASS_7CALLS, JJ___TEST_CLASS_6CALLS, JJ___TEST_CLASS_5CALLS, JJ___TEST_CLASS_4CALLS, JJ___TEST_CLASS_3CALLS, JJ___TEST_CLASS_2CALLS, JJ___TEST_CLASS_1CALLS)(name, __VA_ARGS__)

#define JJ___TEST_CLASS_REG(no) jjM2(JJ_TEST_CLASS_REGISTRAR_,no)();
#define JJ___TEST_CLASS_1REGS(p1) JJ___TEST_CLASS_REG(1)
#define JJ___TEST_CLASS_2REGS(p1, p2) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2)
#define JJ___TEST_CLASS_3REGS(p1, p2, p3) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3)
#define JJ___TEST_CLASS_4REGS(p1, p2, p3, p4) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4)
#define JJ___TEST_CLASS_5REGS(p1, p2, p3, p4, p5) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5)
#define JJ___TEST_CLASS_6REGS(p1, p2, p3, p4, p5, p6) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6)
#define JJ___TEST_CLASS_7REGS(p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7)
#define JJ___TEST_CLASS_8REGS(p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8)
#define JJ___TEST_CLASS_9REGS(p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CLASS_REG(1) JJ___TEST_CLASS_REG(2) JJ___TEST_CLASS_REG(3) JJ___TEST_CLASS_REG(4) JJ___TEST_CLASS_REG(5) JJ___TEST_CLASS_REG(6) JJ___TEST_CLASS_REG(7) JJ___TEST_CLASS_REG(8) JJ___TEST_CLASS_REG(9)
#define JJ___TEST_CLASS_REGS(...) JJ_PP_SELECTOR(__VA_ARGS__, JJ___TEST_CLASS_9REGS, JJ___TEST_CLASS_8REGS, JJ___TEST_CLASS_7REGS, JJ___TEST_CLASS_6REGS, JJ___TEST_CLASS_5REGS, JJ___TEST_CLASS_4REGS, JJ___TEST_CLASS_3REGS, JJ___TEST_CLASS_2REGS, JJ___TEST_CLASS_1REGS)(__VA_ARGS__)

#define JJ___TEST_CASE_DEF(classname, testname) classname::jjM2(registrar_,testname)();
#define JJ___TEST_CASE_1DEFS(classname, p1) JJ___TEST_CASE_DEF(classname, p1)
#define JJ___TEST_CASE_2DEFS(classname, p1, p2) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2)
#define JJ___TEST_CASE_3DEFS(classname, p1, p2, p3) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3)
#define JJ___TEST_CASE_4DEFS(classname, p1, p2, p3, p4) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4)
#define JJ___TEST_CASE_5DEFS(classname, p1, p2, p3, p4, p5) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5)
#define JJ___TEST_CASE_6DEFS(classname, p1, p2, p3, p4, p5, p6) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6)
#define JJ___TEST_CASE_7DEFS(classname, p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7)
#define JJ___TEST_CASE_8DEFS(classname, p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8)
#define JJ___TEST_CASE_9DEFS(classname, p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CASE_DEF(classname, p1) JJ___TEST_CASE_DEF(classname, p2) JJ___TEST_CASE_DEF(classname, p3) JJ___TEST_CASE_DEF(classname, p4) JJ___TEST_CASE_DEF(classname, p5) JJ___TEST_CASE_DEF(classname, p6) JJ___TEST_CASE_DEF(classname, p7) JJ___TEST_CASE_DEF(classname, p8) JJ___TEST_CASE_DEF(classname, p9)
#define JJ___TEST_CASE_DEFS(classname, ...) JJ_PP_SELECTOR(__VA_ARGS__, JJ___TEST_CASE_9DEFS, JJ___TEST_CASE_8DEFS, JJ___TEST_CASE_7DEFS, JJ___TEST_CASE_6DEFS, JJ___TEST_CASE_5DEFS, JJ___TEST_CASE_4DEFS, JJ___TEST_CASE_3DEFS, JJ___TEST_CASE_2DEFS, JJ___TEST_CASE_1DEFS)(classname, __VA_ARGS__)

#define JJ___TEST_CASE_CALL(name, no, args) \
    static void jjM3(runner,no,name)(JJ_THIS_TESTCLASS& inst) { (inst.*&JJ_THIS_TESTCLASS::name) args; }
#define JJ___TEST_CASE_1CALLS(name, p1) JJ___TEST_CASE_CALL(name, 1, p1)
#define JJ___TEST_CASE_2CALLS(name, p1, p2) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2)
#define JJ___TEST_CASE_3CALLS(name, p1, p2, p3) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3)
#define JJ___TEST_CASE_4CALLS(name, p1, p2, p3, p4) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4)
#define JJ___TEST_CASE_5CALLS(name, p1, p2, p3, p4, p5) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5)
#define JJ___TEST_CASE_6CALLS(name, p1, p2, p3, p4, p5, p6) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6)
#define JJ___TEST_CASE_7CALLS(name, p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7)
#define JJ___TEST_CASE_8CALLS(name, p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8)
#define JJ___TEST_CASE_9CALLS(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CASE_CALL(name, 1, p1) JJ___TEST_CASE_CALL(name, 2, p2) JJ___TEST_CASE_CALL(name, 3, p3) JJ___TEST_CASE_CALL(name, 4, p4) JJ___TEST_CASE_CALL(name, 5, p5) JJ___TEST_CASE_CALL(name, 6, p6) JJ___TEST_CASE_CALL(name, 7, p7) JJ___TEST_CASE_CALL(name, 8, p8) JJ___TEST_CASE_CALL(name, 9, p9)
#define JJ___TEST_CASE_CALLS(name, ...) JJ_PP_SELECTOR(__VA_ARGS__, JJ___TEST_CASE_9CALLS, JJ___TEST_CASE_8CALLS, JJ___TEST_CASE_7CALLS, JJ___TEST_CASE_6CALLS, JJ___TEST_CASE_5CALLS, JJ___TEST_CASE_4CALLS, JJ___TEST_CASE_3CALLS, JJ___TEST_CASE_2CALLS, JJ___TEST_CASE_1CALLS)(name, __VA_ARGS__)

#define JJ___TEST_CASE_REG(name, no, args) \
    JJ_THIS_TESTCLASS::JJ_TESTCASE_holder_t::instance().register_testcase(&JJ_THIS_TESTCLASS::jjM3(runner,no,name), #name, #args);
#define JJ___TEST_CASE_1REGS(name, p1) JJ___TEST_CASE_REG(name, 1, p1)
#define JJ___TEST_CASE_2REGS(name, p1, p2) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2)
#define JJ___TEST_CASE_3REGS(name, p1, p2, p3) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3)
#define JJ___TEST_CASE_4REGS(name, p1, p2, p3, p4) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4)
#define JJ___TEST_CASE_5REGS(name, p1, p2, p3, p4, p5) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5)
#define JJ___TEST_CASE_6REGS(name, p1, p2, p3, p4, p5, p6) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6)
#define JJ___TEST_CASE_7REGS(name, p1, p2, p3, p4, p5, p6, p7) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7)
#define JJ___TEST_CASE_8REGS(name, p1, p2, p3, p4, p5, p6, p7, p8) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8)
#define JJ___TEST_CASE_9REGS(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) JJ___TEST_CASE_REG(name, 1, p1) JJ___TEST_CASE_REG(name, 2, p2) JJ___TEST_CASE_REG(name, 3, p3) JJ___TEST_CASE_REG(name, 4, p4) JJ___TEST_CASE_REG(name, 5, p5) JJ___TEST_CASE_REG(name, 6, p6) JJ___TEST_CASE_REG(name, 7, p7) JJ___TEST_CASE_REG(name, 8, p8) JJ___TEST_CASE_REG(name, 9, p9)
#define JJ___TEST_CASE_REGS(name, ...) JJ_PP_SELECTOR(__VA_ARGS__, JJ___TEST_CASE_9REGS, JJ___TEST_CASE_8REGS, JJ___TEST_CASE_7REGS, JJ___TEST_CASE_6REGS, JJ___TEST_CASE_5REGS, JJ___TEST_CASE_4REGS, JJ___TEST_CASE_3REGS, JJ___TEST_CASE_2REGS, JJ___TEST_CASE_1REGS)(name, __VA_ARGS__)


/*! Opens a test class, inside it JJ_TEST_CASE shall be defined (and any other definitions that are required).
Note that internally this starts a struct (ie. something like struct name ... { */
#define JJ_TEST_CLASS(name) JJ_TEST_CLASS_VARIANTS(name,)
/*! Opens a test class. You are responsible to define all constructors to suit all the variants!
For all other see description of JJ_TEST_CLASS. */
#define JJ_TEST_CLASS_VARIANTS(name, ...) \
     struct name : public jj::test::AUX::testclass_base_T<name> \
     { \
         typedef name JJ_THIS_TESTCLASS; \
         const char* JJ_TEST_CLASS_get_name() const { return #name; } \
         JJ___TEST_CLASS_CALLS(name, __VA_ARGS__) \
         static void JJ_TEST_CLASS_register() { \
             JJ___TEST_CLASS_REGS(__VA_ARGS__) \
         }
/*! Finishes the defition of a testclass without defining the global variables. (Can be used in header.) */
#define JJ_TEST_CLASS_END_NODEF(name) \
     };
/*! Defines the global variables associated with testclass. (Shall be used in source file if JJ_TEST_CLASS_END_NODEF used in header.) */
#define JJ_TEST_CLASS_DEF(name, ...) \
     struct jjM2(name,__DEF) \
     { \
         jjM2(name,__DEF)() { \
             name::JJ_TEST_CLASS_register(); \
             jj::test::db_t::instance().register_testholder(#name, &name::JJ_TESTCASE_holder_t::instance()); \
             JJ___TEST_CASE_DEFS(name, __VA_ARGS__) \
         } \
     }; \
     jjM2(name,__DEF) jjM3(g_,name,__DEF);
/*! Finishes the definition of a testclass along with defining all the associated global variables. */
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

#endif // JJ_TEST_H
