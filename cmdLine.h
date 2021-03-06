#ifndef JJ_CMDLINE_H
#define JJ_CMDLINE_H

#include <list>
#include <map>
#include <deque>
#include <functional>
#include "jj/string.h"
#include "jj/options.h"

namespace jj
{
namespace cmdLine
{

/*! Case sensitiviness for option/variable names. */
enum class case_t
{
    SENSITIVE, //!< default; case-sensitive
    INSENSITIVE //!< case-insensitive
};

/*! Abstracts an option name - ie. a pair of prefix and textual name.
The prefix, typically - or -- shall be a nonalphanumeric sequence of one or more (typically same) characters. 
The name on the other hand shall be alphanumeric with optional (typically - or _) delimiters. */
struct name_t
{
    string_t Prefix;
    string_t Name;

    name_t(char_t n) : name_t(string_t(1u, n)) {}
    name_t(char_t p, char_t n) : name_t(string_t(1u, p), string_t(1u, n)) {}
    name_t(const string_t& p, char_t n) : name_t(p, string_t(1u, n)) {}
    name_t(const string_t& n) : name_t(default_prefix(n), n) {}
    name_t(char_t p, const string_t& n) : Prefix(string_t(1u, p)), Name(n) {}
    name_t(const string_t& p, const string_t& n) : Prefix(p), Name(n) {}

    /*! Determines the behavior if no prefix is given to ctor. */
    enum defaultPolicy_t
    {
        DASH, //!< assumes - (single dash) if name is of length 1, -- (double dash) otherwise
        SDASH, //!< assumes - (single dash)
        DDASH, //!< assumes -- (double dash)
        SLASH, //!< assumes / (forward slash)
        CUSTOM //!< assumes whatever is set in DefaultPrefix (reverts to DASH if that is empty)
    };
    static defaultPolicy_t DefaultPolicy; //!< determines the behavior if no prefix is given to ctor
    static string_t DefaultPrefix; //!< used as prefix if CUSTOM is the policy and no prefix given to ctor

private:
    static string_t default_prefix(const string_t& n)
    {
        switch (DefaultPolicy)
        {
        case CUSTOM:
            if (DefaultPrefix.length() > 0)
                return DefaultPrefix;
            // fallthrough
        case DASH:
        default:
            if (n.length() == 1)
                return jjT("-");
            else
                return jjT("--");
        case SDASH:
            return jjT("-");
        case DDASH:
            return jjT("--");
        case SLASH:
            return jjT("/");
        }
    }
};

/*! A predicate to compare prefix+name of option where name can be compare case sensitively or insensitively. */
struct nameCompare_less_t
{
    nameCompare_less_t(case_t cs) : cs_(cs) {}
    /*! Used for options. */
    bool operator()(const name_t& a, const name_t& b) const
    {
        if (a.Prefix < b.Prefix)
            return true;
        if (a.Prefix != b.Prefix)
            return false;
        if (cs_ == case_t::SENSITIVE)
            return a.Name < b.Name;
        else
            return str::lessi(a.Name, b.Name);
    }
    /*! Used for variables. */
    bool operator()(const string_t& a, const string_t& b) const
    {
        if (cs_ == case_t::SENSITIVE)
            return a < b;
        else
            return str::lessi(a, b);
    }
private:
    case_t cs_;
};

/*! Abstracts the actual values of options. */
struct values_t
{
    typedef std::list<string_t> vals_t;
    vals_t Values;
};

enum class multiple_t
{
    OVERRIDE, //!< default; subsequent occurence overrides the previous one
    PRESERVE, //!< keep the value set in previous occurence; note that this does not work with empty list for list options (behavior undefined)
    JOIN, //!< append the new values to the previous; note that this works even with optionDefinition_t::ValueCount, but leads to "strange" results
    ERROR, //!< providing option twice results in error
    VARIABLE /*!< this special value forces the parser assume that the argument value is in form var=value and is processed as a variable and stored among variables.
        If the argument has no values then nothing is done, if there are multiple values then each is processed as a separate variable.
        If any of the values is not in form var=value then it throws. Available for both - regular and list options. */
};

/*! An option is an argument with a name of one or more characters prefixed by a special character (eg. - or --) with exact number of values (0 by default) following it. 

Examples:
-a -x -z A        # 2 short options without values, 3rd with value
-axz A            # same as above, but stacked (when enabled)
--name            # a long option without value
--name value      # a long option with value
--name=value      # same as above (when enabled)
--myargs A B X    # a long option with 3 values
/H                # option with different prefix
+a ++switch       # options with diffenent prefix
*/
struct optionDefinition_t
{
    typedef std::list<name_t> names_t;
    names_t Names; //!< synonyms denoting the option (all short and long names including prefixes)
    string_t Description; //!< help string
    size_t ValueCount; //!< number of values associated with the option
    multiple_t Multi; //!< defines the behavior when option is provided multiple times

