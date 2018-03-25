#include "jj/test/test.h"
#include "jj/configuration.h"
#include "jj/configurationStorage.h"

#include <limits>

struct cfg_t
{
    jj::conf::integer_t<> Integer;
    jj::conf::numericRange_T<long, 0, 5, 1> Long;
    jj::conf::text_t Text;
    jj::conf::floatingPoint_t<> NotInt;
    jj::conf::floatingPointRange_t<> FRange;
    jj::conf::logical_t<> Bool;

    cfg_t() : Text(jjT("kak")), FRange(-1.2, 2.3, 1) {}

    template<typename STOR>
    void save(STOR& s, const jj::string_t& path)
    {
        s.begin_structure(jjT("cfg"));
        s.save(path, jjT("Integer"), Integer);
        s.save(path, jjT("Long"), Long);
        s.save(path, jjT("Text"), Text);
        s.save(path, jjT("NotInt"), NotInt);
        s.save(path, jjT("FRange"), FRange);
        s.save(path, jjT("Bool"), Bool);
        s.end_structure();
    }

    template<typename STOR>
    void load(STOR& s)
    {
        s.load_structure({
            { jjT("Integer"), [this,&s](const jj::string_t&, const jj::string_t&) { jj::conf::load(s, Integer); } },
            { jjT("Long"), [this,&s](const jj::string_t&, const jj::string_t&) { jj::conf::load(s, Long); } },
            { jjT("Text"), [this,&s](const jj::string_t&, const jj::string_t&) { jj::conf::load(s, Text); } },
            { jjT("NotInt"), [this,&s](const jj::string_t&, const jj::string_t&) { jj::conf::load(s, NotInt); } },
            { jjT("FRange"), [this,&s](const jj::string_t&, const jj::string_t&) { jj::conf::load(s, FRange); } },
            { jjT("Bool"), [this,&s](const jj::string_t&, const jj::string_t&) { jj::conf::load(s, Bool); } }
        });
    }
};

JJ_TEST_CLASS(configurationTests_t)

JJ_TEST_CASE(defaults)
{
    cfg_t cfg;
    JJ_TEST(cfg.Integer.is_default());
    JJ_TEST(cfg.Long.is_default());
    JJ_TEST(cfg.Text.is_default());
    JJ_TEST(cfg.NotInt.is_default());
    JJ_TEST(cfg.FRange.is_default());
    JJ_TEST(cfg.Bool.is_default());
}

JJ_TEST_CASE(basic)
{
    cfg_t cfg;
    JJ_TEST(cfg.Integer.get() == 0);
    cfg.Integer.set(20);
    JJ_TEST(cfg.Integer.get() == 20);
    JJ_TEST(cfg.Long.get() == 1);
    JJ_TEST_THAT_THROWS(cfg.Long.set(6), std::out_of_range);
    JJ_TEST(cfg.Long.get() == 1);
    cfg.Long.set(5);
    JJ_TEST(cfg.Long.get() == 5);
    JJ_TEST(cfg.Text.get() == jjT("kak"));
    cfg.Text.set(jjT("blah"));
    JJ_TEST(cfg.Text.get() == jjT("blah"));
    JJ_TEST(cfg.NotInt.get() == 0);
    cfg.NotInt.set(55.3);
    JJ_TEST(cfg.NotInt.get() == 55.3);
    JJ_TEST(cfg.FRange.get() == 1);
    JJ_TEST_THAT_THROWS(cfg.FRange.set(-1.25), std::out_of_range);
    JJ_TEST(cfg.FRange.get() == 1);
    cfg.FRange.set(-1.2);
    JJ_TEST(cfg.FRange.get() == -1.2);
    JJ_TEST(!cfg.Bool.get());
    cfg.Bool.set(true);
    JJ_TEST(cfg.Bool.get());
}

JJ_TEST_CASE(sstreamstore_saveload_defaults)
{
    cfg_t cfg;

    jj::conf::osstreamStorage_t stor;
    cfg.save(stor, jjT("xyz"));

    jj::conf::isstreamStorage_t load(stor.str());
    cfg_t cfg2;
    jj::string_t name;
    load.load_root(name);
    JJ_TEST(name == jjT("cfg"));
    cfg2.load(load);
    JJ_TEST(cfg2.Integer.is_default());
    JJ_TEST(cfg2.Long.is_default());
    JJ_TEST(cfg2.Text.is_default());
    JJ_TEST(cfg2.NotInt.is_default());
    JJ_TEST(cfg2.FRange.is_default());
    JJ_TEST(cfg2.Bool.is_default());
}

