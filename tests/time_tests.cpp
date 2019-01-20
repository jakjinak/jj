#include "jj/time.h"
#include "jj/test/test.h"
#ifdef JJ_OS_WINDOWS
#include <Windows.h>
#endif

JJ_TEST_CLASS(timeRealTests_t)

struct stampchecker
{
    enum type_t
    {
        LOCAL,
        UTC,
        UPTIME
    };
    stampchecker(type_t type)
    {
        time_t t;
        if (type == LOCAL || type == UTC)
            time(&t);
        else // UPTIME
        {
#ifdef JJ_OS_WINDOWS
            LARGE_INTEGER cnt, frq;
            QueryPerformanceCounter(&cnt);
            QueryPerformanceFrequency(&frq);
            t = cnt.QuadPart / frq.QuadPart;
#else
            timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            t = ts.tv_sec;
#endif
        }
        tm ti;
#ifdef JJ_OS_WINDOWS
        if (type == LOCAL)
            localtime_s(&ti, &t);
        else
            gmtime_s(&ti, &t);
#else
        if (type == LOCAL)
            localtime_r(&t, &ti);
        else
            gmtime_r(&t, &ti);
#endif
        Y = ti.tm_year + 1900;
        M = ti.tm_mon + 1;
        D = ti.tm_mday;
        yD = ti.tm_yday;
        wD = ti.tm_wday;
        h = ti.tm_hour;
        m = ti.tm_min;
        s = ti.tm_sec;
    }
    int Y, M, D, yD, wD, h, m, s;
};

template<typename CLOCK>
void stamptest(stampchecker::type_t type, jj::time::stamp_t::type_t stype = jj::time::stamp_t::LOCAL)
{
    jj::time::stamp_t s(CLOCK::now(), stype);
    stampchecker c(type);
    JJ_TEST(s.isValid());
    JJ_TEST(s.year() == c.Y, jjOOO(s.year(),==,c.Y));
    JJ_TEST(s.month() == c.M, jjOOO(s.month(),==,c.M));
    JJ_TEST(s.day() == c.D, jjOOO(s.day(),==,c.D));
    JJ_TEST(s.yearDay() == c.yD, jjOOO(s.yearDay(),==,c.yD));
    JJ_TEST(s.weekDay() == c.wD, jjOOO(s.weekDay(),==,c.wD));
    JJ_TEST(s.hour() == c.h, jjOOO(s.hour(),==,c.h));
    JJ_TEST(s.minute() == c.m, jjOOO(s.minute(),==,c.m));
    if (s.second() + 1 == c.s || c.s + 59 == s.second())
    {
        JJ_WARN(s.second() == c.s, jjT("Differs by one second. Tests skipped."));
        return;
    }
    else if (s.second() == c.s + 1 || c.s == s.second() + 59)
    {
        // strangely the dummy clock is off by 1 second under Windows in many cases, considered ok
        JJ_WARN(s.second() == c.s, jjOOO(s.second(),==,c.s));
    }
    else
    {
        JJ_TEST(s.second() == c.s, jjOOO(s.second(),==,c.s));
    }
}

JJ_TEST_CASE(stamp_current_systime)
{
    stamptest<std::chrono::system_clock>(stampchecker::LOCAL);
}

JJ_TEST_CASE(stamp_current_systime_utc)
{
    stamptest<std::chrono::system_clock>(stampchecker::UTC, jj::time::stamp_t::UTC);
}

JJ_TEST_CASE(stamp_current_steady)
{
    stamptest<std::chrono::steady_clock>(stampchecker::UPTIME, jj::time::stamp_t::UTC);
}

JJ_TEST_CASE(stamp_current_hr)
{
    stamptest<std::chrono::high_resolution_clock>(std::chrono::high_resolution_clock::is_steady ? stampchecker::UPTIME : stampchecker::UTC, jj::time::stamp_t::UTC);
}

struct momentchecker
{
    enum type_t
    {
        UTC,
        UPTIME
    };
    momentchecker(type_t type)
    {
        time_t t;
        if (type == UTC)
            time(&t);
        else // UPTIME
        {
#ifdef JJ_OS_WINDOWS
            LARGE_INTEGER cnt, frq;
            QueryPerformanceCounter(&cnt);
            QueryPerformanceFrequency(&frq);
            t = cnt.QuadPart / frq.QuadPart;
#else
            timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            t = ts.tv_sec;
#endif
        }
        tm ti;
#ifdef JJ_OS_WINDOWS
        gmtime_s(&ti, &t);
#else
        gmtime_r(&t, &ti);
#endif
        d = (ti.tm_year - 70) * 365 + ti.tm_yday;
        h = ti.tm_hour;
        m = ti.tm_min;
        s = ti.tm_sec;
    }
    int d, h, m, s;
};

