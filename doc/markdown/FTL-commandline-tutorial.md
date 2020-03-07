# FTL Client Tool Command Line Interface

  

# Built-in Commands<span style="font-family: monospace;"></span>

<span style="font-family: monospace;"></span>It is a simple matter for
FTL-based tools to add a set of standard commands to those that it
defines.  For this reason most utilities will have the following
commands available.  

They are divided into groups with prefixes

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

Program control:

  - do \<do\> \<test\> - execute \<do\> while \<test\> evaluates
    non-zero
  - every \<n\> \<command\> - repeat command every \<n\> ms
  - exit - abandon all command inputs
  - forall \<env\> \<binding\> - execute \<binding\> for all \<env\>
    values
  - if \<n\> \<then-code\> \<else-code\> - execute \<then-code\> if
    \<n\>\!=0
  - return \<rc\> - abandon current command input returning \<rc\>
  - source \<filename\> - read from file \<filename\>
  - sourcetext \<stringexpr\> - read characters from string
  - while \<test\> \<do\> - while \<test\> evaluates non-zero execute
    \<do\>

General features:

  - cmd \<function\> \<help\> - create a command from a function
  - cmp \<expr\> \<expr\> - returns integer comparing its arguments
  - echo \<whole line\> - prints the line out
  - eval \<expr\> - evaluate expression
  - help - prints command information
  - rnd \<n\> - return random number less than \<n\>
  - set \<name\> \<expr\> - set value in environment
  - sleep \<n\> - sleep for \<n\> milliseconds

Types:

  - NULL - nul value
  - type \<typename\> - return the type with the given name
  - typeof \<expr\> - returns the type of \<expr\>

Booleans:

  - TRUE - the TRUE value
  - FALSE - an un-TRUE value
  - moreeq \<val\> \<val\> - TRUE if first \<val\> more than or equal to
    second
  - more \<val\> \<val\> - TRUE if first \<val\> more than second
  - lesseq \<val\> \<val\> - TRUE if first \<val\> less than or equal to
    second
  - less \<val\> \<val\> - TRUE if first \<val\> less than second
  - notequal \<val\> \<val\> - TRUE if first \<val\> not equal to second
  - equal \<val\> \<val\> - TRUE if first \<val\> equal to second
  - invert \<val\> - TRUE if \<val\> is FALSE, FALSE otherwise
  - logand \<val1\> \<val2\> - FALSE if \<val1\> is FALSE, \<val2\>
    otherwise
  - logor \<val1\> \<val2\> - TRUE if \<val1\> is TRUE, \<val2\>
    otherwise

Integers:

  - bitor \<n1\> \<n2\> - return n1 "or"ed with n2
  - bitxor \<n1\> \<n2\> - return n1 exclusive "or"ed with n2
  - bitand \<n1\> \<n2\> - return n1 "and"ed with n2
  - bitnot \<n\> - return the bitwise "not" of n
  - shiftl \<n1\> \<n2\> - return n1 left shifted by n2 bits
  - shiftr \<n1\> \<n2\> - return n1 right shifted by n2 bits
  - add \<n1\> \<n2\> - return n1 with n2 added
  - sub \<n1\> \<n2\> - return n1 with n2 subtracted
  - mul \<n1\> \<n2\> - return n1 multiplied by n2
  - div \<n1\> \<n2\> - return n1 divided by n2
  - neg \<n\> - return negated \<n\>
  - rnd \<n\> - return random number less than \<n\>
  - int \<integer expr\> - numeric value

Environments and directories:

  - domain \<env\> - generate vector of names in \<env\>
  - enter \<env\> - add commands from \<env\> to current environment
  - inenv \<env\> \<name\> - returns 0 unless string \<name\> is in
    \<env\>
  - len \[\<env\>|\<closure\>|\<string\>\] - number of elements in
    object
  - lock \<env\> - prevent new names being added to \<env\>
  - new \<env\> - copy all \<env\> values
  - range \<env\> - generate vector of values in \<env\>
  - restrict \<env\> - restrict further commands to those in \<env\>
  - select \<binding\> \<env\> - subset of \<env\> for which \<binding\>
    returns TRUE
  - sort \<env\> - sorted vector of values in \<env\>
  - sortby \<cmpfn\> \<env\> - sorted vector of values in \<env\> using
    \<cmpfn\>
  - sortdom \<env\> - sorted vector of names in \<env\>
  - sortdomby \<cmpfn\> \<env\> - sorted vector of names in \<env\>
    using \<cmpfn\>

