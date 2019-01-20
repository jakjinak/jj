#include "jj/log.h"
#include "jj/test/test.h"
#include "jj/time.h"
#include <sstream>

#define LLL1(msg) JJ__LOGGER(JJ_LOGLEVEL_OFF - 1, jjT("LLL1"), msg)
#define LLL2(msg) JJ__LOGGER(JJ_LOGLEVEL_INFO + 1, jjT("LLL2"), msg)
#define LLL3(msg) JJ__LOGGER(JJ_LOGLEVEL_INFO - 1, jjT("LLL3"), msg)
#define LLL4(msg) JJ__LOGGER(JJ_LOGLEVEL_SCOPE - 1, jjT("LLL4"), msg)

struct adummylogclass
{
    adummylogclass(int x) : x_(x) {}
    int get_x() const { return x_; }
private:
    int x_;
};

template<typename CH, typename TR>
std::basic_ostream<CH, TR>& operator<<(std::basic_ostream<CH, TR>& s, const adummylogclass& v)
{
    s << jj::str::literals_t<CH>::LSB << v.get_x() << jj::str::literals_t<CH>::RSB;
    return s;
}

struct testTargetCb : public jj::log::logTarget_base_t
{
    testTargetCb(std::function<void(const jj::log::message_t&)> fn)
        : fn_(fn)
    {
    }

    virtual void log(const jj::log::message_t& log) override
    {
        fn_(log);
    }
private:
    std::function<void(const jj::log::message_t&)> fn_;
};

struct testTargetCnt : public jj::log::logTarget_base_t
{
    jj::sstreamt_t s;
    size_t lines;
    testTargetCnt() : lines(0) {}
    virtual void log(const jj::log::message_t& log) override
    {
        s << jj::time::stamp_t(log.Time) << jjT(' ') << jjT('[') << log.LevelName << jjT(']') << jjT(' ') << log.Message << jjT('(') << log.File << jjT(':') << log.Line << jjT('|') << log.Function << jjT(')') << jjT('\n');
        ++lines;
    }
};

JJ_LOGGER_INITIALIZER(,)

JJ_TEST_CLASS(logTests_t)