    typedef std::function<bool(const optionDefinition_t& p, values_t&)> CB_t;
    CB_t CB; //!< the callback is called before the values are assigned to the parsed arguments; throw to signalize error in value, return false to ignore the value
};

/*! A list option is similar to a regular option. The difference is that instead of hardcoded number of values (as in option) the list has an undefined list of values terminated by a delimiter.
Example:
--values Several Values Can Be Given -- # values as individual arguments, list is terminated by --
-a Another set of values as individual arguments '' # list is terminated by empty string
*/
struct listDefinition_t
{
    typedef std::list<name_t> names_t;
    names_t Names; //!< synonyms denoting the option (all short and long names including prefixes)
    string_t Delimiter; //!< value denoting the end of list, may even be empty string (which might be unintuitive)
    string_t Description; //!< help string
    multiple_t Multi; //!< defines the behavior when option is provided multiple times

    typedef std::function<bool(const listDefinition_t& p, values_t&)> CB_t;
    CB_t CB; //!< the callback is called before the values are assigned to the parsed arguments; throw to signalize error in value, return false to ignore the value
};

/*! A positinal argument is a value which remains after processing all options (and variable if enabled). */
struct positionalDefinition_t
{
    string_t Shorthand; //!< the help id of the positional argument
    string_t Description; //!< help string
    bool Mandatory; //!< defines whether this positional parameter must be given (=true) or is optional (=false); note that only the last mandatory needs to have this flag set

    typedef std::function<bool(const positionalDefinition_t& p, string_t& v)> CB_t;
    CB_t CB; //!< the callback is called before the values are assigned to the parsed arguments; throw to signalize error in value, return false to ignore the value
};

/*! A variable is a special positional parameter in form name=value.
Note that there can also be "variable" options. See multiple_t::VARIABLE. */
struct variableDefinition_t
{
    string_t Name; //!< name variable
    string_t Description; //!< help string
    string_t Default; //!< default value of the variable (ie. the variable is always defined even if not given in arguments)

    typedef std::function<bool(const variableDefinition_t& p, string_t& v)> CB_t;
    CB_t CB; //!< the callback is called before the values are assigned to the parsed arguments; throw to signalize error in value, return false to ignore the value
};

/*! Set of all definitions which can be given to the parser (arguments_t). */
struct definitions_t
{
    typedef std::list<optionDefinition_t> opts_t;
    typedef std::list<listDefinition_t> lists_t;
    typedef std::list<positionalDefinition_t> poss_t;
    typedef std::list<variableDefinition_t> vars_t;

    typedef std::pair<string_t, string_t> helpSection_t;
    typedef std::list<helpSection_t> helpSections_t;

    opts_t Options; //!< regular option definitions
    lists_t ListOptions; //!< list option definitions
    poss_t Positionals; //!< positional argument definitions
    vars_t Variables; //!< variable definitions
    helpSections_t Sections; //!< additional sections of information related to arguments
};

/*! Modifiers of parser behavior. */
enum class flags_t
{
    USE_POSITIONAL_DELIMITER, //!< whether value in PositionalDelimiter shall be taken into account
    USE_RETURN_DELIMITER, //!< whether value in ReturnDelimiter shall be taken into account
    TREAT_VARIABLES_IN_EXPLICIT_POSITIONALS, //!< if set then parser will check for variable definitions within a section after PositionalDelimiter (before ReturnDelimiter); by default it does not treat variables there
    LIST_MUST_TERMINATE, //!< the list end delimiter must always be specified (even if it would be the last argument)
    DENY_ADDITIONAL, //!< no unprocessed positional parameters can remain
    ALLOW_STACKS, //!< allows specifying short options in stack (eg. -abc instead of -a -b -c)
    ALLOW_STACK_VALUES, //!< when an option that requires a value is encountered in stack then the remaining characters in stack (if any) are considered to be the value (eg. -avalue instead of -a value); if such option is last character in stack then still the next argument is taken as value; this works also without ALLOW_STACKS
    ALLOW_SHORT_ASSIGN, //!< allow values to be part of same argument behind a = (eg. -a=value instead of -a value); note: such = does not work with LOOSE_STACK_VALUES
    ALLOW_LONG_ASSIGN, //!< allow values to be part of same argument behind a = (eg. --arg=value instead of --arg value)

    MAX_FLAGS
};
/*! Modifiers of parser behavior (only applies if ALLOW_STACKS is given). */
enum class stackOptionValues_t
{
    REGULAR, //!< default; in stacks of short options, only the last in stack can have value
    LOOSE //!< allows values for short options in stack not only at the end (eg. -ab 3 5 instead of -a 3 -b 5)
};

/*! Determines what happens with arguments in form 'var=value' where var is unknown. */
enum class unknownVariableBehavior_t
{
    IS_POSITIONAL, //!< arguments are considered additional positional arguments
    IS_VARIABLE, //!< arguments are considered a new variable; such ones will have no associated definition
    IS_ERROR //!< encountering this will cause throwing std::runtime_error
};

/*! Specifies how individual option prefixes are treated.
By default - is short option, -- and / are long options.
Note eg. stacking (ie. -abc = -a -b -c) only works for short options. */
enum type_t
{
    SHORT_OPTION, //!< prefix treated as short option (eg. stacking works), by default -
    LONG_OPTION //!< prefix treated as long option, by default --
};

/*! Metadata for an option prefix. */
struct prefixInfo_t
{
    type_t Type; //!< type of option with this prefix
};

/*! The actual parser (and holder) of command line arguments. */
struct arguments_t
{
    /*! Ctor */
    arguments_t();