Character handling:

  - collate \<str1\> \<str2\> - compare collating sequence of chars in
    strings
  - len \[\<env\>|\<closure\>|\<string\>\] - number of elements in
    object
  - joinchr \<delim\> \<str\> - join vector of chars and strings
    separated by \<delim\>s
  - join \<delim\> \<str\> - join vector of octets and strings separated
    by \<delim\>s
  - chr \<int\> - returns string of (multibyte) char with given ordinal
  - octet \<int\> - returns single byte string containing given octet
  - chrcode \<string\> - returns ordinal of the first character of
    string
  - octetcode \<string\> - returns ordinal of the first byte of string
  - split \<delim\> \<str\> - make vector of strings separated by
    \<delim\>s
  - str \<expr\> - evaluate expression and return string representation
  - strf \<fmt\> \<env\> - formatted string from values in \<env\>
  - strcoll \<str1\> \<str2\> - compare collating sequence of chars in
    strings
  - strlen \[\<string\>\] - number of (possibly multibyte) chars in
    string
  - tolower \<str\> - lower case version of string
  - toupper \<str\> - upper case version of string

Input and Output:

  - fmt - updateable directory of formatting functions for io.fprintf
  - io close \<stream\> - close stream
  - io err - default error stream
  - io file \<filename\> \<rw\> - return stream for opened file
  - io filetostring \<filename\> \[\<outfile\>\] - write file out as a C
    string
  - io getc \<stream\> - read the next character from the stream
  - io in - default input stream
  - io instring \<string\> \<rw\> - return stream for reading string
  - io out - default output stream
  - io outstring \<closure\> - apply output stream to closure and return
    string
  - io read \<stream\> \<size\> - read up to \<size\> bytes from stream
  - io stringify \<stream\> \<expr\> - write FTL representation to
    stream
  - io write \<stream\> \<string\> - write string to stream
  - io fprintf \<stream\> \<format\> \<env\> - write formatted string to
    stream

Parser interface:

  - parse argv \<subcommand\> - commands:
  - parse codeid - name of interpreter
  - parse env - return current invocation environment
  - parse errors - total number of errors encountered by parser
  - parse errors\_reset \<n\> - reset total number of errors to \<n\>
  - parse exec \<cmds\> \<stream\> - return value of executing initial
    \<cmds\> then stream
  - parse line - number of the line in the character source
  - parse newerror - register the occurrance of a new error
  - parse op - environment containing operation definitions
  - parse opeval \<opdefs\> \<code\> - execute code according to
    operator definitions
  - parse opset \<opdefs\> \<prec\> \<assoc\> \<name\> \<function\> -
    define an operator in opdefs
  - parse root - return current root environment
  - parse scan \<string\> - return parse object from string
  - parse scanned \<parseobj\> - return text remaining in parse object
  - parse scanempty \<parseobj\> - parse empty line from string from
    parse object, update string
  - parse scanwhite \<parseobj\> - parse white spce from string from
    parse object, update string
  - parse scanspace \<parseobj\> - parse over white space from string
    from parse object, update string
  - parse scanint \<@int\> \<parseobj\> - parse integer from string from
    parse object, update string
  - parse scanhex \<@int\> \<parseobj\> - parse hex string from parse
    object, update string
  - parse scanhexw \<width\> \<@int\> \<parseobj\> - parse hex in
    \<width\> chars from parse object, update string
  - parse scanstr \<@string\> \<parseobj\> - parse item until delimiter,
    update string
  - parse scanid \<@string\> \<parseobj\> - parse identifier, update
    string
  - parse scanitemstr \<@string\> \<parseobj\> - parse item or string,
    update string
  - parse scanitem \<delims\> \<@string\> \<parseobj\> - parse item
    until delimiter, update string
  - parse scanmatch \<dir\> \<@val\> \<parseobj\> - parse prefix in dir
    from string in parse object giving matching value, update string
  - parse source - name of the source of chars at start of the last line

Operating System interface:

  - sys env - system environment variable environment
  - sys osfamily - name of operating system type
  - sys run \<line\> - execute system \<line\>
  - sys uid \<user\> - return the UID of the named user
  - sys utctime \<time\> - broken down UTC time
  - sys localtime \<time\> - broken down local time
  - sys utctimef \<format\> \<time\> - formatted UTC time
  - sys localtimef \<format\> \<time\> - formatted local time
  - sys time - system calendar time in seconds

Windows-only interface:

  - reg value HKEY\_CLASSES\_ROOT \<key\> - open key values in
    HKEY\_CLASSES\_ROOT
  - reg value HKEY\_USERS \<key\> - open key values in HKEY\_USERS
  - reg value HKEY\_CURRENT\_USER \<key\> - open key values in
    HKEY\_CURRENT\_USER
  - reg value HKEY\_LOCAL\_MACHINE \<key\> - open key values in
    HKEY\_LOCAL\_MACHINE
  - reg value HKEY\_CURRENT\_CONFIG \<key\> - open key values in
    HKEY\_CURRENT\_CONFIG
  - reg allow\_edit \<boolean\> - enable/disable write to the registry
    from new keys
  - reg key - directory of key value types

The \<name\>s defined by `set` can be used in macro ($) expansion. For
example

``` 
    > set ip "10.20.128.33" 
    > echo My IP address is $ip 
    My IP address is 10.20.128.33 
 
    > set tick 4000 
    > every $tick echo Tick 
    Tick 
    Tick 
    Tick 
    : 
```

