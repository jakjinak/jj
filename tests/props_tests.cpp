#include "jj/props.h"
#include "jj/propsTextSerializer.h"
#include "jj/propsTextDeserializer.h"
#include "jj/source.h"
#include "jj/test/test.h"
#include <limits>

struct color_t
{
    unsigned char R, G, B;

    color_t() : R(0), G(0), B(0) {}
    color_t(unsigned char r, unsigned char g, unsigned char b) : R(r), G(g), B(b) {}
};

template<typename S>
jj::props::textSerializer_t<S> operator<<(jj::props::textSerializer_t<S>& s, const color_t& c)
{
    s.stream() << jjT('{') << static_cast<unsigned int>(c.R) << jjT(',') << static_cast<unsigned int>(c.G) << jjT(',') << static_cast<unsigned int>(c.B) << jjT('}');
    return s;
}

template<typename S, typename P>
jj::props::textDeserializer_t<S, P>& operator>>(jj::props::textDeserializer_t<S, P>& s, color_t& v)
{
    S& src = s.source();
    v = color_t();
    typedef typename S::char_type ch_t;
    ch_t ch;
    int tmp;
    if (!src.get(ch) || ch != jjT('{'))
        throw jj::exception::base("Expected {");
    src.stream() >> tmp;
    if (tmp < 0 || tmp>255)
        throw jj::exception::base("Out of range");
    v.R = static_cast<unsigned char>(tmp);
    if (!src.get(ch) || ch != jjT(','))
        throw jj::exception::base("Expected ,");
    src.stream() >> tmp;
    if (tmp < 0 || tmp>255)
        throw jj::exception::base("Out of range");
    v.G = static_cast<unsigned char>(tmp);
    if (!src.get(ch) || ch != jjT(','))
        throw jj::exception::base("Expected ,");
    src.stream() >> tmp;
    if (tmp < 0 || tmp>255)
        throw jj::exception::base("Out of range");
    v.B = static_cast<unsigned char>(tmp);
    if (!src.get(ch) || ch != jjT('}'))
        throw jj::exception::base("Expected }");
    return s;
}

using namespace jj::props;

typedef props<setup::cstrkey_t<jj::char_t>, int, bool, jj::string_t, double, std::list<jj::string_t>, std::list<int>, color_t> myprops;

struct LEAF : myprops
{
    color_t fore, back;
    LEAF()
        : fore(192, 0, 0), back()
    {
        addProp(jjT("fore"), fore);
        addProp(jjT("back"), back);
    }
};

struct NESTED : myprops
{
    LEAF colors;
    std::list<int> numbers;
    double precise;
    NESTED()
        : numbers({ 3, 14, 15 }), precise(-22.853)
    {
        addNested(jjT("colors"), colors);
        addProp(jjT("numbers"), numbers);
        addProp(jjT("precise"), precise);
    }
};

struct MAIN : myprops
{
    int num1, num2, num3;
    bool flag;
    jj::string_t text;
    std::list<jj::string_t> words;

    NESTED spec1, spec2;

    MAIN()
        : num1(-1), num2(-2), num3(-3), flag(true), text(jjT("text")), words({ jjT("some"), jjT("random"), jjT("words") })
    {
        addProp(jjT("num1"), num1);
        addProp(jjT("num2"), num2);
        addProp(jjT("num3"), num3);
        addProp(jjT("flag"), flag);
        addProp(jjT("text"), text);
        addProp(jjT("words"), words);
        addNested(jjT("1"), spec1);
        addNested(jjT("2"), spec2);
    }
};

struct ApplyKeyPROPS : public myprops
{
    int num0, numA, numB;
    double flA;
    bool flag;
    std::list<jj::string_t> lsA;

    ApplyKeyPROPS() : num0(0), numA(5), numB(6), flA(33.33), flag(true), lsA({ jjT("alpha"), jjT("beta") })
    {
        addProp(jjT("0"), num0);
        addProp(jjT("A"), numA);
        addProp(jjT("B"), numB);
        addProp(jjT("A"), flA);
        addProp(jjT("flag"), flag);
        addProp(jjT("A"), lsA);
    }
};

