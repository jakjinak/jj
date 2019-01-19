#ifndef JJ_LOG_H
#define JJ_LOG_H

#include "jj/defines.h"
#include "jj/string.h"
#include "jj/stream.h"
#include "jj/singleton.h"
#include <sstream>
#include <climits>
#include <list>
#include <chrono>
#include <memory>

/*! Logging system
Provides logging macros to log on various logging levels into various logging targets.

The logger
The center point of the system is the logger_t singleton. It allows the setting of log level
and holds all registered logging targets.
The logger_t is created on first usage of the singleton. On its creation it calls 
the jj::log::initialize(jj::log::logger_t&) which must be implemented in the program. Use
the JJ_LOGGER_INITIALIZER macro.
Few additional macros can be used to tweak the behavior of the logger:
JJ_LOG_STREAM_PROVIDER - to convert the message parameter of the logging statement macros to a string the logger
    has to use a jj::osstream_t type of class. A way how the obtain such a stream is defined by the macro.
    The default behaviour provides a new stream for every log message.
    To adjust redefine the macro - it has to be any expression that evaluates to an object taking << operator
    and having a str() method.

The logging levels
The logging level is simply a number in the whole range of the unsigned int type - the higher
the number is the more important the log is; the highest number is reserved to completely disable
logging and called JJ_LOGLEVEL_OFF. Other log levels are defined as JJ_LOGLEVEL_FATAL, JJ_LOGLEVEL_ERROR,
JJ_LOGLEVEL_ALERT, JJ_LOGLEVEL_WARNING, JJ_LOGLEVEL_INFO, JJ_LOGLEVEL_VERBOSE, JJ_LOGLEVEL_DEBUG which
are evenly spread through the unsigned int range.
A special JJ_LOGLEVEL_SCOPE is defined and used for method ENTER / LEAVE logging.

The logging limits
There are two kinds of limits: 
The hard limit defined by JJ_LOGLIMIT_HARD macro defines the log level
under which the logging statements are removed from code, if it's set to JJ_LOGLEVEL_OFF then all
the logging calls and classes are removed at compile time. Redefine the JJ_LOGLIMIT_HARD to adjust.
The soft limit defines the behavior at runtime, it's controlled by the jj::log::logger_t::setLevel()
method. It sets the log level under which the logger calls are discarded at runtime. Retrieve the
current value by jj::log::logger_t::enabled() method.
Note that logger_t methods mentioned above only operate on the main log component. It is possible
to define custom component and adjust their "soft" log limits individually.

The logging components
The usage of components is not mandatory and by default all logging statements use the main component
which is defined in root namespace. The component is available under a variable which is defined
in the JJ_LOG_COMPONENT macro, in special situations you might need to redefine it, see below.
To define a custom component you can use three macros - JJ_DECLARE_LOG_COMPONENT is used to declare the
singleton used with a component (this would be used in a header if same component needs to be shared
among multiple source files), JJ_REFERENCE_LOG_COMPONENT which creates a reference named by JJ_LOG_COMPONENT
in the "local" scope (in an anonymous namespace) and JJ_DEFINE_LOG_COMPONENT which combines the other
two macros.
Components are meant to be defined/referenced within a namespace, should you need to make a custom
component in the root namespace you need to redefine the JJ_LOG_COMPONENT to a different variable name
otherwise you would receive "ambiguous" compiler errors.

The logging statements
Macros jjLF, jjLE, jjLA, jjLW, jjLI, jjLV, jjLD are pre-defined, all taking a message as parameter.
The message is either a single value or a stream (value1 << value2 << ... << valuen).
To define custom log levels use the JJ__LOGGER helper macro and provide the log level and log level name.
Note that the logging statements do nothing if their log level is below the set one, they convert the message
parameter to a jj::log::message_t object that is passed to jj::log::logger_t::log() method which will
distribute it to the individual logging targets.

The logging targets
The logger_t can contain several different targets to which it will distribute all the log messages it
receives. Actually, it has to contain at least one (see JJ_LOGGER_INITIALIZER) otherwise it won't do
anything. Use jj::log::logger_t methods registerTarget() or replaceTargets() to set these.
Each log target shall derive from the logTarget_base_t class.

Logger and multi-threading
The logger defined in this header is not designed for usage in multi-threaded environment. However,
if you want to use it in such an environment make sure:
1) you initialize it (call the instance() method) while still single-threaded
2) do not modify the list of registered log targets after logger is initialized
3) use alternative log targets that are thread-safe
If all are satisfied then the logger works fine even in multi-threaded.
*/

