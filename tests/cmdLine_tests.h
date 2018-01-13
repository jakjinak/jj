#include "jj/string.h"
#include <list>

/*! Helper for commandline arguments testing containing the args and able to convert from initializer list.
\warning DONT EVER MODIFY args/argv/argc manually! */
struct arg_info_t
{
    static const int MAX_SIZE = 32; //!< maximum args to store
    static const jj::char_t* DefaultProgramName; // jjT("ProgramName")

    std::list<jj::string_t> args; //!< arguments in C++ string form (dummy program name auto-added)
    int argc; //!< the count of args (and argv)
    const jj::char_t* argv[MAX_SIZE]; //!< the regular argv (pointers to args)

    arg_info_t() : argc(0) {}
    arg_info_t(const std::initializer_list<jj::string_t>& x) { setup(x, *this); }
    arg_info_t(const jj::string_t& pn, const std::initializer_list<jj::string_t>& x) { setup(pn, x, *this); }

    static void setup(const jj::string_t& pn, const std::initializer_list<jj::string_t>& x, arg_info_t& info)
    {
        info.args.clear();
        info.args.push_back(pn);
        info.argv[0] = info.args.back().c_str();
        size_t i=1;
        for (auto& s : x)
        {
            info.args.push_back(s);
            info.argv[i] = info.args.back().c_str();
            ++i;
            if (i>=MAX_SIZE)
                break;
        }
        info.argc = i;
    }
    static void setup(const std::initializer_list<jj::string_t>& x, arg_info_t& info)
    {
        setup(DefaultProgramName, x, info);
    }

    void print();
};
