FTL User's Guide

## Introduction

### Version

This guide applies to the FTL run-time library with version 1.28. 

### Overview

The primary use of FTL is to facilitate the simple creation of tools
driven by command line scripts that can either be read from a script
file or typed line-by-line on a console. This is achieved by writing
programs in C or C++ which create FTL *commands* and FTL *functions*
which are used directly to provide the basic operations a tool wishes to
make available. This C or C++ can make use of the FTL library which 

  - can handle common aspects of a script interpretation
  - provides a run-time library of additional FTL commands, functions
    and values which can be made available alongside the features
    provided by the tool.

The advantage to the tool writer of using FTL is that the simple script
language supported can be extended using the FTL language in such a way
that the scripts made available to the tool user can be far more
sophisticated than would otherwise been the case.

The typical structure of a tool written using FTL involves

  - the FTL run-time library
  - zero or more FTL run-time library extensions
  - C/C++ code written by the tool writer
  - a "prolog" written in FTL which provides a user scripting
    environment based on that provided by the tool writer

The FTL language itself is syntactically more diverse (and therefore
more complex for the tool user to learn) than the basic script syntax
we'd expect tool users to require. Although FTL is available to the tool
user, typically only the separate, and simpler, basic script syntax
should be needed.

### **Basic Script** Syntax

Command lines are extracted from a command file (or command line
console) after 

  - discard of commented-out lines (those beginning '\#') 
  - concatenation of lines ending with an escape ('\\') 
  - separation of command lines joined on the line using semicolon (';')
    
  - expansion of textual substitutions introduced with an expansion
    escape ('$') 
  - incorporation in to a single command line of every line that is
    enclosed by brackets( and *)*,  *{* and *}*,  *\<* and *\>* , and
    double quoted (*"*) strings.

The characters '\#', '\\' and '$' are not used as part of FTL expression
syntax. 

Some examples (including the new line prompt '\> ') might include:

\> echo Hello World

\> echo A Long \\

\> Hello \\

\> World

\> echo Hello World; echo Hello Again

\> set lines "more than

\> one line

\> with newlines in it"

\> set program {

\>   echo Hello World

\> }

\> echo $lines; echo ${lines}

The macro expansion is similar to that traditionally found in POSIX
shell programs, and is describe in a section below "Macro Expansion".

Command lines usually have a structure (also familiar to those who have
used POSIX shells) in which an initial word is taken as the name of a
command and further text provides further detail about what is required
often with no syntax more complex than further words, numbers and quoted
strings. The way that these lines are interpreted in the light of the
current values of variables in the FTL language is explained just below
"Standard Command Line Parsing".

In a fashion, the interpretation of these command lines amounts to a
very simple programming language. That language, however, is not FTL
itself. 

One consequence of this is that the way a variable is used will be
different depending on whether it is used as part of the command line
syntax or as part of FTL expression syntax.

#### Standard Command Line Parsing

The run-time library consists of a number of objects provided by default
in the root environment. They fall in to the following categories:  

|             |                                                                                |
| ----------- | ------------------------------------------------------------------------------ |
| Function    | Built-in closure taking a fixed number of FTL arguments                        |
| Command     | Built-in program that parses its own command line after parameter substitution |
| Variable    | FTL value initialized by the interpreter                                       |
| Environment | FTL directory value providing values for specific names                        |

  
These are used in two separate environments:

1.  As the command on a command line - one of the initial elements on an
    interpreted command line 

<!-- end list -->

  -   - When a *function* name is used as the first element in the line
        the rest of the command line must provide its arguments,
        separated by spaces.  Enough arguments to bind to all unbound
        variables must be provided (and no more).  The function is then
        executed and the result, if not NULL, is printed out. 
      - When a *command* name is used as the first element in the line
        the rest of the command line is $-expanded and given to the
        command to parse.  A warning is produced if it completes before
        using all the characters on the line and the result, if not
        NULL, is otherwise printed out. 
      - When an *environment* name is used as the first element in the
        line that environment is pushed on to the environment stack and
        the rest of the line is reparsed as a command in that
        environment (as above). 
      - A *variable* name, which does not refer to any of the above
        types of value can not be used. 

<!-- end list -->

1.  As an FTL value 

<!-- end list -->

  -   - The value is returned as part of an FTL expression. 

These are some examples of command lines:

\> command anything

\> directory subdirectory-in-directory command-in-subdirectory any
syntax at all

\> function FTL-expression-1 FTL-expression-2

\> directory FTL-expression-in-directory-1 FTL-expression-in-directory-2

Because the primary aim of the language is to provide a command
interpreter most input files are interpreted as scripts in the script
syntax, with FTL expressions used only when required for command
definition and extension.

### FTL Literals Syntax

> \<code\_literal\> ::= '{' \[ '\\' \<char\> | \<char\> \]\* \] '}'  
> \<string\> ::= '"' \['\\' \<escape\> | \<char\> \]\* '"'  
> \<integer\> ::= \[\<base\>\] \<basedigits\>  
> \<real\> ::= \[\<base\>\] \<basedigits\> '.' \[\<basedigits\>\]
> \[\<exp\>\<basedigits\>\]  
> \<base\> ::= '0x' | '0X' | '0o' | '0O' | '0b' | '0B'  
> \<identifier\> ::= \<common\_identifier\> | \<builtin\_identifier\>  
> \<common\_identifier\> ::= \<alphabetic\> (\<alphabetic\> | '\_' |
> \<digit\>)\*  
> \<builtin\_identifier\> ::= '\_' \<digit\>+ 

