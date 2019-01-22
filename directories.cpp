#include "jj/directories.h"
#include "jj/exception.h"
#include "jj/stream.h"
#include <sstream>
#if defined(JJ_OS_WINDOWS)
#include <Windows.h>
#include <Shlobj_core.h>
#else
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#endif 

namespace jj
{
namespace directories
{
jj::string_t current()
{
#if defined(JJ_OS_WINDOWS)
    DWORD ret = 0;
    {
        static const DWORD MAX = 512u;
        TCHAR buff[MAX];
        ret = GetCurrentDirectory(MAX, buff);
        if (ret == 0)
        {
            int err = GetLastError();
            throw jj::exception::base(jjS2(std::ostringstream, "Cannot retrieve current directory; error=" << err));
        }
        if (ret < MAX)
            return jj::string_t(buff, ret);
    }

    TCHAR* buff = new TCHAR[ret];
    DWORD ret2 = GetCurrentDirectory(ret, buff);
    if (ret == 0)
    {
        int err = GetLastError();
        throw jj::exception::base(jjS2(std::ostringstream, "Cannot retrieve current directory; error=" << err));
    }
    if (ret2 >= ret)
        throw jj::exception::base("Cannot retrieve current directory.");
    return jj::string_t(buff, ret2);
#else
    {
        static const size_t MAX = 512u;
        char buff[MAX];
        char* ret = getcwd(buff, MAX);
        if (ret == NULL)
        {
            auto err = errno;
            if (errno == ERANGE)
                ; // not large enought, retry
            else
                throw jj::exception::base(jjS2(std::ostringstream, "Cannot retrieve current directory; error=" << err));
        }
        else
            return jj::string_t(ret);
    }
    char* ret = getcwd(NULL, 0u);
    if (ret == NULL)
    {
        auto err = errno;
        throw jj::exception::base(jjS2(std::ostringstream, "Cannot retrieve current directory; error=" << err));
    }
    jj::string_t str(ret);
    free(ret);
    return str;
#endif
}

jj::string_t program(bool includeProgramName)
{
#if defined(JJ_OS_WINDOWS)
    DWORD ret = 0;
    {
        static const DWORD MAX = 511u;
        TCHAR buff[MAX + 1];
        buff[MAX] = 0;
        ret = GetModuleFileName(NULL, buff, MAX);
        int err = GetLastError();
        if (ret == MAX && err == ERROR_INSUFFICIENT_BUFFER)
            ; // retry with bigger buffer
        else if (ret == 0)
            throw jj::exception::base(jjS2(std::ostringstream, "Cannot retrieve program location; error=" << err));
        else if (includeProgramName)
            return jj::string_t(buff, ret);
        else
        {
            jj::string_t path(buff, ret);
            size_t fnd = path.rfind(jjT('\\'));
            if (fnd == jj::string_t::npos)
                throw jj::exception::base(jjS2(std::ostringstream, jjT("Cannot retrieve program location, [") << jj::strcvt::to_string(path) << jjT("] is not absolute path.")));
            return path.substr(0u, fnd);
        }
    }
    static const DWORD MAX = 2047u;
    TCHAR buff[MAX + 1];
    buff[MAX] = 0;
    ret = GetModuleFileName(NULL, buff, MAX);
    int err = GetLastError();
    if (ret == 0 || ret == MAX && err == ERROR_INSUFFICIENT_BUFFER)
        throw jj::exception::base(jjS2(std::ostringstream, "Cannot retrieve program location; error=" << err));
    else if (includeProgramName)
        return jj::string_t(buff, ret);
    else
    {
        jj::string_t path(buff, ret);
        size_t fnd = path.rfind(jjT('\\'));
        if (fnd == jj::string_t::npos)
            throw jj::exception::base(jjS2(std::ostringstream, jjT("Cannot retrieve program location, [") << jj::strcvt::to_string(path) << jjT("] is not absolute path.")));
        return path.substr(0u, fnd);
    }
#else
    static const size_t MAX = 2047;
    char buff[MAX + 1];
    buff[MAX] = 0;
    ssize_t ret = readlink("/proc/self/exe", buff, MAX);
    if (ret == -1)
    {
        char pbuf[MAX + 1];
        snprintf(pbuf, MAX, "/proc/%d/exe", getpid());
        ret = readlink(pbuf, buff, MAX);
        if (ret == -1)
        {
            auto err = errno;
            throw jj::exception::base(jjS2(std::ostringstream, "Cannot retrieve program location; error=" << err));
        }
    }
    if (ret < -1 || ret == 0)
        throw jj::exception::base("Cannot retrieve program location!");
    if (includeProgramName)
        return jj::string_t(buff, ret);
    else
    {
        jj::string_t path(buff, ret);
        size_t fnd = path.rfind('/');
        if (fnd == jj::string_t::npos)
            throw jj::exception::base(jjS2(std::ostringstream, jjT("Cannot retrieve program location, [") << path << jjT("] is not absolute path.")));
        return path.substr(0u, fnd);
    }
#endif
}

jj::string_t personal(bool hidden)
{
#if defined(JJ_OS_WINDOWS)
    TCHAR buff[MAX_PATH];
    HRESULT ret = SHGetFolderPath(NULL, (hidden ? CSIDL_LOCAL_APPDATA : CSIDL_MYDOCUMENTS), NULL, SHGFP_TYPE_CURRENT | CSIDL_FLAG_NO_ALIAS, buff);
    if (SUCCEEDED(ret))
        return jj::string_t(buff);
    throw jj::exception::base(jjS2(std::ostringstream, "Cannot retrieve home directory; error=" << ret));
#else
    {
#if defined(_GNU_SOURCE) && !defined(JJ_OS_CYGWIN)
        char* ret = secure_getenv("HOME");
#else
        char* ret = getenv("HOME");
#endif // _GNU_SOURCE
        if (ret == NULL)
            ; // retry alternatively
        else
            return jj::string_t(ret);
    }

    // copied from http://man7.org/linux/man-pages/man3/getpwuid.3p.html
    uid_t uid = getuid();
    long int initlen = sysconf(_SC_GETPW_R_SIZE_MAX);
    size_t len = (initlen == -1 ? 1024 : (size_t)initlen);
    struct passwd result;
    struct passwd *resultp;
    char* buffer = (char*)malloc(len);
    if (buffer == NULL)
        throw jj::exception::base("Cannot retrieve home directory; error=out of memory");
    int e;
    while ((e = getpwuid_r(uid, &result, buffer, len, &resultp)) == ERANGE)
    {
        size_t newlen = 2 * len;
        if (newlen < len)
            throw jj::exception::base("Cannot retrieve home directory; error=out of memory");
        len = newlen;
        char* newbuffer = (char*)realloc(buffer, len);
        if (newbuffer == NULL)
        {
            free(buffer);
            throw jj::exception::base("Cannot retrieve home directory; error=out of memory");
        }
        buffer = newbuffer;
    }
    if (e != 0)
    {
        free(buffer);
        throw jj::exception::base(jjS2(std::ostringstream, "Cannot retrieve home directory; error=" << e));
    }
    if (result.pw_dir == NULL || *(result.pw_dir) == 0)
    {
        free(buffer);
        throw jj::exception::base("Cannot retrieve home directory.");
    }
    jj::string_t res(result.pw_dir);
    free(buffer);
    return res;
#endif
}

} // namespace directories
} // namespace jj