JJ_TEST_CASE(sstreamstore_saveload_values)
{
    cfg_t cfg;
    cfg.Integer.set(20);
    cfg.Long.set(5);
    cfg.Text.set(jjT("blah"));
    cfg.NotInt.set(55.3);
    cfg.FRange.set(-1.2);
    cfg.Bool.set(true);

    jj::conf::osstreamStorage_t stor;
    cfg.save(stor, jjT("xyz"));
    //jj::cout << stor.str();

    jj::conf::isstreamStorage_t load(stor.str());
    cfg_t cfg2;
    jj::string_t name;
    load.load_root(name);
    JJ_TEST(name == jjT("cfg"));
    cfg2.load(load);
    JJ_TEST(cfg2.Integer.get() == 20);
    JJ_TEST(cfg2.Long.get() == 5);
    JJ_TEST(cfg2.Text.get() == jjT("blah"));
    JJ_TEST(cfg2.NotInt.get() == 55.3);
    JJ_TEST(cfg2.FRange.get() == -1.2);
    JJ_TEST(cfg2.Bool.get() == true);
}

template<typename T>
struct singleValueCfg_t
{
    T Num;

    template<typename STOR>
    void save(STOR& s, const jj::string_t& path)
    {
        s.begin_structure(jjT("cfg"));
        s.save(path, jjT("Num"), Num);
        s.end_structure();
    }
    template<typename STOR>
    void load(STOR& s)
    {
        s.load_structure({
            { jjT("Num"), [this,&s](const jj::string_t&, const jj::string_t&) { jj::conf::load(s, Num); } }
        });
    }
};
template<typename CFG, typename T>
void singleValueCfg_test(T x)
{
    singleValueCfg_t<CFG> cfg;
    cfg.Num.set(x);

    jj::conf::osstreamStorage_t stor;
    cfg.save(stor, jjT("xyz"));
    //jj::cout << stor.str();

    jj::conf::isstreamStorage_t load(stor.str());
    singleValueCfg_t<CFG> cfg2;
    jj::string_t name;
    load.load_root(name);
    JJ_TEST(name == jjT("cfg"));
    cfg2.load(load);
    JJ_TEST(cfg2.Num.get() == x);
}

template<typename CFG, typename T>
void singleValueCfg_readtest(const jj::string_t& input, T value)
{
    singleValueCfg_t<CFG> cfg;
    jj::string_t inp;
    inp += jjT("cfg={\nNum=");
    inp += input;
    inp += jjT("\n}");
    jj::conf::isstreamStorage_t load(inp);
    jj::string_t name;
    load.load_root(name);
    JJ_TEST(name == jjT("cfg"));
    cfg.load(load);
    JJ_TEST(cfg.Num.get() == value, jjT("Actual value [") << cfg.Num.get() << jjT("] matches expected value [") << value << jjT("]."));
}

template<typename CFG>
void singleValueCfg_readfail(const jj::string_t& input)
{
    singleValueCfg_t<CFG> cfg;
    jj::string_t inp;
    inp += jjT("cfg={\nNum=");
    inp += input;
    inp += jjT("\n}");
    jj::conf::isstreamStorage_t load(inp);
    jj::string_t name;
    load.load_root(name);
    JJ_TEST(name == jjT("cfg"));
    cfg.load(load);
}

JJ_TEST_CASE_VARIANTS(edge_saveload_int,(int x),(1),(-1),(0),(std::numeric_limits<int>::max()),(std::numeric_limits<int>::min()))
{
    singleValueCfg_test<jj::conf::numeric_T<int, 5>, int>(x);
}

JJ_TEST_CASE_VARIANTS(load_int, (const jj::char_t* input, int value), (jjT(""),5),(jjT("blah"),5),(jjT("\\x"),5),(jjT("\\xa"),10),(jjT("\\xA0"),160),(jjT("\\xx"),5))
{
    singleValueCfg_readtest<jj::conf::numeric_T<int, 5>, int>(input, value);
}