/*! \def JJ_LOGLEVEL_DEBUG
Use this for logs that are normally completely disabled = removed from code. */
/*! \def JJ_LOGLEVEL_SCOPE
This log level is used for the ENTER / LEAVE logs. */
/*! \def JJ_LOGLEVEL_VERBOSE
Use this for more detailed logs. */
/*! \def JJ_LOGLEVEL_INFO
Use this for regular logs. */
/*! \def JJ_LOGLEVEL_WARNING
Use this for logs signalizing that is not expected, but probably not an issue. */
/*! \def JJ_LOGLEVEL_ALERT
Use this for logs signalizing something unexpected which should be checked by the operator. */
/*! \def JJ_LOGLEVEL_ERROR
Use this for logs that are definitely an error. */
/*! \def JJ_LOGLEVEL_FATAL
Use this for logs that are an error after which the program cannot continue. */

#if (UINT_MAX == 4294967295U)
#define JJ_LOGLEVEL_DEBUG    477218591U
#define JJ_LOGLEVEL_SCOPE    954437182U
#define JJ_LOGLEVEL_VERBOSE 1431655773U
#define JJ_LOGLEVEL_INFO    1908874364U
#define JJ_LOGLEVEL_WARNING 2386092955U
#define JJ_LOGLEVEL_ALERT   2863311546U
#define JJ_LOGLEVEL_ERROR   3340530137U
#define JJ_LOGLEVEL_FATAL   3817748728U
#elif (UINT_MAX == 18446744073709551615UL)
#define JJ_LOGLEVEL_DEBUG    2049638230412172401UL
#define JJ_LOGLEVEL_SCOPE    4099276460824344802UL
#define JJ_LOGLEVEL_VERBOSE  6148914691236517203UL
#define JJ_LOGLEVEL_INFO     8198552921648689604UL
#define JJ_LOGLEVEL_WARNING 10248191152060862005UL
#define JJ_LOGLEVEL_ALERT   12297829382473034406UL
#define JJ_LOGLEVEL_ERROR   14347467612885206807UL
#define JJ_LOGLEVEL_FATAL   16397105843297379208UL
#else
#error This operating system not supported. Must have unsigned int of 32 or 64 bit size.
#endif

/*! \def JJ_LOGLEVEL_DEBUG
Used to completely disable all logging. Understand physically exclude all logging from code. */
#define JJ_LOGLEVEL_OFF UINT_MAX

#if JJ_LOGLIMIT_HARD < JJ_LOGLEVEL_OFF
namespace jj
{
namespace log
{
// forward declaration
class logger_t;

// common log types
typedef unsigned int level_t; //!< the type used for log level
typedef const jj::char_t* levelName_t; //!< the type used for log level name
typedef std::chrono::system_clock clock_t; //!< the clock type used for "now"
typedef typename clock_t::time_point timestamp_t; //!< a "now" type

/*! Provides a way how to group logs within a binary.
Also allows to set "soft" log level limit per component.
If components are not used the logger contains a default
central component and its limit is used. */
class component_t
{
    const jj::char_t* id_; //!< the "id" of the component
    const jj::char_t* name_; //!< the "user friendly name" of the component
    level_t level_; //!< log level for the particular component
public:
    /*! Ctor */
    component_t(const jj::char_t* id, const jj::char_t* displayName) : id_(id), name_(displayName), level_(0) { setLevel(JJ_LOGLEVEL_INFO); }

    /*! Returns the id of the component. */
    const jj::char_t* id() const { return id_; }
    /*! Returns the user-friendly name of the component. */
    const jj::char_t* name() const { return name_; }