JJ_TEST_CLASS(propsTests_t)

JJ_TEST_CASE(justinitandgetandset_doesnotthrow)
{
    MAIN ps;
    JJ_TEST(ps.get<int>(jjT("num1")) == -1);
    ps.num1 = -53;
    JJ_TEST(ps.get<int>(jjT("num1")) == -53);
    JJ_TEST(ps.get<jj::string_t>(jjT("text")) == jjT("text"));
    ps.set<jj::string_t>(jjT("text"), jjT("ABC"));
    JJ_TEST(ps.get<jj::string_t>(jjT("text")) == jjT("ABC"));

    JJ_TEST(ps.getNested(jjT("2")).getNested(jjT("colors")).get<color_t>(jjT("fore")).R == 192);
    JJ_TEST(ps.findNested(jjT("1")) == &ps.spec1);
}

JJ_TEST_CASE(get_doesnotexist_throws)
{
    myprops ps;
    JJ_TEST_THAT_THROWS(ps.get<int>(jjT("Invalid")), jj::props::exception::keyNotFound);
    JJ_TEST_THAT_THROWS(ps.get<jj::string_t>(jjT("Invalid")), jj::props::exception::keyNotFound);
    JJ_TEST_THAT_THROWS(ps.get<std::list<jj::string_t>>(jjT("Invalid")), jj::props::exception::keyNotFound);
    JJ_TEST_THAT_THROWS(ps.get<color_t>(jjT("Invalid")), jj::props::exception::keyNotFound);
    JJ_TEST_THAT_THROWS(ps.getNested(jjT("Invalid")), jj::props::exception::keyNotFound);
    const myprops& cps = ps;
    JJ_TEST_THAT_THROWS(cps.get<int>(jjT("Invalid")), jj::props::exception::keyNotFound);
    JJ_TEST_THAT_THROWS(cps.get<jj::string_t>(jjT("Invalid")), jj::props::exception::keyNotFound);
    JJ_TEST_THAT_THROWS(cps.get<std::list<jj::string_t>>(jjT("Invalid")), jj::props::exception::keyNotFound);
    JJ_TEST_THAT_THROWS(cps.get<color_t>(jjT("Invalid")), jj::props::exception::keyNotFound);
    JJ_TEST_THAT_THROWS(cps.getNested(jjT("Invalid")), jj::props::exception::keyNotFound);
}

JJ_TEST_CASE(find_doesnotexist_returnsnull)
{
    myprops ps;
    JJ_TEST(ps.find<int>(jjT("Invalid")) == nullptr);
    JJ_TEST(ps.find<jj::string_t>(jjT("Invalid")) == nullptr);
    JJ_TEST(ps.find<std::list<jj::string_t>>(jjT("Invalid")) == nullptr);
    JJ_TEST(ps.find<color_t>(jjT("Invalid")) == nullptr);
    JJ_TEST(ps.findNested(jjT("Invalid")) == nullptr);
    const myprops& cps = ps;
    JJ_TEST(cps.find<int>(jjT("Invalid")) == nullptr);
    JJ_TEST(cps.find<jj::string_t>(jjT("Invalid")) == nullptr);
    JJ_TEST(cps.find<std::list<jj::string_t>>(jjT("Invalid")) == nullptr);
    JJ_TEST(cps.find<color_t>(jjT("Invalid")) == nullptr);
    JJ_TEST(cps.findNested(jjT("Invalid")) == nullptr);
}

JJ_TEST_CASE(addsametwice_throws)
{
    struct PROPS : myprops
    {
        int A, B;
        PROPS() : A(), B()
        {
            addProp(jjT("A"), A);
            addProp(jjT("A"), B);
        }
    };
    JJ_TEST_THAT_THROWS(PROPS ps, jj::props::exception::duplicateKey);
}

