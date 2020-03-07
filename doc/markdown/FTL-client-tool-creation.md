# <span dir="ltr">Writing New FTL Client Tools  
</span>

### 

A llibrary, called "libftl", is available that can be used to extend
existing, or create new, interactive command line interfaces.

It can be embedded into any number of command line utilities. The 'ftl'
tool is available for testing (just) the library itself.

## Overview

This library provides a framework for the execution of textual commands.
It's main features are

  - command lines can be read from many sources including the terminal
    and files
  - command lines from the terminal have history and "readline" support
    on Linux
  - macro substitution and long-line processing are dealt with
  - will read initial lines from a ".rc" file
  - C functions that implement new commands are easily written and
    include help text
  - provides a number of built-in commands, including "help", "set",
    "source"
  - a small language,
    [FTL](https://sites.google.com/site/graygirling/ftl), is available
    for extending the set of commands, manipulating values returned by
    commands, obtaining values from the system etc.

Below there are sections describing

  - Simple Use - all you need to read in order to make use of the
    library
  - Built-in Commands - some "free" commands you can give your user

[Elsewhere](https://sites.google.com/site/graygirling/ftl/command-line-tutorial)
you can find more information about:  

  - FTL expressions - how values are generated in the built-in language
  - Scripting - ways that you and your user can extend the functionality
    of your interpreter
  - Control information available to scripts - command line,
    environment, file and windows registry access
  - The FTL script-testing utility: ftl

## Simple Use

This example code defines a small number of commands including a "sleep"
command.

``` 
 
#include <unistd.h> /* for sleep() */ 
 
#include "ci/efabcfg/ftl.h" 
 
: 
: 
 
static const value_t * 
cmd_sleep(const char **ref_line, const value_t *this_cmd, parser_state_t *state) 
{   number_t duration_s; 
     
    if (parse_int_expr(ref_line, state, &duration_s) &&  
        parse_empty(ref_line)) 
    {   sleep(duration_s); 
    } else 
        parser_report_help(state, this_cmd); 
 
    return &value_null; 
} 
 
 
 
static void 
cmds_mine(dir_t *cmds) 
{   mod_add(cmds, "dothis", "<syntax> - do this", &cmd_dothis); 
    mod_add(cmds, "dothat", "<thatstuff> - do that", &cmd_dothat); 
    : 
    mod_add(cmds, "sleep", "<number> - sleeps for <number> seconds", &cmd_sleep); 
    --       cmd name ^    help ^           function implementing command ^ 
} 
 
: 
: 
 
extern int 
main(int argc, char **argv) 
{   const char *init = NULL;               -- pre-execution string 
    char *app_argv[APP_ARGC_MAX]; 
    int app_argc = 0; 
     
    ftl_init();                            -- initialize library 
 
    if (parse_args(argc, argv, .. &init, &app_argc, &app_argv, APP_ARGC_MAX)) 
    {   parser_state_t *state = parser_state_new(dir_id_new()); 
                                           -- create a place to put commands etc. 
        if (NULL != state) 
        {   cmds_generic(state, app_argc, app_argv); 
                                           -- add pre-defined generic commands 
  
            cli(state, init, "mytester");  -- run the command line interpreter 
            -- commands ^     ^ name of utility 
            parser_state_free(state);      -- discard state 
        } 
    } else 
        usage(); 
     
   ftl_end();                              -- finalize library 
   return 0; 
} 
 
```

In the above `init` can be set to a string (such as "sleep; echo done;
exit") which will be executed before commands are taken from the
terminal. The `app_argc` and `app_argv` values optionally give scripts
access to command line arguments. (The function `parse_args` is not
provided as part of the library.) The string `"mytester"` provides a
name for the utility that will be used when providing error messages
etc.

### Parsing Support

Each command function has the form:

    static value_t * 
    cmd_sleep(const char **ref_line, value_t *this_cmd, parser_state_t *state) 

in which:

  - *ref\_line* is a pointer to a pointer to a zero terminated string of
    arguments
  - *this\_cmd* represents the command that is being executed
  - *state* indirects to various pieces of information such as the
    execution stack and the list of commands currently in scope

The argument line has already been assembled from input text - splitting
at each ";", combining lines ending "\\", and expanding any macros
(specified as "$\<name\>" or "${\<name\>}") etc.

The command must update ref\_line to point to the end of the string,
otherwise the interpreter will warn that there was unparsed input.

A number of functions are available to help with this with the general
format:

    bool parse_<something>(const char **ref_line, <output args>) 

which can conveniently be combined (for simple command lines) into a
single if condition that checks the correctness of the syntax. In
general each function will parse a \<something\>, updating ref\_line to
skip over it if successful.

These functions include:

|                                                                  |                                                                                                                                                                                                                                                          |
| ---------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| parse\_empty(ref\_line);                                         | succeeds only if the line contains no more characters - this should always be called as a final parse function                                                                                                                                           |
| parse\_white(ref\_line);                                         | true iff there is leading white space on the line (and fails if there is not)                                                                                                                                                                            |
| parse\_space(ref\_line);                                         | always true - this function simply skips over any white space                                                                                                                                                                                            |
| parse\_key(ref\_line, "keyword");                                | true iff the text "keyword" is next on the line                                                                                                                                                                                                          |
| parse\_int(ref\_line, \&longint);                                | true iff the next characters specify a decimal number                                                                                                                                                                                                    |
| parse\_hex(ref\_line, \&usigned\_longint);                       | true iff the next character specify a hexadecimal number                                                                                                                                                                                                 |
| parse\_hex\_width(ref\_line, width, \&unsigned\_longint);        | true iff the next \<width\> characters specify a hexadecimal number                                                                                                                                                                                      |
| parse\_int\_val(ref\_line, \&longint);                           | true iff the next characters are a number specified \[-\]0x\<hex\> or \[-\]\<decimal\>                                                                                                                                                                   |
| parse\_int\_expr(ref\_line, state, \&longint)                    | true iff the next characters can be parsed as a numeric expression potentially involving brackets, +, -, \*, /, rsh, lsh, ge, le, gt, lt, ==, \!= with the normal priorities (rsh & lsh are shifts) - an ftl expression can occur (only) within brackets |
| parse\_item(ref\_line, delims, ndelims, buf, len);               | true iff there is a non-empty item of non-delimiter characters - which are placed in the buffer provided                                                                                                                                                 |
| parse\_id(ref\_line, buf, len);                                  | true iff there is an identifier (alphabetic or '\_' followed by alphanumerics or '\_') which is written to the buffer provided                                                                                                                           |
| parse\_string(ref\_line, buf, len);                              | true iff there is a string beginning " or ' - written to the buffer                                                                                                                                                                                      |
| parse\_string\_expr(ref\_line, state, buf, len, \&string\_value) | true iff the next characters can be parsed as a numeric expression potentially involving brackets and + (concatenation) - an ftl expression can occur (only) within brackets                                                                             |
| parse\_itemstr(ref\_line, buf, len);                             | true iff there is either an item or a string on the command line                                                                                                                                                                                         |
| parse\_ipaddr(ref\_line, \&ipaddr);                              | true iff there is a dotted quad (n.n.n.n) on the command line                                                                                                                                                                                            |
| parse\_macaddr(ref\_line, \&macaddr);                            | true iff there is a mac address (h:h:h:h:h:h) on the command line                                                                                                                                                                                        |

For example to parse the syntax "\<integer\> \[name \<string\>\]
\<possibly-quoted-item\>" the following could be used

``` 
    char **ref_line = ... /* position in string to be parsed */ 
    int i; 
    bool name_is_set; 
    char name[32]; 
    char item[32]; 
     
    if (parse_int_val(ref_line, &i) &&  
        parse_space(ref_line) && 
 
        (!(name_is_set = parse_key(ref_line, "name")) ||  
         (parse_space(ref_line) &&  
          parse_string(ref_line, &name[0], sizeof(name)) &&  
          parse_space(ref_line)) 
        ) && 
 
        parse_itemstr(ref_line, &item[0], sizeof(item)) && 
        parse_empty(ref_line)) 
    { 
        /* function using i, name_is_set, name and item */ 
    }  
    else 
        /* syntax error */ 
        ... 
```

### Subcommands

It is possible to specify a set of commands as a kind of command itself.
For example if the *cmds\_mine* function above had been written as
follows:

    static void 
    cmds_mine(mod_t *cmds) 
    {   dir_t *mycmds = dir_id_new(); 
     
        if (NULL != mycmds) 
        {   mod_add(mycmds, "dothis", "<syntax> - do this", &cmd_dothis); 
            mod_add(mycmds, "dothat", "<thatstuff> - do that", &cmd_dothat); 
            : 
            mod_add(mycmds, "sleep", "<number> - sleeps for <number> seconds", &cmd_sleep); 
     
            mod_add_dir(cmds, "my", mycmds); 
        } 
    } 

then commands "my dothis", "my dothat", ... "my sleep", "my help" would
be available in place of "dothis", "dothat" ... "sleep". The command
"help" is predefined by the library and will be able to be used as "my
help" - showing help about only the "my" subcommands.

There is also another way to add commands by adding new functions to the
testing language - this is described "Adding built-in commands" below.

## Built-in Commands

In the above example the line

``` 
        cmds_generic(state); 
```

provides a number of commands (that may grow in the future). They are
divided into groups with prefixes

  - \<none\> - language features
  - io - input and output
  - sys - operating system interface
  - parse - language parsing library
  - reg - Windows registry (Windows only)

The language features can be divided into those for:

  - Program control
  - General features
  - Types
  - Booleans
  - Integers
  - Environments and directories
  - Character handling
  - Input and Output
  - Parser interface
  - Operating System interface
  - Windows-only interface

These are described in more detail
[elsewhere](https://sites.google.com/site/graygirling/ftl/ftl-user-guide).  
  

## Adding FTL Functions

"Simple Use", above described how a command that parses its own
arguments can be created.

It is also possible to add FTL functions - which are presented with a
fixed number of FTL values to operate on. For example, the
implementation of the "rnd" functions is:

``` 
    static const value_t * 
    fn_rnd(const value_t *this_fn, parser_state_t *state) 
    {   const value_t *upb = parser_builtin_arg(state, 1); /* get the first argument */ 
        const value_t *val = &value_null;                  /* default return value */ 
     
        if (value_istype(upb, type_int)) 
        {   number_t upbval = value_int_number(upb); 
            val = value_int_new((int)(((float)upbval)*rand()/(RAND_MAX+1.0))); 
        } else 
            parser_report_help(state, this_fn); 
 
        return val; 
    } 
```

and it was added to the functions in the library using

``` 
    mod_addfn(cmds, "rnd", 
              "<n> - return random number less than <n>",  &fn_rnd, /*no of args*/ 1); 
```

Other functions can easily be added. FTL values can be of a number of
types including: null values, type values, integer values, string
values, FTL code values, environment values, closure values, and stream
values. Each of these has their own support provided by functions
declared in the header "ftl.h".
