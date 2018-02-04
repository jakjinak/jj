#include "jj/test/test.h"
#include "jj/string.h"
#include "jj/stream.h"
#include <vector>

JJ_TEST_CLASS(filterTests_t)

JJ_TEST_CASE_VARIANTS(Simple,(const jj::string_t& txt,bool ok,const std::vector<jj::string_t>& result),\
    (jjT("case"),true,{jjT(""),jjT(""),jjT("case"),jjT("")}),\
    (jjT("class/"),true,{jjT("class"),jjT(""),jjT(""),jjT("")}),\
    (jjT("class/case"),true,{jjT("class"),jjT(""),jjT("case"),jjT("")}),\
    (jjT("   case   "),true,{jjT(""),jjT(""),jjT("case"),jjT("")}),\
    (jjT("case   "),true,{jjT(""),jjT(""),jjT("case"),jjT("")}),\
    (jjT("   case"),true,{jjT(""),jjT(""),jjT("case"),jjT("")}),\
    (jjT("   class   /   "),true,{jjT("class"),jjT(""),jjT(""),jjT("")}),\
    (jjT("class/   "),true,{jjT("class"),jjT(""),jjT(""),jjT("")}),\
    (jjT("class   /"),true,{jjT("class"),jjT(""),jjT(""),jjT("")}),\
    (jjT("   class/"),true,{jjT("class"),jjT(""),jjT(""),jjT("")}),\
    (jjT("   class   /    case   "),true,{jjT("class"),jjT(""),jjT("case"),jjT("")}),\
    (jjT("class/case   "),true,{jjT("class"),jjT(""),jjT("case"),jjT("")}),\
    (jjT("class/   case"),true,{jjT("class"),jjT(""),jjT("case"),jjT("")}),\
    (jjT("class   /case"),true,{jjT("class"),jjT(""),jjT("case"),jjT("")}),\
    (jjT("class   /    case"),true,{jjT("class"),jjT(""),jjT("case"),jjT("")}),\
    (jjT("   class/case"),true,{jjT("class"),jjT(""),jjT("case"),jjT("")}),\
    (jjT("   class   /case"),true,{jjT("class"),jjT(""),jjT("case"),jjT("")}),\
    (jjT("/case"),false,{}),\
    (jjT("class//case"),false,{}),\
    (jjT("class//"),false,{}),\
    (jjT("class/ /"),false,{}),\
    (jjT("class/ /"),false,{}),\
    (jjT("class/case/"),false,{}),\
    (jjT("class/ /case"),false,{}))
{
    if (ok)
    {
        jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, txt);
        JJ_ENSURE(result.size()>=4);
        JJ_TEST(f.Class == result[0]);
        JJ_TEST(f.ClassVariant == result[1]);
        JJ_TEST(f.Case == result[2]);
        JJ_TEST(f.CaseVariant == result[3]);
    }
    else
    {
        JJ_TEST_THAT_THROWS(jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, txt), std::runtime_error);
    }
}

JJ_TEST_CASE_VARIANTS(SimpleClassVariant,(const jj::string_t& txt,bool ok,const std::vector<jj::string_t>& result),\
    (jjT("class()/"),true,{jjT("class"),jjT("()"),jjT(""),jjT("")}),\
    (jjT("class(var)/"),true,{jjT("class"),jjT("(var)"),jjT(""),jjT("")}),\
    (jjT("class   (var)   /   "),true,{jjT("class"),jjT("(var)"),jjT(""),jjT("")}),\
    (jjT("class(var)  /"),true,{jjT("class"),jjT("(var)"),jjT(""),jjT("")}),\
    (jjT("class( var )/"),true,{jjT("class"),jjT("( var )"),jjT(""),jjT("")}),\
    (jjT("class(v,a,r)/"),true,{jjT("class"),jjT("(v,a,r)"),jjT(""),jjT("")}),\
    (jjT("class(var)/"),true,{jjT("class"),jjT("(var)"),jjT(""),jjT("")}),\
    (jjT("class(var)/case"),true,{jjT("class"),jjT("(var)"),jjT("case"),jjT("")}),\
    (jjT("class   (var)   /   case"),true,{jjT("class"),jjT("(var)"),jjT("case"),jjT("")}),\
    (jjT("class(var"),false,{}),\
    (jjT("class(var/case"),false,{}),\
    (jjT("class(   "),false,{}),\
    (jjT("class(/"),false,{}),\
    (jjT("()/"),false,{}))
{
    if (ok)
    {
        jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, txt);
        JJ_ENSURE(result.size()>=4);
        JJ_TEST(f.Class == result[0]);
        JJ_TEST(f.ClassVariant == result[1]);
        JJ_TEST(f.Case == result[2]);
        JJ_TEST(f.CaseVariant == result[3]);
    }
    else
    {
        JJ_TEST_THAT_THROWS(jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, txt), std::runtime_error);
    }
}