    /*! Returns whether a given log level is currently enabled (above the "soft" limit). */
    bool enabled(level_t level) const { return level >= level_; }
    /*! Returns the currently set "soft" level limit for log messages in this component. */
    level_t limit() const { return level_; }
    /*! Changes the currently set "soft" log level limit.
    Note that it will be adjusted if it would go below the "hard" limit. */
    void setLevel(level_t level)
    {
        if (level < JJ_LOGLIMIT_HARD)
            level_ = JJ_LOGLIMIT_HARD;
        else
            level_ = level;
    }
};
} // namespace log
} // namespace jj

// TODO JJ_DECLARE_LOG_COMPONENT and JJ_DEFINE_LOG_COMPONENT must be variadic

/*! Defines the name under which the "local" component is to be found.
Note that the main component is in the root namespace. */
#define JJ_LOG_COMPONENT jjTheLogComponent
extern jj::log::component_t& jjTheLogComponent;

#define JJ_DECLARE_LOG_COMPONENT3(id,dname,initll) \
    struct id##Component_t : public jj::singleton_t<id##Component_t>, public jj::log::component_t \
    { \
        id##Component_t() : jj::singleton_t<id##Component_t>(), jj::log::component_t(jjT(#id), jjT(dname)) { setLevel(initll); } \
    }
#define JJ_DECLARE_LOG_COMPONENT2(id,dname) JJ_DECLARE_LOG_COMPONENT3(id,dname,JJ_LOGLEVEL_INFO)
#define JJ_DECLARE_LOG_COMPONENT1(id) JJ_DECLARE_LOG_COMPONENT2(id,#id)
/*! Use this to declare a custom component. This takes up to three parameters - id (used to define
a singleton type representing the actual component, defaults to id), dname (a display name that
can be used by the log target to display info about component) and initll (is the initial log limit,
which defaults to JJ_LOGLEVEL_INFO).*/
#define JJ_DECLARE_LOG_COMPONENT(id) JJ_DECLARE_LOG_COMPONENT1(id)

/*! Use this to reference a component declared by JJ_DECLARE_LOG_COMPONENT.
Note: If you are referencing in global namespace, it might be necessary to redefine the
JJ_LOG_COMPONENT to a different variable name. */
#define JJ_REFERENCE_LOG_COMPONENT(id) \
    namespace /*<anonymous>*/ \
    { \
        jj::log::component_t& JJ_LOG_COMPONENT = id##Component_t::instance(); \
    }

#define JJ_DEFINE_LOG_COMPONENT3(id,dname,initll) \
    JJ_DECLARE_LOG_COMPONENT3(id,dname,initll); \
    JJ_REFERENCE_LOG_COMPONENT(id)
#define JJ_DEFINE_LOG_COMPONENT2(id,dname) JJ_DEFINE_LOG_COMPONENT3(id,dname,JJ_LOGLEVEL_INFO)
#define JJ_DEFINE_LOG_COMPONENT1(id) JJ_DEFINE_LOG_COMPONENT2(id,#id)
/*! Use this to both declare a component and define it as a local name - combines
the JJ_DECLARE_LOG_COMPONENT and JJ_REFERENCE_LOG_COMPONENT macros. */
#define JJ_DEFINE_LOG_COMPONENT(id) JJ_DEFINE_LOG_COMPONENT1(id)

namespace jj
{
namespace log
{
/*! This method has to be defined in a program that uses logging.
Usually this will set the log targets and enabled log level. */
extern void initialize(logger_t&);

/*! Any log statement creates an object of this type. */
struct message_t
{
    timestamp_t Time; //!< the time at which the log occured (auto-set)
    level_t Level; //!< the log level (based on the log statement used)
    levelName_t LevelName; //!< the name of the log level (based on the log statement used)
    jj::string_t Message; //!< the actual log message
    const char* Function; //!< the function in which the log occurred
    const char* File; //!< the file name of the source file in which the log occurred
    int Line; //!< the source file line on which the log occurred
    component_t& Component;

