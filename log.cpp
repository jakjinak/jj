#include "jj/log.h"
#include "jj/stream.h"
#include "jj/time.h"
#include <iostream>

JJ_DECLARE_LOG_COMPONENT2(jjMainLog, "<main>");
jj::log::component_t& jjTheLogComponent = jjMainLogComponent_t::instance();

namespace jj
{
namespace log
{

template<typename S>
void logToStream(S& s, const message_t& log)
{
    s << jj::time::stamp_t(log.Time) << jjT(' ') << jjT('[') << log.LevelName << jjT(']') << jjT(' ') << log.Message << jjT('\n');
}

void stdoutTarget_t::log(const message_t& log)
{
    logToStream(jj::cout, log);
}

void stderrTarget_t::log(const message_t& log)
{
    logToStream(jj::cerr, log);
}

const levelName_t logger_t::NAME_FATAL = jjT("FATAL");
const levelName_t logger_t::NAME_ERROR = jjT("ERROR");
const levelName_t logger_t::NAME_ALERT = jjT("ALERT");
const levelName_t logger_t::NAME_WARNING = jjT("WARN");
const levelName_t logger_t::NAME_INFO = jjT("INFO");
const levelName_t logger_t::NAME_VERBOSE = jjT("VERB");
const levelName_t logger_t::NAME_ENTER = jjT("ENTER");
const levelName_t logger_t::NAME_LEAVE = jjT("LEAVE");
const levelName_t logger_t::NAME_DEBUG = jjT("DEBUG");

} // namespace log
} // namespace jj