template<typename CLOCK>
void momenttest(momentchecker::type_t type)
{
    jj::time::moment_t s(CLOCK::now());
    momentchecker c(type);
    JJ_TEST(s.isValid());
    JJ_TEST(s.days() == c.d, jjOOO(s.days(),==,c.d));
    JJ_TEST(s.hours() == c.h, jjOOO(s.hours(),==,c.h));
    JJ_TEST(s.minutes() == c.m, jjOOO(s.minutes(),==,c.m));
    if (s.seconds() + 1 == c.s || c.s + 59 == s.seconds())
    {
        JJ_WARN(s.seconds() == c.s, jjT("Differs by one second. Tests skipped."));
        return;
    }
    else if (s.seconds() == c.s + 1 || c.s == s.seconds() + 59)
    {
        // strangely the dummy clock is off by 1 second under Windows in many cases, considered ok
        JJ_WARN(s.seconds() == c.s, jjOOO(s.seconds(),==,c.s));
    }
    else
    {
        JJ_TEST(s.seconds() == c.s, jjOOO(s.seconds(),==,c.s));
    }
}

JJ_TEST_CASE(moment_current_steady)
{
    momenttest<std::chrono::steady_clock>(momentchecker::UPTIME);
}

JJ_TEST_CASE(moment_current_systime)
{
    momenttest<std::chrono::system_clock>(momentchecker::UTC);
}

JJ_TEST_CASE(moment_current_hr)
{
    momenttest<std::chrono::high_resolution_clock>(std::chrono::high_resolution_clock::is_steady ? momentchecker::UPTIME : momentchecker::UTC);
}

JJ_TEST_CLASS_END(timeRealTests_t, stamp_current_systime, stamp_current_systime_utc, stamp_current_steady, stamp_current_hr, \
    moment_current_steady, moment_current_systime, moment_current_hr)

template<long long NUM = 1ll, long long DENOM = 1000000000ll, bool STEADY = false>
struct fake_clock
{
    using rep = long long;
    using period = std::ratio<NUM, DENOM>;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<fake_clock, duration>;
    static constexpr bool is_steady = STEADY;

    static time_point now() { return time_point(duration(Ticks)); }

    static rep Ticks;
};

template<long long NUM, long long DENOM, bool STEADY>
typename fake_clock<NUM, DENOM, STEADY>::rep fake_clock<NUM, DENOM, STEADY>::Ticks = 0;

JJ_TEST_CLASS(timeFakeTests_t)

JJ_TEST_CASE_VARIANTS(momentZero, (long long ticks), (0), (1), (123456789), (999999999))
{
    fake_clock<>::Ticks = ticks;
    jj::time::moment_t t = jj::time::moment_t(fake_clock<>::now());
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == ticks, jjOOO(t.ticks(),==,ticks));
    JJ_TEST(t.days() == 0, jjOOO(t.days(),==,0));
    JJ_TEST(t.hours() == 0, jjOOO(t.hours(),==,0));
    JJ_TEST(t.minutes() == 0, jjOOO(t.minutes(),==,0));
    JJ_TEST(t.seconds() == 0, jjOOO(t.seconds(),==,0));
    JJ_TEST(t.nanoseconds() == ticks, jjOOO(t.nanoseconds(),==,ticks));
}

JJ_TEST_CASE(moment_resolution_1000th)
{
    typedef fake_clock<1ll, 1000ll> clock;
    clock::Ticks = 987;
    jj::time::moment_t t = jj::time::moment_t(clock::now());
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == 987000000, jjOOO(t.ticks(),==,987000000));
    JJ_TEST(t.days() == 0, jjOOO(t.days(),==,0));
    JJ_TEST(t.hours() == 0, jjOOO(t.hours(),==,0));
    JJ_TEST(t.minutes() == 0, jjOOO(t.minutes(),==,0));
    JJ_TEST(t.seconds() == 0, jjOOO(t.seconds(),==,0));
    JJ_TEST(t.nanoseconds() == 987000000, jjOOO(t.nanoseconds(),==,987000000));
}