JJ_TEST_CASE(fields)
{
    int ln;
    const char* f = JJ_FUNC;
    auto fn = [&](const jj::log::message_t& log) {
        JJ_ENSURE(log.File != nullptr);
        JJ_TEST(std::string(__FILE__) == log.File);
        JJ_TEST(log.Line == ln);
        JJ_ENSURE(log.Function != nullptr);
        JJ_TEST(std::string(f) == log.Function);

    };
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(std::make_shared<testTargetCb>(fn));

    ln = __LINE__ + 1;
    jjLF(jjT("F F F"));
    ln = __LINE__ + 1;
    jjLI(jjT("I I I"));

    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CASE_VARIANTS(levels, (jj::log::level_t level, size_t cnt), \
    (JJ_LOGLEVEL_OFF, 0), (JJ_LOGLEVEL_FATAL, 2), (JJ_LOGLEVEL_ERROR, 3), (JJ_LOGLEVEL_ALERT, 4), \
    (JJ_LOGLEVEL_WARNING, 5), (JJ_LOGLEVEL_INFO, 7), (JJ_LOGLEVEL_VERBOSE, 9), (JJ_LOGLEVEL_DEBUG, 9))
{
    jj::log::logger_t::instance().setLevel(level);
    std::shared_ptr<testTargetCnt> tgt;
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(tgt = std::make_shared<testTargetCnt>());
    LLL1(jjT("XYZ1"));
    jjLF(jjT("A B C"));
    jjLE(jjT("D E F"));
    jjLA(jjT("G H I"));
    jjLW(jjT("J K L"));
    LLL2(jjT("XYZ2"));
    jjLI(jjT("M N O"));
    LLL3(jjT("XYZ3"));
    jjLV(jjT("P Q R"));
    LLL4(jjT("XYZ4"));
    jjLD(jjT("S T U"));
    jj::string_t line;
    size_t lcnt = 0;
    while (std::getline(tgt->s, line))
    {
        ++lcnt;
        if (lcnt > cnt)
            continue;
        if (lcnt == 1)
        {
            JJ_TEST(line.find(jjT("XYZ1")) != jj::string_t::npos);
            JJ_TEST(line.find(jjT("LLL1")) != jj::string_t::npos);
        }
        if (lcnt == 2)
        {
            JJ_TEST(line.find(jjT("A B C")) != jj::string_t::npos);
            JJ_TEST(line.find(jj::log::logger_t::NAME_FATAL) != jj::string_t::npos);
        }
        if (lcnt == 3)
        {
            JJ_TEST(line.find(jjT("D E F")) != jj::string_t::npos);
            JJ_TEST(line.find(jj::log::logger_t::NAME_ERROR) != jj::string_t::npos);
        }
        if (lcnt == 4)
        {
            JJ_TEST(line.find(jjT("G H I")) != jj::string_t::npos);
            JJ_TEST(line.find(jj::log::logger_t::NAME_ALERT) != jj::string_t::npos);
        }
        if (lcnt == 5)
        {
            JJ_TEST(line.find(jjT("J K L")) != jj::string_t::npos);
            JJ_TEST(line.find(jj::log::logger_t::NAME_WARNING) != jj::string_t::npos);
        }
        if (lcnt == 6)
        {
            JJ_TEST(line.find(jjT("XYZ2")) != jj::string_t::npos);
            JJ_TEST(line.find(jjT("LLL2")) != jj::string_t::npos);
        }
        if (lcnt == 7)
        {
            JJ_TEST(line.find(jjT("M N O")) != jj::string_t::npos);
            JJ_TEST(line.find(jj::log::logger_t::NAME_INFO) != jj::string_t::npos);
        }
        if (lcnt == 8)
        {
            JJ_TEST(line.find(jjT("XYZ3")) != jj::string_t::npos);
            JJ_TEST(line.find(jjT("LLL3")) != jj::string_t::npos);
        }
        if (lcnt == 9)
        {
            JJ_TEST(line.find(jjT("P Q R")) != jj::string_t::npos);
            JJ_TEST(line.find(jj::log::logger_t::NAME_VERBOSE) != jj::string_t::npos);
        }
    }
    JJ_TEST(tgt->lines == cnt);
    JJ_TEST(lcnt == cnt);
    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CASE(message1)
{
    auto fn = [&](const jj::log::message_t& log) {
        JJ_TEST(log.Message == jjT(""));
    };
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(std::make_shared<testTargetCb>(fn));
    jj::log::logger_t::instance().setLevel(JJ_LOGLEVEL_INFO);
    jjLI(jjT(""));
    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CASE(message2)
{
    auto fn = [&](const jj::log::message_t& log) {
        JJ_TEST(log.Message == jjT("1       A         Z       556-3.1415"), jjOOO(log.Message,==,jjT("1       A         Z       556-3.1415")));
    };
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(std::make_shared<testTargetCb>(fn));
    jj::log::logger_t::instance().setLevel(JJ_LOGLEVEL_INFO);
    jjLI(jj::string_t(jjT("")) << true << jj::string_t(jjT("       A         Z       ")) << 556 << jjT('-') << 3.1415);
    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CASE(message3)
{
    auto fn = [&](const jj::log::message_t& log) {
        JJ_TEST(log.Message == jjT("20[99999]"));
    };
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(std::make_shared<testTargetCb>(fn));
    jj::log::logger_t::instance().setLevel(JJ_LOGLEVEL_INFO);
    jjLI(2 << false << adummylogclass(99999));
    jj::log::logger_t::instance().replaceTargets(olog);
}

struct Rclass
{
    bool M1(int x)
    {
        jjLI(jjT("A ") << x << jjT(" Z"));
        return true;
    }
    int M2()
    {
        jjLW(jjT("B ") << M1(2) << jjT(" Y"));
        return 2;
    }
};

JJ_TEST_CASE(recursion)
{
    std::shared_ptr<testTargetCnt> tgt;
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(tgt = std::make_shared<testTargetCnt>());

    jj::log::logger_t::instance().setLevel(JJ_LOGLEVEL_VERBOSE);
    jjLV(jjT("C ") << Rclass().M2() << jjT(" X"));
    jj::string_t line;
    size_t lcnt = 0;
    while (std::getline(tgt->s, line))
    {
        ++lcnt;
        if (lcnt == 1)
        {
            JJ_TEST(line.find(jjT("A 2 Z")) != jj::string_t::npos);
            JJ_TEST(line.find(jj::log::logger_t::NAME_INFO) != jj::string_t::npos);
        }
        else if (lcnt == 2)
        {
            JJ_TEST(line.find(jjT("B 1 Y")) != jj::string_t::npos);
            JJ_TEST(line.find(jj::log::logger_t::NAME_WARNING) != jj::string_t::npos);
        }
        else if (lcnt == 3)
        {
            JJ_TEST(line.find(jjT("C 2 X")) != jj::string_t::npos);
            JJ_TEST(line.find(jj::log::logger_t::NAME_VERBOSE) != jj::string_t::npos);
        }
    }
    JJ_TEST(lcnt == 3);
    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CASE(multiple_targets)
{
    std::shared_ptr<testTargetCnt> tgt1, tgt2, tgt3;
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(tgt1 = std::make_shared<testTargetCnt>());
    jj::log::logger_t::instance().registerTarget(tgt2 = std::make_shared<testTargetCnt>());
    jj::log::logger_t::instance().registerTarget(tgt3 = std::make_shared<testTargetCnt>());

    jjLI(jjT("X Y Z"));
    jjLE(jjT("E E"));
    for (auto t : { tgt1, tgt2, tgt3 })
    {
        jj::string_t line;
        size_t lcnt = 0;
        while (std::getline(t->s, line))
        {
            ++lcnt;
            if (lcnt == 1)
            {
                JJ_TEST(line.find(jjT("X Y Z")) != jj::string_t::npos);
                JJ_TEST(line.find(jj::log::logger_t::NAME_INFO) != jj::string_t::npos);
            }
            else if (lcnt == 2)
            {
                JJ_TEST(line.find(jjT("E E")) != jj::string_t::npos);
                JJ_TEST(line.find(jj::log::logger_t::NAME_ERROR) != jj::string_t::npos);
            }
        }
        JJ_TEST(lcnt == 2);
    }
    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CASE(targets_switch)
{
    std::shared_ptr<testTargetCb> tgt1, tgt2, tgt3, tgt4;
    size_t c1 = 0, c2 = 0, c3 = 0, c4 = 0;
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(tgt1 = std::make_shared<testTargetCb>([&](const jj::log::message_t& log) { ++c1; }));
    jjLI(1);
    JJ_TEST(c1 == 1);
    JJ_TEST(c2 == 0);
    JJ_TEST(c3 == 0);
    JJ_TEST(c4 == 0);
    jj::log::logger_t::instance().replaceTargets(tgt2 = std::make_shared<testTargetCb>([&](const jj::log::message_t& log) { ++c2; }));
    jjLI(2);
    JJ_TEST(c1 == 1);
    JJ_TEST(c2 == 1);
    JJ_TEST(c3 == 0);
    JJ_TEST(c4 == 0);
    jj::log::logger_t::instance().registerTarget(tgt3 = std::make_shared<testTargetCb>([&](const jj::log::message_t& log) { ++c3; }));
    jjLI(3);
    JJ_TEST(c1 == 1);
    JJ_TEST(c2 == 2);
    JJ_TEST(c3 == 1);
    JJ_TEST(c4 == 0);
    jj::log::logger_t::instance().replaceTargets(tgt4 = std::make_shared<testTargetCb>([&](const jj::log::message_t& log) { ++c4; }));
    jjLI(4);
    JJ_TEST(c1 == 1);
    JJ_TEST(c2 == 2);
    JJ_TEST(c3 == 1);
    JJ_TEST(c4 == 1);
    jj::log::logger_t::instance().replaceTargets(olog);
}

int scopeline1,scopeline2;

void scopemethod1(bool ret, bool msg)
{
    scopeline1 = __LINE__; jjLscope(jjT("A") << 5 << jjT("Z"));
    if (msg)
    {
        if (ret)
            return scopeline2 = __LINE__, jjLLmsg(jjT("KLM") << 111);
    }
    else
    {
        if (ret)
            return scopeline2 = __LINE__, jjLLvoid;
    }
}

int scopemethod2(bool val, bool msg)
{
    scopeline1 = __LINE__; jjLscope(jjT(""));
    if (msg)
    {
        if (val)
            return scopeline2 = __LINE__, jjLLval(555, jjT("XYZ" << 444));
        else
        {
            int a = 666;
            return scopeline2 = __LINE__, jjLL(a, jjT("XYZ") << 333);
        }
    }
    else
    {
        if (val)
            return scopeline2 = __LINE__, jjLLval(555);
        else
        {
            int a = 666;
            return scopeline2 = __LINE__, jjLL(a);
        }
    }
}

struct testEx : public std::exception
{
    testEx(const char* msg) : msg_(msg) {}
    const char* what() const noexcept override { return msg_.c_str(); }
private:
    std::string msg_;
};

void scopemethod3(bool val, bool msg)
{
    scopeline1 = __LINE__; jjLscope(jjT("T"));
    if (msg)
    {
        if (val)
            scopeline2 = __LINE__, throw jjLXval(testEx("EXCE"), jjT("some") << jjT("thing"));
        else
            scopeline2 = __LINE__, throw jjLX(testEx("EXCE"), jjT("some") << jjT("thing"));
    }
    else
    {
        if (val)
            scopeline2 = __LINE__, throw jjLXval(testEx("EX"));
        else
            scopeline2 = __LINE__, throw jjLX(testEx("EX"));
    }
}

JJ_TEST_CASE(scope_default)
{
    int cnt = 0;
    auto fn = [&](const jj::log::message_t& log) {
        JJ_TEST(log.Level == JJ_LOGLEVEL_SCOPE);
        if (cnt == 0)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_ENTER));
            JJ_TEST(log.Message == jjT("A5Z"));
            JJ_TEST(log.Line == scopeline1);
        }
        else if (cnt == 1)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_LEAVE));
            JJ_TEST(log.Message == jjT("<end-of-scope>"));
            JJ_TEST(log.Line == -1);
        }
        ++cnt;
    };
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(std::make_shared<testTargetCb>(fn));
    jj::log::logger_t::instance().setLevel(JJ_LOGLEVEL_SCOPE);
    scopemethod1(false, false);
    JJ_TEST(cnt == 2);
    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CASE(scope_return)
{
    int cnt = 0;
    auto fn = [&](const jj::log::message_t& log) {
        JJ_TEST(log.Level == JJ_LOGLEVEL_SCOPE);
        if (cnt % 2 == 0)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_ENTER));
            JJ_TEST(log.Message == jjT(""));
            JJ_TEST(log.Line == scopeline1);
        }
        else if (cnt == 1 || cnt == 5)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_LEAVE));
            JJ_TEST(log.Message == (cnt == 1 ? jjT("555") : jjT("XYZ444; 555")));
            JJ_TEST(log.Line == scopeline2);
        }
        else if (cnt == 3 || cnt == 7)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_LEAVE));
            JJ_TEST(log.Message == (cnt == 3 ? jjT("") : jjT("XYZ333")));
            JJ_TEST(log.Line == scopeline2);
        }
        ++cnt;
    };
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(std::make_shared<testTargetCb>(fn));
    jj::log::logger_t::instance().setLevel(JJ_LOGLEVEL_SCOPE);
    int result = scopemethod2(true, false);
    JJ_TEST(result == 555);
    result = scopemethod2(false, false);
    JJ_TEST(result == 666);
    result = scopemethod2(true, true);
    JJ_TEST(result == 555);
    result = scopemethod2(false, true);
    JJ_TEST(result == 666);
    JJ_TEST(cnt == 8);
    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CASE(scope_returnvoid)
{
    int cnt = 0;
    auto fn = [&](const jj::log::message_t& log) {
        JJ_TEST(log.Level == JJ_LOGLEVEL_SCOPE);
        if (cnt == 0 || cnt == 2)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_ENTER));
            JJ_TEST(log.Message == jjT("A5Z"));
            JJ_TEST(log.Line == scopeline1);
        }
        else if (cnt == 1 || cnt == 3)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_LEAVE));
            JJ_TEST(log.Message == (cnt == 1 ? jjT("") : jjT("KLM111")));
            JJ_TEST(log.Line == scopeline2);
        }
        ++cnt;
    };
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(std::make_shared<testTargetCb>(fn));
    jj::log::logger_t::instance().setLevel(JJ_LOGLEVEL_SCOPE);
    scopemethod1(true, false);
    scopemethod1(true, true);
    JJ_TEST(cnt == 4);
    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CASE(scope_throw)
{
    int cnt = 0;
    auto fn = [&](const jj::log::message_t& log) {
        JJ_TEST(log.Level == JJ_LOGLEVEL_SCOPE);
        if (cnt % 2 == 0)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_ENTER));
            JJ_TEST(log.Message == jjT("T"));
            JJ_TEST(log.Line == scopeline1);
        }
        else if (cnt == 1 || cnt == 3)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_LEAVE));
            JJ_TEST(log.Message == (cnt == 1 ? jjT("exception was thrown") : jjT("something")));
            JJ_TEST(log.Line == scopeline2);
        }
        else if (cnt == 5 || cnt == 7)
        {
            JJ_TEST(jj::str::equal(log.LevelName, jj::log::logger_t::NAME_LEAVE));
            JJ_TEST(log.Message == (cnt == 5 ? jjT("exception was thrown; EX") : jjT("something; EXCE")));
            JJ_TEST(log.Line == scopeline2);
        }
        ++cnt;
    };
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(std::make_shared<testTargetCb>(fn));
    jj::log::logger_t::instance().setLevel(JJ_LOGLEVEL_SCOPE);
    try { scopemethod3(false, false); } catch (...) {}
    try { scopemethod3(false, true); } catch (...) {}
    try { scopemethod3(true, false); } catch (...) {}
    try { scopemethod3(true, true); } catch (...) {}
    JJ_TEST(cnt == 8);
    jj::log::logger_t::instance().replaceTargets(olog);
}