    typedef jj::options_T<opt::f<flags_t, flags_t::MAX_FLAGS>, opt::e<stackOptionValues_t>, opt::e<unknownVariableBehavior_t>> parserOptions_t;
    parserOptions_t ParserOptions; //!< options of the parser
    case_t OptionCase, //!< whether option names shall be treated case-sensitively
        VariableCase; //!< whether variable names shall be treated case-sensitively
    size_t ParseStart; //!< where to start the parsing of arguments, effectively an index into argv; note that setting this to >0 affects whether the ProgramName is parsed

    typedef std::map<string_t, prefixInfo_t> prefixes_t;
    /*! Defines how option prefixes are treated.
    If a prefix is not defined here explicitly then if encountered in the definitions it will auto-defined
    by assuming single character prefixes to be short options and multi character prefixes long options. */
    prefixes_t PrefixInfo;
    void setup_basic_prefixes();
    const prefixInfo_t& ensure_prefix(const string_t& prefix);

    string_t PositionalDelimiter, //!< if this sequence is matching an argument then all arguments beyond it are treated as positional parameters; only if USE_POSITIONAL_DELIMITER is set
        ReturnDelimiter; //!< in section after PositionalDelimiter was matched if this sequence is matched then the parser returns to regular processing of arguments; only if USE_RETURN_DELIMITER is set

    /*! Parses given argument definitions and verifies their consistency. Throws on error.
    You MUST ensure the lifetime of defs is longer than the last call of the other parse.
    You MUST always call this after you modify the options of the parser and before calling parse(argc,argv). */
    void parse(const definitions_t& defs);
    /*! Inserts the default help option into defs.Options. If used then it has to be called before parse(definitions_t) call. */
    void add_default_help(definitions_t& defs);
    /*! Prints the "help" to standard output. Can only be called after parse(definitions_t) was succesfully called. */
    void print_default_help();
    /*! Parses the ProgramName only from given string (which usually would be argv[0]). */
    void parse_program_name(const char_t* pn);
    /*! Parses given commandline arguments based on definitions parsed before. Throws on error. */
    void parse(int argc, const char_t** argv);
    /*! A shorthand for the other parse() methods. */
    void parse(const definitions_t& defs, int argc, const char_t** argv) { parse(defs); parse(argc, argv); }

    /*! Union helper. */
    enum optionType_t { TREG, TLIST };
    struct optionData_t
    {
        optionType_t Type; //!< which member of union is valid
        union {
            const optionDefinition_t* Opt;
            const listDefinition_t* List;
        } u;
        optionData_t(const optionDefinition_t* opt) : Type(TREG) { u.Opt = opt; }
        optionData_t(const listDefinition_t* lst) : Type(TLIST) { u.List = lst; }
    };
    struct varproxy_t
    {
        string_t Value;
        bool IsDefault;
        const variableDefinition_t* Var;
    };

    string_t ProgramName; //!< holds the program name (argv[0] usually)
    typedef std::pair<optionData_t, values_t> option_t;
    typedef std::map<name_t, option_t, nameCompare_less_t> options_t;
    options_t Options; //!< holds the parsed options (regular and list)
    typedef std::pair<const positionalDefinition_t*, string_t> positional_t;
    typedef std::list<positional_t> positionals_t;
    positionals_t Positionals; //!< holds the parsed positional arguments (both defined through positionalDefinition_t definitions and undefined (for those definition pointer is nullptr))
    typedef std::map<string_t, varproxy_t, nameCompare_less_t> varmap_t;
    varmap_t Variables; //!< preprocessed variable definition data (and actual variable values after argv parsed)

private:
    typedef std::map<name_t, optionData_t, nameCompare_less_t> optmap_t;
    optmap_t opts_; //!< preprocessed option definition data

    const definitions_t* defs_; //!< definitions as passed to parse()

    void clear_data();
    typedef std::deque<options_t::value_type> missingValues_t;
    void add_option(options_t::value_type& opt);
    void add_option_value(missingValues_t& mv, const char_t* value);
    void handle_new_option(missingValues_t& mv, const name_t& name, optionData_t data, const char_t* value);
    void process_long_option(missingValues_t& mv, const string_t& prefix, const char_t* arg);
    void process_short_option(missingValues_t& mv, const string_t& prefix, const char_t* arg);
    bool process_variable(bool mustbe, const char_t* arg);
    void process_positional(bool explicitPositionals, definitions_t::poss_t::const_iterator& cpos, const char_t* arg);
};

} // namespace cmdLine
} // namespace jj

#endif // JJ_CMDLINE_H
