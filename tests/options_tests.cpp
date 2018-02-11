#include "jj/test/test.h"
#include "jj/options.h"

struct A { jj::string_t Avalue; A() {} A(jj::string_t a) : Avalue(a) {} };
struct B { jj::string_t Bvalue; B() {} B(jj::string_t b) : Bvalue(b) {} };
struct C { jj::string_t Cvalue; C() {} C(jj::string_t c) : Cvalue(c) {} };
struct I { int Ivalue; I() : Ivalue() {} I(int i) : Ivalue(i) {} };

enum E
{
    X,
    Y,
    Z,
    MAX
};

JJ_TEST_CLASS(optionsTests_t)

JJ_TEST_CASE(regularValues)
{
    typedef jj::options_T<A, B, I> options_t;
    // MANUAL typedef jj::options_T<A, B, int> options_t; // would have failed
    // MANUAL typedef jj::options_T<A, A, I> options_t; // would have failed
    options_t o;
    JJ_TEST(o.Avalue.empty());
    JJ_TEST(o.Bvalue.empty());
    JJ_TEST(o.Ivalue == 0);
    A& a = o;
    B& b = o;
    I& i = o;
    o << A(jjT("AAAA")) << I(6);
    JJ_TEST(a.Avalue == jjT("AAAA"));
    JJ_TEST(o.Avalue == jjT("AAAA"));
    JJ_TEST(b.Bvalue.empty());
    JJ_TEST(o.Bvalue.empty());
    JJ_TEST(i.Ivalue == 6);
    JJ_TEST(o.Ivalue == 6);
    o >> A() >> B() >> I(); // does nothing for regular values
    JJ_TEST(a.Avalue == jjT("AAAA"));
    JJ_TEST(o.Avalue == jjT("AAAA"));
    JJ_TEST(b.Bvalue.empty());
    JJ_TEST(o.Bvalue.empty());
    JJ_TEST(i.Ivalue == 6);
    JJ_TEST(o.Ivalue == 6);
    // MANUAL o << C(); // would have failed
}

JJ_TEST_CASE(oneof)
{
    typedef jj::options_T<A, jj::opt::e<E>> options_t;
    options_t o;
    JJ_TEST(o.Avalue.empty());
    JJ_TEST(static_cast<jj::opt::e<E>&>(o).Value == X);
    o << Y;
    jj::opt::e<E>& e = o;
    JJ_TEST(e.Value == Y);
    o << X << Z;
    JJ_TEST(e.Value == Z);
    JJ_TEST(o.Avalue.empty());
}

JJ_TEST_CASE(anyof)
{
    // note: presence of e<E> merely points to fact that template args searched linearly, first match wins
    typedef jj::options_T<A, jj::opt::f<E, E::MAX>, jj::opt::e<E>> options_t;
    options_t o;
    jj::opt::e<E>& e = o;
    jj::opt::f<E, E::MAX>& f = o;
    JJ_TEST(e.Value == X);
    JJ_TEST(!(f*X));
    JJ_TEST(!(f*Y));
    JJ_TEST(!(f*Z));
    o << X << Z;
    JJ_TEST(f*X);
    JJ_TEST(!(f*Y));
    JJ_TEST(f*Z);
    o << X << Y;
    JJ_TEST(f*X);
    JJ_TEST(f*Y);
    JJ_TEST(f*Z);
    o >> X;
    JJ_TEST(e.Value == X);
    JJ_TEST(!(f*X));
    JJ_TEST(f*Y);
    JJ_TEST(f*Z);
    JJ_TEST(o.Avalue.empty());
}

JJ_TEST_CASE(anyof_unsetset)
{
    typedef jj::options_T<jj::opt::f<E, E::MAX>> options_t;
    options_t o;
    jj::opt::f<E, E::MAX>& f = o;
    o << X;
    JJ_TEST(f*X);
    JJ_TEST(!(f*Y));
    JJ_TEST(!(f*Z));
    o >> Y << Y;
    JJ_TEST(f*X);
    JJ_TEST(f*Y);
    JJ_TEST(!(f*Z));
    o << Y >> Y;
    JJ_TEST(f*X);
    JJ_TEST(!(f*Y));
    JJ_TEST(!(f*Z));
}

JJ_TEST_CLASS_END(optionsTests_t, regularValues, oneof, anyof, anyof_unsetset)
