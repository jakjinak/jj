#include "jj/test/test.h"
#include "jj/string.h"
#include "jj/stream.h"

JJ_TEST_CLASS(testTests_t)

JJ_TEST_CASE(passes_reportsok)
{
    JJ_TEST(true);
    JJ_TEST(true, "succeeds");
}

JJ_TEST_CASE(notests_reportsok)
{
}

JJ_TEST_CASE(warning_reportsok)
{
    JJ_WARN(false);
    JJ_WARN(false, "this is warning");
    JJ_TEST(true);
}

JJ_TEST_CASE(error_reportsfail)
{
    JJ_TEST(false);
    JJ_TEST(false, "this is error");
    JJ_TEST(true);
}

JJ_TEST_CASE(error_endscase)
{
    JJ_ENSURE(false);
    JJ_TEST(true);
}

JJ_TEST_CASE(error_endscase2)
{
    JJ_ENSURE(false, "this is error");
    JJ_TEST(true);
}

JJ_TEST_CASE(error_endsclass)
{
    JJ_MUSTBE(false);
    JJ_TEST(true);
}

JJ_TEST_CASE(error_endsclass2)
{
    JJ_MUSTBE(false, "this is fatal");
    JJ_TEST(true);
}

JJ_TEST_CLASS_END(testTests_t, passes_reportsok, notests_reportsok, warning_reportsok, error_reportsfail, error_endscase, error_endscase2, error_endsclass, error_endsclass2)

//================================================

JJ_TEST_CLASS_VARIANTS(varclass,(1),(2))

int i_;

varclass(int i) : i_(i) {}

JJ_TEST_CASE(test)
{
    JJ_TEST(i_==1);
}

JJ_TEST_CASE_VARIANTS(vartest,(int x),(1),(2))
{
    JJ_TEST(i_==x);
}

JJ_TEST_CLASS_END(varclass,test,vartest)