JJ_TEST_CASE(addsametwice_butdifferenttype_doesnotthrow)
{
    struct PROPZ : myprops
    {
        double X;

        PROPZ() : X(33.33)
        {
            addProp(jjT("X"), X);
        }
    };
    struct PROPS : myprops
    {
        int A;
        jj::string_t S;
        PROPZ Z;

        PROPS() : A(5), S(jjT("X"))
        {
            addProp(jjT("A"), A);
            addProp(jjT("A"), S);
            addNested(jjT("A"), Z);
        }
    };
    PROPS ps;
    JJ_TEST(ps.get<int>(jjT("A")) == 5);
    JJ_TEST(ps.get<jj::string_t>(jjT("A")) == jjT("X"));
    JJ_TEST(ps.getNested(jjT("A")).get<double>(jjT("X")) == 33.33);
}

JJ_TEST_CASE(addsamenestedtwice_throws)
{
    struct NESTED : myprops
    {
        double X;

        NESTED() : X(33.33)
        {
            addProp(jjT("X"), X);
        }
    };
    struct PROPS : myprops
    {
        NESTED X, Y;

        PROPS()
        {
            addNested(jjT("X"), X);
            addNested(jjT("X"), Y);
        }
    };
    JJ_TEST_THAT_THROWS(PROPS ps, jj::props::exception::duplicateKey);
}

JJ_TEST_CASE(addsametwice_differentcase_doesnotthrow)
{
    typedef props<setup::cstrkey_t<jj::char_t>, int, jj::string_t> theprops;
    struct PROPS : theprops
    {
        int ABC, Abc, abc;
        PROPS() : ABC(1), Abc(2), abc(3)
        {
            addProp(jjT("ABC"), ABC);
            addProp(jjT("Abc"), Abc);
            addProp(jjT("abc"), abc);
        }
    };
    PROPS ps;
    JJ_TEST(ps.get<int>(jjT("ABC")) == 1);
    JJ_TEST(ps.get<int>(jjT("Abc")) == 2);
    JJ_TEST(ps.get<int>(jjT("abc")) == 3);
}

JJ_TEST_CASE(addsametwiceicase_differentcase_throws)
{
    typedef props<setup::icstrkey_t<jj::char_t>, int, jj::string_t> theprops;
    struct PROPS : theprops
    {
        int ABC, Abc, abc;
        PROPS() : ABC(1), Abc(2), abc(3)
        {
            addProp(jjT("ABC"), ABC);
            addProp(jjT("Abc"), Abc);
            addProp(jjT("abc"), abc);
        }
    };
    JJ_TEST_THAT_THROWS(PROPS ps, jj::props::exception::duplicateKey);
}

JJ_TEST_CASE(addsametwice_differentcaseandtype_doesnotthrow)
{
    typedef props<setup::cstrkey_t<jj::char_t>, int, jj::string_t> theprops;
    struct PROPS : theprops
    {
        int ABC, A;
        jj::string_t abc;
        PROPS() : ABC(1), A(2), abc(jjT("3"))
        {
            addProp(jjT("ABC"), ABC);
            addProp(jjT("A"), A);
            addProp(jjT("abc"), abc);
        }
    };
    PROPS ps;
    JJ_TEST(ps.get<int>(jjT("ABC")) == 1);
    JJ_TEST(ps.get<int>(jjT("A")) == 2);
    JJ_TEST(ps.get<jj::string_t>(jjT("abc")) == jjT("3"));
}

struct visitor
{
    size_t down;
    int balance;
    std::list<jj::string_t> nodes;

    visitor() : down(0), balance(0) {}

    template<typename CTX, typename K, typename T>
    void onValue(CTX& ctx, K key, T& v)
    {
        nodes.push_back(key);
    }
    template<typename CTX>
    void onNestedBegin(CTX& ctx)
    {
        ++down;
        ++balance;
        nodes.push_back(ctx.Dive.back());
    }
    template<typename CTX>
    void onNestedEnd(CTX& ctx)
    {
        --balance;
    }
};