JJ_TEST_CASE(moment_resolution_10Mth)
{
    typedef fake_clock<1ll, 10000000ll> clock;
    clock::Ticks = 9876543;
    jj::time::moment_t t = jj::time::moment_t(clock::now());
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == 987654300, jjOOO(t.ticks(),==,987654300));
    JJ_TEST(t.days() == 0, jjOOO(t.days(),==,0));
    JJ_TEST(t.hours() == 0, jjOOO(t.hours(),==,0));
    JJ_TEST(t.minutes() == 0, jjOOO(t.minutes(),==,0));
    JJ_TEST(t.seconds() == 0, jjOOO(t.seconds(),==,0));
    JJ_TEST(t.nanoseconds() == 987654300, jjOOO(t.nanoseconds(),==,987654300));
}

JJ_TEST_CASE(moment_resolution_100sec)
{
    typedef fake_clock<100ll, 1ll> clock;
    clock::Ticks = 1;
    jj::time::moment_t t = jj::time::moment_t(clock::now());
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == 100000000000ll, jjOOO(t.ticks(),==,100000000000ll));
    JJ_TEST(t.days() == 0, jjOOO(t.days(),==,0));
    JJ_TEST(t.hours() == 0, jjOOO(t.hours(),==,0));
    JJ_TEST(t.minutes() == 1, jjOOO(t.minutes(),==,1));
    JJ_TEST(t.seconds() == 40, jjOOO(t.seconds(),==,40));
    JJ_TEST(t.nanoseconds() == 0, jjOOO(t.nanoseconds(),==,0));
}

JJ_TEST_CASE_VARIANTS(stampZero, (long long ticks), (0), (1), (123456789), (999999999))
{
    fake_clock<>::Ticks = ticks;
    jj::time::stamp_t t = jj::time::stamp_t(fake_clock<>::now(), jj::time::stamp_t::UTC);
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == ticks, jjOOO(t.ticks(),==,ticks));
    JJ_TEST(t.year() == 1970, jjOOO(t.year(),==,1970));
    JJ_TEST(t.month() == 1, jjOOO(t.month(),==,1));
    JJ_TEST(t.day() == 1, jjOOO(t.day(),==,1));
    JJ_TEST(t.hour() == 0, jjOOO(t.hour(),==,0));
    JJ_TEST(t.minute() == 0, jjOOO(t.minute(),==,0));
    JJ_TEST(t.second() == 0, jjOOO(t.second(),==,0));
    JJ_TEST(t.nanosecond() == ticks, jjOOO(t.nanosecond(),==,ticks));
}

JJ_TEST_CASE(stamp_resolution_1000th)
{
    typedef fake_clock<1ll, 1000ll> clock;
    clock::Ticks = 987;
    jj::time::stamp_t t = jj::time::stamp_t(clock::now(), jj::time::stamp_t::UTC);
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == 987000000, jjOOO(t.ticks(),==,987000000));
    JJ_TEST(t.year() == 1970, jjOOO(t.year(),==,1970));
    JJ_TEST(t.month() == 1, jjOOO(t.month(),==,1));
    JJ_TEST(t.day() == 1, jjOOO(t.day(),==,1));
    JJ_TEST(t.hour() == 0, jjOOO(t.hour(),==,0));
    JJ_TEST(t.minute() == 0, jjOOO(t.minute(),==,0));
    JJ_TEST(t.second() == 0, jjOOO(t.second(),==,0));
    JJ_TEST(t.nanosecond() == 987000000, jjOOO(t.nanosecond(),==,987000000));
}

JJ_TEST_CASE(stamp_resolution_10Mth)
{
    typedef fake_clock<1ll, 10000000ll> clock;
    clock::Ticks = 9876543;
    jj::time::stamp_t t = jj::time::stamp_t(clock::now(), jj::time::stamp_t::UTC);
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == 987654300, jjOOO(t.ticks(),==,987654300));
    JJ_TEST(t.year() == 1970, jjOOO(t.year(),==,1970));
    JJ_TEST(t.month() == 1, jjOOO(t.month(),==,1));
    JJ_TEST(t.day() == 1, jjOOO(t.day(),==,1));
    JJ_TEST(t.hour() == 0, jjOOO(t.hour(),==,0));
    JJ_TEST(t.minute() == 0, jjOOO(t.minute(),==,0));
    JJ_TEST(t.second() == 0, jjOOO(t.second(),==,0));
    JJ_TEST(t.nanosecond() == 987654300, jjOOO(t.nanosecond(),==,987654300));
}

