#include "jj/cmdLine.h"
#include <exception>
#include "jj/stream.h"
#include <sstream>

namespace jj
{
namespace cmdLine
{

name_t::defaultPolicy_t name_t::DefaultPolicy(DASH);
string_t name_t::DefaultPrefix;

arguments_t::arguments_t()
    : OptionCase(case_t::SENSITIVE), VariableCase(case_t::SENSITIVE), ParseStart(0), Options(nameCompare_less_t(case_t::SENSITIVE)), opts_(nameCompare_less_t(case_t::SENSITIVE)), Variables(nameCompare_less_t(case_t::SENSITIVE)), defs_(nullptr)
{
    ParserOptions << flags_t::ALLOW_STACKS << flags_t::ALLOW_SHORT_ASSIGN << flags_t::ALLOW_LONG_ASSIGN << unknownVariableBehavior_t::IS_ERROR;
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
    opts_ = optmap_t(nameCompare_less_t(OptionCase));
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
    Variables = varmap_t(nameCompare_less_t(VariableCase));
    for (const definitions_t::vars_t::value_type& v : defs.Variables)
    {
        if (v.Name.empty())
            throw std::runtime_error("Empty variable name");
        bool ret = Variables.insert(varmap_t::value_type(v.Name, varproxy_t{ v.Default, true, &v })).second;
        if (!ret)
            throw std::runtime_error(strcvt::to_string(jjS(jjT("Duplicate definition for variable '") << v.Name << jjT("'"))));
    }
    defs_ = &defs;
}

void arguments_t::add_default_help(definitions_t& defs)
{
    defs.Options.push_back({
       {name_t(jjT('h')), name_t(jjT("help"))
#if defined(_WINDOWS) || defined(_WIN32)
       ,name_t(jjT('?'))
#endif // defined(...
       },
       jjT("Prints this help and exits."),
       0u,
       multiple_t::OVERRIDE,
       [this](const optionDefinition_t&, values_t&){ this->print_default_help(); exit(0); return true; } 
       });
}

void arguments_t::print_default_help()
{
    if (ProgramName.empty())
        jj::cout << jjT("programname");
    else
        jj::cout << ProgramName;
    if (opts_.size()>0)
        jj::cout << jjT(" OPTIONS...");
    if (Variables.size()>0)
        jj::cout << jjT(" VARIABLES...");

    const definitions_t::poss_t& pospar = defs_->Positionals;
    for (const positionalDefinition_t& p : pospar)
        jj::cout << jjT(' ') << p.Shorthand;
    jj::cout << jjT('\n');

    if (opts_.size()>0)
    {
        jj::cout << jjT("\nOPTIONS\n");
        for (const optionDefinition_t& o : defs_->Options)
        {
            for (const name_t& n : o.Names)
            {
                jj::cout << n.Prefix << n.Name;
                if (o.ValueCount == 0)
                    ;
                else if (o.ValueCount == 1)
                    jj::cout << jjT(" value");
                else for (size_t i=0; i<o.ValueCount; ++i)
                    jj::cout << jjT(" value") << (i+1);
                jj::cout << jjT('\n');
            }
            jj::cout << jjT('\t') << o.Description << jjT('\n');
        }
        for (const listDefinition_t& o : defs_->ListOptions)
        {
            for (const name_t& n : o.Names)
                jj::cout << n.Prefix << n.Name << jjT(" ... ") << o.Delimiter << jjT('\n');
            jj::cout << jjT('\t') << o.Description << jjT('\n');
        }
    }

    if (defs_->Positionals.size() > 0)
    {
        jj::cout << jjT("\nPOSITIONAL ARGUMENTS\n");
        // search first for last mandatory
        const positionalDefinition_t* mandatory = nullptr;
        for (definitions_t::poss_t::const_reverse_iterator it = defs_->Positionals.rbegin(); it != defs_->Positionals.rend(); ++it)
        {
            if (it->Mandatory)
            {
                mandatory = &*it;
                break;
            }
        }

        bool isMandatory = mandatory != nullptr;
        for (const positionalDefinition_t& p : defs_->Positionals)
        {
            jj::cout << p.Shorthand << jjT('\n') << jjT('\t') << p.Description;
            if (isMandatory)
                jj::cout << jjT("\tThis argument is mandatory.\n");
            if (mandatory == &p)
                isMandatory = false;
        }
    }

    if (Variables.size() > 0)
    {
        jj::cout << jjT("\nVARIABLES\n");
        jj::cout << jjT("\tThe following variables can be set per arguments in form variable=value.\n");
        for (const variableDefinition_t& v : defs_->Variables)
        {
            jj::cout << v.Name << jjT('\n');
            jj::cout << jjT('\t') << v.Description << jjT('\n');
            if (!v.Default.empty())
                jj::cout << jjT("\tDefault value is \"") << v.Default << jjT('"') << jjT('\n');
        }
    }

    for (const definitions_t::helpSection_t& s : defs_->Sections)
    {
        jj::cout << jjT('\n') << s.first << jjT('\n');
        jj::cout << s.second;
    }
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
            process_positional(inPositionals, currentPositional, argv[argi]);
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
            process_positional(inPositionals, currentPositional, argv[argi]);
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
    Options = options_t(nameCompare_less_t(OptionCase));
    Positionals.clear();
    for (varmap_t::value_type& v : Variables)
    {
        if (v.second.Var == nullptr)
            v.second.Value == jj::str::Empty;
        else
            v.second.Value = v.second.Var->Default;
    }
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

    if (multi == multiple_t::VARIABLE)
    {
        for (const jj::string_t& v : opt.second.second.Values)
            process_variable(true, v.c_str());
        return;
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
    if (*arg == 0) // verify that there is more than just the prefix
        throw std::runtime_error(strcvt::to_string(jjS(jjT("No option given with prefix '") << prefix << jjT("'."))));
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

bool arguments_t::process_variable(bool mustbe, const char_t* arg)
{
    // try locate =
    const char_t* fnd = str::find(arg, jjT('='));
    if (fnd == nullptr)
    {
        if (mustbe)
            throw std::runtime_error(jj::strcvt::to_string(jjS(jjT("The '") << arg << jjT("' assumed to be a variable definition, but no '=' found."))));
        return false; // no = found
    }

    string_t name(arg, fnd - arg), value(++fnd);
    varmap_t::iterator var = Variables.find(name);
    if (var != Variables.end())
    {
        if (var->second.Var == nullptr)
        {
            if (name.empty())
                throw std::runtime_error("Encountered empty variable name.");
            // "unknown" variables allowed and rewriting one already found
            var->second.Value = value;
            var->second.IsDefault = value.empty();
            return true; // processed as "unknown" variable
        }
        if (var->second.Var->CB == nullptr || var->second.Var->CB(*var->second.Var, value))
        {
            var->second.Value = value;
            var->second.IsDefault = value == var->second.Var->Default;
        }
        return true; // processed as variable definition
    }

    jj::opt::e<unknownVariableBehavior_t>& uv = ParserOptions;
    switch (uv.Value)
    {
    case unknownVariableBehavior_t::IS_POSITIONAL:
        if (mustbe)
            throw std::runtime_error(jj::strcvt::to_string(jjS(jjT("The '") << arg << jjT("' assumed to be a variable definition, but no '=' found."))));
        return false; // simply ignore; handle below as positional argument
    case unknownVariableBehavior_t::IS_VARIABLE:
        if (name.empty())
            throw std::runtime_error("Encountered empty variable name.");
        Variables[name] = varproxy_t{ value, value.empty(), nullptr };
        return true; // processed as "unknown" variable
    case unknownVariableBehavior_t::IS_ERROR:
    default:
        throw std::runtime_error(strcvt::to_string(jjS(jjT("Found unknown variable '") << name << jjT("'."))));
    }
}

void arguments_t::process_positional(bool explicitPositionals, definitions_t::poss_t::const_iterator& cpos, const char_t* arg)
{
    jj::opt::e<unknownVariableBehavior_t>& uv = ParserOptions;
    if ((!explicitPositionals || ParserOptions*flags_t::TREAT_VARIABLES_IN_EXPLICIT_POSITIONALS) && (!Variables.empty() || uv.Value != unknownVariableBehavior_t::IS_POSITIONAL))
    {
        if (process_variable(false, arg))
            return;
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