    /*! Ctor */
    message_t(timestamp_t time, level_t level, levelName_t levelName, jj::string_t msg, const char* func, const char* file, int line, component_t& component)
        : Time(time), Level(level), LevelName(levelName), Message(msg), Function(func), File(file), Line(line), Component(component)
    {
    }
};

/*! The default class converting log messages into a single string value. */
struct simpleStreamProvider_t : public AUX::SStreamWrap<jj::osstream_t>
{
};

/*! The base class for all log targets. 
See jj::log::logger_t::registerTarget() or jj::log::logger_t::replaceTargets(). */
class logTarget_base_t
{
public:
    /*! Dtor */
    virtual ~logTarget_base_t() {}
    /*! Invoked whenever a new message arrives. */
    virtual void log(const message_t& log) = 0;
};

/*! A simple log target that logs to standard output. */
class stdoutTarget_t : public logTarget_base_t
{
public:
    virtual void log(const message_t& log) override;
};

/*! A simple log target that logs to standard error output. */
class stderrTarget_t : public logTarget_base_t
{
public:
    virtual void log(const message_t& log) override;
};

/*! The main logging class. */
class logger_t
{
    typedef std::shared_ptr<logTarget_base_t> logTarget_t;
    typedef std::list<logTarget_t> logTargets_t;
    logTargets_t tgts_;

public:
    /*! Returns the instance of the singleton.
    Be warned that creation of the instance is not thread-safe, if you intend to use the logger in a multi-thread
    environment, force the creation by calling instance while still single-threaded. */
    static logger_t& instance()
    {
        static logger_t inst;
        return inst;
    }

    /* Predefined name for the predefined log levels. */
    static const levelName_t NAME_FATAL;
    static const levelName_t NAME_ERROR;
    static const levelName_t NAME_ALERT;
    static const levelName_t NAME_WARNING;
    static const levelName_t NAME_INFO;
    static const levelName_t NAME_VERBOSE;
    static const levelName_t NAME_ENTER;
    static const levelName_t NAME_LEAVE;
    static const levelName_t NAME_DEBUG;

private:
    /*! Ctor - invokes the initializer method, see JJ_LOGGER_INITIALIZER. */
    logger_t() { initialize(*this); }

public:
    /*! Performs a log ignoring the log level checks. Do not use this directly, use the log statement
    macros instead. You might use this directly in a special case, like printing the program version,
    or environment info that might be required at the beginning of the log regardless of soft limit. */
    void log(const message_t& msg) { for (auto& t : tgts_) { if (t) t->log(msg); } }

    /*! Returns whether a given log level is currently enabled (above the "soft" limit) in the main component. */
    bool enabled(level_t level) const { return ::jjTheLogComponent.enabled(level); }
    /*! Returns the currently set "soft" level limit for log messages in the main component. */
    level_t limit() const { return ::jjTheLogComponent.limit(); }
    /*! Changes the currently set "soft" log level limit of the main component.
    Note that it will be adjusted if it would go below the "hard" limit. */
    void setLevel(level_t level) { ::jjTheLogComponent.setLevel(level); }

    /*! Adds an addtional log target to the current ones.
    Note that the targets will be called in the order as they have been registered. */
    void registerTarget(logTarget_t tgt) { tgts_.push_back(tgt); }
    /*! Removes any existing log targets and instead registers the given one.
    Returns the first of the previously registered targets (if there was any) or none. */
    logTarget_t replaceTargets(logTarget_t tgt)
    {
        logTarget_t fst(tgts_.size() == 0 ? nullptr : tgts_.front());
        tgts_.clear(); 
        registerTarget(tgt);
        return fst;
    }
};

} // namespace log
} // namespace jj

/*! A helper macro to define the internals of the individual log statement macros, does the "soft" limit
check, converts from streamed values to a string and constructs a message_t of it, finally calls the logger.
Use this one should you want to define custom log levels and their statement macros.
Evaluates to true or false based on whether the log level was enabled or not. */
#define JJ__LOGGER(level,levelname,msg) \
    (JJ_LOG_COMPONENT.enabled(level) ? \
        jj::log::logger_t::instance().log(jj::log::message_t( \
            jj::log::clock_t::now(), \
            level, levelname, \
            ((JJ_LOG_STREAM_PROVIDER << msg).str()), \
            JJ_FUNC, \
            __FILE__, __LINE__, \
            JJ_LOG_COMPONENT \
        )), true : \
        false)

