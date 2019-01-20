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
#if defined(JJ_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable:4003)
#endif
    v = JJ_VARG_N(TESTMACRO);
    JJ_TEST(v == "1"); // this should be 0, but preprocessors do not support it
    v = JJ_VARG_N(TESTMACRO, );
    JJ_TEST(v == "1");
#if defined(JJ_COMPILER_MSVC)
#pragma warning(pop)
#endif
    v = JJ_VARG_N(TESTMACRO, A);
    JJ_TEST(v == "1");
    v = JJ_VARG_N(TESTMACRO, A,B);
    JJ_TEST(v == "2");
    v = JJ_VARG_N(TESTMACRO, A B);
    JJ_TEST(v == "1");
    v = JJ_VARG_N(TESTMACRO, A, B, C, D, E, F, G, H, I, J, K, L, M);
    JJ_TEST(v == "13");
}

JJ_TEST_CASE(merge)
{
    int ABC = 567;
    int i;
    i = jjM(1, 2, 3, 4);
    JJ_TEST(i == 1234);
    i = jjM(1);
    JJ_TEST(i == 1);
#define TESTMERGE_A A
#define TESTMERGE_B B
#define TESTMERGE_C C
    i = jjM(TESTMERGE_A, TESTMERGE_B, TESTMERGE_C);
    JJ_TEST(i == 567);
#undef TESTMERGE_A
#undef TESTMERGE_B
#undef TESTMERGE_C

#define TESTMERGEA2(x,y) x - y
#define TESTMERGEA3(x,y,z) x * y + z
    i = JJ_VARG_N(jjM(TEST, MERGE, A), 1, 2);
    JJ_TEST(i == -1);
    i = JJ_VARG_N(jjM(TESTMERGE, A), 3, 10, 1);
    JJ_TEST(i == 31);
#undef TESTMERGEA1
#undef TESTMERGEA2
}

JJ_TEST_CLASS_END(ppTests_t, count, choose, merge)
