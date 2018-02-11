#include "jj/test/test.h"
#include "jj/flagSet.h"

JJ_TEST_CLASS(flagSetTests_t)

enum E
{
    A,
    B,
    C,
    D,
    MAX
};

enum BAD
{
    X,
    MAXX,
    Y,
    Z
};

JJ_TEST_CASE(initializing_testing)
{
    jj::flagSet_t<E, MAX> s, s1({ B, D });

    JJ_TEST(!s.test(A));
    JJ_TEST(!s.test(B));
    JJ_TEST(!s.test(C));
    JJ_TEST(!s.test(D));
    JJ_TEST(!(s*A));
    JJ_TEST(!(s*B));
    JJ_TEST(!(s*C));
    JJ_TEST(!(s*D));

    JJ_TEST(!s1.test(A));
    JJ_TEST(s1.test(B));
    JJ_TEST(!s1.test(C));
    JJ_TEST(s1.test(D));
    JJ_TEST(!(s1*A));
    JJ_TEST(s1*B);
    JJ_TEST(!(s1*C));
    JJ_TEST(s1*D);

    jj::flagSet_t<E, MAX> s2(s1);
    JJ_TEST(!(s1*A));
    JJ_TEST(s1*B);
    JJ_TEST(!(s1*C));
    JJ_TEST(s1*D);
    JJ_TEST(!(s2*A));
    JJ_TEST(s2*B);
    JJ_TEST(!(s2*C));
    JJ_TEST(s2*D);

    s1.toggle(C);
    JJ_TEST(!(s1*A));
    JJ_TEST(s1*B);
    JJ_TEST(s1*C);
    JJ_TEST(s1*D);
    JJ_TEST(!(s2*A));
    JJ_TEST(s2*B);
    JJ_TEST(!(s2*C));
    JJ_TEST(s2*D);
}

JJ_TEST_CASE(setting_resetting)
{
    jj::flagSet_t<E, MAX> s;

    s.set(A);
    JJ_TEST(s*A);
    JJ_TEST(!(s*B));
    JJ_TEST(!(s*C));
    JJ_TEST(!(s*D));

    s += C;
    JJ_TEST(s*A);
    JJ_TEST(!(s*B));
    JJ_TEST(s*C);
    JJ_TEST(!(s*D));

    s.set(A);
    JJ_TEST(s*A);
    JJ_TEST(!(s*B));
    JJ_TEST(s*C);
    JJ_TEST(!(s*D));

    s |= D;
    JJ_TEST(s*A);
    JJ_TEST(!(s*B));
    JJ_TEST(s*C);
    JJ_TEST(s*D);

    s.reset(D);
    JJ_TEST(s*A);
    JJ_TEST(!(s*B));
    JJ_TEST(s*C);
    JJ_TEST(!(s*D));

    s.reset(D);
    JJ_TEST(s*A);
    JJ_TEST(!(s*B));
    JJ_TEST(s*C);
    JJ_TEST(!(s*D));

    s -= A;
    JJ_TEST(!(s*A));
    JJ_TEST(!(s*B));
    JJ_TEST(s*C);
    JJ_TEST(!(s*D));

    s &= C;
    JJ_TEST(!(s*A));
    JJ_TEST(!(s*B));
    JJ_TEST(!(s*C));
    JJ_TEST(!(s*D));
}

JJ_TEST_CASE(setting_resetting_new)
{
    jj::flagSet_t<E, MAX> s({ B,C }), m, n, p, q;
    m = s + A;
    n = s | D;
    p = s - B;
    q = s & C;
    JJ_TEST(!(s*A));
    JJ_TEST(s*B);
    JJ_TEST(s*C);
    JJ_TEST(!(s*D));
    JJ_TEST(m*A);
    JJ_TEST(m*B);
    JJ_TEST(m*C);
    JJ_TEST(!(m*D));
    JJ_TEST(!(n*A));
    JJ_TEST(n*B);
    JJ_TEST(n*C);
    JJ_TEST(n*D);
    JJ_TEST(!(p*A));
    JJ_TEST(!(p*B));
    JJ_TEST(p*C);
    JJ_TEST(!(p*D));
    JJ_TEST(!(q*A));
    JJ_TEST(q*B);
    JJ_TEST(!(q*C));
    JJ_TEST(!(q*D));
}

JJ_TEST_CASE(toggling)
{
    jj::flagSet_t<E, MAX> s({ B });
    s.toggle(C);
    JJ_TEST(!(s*A));
    JJ_TEST(s*B);
    JJ_TEST(s*C);
    JJ_TEST(!(s*D));
    s.toggle(C);
    JJ_TEST(!(s*A));
    JJ_TEST(s*B);
    JJ_TEST(!(s*C));
    JJ_TEST(!(s*D));

    s.flip(A);
    JJ_TEST(s*A);
    JJ_TEST(s*B);
    JJ_TEST(!(s*C));
    JJ_TEST(!(s*D));
    s.flip(A);
    JJ_TEST(!(s*A));
    JJ_TEST(s*B);
    JJ_TEST(!(s*C));
    JJ_TEST(!(s*D));

    s ^= B;
    JJ_TEST(!(s*A));
    JJ_TEST(!(s*B));
    JJ_TEST(!(s*C));
    JJ_TEST(!(s*D));
    s ^= B;
    JJ_TEST(!(s*A));
    JJ_TEST(s*B);
    JJ_TEST(!(s*C));
    JJ_TEST(!(s*D));
}

JJ_TEST_CASE(toggling_new)
{
    jj::flagSet_t<E, MAX> s({ B }), x;
    x = s ^ C;
    JJ_TEST(!(s*A));
    JJ_TEST(s*B);
    JJ_TEST(!(s*C));
    JJ_TEST(!(s*D));
    JJ_TEST(!(x*A));
    JJ_TEST(x*B);
    JJ_TEST(x*C);
    JJ_TEST(!(x*D));

    jj::flagSet_t<E, MAX> y;
    y = x ^ B;
    JJ_TEST(!(x*A));
    JJ_TEST(x*B);
    JJ_TEST(x*C);
    JJ_TEST(!(x*D));
    JJ_TEST(!(y*A));
    JJ_TEST(!(y*B));
    JJ_TEST(y*C);
    JJ_TEST(!(y*D));
}

JJ_TEST_CASE(abovethelimit)
{
    jj::flagSet_t<BAD, MAXX> x;
    JJ_TEST_THAT_THROWS(x.set(Y), std::out_of_range);
    JJ_TEST_THAT_THROWS(x * Z, std::out_of_range);
}

JJ_TEST_CLASS_END(flagSetTests_t, initializing_testing, setting_resetting, setting_resetting_new, toggling, toggling_new, abovethelimit)