JJ_TEST_CASE_VARIANTS(SimpleCaseVariant,(const jj::string_t& txt,bool ok,const std::vector<jj::string_t>& result),\
    (jjT("case()"),true,{jjT(""),jjT(""),jjT("case"),jjT("()")}),\
    (jjT("case(var)"),true,{jjT(""),jjT(""),jjT("case"),jjT("(var)")}),\
    (jjT("case( var )"),true,{jjT(""),jjT(""),jjT("case"),jjT("( var )")}),\
    (jjT("   case   ()   "),true,{jjT(""),jjT(""),jjT("case"),jjT("()")}),\
    (jjT("case   ()   "),true,{jjT(""),jjT(""),jjT("case"),jjT("()")}),\
    (jjT("case(v,a,r)"),true,{jjT(""),jjT(""),jjT("case"),jjT("(v,a,r)")}),\
    (jjT("case(var"),false,{}),\
    (jjT("case("),false,{}),\
    (jjT("class(var)/case()"),true,{jjT("class"),jjT("(var)"),jjT("case"),jjT("()")}))
{
    if (ok)
    {
        {
            jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, txt);
            JJ_ENSURE(result.size()>=4);
            JJ_TEST(f.Class == result[0]);
            JJ_TEST(f.ClassVariant == result[1]);
            JJ_TEST(f.Case == result[2]);
            JJ_TEST(f.CaseVariant == result[3]);
        }
        if (result[0].empty())
        {
            jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, jjT("class/")+txt);
            JJ_ENSURE(result.size()>=4);
            JJ_TEST(f.Class == jjT("class"));
            JJ_TEST(f.ClassVariant == result[1]);
            JJ_TEST(f.Case == result[2]);
            JJ_TEST(f.CaseVariant == result[3]);
        }
    }
    else
    {
        JJ_TEST_THAT_THROWS(jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, txt), std::runtime_error);
    }
}

JJ_TEST_CASE_VARIANTS(ComplexVariant,(const jj::string_t& txt,bool ok,jj::string_t result),\
    (jjT("(var)"),true,jjT("")),\
    (jjT("((var))"),true,jjT("")),\
    (jjT("((a),(b))"),true,jjT("")),\
    (jjT("((a+b)/c)"),true,jjT("")),\
    (jjT("(\"var\")"),true,jjT("")),\
    (jjT("('v')"),true,jjT("")),\
    (jjT("(\"'\")"),true,jjT("")),\
    (jjT("('\"')"),true,jjT("")),\
    (jjT("(\"(\")"),true,jjT("")),\
    (jjT("('(')"),true,jjT("")))
{
    if (ok)
    {
        if (result.empty())
            result = txt;
        {
            jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, jjT("case")+txt);
            JJ_ENSURE(result.size()>=4);
            JJ_TEST(f.Class == jjT(""));
            JJ_TEST(f.ClassVariant == jjT(""));
            JJ_TEST(f.Case == jjT("case"));
            JJ_TEST(f.CaseVariant == result);
        }
        {
            jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, jjT("class")+txt+jjT("/"));
            JJ_ENSURE(result.size()>=4);
            JJ_TEST(f.Class == jjT("class"));
            JJ_TEST(f.ClassVariant == result);
            JJ_TEST(f.Case == jjT(""));
            JJ_TEST(f.CaseVariant == jjT(""));
        }
        {
            jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, jjT("class")+txt+jjT("/case")+txt);
            JJ_ENSURE(result.size()>=4);
            JJ_TEST(f.Class == jjT("class"));
            JJ_TEST(f.ClassVariant == result);
            JJ_TEST(f.Case == jjT("case"));
            JJ_TEST(f.CaseVariant == result);
        }
    }
    else
    {
        JJ_TEST_THAT_THROWS(jj::test::AUX::filter_t f(jj::test::AUX::filter_t::ADD, txt), std::runtime_error);
    }
}

JJ_TEST_CLASS_END(filterTests_t, Simple, SimpleClassVariant, SimpleCaseVariant, ComplexVariant)
