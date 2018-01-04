#include "jj/test/test.h"
#include "jj/string.h"

JJ_TEST_CLASS(stringTests_t)

JJ_TEST_CASE_VARIANTS(charp2string,(const char* in, const char* out), ("", ""), ("a", "a"), ("AbC023as;lk/./.as2190","AbC023as;lk/./.as2190"))
{
    JJ_TEST(jj::strcvt::to_string(in) == out);
}

JJ_TEST_CLASS_END(stringTests_t, charp2string)
