#include "jj/cmdLine.h"
#include <algorithm>
#include <cctype>
#include <exception>
#include "jj/stream.h"
#include <sstream>

namespace jj
{
namespace cmdLine
{

name_t::defaultPolicy_t name_t::DefaultPolicy(DASH);
string_t name_t::DefaultPrefix;

bool nameCompare_t::compare_icase(const string_t& a, const string_t& b)
{
    // TODO do per-character and move to string.h
    string_t ta(a), tb(b);
    std::transform(ta.begin(), ta.end(), ta.begin(), [](char_t ch) { return std::tolower(ch); });
    std::transform(tb.begin(), tb.end(), tb.begin(), [](char_t ch) { return std::tolower(ch); });
    return ta < tb;
}

arguments_t::arguments_t()
    : OptionCase(case_t::SENSITIVE), VariableCase(case_t::SENSITIVE), ParseStart(0), Options(nameCompare_t(case_t::SENSITIVE)), opts_(nameCompare_t(case_t::SENSITIVE)), vars_(nameCompare_t(case_t::SENSITIVE)), defs_(nullptr)
{
    ParserOptions << flags_t::ALLOW_STACKS << flags_t::ALLOW_SHORT_ASSIGN << flags_t::ALLOW_LONG_ASSIGN;
    setup_basic_prefixes();
}

void arguments_t::setup_basic_prefixes()
{
    PrefixInfo[jjT("-")] = prefixInfo_t{ SHORT_OPTION };
    PrefixInfo[jjT("--")] = prefixInfo_t{ LONG_OPTION };
    PrefixInfo[jjT("/")] = prefixInfo_t{ LONG_OPTION };
}

const prefixInfo_t& arguments_t::ensure_prefix(const string_t& prefix)
{
    prefixes_t::const_iterator fnd = PrefixInfo.find(prefix);
    if (fnd != PrefixInfo.end())
        return fnd->second;
    prefixInfo_t info;
    info.Type = prefix.length() == 1 ? SHORT_OPTION : LONG_OPTION;
    return PrefixInfo.insert(prefixes_t::value_type(prefix, info)).first->second;
}

void arguments_t::parse(const definitions_t& defs)
{
    // reset options metadata and rebuild it from regular and list option lists
    opts_ = optmap_t(nameCompare_t(OptionCase));
    for (const definitions_t::opts_t::value_type& o : defs.Options)
    {
        for (const optionDefinition_t::names_t::value_type& n : o.Names)
        {
            if (n.Prefix.empty())
                throw std::runtime_error("Empty option prefix");
            if (n.Name.empty())
                throw std::runtime_error("Empty option name");
            const prefixInfo_t& pr = ensure_prefix(n.Prefix);
            if (pr.Type == SHORT_OPTION && n.Name.length() > 1)
                throw std::runtime_error(strcvt::to_string(jjS(jjT("Prefix '") << n.Prefix << jjT("' implies short option for '") << n.Name << jjT("'."))));
            bool ret = opts_.insert(optmap_t::value_type(n, optionData_t(&o))).second;
            if (!ret)
                throw std::runtime_error(strcvt::to_string(jjS(jjT("Duplicate definition for option '") << n.Prefix << n.Name << jjT("'"))));
        }
    }
    for (const definitions_t::lists_t::value_type& o : defs.ListOptions)
    {
        for (const listDefinition_t::names_t::value_type& n : o.Names)
        {
            if (n.Prefix.empty())
                throw std::runtime_error("Empty option prefix");
            if (n.Name.empty())
                throw std::runtime_error("Empty option name");
            const prefixInfo_t& pr = ensure_prefix(n.Prefix);
            if (pr.Type == SHORT_OPTION && n.Name.length() > 1)
                throw std::runtime_error(strcvt::to_string(jjS(jjT("Prefix '") << n.Prefix << jjT("' implies short option for '") << n.Name << jjT("'."))));
            bool ret = opts_.insert(optmap_t::value_type(n, optionData_t(&o))).second;
            if (!ret)
                throw std::runtime_error(strcvt::to_string(jjS(jjT("Duplicate definition for option '") << n.Prefix << n.Name << jjT("'"))));
        }
    }

    // reset variables metadata and rebuild them from definition list
    vars_ = varmap_t(nameCompare_t(VariableCase));
    for (const definitions_t::vars_t::value_type& v : defs.Variables)
    {
        if (v.Name.empty())
            throw std::runtime_error("Empty variable name");
        bool ret = vars_.insert(varmap_t::value_type(v.Name, varproxy_t{ v.Default, &v })).second;
        if (!ret)
            throw std::runtime_error(strcvt::to_string(jjS(jjT("Duplicate definition for variable '") << v.Name << jjT("'"))));
    }
    defs_ = &defs;
}

void arguments_t::parse_program_name(const char_t* pn)
{
    if (pn == nullptr)
        throw std::runtime_error("Given program name is nullptr");
    const char_t* tmp = pn, *start = pn;
    while (*tmp != 0)
    {
#if defined(_WINDOWS) || defined(_WIN32)
        if (*tmp == jjT('/') || *tmp == jjT('\\'))
#else
        if (*tmp == '/')
#endif //  defined(_WINDOWS) || defined(_WIN32)
            start = tmp + 1;
        ++tmp;
    }
    // TODO for the above use path when available

    if (*start == 0)
        throw std::runtime_error("Invalid program name.");

    ProgramName.assign(start);
}

void arguments_t::parse(int argc, const char_t** argv)
{
    if (defs_ == nullptr)
        throw std::runtime_error("Call parse(definitions_t) first.");
    if (argc == 0 || argv == nullptr)
        throw std::runtime_error("Empty argument list");

    clear_data();

    definitions_t::poss_t::const_iterator currentPositional = defs_->Positionals.begin();
    bool inPositionals = false;

    missingValues_t missingValues;
    int argi = int(ParseStart);
    if (argi < 0)
        throw std::runtime_error("Invalid value of ParseStart.");
    if (argi == 0)
    {
        parse_program_name(argv[0]);
        ++argi;
    }

    for (; argi < argc; ++argi)
    {
        if (argv[argi] == nullptr)
            throw std::runtime_error("One of argv[i] is nullptr.");

        // handle missing values for options
        if (!missingValues.empty())
        { 
            add_option_value(missingValues, argv[argi]);
            continue;
        }

        // handle section where all arguments treated as positionals (if enabled)
        if (inPositionals)
        {
            if (ParserOptions*flags_t::USE_RETURN_DELIMITER && ReturnDelimiter == argv[argi])
            {
                // switching back from treating all as positionals
                inPositionals = false;
                continue;
            }
            process_positional(currentPositional, argv[argi]);
            continue;
        }
        if ((ParserOptions*flags_t::USE_POSITIONAL_DELIMITER) && PositionalDelimiter == argv[argi])
        {
            // switching to treat all arguments as positionals
            inPositionals = true;
            continue; // nothing more to do with the delimiter argument
        }

        // in normal processing here

        // try locating longest match prefix
        prefixes_t::const_reverse_iterator fnd = PrefixInfo.crend();
        for (prefixes_t::const_reverse_iterator rit = PrefixInfo.crbegin(); rit != PrefixInfo.crend(); ++rit)
        {
            if (!str::starts_with(argv[argi], rit->first))
                continue;
            fnd = rit;
            break;
        }
        if (fnd == PrefixInfo.crend())
        {
            // no prefix found, treat as positional/variable
            process_positional(currentPositional, argv[argi]);
        }
        else if (fnd->second.Type == LONG_OPTION) // found a prefix identifying a long option
            process_long_option(missingValues, fnd->first, argv[argi] + fnd->first.length());
        else // found a prefix identifying a short option
            process_short_option(missingValues, fnd->first, argv[argi] + fnd->first.length());
    }

    // check that there are no pending option values nor unterminated list
    for (auto& mv : missingValues)
    {
        switch (mv.second.first.Type)
        {
        case TREG:
            throw std::runtime_error(strcvt::to_string(jjS(jjT("Option argument '") << mv.first.Prefix << mv.first.Name << jjT("' is missing a value. ") << mv.second.first.u.Opt->ValueCount << jjT(" were expected but only have ") << mv.second.second.Values.size() << jjT("."))));
        case TLIST:
            if (ParserOptions*flags_t::LIST_MUST_TERMINATE)
                throw std::runtime_error(strcvt::to_string(jjS(jjT("List option '") << mv.first.Prefix << mv.first.Name << jjT("' is not terminated."))));
            else
                add_option(mv);
        }
    }

    // check that all mandatory positionals were present
    for (; currentPositional != defs_->Positionals.cend(); ++currentPositional)
        if (currentPositional->Mandatory)
            throw std::runtime_error(strcvt::to_string(jjS(jjT("Mandatory positional argument '") << currentPositional->Shorthand << jjT("' missing."))));
}

void arguments_t::clear_data()
{
    Options = options_t(nameCompare_t(OptionCase));
    Positionals.clear();
    for (varmap_t::value_type& v : vars_)
        v.second.Value = v.second.Var->Default;
}

void arguments_t::add_option(options_t::value_type& opt)
{
    multiple_t multi;
    switch (opt.second.first.Type)
    {
    default:
        throw std::runtime_error("Internal error");
    case TREG:
        if (opt.second.first.u.Opt->CB && !opt.second.first.u.Opt->CB(*opt.second.first.u.Opt, opt.second.second))
            return; // do nothing, ignore this occurrence
        multi = opt.second.first.u.Opt->Multi;
        break;
    case TLIST:
        if (opt.second.first.u.List->CB && !opt.second.first.u.List->CB(*opt.second.first.u.List, opt.second.second))
            return; // do nothing, ignore this occurrence
        multi = opt.second.first.u.List->Multi;
        break;
    }

    auto ret = Options.insert(opt);

    if (ret.second)
        ; // this is the first time option was encountered, no special treatment
    else switch (multi)
    {
    case multiple_t::ERROR:
        throw std::runtime_error(strcvt::to_string(jjS(jjT("Option argument '") << opt.first.Prefix << opt.first.Name << jjT("' provided multiple times."))));
    case multiple_t::OVERRIDE:
        ret.first->second.second.Values = opt.second.second.Values;
        break;
    case multiple_t::JOIN:
        for (const string_t& s : opt.second.second.Values)
            ret.first->second.second.Values.push_back(s);
        break;
    case multiple_t::PRESERVE:
        break; // ignore this occurence
    }
}

void arguments_t::add_option_value(missingValues_t& mv, const char_t* value)
{
    if (mv.empty() || value == nullptr)
        throw std::runtime_error("Internal error.");

    options_t::value_type& mi = mv.front();
    switch (mi.second.first.Type)
    {
    default:
        throw std::runtime_error("Internal error");
    case TREG:
        mi.second.second.Values.push_back(value);
        if (mi.second.second.Values.size() < mi.second.first.u.Opt->ValueCount)
            break;
        add_option(mi);
        mv.pop_front();
        break;
    case TLIST:
        if (mi.second.first.u.List->Delimiter == value)
        {
            add_option(mi);
            mv.pop_front();
        }
        else
            mi.second.second.Values.push_back(value);
        break;
    }
}

void arguments_t::handle_new_option(missingValues_t& mv, const name_t& name, optionData_t data, const char_t* value)
{
    const name_t* name1 = nullptr;
    bool miss = false;
    switch (data.Type)
    {
    default:
        throw std::runtime_error("Internal error");
    case TREG:
        name1 = &data.u.Opt->Names.front();
        miss = data.u.Opt->ValueCount > 0;
        break;
    case TLIST:
        name1 = &data.u.List->Names.front();
        miss = true;
        break;
    }
    options_t::value_type item(*name1, option_t(data, values_t()));

    if (!miss && value != nullptr)
        throw std::runtime_error(strcvt::to_string(jjS(jjT("Value provided for option argument '") << name.Prefix << name.Name << jjT("' but none was expected."))));

    if (!miss)
    {
        // no values expected, insert
        add_option(item);
    }
    else
    {
        mv.push_back(item);
        if (value != nullptr)
            add_option_value(mv, value);
    }
}

void arguments_t::process_long_option(missingValues_t& mv, const string_t& prefix, const char_t* arg)
{
    string_t name;
    const char_t* value = nullptr;
    if (ParserOptions*flags_t::ALLOW_LONG_ASSIGN)
    {
        // try locate =
        const char_t* fnd = str::find(arg, jjT('='));
        if (fnd == nullptr)
            name.assign(arg);
        else
        {
            name.assign(arg, fnd - arg);
            value = ++fnd;
        }
    }
    else
        name.assign(arg);

    const optmap_t::const_iterator fnd = opts_.find(name_t(prefix, name));
    if (fnd == opts_.cend())
        throw std::runtime_error(strcvt::to_string(jjS(jjT("Found undefined option argument '") << prefix << name << jjT("'."))));
    handle_new_option(mv, fnd->first, fnd->second, value);
}

void arguments_t::process_short_option(missingValues_t& mv, const string_t& prefix, const char_t* arg)
{
    // note that the parse(definitions_t) ensures that there are only single letter options among the short options
    while (*arg != 0)
    {
        string_t o(1u, *arg);
        ++arg;
        const optmap_t::const_iterator fnd = opts_.find(name_t(prefix, o));
        if (fnd == opts_.cend())
            throw std::runtime_error(strcvt::to_string(jjS(jjT("Found undefined option argument '") << prefix << o << jjT("'."))));
        // ok found relevant definition
        // check if it needs a value
        bool needsValue = false;
        switch (fnd->second.Type)
        {
        default:
            throw std::runtime_error("Internal error");
        case TREG:
            needsValue = fnd->second.u.Opt->ValueCount > 0;
            break;
        case TLIST:
            needsValue = true;
            break;
        }
        const char_t* value = nullptr;
        if (needsValue && *arg != 0)
        {
            if (*arg == jjT('=') && ParserOptions*flags_t::ALLOW_SHORT_ASSIGN)
            {
                ++arg;
                value = arg;
                arg = jjT("");
            }
            else if (ParserOptions*flags_t::ALLOW_STACK_VALUES)
            {
                value = arg;
                arg = jjT("");
            }
            else if (ParserOptions.opt::e<stackOptionValues_t>::Value==stackOptionValues_t::LOOSE)
            {
                // these are gonna be handled as options in next loop(s)
            }
            else
                throw std::runtime_error(strcvt::to_string(jjS(jjT("Invalid characters '") << arg << jjT("' following '") << fnd->first.Prefix << fnd->first.Name << jjT("'. Did you mean to enter value as separate argument?"))));
        }
        if (*arg != 0 && !(ParserOptions*flags_t::ALLOW_STACKS))
            throw std::runtime_error(strcvt::to_string(jjS(jjT("Invalid characters '") << arg << jjT("' following '") << fnd->first.Prefix << fnd->first.Name << jjT("'. Did you mean separate options?"))));
        handle_new_option(mv, fnd->first, fnd->second, value);
    }
}

void arguments_t::process_positional(definitions_t::poss_t::const_iterator& cpos, const char_t* arg)
{
    if (!vars_.empty())
    {
        // try locate =
        const char_t* fnd = str::find(arg, jjT('='));
        if (fnd != nullptr)
        {
            string_t name(arg, fnd - arg), value(++fnd);
            varmap_t::iterator var = vars_.find(name);
            if (var != vars_.end())
            {
                if (var->second.Var->CB == nullptr || var->second.Var->CB(*var->second.Var, value))
                    var->second.Value = value;
                return; // positional processed as variable definition
            }
            else if (ParserOptions*flags_t::UNKNOWN_VARS_ARE_POSITIONALS)
                ; // simply ignore
            else
                throw std::runtime_error(strcvt::to_string(jjS(jjT("Found unknown variable '") << name << jjT("'."))));
        }
    }

    // no matching variable definition found so we are definitely positional
    if (cpos != defs_->Positionals.cend())
    {
        // found positional
        positional_t tmp(&*cpos, arg);
        if (cpos->CB == nullptr || cpos->CB(*cpos, tmp.second))
            Positionals.push_back(tmp);
        ++cpos;
    }
    else if (ParserOptions*flags_t::DENY_ADDITIONAL)
        throw std::runtime_error(strcvt::to_string(jjS(jjT("Positional argument '") << arg << jjT("' found, but no more expected."))));
    else
    {
        // undefined positional
        positional_t tmp(nullptr, arg);
        Positionals.push_back(tmp);
    }
}

} // namespace cmdLine
} // namespace jj