JJ_TEST_CASE(traverse_goesinorder)
{
    MAIN ps;
    visitor v;
    traversalContext_t<const jj::char_t*> ctx;
    ps.traverse(v, ctx);
    std::list<jj::string_t> expected = { jjT("num1"), jjT("num2"), jjT("num3"), jjT("flag"), jjT("text"), jjT("words"), jjT("1"), jjT("precise"), jjT("numbers"), jjT("colors"), jjT("back"), jjT("fore"), jjT("2"), jjT("precise"), jjT("numbers"), jjT("colors"), jjT("back"), jjT("fore") };

    JJ_TEST(v.down == 4);
    JJ_TEST(v.balance == 0);
    {
        JJ_ENSURE(v.nodes.size() == expected.size());
        auto ai = v.nodes.begin();
        auto ei = expected.begin();
        for (; ai != v.nodes.end() && ei != expected.end(); ++ai, ++ei)
        {
            JJ_TEST(*ai == *ei, jjT('"') << *ai << jjT("\" == \"") << *ei << jjT('"'));
        }
    }

    const MAIN& cps = ps;
    v.down = 0;
    v.balance = 0;
    v.nodes.clear();
    ps.traverse(v, ctx);

    JJ_TEST(v.down == 4);
    JJ_TEST(v.balance == 0);
    {
        JJ_ENSURE(v.nodes.size() == expected.size());
        auto ai = v.nodes.begin();
        auto ei = expected.begin();
        for (; ai != v.nodes.end() && ei != expected.end(); ++ai, ++ei)
        {
            JJ_TEST(*ai == *ei, jjT('"') << *ai << jjT("\" == \"") << *ei << jjT('"'));
        }
    }
}

JJ_TEST_CASE(apply_listsalltypes)
{
    myprops ps;
    struct ACTION
    {
        std::list<jj::string_t> types;
        void operator()(aux::holder_t<setup::cstrkey_t<jj::char_t>, int>&) { types.push_back(jjT("int")); }
        void operator()(aux::holder_t<setup::cstrkey_t<jj::char_t>, bool>&) { types.push_back(jjT("bool")); }
        void operator()(aux::holder_t<setup::cstrkey_t<jj::char_t>, jj::string_t>&) { types.push_back(jjT("string")); }
        void operator()(aux::holder_t<setup::cstrkey_t<jj::char_t>, double>&) { types.push_back(jjT("double")); }
        void operator()(aux::holder_t<setup::cstrkey_t<jj::char_t>, std::list<jj::string_t>>&) { types.push_back(jjT("list<string>")); }
        void operator()(aux::holder_t<setup::cstrkey_t<jj::char_t>, std::list<int>>&) { types.push_back(jjT("list<int>")); }
        void operator()(aux::holder_t<setup::cstrkey_t<jj::char_t>, color_t>&) { types.push_back(jjT("color")); }
    };
    ACTION a;
    ps.apply(a);
    std::list<jj::string_t> expected = { jjT("int"), jjT("bool"), jjT("string"), jjT("double"), jjT("list<string>"), jjT("list<int>"), jjT("color") };
    JJ_ENSURE(a.types.size() == expected.size());
    auto ai = a.types.begin();
    auto ei = expected.begin();
    for (; ai != a.types.end() && ei != expected.end(); ++ai, ++ei)
    {
        JJ_TEST(*ai == *ei, jjT('"') << *ai << jjT("\" == \"") << *ei << jjT('"'));
    }
}