In \<code\_literal\>s all the characters between an initial '{' and a
matching '}' are used except '\\' which is used to ensure the following
character is included (e.g. '\\\\' will include the '\\' character). A
'}' matches the initial '{' only when it does not occur within bracketed
text (i.e. text between '{' and '}', or between an initial double quote
(") and its matching closing double quote). 

In a \<string\> all characters between an initial double quote (") and
its matching closing double quote are included except '\\' which is used
to introduce one or more characters in an escape sequence.

> \<basedigits\> ::= \<digit\> (\<digit\> | '\_')\*

For \<integers\> the numeric base 16 is specified by '0x' and '0X'; the
base 8 is specified by '0o' and '0O'; and the base 2 is specified by
'0b' and '0B'. Integers beginning with a numeric base specification use
digits from '0' up to a character representing one less than the base.
The characters 'A', 'B', 'C', 'D', 'E' and 'F', or 'a', 'b, 'c', 'd',
'e', and 'f' are used for digits of value 10, 11, 12, 13, 14, and 15 as
required by base 16. Following the initial digit the character '\_' can
be used – it is ignored in the value and is accepted only to aid the
comprehension of long numbers (e.g. it might be used every three
characters as a thousands separator, or every eight binary digits as a
byte separator). 

> \<exp\> ::= \<exp14\> | \<exp25\>  
> \<exp14\> ::= 'e' | 'E'  
> \<exp25\> ::= 'p' | 'P'

Note that \<real\>s are supported only as a compile time option in the
library. When they are available the same base selection is used for
\<integer\>s and applied to each of the optional \<basedigits\> parts of
the literal. The first of these provides the integer part and is always
present (it may be zero). The second is the factional part, and the
third specifies the exponent (of the base used). The character
introducing the exponent is \<exp25\> (e.g. 'p') for hex real literals
and \<exp14\> (e.g. 'e') otherwise (following the convention used in C).

### FTL Expression Syntax

\<expression\> ::= (\<index\> '=')\* \<invocation\>

\<invocation\> ::= (\<substitution\> \['\!'\]\*)+ 

\<substitution\> ::= \['&'\] \<operation\>+

\<operation\> ::= user\_operator\_parse(\<retrieval\> | \<reference\>)

\<reference\> ::= '@' \<retrieval\>

\<retrieval\> ::= \[\<closure\>\]\['.' \<fieldsindex\>\]\*

\<fieldsindex\> ::= \<index\> |

\<vector\> |

\<id\_environment\>

\<closure\> ::= (\<base\> \[':' | '::'\])\* \<base\>

\<base\> ::= '(' \<expression\> ')' |

\<code\> |

\<id\_environment\> |

\<identifier\> |

\<vector\> |

\<type\_literal\>

\<code\> ::= \<code\_literal\>  
\<type\_literal\> ::= \<integer\> | \<real\> | \<string\> 

\<expression\_list\> ::= \<expression\> (';' \<expression\>)\* \[';'\]

\<id\_environment\> ::= '\[' \[\<binding\_list\> | 

\<binding\_list\> ',' \<unbound\_list\> |

\<unbound\_list\>\] '\]' 

\<binding\_list\> ::= \[\<binding\> (',' \<binding\>)\*\] 

\<unbound\_list\> ::= \<index\> (',' \<index\>)\*

\<binding\> ::= \<index\> '=' \<invocation\>

\<vector\> ::= '\<' \<implied\_series\> | \<series\> '\>'

\<index\> ::= \<identifier\> | \<integer\> | \<string\> | '('
\<expression\> ')'

\<series\> ::= \[\<series\_binding\> (',' \<seres\_binding\>)\*\]

\<implied\_series\> ::= \[\<series\_binding\> \[','
\<series\_binding\>\]\] '..' \[\<series\_binding\>\]

\<series\_binding\> ::= \[\<invocation\> '='\] \<invocation\>

In a \<code\> value the \<code\_literal\> is interpreted as an
\<expression\_list\>.

### FTL Expression evaluation

#### User-Operator Parsing

User-operators are provided as a convenience to the language user.  The
functionality of the language is completely accessible without them.  
  
User-operator parsing is determined by the set of operations defined
using parse.opset (described as part of the run-time library below).
 Each operator definition links the name of an operator (which may be
one or more characters and can involve unused punctuation characters or
identifiers) with a precedence, an associativity and a monadic or diadic
binding.  The associativity determines whether the operator is a prefix,
infix, or postfix one and which of its arguments may be parsed at the
same precedence . The precedence defines how tightly the arguments of
the operator are bound to the operator in comparison with other
operators.  The binding should take either one (for a monadic operator)
or two (for a dyadic operator) arguments and will be executed once the
operator's arguments have been parsed successfully.  

#### Standard Operator Definitions

A number of user-operators are defined as part of the basic run-time
library, but these may be removed, changed or added to.  Operator
definitions are held in the priority-ordered vector parse.op, which
initially contains:

|          |            |          |       |
| -------- | ---------- | -------- | ----- |
| Priority | Operator   | Function | Assoc |
| 0        | \_or\_     | logor    | xfy   |
| 1        | \_and\_    | logand   | xfy   |
| 2        | \_not\_    | invert   | fy    |
| 3        | \_bitor\_  | bitor    | xfy   |
| 4        | \_bitxor\_ | bitxor   | xfy   |
| 5        | \_bitand\_ | bitand   | xfy   |
| 6        | \_bitnot\_ | bitnot   | fy    |
| 7        | \==        | equal    | xfx   |
| \!=      | notequal   | xfx      |       |
| \_lt\_   | less       | xfx      |       |
| \_le\_   | lesseq     | xfx      |       |
| \_gt\_   | more       | xfx      |       |
| \_ge\_   | moreeq     | xfx      |       |
| 8        | \_shl\_    | shiftl   | xfy   |
| \_shr\_  | shiftr     | xfy      |       |
| 9        | \-         | neg      | fy    |
| 10       | \+         | add      | xfy   |
| \-       | sub        | xfy      |       |
| 11       | \*         | mul      | xfy   |
| /        | div        | xfy      |       |
| \_rem\_  | mod        | xfy      |       |
| 12       | \*\*       | pow      | xfy   |

Note that the priorities refer to the relative position in the vector
parse.op - their absolute value is not relevant.

#### Default Parsing

After parsing user-operators all evaluation in FTL occurs in an
\<invocation\> 

  - where a final '\!' has been used.  In an invocation a \<code\> value
    or a \<closure\> is executed; or,
  - where the \<invocation\> is marked for automatic execution (e.g.
    using the func command) and the \<invocation\> has no unbound
    variables.

When a \<code\> value is executed the associated text is interpreted as
an \<expression\_list\> in the current environment (i.e. so that names
within the code body will have the values currently in scope at the
point of invocation).

A \<closure\> is an association between a \<code\> value and an
environment, which is normally constructed from a stack of component
environments.  The environment can consist both of names with associated
values (bound variables) and names with no current value (unbound
variables). Only closures with no unbound variables can be executed.
 When it is executed the environment at the point of invocation
replaced by the environment associated with the closure and the \<code\>
value is then executed in that environment.  The values in scope are
those currently available in the (stacked) closure environments - not
the values that were defined in those environments at the time of
definition of the closure (see below).

There are two operators provided to create a \<closure\> both involving
the specification of arguments that are an environment or \<code\>
values.  The first is the double-colon operator (::), which creates a
closure whose environment is precisely what has been specified by the
environment(s) supplied.  The second is the colon operator (:), which
creates a closure whose environment is the provided value stacked on to
the current environment (when the colon operator is executed).

Although it is not currently precisely the case, it is intended that all
the symbols (e.g. eventually including numeric and string operators and
constants) in a \<code\> value are interpreted relative to the
environment in which they execute. 

### FTL Variable Scope

At any given time the currently accessible scope is defined as an
environment that is normally itself constructed as a stack of other
environments or simple directories.  Within a directory some name
bindings are constant - which means that the value associated with the
name can not be changed,  Also, some stack environments may be "locked"
- which means that they can not be extended with new values, and names
can not be removed from them.

A directory can contain only one value for a name but, because the same
name can be used in different directories in a stacked environment it is
possible that there will be more than one name-value pair in the
environment.  When a name is used in an environment it refers to the
value the name given in the most recently pushed directory,
independently of whether it is constant or not (writing to a constant
value will fail).

The only visible value for 'a' in this environment

\[a=X\]::\[a=Y\]::\[a=Z\]

will be Z.  
  
If a value is being provided for a new name, without specifying a parent
directory, and no name is found in the current environment then the
directory used to store the new name in is the one most recently pushed,
independently of whether it is locked or not (so creating a new
name-value pair when the last pushed directory is locked will fail).    
  
The empty directory name as a parent specifies the entire environment.
 This notation can be used to create variable names using the FTL
indexing notation (see below).  Thus ".*name*" will refer to the same
variable that  *name* does.

### FTL Indexing

The dot ('.') provides a couple of syntactical short-cuts.  Normally

X.Y

is equivalent to

X::{Y}\!

That is, "Y" is interpreted in the context of the environment "X".  Thus
if X is an environment containing strings X."*string*" will return the
value associated with the string "*string*" and if it contains integers
X.*integer* refers to the value associated with the integer *integer*.
 For convenience FTL also allows the syntax X.*identifier* as an
equivalent to X."*identifier*".  Not all strings are identifiers:
identifiers are those which begin with an alphabetic character and then
continue with characters that are either alphanumeric or the character
'\_'.  
  
Similarly if Y is itself a literal environment each of the values in it
will be interpreted as names in X and will be replaced by their values
in X.  This applies recursively to any values in Y that are
environments.  Currently the convenience of using string values without
a string denotation does not also apply to literal environments.  
  
The syntax 

X.(Y) 

can be used when a literal value is not available.  The value of
expression Y is used as an object to be interpreted in the context of X.
 If this value is an integer, string or environment it is treated in the
way indicated above.  
  
Examples:<span id="anchor"></span>

\> set vec \<2,6,9\>

\> set ids \[a=6,b=2,h=9\]

\> eval vec.0

2

\> eval ids.h

9

\> eval ids."h"

9

\> eval vec.(ids.b)

9

\> eval vec.\<2,1,2\>

\<9, 6, 9\>

\> eval vec.\[f=2,g=1,h=2\]

\[f=9, g=6, h=9\]

\> eval ids.\<"b","a"\>

\<2, 6\>

\> eval ids.\[q="h", l="a"\]

\[q=9, l=6\]

\> eval vec.\<4\>

\<\>

\> eval vec.\<4,0,1\>

\<1=2, 6\>

\> eval vec.(ids)

\[a=, b=9, h=\]

\> eval ids.\[x="i", y="b"\]

\[x=, y=2\]

\> inenv ids.\[x="i", y="b"\] "x"

FALSE

\> inenv ids.\[x="i", y="b"\] "y"

TRUE

### FTL Local Variables

A directory can be used as a set of local variable names using the enter
built in function (see below); or a closure with local variables as its
arguments can be used. For example a function to check whether a string
is empty that uses a local variable "enter" could be written like this:

> set is\_empty \[string\]: {

>     \[answer = FALSE\]:{

>         (len string\!) == 0 {answer = TRUE}\!;

>         answer}\!

> }

or using "enter":

> set is\_empty \[string\]: {

>     enter \[answer = FALSE\]\!;

>     (len string\!) == 0 {answer = TRUE}\!;

>     answer

> }

The "leave" or "leaving" functions return to the previous environment as
it was before a matching enter. The difference between them is that
leaving returns the environment that was built up since the matching
enter.

> set make\_imaginary \[re,im\]: {

>     enter \[numbertype = "complex"\]\!;

>     .real = re;

>     .imaginary = im;

>     leaving\!

> }

> \> make\_imaginary 3 4

> \[numbertype="complex",real=3,imaginary=4\]

### FTL References

Left hand values (references) can be established with the '@' operator
placed before a \<name\> or retrieval consisting of a number of names
concatenated using '.'.  The reference is closure that takes a single
argument whose value will be assigned to the name when it is executed.
 When the left hand value of something specified with more than one
concatenated name is taken the object specified by the names up to the
last must exist, and will constitute the environment in which the final
name is assigned when the reference is executed.  The following are
equivalent:

@a 20\!

and

a = 20

### FTL Values

Each value associated with a name belongs to one of the value types
supported.  These types, and the values they support include the
following:

<table>
<tbody>
<tr class="odd">
<td>Type</td>
<td>Values</td>
</tr>
<tr class="even">
<td>nul</td>
<td>This type has only a single value, NULL </td>
</tr>
<tr class="odd">
<td>type</td>
<td>This type contains different values corresponding to each of the supported types.</td>
</tr>
<tr class="even">
<td>int</td>
<td>This type is used for integers.  The integers supported are those that can be represented in 64 bits.  Denotations are available for specifying both positive and negative integer values however denotations that appear more than  9223372036854775807 (0x7fffffffffffffff) can be expected to be indistinguishable from values for negative numbers. </td>
</tr>
<tr class="odd">
<td>real</td>
<td>This type is used for floating point values. The number of bits used in this representation depends on the compiler used. Ideally it would allow the precise representation of a signed 63-bit integer, but on many Windows compilers this is unfortunately not possible (only a signed 52-bit integer).<br />
This type is available depending on a compile-time option.</td>
</tr>
<tr class="even">
<td>bool</td>
<td>This type contains the values TRUE and FALSE.<br />
This is not a true built-in type as the others are, TRUE and FALSE are both actually closures (see below).</td>
</tr>
<tr class="odd">
<td>string</td>
<td>This type is used for a sequence of 8-bit octets (bytes). Memory-size permitting, the sequence can contain an implementation-defined number of bytes which must be at least 2^31.  The bytes in the sequence can contain any 8-bit value (including zero).  Denotations are available that allow specific hexadecimal byte values to be inserted, byte values taken from the default character set; and, multi-byte sequences corresponding both to specific characters from a wide (normally Unicode) character set and from the input character set.  In general the run-time library provides support for access to strings treated as octet containers separately from those treated as characters containers.  Each character is encoded as a fixed sized, possibly multi-byte, sequence of octets.</td>
</tr>
<tr class="even">
<td>code</td>
<td>This type is used for program text.  Normally this text can not be "executed" until an environment in which its values can be interpreted is supplied.</td>
</tr>
<tr class="odd">
<td>dir</td>
<td>This type is used to contain a mapping between "names" and values.  The names are themselves values and these types must come from types that support a comparison operator.  Currently the types are limited to integers and strings.   Currently separate denotations are available for string indexed directories, integer indexed directories and integer indexed integer sequences.  Currently the storage requirement for an integer indexed directory is proportional to the maximum index used, and only positive indeces are supported.<br />
In addition to "names" with associated values directories can also contain an ordered set of unbound names with no associated value.  </td>
</tr>
<tr class="even">
<td>closure</td>
<td>This type incorporates both a code value and a dir value that specifies the environment in which code should be interpreted.  Values can be bound to unbound names held in their dir values.  These values can not be "executed" unless there are no unbound names in their dir value.  When they are executed their code value is interpreted in the environment specified by their dir value.<br />
Closures can be marked for automatic execution. If they are not marked for automatic execution, execution requires an explicit “!”. Otherwise execution will occur automatically when the closure has no unbound names.</td>
</tr>
<tr class="odd">
<td>cmd</td>
<td>This type supports different kind of executable value from a closure.  Each value incorporates an executable program which will parse a single string argument.  A number of commands are built-in in the run time library.</td>
</tr>
<tr class="even">
<td>function</td>
<td>This type supports built-in closure values that use a fixed number of arguments taken from a specific range of names ("_1", "_2" ... etc.).  The run-time library provides a number of these values.</td>
</tr>
<tr class="odd">
<td>stream</td>
<td>This type supports values that can either supply or consume a series of octets (bytes).</td>
</tr>
<tr class="even">
<td>ipaddr</td>
<td>This type is used to contain an Internet IP address.  Denotations are available both for specifying the address as a  "dotted quad" or an Internet host name. Currently only addresses for IPv4 are supported.</td>
</tr>
<tr class="odd">
<td>macaddr</td>
<td>This type is used to contain an IEEE 802.3 MAC address.  A denotation is available for specifying the address as a colon-separated sextet. Currently only 48-bit addresses are supported.</td>
</tr>
</tbody>
</table>

### Boolean Values

In this version of FTL the values of TRUE and FALSE are:

TRUE = \[val\]::{val\!}

FALSE = \[val\]::{FALSE}

That is, TRUE is a closure that will execute its argument and FALSE is a
closure that will discard its argument and return FALSE instead.  Thus
if a value that is either TRUE or FALSE is given code as an argument: 

\<true\_or\_false\> { \<code\> }\!

the result will be either the result of the code's execution or the
value FALSE.  Thus the code is conditionally executed.  
A compound boolean expression can be built up in which each item in the
chain is executed only if previous ones have evaluated to TRUE:

\<true\_or\_false\> {\<boolean\_expression\>}\!
{\<boolean\_expression\>}\! ... {\<boolean\_expression\>}\!

Thus this construct can be used as a conditionally executed "and".

### Automatic Execution of Closures

The most explicit form of FTL does not use automatic execution of
closures: a “\!” is always required to indicate when a closure is to be
executed, even if it has no unbound variables. For example the built in
function add has two unbound variables:

\> eval add

\[\_1, \_2\]::\<func:0x7fe091a2856b,2\>

\> eval add 1

\[\_2\]::\<func:0x7fe091a2856b,2\>

\> eval add 1 2

\[\]::\<func:0x7fe091a2856b,2\>

\> eval add 1 2 \!

3

While this makes the meaning of code very clear the “\!” is nearly
redundant – in the sense that, once an argument has been applied that
assigns the last unbound variable, it is almost always then executed.
For this reason, for convenience, a closure can be marked for automatic
execution, whereby such closures are executed automatically if they
occur with no bound variables.

By default none of the built-in functions have closure values values
marked for automatic execution. However this is dependent on a compile
time option. When provided this option will make all the built in
functions execute automatically and effectively results in a completely
different dialect, which is not used in the examples provided by this
document.

The func command is provided to mark a normal closure as one for
automatic execution. One closure created from another by substituting a
value for an unbound variable will inherit the marking. For example

\> func fnadd\[a,b\]:{add a b\!}

\> eval fnadd

\[a, b\]:{add a b\!}

\> eval fnadd 1

\[b\]:{add a b\!}

\> eval fnadd 1 2

3

Because a closure with no unbound variables can be useful as a value,
there is an operator ‘&’ that can be used to return a closure which is
not marked for automatic execution. For example:

\> eval \&fnadd 1

\[b\]:{add a b\!}

\> eval \&fnadd 1 2

\[\]:{add a b\!}

\> eval \&fnadd 1 2 \!

3

### Macro Expansion

The command line handler incorporates a traditional-style macro handler
which uses the '$' character to introduce a macro invocation.

#### What a Macro Invocation Looks Like

Macro invocation uses one of these syntaxes:

  - $\<decimal\_number\> - expand the value of FTL expression
    ".\<decimal\_number\>"
  - $\<identifier\> - expand the value of FTL expression
    "\<identifier\>"
  - ${\<expression\>} - expand the value of FTL expression
    "\<expression\>"

The expression .1, as a \<decimal\_number\>, for example will return the
value of symbol 1 in top level of the current environment. This means
that, when a series of environments have been stacked (as might occur
when integers are used as unbound variables in a series of closures
which have invoked each other) .1 will refer only to the current
invocation.

The expression var, as an \<identifier\>, will refer to the current
value of that symbol (at any level in the environment).

Note that the first two syntaxes can be re-expressed using the third,
since $\<decimal\_number\> is the same as ${.\<decimal\_number\>} and
$\<identifier\> is the same as ${\<identifier\>}. This can be convenient
when the macro is to be expanded in text which might be consumed as part
of a \<decimal\_number\> or an \<identifier\>. For example:

  - '12 ${item}s' can be used in place of the otherwise ambiguous '12
    $items'
  - '${.1}000 bytes' could be used in place of the otherwise ambiguous
    '$1000 bytes'

When an expression is expanded its value is calculated and then the
macro invocation characters are replaced by text. If the value is a
string that string forms the text of the replacement. Otherwise the
replacement text will be the text with which that value would normally
be printed.

#### When Macro Expansion Takes Place

Macro expansion occurs on two separate occasions:

1.  When using the standard command line parsing method: after each line
    of input is complete (e.g. once a number of input lines using '\\'
    at the end of each one are concatenated, or the lines between
    matching brackets are concatenated).
2.  Before a command is invoked as part of an FTL expression the
    expansion is applied to its argument. (When commands, which usually
    consume the text of the rest of the line when used on a command
    line, are used in this way they take a single string argument.) 

In either case macros invocations are sought throughout the text except:

  - if the leading '$' is preceded by a backslash (\\) character; or,

  - if the invocation occurs in a section of the text bracketed by '{'
    and '}'
    
      - where the final '}' matches the first '{' in terms of included
        '{ .. }' sections
      - where the initial '{' is not preceded by a backslash (\\)
        character.

Naturally, the result of a macro expansion depends on the values of
symbols in the environment when the expansion takes place. 

In the first case, of a completed line of input, the environment is the
one left by any previous line. This is true even if the line includes a
prefix which denotes a change or restriction of the current environment.

In the second case the environment used is the latest one, which
includes the binding of a single unbound variable representing the
command's string argument.

These examples use the standard command line parsing method:

\> set name "Eric"

\> echo Your name is $name

Your name is Eric

\> echo {Your name is $name}

{Your name is $name}

\> set hi\[name\]:{echo "Hello $name"\!}

\> hi "sunshine"

Hello sunshine

\> echo hi is $hi

hi is \[name\]:{echo "Hello $name"\!}

\> set age 50

\> eval echo "Hi $name, happy ${age}th"\!

Hi Eric, happy 50th

\> eval echo {Hi $name, happy ${age}th}\!

Hi Eric, happy 50th

\> set person \[name="Sunny Joe", age=30\]

\> person eval echo "Hi $name, happy ${age}th"\!

Hi Eric, happy 50th

\> person eval echo {Hi $name, happy ${age}th}\!

Hi Sunny Joe, happy 30th

\> set args \<"copy", "fileA", "fileB"\>

\> echo Program ${args.0}

Program copy

\> args eval echo {${.0}ing $1 to $2}\!

copying fileA to fileB

\> set log\[0,1,2\]:{ echo ${.0}ing $1 to $2"\! }

\> log "copy" "fileA" "fileB"

copying fileA to fileB

### Run-time library overview

The values available in the run time library include those in the
following categories. 

  - Program control 
    
      - catch \<excep\> \<do\> - run \<do\> executing \<excep\> \<ex\>
        on an exception
      - do \<do\> \<test\> - execute \<do\> while \<test\> evaluates
        non-zero 
      - every \<n\> \<command\> - repeat command every \<n\> ms
      - exit - abandon all command inputs
      - for \<env\> \<binding\> - execute \<binding\> for all \<env\>
        values
      - forall \<env\> \<binding\> - execute \<binding\> \<val\>
        \<name\> for all entries in \<env\>
      - forwhile \<env\> \<binding\> - for \<binding\> while it is not
        FALSE                 
      - forallwhile \<env\> \<binding\> - forall \<binding\> while it is
        not FALSE
      - if \<b\> \<then-code\> \<else-code\> - execute \<then-code\> if
        \<n\>\!=FALSE
      - return \<rc\> - abandon current command input returning \<rc\>
      - source \<filename\> - read from file \<filename\>
      - sourcetext \<string\>|\<code\> - read characters from string or
        code
      - throw \<value\> - signal an exception with \<value\>, exit outer
        'catch'
      - while \<test\> \<do\> - while \<test\> evaluates non-zero
        execute \<do\> 

  - General 
    
      - cmd \<function\> \<help\> - create a command from a function 
      - cmp \<expr\> \<expr\> - returns integer comparing its arguments 
      - command \<whole line\> - execute as a line of source
      - echo \<whole line\> - prints the line out 
      - eval \<expr\> - evaluate expression 
      - func \<name\> \<closure\> - set auto exec closure value in
        environment
      - help - prints command information
      - set \<name\> \<expr\> - set value in environment
      - sleep \<n\> - sleep for \<n\> milliseconds

  - Types 
    
      - basetype - directory of built in type values
      - NULL - nul value 
      - type \<typename\> - return the type with the given name 
      - typeof \<expr\> - returns the type of \<expr\> 
      - typename \<expr\> - returns the name of the type of \<expr\>

  - Booleans 
    
      - TRUE - TRUE value (an un-FALSE value)
      - FALSE - the FALSE value 
      - equal \<val\> \<val\> - TRUE if first \<val\> equal to second 
      - invert \<val\> - TRUE if \<val\> is FALSE, FALSE otherwise
      - less \<val\> \<val\> - TRUE if first \<val\> less than second 
      - lesseq \<val\> \<val\> - TRUE if first \<val\> less than or
        equal to second 
      - logand \<val1\> \<val2\> - FALSE if \<val1\> is FALSE, \<val2\>
        otherwise 
      - logor \<val1\> \<val2\> - TRUE if \<val1\> is TRUE, \<val2\>
        otherwise
      - more \<val\> \<val\> - TRUE if first \<val\> more than second 
      - moreeq \<val\> \<val\> - TRUE if first \<val\> more than or
        equal to second 
      - notequal \<val\> \<val\> - TRUE if first \<val\> not equal to
        second 

  - Integers 
    
      - bitor \<n1\> \<n2\> - return n1 "or"ed with n2 
      - bitxor \<n1\> \<n2\> - return n1 exclusive "or"ed with n2 
      - bitand \<n1\> \<n2\> - return n1 "and"ed with n2 
      - bitnot \<n\> - return the bitwise "not" of n 
      - int \<integer expr\> - numeric value 
      - intseq \<first\> \<inc\> \<last\> - vector of integers
        incrementing by \<inc\> 
      - int\_fmt\_hexbits \<n\> - print decimal if all bits in this
        mask, else hex
      - rndseed \<n\> | \<string\> - set random seed based on argument
      - rndseq \<n\> - vector of integers containing 0..\<n\>-1 in a
        random order
      - rnd \<n\> - return random number less than \<n\>
      - shiftl \<n1\> \<n2\> - return n1 left shifted by n2 bits
      - shiftr \<n1\> \<n2\> - return n1 right shifted by n2 bits

  - Integers or Reals
    
      - abs \<n\> - absolute (positive) value of \<n\>
      - add \<n1\> \<n2\> - return n1 with n2 added
      - div \<n1\> \<n2\> - return n1 divided by n2
      - mod \<n1\> \<n2\> - return n1 modulus n2
      - mul \<n1\> \<n2\> - return n1 multiplied by n2
      - neg \<n\> - return negated \<n\>
      - pow \<n1\> \<n2\> - return n1 to the power of n2
      - sub \<n1\> \<n2\> - return n1 with n2 subtracted

  - Reals
    
      - real \<real expr\> - real value
      - rint \<n\> - nearby integer to \<n\>
      - ceil \<n\> - number smallest integer not less than \<n\>
      - floor \<n\> - number largest integer not greater than \<n\>
      - round \<n\> - number of the nearest integer
      - sqrt \<x\> - square root of \<x\>
      - exp \<x\> - e to the power of \<x\>
      - log \<x\> - logarithm of \<x\> base e
      - sin \<x\> - trigonometric sine of \<x\> radians
      - cos \<x\> - trigonometric cosine of \<x\> radians
      - tan \<x\> - trigonometric tangent of \<x\> radians
      - asin \<x\> - trigonometric inverse sine of \<x\>
      - acos \<x\> - trigonometric inverse cosine of \<x\>
      - atan \<x\> - trigonometric inverse tangent of \<x\>
      - sinh \<x\> - hyperbolic sine of \<x\> radians
      - cosh \<x\> - hyperbolic cosine of \<x\> radians
      - tanh \<x\> - hyperbolic tangent of \<x\> radians
      - asinh \<x\> - hyperbolic inverse sine of \<x\>
      - acosh \<x\> - hyperbolic inverse cosine of \<x\>
      - atanh \<x\> - hyperbolic inverse tangent of \<x\>

  - Environments and directories 
    
      - domain \<env\> - generate vector of names in \<env\> 
      - dynenv \<getall\> \<count\> \<set\> \<get\> - dynamic env built
        from given functions
      - enter \<env\> - add commands from \<env\> to current environment
      - envjoin \<env1\> \<env2\> - composed directory indexing \<env2\>
        with \<env1\> values
      - inenv \<env\> \<name\> - returns 0 unless string \<name\> is in
        \<env\>
      - leave - exit the environment last entered or restricted 
      - leaving - exit the environment last entered or restricted &
        return it
      - len \[\<env\>|\<closure\>|\<string\>\] - number of elements in
        object
      - lock \<env\> - prevent new names being added to \<env\>
      - new \<env\> - copy all \<env\> values
      - range \<env\> - generate vector of values in \<env\> 
      - restrict \<env\> - restrict further commands to those in \<env\>
        
      - rndseq \<n\> - vector of integers containing 0..\<n\>-1 in a
        random order
      - select \<binding\> \<env\> - subset of \<env\> for which
        \<binding\> returns TRUE
      - sort \<env\> - sorted vector of values in \<env\> 
      - sortby \<cmpfn\> \<env\> - sorted vector of values in \<env\>
        using \<cmpfn\> 
      - sortdom \<env\> - sorted vector of names in \<env\> 
      - sortdomby \<cmpfn\> \<env\> - sorted vector of names in \<env\>
        using \<cmpfn\> 
      - zip \<dom\> \<range\> - generate environment with given domain
        and range taken from \<range\>

  - Closures
    
      - argname \<closure\> - return name of 1st argument to be bound or
        NULL 
      - argnames \<closure\> - return vector of arguments to be bound
      - bind \<closure\> \<arg\> - bind argument to unbound closure
        argument 
      - closure \<push\> \<dir\> \<code\> - create closure from code and
        dir (+ stack if push)
      - code \<closure\> - return code component of a closure 
      - context \<closure\> - return environment component of a closure
      - deprime \<closure\> - unmark closure for automatic execution
      - func \<name\> \<closure\> - set auto exec closure value in
        environment
      - prime \<closure\> - mark closure for automatic execution when
        all args bound

  - Character handling 
    
      - binsplit \<le?\> \<signed?\> \<n\> \<str\> - make vector of
        \<signed\>\> \<n\>-byte ints with \<le?\> endianness
      - chr \<int\> - returns string of (multibyte) char with given
        ordinal 
      - chrcode \<string\> - returns ordinal of the first character of
        string 
      - collate \<str1\> \<str2\> - compare collating sequence of chars
        in strings 
      - chop \<stride\> \<str\> - make vector of strings each \<stride\>
        bytes or less
      - chopn \<n\> \<stride\> \<str\> - make vector of \<n\> strings
        each \<stride\> bytes or less
      - len \[\<env\>|\<closure\>|\<string\>\] - number of elements in
        object
      - join \<delim\> \<str\> - join vector of octets and strings
        separated by \<delim\>s
      - joinchr \<delim\> \<str\> - join vector of chars and strings
        separated by \<delim\>s
      - octet \<int\> - returns single byte string containing given
        octet
      - octetcode \<string\> - returns ordinal of the first byte of
        string
      - split \<delim\> \<str\> - make vector of strings separated by
        \<delim\>s 
      - str \<expr\> - evaluate expression and return string
        representation 
      - strf \<fmt\> \<env\> - formatted string from values in \<env\> 
      - strcoll \<str1\> \<str2\> - compare collating sequence of chars
        in strings 
      - strlen \[\<string\>\] - number of (possibly multibyte) chars in
        string 
      - tolower \<str\> - lower case version of string 
      - toupper \<str\> - upper case version of string 

  - Binary data handling
    
      - binsplit \<le?\> \<signed?\> \<n\> \<str\> - make vector of
        \<signed\>\> \<n\>-byte ints with \<le?\> endianness
      - mem base\_can \<mem\> \[rwgc\] \<ix\> - return start of area
        ending at \<ix\> that can do ops
      - mem base\_cant \<mem\> \[rwgc\] \<ix\> - return start of area
        ending at \<ix\> that can do ops
      - mem bin \<base\> \<string\> - create mem with base index and
        read-only string
      - mem block \<base\> \<len\> - create block of mem with len bytes
        and base index
      - mem dump \<+char?\> \<ln2entryb\> \<mem\> \<ix\> \<len\> - dump
        content of memory
      - mem get \<mem\> \<ix\> \<len\> - force read \<len\> string at
        \<ix\> in memory
      - mem len\_can \<mem\> \[rwgc\] \<ix\> - return length of area at
        \<ix\> that can do ops
      - mem len\_cant \<mem\> \[rwgc\] \<ix\> - return length of area at
        \<ix\> that can not do ops
      - mem read \<mem\> \<ix\> \<len\> - read \<len\> string at \<ix\>
        in memory
      - mem rebase \<mem\> \<base\> - place \<mem\> at byte index
        \<base\>
      - mem write \<mem\> \<ix\> \<str\> - write binary string at index
        to memory

  - Input and Output 
    
      - io binfile \<filename\> \<rw\> - return stream for opened binary
        file
      - io close \<stream\> - close stream
      - io connect \<protocol\> \<netaddress\> \<rw\> - return stream
        for remote connection
      - io err - default error stream
      - io file \<filename\> \<rw\> - return stream for opened file 
      - io filetostring \<filename\> \[\<outfile\>\] - write file out as
        a C string
      - io flush \<stream\> - ensure unbuffered output is written
      - io fmt - updateable directory of formatting functions for
        io.fprintf 
      - io fprintf \<stream\> \<format\> \<env\> - write formatted
        string to stream 
      - io getc \<stream\> - read the next character from the stream 
      - io in - default input stream 
      - io inblocked \<stream\> - TRUE if reading would block
      - io instring \<string\> \<rw\> - return stream for reading string
      - io listen \<protocol\> \<netport\> \<rw\> - return stream for
        local port
      - io out - default output stream 
      - io outstring \<closure\> - apply output stream to closure and
        return string 
      - io pathfile \<path\> \<filename\> \<rw\> - return stream for
        opened file on path 
      - io pathbinfile \<path\> \<filename\> \<rw\> - return stream for
        opened binary file on path
      - io read \<stream\> \<size\> - read up to \<size\> bytes from
        stream
      - io ready \<stream\> - return whether next write may not cause
        wait
      - io stringify \<stream\> \<expr\> - write FTL representation to
        stream 
      - io write \<stream\> \<string\> - write string to stream 

  - Parser interface 
    
      - parse argv - directory of command line arguments for command
      - parse assoc - environment containing operator associativity
        definitions 
      - parse codeid - name of interpreter
      - parse env - return current invocation environment
      - parse errors - total number of errors encountered by parser 
      - parse errors\_reset \<n\> - reset total number of errors to
        \<n\> 
      - parse exec \<cmds\> \<stream\> - return value of executing
        initial \<cmds\> then stream
      - parse execargv - directory of command line arguments given to
        interpreter
      - parse expand \<inc{}\> \<env\> \<val\> - expand macros in string
        or code \<val\>
      - parse line - number of the line in the character source
      - parse local - return local current invocation directory
      - parse newerror - register the occurrance of a new error 
      - parse op - environment containing operation definitions 
      - parse opeval \<opdefs\> \<code\> - execute code according to
        operator definitions 
      - parse opset \<opdefs\> \<prec\> \<assoc\> \<name\> \<function\>
        - define an operator in opdefs 
      - parse readline - read and expand a line from the command input 
      - parse rdenv \<cmds\> \<stream\> - return env made executing
        initial \<cmds\> then stream
      - parse rdmod \<module\> - return env made executing module file
        on path
      - parse root - return current root environment 
      - parse scan \<string\> - return parse object from string 
      - parse scancode \<@string\> \<parseobj\> - parse {code} block
        from parse object
      - parse scanempty \<parseobj\> - parse empty line from string from
        parse object, update string 
      - parse scanspace \<parseobj\> - parse over white space from
        string from parse object, update string
      - parse scanhex \<@int\> \<parseobj\> - parse hex string from
        parse object, update string
      - parse scanhexw \<width\> \<@int\> \<parseobj\> - parse hex in
        \<width\> chars from parse object, update string 
      - parse scanid \<@string\> \<parseobj\> - parse identifier, update
        string
      - parse scanint \<@int\> \<parseobj\> - parse integer from string
        from parse object, update string 
      - parse scanintval \<@int\> \<parseobj\> - parse signed based
        integer from parse object
      - parse scanitem \<delims\> \<@string\> \<parseobj\> - parse item
        until delimiter, update string 
      - parse scanitemstr \<@string\> \<parseobj\> - parse item or
        string, update string
      - parse scanmatch \<dir\> \<@val\> \<parseobj\> - parse prefix in
        dir from parse object giving matching value
      - parse scanned \<parseobj\> - return text remaining in parse
        object 
      - scanopterm \<opdefs\> \<scanfn\> \<@val\> \<parseobj\> - parse
        term using \<opdefs\> & base \<scanfn\>
      - parse scanstr \<@string\> \<parseobj\> - parse item until
        delimiter, update string
      - scantomatch \<dir\> \<@val\> \<parseobj\> - parse up to
        delimiter named in dir in \<parseobj\> giving matching value
      - parse scanvalue \<@string\> \<parseobj\> - parse a basic value
        in \<parseobj\>
      - parse scanwhite \<parseobj\> - parse white spce from string from
        parse object, update string
      - parse source - name of the source of chars at start of the last
        line 
      - parse stack - return local current invocation directory stack

  - Operating System interface 
    
      - sys env - system environment variable environment 
      - sys fs casematch - whether case must match in file system file
        names value 
      - sys fs absname \<file\> - TRUE iff file has an absolute path
        name 
      - sys fs home - the user's home directory name
      - sys fs nowhere - name of file available as empty source or sink
      - sys fs rcfile - name of the file, in the home directory, holding
        initial commands 
      - sys fs sep - string separating path elements in a file name 
      - sys fs thisdir - name of directory representing the current
        directory
      - sys localtime \<time\> - broken down local time 
      - sys localtimef \<format\> \<time\> - formatted local time 
      - sys osfamily - name of operating system type
      - sys run \<line\> - execute system \<line\> 
      - sys runrc \<command\> - execute system command & return result
        code 
      - sys shell path \<path\> \<file\> - return name of file on path 
      - sys shell pathsep - character value separating directory
        elements in a path environment
      - sys shell self - file system name of interpreter binary
      - sys ticks - current elapsed time measure in ticks
      - sys ticks\_hz - number of ticks per second
      - sys ticks\_start - ticks value when interpreter was started
      - sys time - system calendar time in seconds 
      - sys uid \<user\> - return the UID of the named user 
      - sys utctime \<time\> - broken down UTC time 
      - sys utctimef \<format\> \<time\> - formatted UTC time 

  - Windows-only interface 
    
      - reg value HKEY\_CLASSES\_ROOT \<key\> - open key values in
        HKEY\_CLASSES\_ROOT 
      - reg value HKEY\_USERS \<key\> - open key values in HKEY\_USERS 
      - reg value HKEY\_CURRENT\_USER \<key\> - open key values in
        HKEY\_CURRENT\_USER 
      - reg value HKEY\_LOCAL\_MACHINE \<key\> - open key values in
        HKEY\_LOCAL\_MACHINE 
      - reg value HKEY\_CURRENT\_CONFIG \<key\> - open key values in
        HKEY\_CURRENT\_CONFIG
      - reg allow\_edit \<boolean\> - enable/disable write to the
        registry from new keys
      - reg key - directory of key value types 

## Run-time Library

In the following each library entity is described using a proforma
including a specification of the argument syntax.  This uses
"\<label:syntax\>" to refer to something parsed according to syntax
indicated in the following table that is then referred to in a
description using the name "label".

<table>
<tbody>
<tr class="odd">
<td>any</td>
<td>FTL expression returning any type of value</td>
</tr>
<tr class="even">
<td>int</td>
<td>FTL expression returning an integer value</td>
</tr>
<tr class="odd">
<td>real</td>
<td>FTL expression returning a real value (when optionally supported)</td>
</tr>
<tr class="even">
<td>bool</td>
<td>FTL expression returning either TRUE or FALSE</td>
</tr>
<tr class="odd">
<td>code</td>
<td>FTL expression returning a code value</td>
</tr>
<tr class="even">
<td>dir</td>
<td>FTL expression rerturning a directory value</td>
</tr>
<tr class="odd">
<td>closure</td>
<td>FTL expression rerturning a closure value</td>
</tr>
<tr class="even">
<td>clodir</td>
<td>&lt;closure&gt; | &lt;dir&gt;</td>
</tr>
<tr class="odd">
<td>clocode</td>
<td>&lt;closure&gt; | &lt;code&gt;</td>
</tr>
<tr class="even">
<td>intordir</td>
<td>&lt;int&gt; | &lt;dir&gt;</td>
</tr>
<tr class="odd">
<td>number</td>
<td>&lt;int&gt; | &lt;real&gt;</td>
</tr>
<tr class="even">
<td>string</td>
<td>FTL expression rerturning a string value</td>
</tr>
<tr class="odd">
<td>stringnl</td>
<td>FTL expression rerturning a string, NULL or integer</td>
</tr>
<tr class="even">
<td>stringorcode</td>
<td>FTL expression rerturning either a string or a code value</td>
</tr>
<tr class="odd">
<td>token</td>
<td>Any sequence of characters with no white space</td>
</tr>
<tr class="even">
<td>tokstring</td>
<td>Either a &lt;token&gt; or a literal string</td>
</tr>
<tr class="odd">
<td>restofline</td>
<td><p>The sequence of characters starting at the first non-white character and ending at the end of the line.</p>
<p>Note: when supplied to a command inside an FTL expression this is supplied as a string value or a code value. When a string is used it is subject to macro expansion before being provided to the command.</p></td>
</tr>
</tbody>
</table>

  
The names in the default run-time library are presented in alphabetic
order below. 

### **abs** \<**number**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>abs</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n:number&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes an integer or real and returns its absolute (i.e. positive) value.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer or real with the value |n|.<br />
See also: neg, add, sub, mul, div, pow</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; abs 10</p>
<p>10</p>
<p>&gt; abs -10</p>
<p>10</p>
<p>&gt; abs -10.10</p>
<p>10.1</p>
<p>&gt; abs 10.10</p>
<p>10.1</p></td>
</tr>
</tbody>
</table>

### **acos** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>acos</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns a number of radians whose trigonometric cosine would equal that value.<br />
The inverse function to cos.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value cos<sup>-1</sup>(x).<br />
See also: asin, atan, sin, cos, tan, acosh</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (acos 0.0!)/PI</p>
<p>0.500000000000</p>
<p>&gt; preal (acos 0.75**0.5!)/PI</p>
<p>0.166666666667</p>
<p>&gt; preal (acos 0.5**0.5!)/PI</p>
<p>0.250000000000</p>
<p>&gt; preal (acos 0.0!)/PI</p>
<p>0.500000000000</p></td>
</tr>
</tbody>
</table>

### **acosh** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>acosh</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns a number whose hyperbolic cosine would equal that value.<br />
The inverse function to cosh.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value cosh<sup>-1</sup>(x).<br />
See also: asinh, atanh, sinh, cosh, tanh, acos</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (acosh 0.0!)</p>
<p>-nan</p>
<p>&gt; preal (acosh 1.0!)</p>
<p>0.000000000000</p>
<p>&gt; preal (acosh (-1.0)!)</p>
<p>-nan</p>
<p>&gt; preal (acosh (cosh 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### add \<**number**\> \<**number**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>add</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n1:number&gt; &lt;n2:number&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two integers or reals and adds them together.<br />
If either n1 or n2 are real numbers the result will be a real.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer or real with the value n1+n2.<br />
See also: sub, mul, div, pow, abs, neg, shiftr, shiftl</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; add 0x100 12</p>
<p>268</p>
<p>&gt; sys localtimef "%T" (add (sys.time!) 600!)</p>
<p>"17:30:14"</p>
<p>&gt; eval add 4 0.3!</p>
<p>4.3</p></td>
</tr>
</tbody>
</table>

### argname \<closure\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>argname</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;closure&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Return only the unbound variable part of a closure (e.g. a function) that would be bound to an argument.<br />
There are three main portions of a closure: its bound environment, its unbound environment and its code.  This function returns the last created unbound name.  If there are no unbound variable NULL is returned.<br />
See also: bind, code, context, closure, argnames</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the name of the variable next to be bound to an argument.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; argname [b=1,a]::{a+b}</p>
<p>"a"</p>
<p>&gt; argname set</p>
<p>"_1"</p>
<p>&gt; set needs_argument[fn]:{NULL != (argname fn!)}</p>
<p>&gt; needs_argument needs_argument</p>
<p>TRUE</p>
<p>&gt; needs_argument []:{echo "no"!}</p>
<p>FALSE</p></td>
</tr>
</tbody>
</table>

### argnames \<closure\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>argnames</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;closure&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns a vector containing only the names of the unbound variables in a closure (e.g. a function).<br />
There are three main portions of a closure: its bound environment, its unbound environment and its code.  This function returns the unbound names.  The order of the names is the same as the order in which they will be taken by succeeding bindings (i.e. the first element in the sequence is the name of the variable that will be bound first - this is the value that 'argname' would return). If there are no unbound variable an empty vector is returned.<br />
See also: bind, code, context, closure, argname</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the name of the variable next to be bound to an argument.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; argnames [b=1,a]::{a+b}</p>
<p>&lt;"a"&gt;</p>
<p>&gt; argnames add</p>
<p>&lt;"_1", "_2"&gt;</p>
<p>&gt; set argument_count[fn]:{len (argnames fn!)!}</p>
<p>&gt; argument_count add</p>
<p>2</p></td>
</tr>
</tbody>
</table>

### **asin** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>asin</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns a number of radians whose trigonometric sine would equal that value.<br />
The inverse function to sin.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value sin<sup>-1</sup>(x).<br />
See also: acos, atan, sin, cos, tan, asinh</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (asin 0.0!)</p>
<p>0.000000000000</p>
<p>&gt; preal (asin 0.75**0.5!)/PI</p>
<p>0.333333333333</p>
<p>&gt; preal (asin 0.5**0.5!)/PI</p>
<p>0.250000000000</p>
<p>&gt; preal (asin 1.0!)/PI</p>
<p>0.500000000000</p>
<p>&gt; preal (asin (sin 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### **asinh** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>asinh</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns a number whose hyperbolic sine would equal that value.<br />
The inverse function to sinh.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value sinh<sup>-1</sup>(x).<br />
See also: acosh, atanh, sinh, cosh, tanh, asin</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (asinh 0.0!)</p>
<p>0.000000000000</p>
<p>&gt; preal (asinh 1.0!)</p>
<p>0.881373587020</p>
<p>&gt; preal (asinh (-1.0)!)</p>
<p>-0.881373587020</p>
<p>&gt; preal (asinh (sinh 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### **atan** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>atan</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns a number of radians whose trigonometric tangent would equal that value.<br />
The inverse function to tan.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value tan<sup>-1</sup>(x).<br />
See also: sin, cos, asin, acos, atan, tanh</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (atan 0.0!)</p>
<p>0.000000000000</p>
<p>&gt; preal (atan 3.0**0.5!)/PI</p>
<p>0.333333333333</p>
<p>&gt; preal (atan 1.0!)/PI</p>
<p>0.250000000000</p>
<p>&gt; preal (atan (tan 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### **atanh** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>atanh</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns a number whose hyperbolic tangent would equal that value.<br />
The inverse function to tanh.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value tanh<sup>-1</sup>(x).<br />
See also: sinh, cosh, asinh, acosh, atanh, tan</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (atanh 0.0!)</p>
<p>0.000000000000</p>
<p>&gt; preal (atanh 1.0!)</p>
<p>inf</p>
<p>&gt; preal (atanh (-1.0)!)</p>
<p>-inf</p>
<p>&gt; preal (atanh 0.5!)</p>
<p>0.549306144334</p>
<p>&gt; preal (atanh (tanh 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### basetype

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>basetype</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Directory</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Contains the values of types built-in to the parser.<br />
When printed, types contain the names held by this directory.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Not a function.<br />
See also:type, typename, typeof</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; typename “some text”</p>
<p>“string”</p>
<p>&gt; typeof "some text"</p>
<p>$basetype.string</p>
<p>&gt; eval basetype.string</p>
<p>$basetype.string</p>
<p>&gt; help all basetype</p>
<p>all basetype &lt;subcommand&gt; - commands:</p>
<p>    type - type value</p>
<p>    nul - type value</p>
<p>    string - type value</p>
<p>    code - type value</p>
<p>    closure - type value</p>
<p>    int - type value</p>
<p>    dir - type value</p>
<p>    cmd - type value</p>
<p>    fn - type value</p>
<p>    stream - type value</p>
<p>    ip - type value</p>
<p>    mac - type value</p>
<p>    coroutine - type value</p>
<p>    mem - type value</p></td>
</tr>
</tbody>
</table>

### bind \<closure\> \<any\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>bind</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;fn:closure&gt; &lt;arg:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a closure with at least one unbound variable and binds the given value to the next to be bound.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td><p>A closure that has a previously unbound variable bound to the given value. Note that this does not cause the resulting closure to be invoked even if there are no remaining unbound variables in the closure.<br />
This is an explicit function to represent what normally occurs when using the syntax: &lt;fn&gt; &lt;arg&gt;. It is equivalent to a function defined:</p>
<p>set bind[fn, arg]:{fn arg}</p>
<p>See also: closure, code, context, argname, argnames</p></td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set dir bind [a] 33!</p>
<p>&gt; eval dir</p>
<p>[a=33]</p>
<p>&gt; set inc bind add 1!</p>
<p>&gt; inc 3</p>
<p>4</p></td>
</tr>
</tbody>
</table>

### binsplit \<bool\> \<bool\> \<int\> \<string\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>binsplit</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;le:bool&gt; &lt;signed:bool&gt; &lt;bytes:int&gt; &lt;data:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Creates a read-only vector of ‘bytes’ length integers taken from the provided data. Each ‘bytes’-length integer is created from consecutive bytes in the data treated as a little-endian value if le is TRUE and a big-endian value if it is FALSE. Similarly each value is taken as a signed value if ‘signed’ is TRUE and unsigned otherwise.<br />
The result is constructed from whole sequences of ‘bytes’ bytes starting from the first byte. The remainder of the data (that is less than ‘bytes’ in length) is not represented in the resulting vector.</p>
<p>This function creates an alternative representation of the data ‘data’ and does not duplicate the memory required to hold it.</p>
<p>Valid values for ‘bytes’ are only 1, 2, or 4.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Bitwise "and" of b1 and b2.<br />
See also: chop</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set bin "\xff\xff\xff\xff\x80\x00\x00\x01\x01\x00\x00\xff"</p>
<p>&gt; int_fmt_hexbits 255</p>
<p>-1</p>
<p>&gt; binsplit TRUE TRUE 4 bin</p>
<p>&lt;-1, 0x1000080, 0xffffffffff000001&gt;</p>
<p>&gt; binsplit TRUE FALSE 4 bin</p>
<p>&lt;0xffffffff, 0x1000080, 0xff000001&gt;</p>
<p>&gt; binsplit FALSE TRUE 4 bin</p>
<p>&lt;-1, 0xffffffff80000001, 0x10000ff&gt;</p>
<p>&gt; binsplit FALSE FALSE 4 bin</p>
<p>&lt;0xffffffff, 0x80000001, 0x10000ff&gt;</p>
<p>&gt; binsplit TRUE TRUE 2 bin</p>
<p>&lt;-1, -1, 128, 0x100, 1, 0xffffffffffffff00&gt;</p>
<p>&gt; binsplit TRUE FALSE 2 bin</p>
<p>&lt;0xffff, 0xffff, 128, 0x100, 1, 0xff00&gt;</p>
<p>&gt; binsplit FALSE TRUE 2 bin</p>
<p>&lt;-1, -1, 0xffffffffffff8000, 1, 0x100, 255&gt;</p>
<p>&gt; binsplit FALSE FALSE 2 bin</p>
<p>&lt;0xffff, 0xffff, 0x8000, 1, 0x100, 255&gt;</p>
<p>&gt; binsplit TRUE TRUE 1 bin</p>
<p>&lt;-1, -1, -1, -1, -128, 0, 0, 1, 1, 0, 0, -1&gt;</p>
<p>&gt; binsplit TRUE FALSE 1 bin</p>
<p>&lt;255, 255, 255, 255, 128, 0, 0, 1, 1, 0, 0, 255&gt;</p></td>
</tr>
</tbody>
</table>

### bitand \<int\> \<int\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>bitand</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;b1:int&gt; &lt;b2:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two sets of 64 bits (held in the two integers represented in two's complement arithmetic) and returns the integer representing the "and" of corresponding bits.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Bitwise "and" of b1 and b2.<br />
See also: or, bitor, bitxor, bitnot</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; bitand 1 3</p>
<p>1</p>
<p>&gt; bitand 0 0xffff</p>
<p>0</p>
<p>&gt; strf "0x%09x" &lt;bitand 0x000fff000 0x123456789!&gt;</p>
<p>"0x000456000"</p></td>
</tr>
</tbody>
</table>

### bitnot \<int\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>bitnot</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;b:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a set of 64 bits (held by the integer represented in two's complement arithmetic) and returns the integer representing the "not" of each bit.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Bitwise inversion of b.<br />
See also: and, bitor, bitxor, bitand</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; bitnot 0</p>
<p>-1</p>
<p>&gt; bitnot 0xffff</p>
<p>-65536</p>
<p>&gt; strf "0x%09x" &lt;bitnot 0x000fff000!&gt;</p>
<p>"0xffffffffff000fff"</p></td>
</tr>
</tbody>
</table>

### bitor \<int\> \<int\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>bitor</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;b1:int&gt; &lt;b2:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two sets of 64 bits (held in the two integers represented in two's complement arithmetic) and returns the integer representing the "or" of corresponding bits.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Bitwise "or" of b1 and b2.<br />
See also: and, bitand, bitxor, bitnot</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; bitor 1 2</p>
<p>3</p>
<p>&gt; bitor 0xffff0000 0xffff</p>
<p>4294967295</p>
<p>&gt; strf "0x%09x" &lt;bitor 0x000fff000 0x123456789!&gt;</p>
<p>"0x123fff789"</p></td>
</tr>
</tbody>
</table>

### bitxor \<int\> \<int\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>bitxor</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;b1:int&gt; &lt;b2:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two sets of 64 bits (held in the two integers represented in two's complement arithmetic) and returns the integer representing the "exclusive or" of corresponding bits.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Bitwise "exclusive or" of b1 and b2.<br />
See also: and, bitand, bitor, bitnot</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; bitxor 1 3</p>
<p>2</p>
<p>&gt; bitxor 0xffff 0xff00</p>
<p>255</p>
<p>&gt; strf "0x%09x" &lt;bitxor 0x000fff000 0x555555555!&gt;</p>
<p>"0x555aaa555"</p></td>
</tr>
</tbody>
</table>

### **ceil** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>ceil</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real and returns a real number representing the smallest integer not less than n (the ceiling of the integral part of the real).</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An real with the smallest integral number less than n.<br />
Note: the result is not an integer – rint can be used to convert it to an integer though.<br />
See also: rint, floor, round</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; ceil 10.1</p>
<p>11</p>
<p>&gt; ceil 10.9</p>
<p>11</p>
<p>&gt; ceil -10.1</p>
<p>-10</p>
<p>&gt; ceil -10.9</p>
<p>-10</p>
<p>&gt; typeof (ceil 10!)</p>
<p>$basetype.int</p>
<p>&gt; typeof (ceil 10.1!)</p>
<p>$basetype.real</p></td>
</tr>
</tbody>
</table>

### chop \<intordir\> \<string\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>chop</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;stride:intordir&gt; &lt;str:string&gt; </td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Returns a vector of successive substrings from &lt;str&gt; as determined by stride. When stride is an integer each substring is |&lt;stride&gt;| octets long except the last which may have fewer (being all that is left).  The substrings are taken sequentially from str with postive values of &lt;stride&gt; being taken from the front of he string and negative ones being taken from the back.</p>
<p>When stride is a directory each element in the vector will have the same shape as the directory (and its subdirectories) except that the values are replaced by successive substrings from &lt;str&gt;.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A vector of successive substrings or directories of substrings taken from the string.<br />
See also: chopn, zip, join</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; chop 3 "abcdefghijk"</p>
<p>&lt;"abc", "def", "ghi", "jk"&gt;</p>
<p>&gt; chop -3 "abcdefghijk"</p>
<p>&lt;"ijk", "fgh", "cde", "ab"&gt;</p>
<p>&gt; set rev[x]:{x.&lt;(len x!)-1..0&gt;}</p>
<p>&gt; join "," (rev (chop -3 (strf "%d" &lt;10000000&gt;!)!)!)</p>
<p>"10,000,000"</p>
<p>&gt; chop [word=-4] "thisand that"</p>
<p>&lt;[word="that"], [word="and "], [word="this"]&gt;</p>
<p>&gt; chop [first=&lt;1,1&gt;,body=6] "* thing1**thing2+*thing3"</p>
<p>&lt;[first=&lt;"*", " "&gt;, body="thing1"], [first=&lt;"*", "*"&gt;, body="thing2"], [first=&lt;"+", "*"&gt;, body="thing3"]&gt;</p>
<p>&gt; join NULL (chop &lt;-5,-4,-2,-4,-2,-4,-5&gt; "able was I ere I saw elba "!)</p>
<p>"elba saw I ere I was able "</p></td>
</tr>
</tbody>
</table>

### chopn \<int\> \<intordir\> \<string\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>chopn</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;items:int&gt; &lt;stride:intordir&gt; &lt;str:string&gt; </td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Returns a vector of two items. The first is similar to that returned by chop with up to &lt;items&gt; successive substrings from &lt;str&gt; as determined by stride. When stride is an integer each substring is |&lt;stride&gt;| octets long except the last which may have fewer (being all that is left).  The substrings are taken sequentially from str with postive values of &lt;stride&gt; being taken from the front of he string and negative ones being taken from the back.</p>
<p>When stride is a directory each element in the vector will have the same shape as the directory (and its subdirectories) except that the values are replaced by successive substrings from &lt;str&gt;.</p>
<p>The second item in the vector returned is a string containing any remaining characters not removed from the string.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A vector of two items. The first a vector of successive substrings or directory of substrings taken from the string; and the second any remaining characters in the string.<br />
See also: zip, chop, join</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set first[n,str]:{ (chopn 1 n str!).0 }</p>
<p>&gt; set last[n,str]:{ (chopn 1 (-n) str!).0 }</p>
<p>&gt; set allbutfirst[n,str]:{ (chopn 1 n str!).1 }</p>
<p>&gt; set allbutlast[n,str]:{ (chopn 1 (-n) str!}.1 }</p>
<p>&gt; first 3 "abcdefghij"</p>
<p>"abc"</p>
<p>&gt; last 3 "abcdefghij"</p>
<p>"hij"</p>
<p>&gt; allbutfirst 3 "abcdefghij"</p>
<p>"defghij"</p>
<p>&gt; allbutlast 3 "abcdefghij"</p>
<p>"abcdefg"</p>
<p>&gt; chopn 1 [op=1,header=7] "3headerhpacketcontent"</p>
<p>&lt;[op="3", header="headerh"], "packetcontent"&gt;</p></td>
</tr>
</tbody>
</table>

### chr \<int\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>chr</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;widechar:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns a string containing a single (possibly-multibyte) character with the given position in the current locale's character set.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string of octets representing the character with the given ordinal number.<br />
See also: octet, chrcode</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; echo ${chr 163!}</p>
<p>£</p>
<p>&gt; chr 163</p>
<p>"\xc2\xa3"</p>
<p>&gt; eval chr 65!</p>
<p>"A"</p></td>
</tr>
</tbody>
</table>

### chrcode \<char\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>chrcode</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;char:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns the ordinal position of the first (possibly-multibyte) character in its argument in the current locale's character set.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer containing the ordinal number.<br />
See also: chr, octetcode</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; eval "£"</p>
<p>"\xc2\xa3"</p>
<p>&gt; chrcode "\xc2\xa3"</p>
<p>163</p>
<p>&gt; eval chrcode "€"!</p>
<p>8364</p></td>
</tr>
</tbody>
</table>

### closure \<bool\> \<dir\> \<code\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>closure</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;bool&gt; &lt;dir&gt; &lt;code&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Create a closure from the environment &lt;dir&gt; and code &lt;code&gt;.  If &lt;bool&gt; is TRUE the new closure includes the current environment just as </p>
<p>&lt;dir&gt;:&lt;code&gt;</p>
<p>would. Otherwise it does not include the current envionment (i.e. the environment is precisely &lt;dir&gt;) just as</p>
<p>&lt;dir&gt;::&lt;code&gt;</p>
<p>would.<br />
Any unbound variables in &lt;dir&gt; are ignored.<br />
See also: bind, code, context, argname, argnames</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A closure with no unbound variables created from &lt;dir&gt; and &lt;code&gt;.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; closure FALSE [inc=3] {&lt;0, inc .. 10&gt;}</p>
<p>[inc=3]::{&lt;0, inc .. 10&gt;}</p>
<p>&gt; set key_chooser[vals]:{(closure FALSE vals {key}!)!}</p>
<p>&gt; key_chooser ["lock"=8,"key"=-9,"bolt"=2]</p>
<p>-9</p>
<p>&gt; key_chooser ["lock"=8,"bolt"=2]</p>
<p>ftl $*console*@26@0: in</p>
<p>ftl $*console*@26: in</p>
<p>ftl $*console*:28: undefined symbol 'key'</p>
<p>ftl $*console*:28: error in closure code body</p>
<p>&gt; set init_42 closure TRUE [first=42] {[value=first]}!</p></td>
</tr>
</tbody>
</table>

### cmd \<function\> \<help\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>cmd</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;function:closure&gt; &lt;help:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Create a command from a function that parses the command line as a string.  The object created is suitable for entry into a directory of commands and will incorporate a help string as specified.  The function provided must be a closure that has precisely one unbound variable which will be bound to a string when it is executed.<br />
This can be used to support command lines which limit the side-effects that arguments can have (since using a function as a command always allow an arbitrary expression as an argument).  Alternatively it can be used to support command line syntax which could not be provided simply by parsing a fixed number of expression - such as when names from a given environment are required or when a variable number of arguments are needed.<br />
See also: parse.scan* functions</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A closure embedding the help string and a new command which evaluates the given function. </td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set onetwo_fn[s]:{ (split " " s!).&lt;0,1&gt; }</p>
<p>&gt; set onetwo cmd onetwo_fn "&lt;first&gt; &lt;second&gt; .. return 1st and 2nd items"!</p>
<p>&gt; help</p>
<p>...</p>
<p>onetwo &lt;first&gt; &lt;second&gt; .. return 1st and 2nd items</p>
<p>...</p>
<p>&gt; onetwo beaky mick titch</p>
<p>&lt;"beaky", "mick"&gt;</p>
<p>&gt; eval onetwo "-o file -- opts"!</p>
<p>&lt;"-o", "file"&gt;</p></td>
</tr>
</tbody>
</table>

### cmp \<any\> \<any\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>cmp</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;arg1:any&gt; &lt;arg2:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Compares two values returning an integer as follows:</p>
<p>This function does not compare the content of directories or closures.  Equality in this case is based on identity (that is a directory or closure is equal only to itself, not another of the same type with identical value).<br />
See also: equal, notequal, more, less, moreeq, lesseq, collate </p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer value comparing its arguments.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; cmp 9 1</p>
<p>1</p>
<p>&gt; cmp "Astring" "Bstring"</p>
<p>-1</p>
<p>&gt; cmp "string\0_B" "string\0_A"</p>
<p>1</p>
<p>&gt; cmp &lt;1&gt; &lt;1&gt;</p>
<p>-1</p>
<p>&gt; set dir &lt;1&gt;</p>
<p>&gt; cmp dir dir</p>
<p>0</p>
<p>&gt; cmp NULL ({}!)</p>
<p>0</p></td>
</tr>
</tbody>
</table>

### code \<closure\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>code</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;closure&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Return only the 'code' portion of a closure (e.g. a function).<br />
There are three main portions of a closure: its bound environment, its unbound environment and its code.  This function returns the latter.<br />
See also: bind, closure, context, argname, argnames</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A code value.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; code [b=1,a]:{a+b}</p>
<p>{a+b}</p>
<p>&gt; code set</p>
<p>&lt;cmd:0x8063679,1&gt;</p>
<p>&gt; code code</p>
<p>&lt;func:0x805ac4e,1&gt;</p></td>
</tr>
</tbody>
</table>

### collate \<string1\> \<string2\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>collate</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;chars1:string&gt; &lt;chars2:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns comparison of the lexicographic positions of the two strings derived from the collating sequence of the characters contained in the two strings.  The characters may be encoded in more than one octet.<br />
Note that this is a different function to cmp which will compare the octets that constitute each character (which will normally result in a comparison of the ordering of the character's positions in their character set instead of their collating position - which is not always equivalent).<br />
The collating sequence used is provided by the locale in which the interpreter runs.<br />
See also: cmp</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer comparing the lexicographic collating sequences of the two strings.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; collate "same" "same"</p>
<p>0</p>
<p>&gt; collate "early" "late"</p>
<p>-1</p>
<p>&gt; sortby collate &lt;"fred", "£", "FORMER", "104"&gt;</p>
<p>&lt;3, 2, 0, 1&gt;</p></td>
</tr>
</tbody>
</table>

### **command** \<**whole line**\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>command</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;restofline&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Execute the text on the rest of the line as if it was a source command.</p>
<p>Normally execution continues until the line is completely read or until a return or exit command are executed. </p>
<p>As with other commands, when used as a function multiple lines of source can be provided either as a string or as a code value and the text provided is subject to macro expansion in the current environment. This means that, unlike sourcetext, macros in the text are expanded before the commands are executed.</p>
<p>See also: parse.exec, source, sourcetext, return, exit </p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Any value provided by a return command.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set doit "echo"</p>
<p>&gt; command $doit some arguments for $doit</p>
<p>some arguments for echo</p>
<p>&gt; # some arguments for echo</p>
<p>&gt; command return 2+2</p>
<p>4</p>
<p>&gt; #4</p>
<p>&gt;</p>
<p>&gt; set PROFORMA[cond,then]:{cond {command then!}!}</p>
<p>&gt; set WHEN[cond,then]:{cond {sourcetext then!}!}</p>
<p>&gt; set scope 1</p>
<p>&gt; WHEN TRUE {</p>
<p>&gt;    set scope 2</p>
<p>&gt;    echo scope is $scope</p>
<p>&gt; }</p>
<p>scope is 2</p>
<p>&gt; eval scope</p>
<p>2</p>
<p>&gt; set scope 1</p>
<p>&gt; PROFORMA TRUE {</p>
<p>&gt;    set scope 2</p>
<p>&gt;    echo scope is $scope</p>
<p>&gt; }</p>
<p>scope is 1</p></td>
</tr>
</tbody>
</table>

### context \<closure\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>context</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;closure&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Return only the 'code' portion of a closure (e.g. a function).<br />
There are three main portions of a closure: its bound environment, its unbound environment and its code.  This function returns the latter.<br />
See also: bind, code, closure, argname, argnames</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A directory representing the environment in which &lt;closure&gt; will run.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; context [b=1,a]::{a+b}</p>
<p>[b=1, a]</p>
<p>&gt; context set</p>
<p>[_help="&lt;name&gt; &lt;expr&gt; - set value in environment", _1]</p>
<p>&gt; eval (context [b=1,a]:{a+b}!).b</p>
<p>1</p>
<p>&gt; eval (context [b=1,a]:{a+b}!).a</p>
<p>ftl $*console*:12: index symbol undefined in parent '"a"'</p>
<p>ftl $*console*:12: failed to evaluate expression</p></td>
</tr>
</tbody>
</table>

### **cos** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>cos</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number of radians and returns its trigonometric cosine.<br />
The inverse function to acos.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value cos(x).<br />
See also: sin, tan, asin, acos, atan, cosh</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (cos 0.0!)</p>
<p>1.000000000000</p>
<p>&gt; preal (cos PI/3!)**2</p>
<p>0.250000000000</p>
<p>&gt; preal (cos (-PI/3)!)**2</p>
<p>0.250000000000</p>
<p>&gt; preal (cos PI/4!)**2</p>
<p>0.500000000000</p>
<p>&gt; preal (cos PI/2!)**2</p>
<p>0.000000000000</p>
<p>&gt; preal (cos (acos 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### **cosh** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>cosh</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns its hyperbolic cosine (½(e<sup>x</sup>-e<sup>-x</sup>)).<br />
The inverse function to acosh.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value cosh(x).<br />
See also: sinh, tanh, asinh, acosh, atanh, cos</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (cosh (0.0)!)</p>
<p>1.000000000000</p>
<p>&gt; preal (cosh (1.0)!)</p>
<p>1.543080634815</p>
<p>&gt; preal (cosh (-1.0)!)</p>
<p>1.543080634815</p>
<p>&gt; preal (cosh (acosh 0.123!)!)</p>
<p>-nan</p>
<p>&gt; preal (cosh (acosh 1.23!)!)</p>
<p>1.230000000000</p></td>
</tr>
</tbody>
</table>

### div \<**number**\> \<**number**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>div</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n1:number&gt; &lt;n2:number&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two integer or reals and divides the first by the second returning the result.<br />
If either n1 or n2 are real numbers the result will be a real.<br />
If both n1 and n2 are integers the result is the integer part of the division (given as an integer).</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer or real with the value n1/n2.<br />
See also: add, sub, mul, pow, abs, neg, shiftr, shiftl</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; div 14 6</p>
<p>2</p>
<p>&gt; div -14 6</p>
<p>-2</p>
<p>&gt; div 14 (-6)</p>
<p>-2</p>
<p>&gt; div -14 (-6)</p>
<p>2</p>
<p>&gt; eval div 4 3!</p>
<p>1</p>
<p>&gt; eval div 4 3.0!</p>
<p>1.333333333333333</p>
<p>&gt;</p></td>
</tr>
</tbody>
</table>

### do \<code\> \<code\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>do</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;do:code&gt; &lt;test:code&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>execute &lt;do&gt; while &lt;test&gt; evaluates to TRUE</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Returns the value associated with &lt;code&gt; on its last execution</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>set nosix_run[]: {</p>
<p>    count = 0;</p>
<p>    do {n = rnd 6!; </p>
<p>        count = 1+(count);</p>
<p>        count </p>
<p>    } { 0 != (n) }!</p>
<p>}</p>
<p>&gt; nosix_run</p>
<p>5</p>
<p>&gt; nosix_run</p>
<p>10</p>
<p>&gt; nosix_run</p>
<p>1</p></td>
</tr>
</tbody>
</table>

### domain \<dir\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>domain</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;env:dir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Provides a means to find just the in-scope names that are bound in env.  Names that are hidden because of scoping rules will not appear nor will unbound names.<br />
The order of elements in the vector is undefined, but is the same ordering that is used in the range function.<br />
See also: range</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>a vector of names used in env, in any order</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; domain [a=3, b="asdf", "c"=[], d]</p>
<p>&lt;"a", "b", "c"&gt;</p>
<p>&gt; join ", " (domain [a=3, b="asdf", "c"=[], d]!)</p>
<p>"a, b, c"</p>
<p>&gt; set rndelement[env]: { env.((domain env!).(rnd (len env!)!)) }</p>
<p>&gt; rndelement [a=3, b="asdf", "c"=[], d]</p>
<p>"asdf"</p>
<p>&gt; rndelement [a=3, b="asdf", "c"=[], d]</p>
<p>3</p>
<p>&gt; rndelement [a=3, b="asdf", "c"=[], d]</p>
<p>[]</p></td>
</tr>
</tbody>
</table>

### equal \<val\> \<val\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>equal</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;val1:any&gt; &lt;val2:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Compares its first argument with the second and returns TRUE iff they are equal and FALSE otherwise. The comparison is made as described in the cmp function.<br />
This function is identical to:</p>
<p>[val1, val2]: { 0 == (cmp val1 val2!) }</p>
<p>See also: cmp, notequal, more, less, moreeq, lesseq<br />
Normally this function is associated with the == operator.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE or FALSE</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>{  if (equal var NULL!) {</p>
<p>        echo "nothing there"!</p>
<p>    }{ do_something_with var!</p>
<p>    }!</p>
<p>}</p>
<p>&gt; eval (3 == 3) {echo "hi"!}!</p>
<p>hi</p>
<p>FALSE</p></td>
</tr>
</tbody>
</table>

### echo \<whole line\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>echo</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;restofline&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Writes the characters following it starting at the first non-blank character to the current output stream followed by a newline.<br />
As with other commands the line is subject to $-expansion first.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; echo  === STARTING ===</p>
<p>=== STARTING ===</p>
<p>&gt; set option "on"</p>
<p>&gt; echo Option is currently $option</p>
<p>Option is currently on</p>
<p>&gt; set follow[a]: { [next = add a 1!]:{ echo "Next $next"! }! }</p>
<p>&gt; follow 4</p>
<p>Next 5</p>
<p>&gt; set copy[1,2]:{ sys.run "echo cp '$1' '$2'"! }</p>
<p>&gt; copy "fileA" "fileB"</p>
<p>cp fileA fileB</p></td>
</tr>
</tbody>
</table>

### enter \<dir\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>enter</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;env:dir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This command pushes its argument directory env onto the current environment stack thus bringing all the names defined in the directory into scope.<br />
Since the commands parse.local and help are active only on the top directory in the current environment the command has a direct effect on them.<br />
Note that (unless left explicitly using leave or leaving) the environment entered during the execution of a block goes out of scope at the end of the block, together with any pushed directories.<br />
See also: restrict, leave, leaving</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; enter [h=help, e=echo, ans=42]</p>
<p>&gt; help</p>
<p>ans - int value</p>
<p>e &lt;whole line&gt; - prints the line out</p>
<p>h - prints command information</p>
<p>&gt; eval h</p>
<p>["_help"="- prints command information"]::&lt;func:0x8053dfc,0&gt;</p>
<p>&gt; eval help</p>
<p>["_help"="- prints command information"]::&lt;func:0x8053dfc,0&gt;</p>
<p>&gt;</p>
<p>&gt; set f [rec]:{ x="original"; enter rec!; echo x!; }</p>
<p>&gt; f [x="new"]</p>
<p>new</p>
<p>&gt; f [y="new"]</p>
<p>original</p>
<p>&gt;</p>
<p>&gt; eval ans</p>
<p>42</p>
<p>&gt; set f1[]:{ans=80;ans}</p>
<p>&gt; f1</p>
<p>80</p>
<p>&gt; eval ans</p>
<p>80</p>
<p>&gt; set f1[]:{enter [ans=9]!;ans}</p>
<p>&gt; f1</p>
<p>9</p>
<p>&gt; eval ans</p>
<p>80</p>
<p>&gt;</p>
<p>&gt; set item_bill[purchase]: {</p>
<p>    enter purchase!;</p>
<p>        total = items * unit_cost;</p>
<p>    leave!;</p>
<p>    purchase.total</p>
<p>};</p>
<p>&gt; set item1 [name="Cheese", total=NULL, items=3, unit_cost=200]</p>
<p>&gt; item_bill item1</p>
<p>600</p>
<p>&gt; print item1</p>
<p>[name="Cheese",total=600,items=3,unit_cost=200]</p></td>
</tr>
</tbody>
</table>

### eval \<expr\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>eval</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;expression:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Parses the characters following it as an FTL expression and evaluates it.<br />
As with other commands the line is subject to $-expansion first.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The value of the FTL expression</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; eval f="foibles"</p>
<p>"foibles"</p>
<p>&gt; eval [a]:{echo a!; a} "asparagus"!</p>
<p>asparagus</p>
<p>"asparagus"</p>
<p>&gt; set demo [expr]:{ </p>
<p>&gt;   io.write io.out ""+(expr)+" = "+(str (eval expr!)!)+"\n"! </p>
<p>&gt; }</p>
<p>&gt; forall &lt;3..5&gt; [n]:{demo "100/"+(str n!)!}</p>
<p>100/3 = 33</p>
<p>100/4 = 25</p>
<p>100/5 = 20</p></td>
</tr>
</tbody>
</table>

### every \<n\> \<command\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>every</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;milliseconds:int&gt; &lt;commands:restofline&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Executes the command line &lt;restofline&gt; every milliseconds forever or until the value returned by the command is first FALSE.<br />
(Note that the command line is not an FTL expression.)<br />
The function sleep is implicitly invoked by this command.<br />
See also: sleep</p>
<p>If the number of milliseconds is negative the sleep function is not invoked. (This may have a different effect to the sleep function being called with the argument zero.)</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Does not return</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; every 1000 echo tick</p>
<p>tick</p>
<p>tick</p>
<p>tick</p>
<p>tick</p>
<p>tick</p>
<p>tick</p>
<p>tick</p>
<p>tick</p>
<p>:</p>
<p>&gt; set n 5<br />
&gt; set f[]:{n=n-1; io.fprintf io.out "%d\n"&lt;n&gt;!; n _gt_ 0}<br />
&gt; every -1 f<br />
4<br />
3<br />
2<br />
1<br />
0<br />
&gt;</p></td>
</tr>
</tbody>
</table>

### exit

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>exit</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>no argument</td>
</tr>
<tr class="even">
<td>Description</td>
<td>By closing all input streams on the current input stack this function normally causes execution in the interpreter to come to an end.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Does not return</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; exit</p>
<p>&gt; set error[msg]:{ echo msg!; exit! }</p></td>
</tr>
</tbody>
</table>

### **exp** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>exp</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns Euler's number (e) raised to that power.<br />
The inverse function to log.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value e<sup>x</sup>.<br />
See also: mul, pow, sqrt, log</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; exp 0</p>
<p>1</p>
<p>&gt; exp 1</p>
<p>2.718281828459045</p>
<p>&gt; exp (log 3!)</p>
<p>3</p></td>
</tr>
</tbody>
</table>

### FALSE

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>FALSE</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Value</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>(no args)</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Provides the FALSE value which can be used in boolean expressions.<br />
Note that the value used for FALSE behaves as if it were defined as follows:</p>
<p>[code]:{FALSE}</p>
<p>See also: TRUE, the section about Boolean Values above.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A value that is different to the one used for TRUE.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set done FALSE</p>
<p>&gt; set once[fn]: { if (done) { done=TRUE; fn! } {}! } </p>
<p>{  rc = fn!;</p>
<p>    if (rc.error_occured) {echo "ERROR"!; FALSE} {TRUE}!</p>
<p>}</p>
<p>&gt; eval FALSE {echo "never run"!}!</p>
<p>FALSE</p></td>
</tr>
</tbody>
</table>

### **floor** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>floor</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real and returns a real number representing the smallest integer not less than n (the floor of the integral part of the real).</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An real with the largest integral number not greater than n.<br />
Note: the result is not an integer – rint can be used to convert it to an integer though.<br />
See also: rint, ceil, round</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; floor 10.1</p>
<p>10</p>
<p>&gt; floor 10.9</p>
<p>10</p>
<p>&gt; floor -10.1</p>
<p>-11</p>
<p>&gt; floor -10.9</p>
<p>-11</p>
<p>&gt; typeof (floor 10!)</p>
<p>$basetype.int</p>
<p>&gt; typeof (floor 10.1!)</p>
<p>$basetype.real</p></td>
</tr>
</tbody>
</table>

### for \<dir\> \<closure\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>for</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;set:clodir&gt; &lt;enumerate:clocode&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function executes enumerate for every name value pair in set.<br />
If enumerate is code it is simply executed in the environment where for was invoked once for every member in the set, otherwise enumerate should be a closure with one argument.<br />
If it is a closure with one argument, that argument is successively bound to different values in the name-value pairs in the set.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; for &lt;..3&gt; { echo "next"! }</p>
<p>next</p>
<p>next</p>
<p>next</p>
<p>&gt; for &lt;"one", "fine", "day"&gt; echo</p>
<p>one</p>
<p>fine</p>
<p>day</p></td>
</tr>
</tbody>
</table>

### forall \<dir\> \<closure\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>forall</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;set:clodir&gt; &lt;enumerate:closure&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function executes enumerate for every name value pair in set.<br />
enumerate should be a closure with two arguments. The first argument is successively bound to different values, while the second is bound to the corresponding name.<br />
The order of evaluation is defined only for vectors.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>set sieve[n]: { </p>
<p>    forall &lt;0, n .. (len prime!)&gt; [i]:{ </p>
<p>        prime.(i) = NULL </p>
<p>    }! </p>
<p>}</p>
<p>&gt; forall [a="ay", b="bee", c="see"] [pron, let]:{</p>
<p>&gt;    echo "$let is pronounced '$pron'"!</p>
<p>&gt; }</p>
<p>a is pronounced 'ay'</p>
<p>b is pronounced 'bee'</p>
<p>c is pronounced 'see'</p>
<p>&gt; forall ([a,b]::{b=20+(a)} 1 2) [val, name]:{echo "$name = $val"!}</p>
<p>b = 2</p>
<p>a = 1</p></td>
</tr>
</tbody>
</table>

### forallwhile \<dir\> \<closure\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>forallwhile</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;vals:clodir&gt; &lt;enumerate:closure&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function executes enumerate for every name value pair, in order, in vals until the value returned by enumerate is not FALSE.<br />
Enumerate must be a closure with exactly two unbound variables. The first argument is successively bound to different values, while the second is bound to the corresponding name. If the execution returns NULL (e.g. leaves no value) the enumerations will not stop.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if all the values in the sequence were used and FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set lastlessthan[seq,max]:{</p>
<p>    .ix = NULL;</p>
<p>    forallwhile seq [val,index]:{</p>
<p>        val _lt_ max {</p>
<p>            ix = index;</p>
<p>            TRUE</p>
<p>        }!</p>
<p>    }!;</p>
<p>    ix</p>
<p>}</p>
<p>&gt; lastlessthan &lt;14,88,142,156,188&gt; 150</p>
<p>2</p></td>
</tr>
</tbody>
</table>

### forwhile \<dir\> \<closure\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>forallwhile</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;vals:clodir&gt; &lt;enumerate:clocode&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function executes enumerate for every name value pair, in order, in vals until the value returned by enumerate is not FALSE. If the execution returns NULL (e.g. leaves no value) the enumerations will not stop.<br />
Enumerate must be a closure with exactly one unbound variables which is successively bound to each value in vals.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if all the values in the sequence were used and FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set list [cmds]:{ </p>
<p>    forwhile cmds [cmd]:{</p>
<p>        if cmd != "stop" {echo cmd!}{FALSE}!</p>
<p>    }!</p>
<p>}</p>
<p>&gt; list &lt;&gt;</p>
<p>&gt; list &lt;"add","subtract","print"&gt;</p>
<p>add</p>
<p>subtract</p>
<p>print</p>
<p>TRUE</p>
<p>&gt; list &lt;"add","subtract","stop","ignore","ignore"&gt;</p>
<p>add</p>
<p>subtract</p>
<p>FALSE</p></td>
</tr>
</tbody>
</table>

### func

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>func</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;name:token&gt; &lt;value:closure&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Provide a name for the given closure in the current environment and mark it for automatic execution. If the name does not appear in the current environment it is added (to the environment with innermost-scope).  The name may specify the directly in which it is set explicitly (by using the '&lt;dir&gt;.&lt;name&gt;'  syntax).  The innermost-scope can be specified explicitly using the syntax '.&lt;name&gt;'.</p>
<p>See also: set</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set noauto[a,b]:{a+b}</p>
<p>&gt; func auto[a,b]:{a+b}</p>
<p>&gt; eval noauto 1</p>
<p>[b]:{a+b}</p>
<p>&gt; eval auto 1</p>
<p>[b]:{a+b}</p>
<p>&gt; eval noauto 1 2</p>
<p>[]:{a+b}</p>
<p>&gt; eval auto 1 2</p>
<p>3</p>
<p>&gt; eval noauto 1 2 !</p>
<p>3</p>
<p>&gt; eval auto 1 2 !</p>
<p>ftl $*console*:+39 in</p>
<p>ftl $*console*:40: a int value is not executable:</p>
<p>3</p>
<p>ftl $*console*:+39: failed to evaluate expression</p>
<p>&gt; eval &amp;auto 1 2</p>
<p>[]:{a+b}</p>
<p>&gt; eval &amp;auto 1 2 !</p>
<p>3</p></td>
</tr>
</tbody>
</table>

### help

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>help [all] [&lt;command&gt;]</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>[all] [&lt;command&gt;]</td>
</tr>
<tr class="even">
<td>Description</td>
<td>If no &lt;command&gt; is provided on the command line this will print information about the commands in the top level environment.  If &lt;command&gt; is provided help is provided for &lt;command&gt; explicitly.<br />
Because named environments appearing at the front of a command are entered the syntax "&lt;environment&gt; help" can be used as an idiom that prints help for objects in &lt;environment&gt;.<br />
For names associated with a directory this command indicates that this idiom can be used to obtain more help or, if all is given, it recursively requests help for each name in the directory.<br />
For names associated with a closure the command will print a string value associated with the name "_help".  <br />
Other types of value the command prints only the type of the value held.  These, however, are only included in the output if "all" is given on the command line.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; help</p>
<p>help [all] - prints command information</p>
<p>set &lt;name&gt; &lt;expr&gt; - set value in environment</p>
<p>exit - abandon all command inputs</p>
<p>:</p>
<p>sys help - show subcommands</p>
<p>parse help - show subcommands</p>
<p>io help - show subcommands</p>
<p>:</p>
<p>echo &lt;whole line&gt; - prints the line out</p>
<p>eval &lt;expr&gt; - evaluate expression</p>
<p>sleep &lt;n&gt; - sleep for &lt;n&gt; milliseconds</p>
<p>len [&lt;env&gt;|&lt;closure&gt;|&lt;string&gt;] - number of elements in object</p>
<p>&gt; sys help</p>
<p>run &lt;line&gt; - execute system &lt;line&gt;</p>
<p>uid &lt;user&gt; - return the UID of the named user</p>
<p>time - system calendar time in seconds</p>
<p>localtime &lt;time&gt; - broken down local time</p>
<p>utctime &lt;time&gt; - broken down UTC time</p>
<p>localtimef &lt;format&gt; &lt;time&gt; - formatted local time</p>
<p>utctimef &lt;format&gt; &lt;time&gt; - formatted UTC time</p>
<p>&gt; enter [ e=echo, h=help, x=exit, o=1, t=2 ]</p>
<p>&gt; help</p>
<p>e &lt;whole line&gt; - prints the line out</p>
<p>h [all] [&lt;cmd&gt;] - prints information about commands</p>
<p>x - abandon all command inputs</p>
<p>&gt; help all</p>
<p>e &lt;whole line&gt; - prints the line out</p>
<p>h [all] [&lt;cmd&gt;] - prints information about commands</p>
<p>x - abandon all command inputs</p>
<p>o - int value</p>
<p>t - int value</p>
<p>&gt; help exit</p>
<p>exit - abandon all command inputs</p>
<p>&gt; sys help time</p>
<p>time - system calendar time in seconds</p>
<p>&gt; help sys.time</p>
<p>sys.time - system calendar time in seconds</p>
<p>&gt; enter [fn = [_help="- my function"]:{echo _help!}, env = [x="variable"], var = "variable" ]</p>
<p>&gt; help</p>
<p>fn - my function</p>
<p>env help - show subcommands</p>
<p>&gt; help all</p>
<p>fn - my function</p>
<p>env &lt;subcommand&gt; - commands:</p>
<p>    x - string value</p>
<p>var - string value</p>
<p>&gt; env help</p>
<p>&gt; env help all</p>
<p>x - string value</p></td>
</tr>
</tbody>
</table>

### if \<bool\> \<then-code\> \<else-code\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>if</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;test:bool&gt; &lt;then:code&gt; &lt;else:code&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Compares test with FALSE and then executes then if test was not FALSE and else otherwise. </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Returns the value returned by the execution of then or else, depending on which was selected for execution.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; if 3 lt (rnd 6!) { echo "heads"! } { echo "tails"! }</p>
<p>&gt; eval echo (if 3 lt (rnd 6!) {"heads"}{"tails"}!)!</p>
<p>set doit[cmd]: {</p>
<p>    if 0 == (cmp cmd "exit"!) {</p>
<p>       FALSE</p>
<p>    }{</p>
<p>       eval cmd!;</p>
<p>       TRUE</p>
<p>    }!</p>
<p>}</p>
<p>&gt; if TRUE (echo "this") (echo "that")</p>
<p>this</p>
<p>&gt; if FALSE (echo "this") (echo "that")</p>
<p>that</p></td>
</tr>
</tbody>
</table>

### inenv \<env\> \<name\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>inenv</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;env:clodir&gt; &lt;name&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>returns 0 unless string &lt;name&gt; has a value in &lt;env&gt;</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; inenv parse.env "inenv"</p>
<p>TRUE</p>
<p>&gt; inenv [a=NULL, b] "a"</p>
<p>TRUE</p>
<p>&gt; inenv [a=NULL, b] "b"</p>
<p>FALSE</p>
<p>&gt; inenv [a=NULL, b] "c"</p>
<p>FALSE</p></td>
</tr>
</tbody>
</table>

### invert \<bool\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>invert</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;b:int&gt; </td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a booleans and returns the TRUE if the first is FALSE, or otherwise returns FALSE.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Either the value TRUE or FALSE.  When the argument is boolean the result is a boolean representing the not of b.<br />
See also: TRUE, FALSE, if, and, invert</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; invert TRUE</p>
<p>FALSE</p>
<p>&gt; invert FALSE</p>
<p>TRUE</p></td>
</tr>
</tbody>
</table>

### io binfile \<filename\> \<rw\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.binfile</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;filename:string&gt; &lt;access:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>opens a binary file from the local filing system with name filename to which the requested access is provided.  The access string consists of one or more of the following characters:</p>
<p>Note that the binding "io.binfile &lt;filename&gt;" can be used as a generic object to represent something openable as a stream.</p>
<p>On some operating systems this call differs from io.file in the handling of certain characters. All of the characters in the file are provided without translation when io.binfile is used.<br />
<br />
Once opened the resulting stream should eventually be closed using io.close.<br />
<br />
See also: io.file, io.pathfile, io.close </p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL if the file could not be opened with the requested access otherwise a stream giving access (via io.write etc.) to the file </td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set exists[f]: { 0 != (cmp NULL (io.binfile f "r"!)!) }</p>
<p>&gt; exists "/tmp/gone"</p>
<p>FALSE</p>
<p>&gt; exists "ftl"</p>
<p>TRUE</p>
<p>&gt; parse exec "" (io.binfile "script" "r"!)</p></td>
</tr>
</tbody>
</table>

### io close \<stream\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.close</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;open:stream&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Completes access to the given stream and makes it unavailable for future access </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; io close io.file "test" "w"!</p>
<p>set log[msg]:{</p>
<p>    logstr = io.file "log" "w"!;</p>
<p>    io.write logstr msg!;</p>
<p>    io.write logstr "\n"!;</p>
<p>    io.close logstr!;</p>
<p>}</p></td>
</tr>
</tbody>
</table>

### io err

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.err</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Read only value</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Description</td>
<td>The standard stream to use for error output.  (The same stream that is associated with stderr in C when the interpreter was started.)<br />
See variables io out and io in too.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; io write io.err "Wrong!\n"</p>
<p>7</p>
<p>set error[rc, msg]: {</p>
<p>    io.write io.err ""+(str rc!)+" "+(msg)+"\n"!</p>
<p>}</p></td>
</tr>
</tbody>
</table>

### io file \<filename\> \<rw\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.file</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;filename:string&gt; &lt;access:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>opens a file from the local filing system with name filename to which the requested access is provided.  The access string consists of one or more of the following characters:</p>
<p>Note that the binding "file &lt;filename&gt;" can be used as a generic object to represent something openable as a stream.</p>
<p>On some operating systems this call differs from io.binfile in the handling of certain characters. For example, on Windows, a control-Z character in the file is treated as an end-of-file character, preventing the reading of subsequent characters.</p>
<p>Once opened the resulting stream should eventually be closed using io.close.<br />
<br />
See also: io.file, io.pathfile, io.close </p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL if the file could not be opened with the requested access otherwise a stream giving access (via io.write etc.) to the file </td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set exists[f]: { 0 != (cmp NULL (io.file f "r"!)!) }</p>
<p>&gt; exists "/tmp/gone"</p>
<p>FALSE</p>
<p>&gt; exists "ftl"</p>
<p>TRUE</p>
<p>&gt; parse exec "" (io.file "script" "r"!)</p></td>
</tr>
</tbody>
</table>

### io fprintf \<stream\> \<format\> \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.fprintf</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;out:stream&gt; &lt;format:string&gt; &lt;args:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Writes the octets (bytes) of the characters generated by the formatted interpretation of the arguments in args according to the given format to the output stream provided.  The format and arguments are interpreted as specified by the strf function.<br />
The stream must have write access.<br />
See also: strf, io.write</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The number of bytes written to the stream.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; io fprintf io.out "€1 = £%[pounds]d.%[pennies]02d\n" [pennies=60, pounds=0]</p>
<p>€1 = £0.60</p>
<p>14</p>
<p>&gt; set printf io.fprintf io.out</p>
<p>&gt; set n printf "sequence %02d %02d %02d %02d\n" &lt;5..8&gt;!</p>
<p>sequence 05 06 07 08</p>
<p>&gt; eval n</p>
<p>21</p></td>
</tr>
</tbody>
</table>

### io getc

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.getc</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;in:stream&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Reads the next octet from the argument stream as a string.  If no more characters are available from the stream (e.g. at the end of a file) then a NULL is given.<br />
The stream must be open for reading.<br />
See also: io.read </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the next octet read from the given stream.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set sin io.instring "In" "r"!</p>
<p>&gt; io getc sin</p>
<p>"I"</p>
<p>&gt; io getc sin</p>
<p>"n"</p>
<p>&gt; io getc sin</p>
<p>&gt;</p>
<p>set rev[st]:{</p>
<p>    .out="";</p>
<p>    .ch=NULL;</p>
<p>    while {NULL != (ch=io.getc st!)} {</p>
<p>        out=""+(ch)+(out);</p>
<p>    }!;</p>
<p>    out</p>
<p>}</p>
<p>&gt; set sin io.instring "able was I ere" "r"!</p>
<p>&gt; rev sin</p>
<p>"ere I saw elba"</p></td>
</tr>
</tbody>
</table>

### io in

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.in</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Read only value</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Description</td>
<td>The standard stream to use for normal input.  (The same stream that is associated with stdin in C when the interpreter was started.)<br />
See variables io.out and io.err too. </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; io in</p>
<p>&lt;-'&lt;stdin&gt;'</p>
<p>set is_stdin[str]: { 0 == (cmp str io.in!) }</p></td>
</tr>
</tbody>
</table>

### io instring \<string\> \<rw\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.instring</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;input:string&gt; &lt;access:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>creates a stream from a string for reading from.  The access string consists of one or more of the following characters:</p>
<p>However this type of string can only be opened for reading.  Nonetheless the binding "io.instring &lt;string&gt;" can be used as a generic object to represent something openable as a stream. </p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL if the string could not be opened with the requested access otherwise a stream giving access (via parse.exec etc.) to the file </td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; parse exec "" (io.instring "echo hi\necho there" "r"!)</p>
<p>hi</p>
<p>there</p>
<p>&gt; set f io.instring "text to read\n" "r"!</p>
<p>&gt; io read f 4</p>
<p>"text"</p>
<p>&gt; io close f</p></td>
</tr>
</tbody>
</table>

### io out

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.out</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Value</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Description</td>
<td>The standard stream to use for normal output.  (The same stream that is associated with stdout in C when the interpreter was started.)<br />
See also: variables io.err and io.in </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; io out</p>
<p>&lt;-EOF-&gt;</p>
<p>set myecho[msg]: {</p>
<p>    io.write io.out msg!;</p>
<p>    io.write io.out "\n"!;</p>
<p>}</p></td>
</tr>
</tbody>
</table>

### io outstring \<closure\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.outstring</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;code:closure&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Executes the given code with a stream argument that when written to creates a string.  The string created is returned as a result of the function.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The string generated by writes in the closure</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set s io.outstring [out]:{ for &lt;..5&gt; {io.write out "line\n"!}! }!</p>
<p>&gt; echo $s</p>
<p>line</p>
<p>line</p>
<p>line</p>
<p>line</p>
<p>line</p>
<p>&gt; set mystrf[fmt,args]:{io.outstring [out]:{io.fprintf out fmt args!}!}</p>
<p>&gt; mystrf "hi - %s" &lt;"you"&gt;</p>
<p>"hi - you"</p></td>
</tr>
</tbody>
</table>

### io pathfile \<path\> \<filename\> \<rw\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.pathfile</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;path:string&gt; &lt;filename:string&gt; &lt;access:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>opens a file from the local filing system using the given directory path with name filename and to which the requested access is provided.  The directory path &lt;path&gt; consists of a series of directory names separated by the path separator character, which is ";" on Windows and ":" on other platforms. The access string consists of one or more of the following characters:</p>
<p>A file name is created from the concatenation of each directory on the &lt;path&gt;, the operating system's file separator character (sys.fs.sep), and the &lt;filename&gt;.  An attempt to open each file name in turn is made until either an attempt is successful or no further directories are found.<br />
<br />
Note that the binding "pathfile &lt;path&gt; &lt;filename&gt;" can be used as a generic object to represent something openable as a stream.<br />
<br />
Once opened the resulting stream should eventually be closed using io.close.<br />
<br />
See also: sys.fs, io.file, io.close </p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL if the file could not be opened with the requested access in one of the directories given in the path otherwise a stream giving access (via io.write etc.) to a file.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>set include [name]:{</p>
<p>    .rdf = NULL;</p>
<p>    if (inenv sys.env "MYPATH"!) {</p>
<p>        rdf = io.pathfile sys.env.MYPATH name "r"!;</p>
<p>    }{ rdf = io.file name "r"!;</p>
<p>    }!;</p>
<p>    if (equal NULL rdf!) { echo "can't read file "+(str name!)!; } {</p>
<p>        .ret = parse.exec "" rdf!;</p>
<p>        io.close rdf!;</p>
<p>        ret</p>
<p>    }!</p>
<p>}</p>
<p>&gt; include "hello"</p>
<p>hello world</p>
<p>&gt; set sys.env.MYPATH "."</p>
<p>&gt; include "hello"</p>
<p>hello world</p>
<p>&gt; set sys.env.MYPATH ".."</p>
<p>&gt; include "hello"</p>
<p>can't read file "hello"</p></td>
</tr>
</tbody>
</table>

### io read

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.read</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;in:stream&gt; &lt;octets:integer&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Reads up to the given number of octets from the argument stream as a string.  If no more characters are available from the stream (e.g. at the end of a file) then a string of length 0 is given.  If there are characters available then at least one will be returned.<br />
The stream must be open for reading.<br />
See also: io.read </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing at least one character read from the given stream and up to the given number, unless there are no more characters available, in which case an empy string is returned.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set f io.file "/tmp/1" "r"!</p>
<p>&gt; io read f 50</p>
<p>"    Logs from Test inherited_option\n\n\nTest: inher"</p></td>
</tr>
</tbody>
</table>

### io stringify \<stream\> \<expr\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.stringify</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;out:stream&gt; &lt;expr:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Writes the expression expr to the output stream in a syntax (where possible) that would be parsed to re-create the expression value.<br />
This function is equivalent to</p>
<p>    [stream, val]:{io.write stream (str val!)!} </p>
<p>The stream must have write access.<br />
See also: io.fprintf, io.write, str</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The number of characters written to the output stream</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set n io.stringify io.out "that's \"£££\"s\n"!</p>
<p>"that\'s \"\xc2\xa3\xc2\xa3\xc2\xa3\"s\n"&gt;</p>
<p>&gt; eval n</p>
<p>41</p>
<p>&gt; io stringify io.out [a=3, b="this", c=&lt;"one", 2, 3&gt;]</p>
<p>["a"=3,"b"="this","c"=&lt;"one", 2, 3&gt;]36</p></td>
</tr>
</tbody>
</table>

### io write \<stream\> \<string\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>io.write</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;out:stream&gt; &lt;text:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Writes the octets (bytes) of the characters in text to the output stream provided.<br />
The stream must have write access.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The number of bytes written to the stream.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt;io write io.out "€1 = £0.60\n"</p>
<p>€1 = £0.60</p>
<p>14</p>
<p>&gt; set n io.write io.out "that's \"£££\"s\n"!</p>
<p>that's "£££"s</p>
<p>&gt; eval n</p>
<p>17</p></td>
</tr>
</tbody>
</table>

### joinchr \<delim\> \<str\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>joinchr</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;delim:strintnl&gt; &lt;vec:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Join a vector of characters and strings separated by delimiters.<br />
This function behaves the same way as join, but interprets integers in the delimiter or in  the vector as a character ordinal number from the local "wide" character set.<br />
Each character ordinal number is converted into a string of bytes representing the identified character.<br />
See also: join and split.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>returns a string concatenating the items in the vector separated by the delimiter</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; eval echo (joinchr 167 &lt;"para1", "para2", "para3"&gt;!)!</p>
<p>para1§para2§para3</p>
<p>&gt; eval echo (joinchr NULL &lt;915, 961, 945, 953&gt;!)!</p>
<p>Γραι</p>
<p>&gt; set chshow [n]:{ echo (joinchr " " &lt;n .. 32+(n)&gt;!)! }</p>
<p>&gt; chshow 913</p>
<p>Α Β Γ Δ Ε Ζ Η Θ Ι Κ Λ Μ Ν Ξ Ο Π Ρ ΢ Σ Τ Υ Φ Χ Ψ Ω Ϊ Ϋ ά έ ή ί ΰ α</p></td>
</tr>
</tbody>
</table>

### join \<delim\> \<str\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>join</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;delim:strintnl&gt; &lt;vec:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Join a vector of octets and strings separated by delimiters.<br />
If the delimiter is NULL the items in the vector are simply concatenated.<br />
Any numbers occurring in the delimiter or in the vector are converted into octets (bytes) that are  incorporated into the string.  Such numbers are truncated to the normal octet range (0 to 255).<br />
See also: joinchr and split. </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>returns a string concatenating the items in the vector separated by the delimiter</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; join "/" &lt;"dir", "subdir", "file"&gt;</p>
<p>"dir/subdir/file"</p>
<p>&gt; join 0 &lt;"line1", "line2"&gt;</p>
<p>"line1\0line2"</p>
<p>&gt; join NULL &lt;0x41, " line", 0x320, "made from", 32, "bytes"&gt;</p>
<p>"A line made from bytes"</p></td>
</tr>
</tbody>
</table>

### leave

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>leave</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>No argument</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This command pops the last argument directory pushed using either enter or restrict onto the current environment stack thus removing all the names in their directories from scope.<br />
Since the commands parse.local and help are active only on the top directory in the current environment the command has a direct effect on them.<br />
Note that (unless left explicitly using leave) the environment entered during the execution of a block goes out of scope at the end of the block, together with any pushed directories.<br />
See also: enter, restrict, leaving</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; enter [h=help, e=echo, ans=42]</p>
<p>&gt; help</p>
<p>ans - int value</p>
<p>e &lt;whole line&gt; - prints the line out</p>
<p>h - prints command information</p>
<p>&gt; eval h</p>
<p>["_help"="- prints command information"]::&lt;func:0x8053dfc,0&gt;</p>
<p>&gt; eval help</p>
<p>["_help"="- prints command information"]::&lt;func:0x8053dfc,0&gt;</p>
<p>&gt;</p>
<p>&gt; set f [rec]:{ x="original"; enter rec!; echo x!; }</p>
<p>&gt; f [x="new"]</p>
<p>new</p>
<p>&gt; f [y="new"]</p>
<p>original</p>
<p>&gt;</p>
<p>&gt; eval ans</p>
<p>42</p>
<p>&gt; set f1[]:{ans=80;ans}</p>
<p>&gt; f1</p>
<p>80</p>
<p>&gt; eval ans</p>
<p>80</p>
<p>&gt; set f1[]:{enter [ans=9]!;ans}</p>
<p>&gt; f1</p>
<p>9</p>
<p>&gt; eval ans</p>
<p>80</p>
<p>&gt;</p>
<p>&gt; set item_bill[purchase]: {</p>
<p>    enter purchase!;</p>
<p>        total = items * unit_cost;</p>
<p>    leave!;</p>
<p>    purchase.total</p>
<p>};</p>
<p>&gt; set item1 [name="Cheese", total=NULL, items=3, unit_cost=200]</p>
<p>&gt; item_bill item1</p>
<p>600</p>
<p>&gt; print item1</p>
<p>[name="Cheese",total=600,items=3,unit_cost=200]</p></td>
</tr>
</tbody>
</table>

### **leaving**

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>leaving</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>No argument</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This command pops the last argument directory pushed using either enter or restrict onto the current environment stack thus removing all the names in their directories from scope. It then returns the environment removed in that way. (i.e. it is identical to leave except it returns the environment just left).<br />
Since the commands parse.local and help are active only on the top directory in the current environment the command has a direct effect on them.<br />
Note that (unless left explicitly using leave or leaving) the environment entered during the execution of a block goes out of scope at the end of the block, together with any pushed directories.<br />
See also: enter, leave, restrict</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>&lt;env:clodir&gt;</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; enter []</p>
<p>&gt; set this 8</p>
<p>&gt; leaving</p>
<p>[this=8]</p></td>
</tr>
</tbody>
</table>

### len \[\<env\>|\<closure\>|\<string\>\]

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>len</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;obj:clodirstr&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Evaluates the number of elements in the directory, closure or string.<br />
The number of elements in a directory is the same as the number of name-value pairs in it.<br />
The number of elements in a closure is also the same as the number of name-value pairs in it - any unbound names are not included and the code value is not involved.<br />
The number of elements in a string is the number of bytes it contains.<br />
See also: strlen.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The number of elements in the argument</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; len "one\0two"</p>
<p>7</p>
<p>&gt; len [a=5,b,c]</p>
<p>1</p>
<p>&gt; len &lt;..4&gt;</p>
<p>4</p>
<p>&gt; len "£"</p>
<p>2</p>
<p>&gt; set stack &lt;&gt;</p>
<p>&gt; set push [x]: { stack.(len stack!) = x; }</p>
<p>&gt; push "this"</p>
<p>&gt; push "that"</p>
<p>&gt; eval stack</p>
<p>&lt;"this", "that"&gt;</p></td>
</tr>
</tbody>
</table>

### less \<val\> \<val\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>less</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;val1:any&gt; &lt;val2:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Compares its first argument with the second and returns TRUE iff the first is strictly less than the second and FALSE otherwise. The comparison is made as described in the cmp function.<br />
This function is identical to:</p>
<p>[val1, val2]: { 0 lt (cmp val1 val2!) }</p>
<p>See also:cmp, equal, notequal, more, moreeq, lesseq<br />
Normally this function is associated with the lt operator.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE or FALSE</td>
</tr>
<tr class="even">
<td>Example</td>
<td>set min[a,b]:{ if (less a b!){a}{b}! }</td>
</tr>
</tbody>
</table>

### lesseq \<val\> \<val\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>lesseq</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;val1:any&gt; &lt;val2:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Compares its first argument with the second and returns TRUE iff the first is less than, or equal to, the second and FALSE otherwise. The comparison is made as described in the cmp function.<br />
This function is identical to:</p>
<p>[val1, val2]: { 0 le (cmp val1 val2!) }</p>
<p>See also:cmp, equal, more, lesss, moreeq, lesseq<br />
Normally this function is associated with the le operator.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE or FALSE</td>
</tr>
<tr class="even">
<td>Example</td>
<td>set inrange[x,low,high]: { 1 and (moreeq x low!) and (lesseq x high!) }</td>
</tr>
</tbody>
</table>

### lock \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>lock</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;env:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Prevents the addition or removal of names from the directory provided.  It does not prevent the modification of values associated with existing names.  There is no associated unlock function.<br />
When a closure is locked the unbound variables can still be bound.<br />
See also: new.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set milk [mon=4, tue=2, wed=4, thu=3, fri=3, sat=6, sun=0]</p>
<p>&gt; lock milk</p>
<p>&gt; set milk.mon 5</p>
<p>&gt; eval milk.mon</p>
<p>5</p>
<p>&gt; set milk.feb 4</p>
<p>ftl $*console*:6: can't set a value for "feb" here</p>
<p>&gt; set config_empty []</p>
<p>&gt; lock config_empty</p></td>
</tr>
</tbody>
</table>

### **log** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>log</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns its natural logarithm (logarithm to the base of Euler's number (e)).<br />
The inverse function to exp.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value e<sup>x</sup>.<br />
See also: mul, pow, sqrt, exp</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; log 0</p>
<p>-inf</p>
<p>&gt; log 1</p>
<p>0</p>
<p>&gt; log E</p>
<p>1</p>
<p>&gt; log E**2</p>
<p>2</p>
<p>&gt; log (exp 3.4!)</p>
<p>3.4</p></td>
</tr>
</tbody>
</table>

### logand \<bool\> \<bool\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>logand</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;b1:bool&gt; &lt;b2:bool&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two booleans and returns the FALSE if the first is FALSE, or otherwise returns the second.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Either the value FALSE or the second argument.  When both arguments are boolean the result is a boolean representing the anded combination of b1 and b2.<br />
See also: TRUE, FALSE, if, logor, invert, bitand</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; logand TRUE TRUE</p>
<p>TRUE</p>
<p>&gt; logand TRUE FALSE</p>
<p>FALSE</p>
<p>&gt; logand FALSE TRUE</p>
<p>FALSE</p>
<p>&gt; logand FALSE FALSE</p>
<p>FALSE</p></td>
</tr>
</tbody>
</table>

### logor \<bool\> \<bool\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>logor</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;b1:bool&gt; &lt;b2:bool&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two booleans and returns the TRUE if the first is not FALSE, or otherwise returns the second.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Either the value TRUE or the second argument.  When both arguments are boolean the result is a boolean representing the ored combination of b1 and b2.<br />
See also: TRUE, FALSE, if, logand, invert, bitor</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; logor TRUE TRUE</p>
<p>TRUE</p>
<p>&gt; logor TRUE FALSE</p>
<p>TRUE</p>
<p>&gt; logor FALSE TRUE</p>
<p>TRUE</p>
<p>&gt; logor FALSE FALSE</p>
<p>TRUE</p></td>
</tr>
</tbody>
</table>

### more \<val\> \<val\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>more</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;val1:any&gt; &lt;val2:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Compares its first argument with the second and returns TRUE iff the first is strictly more than the second and FALSE otherwise. The comparison is made as described in the cmp function.<br />
This function is identical to:</p>
<p>[val1, val2]: { 0 gt (cmp val1 val2!) }</p>
<p>See also:cmp, equal, notequal, less, moreeq, lesseq<br />
Normally this function is associated with the gt operator.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE or FALSE</td>
</tr>
<tr class="even">
<td>Example</td>
<td>set max[a,b]:{ if (more a b!){a}{b}! }</td>
</tr>
</tbody>
</table>

### moreeq \<val\> \<val\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>moreeq</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;val1:any&gt; &lt;val2:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Compares its first argument with the second and returns TRUE iff the first is more than, or equal to, the second and FALSE otherwise. The comparison is made as described in the cmp function.<br />
This function is identical to:</p>
<p>[val1, val2]: { 0 ge (cmp val1 val2!) }</p>
<p>See also:cmp, equal, notequal, more, less, lesseq<br />
Normally this function is associated with the ge operator.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE or FALSE</td>
</tr>
<tr class="even">
<td>Example</td>
<td>set inrange[x,low,high]: { (moreeq x low!) and (lesseq x high!) }</td>
</tr>
</tbody>
</table>

### mul \<**number**\> \<**number**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>mul</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n1:number&gt; &lt;n2:number&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two integer or reals and multiplies them together.<br />
If either n1 or n2 are real numbers the result will be a real.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer or real with the value n1*n2.<br />
See also: add, sub, div, pow, shiftl, shiftr, abs, neg</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; mul 7 24</p>
<p>168</p>
<p>&gt; mul 12 1/15.0</p>
<p>0.8</p>
<p>&gt; mul 90 0.1</p>
<p>9</p></td>
</tr>
</tbody>
</table>

### neg \<**number**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>neg</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n:number&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes an integer or real and returns its negated value.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer or real with the value -n.<br />
See also: abs, add, sub, mul, div, pow</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; neg 10</p>
<p>-10</p>
<p>&gt; neg -10</p>
<p>10</p>
<p>&gt; neg 010.10</p>
<p>-10.1</p>
<p>&gt; neg -10.10</p>
<p>10.1</p></td>
</tr>
</tbody>
</table>

### new \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>new</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;env:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Creates a new copy of the argument made by copying each name-value pair in the argument.  Note, though, that it does not make copies of the values copied.<br />
<br />
Bug: does not copy unbound variables, but does copy code body in closure</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set base [val=1]</p>
<p>&gt; set copy1 base</p>
<p>&gt; set copy2 (new base!)</p>
<p>&gt; set base.val 2</p>
<p>&gt; eval copy1</p>
<p>[val=2]</p>
<p>&gt; eval copy2</p>
<p>[val=1]</p>
<p>&gt; </p>
<p>&gt; set class [val=0]:{ [read=[]:{val}, write=[n]:{val=n;}] }</p>
<p>&gt; set rw1 class!</p>
<p>&gt; set rw2 class!</p>
<p>&gt; eval rw1.write 42!</p>
<p>&gt; eval rw2.read!</p>
<p>42</p>
<p>&gt; # oops!</p>
<p>&gt;</p>
<p>&gt; set class [val=0]:{ [read=[]:{val}, write=[n]:{val=n;}] }</p>
<p>&gt; set rw1 (new class!)!</p>
<p>&gt; set rw2 (new class!)!</p>
<p>&gt; eval rw1.write 42!</p>
<p>&gt; eval rw2.read!</p>
<p>0</p>
<p>&gt; eval rw1.read!</p>
<p>42</p></td>
</tr>
</tbody>
</table>

### notequal \<val\> \<val\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>notequal</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;val1:any&gt; &lt;val2:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Compares its first argument with the second and returns TRUE iff they are not equal and FALSE otherwise. The comparison is made as described in the cmp function.<br />
This function is identical to </p>
<p>[val1, val2]: { 0 != (cmp val1 val2!) }</p>
<p>See also:cmp, equal, more, less, moreeq, lesseq<br />
Normally this function is associated with the != operator.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE or FALSE</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>{  if (notequal var NULL!) { </p>
<p>        do_something_with var!</p>
<p>    }{}!</p>
<p>}</p></td>
</tr>
</tbody>
</table>

### NULL

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Value</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>(no args)</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Provides access to the only value in the "nul" type.  This can be used to provide a value that is guaranteed not to be equal to values of other types.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>string of (multi-octet) characters with ordinal widechar</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set newobj [name1=NULL, name2=NULL]</p>
<p>&gt; set lastval NULL</p>
<p>&gt; set val[n]: { lastval=n }</p>
<p>&gt; cmp NULL ({}!)</p>
<p>0</p>
<p>{  rc = fn!;</p>
<p>    if (rc.error_occured) {NULL} {rc.answer}!</p>
<p>}</p></td>
</tr>
</tbody>
</table>

### octet \<int\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>octet</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;byte:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns single byte string containing the given octet<br />
See also: chr</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string of len 1 whose only octet has the ordinal number provided.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; echo Wow${octet 46!}</p>
<p>Wow.</p>
<p>&gt; octet 0</p>
<p>"\0"</p></td>
</tr>
</tbody>
</table>

### octetcode \<char\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>octetcode</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;char:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns the ordinal position of the first byte (octet) in its argument in the current locale's character set.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer containing the ordinal number.<br />
See also: octet, chrcode</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; eval "£"</p>
<p>"\xc2\xa3"</p>
<p>&gt; octetcode "\xc2\xa3"</p>
<p>164</p>
<p>&gt; set dump[a]:{</p>
<p>&gt;    forall (split NULL a!) [x]:{</p>
<p>&gt;        io.write io.out " "+(strf "%02x" (octetcode x!)!)!</p>
<p>&gt;    }!; </p>
<p>&gt;    io.write io.out "\n"!;</p>
<p>&gt; }</p>
<p>&gt; dump "hexadecimal"</p>
<p> 68 65 78 61 64 65 63 69 6d 61 6c</p></td>
</tr>
</tbody>
</table>

### parse argv

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.argv</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Directory</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Description</td>
<td>A read-only vector of command line argument strings.  The value with name 0 is the command, as invoked.  (Compare with parse.codeid)  </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>% ftl -- one two</p>
<p>:</p>
<p>&gt; eval parse.argv</p>
<p>[0="ftl", 1="one", 2="two"]    </p>
<p>/usr/bin/ftl -- --myopt val</p>
<p>:</p>
<p>&gt; eval parse.codeid!</p>
<p>"ftl"</p>
<p>&gt; eval parse.argv.0</p>
<p>"/usr/bin/ftl"</p></td>
</tr>
</tbody>
</table>

### parse codeid

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.codeid</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>None</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns an identification assigned to the interpreter by the invoker.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the name of the interpreter assigned by the library-using application</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; parse codeid</p>
<p>"ftl"</p>
<p>&gt; set err[msg]:{</p>
<p>&gt;    io.write io.err ""+(parse.codeid!)+": "+(msg)+"\n"!;</p>
<p>&gt; }</p>
<p>&gt; err "something wrong"</p>
<p>ftl: something wrong</p></td>
</tr>
</tbody>
</table>

### parse env

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.env</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>None</td>
</tr>
<tr class="even">
<td>Description</td>
<td>When run this function returns a directory containing all the names and values in scope at the point of execution.  <br />
See also: restrict, parse.root, parse.local </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The current invocation environment.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; join " " (sortdom (parse.env!)!)</p>
<p>"FALSE NULL TRUE chr chrcode cmd cmp collate do domain echo enter equal eval every exit fmt forall help if inenv int io ip join</p>
<p>joinchr len less lesseq lock mac more moreeq new notequal octet </p>
<p>octetcode parse range restrict return rnd set sleep sort sortby </p>
<p>sortdom sortdomby source sourcetext split str strf strlen sys</p>
<p>tolower toupp</p>
<p>er type typename typeof while"</p>
<p>&gt; set extra [val1=1, val2=2]:{ domain (parse.local!)! }</p>
<p>&gt; extra</p>
<p>&lt;"val2", "val1", "extra", "push", "stack", "len", "sleep", "eval", </p>
<p>"echo", "cmd", "every", "sortdomby", "sortby", "sortdom", "sort", </p>
<p>"range", "domain", "restrict", ...</p></td>
</tr>
</tbody>
</table>

### parse errors

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.errors</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>(no argument)</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Provides an indication of the number of errors so far encountered.  If this number is greater than zero most interpreters will pass back a non-zero return code to their execution environment.<br />
Also see parse.errors_reset and parse.newerror.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>total number of errors encountered by parser</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; parse errors</p>
<p>5</p>
<p>if 0 == (parse.errors!) { } { </p>
<p>    echo "Parse failed with "+(str (parse.errors!)!)+"errors"! </p>
<p>}!</p></td>
</tr>
</tbody>
</table>

### parse errors\_reset \<errors\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.errors_reset</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;errors:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Resets the number of errors so far encountered to errors.  If this number is greater than zero most interpreters will pass back a non-zero return code to their execution environment.<br />
Also see parse.errors and parse.newerror.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>total number of errors encountered by parser</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; parse errors_reset</p>
<p>&gt; parse errors</p>
<p>0</p>
<p>{  saved = parse.errors!;</p>
<p>    dodgy_function!;</p>
<p>    parse.errors_reset saved!;</p>
<p>}!</p></td>
</tr>
</tbody>
</table>

### parse exec \<cmds\> \<stream\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.exec</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;cmds:string&gt; &lt;in:stream&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Executes the commands in the given string and then reads further commands from the given stream and executes them, returning once the commands have been read.  The value generated by the last command is then returned.<br />
If the intitial commands string is NULL it is ignored.<br />
If the stream is NULL (e.g. unopened) it is ignored.<br />
See also: source</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The value returned by the last command executed.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set script_in io.instring "e restricted; e script; x" "r"!</p>
<p>&gt; parse exec "restrict [e=echo, x=exit]" script_in</p>
<p>restricted</p>
<p>script</p>
<p>&gt; sys run cat /tmp/1</p>
<p>"Temporary script"</p>
<p>&gt; set run[script]:{ parse.exec "" (io.file script "r"!)! }</p>
<p>&gt; set result run "/tmp/1"!</p>
<p>"Temporary script"</p>
<p>&gt; eval result</p>
<p>"Temporary script"</p>
<p>&gt; parse exec "echo command line" NULL</p>
<p>command line</p></td>
</tr>
</tbody>
</table>

### parse local

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.local</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>None</td>
</tr>
<tr class="even">
<td>Description</td>
<td>When run this function returns a directory containing all the names and values in scope locally. (That is in the most recently established environment forming part of the execution environment).  <br />
See also: parse.root, parse.env </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The local invocation environment (in which local variables are created).</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set extra [domain=domain, parse=parse]::{ domain (parse.local!)! }</p>
<p>&gt; extra</p>
<p>&lt;"parse", "domain"&gt;</p>
<p>&gt; set extra [val1=1, val2=2]:{ domain (parse.env!)! }</p>
<p>&gt; extra</p>
<p>&lt;"val1", "val2", "help", "set", "exit", "forall", "if", "while", </p>
<p>"do", "sys", "parse", "io", "source", "sourcetext", "return", </p>
<p>"typeof", "typename", "cmp", ...</p></td>
</tr>
</tbody>
</table>

### parse line

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.line</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>None</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns the line number in the current source of characters being interpreted.  The character source used is the one that was selected at the first character of the last command executed.<br />
See also: parse.source</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The current line number.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; parse line</p>
<p>27</p>
<p>&gt; set thisline[]:{ echo "line "+(str (parse.line!)!)! }</p>
<p>&gt; parse exec "echo init; thisline" (io.instring "thisline" "r"!)</p>
<p>init</p>
<p>line 1</p>
<p>line 2</p></td>
</tr>
</tbody>
</table>

### parse newerror

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.newerror</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>(no argument)</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Adds one to the number of errors registered encountered by the parser.<br />
Also see parse.errors and parse.errors_reset</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set myerror[msg] {</p>
<p>    echo msg!;</p>
<p>    parse.newerror!; </p>
<p>}</p>
<p>&gt; parse errors</p>
<p>17</p>
<p>&gt; parse newerror</p>
<p>&gt; parse errors</p>
<p>18</p></td>
</tr>
</tbody>
</table>

### parse assoc

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.assoc</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Directory</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>This directory names the different kinds of operator associativity that are meaningful in operator definitions.  The values are integers that can be used in operator definitions.  The names are strings of up to three characters constructed: </p>
<p>&lt;left-hand associativity&gt;f&lt;right-hand associativity&gt; </p>
<p>in which a prefix operator has no left hand associativity, a postfix operator has no right hand associativity but an infix operator has both.  Where it exists the associativity is specified either as "x" or "y".  An "x" indicates that no association is possible: any argument on the relevant side must have higher precidence.  A "y" indicates that other operators at the same precidence are possible on that side.  In consequence a series of "xfy" operators will associate to the left:</p>
<p>a xfy b xfy c = (a xfy b) xfy c</p>
<p>and a series of "yfx" operators will associate to the right:</p>
<p>a yfx b yfx b = a yfx (b yfx b)</p>
<p>Also see parse.op, parse.opset and parse.scanop</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; domain parse.assoc</p>
<p>&lt;"yfy", "yfx", "xfy", "xfx", "xf", "yf", "fx", "fy"&gt;</p>
<p>&gt; parse opset parse.op 3 parse.assoc.xfx "eq" equal</p>
<p>&gt; eval 9 eq 9</p>
<p>TRUE</p>
<p>&gt; # xfx is non-associative </p>
<p>&gt; eval 9 eq 7 eq 5</p>
<p>ftl $*console*:116: undefined symbol 'eq'</p>
<p>ftl $*console*:116: trailing text in expression: ...5</p>
<p>ftl $*console*:116: warning - trailing text '5'</p>
<p>&gt; set assocstr[as]:{ (domain (select (equal as) parse.assoc!)!).0 }</p>
<p>&gt; assocstr 2</p>
<p>"fy"</p>
<p>&gt; assocstr parse.assoc.xfy</p>
<p>"xfy"</p></td>
</tr>
</tbody>
</table>

### parse op

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.op</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Directory</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This directory is the definition of a set of operators.  This specific directory is one used by the default parser, and so opertor definitions that appear in it will be part of the language.<br />
<br />
Such definitions are numerically indexed from zero contiguously.  The index determines the relative precidence between operators.  Each entry is itself a directory of name-value pairs in which the names are operator names and the values are directories with values for the names "assoc" and "fn".  The associativity and arity of the operator is determined by the value given to "assoc" and may take one of the values held in parse.assoc (with meaning described there).  The function that is executed to represent the operator is given to "fn".  This will be a closure with two unbound variables if the operator is diadic, and with one unbound variable if it is monadic.<br />
<br />
Although the function parse.opset can be used to create new entries in this directory, the data can also be inspected, rewritten or altered by the user in any other way.<br />
<br />
Also see parse.assoc, parse.opset and parse.scanop</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set forops[opdefs, fn]:{</p>
<p>&gt;    forall parse.op [precfns, prec]:{</p>
<p>&gt;        forall precfns [precfn, name]:{</p>
<p>&gt;            fn name prec precfn.assoc precfn.fn!</p>
<p>&gt;        }!</p>
<p>&gt;    }!</p>
<p>&gt; }</p>
<p>&gt; set assocstr[as]:{ (domain (select (equal as) parse.assoc!)!).0 }</p>
<p>&gt; set printf io.fprintf io.out</p>
<p>&gt; forops parse.op [name, prec, assoc, fn]:{</p>
<p>&gt;    printf "%3d: %3s %v\n" &lt;prec, assocstr assoc!, name&gt;!;</p>
<p>&gt; }</p>
<p>  0: xfy "or"</p>
<p>  1: xfy "and"</p>
<p>  2: fy "not"</p>
<p>  3: xfx "=="</p>
<p>  3: xfx "!="</p>
<p>  3: xfx "lt"</p>
<p>  3: xfx "le"</p>
<p>  3: xfx "gt"</p>
<p>  3: xfx "ge"</p>
<p>  4: xfy "shl"</p>
<p>  4: xfy "shr"</p>
<p>  5: fy "-"</p>
<p>  6: xfy "+"</p>
<p>  6: xfy "-"</p>
<p>  7: xfy "*"</p>
<p>  7: xfy "/"</p>
<p>&gt; # ignore operators from now on:</p>
<p>&gt; set parse.op NULL</p>
<p>&gt; eval parse.op.4.shr</p>
<p>["fn"=["_help"="&lt;n1&gt; &lt;n2&gt; - return n1 right shifted by n2 bits", "_1", "_2"]::</p>
<p>&lt;func:0x805c5d9,2&gt;, "assoc"=7]</p>
<p>&gt; eval parse.op.4.shr.fn 16 2!</p>
<p>4</p>
<p>&gt; eval 16 shr 2</p>
<p>4</p></td>
</tr>
</tbody>
</table>

### parse opset \<opdefs\> \<prec\> \<assoc\> \<name\> \<function\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.opset</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;opdefs:dir&gt; &lt;prec:int&gt; &lt;assoc:int&gt; &lt;name:string&gt; &lt;function:closure&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function modifies the operator definition opdefs so that it includes or replaces an entry that defines a new operator with the given name as having the precidence indicated by prec, the associativity indicated by assoc, and which will be implemented by the given function.  <br />
The associativity must be one of the integers held by parse.assoc.<br />
The directory created will be have entries for separate precidences as indicated by the prec argument.  Note, however, that when the definitions come to be used (e.g. in parse.opeval) a continuous range of precidences from zero upward is expected.  For this reason it may be necessary to apply the definitions to the range function before they are used.<br />
This function can be used to prepare a new set of operation definitions for parse.op.<br />
Also see parse.assoc, parse.op and parse.scanop </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set ops &lt;&gt;          # op definitions with explicit priorities</p>
<p>&gt;</p>
<p>&gt; parse opset ops 6 parse.assoc."fy" "_ng_" neg</p>
<p>&gt; parse opset ops 8 parse.assoc."xfy" "_sb_" sub</p>
<p>&gt; parse opset ops 8 parse.assoc."yfx" "_rsb_" [x,y]:{y-x} # reverse subtract</p>
<p>&gt; parse opset ops 10 parse.assoc."fy" "_mi_" neg</p>
<p>&gt;</p>
<p>&gt; set opdef range ops! # op definitions with relative priorities</p>
<p>&gt;</p>
<p>&gt; parse opeval opdef {20 _sb_ 4}</p>
<p>16</p>
<p>&gt; parse opeval opdef {15 _rsb_ 30}</p>
<p>15</p>
<p>&gt; parse opeval opdef {20 _rsb_ 15 _rsb_ 30} # 20 _rsb_ (15 _rsb_ 30)</p>
<p>-5</p>
<p>&gt; parse opeval opdef {20 _sb_ 4 _sb_ 6}    # (20 _sb_ 4) _sb_ 6</p>
<p>10</p>
<p>&gt; parse opeval opdef {_ng_ 20 _sb_ 4}      # _ng_ (20 _sb_ 4)</p>
<p>-16</p>
<p>&gt; parse opeval opdef {_mi_ 20 _sb_ 4}      # (_mi_ 20) _sb_ 4</p>
<p>-24</p></td>
</tr>
</tbody>
</table>

### parse opeval \<opdefs\> \<code\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.opeval &lt;opdefs&gt; &lt;code&gt;</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;opdefs:dir&gt; &lt;code:code&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function uses the operator definitions held in opdefs to evaluate the expression held in the given code value.<br />
Also see parse.assoc, parse.op, parse.opset and parse.scanop </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The result of evaluating the expression.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set ops &lt;&gt;     # op definitions with explicit priorities</p>
<p>&gt; set opdef NULL # op definitions with relative priorities</p>
<p>&gt;</p>
<p>&gt; set opset[pri, as, name, fn]:{</p>
<p>&gt;    parse.opset ops pri parse.assoc.(as) name fn!;</p>
<p>&gt;    opdef = range ops!;</p>
<p>&gt; }</p>
<p>&gt;</p>
<p>&gt; # unquoted value format</p>
<p>&gt; set fmt.t [f,p,v]:{ if (equal "string" (typename v!)!) {v} {str v!}!}</p>
<p>&gt;</p>
<p>&gt; set diadic[strf=strf,name,l,r]::{ strf "(%s %t %t)" &lt;name, l, r&gt;! }</p>
<p>&gt; set monadic[strf=strf,name,lr]::{ strf "(%s %t)" &lt;name, lr&gt;! }</p>
<p>&gt; opset 6 "fy" "fy" (monadic "fy")</p>
<p>&gt; opset 6 "fx" "fx" (monadic "fx")</p>
<p>&gt; opset 6 "yfy" "yfy" (diadic "yfy")</p>
<p>&gt; opset 6 "xfx" "xfx" (diadic "xfx")</p>
<p>&gt; opset 6 "xfy" "xfy" (diadic "xfy")</p>
<p>&gt; opset 6 "yfx" "yfx" (diadic "yfx")</p>
<p>&gt; opset 6 "yf" "yf" (monadic "yf")</p>
<p>&gt; opset 6 "xf" "xf" (monadic "xf")</p>
<p>&gt; opset 8 "fy" "Fy" (monadic "Fy")</p>
<p>&gt; opset 8 "fx" "Fx" (monadic "Fx")</p>
<p>&gt; opset 8 "yfy" "yFy" (diadic "yFy")</p>
<p>&gt; opset 8 "xfx" "xFx" (diadic "xFx")</p>
<p>&gt; opset 8 "xfy" "xFy" (diadic "xFy")</p>
<p>&gt; opset 8 "yfx" "yFx" (diadic "yFx")</p>
<p>&gt; opset 8 "yf" "yF" (monadic "yF")</p>
<p>&gt; opset 8 "xf" "xF" (monadic "xF")</p>
<p>&gt;</p>
<p>&gt; set opeval[expr]: { parse.opeval opdef expr! }</p>
<p>&gt;</p>
<p>&gt; opeval {4}</p>
<p>4</p>
<p>&gt; opeval {"str"}</p>
<p>"str"</p>
<p>&gt; opeval {&lt;3&gt;}</p>
<p>&lt;3&gt;</p>
<p>&gt; opeval {(4)}</p>
<p>4</p>
<p>&gt; opeval {5 xfy 6}</p>
<p>"(xfy 5 6)"</p>
<p>&gt; opeval {5 yfx 6}</p>
<p>"(yfx 5 6)"</p>
<p>&gt; opeval {5 yfy 6}</p>
<p>"(yfy 5 6)"</p>
<p>&gt; opeval {5 xfx 6}</p>
<p>"(xfx 5 6)"</p>
<p>&gt; opeval {fx 7}</p>
<p>"(fx 7)"</p>
<p>&gt; opeval {fy 7}</p>
<p>"(fy 7)"</p>
<p>&gt; opeval {4 xf}</p>
<p>"(xf 4)"</p>
<p>&gt; opeval {4 yf}</p>
<p>"(yf 4)"</p>
<p>&gt; opeval {5 xfy 6 xfy 7}</p>
<p>"(xfy (xfy 5 6) 7)"</p>
<p>&gt; opeval {5 yfx 6 yfx 7}</p>
<p>"(yfx 5 (yfx 6 7))"</p>
<p>&gt;</p>
<p>&gt; opeval {3-4}</p>
<p>ftl $*console*:131: code has trailing text: ...-4</p></td>
</tr>
</tbody>
</table>

### parse root

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.root</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>None</td>
</tr>
<tr class="even">
<td>Description</td>
<td>When run, this function returns a directory containing all the names and values in the directory in scope at the start of execution.  Note that additions or deletions to the directory may have been made, but none of the values held in argument directories or entered directories will be visible.<br />
As with other functions access to this command can be eliminated using the restrict function.<br />
See also: parse.env, parse.local and enter. </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The root directory.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; restrict [exit=exit, help=help, domain=domain, parse=parse, enter=enter]</p>
<p>&gt; enter [local=parse.local, env=parse.env, root=parse.root]</p>
<p>&gt; help</p>
<p>local - return local current invocation directory</p>
<p>env - return current invocation environment</p>
<p>root - return current root environment</p>
<p>&gt; domain (parse.local!)</p>
<p>&lt;"local", "env", "root"&gt;</p>
<p>&gt; domain (parse.env!)</p>
<p>&lt;"local", "env", "root", "exit", "help", "domain", "parse", "enter"&gt;</p>
<p>&gt; domain (parse.root!)</p>
<p>&lt;"help", "set", "exit", "forall", "if", "while", "do", "sys", "parse", "io", </p>
<p>"source", "sourcetext", "return", "typeof", "typename", "cmp", "type", "NULL", </p>
<p>"rnd", "int", "TRUE", "FALSE", "equal", "notequal", "less", "lesseq", "more", </p>
<p>"moreeq", "fmt", "str", "strf", "collate", "toupper", "tolower", "strlen", </p>
<p>"octet", "chr", "octetcode", "chrcode", "split", "join", "joinchr", "ip", </p>
<p>"mac", "new", "lock", "inenv", "enter", "restrict", "domain", "range", "sort", </p>
<p>"sortdom", "sortby", "sortdomby", "every", "cmd", "echo", "eval", "sleep", </p>
<p>"len"&gt;</p>
<p>&gt;</p>
<p>&gt; # leaving parse.root available gives people access to all the commands:</p>
<p>&gt; domain (myrun=(parse.root!).sys.run)</p>
<p>&lt;"_help"&gt;</p>
<p>&gt; myrun pwd</p>
<p>/home/cgg/tree/clean/v5/src</p></td>
</tr>
</tbody>
</table>

### parse scan \<cmd\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scan</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;cmd:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function prepares a string for parsing by subsequent parse functions.  It returns a parse object which contains the part of the string that has not yet been parsed.  In general, successful parse functions consume the initial part of this string and update the object.  The string remaining can be inspected using the parse.scanned function.<br />
See also: parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A parse object which can be used as an argument to other parse.scan* functions.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set p parse.scan "123 identifier"!</p>
<p>&gt; parse scanned p</p>
<p>"123 identifier"</p>
<p>&gt; parse scanint @n p</p>
<p>TRUE</p>
<p>&gt; parse scanned p</p>
<p>" identifier"</p></td>
</tr>
</tbody>
</table>

### parse scanned \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanned</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as an argument that was initially created by parse.scan. It is used to retrieve the part of string remaining to be parsed.<br />
See also: parse.scan, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>This string contained by a parse object that remains to be consumed by parse functions.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set p parse.scan "123 identifier"!</p>
<p>&gt; parse scanned p</p>
<p>"123 identifier"</p>
<p>&gt; parse scanint @n p</p>
<p>TRUE</p>
<p>&gt; parse scanned p</p>
<p>" identifier"</p></td>
</tr>
</tbody>
</table>

### parse scanempty \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanempty</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as an argument that was initially created by parse.scan and which contains the remainder of the string to be parsed.  If it is an empty string, with no characters in it.  It will return TRUE, otherwise it returns FALSE.<br />
The intended use of this function is to ensure that the existing parsing of a string is complete - and to ensure that there is no trailing text.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if an empty string was parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td></td>
</tr>
</tbody>
</table>

### parse scanwhite \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanwhite</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as an argument that was initially created by parse.scan and which contains the remainder of the string to be parsed.   If the string begins with white space characters (spaces, newlines, tabs etc.) it will return TRUE and update the parse object with the remainder of the string, otherwise it returns FALSE.<br />
The intended use of this function is to skip over white space that may separate other elements to parse.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if one or more white space characters were parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td></td>
</tr>
</tbody>
</table>

### parse scanspace \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanspace</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as an argument that was initially created by parse.scan and which contains the remainder of the string to be parsed.  If the string begins with white space characters (spaces, newlines, tabs etc.) it will update parseobj.0 with the remainder of the string. It will always return TRUE.<br />
The intended use of this function is to skip over semantically insignificant white space that may separate other elements to parse.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE.</td>
</tr>
<tr class="even">
<td>Example</td>
<td></td>
</tr>
</tbody>
</table>

### parse scanint \<@int\> \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanwint</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;ref_int:closure&gt; &lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as its last argument that was initially created by parse.scan and which contains the remainder of the string to be parsed. If the string begins with an integer it will return TRUE and update parseobj.0 with the remainder of the string, otherwise it returns FALSE.  If the parse succeeds the value of the hexadecimal parsed is applied to the ref_int closure as an integer.<br />
The intended use of ref_int is to provide a destination for the parsed quantity in the form of a reference (e.g. "@name"), however any closure requiring a single argument can be used.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if a number is parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set numid[p,ref_n,ref_id]:{</p>
<p>&gt; (parse.scanint ref_n p)! (parse.scanwhite p)! </p>
<p>&gt; (parse.scanid ref_id p)! (parse.scanempty p)!</p>
<p>&gt; }</p>
<p>&gt; numid (parse.scan "7 baloons"!) @howmany @what</p>
<p>TRUE</p>
<p>&gt; eval howmany</p>
<p>7</p>
<p>&gt; eval what</p>
<p>"baloons"</p></td>
</tr>
</tbody>
</table>

### parse scanhex \<@int\> \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanhex</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;ref_int:closure&gt; &lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as its last argument that was initially created by parse.scan and which contains the remainder of the string to be parsed. If the string begins with a hexadecimal integer it will return TRUE and update parseobj.0 with the remainder of the string, otherwise it returns FALSE.  If the parse succeeds the value of the hexadecimal parsed is applied to the ref_int closure as an integer.<br />
The intended use of ref_int is to provide a destination for the parsed quantity in the form of a reference (e.g. "@name"), however any closure requiring a single argument can be used.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if a hexadecimal number were parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td></td>
</tr>
</tbody>
</table>

### parse scanhexw \<width\> \<@int\> \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanhexw</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;width:int&gt; &lt;ref_int:closure&gt; &lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>TThis function expects a parse object its last argument that was initially created by parse.scan and which contains the remainder of the string to be parsed. If the string begins with exactly width hexadecimal characters it will return TRUE and update parseobj.0 with the remainder of the string, otherwise it returns FALSE.  If the parse succeeds the value of the hexadecimal parsed is applied to the ref_int closure as an integer.<br />
The intended use of ref_int is to provide a destination for the parsed quantity in the form of a reference (e.g. "@name"), however any closure requiring a single argument can be used.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if width hexadecimal characters were parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td></td>
</tr>
</tbody>
</table>

### parse scanstr \<@string\> \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanstr</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;ref_string:closure&gt; &lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as its last argument that was initially created by parse.scan and which contains the remainder of the string to be parsed. If the string begins with a quoted string it will return TRUE and update parseobj.0 with the remainder of the string, otherwise it returns FALSE.  If the parse succeeds the characters of the parsed string applied to the ref_string closure as a string.<br />
The intended use of ref_string is to provide a destination for the parsed quantity in the form of a reference (e.g. "@name"), however any closure requiring a single argument can be used.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if a string is parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td></td>
</tr>
</tbody>
</table>

### parse scanid \<@string\> \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanid</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;ref_string:closure&gt; &lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as its last argument that was initially created by parse.scan and which contains the remainder of the string to be parsed. If the string begins with a identifier (that is an alphabetic character (or '_') followed by a number of alphanumeric (or '_') characters) it will return TRUE and update parseobj.0 with the remainder of the string, otherwise it returns FALSE.  If the parse succeeds the characters of the parsed identifier applied to the ref_string closure as a string.<br />
The intended use of ref_string is to provide a destination for the parsed quantity in the form of a reference (e.g. "@name"), however any closure requiring a single argument can be used.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt;  </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if an identifier is parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set numid[p,ref_n,ref_id]:{</p>
<p>&gt; (parse.scanint ref_n p)! (parse.scanwhite p)! </p>
<p>&gt; (parse.scanid ref_id p)! (parse.scanempty p)!</p>
<p>&gt; }</p>
<p>&gt; numid (parse.scan "7 baloons"!) @howmany @what</p>
<p>TRUE</p>
<p>&gt; eval howmany</p>
<p>7</p>
<p>&gt; eval what</p>
<p>"baloons"</p></td>
</tr>
</tbody>
</table>

### parse scanitem \<delims\> \<@string\> \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanitem</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;delims:string&gt; &lt;ref_string:closure&gt; &lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as its last argument that was initially created by parse.scan and which contains the remainder of the string to be parsed. If the string begins with a sequence of non-delimiter characters (delimiter characters are those from the string delims) it will return TRUE and update parseobj.0 with the remainder of the string, otherwise it returns FALSE.  If the parse succeeds the characters of the parsed item applied to the ref_string closure as a string.<br />
The intended use of ref_string is to provide a destination for the parsed quantity in the form of a reference (e.g. "@name"), however any closure requiring a single argument can be used.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if one or more non-delimiter characters were parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td></td>
</tr>
</tbody>
</table>

### parse scanitemstr \<@string\> \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanitemstr</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;ref_string:closure&gt; &lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as an argument that was initially created by parse.scan and which contains the remainder of the string to be parsed.  If the string begins with either a sequence of non-white characters or a quoted string it will return TRUE and update parseobj.0 with the remainder of the string, otherwise it returns FALSE.  If the parse succeeds a characters of the parsed item or string are applied to the ref_string closure.<br />
The intended use of ref_string is to provide a destination for the parsed quantity in the form of a reference (e.g. "@name"), however any closure requiring a single argument can be used.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if an item or a quoted string is parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td></td>
</tr>
</tbody>
</table>

### parse scanmatch \<prefixes\> \<@string\> \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanmatch</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;prefixes:dir&gt; &lt;ref_val:closure&gt; &lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as an argument that was initially created by parse.scan and which contains the remainder of the string to be parsed. The first argument is a directory of name-value pairs.  This function attempts to parse one of the names as a prefix in the string.  If the string begins one of the names it will return TRUE and update parseobj.0 with the remainder of the string, otherwise it returns FALSE.  If the parse succeeds the value associated with the parsed name in prefixes is applied to the ref_val closure.<br />
The intended use of ref_string is to provide a destination for the parsed quantity in the form of a reference (e.g. "@name"), however any closure requiring a single argument can be used.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt; </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if an prefix contained in delims is parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set cmd "a long line\n"</p>
<p>&gt; set translations [a=5, line="LINE", long=&lt;99&gt;]</p>
<p>&gt; set toparse parse.scan cmd!</p>
<p>&gt; set tran NULL</p>
<p>&gt; parse scanmatch translations @tran toparse</p>
<p>TRUE</p>
<p>&gt; parse scanned toparse</p>
<p>" long line\n"</p>
<p>&gt; eval tran</p>
<p>5</p>
<p>&gt; parse scanspace toparse</p>
<p>TRUE</p>
<p>&gt; parse scanmatch translations @tran toparse</p>
<p>TRUE</p>
<p>&gt; parse scanned toparse</p>
<p>" line\n"</p>
<p>&gt; eval tran</p>
<p>&lt;99&gt;</p>
<p>&gt; parse scanspace toparse</p>
<p>TRUE</p>
<p>&gt; parse scanmatch translations @tran toparse</p>
<p>TRUE</p>
<p>&gt; parse scanned toparse</p>
<p>"\n"</p>
<p>&gt; eval tran</p>
<p>"LINE"</p></td>
</tr>
</tbody>
</table>

### parse scanops \<baseparse\> \<opdefs\> \<@string\> \<parseobj\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.scanops</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;baseparse:closure&gt; &lt;opdefs:dir&gt; &lt;ref_val:closure&gt; &lt;parseobj:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function expects a parse object as its last argument that was initially created by parse.scan and which contains the remainder of the string to be parsed.<br />
The first argument must be a parsing closure that has two unbound arguments: the first returning a value ("@val") and the second containing a directory like the one provided as the first argument to this function.<br />
The second argument is a directory of operator definitions such as that constructed by parse.opset.  The base values connected by these operators are to be parsed by the baseparse argument.<br />
This function attempts to parse an expression in the string constructed with the operators defined in opdefs.  If the string begins with such an expression it will return TRUE and update parseobj.0 with the remainder of the string, otherwise it returns FALSE.  If the parse succeeds the value associated with the parsed expression is applied to the ref_val closure.<br />
The intended use of ref_string is to provide a destination for the parsed quantity in the form of a reference (e.g. "@name"), however any closure requiring a single argument can be used.<br />
See also: parse.scan, parse.scanned, scan&lt;others&gt;, parse.scan&lt;others&gt;, parse.opset<br />
<br />
THIS FUNCTION IS NOT YET IMPLEMENTED </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>TRUE if a prefix contained in delims is parsed, FALSE otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td></td>
</tr>
</tbody>
</table>

### parse source

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>parse.source</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>None</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns the name of the current source of characters being interpreted.  The name is the name  of the source as it was at the first character of the last command executed.<br />
See also: parse.line </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The current interpreted source.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; parse source</p>
<p>"$*console*"</p>
<p>&gt; set thissrc[]:{ echo "source "+(str (parse.source!)!)! }</p>
<p>&gt; parse exec "echo init; thissrc" (io.instring "echo start\n thissrc" "r"!)</p>
<p>init</p>
<p>source "\$&lt;INIT&gt;"</p>
<p>start</p>
<p>source "\$&lt;string-in&gt;"</p></td>
</tr>
</tbody>
</table>

### **pow** \<**number**\> \<**number**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>pow</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n1:number&gt; &lt;n2:number&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two integer or reals and raises the first to the power of the second returning the result. n2 is treated as an unsigned number.<br />
If either n1 or n2 are real numbers the result will be a real otherwise it is an integer. (Note that powers that are too large to be represented as integers are not converted to reals.)</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer or real with the value n1<sup>n2</sup>.<br />
See also: add, sub, mul, div, abs, neg, shiftr, shiftl</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; pow 2 32</p>
<p>4294967296</p>
<p>&gt; pow (-2) 3</p>
<p>-8</p>
<p>&gt; pow (-2) 31</p>
<p>-2147483648</p>
<p>&gt; pow (-2.0000000001) 31</p>
<p>-2147483651.3286</p>
<p>&gt; pow 2147483648 (1.0/31)</p>
<p>2</p>
<p>&gt; eval pow 3 (-1)!</p>
<p>-6148914691236517205</p>
<p>&gt; eval pow 3 0xffffffffffffffff!</p>
<p>-6148914691236517205</p></td>
</tr>
</tbody>
</table>

### range \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>range</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;mapping:dir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Constructs a vector containing the values that are named in the mapping provided.  Values associated with names that are hidden because of scoping rules will not appear.  The order of entries in the vector is the same as those that are generated in the domain function.  If the mapping is itself a vector (numerically indexed) the order of entries will be identical to the mapping, otherwise the order is undefined with respect to the mapping.<br />
See also: domain</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A vector of the values contained in the argument directory,</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; range [mon="eggs", tue="bacon", wed="flour"]</p>
<p>&lt;"eggs", "bacon", "flour"&gt;</p>
<p>&gt; echo ${join " == " (range [a="£3", b="€4.5", "c"="$$4"]!)!}</p>
<p>£3 == €4.5 == $4</p></td>
</tr>
</tbody>
</table>

### restrict \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>restrict</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;env:dir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>restrict further commands to those in &lt;env&gt;</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>This command removes the current environment stack from scope and pushes its argument directory env onto the current environment stack thus bringing all the names defined in the directory into scope.<br />
Since the commands parse.local and help are active only on the top directory in the current environment the command has a direct effect on them.<br />
Note that the environment entered during the execution of a block goes out of scope at the end of the block, together with any pushed directories.  This will mean that the original environment will become available again once the block incorporating the restrict has exited.<br />
This function can be used to reduce the complexity of the syntax available subsequently.  It can be particularly useful as an argument to parse.exec in order to constrain a file's syntax.  Note, however, that including any functions with one or more arguments among those allowed may enable the current environment to be altered (using the &lt;name&gt; = &lt;value&gt; FTL syntax) - this may be overcome by defining commands to replace the functions using the cmd function.  The lock function may be of use if this is a problem.<br />
See also: enter, leave, lock, cmd, parse.exec</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; restrict [h=help, e=echo, x=leave, ans=42]</p>
<p>&gt; h</p>
<p>h [all] [&lt;cmd&gt;] - prints information about commands</p>
<p>e &lt;whole line&gt; - prints the line out</p>
<p>x - exit the environment last entered or restricted</p>
<p>&gt; eval h           # we don't have an 'eval' command any more</p>
<p>ftl $*console*:+27 in</p>
<p>ftl $*console*:28: unknown command 'eval h'</p>
<p>&gt; x</p>
<p>&gt; eval h           # we have eval again, but not 'eval'</p>
<p>ftl $*console*:+29 in</p>
<p>ftl $*console*:30: undefined symbol 'h'</p>
<p>ftl $*console*:+29: failed to evaluate expression</p>
<p>% cat &gt; /tmp/2</p>
<p>e simple</p>
<p>e script</p>
<p>x</p>
<p>^D</p>
<p>% SCRIPT=/tmp/2 ftl</p>
<p>ftl: v1.6</p>
<p>&gt; set script_in io.file sys.env.SCRIPT "r"!</p>
<p>&gt; parse exec "restrict [e=echo, x=exit]" script_in</p>
<p>simple</p>
<p>script</p></td>
</tr>
</tbody>
</table>

### return \<rc\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>return</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;rc:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>abandon current command input returning &lt;rc&gt;</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the name of the interpreter assigned by the library-using application</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set script_in io.instring "echo starting; return 2; echo unreached" "r"!</p>
<p>&gt; set rc parse.exec "" script_in!</p>
<p>starting</p>
<p>2</p>
<p>&gt; eval rc</p>
<p>2</p></td>
</tr>
</tbody>
</table>

### **rint** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>rint</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real (normally representing an integer) and returns an integer. The nearest integer is given.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer with the integral number nearest to n. (Other than the type of the result this function is very similar to round.)<br />
Note: the result is always an integer, not a real.<br />
See also: round, floor, ceil</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; rint 10.0</p>
<p>10</p>
<p>&gt; rint 10.1</p>
<p>10</p>
<p>&gt; rint 10.9</p>
<p>11</p>
<p>&gt; rint -10.0</p>
<p>-10</p>
<p>&gt; rint -10.1</p>
<p>-10</p>
<p>&gt; rint -10.9</p>
<p>-11</p>
<p>&gt; typeof (rint 10!)</p>
<p>$basetype.int</p>
<p>&gt; typeof (rint 10.0!)</p>
<p>$basetype.int</p></td>
</tr>
</tbody>
</table>

### **round** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>round</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real and returns a real number representing the nearest integer.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An real with the integral number nearest to n.<br />
Note: the result is not an integer – rint can be used to convert it to an integer though.<br />
See also: rint, floor, ceil</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; round 10.1</p>
<p>10</p>
<p>&gt; round 10.9</p>
<p>11</p>
<p>&gt; round -10.1</p>
<p>-10</p>
<p>&gt; round -10.9</p>
<p>-11</p>
<p>&gt; typeof (round 10!)</p>
<p>$basetype.int</p>
<p>&gt; typeof (round 10.1!)</p>
<p>$basetype.real</p></td>
</tr>
</tbody>
</table>

### rnd \<n\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>rnd</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Choose pseudo random number less than n and greater or equal to zero.</p>
<p>The random number generator produces results based on a fixed sequence of random numbers. The sequence of numbers produced is determinstic and will depend on the initial value of a "seed".</p>
<p>See also: rndseed</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A random integer</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; echo ${rnd 256!}.${rnd 256!}.${rnd 256!}.${rnd 256!}</p>
<p>4.62.35.205</p>
<p>&gt; set choose[vec]:{ vec.(rnd (len vec!)!) }</p>
<p>&gt; choose &lt;"red", "white", "blue"&gt;</p>
<p>"blue"</p></td>
</tr>
</tbody>
</table>

### rndseed \<n\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>rndseed</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;seed:int&gt; or &lt;seed:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Set the "seed" used for the random number generator.<br />
Up to 32-bits of entropy inferred from the seed which then determines the sequence of values returned from the rnd function. The value given can either be in the form of an integer or a binary string. There may be several different values of seed that will result in the same sequence of values returned from rnd. Whenever the specific seed is provided the same sequence results, though.</p>
<p>If, for example, the random number sequence is used to build test data that same test data can be regenerated in a subsequent run by recording and re-establishing the same rndseed prior to execution.</p>
<p>The sequence of values returned for a given seed is dependent on the build of the interpreter. It is likely, for example, that the same seed will yield different sequences when used on interpreters used on different types of computer or operating system. </p>
<p>See also: rnd, sys.osfamily</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set test[]:{</p>
<p>&gt;    io.fprintf io.out "%d.%d.%d.%d colour " &lt;</p>
<p>&gt;                      rnd 256!, rnd 256!, </p>
<p>&gt;                      rnd 256!, rnd 256! &gt;!;</p>
<p>&gt;    echo &lt;"red", "white", "blue"&gt;.(rnd 3!)!;</p>
<p>&gt; }</p>
<p>&gt; set myseed sys.time!; echo Using seed $myseed</p>
<p>Using seed 1555250522</p>
<p>&gt; rndseed myseed</p>
<p>&gt; test</p>
<p>245.32.252.87 colour blue</p>
<p>&gt; test</p>
<p>128.151.116.88 colour white</p>
<p>&gt; rndseed myseed</p>
<p>&gt; test</p>
<p>245.32.252.87 colour blue</p>
<p>&gt; rndseed "a longer string to extract a seed from"</p></td>
</tr>
</tbody>
</table>

### select \<closure\> \<dir\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>select</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;choose:clocode&gt; &lt;set:clodir&gt; </td>
</tr>
<tr class="even">
<td>Description</td>
<td>This function executes choose for every name-value pair in set and returns the subset of set for which choose returns TRUE.<br />
If choose is a code value it is simply executed in the environment where select was invoked, once for every member in the set, otherwise choose may be a closure with either one or two arguments.<br />
If it is a choose with one argument, that argument is successively bound to different values in the name-value pairs in the set.<br />
If it is a closure with two arguments, the first argument is successively bound to different values, while the second is bound to the corresponding name<br />
The order of evaluation is defined only for vectors.<br />
See also: TRUE</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The subset of set for which choose returned TRUE as a directory.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; eval select [v]:{ equal "int" (typename v!)! } &lt;1,2,"this",4,"that"&gt;!</p>
<p>&lt;1, 2, 3=4&gt;</p>
<p>&gt; eval select [v]:{ equal "string" (typename v!)! } &lt;1,2,"this",4,"that"&gt;!</p>
<p>&lt;2="this", 4="that"&gt;</p>
<p>&gt; set inorder select [max=0,v]:{ if (more v max!) { max=v; TRUE } {FALSE}! }</p>
<p>&gt; inorder &lt;"ay", "bee", "see", "dee", "ee"&gt;</p>
<p>&lt;"ay", "bee", "see"&gt;</p>
<p>&gt; inorder &lt;2,3,4,2,4,0,7&gt;</p>
<p>&lt;2, 3, 4, 6=7&gt;</p>
<p>&gt; set start[n, vec]: { select [v,ix]:{ moreeq ix n! } vec! }</p>
<p>&gt; start 3 &lt;"metis", "adrasthea", "amalthea", "thebe", "io", "europa"&gt;</p>
<p>&lt;3="thebe", "io", "europa"&gt;</p>
<p>&gt; set pints [mon=1, tue=3, wed=2, thu=2, fri=6, sat=0, sun=0]</p>
<p>&gt; select (lesseq 2) pints</p>
<p>["tue"=3, "wed"=2, "thu"=2, "fri"=6]</p>
<p>&gt; select (equal 0) pints</p>
<p>["sat"=0, "sun"=0]</p></td>
</tr>
</tbody>
</table>

### set \<name\> \<expr\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>set</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;name:token&gt; &lt;value:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Provide a name for the given value in the current environment. If the name does not appear in the current environment it is added (to the environment with innermost-scope).  The name may specify the directly in which it is set explicitly (by using the '&lt;dir&gt;.&lt;name&gt;'  syntax).  The innermost-scope can be specified explicitly using the syntax '.&lt;name&gt;'.<br />
This command provides a similar function to the FTL expression &lt;name&gt; = &lt;value&gt;. </p>
<p>See also: func</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set name "string value"</p>
<p>&gt; set dir [old=0]</p>
<p>&gt; set dir.old "new"</p>
<p>&gt; set dir.(dir.old) 1</p>
<p>&gt; eval dir</p>
<p>["old"="new","new"=1]</p>
<p>&gt; set proc dir:{ echo old! }</p>
<p>&gt; proc</p>
<p>new</p>
<p>&gt; set fn [old]:{ echo old! }</p>
<p>&gt; fn "this"</p>
<p>this</p></td>
</tr>
</tbody>
</table>

### shiftl \<int\> \<int\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>shiftl</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n1:int&gt; &lt;n2:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two integers and shifts the bit representation of the first left by the number of bits specified in the second, filling newly vacated bit positions with zeros.<br />
This is normally equivalent to dividing n1 by 2 to the power of n2.<br />
Shifts by a negative amount leave the value unchanged.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer with the value n1 shifted left by n2.<br />
See also: add, sub, mul, div, pow, shiftr, abs, neg</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; shiftl 1 3</p>
<p>8</p>
<p>&gt; shiftl 1 -3</p>
<p>8</p>
<p>&gt; shiftl 1 (-3)</p>
<p>1</p></td>
</tr>
</tbody>
</table>

### shiftr \<int\> \<int\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>shiftr</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n1:int&gt; &lt;n2:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two integers and shifts the bit representation of the first left by the number of bits specified in the second, filling newly vacated bit positions with zeros.<br />
For a positive n1, this is normally equivalent to dividing n1 by 2 to the power of n2.  This is not true for negative numbers however.<br />
Shifts by negative amounts leave the value unchanged.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer with the value n1 shifted left by n2.<br />
See also: add, sub, mul, div, pow, shiftl, abs, neg</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; shiftr 16 3</p>
<p>2</p>
<p>&gt; shiftr 16 (-3)</p>
<p>16</p>
<p>&gt; shiftr -16 3</p>
<p>2305843009213693950</p></td>
</tr>
</tbody>
</table>

### **sin** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sin</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number of radians and returns its trigonometric sine.<br />
The inverse function to asin.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value sin(x).<br />
See also: cos, tan, asin, acos, atan, sinh</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (sin 0.0!)</p>
<p>0.000000000000</p>
<p>&gt; preal (sin PI/3!)**2</p>
<p>0.750000000000</p>
<p>&gt; preal (sin (-PI/3)!)**2</p>
<p>0.750000000000</p>
<p>&gt; preal (sin PI/4!)**2</p>
<p>0.500000000000</p>
<p>&gt; preal (sin PI/2!)**2</p>
<p>1.000000000000</p>
<p>&gt; preal (sin (asin 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### **sinh** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sin</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns its hyperbolic sine (½(e<sup>x</sup>+e<sup>-x</sup>)).<br />
The inverse function to asinh.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value sinh(x).<br />
See also: cosh, tanh, asinh, acosh, atanh, sin</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (sinh 0.0!)</p>
<p>0.000000000000</p>
<p>&gt; preal (sinh 1.0!)</p>
<p>1.175201193644</p>
<p>&gt; preal (sinh (-1.0)!)</p>
<p>-1.175201193644</p>
<p>&gt; preal (sinh (asinh 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### sleep \<n\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sleep</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;milliseconds:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Sleep for the given number of milliseconds.<br />
A C function can be delegated to the parser to execute during this call using the C function parser_suspend_put().  Such functions can perform background processing during the wait.<br />
See also: every</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sleep 500</p>
<p>&gt;</p>
<p>&gt; set rpt[ms,fn]:{ while {1} {fn!; sleep ms!;}! }</p>
<p>&gt; rpt 1000 { echo "tick"! }</p>
<p>tick</p>
<p>tick</p>
<p>tick</p>
<p>tick</p>
<p>tick</p>
<p>tick</p></td>
</tr>
</tbody>
</table>

### sort \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sort</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;env:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Provides a vector of names in the provided environment which is ordered by the values each name is associated with in the environment.<br />
This function is identical to:</p>
<p>sortby cmp</p>
<p>(but is implemented more efficiently).<br />
See also: sortby, sortdom, cmp, range</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A vector of names from the given environment with entries for each of the name-value pairs in it, in which entries are ordered so that the directory values of names with a smaller index are less than or equal to those with a larger index according to the comparison function cmp.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sort &lt;-9,NULL,[a=3],"string",45,&lt;&gt;,3&gt;</p>
<p>&lt;1, 0, 6, 4, 3, 2, 5&gt;</p>
<p>&gt; set max[vec]:{ vec.((sort vec!).(-1+(len vec!))) }</p>
<p>&gt; max &lt;2,6,3,5,6,8,-8&gt;</p>
<p>8</p>
<p>&gt; set min[vec]:{ vec.((sort vec!).0) }</p>
<p>&gt; min &lt;2,6,3,5,6,8,-8&gt;</p>
<p>-8</p></td>
</tr>
</tbody>
</table>

### sortby \<cmpfn\> \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sortby</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;cmpfn:closure&gt; &lt;env:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Provides a vector of names in the provided environment which is ordered by the values each name is associated with in the environment.  The ordering is dictated by the cmpfn function which is executed with two arguments to be compared, and must return </p>
<p>See also: sort, sortdomby, range</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A vector of names from the given environment with entries for each of the name-value pairs in it, in which entries are ordered so that the directory values of names with a smaller index are less than or equal to those with a larger index according to the comparison function provided.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sortby [a,b]:{cmp b a!} &lt;-9,NULL,[a=3],"string",45,&lt;&gt;,3&gt;</p>
<p>&lt;2, 5, 3, 4, 6, 0, 1&gt;</p>
<p>&gt; set cmpmin [v1,v2]:{ cmp v1.((sort v1!).0) v2.((sort v2!).0)! }</p>
<p>&gt; set maxmin[vecvec]:{ sortby cmpmin vecvec! }</p>
<p>&gt; maxmin &lt;&lt;0,2,3&gt;,&lt;8&gt;,&lt;9,7,-1&gt;,&lt;4,3&gt;&gt;</p>
<p>&lt;2, 0, 3, 1&gt;</p></td>
</tr>
</tbody>
</table>

### sortdom \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sortmod</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;env:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Provides a vector of names in the provided environment which is ordered by the names in the environment.  Note that because the "names" in a vector are already in order this function is more useful on other kinds of directory.<br />
This function is identical to:</p>
<p>sortmodby cmp</p>
<p>(but is implemented more efficiently).<br />
See also: sortmodby, sort, cmp, domain </p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A vector of names from the given environment with entries for each of the name-value pairs in it, in which entries are ordered so that the names with a smaller index are less than or equal to those with a larger index according to the comparison function cmp.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sortdom &lt;4,5,67,2&gt;</p>
<p>&lt;0, 1, 2, 3&gt;</p>
<p>&gt; sortdom [one=1, two=2, three=3]</p>
<p>&lt;"one", "three", "two"&gt;</p>
<p>&gt; set in_order [_vals, _fn]:{</p>
<p>&gt;    forall (sortdom _vals!) [_val]:{ _fn _vals.(_val) _val! }!</p>
<p>&gt; }</p>
<p>&gt; in_order [apples=12, pears=8, bananas=7] [n,fruit]:{ </p>
<p>&gt;   io.fprintf io.out "we have %d %s\n" &lt;n,fruit&gt;! </p>
<p>&gt; }</p>
<p>we have 12 apples</p>
<p>we have 7 bananas</p>
<p>we have 8 pears</p></td>
</tr>
</tbody>
</table>

### sortdomby \<cmpfn\> \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sortdomby</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;cmpfn:closure&gt; &lt;env:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Provides a vector of names in the provided environment which is ordered by the names in the environment.  The ordering is dictated by the cmpfn function which is executed with two arguments to be compared, and must return </p>
<p>See also: sortdom, sortby, domain</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A vector of names from the given environment with entries for each of the name-value pairs in it, in which entries are ordered so that the names with a smaller index are less than or equal to those with a larger index according to the comparison function provided.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sortdomby [a,b]:{cmp b a!} [one=1, two=2, three=3]</p>
<p>&lt;"two", "three", "one"&gt;</p>
<p>&gt; sortdomby collate ["fred"=7, "£"=99, "FORMER"=2, "104"=77]</p>
<p>&lt;"104", "FORMER", "fred", "\xc2\xa3"&gt;</p></td>
</tr>
</tbody>
</table>

### source \<filename\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>source</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;tokstring:filename&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Open the filename for reading and interpret lines from that file as commands to be executed.<br />
Normally file execution continues until it is completely read or until a return or exit command are executed.<br />
When the last octet is read from the file, or the interpreter is closed, the file will be closed automatically.<br />
See also: parse.exec, sourcetext, command, return, exit</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>When c:\\extra-commands contains:</p>
<p>echo executing…</p>
<p>&gt; source c:\\extra-commands</p>
<p>executing...</p>
<p>&gt; set test[]:{ source "C:\\extra-commands"!; echo "finished"! }</p>
<p>&gt; test</p>
<p>executing…</p>
<p>finished</p>
<p>&gt;</p></td>
</tr>
</tbody>
</table>

### sourcetext \<stringexpr\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sourcetext</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;text:stringorcode&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Execute the provided text (provided either as a string or a code value) as if it were source commands.</p>
<p>Normally execution continues until it is completely read or until a return or exit command are executed.<br />
See also: parse.exec, source, command, return, exit </p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Any value provided by a return command.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sourcetext "echo executing..."</p>
<p>executing...</p>
<p>&gt; sourcetext {</p>
<p>&gt;   echo executing…</p>
<p>&gt;}</p>
<p>executing…</p>
<p>&gt; set state "executing"</p>
<p>&gt; sourcetext " enter [state=\"running\"]; echo ${state}...; "</p>
<p>executing...</p>
<p>&gt; sourcetext { enter [state="running"]; echo ${state}...; }</p>
<p>running...</p>
<p>&gt; sourcetext " enter [state=\"running\"]; echo \${state}...; "</p>
<p>running…</p>
<p>&gt; set test[]:{ sourcetext "echo executing...\n"!; echo "Started"! }</p>
<p>&gt; test</p>
<p>Started</p>
<p>&gt; set IF[cond,then,else]:{ </p>
<p>&gt;   if cond {sourcetext then!}{sourcetext else!}! </p>
<p>&gt; }</p>
<p>&gt; IF TRUE { </p>
<p>&gt;    echo true</p>
<p>&gt;    return 1 </p>
<p>&gt; } { </p>
<p>&gt;    echo false</p>
<p>&gt;    return 0 </p>
<p>&gt; }</p>
<p>true</p>
<p>1</p></td>
</tr>
</tbody>
</table>

### split \<delim\> \<str\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>split</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;delimiter:stringnl&gt; &lt;arg:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Splits arg into a vector of strings separated by the delimiter.  If the delimiter is an emtpy string the argument is split into (possibly multi-byte) characters.  If it is NULL then the argument is split into octets.<br />
See also: join, joinchr, len, strlen</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the name of the interpreter assigned by the library-using application</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; split "/" "dir/subdir/file"</p>
<p>&lt;"dir", "subdir", "file"&gt;</p>
<p>&gt; split "\0" "line1\0line2"</p>
<p>&lt;"line1", "line2"&gt;</p>
<p>&gt; split "" "£€¢"</p>
<p>&lt;"\xc2\xa3", "\xe2\x82\xac", "\xc2\xa2"&gt;</p>
<p>&gt; split NULL "£€¢"</p>
<p>&lt;"\xc2", "\xa3", "\xe2", "\x82", "\xac", "\xc2", "\xa2"&gt;</p>
<p>&gt; split NULL "ascii"</p>
<p>&lt;"a", "s", "c", "i", "i"&gt;</p>
<p>&gt; set spaceout[s]: { join " - " (split "" s!)! }</p>
<p>&gt; echo ${spaceout "£€¢"!}</p>
<p>£ - € - ¢</p>
<p>&gt; set args[str]: { range (select (notequal "") (split " " str!)!)! }</p>
<p>&gt; args " a  badly spaced    command line"</p>
<p>&lt;"a", "badly", "spaced", "command", "line"&gt;</p></td>
</tr>
</tbody>
</table>

### **sqrt** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sqrt</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns its positive square root.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value √x (a number which, when multiplied by itself gives x).<br />
See also: mul, pow, exp, log, shiftr</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sqrt 625</p>
<p>25</p>
<p>&gt; sqrt -625</p>
<p>-nan</p>
<p>&gt; sqrt 0</p>
<p>0</p>
<p>&gt; sqrt 0.0625</p>
<p>0.25</p>
<p>&gt; set diag[x,y]:{sqrt x**2 + y**2!}</p>
<p>&gt; diag 3 4</p>
<p>5</p></td>
</tr>
</tbody>
</table>

### str \<expr\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>str</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;value:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Generate a string representation of the value provided.  The string representation is one that, if read as a value should (where possible) regenerate the value.  This function is equivalent to </p>
<p>[val]: { io.outstring [out]:{ io.stringify out val! }! }</p>
<p>See also: io.stringify, io.strf</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the name of the interpreter assigned by the library-using application</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set showstr[val]: { io.write io.out "value: "+(str val!)+"\n"!; }</p>
<p>&gt; set show[val]: { io.write io.out "value: "+(val)+"\n"!; }</p>
<p>&gt; show "shaln't"</p>
<p>value: shaln't</p>
<p>&gt; showstr "shaln't"</p>
<p>value: "shaln\'t"</p>
<p>&gt; showstr 100</p>
<p>value: 100</p>
<p>&gt; show 100</p>
<p>ftl: value has wrong type - type is int, expected string</p>
<p>ftl $*console*:49: string expected after '+'</p>
<p>ftl $*console*:49: error in '+(val)+"\n"!; '</p>
<p>["_1"=&lt;-EOF-&gt;,"_help"="&lt;stream&gt; &lt;string&gt; - write string to stream", "_2"]::&lt;func:0x8053ff4,2&gt;</p>
<p>&gt; set s str "that's \"£££\"s\n"!</p>
<p>&gt; echo $s</p>
<p>"that\'s \"\xc2\xa3\xc2\xa3\xc2\xa3\"s\n"</p>
<p>&gt; eval s</p>
<p>"\"that\\\'s \\\"\\xc2\\xa3\\xc2\\xa3\\xc2\\xa3\\\"s\\n\""</p>
<p>&gt; eval str [a=3, b="this", c=&lt;"one", 2, 3&gt;, d=&lt;7..99&gt;]!</p>
<p>"[\"a\"=3,\"b\"=\"this\",\"c\"=&lt;\"one\", 2, 3&gt;,\"d\"=&lt;7 .. 9&gt;]"</p></td>
</tr>
</tbody>
</table>

### strf \<fmt\> \<env\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>strf</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;format:string&gt; &lt;values:clodir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Generate a string according to the format provided from the values given in the values directory.<br />
The format string must conform to the following syntax: </p>
<p>[ &lt;text&gt; | %% | %[&lt;fieldname&gt;]&lt;format&gt; ]*</p>
<p>where &lt;text&gt; is simply a sequence of non '%' characters, the optional &lt;fieldname&gt; is alphanumeric text enclosed by '[' and ']' and the &lt;format&gt; is</p>
<p>[-][+][0][ ][#][&lt;digits&gt;+][.&lt;digits&gt;+][&lt;fmtchar&gt; | {&lt;fmtstr&gt;}]</p>
<p>in which the optional prefixes have these uses:</p>
<p>The format name is then specified either as a single character &lt;fmtchar&gt; or as multiple characters in braces {&lt;fmtstr&gt;}.  This name is looked up in the "fmt" directory in the root environment to obtain a binding that is invoked in order to generate the basic output.  The formats supported can be extended by including additional bindings in this directory.  The bindings are invoked as iff called using</p>
<p>fmt.&lt;name&gt; &lt;flags&gt; &lt;precision&gt; &lt;value&gt;!</p>
<p>This location (fmt) may be altered in future versions.<br />
<br />
Curently the following formats are supported (although the user can add more):</p>
<p><br />
Each &lt;text&gt; is copied directly to the output string.  The sequence %% copies a single percent to the output string.  Each &lt;format&gt; selects an argument from the given values and formats it according to the rules above.  The argument selected will be the one indexed either by the &lt;fieldname&gt; if it is provided, or by the next integer (starting from an initial index of 0). </p>
<p>See also: str, io.fprintf</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the formatted representation of the arguments provided in the given environment.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set on_1line[val]:{io.write io.out (strf "%-.79v" &lt;val&gt;!)!;io.write io.out "\n"!;}</p>
<p>&gt; on_1line 3</p>
<p>3</p>
<p>&gt; on_1line fmt</p>
<p>[d=[_1, _2, _3]::&lt;func:0x55e8c0516e8d,3&gt;, u=[_1, _2, _3]::&lt;func:0x55e8c0516eb9,</p>
<p>&gt; fmt help</p>
<p>d &lt;f&gt; &lt;p&gt; &lt;val&gt; - %d integer format</p>
<p>u &lt;f&gt; &lt;p&gt; &lt;val&gt; - %u unsinged format</p>
<p>x &lt;f&gt; &lt;p&gt; &lt;val&gt; - %x hex format</p>
<p>X &lt;f&gt; &lt;p&gt; &lt;val&gt; - %X hex format</p>
<p>s &lt;f&gt; &lt;p&gt; &lt;val&gt; - %s string format</p>
<p>S &lt;f&gt; &lt;p&gt; &lt;val&gt; - %s zero terminated string format</p>
<p>c &lt;f&gt; &lt;p&gt; &lt;val&gt; - %c character format</p>
<p>b &lt;f&gt; &lt;p&gt; &lt;val&gt; - %b little endian binary format</p>
<p>B &lt;f&gt; &lt;p&gt; &lt;val&gt; - %B big endian binary format</p>
<p>v &lt;f&gt; &lt;p&gt; &lt;val&gt; - %v value format</p>
<p>j &lt;f&gt; &lt;p&gt; &lt;val&gt; - %j (JSON) value format</p>
<p>J &lt;f&gt; &lt;p&gt; &lt;val&gt; - %J (pretty JSON) value format</p>
<p>&gt; set hdump[str]:{</p>
<p>&gt;    forall (split NULL str!) [h]:{</p>
<p>&gt;        io.write io.out (strf "%02X" h!)!;</p>
<p>&gt;     }!</p>
<p>&gt; } </p>
<p>&gt; set printf io.fprintf io.out</p>
<p>&gt; printf "tokens %[one]t (not %[one]v) and %[two]t\n" [one="string", two=9]</p>
<p>tokens string (not "string") and 9</p>
<p>35</p>
<p>&gt; set show[fmt,val]:{</p>
<p>&gt;  .chrs=printf fmt &lt;val&gt;!; </p>
<p>&gt;  printf "\n(%d characters)\n"&lt;chrs&gt;!;</p>
<p>&gt; }</p>
<p>&gt; show "%J" [a=10, b=&lt;3,5,7&gt;, c="this"]</p>
<p>{</p>
<p>    "a" : 10, </p>
<p>    "b" : [3, 5, 7], </p>
<p>    "c" : "this"</p>
<p>}</p>
<p>(57 characters)</p>
<p>&gt; set packet[op,dest,data]:{</p>
<p>&gt;   outstring [s]:{</p>
<p>&gt;       printf "%.1b%.4b%s" &lt;op, dest, data&gt;!;</p>
<p>&gt;   }!</p>
<p>&gt; }</p>
<p>&gt; packet 2 0x12131415 "somedata"</p>
<p>"\x02\x15\x14\x13\x12somedata"</p>
<p>Currently there is a bug in integer formats that occurs when the 0 and the + prefixes are provided together.</p></td>
</tr>
</tbody>
</table>

### strlen \[\<string\>\]

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>strlen</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;chars:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Gives the number of (possibly multibyte) characters in string.  The argument string is interpreted as a sequence of multi-byte characters according to the current locale.  Typically the UTF-8 encoding of the Unicode character set will be used.  Because some characters are not encoded in a single octet this will not always give the same result as len.<br />
The number of characters in a string will correspond to the number of elements in the result of split "".<br />
See also: len, split, joinchr </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>The number of characters held in a string</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; len "£€¢"</p>
<p>7</p>
<p>&gt; strlen "£€¢"</p>
<p>3</p>
<p>&gt; strlen "ascii"</p>
<p>5</p></td>
</tr>
</tbody>
</table>

### sub \<**number**\> \<**number**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sub</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;n1:number&gt; &lt;n2:number&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes two integers or reals and subtracts the second from the first.<br />
If either n1 or n2 are real numbers the result will be a real.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer with the value n1-n2.<br />
See also: add, mul, div, pow, abs, neg, shiftr, shiftl</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sub 0x100 12</p>
<p>244</p>
<p>&gt; sys localtimef "%T" (sub (sys.time!) 600!)</p>
<p>"15:10:14"</p>
<p>&gt; eval sub 6 0.5!</p>
<p>5.5</p></td>
</tr>
</tbody>
</table>

### sys env

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sys.env</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Directory</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Description</td>
<td>A non-enumerable directory containing the names and string values of the system environment.  This directory can be both read from and written to, but must be indexed by strings and must be given string values.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sys env USER</p>
<p>"gray"</p>
<p>&gt; set sys.env.new str [a=4, b=7]!</p>
<p>&gt; eval ${sys.env.new}</p>
<p>["a"=4,"b"=7]</p></td>
</tr>
</tbody>
</table>

### sys localtime \<time\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sys.localtime</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;time:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Takes a number of seconds representing a calendar time (e.g. as returned by sys.time) and breaks it down into components representing elements of the local time relative to the current timezone in a returned directory with fields:</p>
<p>See also: sys.localtimef, sys.utctime, sys.time</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A directory with values for names sec, min, hour, mday, mon, year, wday, yday and isdst.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sys localtime (sys.time!)</p>
<p>["sec"=39, "min"=46, "hour"=12, "mday"=21, "mon"=5, "year"=2006, "wday"=3, </p>
<p>"yday"=171, "isdst"=1]</p>
<p>set logrec[out, when, rec]:{</p>
<p>    t = sys.localtime when!;</p>
<p>    io.write out (octet 0+(t.year)-2000!)!;</p>
<p>    io.write out (octet t.mon!)!;</p>
<p>    io.write out (octet t.mday!)!;</p>
<p>    io.write out (octet t.hour!)!;</p>
<p>    io.write out (octet t.min!)!;</p>
<p>    io.write out (octet (len rec!)!)!;</p>
<p>    io.write out rec!;</p>
<p>}</p></td>
</tr>
</tbody>
</table>

### sys localtimef \<format\> \<time\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sys.localtimef</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;format:string&gt; &lt;time:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Takes a number of seconds representing a calendar time (e.g. as returned by sys.time) and returns the local time (relative to the locally set timezone) formatted according to the given format string.  Specific implementations may vary, but the format string should be interpreted as specified in ISO/IEC 9899:1999 (C programming language) clause 7.23.3.5.3 (strftime function format string) which includes the specification of these fields:</p>
<p>See also: sys.utctimef, sys.localtime, sys.time</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the local time formatted as requested.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set timestr sys.localtimef "%H~%M"</p>
<p>&gt; timestr (sys.time!)</p>
<p>"11~58"</p></td>
</tr>
</tbody>
</table>

### sys osfamily

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sys.osfamily</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Read only variable</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>Not a function</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns a string classifying the category of operating system running. Currently supported possible values are:</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Not a function.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sys osfamily</p>
<p>"linux"</p>
<p>set osfile[path]: {</p>
<p>    sep = if (equal sys.osfamily "windows"!) {"\\"}{"/"}!;</p>
<p>    join sep path!</p>
<p>}</p>
<p>&gt; osfile &lt;"dir","subdir","file"&gt;</p>
<p>"dir/subdir/file"</p></td>
</tr>
</tbody>
</table>

### sys run \<line\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sys.run</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;line:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Executes the system command held on the line.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>NULL if the execution was successful otherwise a string describing the error</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set rc sys.run "asfd"!</p>
<p>sh: asfd: command not found</p>
<p>&gt; eval rc</p>
<p>"ftl \$*console*:63: system command failed - \'asfd\' (rc 32512)\n"</p>
<p>&gt; sys run arp -a</p>
<p>? (10.17.20.254) at 00:06:5B:00:F9:60 [ether] on eth0</p></td>
</tr>
</tbody>
</table>

### sys time

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sys.time</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>No argument</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Provides a representation of calendar time as a number of seconds since an operating system-dependent initial epoch.<br />
See also: sys.localtime, sys.localtimef, sys.utctime, sys.utctimef</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An integer number of seconds.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sys localtimef "%X" (sys.time!)</p>
<p>"10:49:07"</p>
<p>&gt; set timeit[fn]: {</p>
<p>&gt;   start = sys.time!;</p>
<p>&gt;   fn!;</p>
<p>&gt;   0+(sys.time!)-(start)</p>
<p>&gt; }</p>
<p>&gt; timeit (sleep 2000)</p>
<p>2</p></td>
</tr>
</tbody>
</table>

### sys uid \<user\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sys.uid</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;user:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>return a unique identifier for the named user as an integer, or NULL if there is no such user.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>UID of a user.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sys uid gray</p>
<p>&gt; set isroot[id]: { 0 == (cmp id (sys.uid "root"!)!) }</p>
<p>&gt; isroot 500</p>
<p>FALSE</p>
<p>&gt; isroot 0</p>
<p>TRUE</p></td>
</tr>
</tbody>
</table>

### sys utctime \<time\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sys.utctime</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;time:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Takes a number of seconds representing a calendar time (e.g. as returned by sys.time) and breaks it down into components representing elements of the Coordinated Universal Time (UTC) in a returned directory with fields:</p>
<p>See also: sys.utctimef, sys.localtime, sys.time</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A directory with values for names sec, min, hour, mday, mon, year, wday, yday and isdst.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; sys utctime (sys.time!)</p>
<p>["sec"=41, "min"=47, "hour"=11, "mday"=21, "mon"=5, "year"=2006, </p>
<p>"wday"=3, "yday"=171, "isdst"=0]</p>
<p>set logrec[out, when, rec]:{</p>
<p>    t = sys.utctime when!;</p>
<p>    io.write out (octet 0+(t.year)-2000!)!;</p>
<p>    io.write out (octet t.mon!)!;</p>
<p>    io.write out (octet t.mday!)!;</p>
<p>    io.write out (octet t.hour!)!;</p>
<p>    io.write out (octet t.min!)!;</p>
<p>    io.write out (octet (len rec!)!)!;</p>
<p>    io.write out rec!;</p>
<p>}</p></td>
</tr>
</tbody>
</table>

### sys utctimef \<format\> \<time\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>sys.utctimef</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;format:string&gt; &lt;time:int&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a number of seconds representing a calendar time (e.g. as returned by sys.time) and returns the Coordinated Universal Time (UTC) formatted according to the given format string.  Specific implementations may vary, but the format string should be interpreted as specified in ISO/IEC 9899:1999 (C programming language) clause 7.23.3.5.3 (strftime function format string) which includes the specification of the fields used by sys.localtimef.<br />
See also: sys.localtimef, sys.utctime, sys.time</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A string containing the UTC time formatted as requested.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set stamp {sys.utctimef "%T%z" (sys.time!)!}</p>
<p>&gt; stamp</p>
<p>"00:06:52+0000"</p></td>
</tr>
</tbody>
</table>

### **tan** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>tan</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number of radians and returns its trigonometric tangent.<br />
The inverse function to atan.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value tan(x) (which is sin(x)/cos(x)).<br />
See also: sin, cos, asin, acos, atan, tanh</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (tan 0.0!)</p>
<p>0.000000000000</p>
<p>&gt; preal (tan PI/3!)**2</p>
<p>3.000000000000</p>
<p>&gt; preal (tan PI/4!)</p>
<p>1.000000000000</p>
<p>&gt; preal (tan PI/2!)</p>
<p>16331239353195370.000000000000</p>
<p>&gt; preal (tan (atan 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### **tanh** \<**real**\> 

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>tanh</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function (available only when the real type is supported.)</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;x:real&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Takes a real number and returns its hyperbolic tangent (sinh(x)/cosh(x)).<br />
The inverse function to atanh.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A real with the value tanh(x) (which is sinh(x)/cosh(x)).<br />
See also: sinh, cosh, asinh, acosh, atanh, tan</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set preal[x]:{echo (strf "%.12f" &lt;x&gt;!)!}</p>
<p>&gt; preal (tanh 0.0!)</p>
<p>0.000000000000</p>
<p>&gt; preal (tanh 1.0!)</p>
<p>0.761594155956</p>
<p>&gt; preal (tanh (-1.0)!)</p>
<p>-0.761594155956</p>
<p>&gt; preal (tanh (atanh 0.123!)!)</p>
<p>0.123000000000</p></td>
</tr>
</tbody>
</table>

### TRUE

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>TRUE</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Value</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>(no args)</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Provides a TRUE value which can be used in boolean expressions.<br />
Note that the value used for TRUE behaves as if it were defined as follows:</p>
<p>[code]:{code!}</p>
<p>See also: TRUE, the section about Boolean Values above.</p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An value that is not the one used for FALSE.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; set done FALSE</p>
<p>&gt; set once[fn]: { if (done) { done=TRUE; fn! } {}! } </p>
<p>{  rc = fn!;</p>
<p>    if (rc.error_occured) {echo "ERROR"!; FALSE} {TRUE}!</p>
<p>}</p></td>
</tr>
</tbody>
</table>

### type \<typename\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>type</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;typename&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns a type value of the named type.  Values of this type can be compared with each other.<br />
See also: typeof, typename </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A type value</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; type string</p>
<p>string</p>
<p>&gt; set isdir[x]: { 0 == (cmp (type{dir}!) (typeof x!)!) }</p></td>
</tr>
</tbody>
</table>

### typename \<expr\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>typename</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Command</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;expr:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns a string representing the type of the expression when it has been evaluated.<br />
See also: type, typeof </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A type value</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; typeof "string"</p>
<p>string</p>
<p>&gt; set isdir[x]: { 0 == (cmp "dir" (typename x!)!) }</p></td>
</tr>
</tbody>
</table>

### typeof \<expr\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>typename</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;expr:any&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td><p>Returns a type value representing the type that expr evaluates to.  When displayed these types display as the following:</p>
<p>See also: type </p></td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A type value</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; typeof typeof</p>
<p>closure</p>
<p>&gt; typeof []</p>
<p>dir</p>
<p>&gt; typeof {}</p>
<p>code</p>
<p>&gt; typeof ""</p>
<p>string</p>
<p>&gt; typeof 0</p>
<p>int</p>
<p>&gt; typeof NULL</p>
<p>nul</p>
<p>&gt; typeof (typeof NULL!)</p>
<p>type</p>
<p>&gt; eval str (typeof &lt;&gt;!)!</p>
<p>"dir"</p>
<p>set output [msg]: {</p>
<p>    if (equal (type "string"!) (typeof msg!)!) {</p>
<p>        echo msg!</p>
<p>    }{</p>
<p>        echo (str msg!)!</p>
<p>    }!</p>
<p>}</p>
<p>&gt; output "this"</p>
<p>this</p>
<p>&gt; output 34</p>
<p>34</p></td>
</tr>
</tbody>
</table>

### tolower \<string\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>tolower</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;chars:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns a string containing the (possibly multi-byte) characters in the argument string with any that have lower case equivalents replaced by it.  The characters replaced may be encoded in more than one octet of the string.  The number of octets in the string returned is not necessarily the same as those in the argument.<br />
See also: tolower </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An upper-case string</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; echo ${toupper "ægis - ProtechT10N"!}</p>
<p>ÆGIS - PROTECHT10N</p></td>
</tr>
</tbody>
</table>

### toupper \<string\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>toupper</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;chars:string&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Returns a string containing the (possibly multi-byte) characters in the argument string with any that have upper case equivalents replaced by it.  The characters replaced may be encoded in more than one octet of the string.  The number of octets in the string returned is not necessarily the same as those in the argument.<br />
See also: tolower </td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>An upper-case string</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; echo ${tolower "ÆGIS - ProTechT10N"!}</p>
<p>ægis - protecht10n</p></td>
</tr>
</tbody>
</table>

### while \<test\> \<do\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>while</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;test:code&gt; &lt;body:code&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>while &lt;test&gt; evaluates non-zero execute &lt;code&gt;.  Note that the test code is re-executed prior to each re-execution of the code and so it may refer to items that are updated by the code.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>Returns the value of the last &lt;code&gt; executed, or NULL otherwise.</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>set which[app]: {</p>
<p>    .path = split ":" sys.env.PATH!;</p>
<p>    .fname = NULL;</p>
<p>    .n = 0;</p>
<p>    while { 0 == (cmp NULL fname!) and (inenv path n!) } {</p>
<p>        .fn = ""+(path.(n))+"/"+(app);</p>
<p>        .out = io.file fn "r"!;</p>
<p>        n = 1+(n);</p>
<p>        if 0 == (cmp NULL out!) {} {</p>
<p>            fname = fn; io.close out!;</p>
<p>        }!</p>
<p>    }!;</p>
<p>    fname</p>
<p>}</p></td>
</tr>
</tbody>
</table>

### zip \<dir\> \<dir\>

<table>
<tbody>
<tr class="odd">
<td>Name</td>
<td>chop</td>
</tr>
<tr class="even">
<td>Kind</td>
<td>Function</td>
</tr>
<tr class="odd">
<td>Arg syntax</td>
<td>&lt;dom:dir&gt; &lt;range:dir&gt;</td>
</tr>
<tr class="even">
<td>Description</td>
<td>Normally takes two integer-indexed directories (vectors) and creates a directory with domain &lt;dom&gt; and range taken from values in &lt;range&gt;.  The values for the range aree taken sequentially from values in &lt;range&gt;.  If those values are exhausted &lt;range&gt; is repeated.  When &lt;range&gt; is a single value (instead of a vector) it is used for every range value. <br />
If the values in the &lt;dom&gt; vector are integers a new vector is created and if the values are strings a new named directory is created. <br />
Currently values in &lt;dom&gt; that are not the same type as the first element in &lt;dom&gt; are ignored.<br />
If there are no values in &lt;dom&gt; NULL is returned.</td>
</tr>
<tr class="odd">
<td>Returns</td>
<td>A vector of successive substrings taken from the string.<br />
See also: chop</td>
</tr>
<tr class="even">
<td>Example</td>
<td><p>&gt; zip &lt;0..6&gt; &lt;"even", "odd"&gt;</p>
<p>&lt;"even", "odd", "even", "odd", "even", "odd", "even"&gt;</p>
<p>&gt; zip &lt;3..0&gt; &lt;"abc", "def", "ghi", "jk"&gt;</p>
<p>&lt;"jk", "ghi", "def", "abc"&gt;</p>
<p>&gt; zip &lt;0..9&gt; 0</p>
<p>&lt;0, 0, 0, 0, 0, 0, 0, 0, 0, 0&gt;</p>
<p>&gt; zip &lt;"c","c#","d","d#","e","f","f#","g","g#","a","a#","b"&gt; &lt;0..11&gt;</p>
<p>&gt; [c=0, "c#"=1, d=2, "d#"=3, e=4, f=5, "f#"=6, g=7, "g#"=8, a=9, "a#"=10, b=11]</p>
<p>set invert[x]:{zip (range x!) (domain x!)!}</p>
<p>&gt; invert [c=0, "c#"=1, d=2, "d#"=3, e=4, f=5, "f#"=6, g=7, "g#"=8, a=9, "a#"=10, b=11]</p>
<p>&lt;"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"&gt;</p>
<p>&gt; invert [a=4,b=3,c=9,d=2]</p>
<p>&lt;2="d", "b", "a", 9="c"&gt;</p></td>
</tr>
</tbody>
</table>

## Implementation Notes

### Directories

A number of types of directory are supported the most important of which
are indexed and vector directories.  A vector directory is implemented
as a contiguous array with memory elements allocated for every index
between a minimum and a maximum entry.  Although some entries can be
unset this type of directory will not store sparsely indexed data very
well.   Vectors are generated when the '\<' ... '\>' syntax is used.  
  
Name-indexed directories are generated when the '\[' .. '\]' syntax is
used.  Currently they can only be indexed by a string.  This means that
a vector (which is indexed by number) can not be used to hold
name-indexed entries. 

### Garbage Collection

This implementation supports garbage collection of unreferenced values
and also creates garbage quite freely.  However it does not do garbage
collection on-demand (e.g. when storage runs low) it will only perform
garbage collection after the execution of a command line in command mode
- and then only at the outer-most level of stack of parser.exec calls.
 This means that recursive or looping FTL expressions can currently
generate garbage without limit.

### Significance of Braces

When parsing a block of text between ‘{‘ and ‘}’ there is no escape
mechanism to allow ‘}’ to be inserted into the text without a matching
‘{‘.
