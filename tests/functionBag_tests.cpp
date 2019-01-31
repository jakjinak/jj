#include "jj/functionBag.h"
#include "jj/test/test.h"

JJ_TEST_CLASS(functionBagTests_t)

struct custom {};

static std::list<jj::string_t> calls;

static void reset()
{
    calls.clear();
}

static void function0()
{
    calls.push_back(jjT("static0"));
}

static void functionABC(int A, size_t B, const jj::string_t& C)
{
    calls.push_back(jjT("staticABC"));
}

static jj::string_t function_AB(double A, const custom& B)
{
    calls.push_back(jjT("static_AB"));
    return jjT("static");
}

struct functor
{
    void operator()()
    {
        calls.push_back(jjT("functor0"));
    }
    void operator()(int A, size_t B, const jj::string_t& C)
    {
        calls.push_back(jjT("functorABC"));
    }
    jj::string_t operator()(double A, const custom& B)
    {
        calls.push_back(jjT("functor_AB"));
        return jjT("functor");
    }
};

struct something
{
    void member0()
    {
        calls.push_back(jjT("member0"));
    }
    void memberABC(int A, size_t B, const jj::string_t& C)
    {
        calls.push_back(jjT("memberABC"));
    }
    jj::string_t member_AB(double A, const custom& B)
    {
        calls.push_back(jjT("member_AB"));
        return jjT("member");
    }

};

void test(const std::initializer_list<jj::string_t>& c)
{
    auto callsi = calls.begin();
    auto ci = c.begin();
    for (; callsi != calls.end() && ci != c.end(); ++callsi, ++ci)
    {
        JJ_TEST(*callsi == *ci, jjOOO(*callsi, == , *ci));
    }
    JJ_TEST(callsi == calls.end() && ci == c.end(), jjT("Unexpected count of calls"));
}

JJ_TEST_CASE(empty_called_doesnothing)
{
    reset();
    jj::functionBag_t<void> fb0;
    fb0();
    jj::functionBag_t<void, int, size_t, const jj::string_t&> fbABC;
    fbABC(1, 9999u, jjT("XXX"));
    jj::functionBag_t<jj::string_t, double, const custom&> fb_AB;
    auto ret = fb_AB(123.456l, custom());
    test({});
}

JJ_TEST_CASE(multipleadded_called_callseachonce)
{
    // note: this test also tests manual specification of template arguments
    reset();
    something X;
    jj::functionBag_t<void> fb0;
    fb0.add<void()>(&functionBagTests_t::function0);
    fb0.add<functor>(functor());
    fb0.add<something, void(something::*)()>(X, &something::member0);
    fb0.add([] { calls.push_back(jjT("lambda0")); });
    fb0();
    test({ jjT("static0"), jjT("functor0"), jjT("member0"), jjT("lambda0") });
    reset();
    jj::functionBag_t<void, int, size_t, const jj::string_t&> fbABC;
    fbABC.add<void(int, size_t, const jj::string_t&)>(&functionBagTests_t::functionABC);
    fbABC.add<functor>(functor());
    fbABC.add<something, void(something::*)(int, size_t, const jj::string_t&)>(X, &something::memberABC);
    fbABC.add([](int, size_t, const jj::string_t&) { calls.push_back(jjT("lambdaABC")); });
    fbABC(1, 9999u, jjT("XXX"));
    test({ jjT("staticABC"), jjT("functorABC"), jjT("memberABC"), jjT("lambdaABC") });
    reset();
    jj::functionBag_t<jj::string_t, double, const custom&> fb_AB;
    fb_AB.add<jj::string_t(double, const custom&)>(&functionBagTests_t::function_AB);
    fb_AB.add<functor>(functor());
    fb_AB.add<something, jj::string_t(something::*)(double, const custom&)>(X, &something::member_AB);
    fb_AB.add([](double, const custom&) { calls.push_back(jjT("lambda_AB")); return jjT("lambda_AB"); });
    auto ret = fb_AB(123.456l, custom());
    test({ jjT("static_AB"), jjT("functor_AB"), jjT("member_AB"), jjT("lambda_AB") });
}

JJ_TEST_CASE(add_eachreceivesuniqueid)
{
    reset();
    jj::functionBag_t<void> fb;
    int id1 = fb.add(&functionBagTests_t::function0);
    int id2 = fb.add(functor());
    int id3 = fb.add(&functionBagTests_t::function0);
    int id4 = fb.add(functor());
    JJ_TEST(id1 != id2 && id1 != id3 && id1 != id4);
    JJ_TEST(id2 != id3 && id2 != id4);
    JJ_TEST(id3 != id4);
    fb();
    test({ jjT("static0"), jjT("functor0"), jjT("static0"), jjT("functor0") });
}