JJ_TEST_CLASS_END(logTests_t, fields, levels, message1, message2, message3, recursion, multiple_targets, targets_switch, \
    scope_default, scope_return, scope_returnvoid, scope_throw)

template<typename T>
void logRoot(const T& v)
{
    jjLscope(v);
    jjLE(v);
    jjLW(v);
    jjLI(v);
    jjLV(v);
    return jjLLmsg(v);
}

namespace logA
{
JJ_DECLARE_LOG_COMPONENT(compA);
JJ_REFERENCE_LOG_COMPONENT(compA)
template<typename T>
void method(const T& v)
{
    jjLscope(v);
    jjLE(v);
    jjLW(v);
    jjLI(v);
    jjLV(v);
    return jjLLmsg(v);
}
}

namespace logB
{
JJ_DEFINE_LOG_COMPONENT(compB)
template<typename T>
void method(const T& v)
{
    jjLscope(v);
    jjLE(v);
    jjLW(v);
    jjLI(v);
    jjLV(v);
    return jjLLmsg(v);
}
}

namespace logC
{
JJ_DEFINE_LOG_COMPONENT(compC,"COMPOT",JJ_LOGLEVEL_WARNING)
template<typename T>
void method(const T& v)
{
    jjLscope(v);
    jjLE(v);
    jjLW(v);
    jjLI(v);
    jjLV(v);
    return jjLLmsg(v);
}
}