JJ_TEST_CASE(stamp_resolution_100sec)
{
    typedef fake_clock<100ll, 1ll> clock;
    clock::Ticks = 1;
    jj::time::stamp_t t = jj::time::stamp_t(clock::now(), jj::time::stamp_t::UTC);
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == 100000000000ll, jjOOO(t.ticks(),==,100000000000ll));
    JJ_TEST(t.year() == 1970, jjOOO(t.year(),==,1970));
    JJ_TEST(t.month() == 1, jjOOO(t.month(),==,1));
    JJ_TEST(t.day() == 1, jjOOO(t.day(),==,1));
    JJ_TEST(t.hour() == 0, jjOOO(t.hour(),==,0));
    JJ_TEST(t.minute() == 1, jjOOO(t.minute(),==,1));
    JJ_TEST(t.second() == 40, jjOOO(t.second(),==,40));
    JJ_TEST(t.nanosecond() == 0, jjOOO(t.nanosecond(),==,0));
}

JJ_TEST_CASE_VARIANTS(stamp_dates, (long long ticks, int Y, int M, int D, int h, int m, int s, int ns), \
    (0ll, 1970, 1, 1, 0, 0, 0, 0), \
    (86399ll, 1970, 1, 1, 23, 59, 59, 0), \
    (946684799ll, 1999, 12, 31, 23, 59, 59, 0), \
    (946684800ll, 2000, 1, 1, 0, 0, 0, 0), \
    (2222601663ll, 2040, 6, 6, 13, 21, 3, 0), \
    (1077991563ll, 2004, 2, 28, 18, 6, 3, 0), \
    (1078077963ll, 2004, 2, 29, 18, 6, 3, 0), \
    (4107553260ll, 2100, 3, 1, 3, 1, 0, 0), \
    (4139078463ll, 2101, 3, 1, 0, 1, 3, 0))
{
    typedef fake_clock<1ll, 1ll> clock;
    clock::Ticks = ticks;
    jj::time::stamp_t t = jj::time::stamp_t(clock::now(), jj::time::stamp_t::UTC);
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == ticks * 1000000000ll, jjOOO(t.ticks(),==,ticks*1000000000ll));
    JJ_TEST(t.year() == Y, jjOOO(t.year(),==,Y));
    JJ_TEST(t.month() == M, jjOOO(t.month(),==,M));
    JJ_TEST(t.day() == D, jjOOO(t.day(),==,D));
    JJ_TEST(t.hour() == h, jjOOO(t.hour(),==,h));
    JJ_TEST(t.minute() == m, jjOOO(t.minute(),==,m));
    JJ_TEST(t.second() == s, jjOOO(t.second(),==,s));
    JJ_TEST(t.nanosecond() == ns, jjOOO(t.nanosecond(),==,ns));
}

JJ_TEST_CASE_VARIANTS(stamp_dates_negative, (long long ticks, int Y, int M, int D, int h, int m, int s, int ns), \
    (-2180177267ll, 1900, 11, 30, 11, 12, 13, 0), \
    (-2208988801ll, 1899, 12, 31, 23, 59, 59, 0))
{
    typedef fake_clock<1ll, 1ll> clock;
    clock::Ticks = ticks;
    jj::time::stamp_t t = jj::time::stamp_t(clock::now(), jj::time::stamp_t::UTC);
#ifdef JJ_OS_WINDOWS
    JJ_TEST(!t.isValid());
#else
    JJ_TEST(t.isValid());
    JJ_TEST(t.ticks() == ticks * 1000000000ll, jjOOO(t.ticks(),==,ticks*1000000000ll));
    JJ_TEST(t.year() == Y, jjOOO(t.year(),==,Y));
    JJ_TEST(t.month() == M, jjOOO(t.month(),==,M));
    JJ_TEST(t.day() == D, jjOOO(t.day(),==,D));
    JJ_TEST(t.hour() == h, jjOOO(t.hour(),==,h));
    JJ_TEST(t.minute() == m, jjOOO(t.minute(),==,m));
    JJ_TEST(t.second() == s, jjOOO(t.second(),==,s));
    JJ_TEST(t.nanosecond() == ns, jjOOO(t.nanosecond(),==,ns));
#endif
}


JJ_TEST_CLASS_END(timeFakeTests_t, momentZero, moment_resolution_1000th, moment_resolution_10Mth, moment_resolution_100sec, \
    stampZero, stamp_resolution_1000th, stamp_resolution_10Mth, stamp_resolution_100sec, stamp_dates, stamp_dates_negative)
