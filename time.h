#ifndef JJ_TIME_H
#define JJ_TIME_H

#include "jj/defines.h"
#include "jj/stringLiterals.h"
#include <chrono>
#include <type_traits>
#include <time.h>
#include <iosfwd>
#include <iomanip>

namespace jj
{
namespace time
{

/*! Converts the clock (usually the std::chrono::system_clock) to a structure where user can
directly access the date and time and additional info.
See also jj::time::now() and jj::time::moment_t. */
class stamp_t
{
    bool valid_; //!< set to true only after all other members are successfully parsed and set in ctor
    typedef std::intmax_t ticks_t; //!< a "large" ordinal type to hold number of ns
    typedef int small_t; //!< a "small" ordinal type to hold date and time info
    ticks_t ticks_; //!< number of ns since epoch (beginning of January 1st, 1970)
    small_t year_, //!< year (full, eg. 1989 or 2013)
        month_, //!< month (starts at 1=January, through 12=December)
        day_, //!< day (starts at 1)
        yday_, //!< day within year (starts at 0)
        wday_, //!< day within week (0=Sunday, 1=Monday, ... 6=Saturday)
        hour_, //!< hour (0-23)
        min_, //!< minute (0-59)
        sec_, //!< second (0-59)
        ns_, //!< nanosecond (0-999999999), precision may differ per system
        offsec_; //!< timezone offset to UTC in seconds

public:
    /* Local time or UTC. */
    enum type_t
    {
        LOCAL, //!< selects local time
        UTC //!< selects universal time coordinates
    };

    /*! Ctor - takes a time_point (which will be usually coming from std::chrono::system_clock::now() and
    converts it to the internal members for year, month, ..., second, ... etc.
    Based on type the resulting values will be either in local timezone or UTC.
    Check the isValid(), it only returns true if all members could be parsed and set successfully. */
    template<typename T, typename D>
    stamp_t(std::chrono::time_point<T, D> t, type_t type = LOCAL)
        : valid_(false), ticks_(0), year_(-1), month_(-1), day_(-1), yday_(-1), wday_(-1), hour_(-1), min_(-1), sec_(-1), ns_(-1), offsec_(0)
    {
        // first split into seconds and fraction
        static_assert(std::is_same<typename T::rep, typename D::rep>::value, "Clock T and duration D must have same representation types.");
        typedef typename D::rep rep_t;
        typedef typename D::period per_t;
        static_assert(per_t::den > 0, "Weird clock.");
        static const std::intmax_t ns = 1000000000l;
        typedef std::ratio<ns, per_t::den> ns_t;
        ticks_ = t.time_since_epoch().count() * per_t::num * ns_t::num / ns_t::den;
        rep_t ts = ticks_ / ns;

        // convert the seconds part
        time_t tt(ts);
        tm tx{};
#ifdef JJ_OS_WINDOWS
        errno_t err = (type == LOCAL ? localtime_s(&tx, &tt) : gmtime_s(&tx, &tt));
        if (err != 0)
            return;
#else
        tm* out = (type == LOCAL ? localtime_r(&tt, &tx) : gmtime_r(&tt, &tx));
        if (out == NULL)
            return;
#endif
        year_ = tx.tm_year + 1900;
        month_ = tx.tm_mon + 1;
        day_ = tx.tm_mday;
        yday_ = tx.tm_yday;
        wday_ = tx.tm_wday;
        hour_ = tx.tm_hour;
        min_ = tx.tm_min;
        sec_ = tx.tm_sec;

        // set the fraction
        ns_ = ticks_ % ns;

        // set the timezone offset
        long tz(0);
        if (type == LOCAL)
        {
#ifdef JJ_OS_WINDOWS
            err = _get_timezone(&tz);
            if (err != 0)
                return;
#else
            tz = timezone;
#endif
        }
        offsec_ = tz * -1;

        // all done
        valid_ = true;
    }

    /*! Returns true if all other member methods return valid values (if all members could be parsed from
    the ctor parameter), false otherwise. If false is returned none of the values returned by other member
    methods can't be trusted. */
    bool isValid() const { return valid_; }

    /*! Returns the full year (eg. 1989 or 2013). */
    small_t year() const { return year_; }
    /*! Returns month (starts at 1=January, through 12=December). */
    small_t month() const { return month_; }
    /*! Returns day withing month (starts at 1). */
    small_t day() const { return day_; }

    /*! Returns day within year (starts at 0). */
    small_t yearDay() const { return yday_; }
    /*! Returns day within week (0=Sunday, 1=Monday, ... 6=Saturday). */
    small_t weekDay() const { return wday_; }

    /*! Returns hour (0-23). */
    small_t hour() const { return hour_; }
    /*! Returns minute (0-59). */
    small_t minute() const { return min_; }
    /*! Returns second (0-59). No leap seconds. */
    small_t second() const { return sec_; }
    /*! Returns nanosecond (0-999999999), precision may differ per system. */
    small_t nanosecond() const { return ns_; }