JJ_TEST_CASE(addid_updatesoradds)
{
    reset();
    jj::functionBag_t<void> fb;
    int id1 = fb.add(999, &functionBagTests_t::function0); // adds anyway
    fb();
    test({ jjT("static0") });
    reset();
    int id2 = fb.add(id1, [] {calls.push_back(jjT("lambda0")); }); // updates
    JJ_TEST(id2 == id1);
    fb();
    test({ jjT("lambda0") });
    reset();
    int id3 = fb.add(999, functor()); // adds another one
    JJ_TEST(id3 != id2);
    fb();
    test({ jjT("lambda0"), jjT("functor0") });
}

JJ_TEST_CASE(update_onlyupdates)
{
    reset();
    jj::functionBag_t<void> fb;
    int id1 = fb.update(999, &functionBagTests_t::function0);
    JJ_TEST(id1 == -1);
    fb();
    test({});
    reset();
    id1 = fb.add(&functionBagTests_t::function0);
    int id2 = fb.update(999, [] {calls.push_back(jjT("lambda0")); }); // does still nothing 
    JJ_TEST(id2 != id1);
    fb();
    test({ jjT("static0") });
    reset();
    int id3 = fb.update(id1, functor()); // updates
    JJ_TEST(id3 == id1);
    fb();
    test({ jjT("functor0") });
}

JJ_TEST_CASE(remove_removesexistingonly)
{
    reset();
    jj::functionBag_t<void> fb;
    JJ_TEST(!fb.remove(999));
    int id1 = fb.add(&functionBagTests_t::function0);
    int id2 = fb.add(functor());
    fb();
    test({ jjT("static0"), jjT("functor0") });
    reset();
    JJ_TEST(!fb.remove(999));
    JJ_TEST(fb.remove(id1));
    JJ_TEST(!fb.remove(id1));
    fb();
    test({ jjT("functor0") });
    reset();
    JJ_TEST(fb.remove(id2));
    fb();
    test({});
}

JJ_TEST_CASE(firstlast_callbackscalled)
{
    reset();
    jj::functionBag_t<void> fb;
    jj::AUX::setup_first_last_callbacks(fb, [] { calls.push_back(jjT("first")); }, [] {calls.push_back(jjT("last")); });
    fb();
    test({});
    int id1 = fb.add(&functionBagTests_t::function0);
    test({ jjT("first") });
    fb();
    test({ jjT("first"), jjT("static0") });
    reset();
    int id2 = fb.add(&functionBagTests_t::function0);
    test({});
    fb();
    test({ jjT("static0"), jjT("static0") });
    reset();
    fb.remove(id2);
    fb();
    test({ jjT("static0") });
    reset();
    fb.remove(id1);
    test({ jjT("last") });
    fb();
    test({ jjT("last") });
}

JJ_TEST_CASE(invoke_returnsvectorofvalues_inorder)
{
    reset();
    jj::functionBag_t<int> fb;
    fb.add([] { return 0; });
    fb.add([] { return 1; });
    fb.add([] { return 2; });
    fb.add([] { return 3; });
    auto ret = fb();
    JJ_ENSURE(ret.size() == 4);
    JJ_TEST(ret[0] == 0);
    JJ_TEST(ret[1] == 1);
    JJ_TEST(ret[2] == 2);
    JJ_TEST(ret[3] == 3);
}

JJ_TEST_CASE(callindividual_stopsonfalse)
{
    reset();
    jj::functionBag_t<int> fb;
    fb.add([] { return 0; });
    fb.add([] { return 1; });
    fb.add([] { return 2; });
    fb.add([] { return 3; });
    size_t cnt = 0;
    fb.call_individual([&cnt](int r) { ++cnt; return r != 2; });
    JJ_TEST(cnt == 3);
}

JJ_TEST_CLASS_END(functionBagTests_t, empty_called_doesnothing, multipleadded_called_callseachonce, \
    add_eachreceivesuniqueid, addid_updatesoradds, update_onlyupdates, remove_removesexistingonly,\
    firstlast_callbackscalled, invoke_returnsvectorofvalues_inorder, callindividual_stopsonfalse)

std::list<jj::string_t> functionBagTests_t::calls;