The name of the variable to be expanded can be delimited using '{' and
'}'. The system environment variables can be used inside these
delimiters as follows:

``` 
    > echo My IP address is ${ip} 
    My IP address is 10.20.128.33 
    > echo My computer is called ${sys.env.HOST} 
    My computer is called iolite 
```

Names with string values can also be used wherever a string expression
is required:

``` 
    > set hi "echo \"hello\"\n echo world\n" 
    > sourcetext hi 
    "hello" 
    world 
```

Both strings, numbers and bracketed expressions are simple FTL
expressions:

``` 
    > eval 0xff-(4*-3) 
    267 
    > eval hi 
    "echo \"hello\"\n echo world\n" 
    > eval ("IP address $ip") 
    "IP address 10.20.128.33" 
```

# FTL Expressions

The commands typed at the console are not FTL expressions. Nonetheless,
as is evident above, FTL expressions can be used in a number of places
and, if needed, can be used to create new commands to augment those
built-in using C.

The design of the FTL syntax is described in the FTL
specification[](http://www.google.com/url?q=http%3A%2F%2Fintranet%2F%257Ecgg%2FFTL-spec.html&sa=D&sntz=1&usg=AFrqEzdNmiyrY2MWcnXGBTj9F2Ca0AwidA).
It is a simple language with few potentially built-in constructs. A
complete user's guide to the Solarflare implementation can be found in
the FTL User's Guide.

The `eval` command always takes an FTL expression and executes it, so we
can use it to demonstrate how an expression is built up.

Perhaps the most querky aspect of it is the syntax for a function call
which involves a final "\!" to invoke it. For example the `rnd` command
is actually a function in FTL that can be invoked in an FTL expression:

``` 
    > eval rnd 5! 
    2 
```

Some of the built-in commands are FTL functions and others (`echo` and
`system`, for example) are FTL commands like `sleep` defined above. FTL
commands can be executed as part of an expression - they always take
exactly one argument which is a string (the string that would otherwise
have appeared as its argument on the command line). Thus `echo` can be
called in an FTL expression

``` 
    > eval echo "Hello world" ! 
    Hello world 
    > eval sleep "20" ! 
    > 
```

The syntax of a macro expansion is a little more involved than implied
above. The syntax "$\<name\>" provides the same expansion as
"${\<name\>}" but the latter syntax is actually an instance of the more
generally allowed form "${\<ftl-expr\>}". For example:

``` 
    > set rndip "${rnd 256!}.${rnd 256!}.${rnd 256!}.${rnd 256!}" 
    > eval rndip 
   "205.40.102.33" 
```

In FTL, program code and the "environment" in which it runs (the mapping
from symbols to values) are both first-class values that can be
manipulated and returned by functions. A function, itself, is an
association of an environment and a code value. The syntax used to
specify a code item; and then an environment, is illustrated here:

``` 
    > set printrnd { echo "random \$max gives \${rnd max!}"! } 
    > eval printrnd 
    { echo "random \$max gives \${rnd max!}"! } 
    > set env [max=10] 
    > eval env 
    ["max"=10] 
```

Code can be bound to an environment using the ':' operator, which can
then be stored as a value or executed

``` 
    > eval env:printrnd 
    ["max"=10]:{ echo "random \$max gives \${rnd max!}"! } 
    > set printrnd10 env:printrnd 
    > eval printrnd10 
    ["max"=10]:{ echo "random \$max gives \${rnd max!}"! } 
    > eval printrnd10! 
    random 10 gives 8 
```

Values of symbols in an environment can conveniently be accessed using
the '.' operator:

``` 
    > eval env.max 
    10 
    > eval printrnd10.max 
    10 
    > set env.max 15 
    > eval env.max 
    15 
```

The environment used in functions normally involves "unbound" variables
which must be substituted for an argument value before the resulting
closure can be executed.

``` 
    > set prnd [max]:{echo "random \$max gives \${rnd max!}"!} 
    > eval prnd 
    ["max"]:{echo "random \$max gives \${rnd max!}"!} 
    > eval prnd 10 
    ["max"=10]:{echo "random \$max gives \${rnd max!}"!} 
    > eval prnd 10! 
    random 10 gives 5 
    > eval (prnd 10).max 
    10 
```

A special environment `sys.env` is available giving access to system
environment variables:

``` 
    > eval sys.env.HOME 
    "/home/cgg" 
    > set sys.env.local "new envionment variable" 
    > eval sys.env.local 
    "new envionment variable" 
```

# Scripting

If you are a programmer you may find the following useful.  Otherwise a
more detailed description follows that.  

## Scripting in FTL Client Tools - Introduction for Programmers  

When a client tool uses the FTL library it will automatically support a
simple scripting environment.  
  
FTL is a third-party tiny language which is designed to supplement a
traditional command-line interface. You need to know one or two things
about the way it works before you can use it successfully.  
  
Each line of a script begins with an action name. This is the name of an
FTL function or command which is executed, or an FTL directory of
functions, commands or directories which are added to those available on
the rest of the command line.  
  
A series of FTL expressions follow the name of a function which are
evaluated and supplied to it as arguments.  
New functions can be created. They are written in FTL, not the script
language.  
  
Commands parse the command line and can be created from a function that
takes a single string argument.  
  
Directories are associations between names and other FTL values. Naming
one on the command line acts as a prefix that gives access to further
action names. It also makes the values in the directory available to
subsequent actions on the command line.  
  
Unlike most other languages FTL employs an operator ("\!") to require
the execution of a function. A function to which one or more arguments
are supplied is a 'closure' that will not be executed until "\!" is
applied. Such closures are first-class values in the language.  
  

| C-like                   | TCL-like              | FTL-like                  |
| ------------------------ | --------------------- | ------------------------- |
| fputs(stdout, "this\\n") | puts stdout "this\\n" | fputs stdout "this\\n" \! |

  
Also unlike most other languages the control structures of FTL are
simply functions. They take literal closures as arguments.  
  

<table>
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<thead>
<tr class="header">
<th>C-like</th>
<th>TCL-like</th>
<th>FTL-like</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td>if (success)<br />
{  rc = 0;<br />
} else {<br />
    rc = -1;<br />
}</td>
<td>if $success {<br />
   set rc 0<br />
} else {<br />
   set rc -1<br />
}<br />
</td>
<td>if success {<br />
   rc = 0;<br />
} {<br />
   rc = -1;<br />
}!</td>
</tr>
</tbody>
</table>

  
The arguments to 'if' are  

  - success - a boolean value  
  - { rc = 0; } - a closure value  
  - { rc = -1; } - a closure value

<table>
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<thead>
<tr class="header">
<th>C-like</th>
<th>TCL-like</th>
<th>FTL-like</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td>for (i=0; i&lt;20; i++) {<br />
   sum += i;<br />
}<br />
</td>
<td>for { set i 0 } {$i &lt;= 20 } { incr i } {<br />
   set sum [expr {$sum + 1}]<br />
}</td>
<td>forall &lt;0..19&gt; [i]:{<br />
   sum = sum + i;<br />
}!</td>
</tr>
</tbody>
</table>

  
The arguments to 'forall' are  

  - \<0..19\> - an array of the 20 numbers from 0 to 19 inclusive  
  - \[i\]:{sum = sum + i;} - a closure taking an argument used for each
    item in the array

Control functions supported include:  

  - forall \<env\> \<binding\> - execute \<binding\> for all \<env\>
    values
  - if \<n\> \<then-code\> \<else-code\> - execute \<then-code\> if
    \<n\>\!=FALSE  
  - while \<test\> \<do\> - while \<test\> evaluates non-FALSE execute
    \<do\>  
  - do \<do\> \<test\> - execute \<do\> while \<test\> evaluates to
    non-FALSE

Boolean values are either TRUE or FALSE, both of which are functions
requiring a single closure as an argument. TRUE will execute its
argument and FALSE will ignore it.  
  

<table>
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<thead>
<tr class="header">
<th>C-like</th>
<th>TCL-like</th>
<th>FTL-like</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td>if (success)<br />
{ fputs(stdout, "this\n");<br />
}</td>
<td>if $success {<br />
   puts "this\n"<br />
}<br />
</td>
<td>success {<br />
   fputs stdout "this\n" !;<br />
}</td>
</tr>
</tbody>
</table>

  
  
FTL does support a small but extensible set of operators, such as ==,
\!=, +, -, /, \*, \!, not etc. They have associated FTL functions which
are executed immediately (rather than requiring "\!").  
  
All FTL statements return a value, but that value might be NULL. ';' is
an operator that discards the value of the left hand operand.  
  
The 'set' command is equivalent to the assignment operator (=) in FTL.
It assigns values to names in the current environment.  
  
Environments are the other major type of value in the language and, as
well as being implicitly associated with a closure, they occupy the same
position in the language as array and structures or dictionaries in
other languages. The only two kinds of user-created environment are
those indexed by number (vectors), and those indexed by string
(directories).  
  

<table>
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<thead>
<tr class="header">
<th>C-like</th>
<th>TCL-like</th>
<th>FTL-like</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td>const char *names[] = { "harry", "sally" };<br />
<br />
struct {<br />
   int number;<br />
   const char *name;<br />
} shopping = { 3, "bananas" };<br />
<br />
# example use:<br />
static void show(void) {<br />
   puts(names[0]);<br />
   puts(shopping.name);<br />
}<br />
</td>
<td>set names [list "harry" "sally"]<br />
<br />
array set shopping [list \<br />
                                 {number} {3} \<br />
                                 {name} {bananas} \<br />
                             ]<br />
<br />
# example use:<br />
proc show {} {<br />
   puts [lindex $names 0]<br />
   puts shopping(name)<br />
}<br />
</td>
<td>names = &lt;"harry", "sally"&gt; # (a vector)<br />
<br />
shopping = [<br />
         number = 3;<br />
         name = "bananas";<br />
] # (a directory)<br />
<br />
# example use:<br />
show = []:{<br />
   puts names.0!;<br />
   puts shopping.name!;<br />
}<br />
</td>
</tr>
</tbody>
</table>

  
                                                                       
Basic values can be integers (64-bit) or strings (which can be used both
as binary containers or as UTF-8 character lists). There is no floating
point number support. The only structure is an 'environment' but this
comes in two forms: one where every value is named by an integer
(enclosed by '\<' and '\>'); and one where every value is named by a
string (enclosed by '\[' and '\]'). (However the :: operator can be used
to concatenate environments into one containing both kinds of name.)  
  
A closure can be constructed with the ':' operator which takes an
environment, normally with unbound variables, as its left argument and
"code" as its right. This creates a function, which is typically
assigned to a variable. For example:  

    add = [a, b]:{ a+b }

Normally the 'set' command is used to perform this assignment in a
script  
  

<table>
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<thead>
<tr class="header">
<th>C-like</th>
<th>TCL-like</th>
<th>FTL-like</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><pre><code>static int add(int a, int b)
{  return a + b;
</code></pre></td>
<td><pre><code>proc add {a b} {
   return [expr $a + $b]
}</code></pre></td>
<td><pre><code>set add [a, b]:{
   a + b
}</code></pre></td>
</tr>
</tbody>
</table>

  
The value assigned to 'add' is \[a, b\]:{a+b} - a closure value  

## Scripting in more Detail  

There are two approaches to scripting supported.

1.  string values can be defined and expanded
2.  compound FTL expressions can be defined and executed

In both of these the way in which lines are built by the library is
relevant. In addition to concatenating physical lines ending with '\\'
and splitting the result into lines separated by ';' the library will
incorporate line endings into logical lines that are contained within
bracketed structures. The brackets include:

  - '(' and ')' - used to group sub-expressions
  - '\[' and '\]' - used to define identifier environments
  - '\<' and '\>' - used to define vector environment
  - '{' and '}' - used to define code
  - '"' and a corresponding closing '"' - used to define strings

Very long "lines" can be generated by incorporating these structures.

(Note: currently the prompt provided whilst awaiting a closing bracket
does not vary from the normal prompt - this can be a source of
confusion.)

## String scripts

The `sourcetext` command can take an FTL expression evaluating to a
string and use it as literal input to the parser. For example:

``` 
    > sourcetext "rnd 10\n" 
    7 
    > set prog " 
    > echo first command 
    > echo second command 
    > " 
    > sourcetext prog 
    first command 
    second command 
```

It is possible, therefore, to construct simple scripts as a string,
perhaps involving the expansion of variables via the idom '\\$variable'.

The next characters read by the parser are taken from the string given
to *sourcetext* - beware that this can give confusing results if the
text does not incorporate a new line:

``` 
    > sourcetext "echo " 
    > set nextthing 55 
    set nextthing 55 
```

Such scripts can also be placed in files and then executed using the
*source* command, which simply takes the file name as its argument.

## FTL scripts

As described above, '{' and '} can be used to bracket an FTL expression
that can later be executed. This expression can make use of the ';'
operator which discards the value of its left-hand argument and returns
the value of its right-hand one.

``` 
    > eval { rnd 10!; 500 } ! 
    500 
    > eval { 500; rnd 10! } ! 
    3 
    > eval { 500; rnd 10!; } ! 
    > 
```

Simple multi-line scripts can be constructed simply by assigning code to
a variable that is later executed.

``` 
    > set prog { 
    >    echo "first command"!; 
    >    echo "second command"! 
    > } 
    > eval prog! 
    first command 
    second command 
```

As illustrated the code can either be executed as part of an FTL
expression using *eval*.

Functions can be defined as closures in the way implied above:

``` 
    > set fn [msg]: { 
    >    echo "The message follows..."!; 
    >    echo msg!; 
    > } 
    > eval fn "hello world"! 
    The message follows... 
    hello world 
```

There are a small number of control primitives built-in including `if`,
`while`, `do` and `forall`. Each of these are implemented as closures
that take FTL values are arguments.

`if` (always) takes three arguments - the first being an integer value
and the second and third being code values. As you may expect the second
code value is executed if the integer value is zero and the first is
executed otherwise. Both code values must be supplied - the empty code
value "{}" can be provided if necessary. Integer expressions can contain
the following operators:

|     |                       |
| --- | --------------------- |
| \== | equal                 |
| \!= | not equal             |
| le  | less than or equal    |
| lt  | less than             |
| ge  | greater than or equal |
| gt  | greater than          |

(integer expressions are not properly built in to the language, these
are infix operators with no equivalent closures and FTL expressions can
only be incorporated in integer expressions using syntax such as
"+(\<FTL expression\>)". Note, also, that the `cmp` function will
compare values from arbitrary types - including strings.)

``` 
   > set myfor [n, code]:{ 
   >    if (0 lt (n)) {  
   >        code!; myfor (-1+(n)) code!  
   >    } {}! 
   > } 
   > eval myfor 3 { echo "hello"! }! 
   hello 
   hello 
   hello  
   > 
```

`if` returns the value returned by the executed code:

``` 
    > if 0 == (cmp "this" "that"!) { "yes" } { "no" } 
    "no" 
```

A "for" loop need not be constructed in the "myfor" example above
because a function is available that implement it. The `forall` command
takes two arguments - the first an environment to ennumerate and the
second some code to execute for each element in the environment. Perhaps
the most useful type of environment to use is a vector series. Vectors
are environments in which numeric names are assigned numeric values, and
series are vectors that are defined by a simple rule.

``` 
    > eval <42, 50, 11> 
    <42, 50, 11> 
    > eval <42, 50, 11>.1 
    50 
    > eval <4..9> 
    <4 .. 9> 
    > range <4..9> 
    <4, 5, 6, 7, 8, 9> 
    > range <..4> 
    <1, 2, 3, 4> 
    > range <2, 4 .. 10> 
    <2, 4, 6, 8, 10> 
    > forall <..3> { echo "Again!"! } 
    Again! 
    Again! 
    Again! 
```

The code argument to `forall` can be a code value (as above) or a
closure with one or two arguments. If a closure is used the first
argument is bound to the value in the environment for which the code is
executed, and the second is the name that was looked up in the
environment to provide it. (Note that there is no *defined* ordering in
which the name-value pairs chosen in an environment, unless it is a
vector.)

``` 
    > forall <..3> [i]:{ echo "val \$i"! } 
    val 1 
    val 2 
    val 3 
    > forall [first=1, second=2] [val]:{ echo "val \$val"! } 
    val 1 
    val 2 
    > forall [first="one", second=2] [val,name]:{ echo "\$name is \$val"! } 
    first is one 
    second is 2 
```

Other loop constructs available include `do` and `while`.
`do {code} {test} ` and `while {test} {code} ` will execute the code
while `test` evaluates non-zero. The difference between the constructs
is simply that the test is executed before the code in the case of
`while` and after it in the case of `do`.

``` 
    > set nosix_run[]: { 
        count = 0; 
        do {n = rnd 6!;  
            count = 1+(count);   
            count  
        } { 0 != (n) }! 
    } 
    > nosix_run 
    21 
    > nosix_run 
    3 
    > nosix_run 
    5 
    > 
    > set which[app]: { 
        path = split ":" sys.env.PATH!; 
        fname = NULL; 
        n = 0; 
        while { 0 == (cmp NULL fname!) and (inenv path n!) } { 
            fn = ""+(path.(n))+"/"+(app); 
            out = io.file fn "r"!; 
            n = 1+(n); 
            if 0 == (cmp NULL out!) {} { 
                fname = fn; io.close out!; 
            }! 
        }!; 
        fname 
    } 
    > which "echo" 
    "/bin/echo" 
    > which "ftl" 
    "/home/cgg/tree/clean/v5/build/gnu/tools/ftl/ftl" 
```

## Script Extension using FTL

### Adding Built-in Commands

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

### Adding Commands from a Script

The above examples name built-in functions as the first item on the
command line. It is possible to define additional commands simply by
setting a code body or a closure to the name of a new command. The
examples above will have defined new scripting commands illustrated as
follows:

``` 
    > set prog { 
    >    echo "first command"!; 
    >    echo "second command"! 
    > } 
    > prog 
    first command 
    second command 
    > 
    > set fn [msg]: { 
    >    echo "The message follows..."!; 
    >    echo msg!; 
    > } 
    > fn "Hello again" 
    The message follows... 
    Hello again 
```

The `set` command normally defines objects in the "root" name directory,
but other directories can be created and used. If the first name on a
command line is the name of such a directory then the rest of the line
can use use those names as commands (in addition to the root commands).
(This is the mechanism described above in Simple Use - Subcommands.) For
example,

``` 
    > set prefix [version=[str="Ver. 1.00", n=100], random = [msg]:{echo "\$msg: \${rnd 10!}"!}] 
    > eval prefix.version.str 
    "Ver. 1.00" 
    > eval prefix.random "Number"! 
    Number: 7 
    > prefix random "Random" 
    Random: 3 
    > prefix version n 
    100 
    > prefix version eval echo str! 
    Ver. 1.00 
```

### Adding Command Help

The `help` command will include any such new commands automatically:

``` 
    > help 
    prefix <subcommand> - commands: 
        random - closure value 
        version <subcommand> - commands: 
            n - int value 
            str - string value 
    : 
    : 
or 
    > prefix help all 
    random - closure value 
    version <subcommand> - commands: 
        n - int value 
        str - string value 
 
```

It is possible to include your own one-line help string for closures by
defining a "\_help" text.

``` 
    > set prefix.random [_help="<msg> - show random number with message", msg]:{echo "\$msg: \${rnd 10!}"!} 
    > prefix help 
    random <msg> - show random number with message 
    version <subcommand> - commands: 
       n - int value 
       str - string value 
```

This is, in fact, how the help text for the normal commands is provided:

``` 
    > eval echo 
    ["_help"="<whole line> - prints the line out", "_1"]::<cmd:0x8056ff0,1> 
```

### Restricting Available Commands

Once a set of C programmed commands have been extended with FTL-based
ones the range of syntax accepted by the parser may be too great. It is
likely, for example, that many of the built-in commands would better be
faulted than accepted in scripts. The `restrict` command is available to
limit successive commands to those in its argument root directory.

This illustrates the use of FTL to set up a small interpreter for the
parsing of a simple file that defines events.

    > set events [] 
    > set this_event NULL 
    > set default [date="some date", allday=0] 
    > 
    > set ev_new [_help="<name> - define a new event", name]: { 
    >     events.(name) = new default!; 
    >     this_event = name; 
    > } 
    > set ev_date [d]: {events.(this_event).date = d; } 
    > set ev_allday []: {events.(this_event).allday = 1; } 
    > set ev_end [_help="- finish event definition"]: {this_event=NULL;} 
    > set ev_print []:{ 
    >     forall events [ev, name]:{ 
    >         echo "\$name on \${ev.date} \${<\"\", \"all day\">.(ev.allday)}"!; 
    >     }! 
    > } 
    > 
    > restrict [event=ev_new, date=ev_date, 
    >           allday=ev_allday, end=ev_end, print=ev_print, 
    >           include=source, help=help, exit=exit] 

The file to be parsed can then be `include`d or typed in on the command
line. For example if the file "diary" contains the lines

    event "Christmas" 
    date "25 Dec" 
    allday 
    end 
     
    event "Dentist" 
    date "27 Nov" 
    end 
     
    event "Manjana" 
    allday 
    end 

then it is possible to execute the following:

    > include "diary" 
    > print 
    Christmas on 25 Dec all day 
    Dentist on 27 Nov 
    Manjana on some date all day 

More notable, however is what is not possible:

    > help 
    exit - abandon all command inputs 
    help - prints command information 
    include <stringexpr> - execute file <stringexpr> 
    print - closure value 
    end - finish event definition 
    allday - closure value 
    date - closure value 
    event <name> - define a new event 
    > 
    > eval events 
    ftl $*console* line 39: unknown command 'eval events' 
    > 
    > set events."mynew event" [datestr="wrong", allday=55] 
    ftl $*console* line 41: unknown command 'set events."mynew event" [datestr="wrong", allday=55]' 

The commands other than those explicitly adopted in the `restrict`
command are not available, and can not be used, for example, to
interfere with implementation details.

### Restricting Available Syntax

In the example above the line

    ev_new "Christmas" 

evaluates `"Christmas"` as an FTL-expression which returns a string and
then calls the `ev_new` function with the string as its argument.
Because an expression is parsed, a number of other possibilities exist
that also deliver a string. This may, or may not, be considered a good
thing. It offers the person generating the script a degree of additional
flexibility, but also offers the possibility of a number of
side-effects. For example, the rather bizzar

    ev_new "Christmas"+({ev_new "NewYear"!; "Day"}!) 

Will make a "ChristmasDay" event but will - as a side effect - start a
"NewYear" event too.

Only functions (such as `ev_new` in the above) accept FTL expressions in
this way. Commands parse the command line explicitly, and therefore have
much more control over what is acceptable.

The `cmd` function can be used to convert an FTL expression, which takes
the command line as a single argument, into a new command. For example:

    > set evc_new cmd ev_new "<restofline> - give name of an event"! 
    > evc_new Christmas 
    > : 
    > 
    > restrict [event=evc_new, date=(cmd ev_date "<rol> - give event date"!), 
    >           allday=ev_allday, end=ev_end, print=ev_print, 
    >           include=source, help=help, exit=exit] 

## Control Information available to Scripts

Command line interfaces can be controlled using information obtained
from a number of sources including:

  - the program's command line arguments
  - the program environment
  - a file
  - the registry (on Windows only)

they access these sources of information through FTL.

### Information from Command Line Arguments

The example of the library's use above includes the following line used
to initialize obtain access to generic commands:

``` 
    char *app_argv[APP_ARGC_MAX]; 
    int app_argc = 0; 
    :     
    if (parse_args(argc, argv, .. &init, &app_argc, &app_argv, APP_ARGC_MAX)) 
    : 
            cmds_generic(state, app_argc, app_argv); 
```

A function such as `parse_args` above should be provided in such a way
as to pass on a vector of arguments to `cmds_generic`. Typically it
might contain all command line elements following a `"--"` command line
argument.

If this has been done then the `parse.argv` built-in directory can be
used to read these values. For example,

``` 
    % ftl -- -one two "three" 
    : 
    > eval parse.argv 
    [0="ftl", 1="-one", 2="two", 3="three"] 
    > set argc len parse.argv! 
    > eval argc 
    4 
    > set args join " " parse.argv.<1..(argc)>! 
    > eval args 
    "-one two three" 
    > eval parse.argv.<1,2>.[name=0, value=1] 
    ["name"="-one", "value"="two"] 
```

### Information from the Environment

Environment variables can be read (and written) using the built-in
directory `sys.env` function. For example,

``` 
    > sys env USER 
    "gray" 
    > set sys.env.USER "unhelpful" 
    > eval sys.env.USER 
    "unhelpful" 
    > if (inenv sys.env "OPTIONAL"!) {} { sys.env.OPTIONAL = "default"; } 
    > eval sys.env.OPTIONAL 
    "default" 
    > sys run env 
    : 
    OPTIONAL=default 
    : 
    USER=unhelpful 
    : 
```

### Information from Files

We have already described abvoe how an interpreter can be set up to
parse commands from a file. It is also possible to read (and write) to
both character-based and binrary files directly. A number of `io`
functions can be used to access files. `io.file` opens a file,
`io.close` closes it and `io.read` reads a number of bytes from it.

``` 
    > set file io.file "/etc/passwd" "r"! 
    > set content io.read file 10000! 
    > eval io.close file! 
    > # we have the whole file in "content" now 
 
    > set line split "\n" content!  # break it down into lines 
    > eval line.0 
    "root:x:0:0:root:/root:/bin/bash" 
    > eval len line! 
    38 
 
    > split ":" line.0 
    <"root", "x", "0", "0", "root", "/root", "/bin/bash"> 
     
    > # you can use indexing to give more meaning to this kind of breakdown 
    > eval (split ":" line.0!).[name=0, dir=5] 
    ["name"="root", "dir"="/root"] 
 
    > # lets make our own password list.. 
    > set pw [] 
    > forall line [ln]:{rec = (split ":" ln!); pw.(rec.0) = rec.[dir=5, shell=6];} 
    > eval pw.ftp 
    ["dir"="/var/ftp", "shell"="/sbin/nologin"] 
```

### Information from the Windows Registry

On Windows `reg` functions are available that allow the Windows register
to be read. Values from registry keys can be looked up using any of the
following functions

``` 
    reg.value.HKEY_CLASSES_ROOT <key> - open key in HKEY_CLASSES_ROOT 
    reg.value.HKEY_USERS <key> - open key in HKEY_USERS 
    reg.value.HKEY_CURRENT_USER <key> - open key in HKEY_CURRENT_USER 
    reg.value.HKEY_LOCAL_MACHINE <key> - open key in HKEY_LOCAL_MACHINE 
    reg.value.HKEY_CURRENT_CONFIG <key> - open key in HKEY_CURRENT_CONFIG 
```

These values can then be used as normal FTL directories. For example,

``` 
    > set key reg.value.HKEY_CURRENT_USER "Identities"! 
    > forall (domain key!) echo 
    Identity Ordinal 
    Migrated5 
    Last Username 
    Last User ID 
    Identity Login 
    Default User ID 
    > eval key."Last User ID" 
    > eval key."Last User ID" 
    ["type"=1, "data"="{CA01A243-73B8-488B-9AF5-CAB230636DB9}"] 
    > set i reg.value.HKEY_CURRENT_USER "Identities\\"+(key."Default User ID".data)! 
    > forall i [v,n]:{ io.fprintf io.out "%15s: %v\n" <n, v.data>!; } 
           Username: "Main Identity" 
            User ID: "{CA01A243-73B8-488B-9AF5-CAB230636DB9}" 
     Directory Name: 3389104707 
```

## Testing pure FTL scripts

A utility providing only the generic FTL commands **ftl** is available.
It can be built in **.../ftl** either on Linux or on Windows. Before
building on Linux, you can set **export USEREADLINE=1** in your
environment to get better command line editing.

On Windows initial commands will be read from the file **$HOME\\ftl.rc**
and on Linux they will be read from **$HOME/.ftl/ftl.rc** if these files
exist.

The following command line options can be provided:

``` 
  ftl [-r <randseed>] [-e|-ne] [-c <commands> | [-f] <cmdfile>] [[--] <script arg>...] 
 
     [-f] <cmdfile>     - read commands from this file instead of the console 
     -c "cmd;cmd;..."   - execute initial commands 
     -r <n>             - set random seed 
     -[n]e | --[no]echo - [don't] echo executed commands 
     -q | --quiet       - don't report unnecessary info 
```

Script arguments will be available through the *parse.argv* function in
the script.

If you have `ftl` on your current PATH you can make an executable ftl
script file by using the following first line:

``` 
    #!/usr/bin/env ftl 
```

If you use a number of different build trees it is possible to use the
`eftool` script to place your "current" tree's version of `ftl` on your
path. The procedure to do this is as follows.

``` 
    % cd <directory you own on your PATH>  
    % cp <path to "v5" in any of your trees>/scripts/eftool eftool  
    % ln -s eftool ftl  
```