    /*! Returns offset in seconds of the system timezone compared to UTC.
    If UTC was passed as type to ctor, this is 0 of course. 
    The offset can be positive or negative (and is the intuitive way as local - UTC);
    eg. 3600 for CET, 0 for GMT, 19800 for IST or -21600 for CST.
    Please note that the timezone information (and thereby all the other fields)
    depend on the TZ environmental variable if it is set! */
    small_t offset() const { return offsec_; }
    /*! Returns 0 if offset() is 0, -1 is offset() is negative and 1 if offset() is positive. */
    small_t offsetDirection() const { return (offsec_ == 0 ? 0 : (offsec_ > 0 ? 1 : -1)); }
    /*! Returns the hours part of offset(), always positive - check offsetDirection(). */
    small_t offsetHours() const { return (offsec_ < 0 ? (offsec_ / 3600) * -1 : (offsec_ / 3600)); }
    /*! Returns the minutes part of offset(), always positive - check offsetDirection(). */
    small_t offsetMinutes() const { return (offsec_ < 0 ? ((offsec_ % 3600) / 60) * -1 : ((offsec_ % 3600) / 60)); }

    /*! Returns the number of nanoseconds since epoch (=since 19700101T000000+0000). */
    ticks_t ticks() const { return ticks_; }
};

/*! Return the current local or UTC time (or equivalent based on the template parameter). */
template<typename CLOCK = std::chrono::system_clock>
stamp_t now(stamp_t::type_t type = stamp_t::LOCAL) { return stamp_t(CLOCK::now(), type); }

/*! Converts the clock (usually std::chrono::steady_clock) to a structure where user can
directly access days, hours, ..., nanoseconds.
See also jj::time::uptime(). */
class moment_t
{
    bool valid_; //!< set to true only after all other members are successfully parsed and set in ctor
    typedef std::intmax_t ticks_t; //!< a "large" ordinal type to hold number of ns
    typedef int small_t; //!< a "small" ordinal type to hold date and time info
    ticks_t ticks_; //!< number of ns since the beginning of the clock (which usually is system start)
    small_t days_, //!< days part since start
        hours_, //!< hours part since start
        mins_, //!< minutes part since start
        secs_, //!< seconds part since start
        nss_; //!< nanoseconds part since start

public:
    /*! Ctor - takes a time_point (which will be usually coming from std::chrono::steady_clock::now() and
    converts it to the internal members for days, hours, ...
    Check the isValid(), it only returns true if all members could be parsed and set successfully. */
    template<typename T, typename D>
    moment_t(std::chrono::time_point<T, D> t)
        : valid_(false), ticks_(0), days_(-1), hours_(-1), mins_(-1), secs_(-1), nss_(-1)
    {
        // first split into seconds and fraction
        static_assert(std::is_same<typename T::rep, typename D::rep>::value, "Clock T and duration D must have same representation types.");
        typedef typename D::rep rep_t;
        typedef typename D::period per_t;
        static_assert(per_t::den > 0, "Weird clock.");
        static const std::intmax_t ns = 1000000000l;
        typedef std::ratio<ns, per_t::den> ns_t;
        ticks_ = t.time_since_epoch().count() * per_t::num * ns_t::num / ns_t::den;
        rep_t ts = ticks_ / ns;

        // convert the seconds part
        time_t tt(ts);
        tm tx{};
#ifdef JJ_OS_WINDOWS
        errno_t err = gmtime_s(&tx, &tt);
        if (err != 0)
            return;
#else
        tm* out = gmtime_r(&tt, &tx);
        if (out == NULL)
            return;
#endif
        days_ = (tx.tm_year - 70) * 365 /*TODO add leap years */ + tx.tm_yday;
        hours_ = tx.tm_hour;
        mins_ = tx.tm_min;
        secs_ = tx.tm_sec;

        // set the fraction
        nss_ = (ticks_ % ns);

        valid_ = true;
    }

    /*! Returns true if all other member methods return valid values (if all members could be parsed from
    the ctor parameter), false otherwise. If false is returned none of the values returned by other member
    methods can't be trusted. */
    bool isValid() const { return valid_; }

    /*! Returns the days part of the object. */
    small_t days() const { return days_; }

    /*! Returns the hours part of the object. */
    small_t hours() const { return hours_; }
    /*! Returns the minutes part of the object. */
    small_t minutes() const { return mins_; }
    /*! Returns the seconds part of the object. */
    small_t seconds() const { return secs_; }
    /*! Returns the nanoseconds part of the object. */
    small_t nanoseconds() const { return nss_; }

    /*! Returns the number of total nanoseconds since the beginning of clock (=usually system start). */
    ticks_t ticks() const { return ticks_; }
};

/*! Return the current time system (or equivalent based on the template parameter). */
template<typename CLOCK = std::chrono::steady_clock>
moment_t uptime() { return moment_t(CLOCK::now()); }

} // namespace time
} // namespace jj

template<typename CH, typename TR>
std::basic_ostream<CH, TR>& operator<<(std::basic_ostream<CH, TR>& s, const jj::time::stamp_t& v)
{
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(4) << v.year();
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.month();
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.day();
    s << jj::str::literals_t<CH>::T;
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.hour();
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.minute();
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.second();
    s << jj::str::literals_t<CH>::DOT;
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.nanosecond();
    s << (v.offsetDirection() < 0 ? jj::str::literals_t<CH>::MINUS : jj::str::literals_t<CH>::PLUS);
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.offsetHours();
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.offsetMinutes();
    return s;
}

template<typename CH, typename TR>
std::basic_ostream<CH, TR>& operator<<(std::basic_ostream<CH, TR>& s, const jj::time::moment_t& v)
{
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.days();
    s << jj::str::literals_t<CH>::T;
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.hours();
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.minutes();
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.seconds();
    s << jj::str::literals_t<CH>::DOT;
    s << std::setfill(jj::str::literals_t<CH>::d0) << std::setw(2) << v.nanoseconds();
    return s;
}

#endif // JJ_TIME_H