JJ_TEST_CASE_VARIANTS(load_rangedint, (const jj::char_t* input, int value), (jjT(""),1),(jjT("-2"),-2),(jjT("3"),3))
{
    singleValueCfg_readtest<jj::conf::numericRange_T<int, -2, 3, 1>, int>(input, value);
}

JJ_TEST_CASE_VARIANTS(load_rangedint_fails, (const jj::char_t* input), (jjT("-3")), (jjT("4")))
{
    typedef jj::conf::numericRange_T<int, -2, 3, 1> thetype;
    JJ_TEST_THAT_THROWS(singleValueCfg_readfail<thetype>(input), std::out_of_range);
}

JJ_TEST_CASE_VARIANTS(edge_saveload_uint, (unsigned int x), (0),(1),(std::numeric_limits<unsigned int>::max()))
{
    singleValueCfg_test<jj::conf::numeric_T<unsigned int, 5>, unsigned int>(x);
}

JJ_TEST_CASE_VARIANTS(edge_saveload_bool, (bool x), (true), (false))
{
    if (x)
        singleValueCfg_test<jj::conf::logical_t<false>, bool>(true);
    else
        singleValueCfg_test<jj::conf::logical_t<true>, bool>(false);
}

JJ_TEST_CASE_VARIANTS(load_bool, (const jj::char_t* input, bool value), (jjT("0"),false),(jjT("1"),true),\
    (jjT("f"),false),(jjT("F"),false),(jjT("false"),false),(jjT("FALSE"),false),(jjT("fALSe"),false),\
    (jjT("t"),true),(jjT("T"),true),(jjT("true"),true),(jjT("TRUE"),true),(jjT("tRuE"),true),\
    (jjT("n"),false),(jjT("N"),false),(jjT("no"),false),(jjT("NO"),false),(jjT("No"),false),\
    (jjT("y"),true),(jjT("Y"),true),(jjT("yes"),true),(jjT("YES"),true),(jjT("yeS"),true))
{
    if (value)
        singleValueCfg_readtest<jj::conf::logical_t<false>, bool>(input, true);
    else
        singleValueCfg_readtest<jj::conf::logical_t<true>, bool>(input, false);
}

JJ_TEST_CASE_VARIANTS(load_bool_fails, (const jj::char_t* input), (jjT("")), (jjT("fa")), (jjT("TRU")), (jjT("Yea")), (jjT("bla")))
{
    typedef jj::conf::logical_t<true> thetype;
    JJ_TEST_THAT_THROWS(singleValueCfg_readfail<thetype>(input), std::runtime_error);
}

JJ_TEST_CASE_VARIANTS(edge_saveload_ll, (long long x), (1), (-1), (0), (std::numeric_limits<long long>::max()), (std::numeric_limits<long long>::min()))
{
    singleValueCfg_test<jj::conf::numeric_T<long long, -13>, long long>(x);
}

JJ_TEST_CASE_VARIANTS(edge_saveload_d, (double x), (1.00000001), (-1.123456789), (0), (std::numeric_limits<double>::max()), (std::numeric_limits<double>::min()), (std::numeric_limits<double>::lowest()), (std::numeric_limits<double>::epsilon()))
{
    singleValueCfg_test<jj::conf::floatingPoint_t<double>, double>(x);
}

JJ_TEST_CASE_VARIANTS(edge_saveload_ld, (long double x), (1.000000000001), (-1.0000123456789), (0), (std::numeric_limits<long double>::max()), (std::numeric_limits<long double>::min()), (std::numeric_limits<long double>::lowest()), (std::numeric_limits<long double>::epsilon()))
{
    singleValueCfg_test<jj::conf::floatingPoint_t<long double>, long double>(x);
}

JJ_TEST_CASE_VARIANTS(edge_saveload_txt, (const jj::char_t* x), (jjT("")), (jjT(" ")), (jjT("\t")), (jjT("A\nB")), (jjT(" \n\n ")), (jjT("a\n")), (jjT("\na")))
{
    singleValueCfg_test<jj::conf::text_t, jj::string_t>(x);
}

JJ_TEST_CLASS_END(configurationTests_t, defaults, basic, sstreamstore_saveload_defaults, sstreamstore_saveload_values, \
    edge_saveload_int, load_int, load_rangedint, load_rangedint_fails, edge_saveload_uint, edge_saveload_bool, load_bool, load_bool_fails, edge_saveload_ll, \
    edge_saveload_d, edge_saveload_ld, edge_saveload_txt)
