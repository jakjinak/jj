#include "jj/test/test.h"
#include "jj/macros.h"

JJ_TEST_CLASS(ppTests_t)

JJ_TEST_CASE(count)
{
    int cnt;
    cnt = JJ_COUNT();
    JJ_TEST(cnt == 1);
    cnt = JJ_COUNT(a);
    JJ_TEST(cnt == 1);
    cnt = JJ_COUNT(a b);
    JJ_TEST(cnt == 1);
    cnt = JJ_COUNT(a, b);
    JJ_TEST(cnt == 2);
    cnt = JJ_COUNT(a,);
    JJ_TEST(cnt == 2);
    cnt = JJ_COUNT(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q);
    JJ_TEST(cnt == 17);
}

#define TESTMACRO1(a) "1"
#define TESTMACRO2(a,b) "2"
#define TESTMACRO13(a,b,c,d,e,f,g,h,i,j,k,l,m) "13"

JJ_TEST_CASE(choose)
{
    std::string v;
    v = JJ_VARG_N(TESTMACRO);
    JJ_TEST(v == "1"); // this should be 0
    v = JJ_VARG_N(TESTMACRO, );
    JJ_TEST(v == "1");
    v = JJ_VARG_N(TESTMACRO, A);
    JJ_TEST(v == "1");
    v = JJ_VARG_N(TESTMACRO, A,B);
    JJ_TEST(v == "2");
    v = JJ_VARG_N(TESTMACRO, A B);
    JJ_TEST(v == "1");
    v = JJ_VARG_N(TESTMACRO, A, B, C, D, E, F, G, H, I, J, K, L, M);
    JJ_TEST(v == "13");
}

JJ_TEST_CLASS_END(ppTests_t, count, choose)