JJ_TEST_CASE(applyc_listsalltypes)
{
    const myprops ps;
    struct ACTION
    {
        std::list<jj::string_t> types;
        void operator()(const aux::holder_t<setup::cstrkey_t<jj::char_t>, int>&) { types.push_back(jjT("int")); }
        void operator()(const aux::holder_t<setup::cstrkey_t<jj::char_t>, bool>&) { types.push_back(jjT("bool")); }
        void operator()(const aux::holder_t<setup::cstrkey_t<jj::char_t>, jj::string_t>&) { types.push_back(jjT("string")); }
        void operator()(const aux::holder_t<setup::cstrkey_t<jj::char_t>, double>&) { types.push_back(jjT("double")); }
        void operator()(const aux::holder_t<setup::cstrkey_t<jj::char_t>, std::list<jj::string_t>>&) { types.push_back(jjT("list<string>")); }
        void operator()(const aux::holder_t<setup::cstrkey_t<jj::char_t>, std::list<int>>&) { types.push_back(jjT("list<int>")); }
        void operator()(const aux::holder_t<setup::cstrkey_t<jj::char_t>, color_t>&) { types.push_back(jjT("color")); }
    };
    ACTION a;
    ps.apply(a);
    std::list<jj::string_t> expected = { jjT("int"), jjT("bool"), jjT("string"), jjT("double"), jjT("list<string>"), jjT("list<int>"), jjT("color") };
    JJ_ENSURE(a.types.size() == expected.size());
    auto ai = a.types.begin();
    auto ei = expected.begin();
    for (; ai != a.types.end() && ei != expected.end(); ++ai, ++ei)
    {
        JJ_TEST(*ai == *ei, jjT('"') << *ai << jjT("\" == \"") << *ei << jjT('"'));
    }
}

JJ_TEST_CASE(applykey_listexistingkeys)
{
    struct ACTION
    {
        std::list<jj::string_t> encounter;
        void operator()(int& v) { encounter.push_back(jjS(jjT("int/") << v)); }
        void operator()(bool& v) { encounter.push_back(jjS(jjT("bool/") << v)); }
        void operator()(jj::string_t& v) { encounter.push_back(jjS(jjT("string/") << v)); }
        void operator()(double& v) { encounter.push_back(jjS(jjT("double/") << v)); }
        void operator()(std::list<jj::string_t>& v) { encounter.push_back(jjS(jjT("list<string>/") << v.size())); }
        void operator()(std::list<int>& v) { encounter.push_back(jjS(jjT("list<int>/") << v.size())); }
        void operator()(color_t&) { encounter.push_back(jjT("color")); }
    };
    ACTION a;
    ApplyKeyPROPS ps;
    ps.apply(a, jjT("A"));

    std::list<jj::string_t> expected = { jjT("int/5"), jjT("double/33.33"), jjT("list<string>/2") };
    JJ_ENSURE(a.encounter.size() == expected.size());
    auto ai = a.encounter.begin();
    auto ei = expected.begin();
    for (; ai != a.encounter.end() && ei != expected.end(); ++ai, ++ei)
    {
        JJ_TEST(*ai == *ei, jjT('"') << *ai << jjT("\" == \"") << *ei << jjT('"'));
    }
}

JJ_TEST_CASE(applykeyc_listexistingkeys)
{
    struct ACTION
    {
        std::list<jj::string_t> encounter;
        void operator()(const int& v) { encounter.push_back(jjS(jjT("int/") << v)); }
        void operator()(const bool& v) { encounter.push_back(jjS(jjT("bool/") << v)); }
        void operator()(const jj::string_t& v) { encounter.push_back(jjS(jjT("string/") << v)); }
        void operator()(const double& v) { encounter.push_back(jjS(jjT("double/") << v)); }
        void operator()(const std::list<jj::string_t>& v) { encounter.push_back(jjS(jjT("list<string>/") << v.size())); }
        void operator()(const std::list<int>& v) { encounter.push_back(jjS(jjT("list<int>/") << v.size())); }
        void operator()(const color_t&) { encounter.push_back(jjT("color")); }
    };
    ACTION a;
    const ApplyKeyPROPS ps;
    ps.apply(a, jjT("A"));

    std::list<jj::string_t> expected = { jjT("int/5"), jjT("double/33.33"), jjT("list<string>/2") };
    JJ_ENSURE(a.encounter.size() == expected.size());
    auto ai = a.encounter.begin();
    auto ei = expected.begin();
    for (; ai != a.encounter.end() && ei != expected.end(); ++ai, ++ei)
    {
        JJ_TEST(*ai == *ei, jjT('"') << *ai << jjT("\" == \"") << *ei << jjT('"'));
    }
}