namespace logA
{
template<typename T>
void method2(const T& v)
{
    jjLscope(v);
    jjLE(v);
    jjLW(v);
    jjLI(v);
    jjLV(v);
    return jjLLmsg(v);
}
}

void resetCounts(size_t& v1, size_t& v2, size_t& v3, size_t& v4)
{
    v1 = 0;
    v2 = 0;
    v3 = 0;
    v4 = 0;
}

JJ_TEST_CLASS(logComponentTests_t)

JJ_TEST_CASE_VARIANTS(components_and_levels, (jj::log::level_t limitRoot, jj::log::level_t limitB), (JJ_LOGLEVEL_INFO, JJ_LOGLEVEL_INFO), (JJ_LOGLEVEL_SCOPE, JJ_LOGLEVEL_SCOPE))
{
    jj::string_t cmain(jjT("<main>")), cA(jjT("compA")), cB(jjT("compB")), cC(jjT("COMPOT"));
    jj::log::level_t lmain(limitRoot), lA(JJ_LOGLEVEL_INFO), lB(limitB), lC(JJ_LOGLEVEL_INFO);
    size_t Cmain, CA, CB, CC;
    auto fn = [&](const jj::log::message_t& log) {
        if (log.Message == jjT("333"))
        {
            JJ_TEST(log.Component.name() == cmain);
            JJ_TEST(log.Level >= lmain);
            ++Cmain;
        }
        else if (log.Message == jjT("444"))
        {
            JJ_TEST(log.Component.name() == cA);
            JJ_TEST(log.Level >= lA);
            ++CA;
        }
        else if (log.Message == jjT("555"))
        {
            JJ_TEST(log.Component.name() == cB);
            JJ_TEST(log.Level >= lB);
            ++CB;
        }
        else if (log.Message == jjT("666"))
        {
            JJ_TEST(log.Component.name() == cC);
            JJ_TEST(log.Level >= lC);
            ++CC;
        }
        else if (log.Message == jjT("777"))
        {
            JJ_TEST(log.Component.name() == cmain);
            JJ_TEST(log.Level >= lmain);
            ++Cmain;
        }
        else if (log.Message == jjT("888"))
        {
            JJ_TEST(log.Component.name() == cA);
            JJ_TEST(log.Level >= lA);
            ++CA;
        }
    };
    std::shared_ptr<jj::log::logTarget_base_t> olog = jj::log::logger_t::instance().replaceTargets(std::make_shared<testTargetCb>(fn));
    jj::log::logger_t::instance().setLevel(limitRoot);
    logB::compBComponent_t::instance().setLevel(limitB);

    resetCounts(Cmain, CA, CB, CC);
    logRoot(333);
    JJ_TEST(Cmain == 3 + (lmain <= JJ_LOGLEVEL_VERBOSE ? 1 : 0) + (lmain <= JJ_LOGLEVEL_SCOPE ? 2 : 0));
    JJ_TEST(CA == 0);
    JJ_TEST(CB == 0);
    JJ_TEST(CC == 0);

    resetCounts(Cmain, CA, CB, CC);
    logA::method(444);
    JJ_TEST(Cmain == 0);
    JJ_TEST(CA == 3);
    JJ_TEST(CB == 0);
    JJ_TEST(CC == 0);

    resetCounts(Cmain, CA, CB, CC);
    logB::method(555);
    JJ_TEST(Cmain == 0);
    JJ_TEST(CA == 0);
    JJ_TEST(CB == 3 + (lB <= JJ_LOGLEVEL_VERBOSE ? 1 : 0) + (lB <= JJ_LOGLEVEL_SCOPE ? 2 : 0));
    JJ_TEST(CC == 0);

    resetCounts(Cmain, CA, CB, CC);
    logC::method(666);
    JJ_TEST(Cmain == 0);
    JJ_TEST(CA == 0);
    JJ_TEST(CB == 0);
    JJ_TEST(CC == 2);

    resetCounts(Cmain, CA, CB, CC);
    jjLE(777);
    jjLW(777);
    jjLI(777);
    jjLV(777);
    JJ_TEST(Cmain == 3 + (lmain <= JJ_LOGLEVEL_VERBOSE ? 1 : 0));
    JJ_TEST(CA == 0);
    JJ_TEST(CB == 0);
    JJ_TEST(CC == 0);

    resetCounts(Cmain, CA, CB, CC);
    logA::method2(888);
    JJ_TEST(Cmain == 0);
    JJ_TEST(CA == 3);
    JJ_TEST(CB == 0);
    JJ_TEST(CC == 0);
    jj::log::logger_t::instance().replaceTargets(olog);
}

// TODO add dedicated (multifile component tests)

JJ_TEST_CLASS_END(logComponentTests_t, components_and_levels)