/*! A helper macro that allows defining the logger initializer method that has to be defined in every program
that uses this logger.
Pass a variable name as the first parameter, you can make use of that variable (referencing the logger instance)
in the second parameter which is the actual body of the initializer method. */
#define JJ_LOGGER_INITIALIZER(loggervar, body) \
    namespace jj { namespace log { \
    void initialize(logger_t& loggervar) { body } \
    } }

/*! A convenience macro over the JJ_LOGGER_INITIALIZER that sets up the program to use the default log target. */
#define JJ_LOGGER_DEFAULTINIT JJ_LOGGER_INITIALIZER(logger, logger.registerTarget(std::make_shared<stderrTarget_t>());)

#else // JJ_LOGLIMIT_HARD < JJ_LOGLEVEL_OFF

#define JJ_LOGGER_INITIALIZER(loggervar, body)
#define JJ_LOGGER_DEFAULTINIT

#endif // JJ_LOGLIMIT_HARD < JJ_LOGLEVEL_OFF

#if (JJ_LOGLIMIT_HARD <= JJ_LOGLEVEL_FATAL)
/*! Log statement macro that logs a [FATAL] error of JJ_LOGLEVEL_FATAL level. */
#define jjLF(msg) JJ__LOGGER(JJ_LOGLEVEL_FATAL, jj::log::logger_t::NAME_FATAL, msg)
#else
#define jjLF(msg)
#endif

#if (JJ_LOGLIMIT_HARD <= JJ_LOGLEVEL_ERROR)
/*! Log statement macro that logs a [ERROR] error of JJ_LOGLEVEL_ERROR level. */
#define jjLE(msg) JJ__LOGGER(JJ_LOGLEVEL_ERROR, jj::log::logger_t::NAME_ERROR, msg)
#else
#define jjLE(msg)
#endif

#if (JJ_LOGLIMIT_HARD <= JJ_LOGLEVEL_ALERT)
/*! Log statement macro that logs a [ALERT] error of JJ_LOGLEVEL_ALERT level. */
#define jjLA(msg) JJ__LOGGER(JJ_LOGLEVEL_ALERT, jj::log::logger_t::NAME_ALERT, msg)
#else
#define jjLA(msg)
#endif

#if (JJ_LOGLIMIT_HARD <= JJ_LOGLEVEL_WARNING)
/*! Log statement macro that logs a [WARN] error of JJ_LOGLEVEL_WARNING level. */
#define jjLW(msg) JJ__LOGGER(JJ_LOGLEVEL_WARNING, jj::log::logger_t::NAME_WARNING, msg)
#else
#define jjLW(msg)
#endif

#if (JJ_LOGLIMIT_HARD <= JJ_LOGLEVEL_INFO)
/*! Log statement macro that logs a [INFO] error of JJ_LOGLEVEL_INFO level. */
#define jjLI(msg) JJ__LOGGER(JJ_LOGLEVEL_INFO, jj::log::logger_t::NAME_INFO, msg)
#else
#define jjLI(msg)
#endif

#if (JJ_LOGLIMIT_HARD <= JJ_LOGLEVEL_VERBOSE)
/*! Log statement macro that logs a [VERB] error of JJ_LOGLEVEL_VERBOSE level. */
#define jjLV(msg) JJ__LOGGER(JJ_LOGLEVEL_VERBOSE, jj::log::logger_t::NAME_VERBOSE, msg)
#else
#define jjLV(msg)
#endif

#if (JJ_LOGLIMIT_HARD <= JJ_LOGLEVEL_DEBUG)
/*! Log statement macro that logs a [DEBUG] error of JJ_LOGLEVEL_DEBUG level. */
#define jjLD(msg) JJ__LOGGER(JJ_LOGLEVEL_DEBUG, jj::log::logger_t::NAME_DEBUG, msg)
#else
#define jjLD(msg)
#endif

#endif // JJ_LOG_H