JJ_TEST_CLASS_END(propsTests_t, justinitandgetandset_doesnotthrow, get_doesnotexist_throws, find_doesnotexist_returnsnull, \
    addsametwice_throws, addsametwice_butdifferenttype_doesnotthrow, addsamenestedtwice_throws, \
    addsametwice_differentcase_doesnotthrow, addsametwiceicase_differentcase_throws, addsametwice_differentcaseandtype_doesnotthrow, \
    traverse_goesinorder, apply_listsalltypes, applyc_listsalltypes, applykey_listexistingkeys, applykeyc_listexistingkeys)

JJ_TEST_CLASS(propsPathTests_t)

JJ_TEST_CASE(basic_getset)
{
    MAIN ps;
    pathWalker_t<myprops> walk(ps);
    int& num2 = walk.get<int>(jjT("num2"));
    JJ_TEST(num2 == -2);
    walk.set<int>(jjT("num2"), 2);
    JJ_TEST(num2 == 2);
    jj::string_t& txt = walk.get<jj::string_t>(jjT("text"));
    JJ_TEST(txt == jjT("text"));
    walk.set<jj::string_t>(jjT("text"), jjT("change"));
    JJ_TEST(txt == jjT("change"));

    const MAIN& cps = ps;
    pathWalker_t<const myprops> cwalk(cps);
    const int& cnum2 = cwalk.get<int>(jjT("num2"));
    JJ_TEST(num2 == 2);
    const jj::string_t& ctxt = cwalk.get<jj::string_t>(jjT("text"));
    JJ_TEST(ctxt == jjT("change"));
}

JJ_TEST_CASE(path_getset)
{
    MAIN ps;
    pathWalker_t<myprops> walk(ps);
    double& num1 = walk.get<double>(jjT("1/precise"));
    double& num2 = walk.get<double>(jjT("2/precise"));
    JJ_TEST(num1 == -22.853);
    JJ_TEST(num2 == -22.853);
    walk.set<double>(jjT("2/precise"), 5.25);
    JJ_TEST(num1 == -22.853);
    JJ_TEST(num2 == 5.25);

    color_t& f1 = walk.get<color_t>(jjT("1/colors/fore"));
    JJ_TEST(f1.R == 192);
    f1.R = 128;
    JJ_TEST(walk.get<color_t>(jjT("1/colors/fore")).R == 128);
}

JJ_TEST_CASE(path_apply)
{
    struct A : myprops
    {
        ApplyKeyPROPS X1, X2;
        double d;

        A() : d(-1)
        {
            addProp(jjT("d"), d);
            addNested(jjT("X1"), X1);
            addNested(jjT("X2"), X2);
        }
    };
    struct B : myprops
    {
        A a;
        B()
        {
            addNested(jjT("a"), a);
        }
    };
    struct C : myprops
    {
        int a, b, c;
        jj::string_t t1, t2;
        B b1, b2;

        C() : a(1), b(2), c(3), t1(jjT("A")), t2(jjT("B"))
        {
            addProp(jjT("a"), a);
            addProp(jjT("b"), b);
            addProp(jjT("c"), c);
            addProp(jjT("t1"), t1);
            addProp(jjT("t2"), t2);
            addNested(jjT("1"), b1);
            addNested(jjT("2"), b2);
        }
    };
    struct ACTION
    {
        std::list<jj::string_t> encounter;
        void operator()(int& v) { encounter.push_back(jjS(jjT("int/") << v)); }
        void operator()(bool& v) { encounter.push_back(jjS(jjT("bool/") << v)); }
        void operator()(jj::string_t& v) { encounter.push_back(jjS(jjT("string/") << v)); }
        void operator()(double& v) { encounter.push_back(jjS(jjT("double/") << v)); }
        void operator()(std::list<jj::string_t>& v) { encounter.push_back(jjS(jjT("list<string>/") << v.size())); }
        void operator()(std::list<int>& v) { encounter.push_back(jjS(jjT("list<int>/") << v.size())); }
        void operator()(color_t&) { encounter.push_back(jjT("color")); }
    };
    ACTION a;
    C ps;
    pathWalker_t<myprops> walk(ps);
    walk.apply(a, jjT("1/a/X2/A"));

    std::list<jj::string_t> expected = { jjT("int/5"), jjT("double/33.33"), jjT("list<string>/2") };
    JJ_ENSURE(a.encounter.size() == expected.size());
    auto ai = a.encounter.begin();
    auto ei = expected.begin();
    for (; ai != a.encounter.end() && ei != expected.end(); ++ai, ++ei)
    {
        JJ_TEST(*ai == *ei, jjT('"') << *ai << jjT("\" == \"") << *ei << jjT('"'));
    }
}

JJ_TEST_CASE(path_differentseparator1)
{
    MAIN ps;
    pathWalker_t<myprops, aux::pathIterator_t<jj::char_t, jjT(' ')>> walk(ps);
    color_t& f2 = walk.get<color_t>(jjT("2 colors fore"));
    JJ_TEST(f2.R == 192);
    walk.set<color_t>(jjT("2 colors fore"), color_t(128, 128, 128));
    JJ_TEST(f2.R == 128);
}

JJ_TEST_CASE(path_differentseparator2)
{
    MAIN ps;
    pathWalker_t<myprops, aux::pathIterator_t<jj::char_t, jjT(':')>> walk(ps);
    color_t& f2 = walk.get<color_t>(jjT("2:colors:fore"));
    JJ_TEST(f2.R == 192);
    walk.set<color_t>(jjT("2:colors:fore"), color_t(128, 128, 128));
    JJ_TEST(f2.R == 128);
}

JJ_TEST_CLASS_END(propsPathTests_t, basic_getset, path_getset, path_apply, path_differentseparator1, path_differentseparator2)


JJ_TEST_CLASS(propsTSerDeserTests_t)

JJ_TEST_CASE(basic)
{
    MAIN ps;
    ps.num1 = 101;
    ps.num2 = std::numeric_limits<int>::min();
    ps.num3 = std::numeric_limits<int>::max();
    ps.flag = true;
    ps.text = jjT("\"\t\xC5\xBE'ou{{[(     )\\=]}\xD0\xA9+132");
    ps.words = { jjT("some '"), jjT(" random \""), jjT(""), jjT("\t\ngarbage") };
    ps.spec1.numbers = { 0,1,1,2,3,5 };
    ps.spec1.colors.fore.R = 255;
    ps.spec1.colors.fore.G = 255;
    ps.spec2.precise = 1.618;
    ps.spec2.colors.back.B = 63;
    ps.spec2.colors.back.G = 63;

    jj::osstream_t str1;
    textSerializer_t<jj::osstream_t> ser1(str1, 2, jjT(' '));
    traversalContext_t<const jj::char_t*> ctx;
    ps.traverse(ser1, ctx);

    jj::isstream_t str2;
    str2.str(str1.str());
    jj::streamSource_t<jj::isstream_t> ssrc(str2);
    MAIN ps2;
    textDeserializer_t<jj::streamSource_t<jj::isstream_t>, myprops> des(ssrc, ps2);

    JJ_TEST(ps2.num1 == 101);
    JJ_TEST(ps2.num2 == std::numeric_limits<int>::min());
    JJ_TEST(ps2.num3 == std::numeric_limits<int>::max());
    JJ_TEST(ps2.flag == true);
    JJ_TEST(ps2.text == jjT("\"\t\xC5\xBE'ou{{[(     )\\=]}\xD0\xA9+132"));
    JJ_TEST(ps2.spec1.colors.fore.R == 255);
    JJ_TEST(ps2.spec1.colors.fore.G == 255);
    JJ_TEST(ps2.spec1.colors.fore.B == 0);
    JJ_TEST(ps2.spec2.precise == 1.618);
    JJ_TEST(ps2.spec2.colors.back.R == 0);
    JJ_TEST(ps2.spec2.colors.back.B == 63);
    JJ_TEST(ps2.spec2.colors.back.G == 63);

    jj::osstream_t str3;
    textSerializer_t<jj::osstream_t> ser3(str3, 2, jjT(' '));
    ps2.traverse(ser3, ctx);

    JJ_TEST(str1.str() == str3.str());
}

JJ_TEST_CLASS_END(propsTSerDeserTests_t, basic)
