# FTL

## Language Design

The cornerstone construct in FTL is a closure, which can be specified
using the following syntax (which will be extended below)

``` western
<closure> ::= '[' <symbol> ']' '::' <code-expression>
```

where \<code\> is a normal expression that evaluates to a code value,
which could be a literal value such as

``` western
<code> ::= '{' <code-item>* '}'
```

The \<code-items\> in the code primarily consist of symbols whose
meaning is determined by the environment in which the code is executed.
 FTL implements an environment as an ordered set of
"bound" symbol-value pairs in which a value can be found from a given
symbol; and an "unbound" symbol which has not yet been assigned a
value.  
When an argument is applied to a closure a substitution of the unbound
symbol with the value given by \<expression\> is effected:

``` western
<substitution> ::= <closure> <expression>
```

in which the resulting closure value \<substitution\> incorporates an
environment in which the unbound \<symbol\> in the \<closure\> is paired
with the value of the \<expression\>.  When the resulting closure is
subsequently invoked

``` western
<invocation> := <closure-expression> '!'
```

the code of the closure is interpreted in this environment so that the
bound values are available within the code.  
  
So, conceptually a closure value is composed of three parts

1.  an ordered environment stack in which the most recently entered
    symbol values take priority over former values for the same symbol
    when they are sought

2.  an optional unbound symbol

3.  a code value

As a generalization of the simple \<closure\> syntax above the part of
the syntax before '::'

``` western
'[' <symbol> ']'
```

is treated as a literal environment that may be generalized to an
\<environment-expression\> - the '::' being an operator that binds an
environment to some code, or - more generally - extends the environment
in an existing closure with the environment given.  
  
It is often the case that the execution environment at the point of
closure definition is required to be part of the closure's own
environment.  This can be achieved by using the ':' operator in place of
'::'.  That is, a closure formed using

``` western
<environment-expression> '::' <code-expression>
```

creates a closure with whose environment is exactly what is specified in
the \<environment-expression\>, but

``` western
<environment-expression> ':' <code-expression>
```

also includes the current environment at the point of definition plus
the environment in \<environment-expression\>.  
  
Thus the full syntax for a closure expression is:

``` western
<closure> ::= (<environment-expression> [ '::' | ':' ])* <code-expression>
```

where either expression may involve literals, brackets, substitution,
invocation etc.  One option for an \<environment-expression\> is a
\<closure\>, which, when used in this way refers only to the environment
part of the closure.

## <span id="mozTocId409702"></span>Syntactic Sugar

The above specification is sufficient to provide all the language's,
typing, data structuring and code control primitives as well as its
object-orientated features.  However in order to reduce the complexity
of the code there is syntax to simplify certain idioms.

### <span id="mozTocId280832"></span>Creating Bindings

This syntax

``` western
 '[' <symbol2> ']' '::' '[' <symbol1> ']' '::' <code-expression>
```

(in which '::' binds more tightly to the right) can be replaced by:

``` western
'[' <symbol1>, <symbol2> ']' '::' <code-expression>
```

and in general any number of unbound symbols can be placed in an
environment.  Note, that values associated with \<symbol2\> take
precedence over those of \<symbol1\> when both symbols have the same
name.  The first substitution applied to these expressions will bind
\<symbol1\>.  
  
This syntax

``` western
 '[' <symbol> ']' '::' <code-expression> <value>
```

can be replaced by:

``` western
'[' <symbol> '=' <value> ']' '::' <code-expression>
```

and in general

``` western
'[' <sym_m> ']' '::' ... '::' '[' <sym_1> ']''::' <code-expression> <val_1> ... <val_n>
```

  
can be replaced by

``` western
'[' <sym_1> '=' <val_1> ',' ... ',' <sym_n> = <val_n> ',' ... <sym_m> ']' '::' <code-expression>
```

Note that each \<val\_i\> is still evaluated in the environment outside
the closure (in which none of the \<sym\_i\> symbols have been set).  
The symbols with an associated '=' will be bound symbols, and those
without will be unbound.  
  
In addition this environment syntax

``` western
'[' <sym_1> '=' <val_1> ',' ... ',' <sym_n> = <val_n> ',' ... <sym_m> ']'
```

specifies a first-class element in the language that can be used as a
value anywhere.  Note that, to simplify implementation, no bound symbols
can be specified following the first unbound symbol in the
specification.

### <span id="mozTocId807267"></span>Obtaining Bound Values in Closures

This syntax

``` western
<closure> '::' '{' <symbol> '}' '!' 
```

(which simply expands to the value of \<symbol\> in \<closure\>’s
environment) can be replaced by

``` western
<closure> '.' <value>
```

Currently \<value\>s are constrained to be either integers or strings.
For the sake of convenience strings can be provided as symbols (with no
associated quotes) if they have the syntax of an identifier.

In addition the following syntax can be used to create a symbol value
from an expression

``` western
'<closure> '.' '(' <expression> ')'
```

where, for example, an expression returning an integer or a string is
converted to a symbol before use.

### <span id="mozTocId495578"></span>Integers

Although it would theoretically be possible to create an environment
defining all the values for 64 bit integers:

``` western
{ int = { 0=<0 value>, 1=<1 value>, .... 9223372036854775807=<9223372036854775807 value> }:
[ int: [.... main body of program ....] ] 
!
```

this step is assumed as part of the runtime library - thus all numeric
symbols are interpreted with their expected values (unless overridden by
higher priority definitions). 

### <span id="mozTocId541092"></span>Vectors

This syntax

``` western
[ 0=expr0, 1=expr1, ... N=exprN ]
```

can be replaced by

``` western
< expr0, expr1, ... exprN >
```

or ,where not all of the indices are consecutive, this may be broken
into subseries

``` western
< <int_i>=expr_i, expr_i+1, ..., <int_j>=expr_j, expr_j+1..., ... >
```

i.e. using the general syntax

``` western
'<' [ [<integer>=]<expression> [',' [<integer>=]<expression> ]* ] '>' 
```

  
Vectors that are simple numeric series can be represented

``` western
'<' [<expression-int_first> [ <expression-int_second> ]] '..' [ <expression-int_last> ] '>'
```

where, if \<expression-int\_second\> is missing an integer one greater
than \<expression-int\_first\> is used, and if \<expression-int\_first\>
is missing the number 1 is used.   Thus, there would be the 42 numbers
from one to forty-two indexed by the integers 0 to 41 in

``` western
<..42>
```

so that the following would have value 10

``` western
<..42>.11
```

### Code Concatenation

The infix operator ";" is available in a code object to ignore its
left-hand value and replace it with its right (if there is one).  Thus
the value of

``` western
<expression 1> ; <expression 2> ; ... ; <expression n>
```

is \<expression n\>.  And the value of

``` western
<expression 1> ; <expression 2> ; ... ;
```

is the special empty value "NULL" (which is therefore a name for the
value of "{}\!")

### Boolean Expressions

The standard environment in FTL includes two "built-in" values "TRUE"
and "FALSE".  In principle they are implemented as if they had been
defined:

``` western
TRUE = [val]::{val!}
FALSE = [val]::{FALSE}
```

That is, TRUE is a closure that will execute its argument and FALSE is a
closure that will discard its argument and return FALSE instead.  Thus
if a value that is either TRUE or FALSE is given code as an argument:

``` western
<true_or_false> { <code> }!
```

the result will be either the result of the code's execution or the
value FALSE.  Thus the code is conditionally executed. 

## <span id="mozTocId347665"></span>Syntactic Details

The format of the source file is not specified since it is normal for
FTL expressions to be embedded in an external application.  Typically
such an application will apply such formatting conventions as:

  - discard of commented out lines (e.g. those beginning '\#')

  - concatenation of lines ending with an escape (e.g. '\\')

  - expansion of textual substitutions (e.g. base on FTL expressions)
    introduced with an expansion escape (e.g. '$')

The characters '\#', '\\' and '$' are not used as part of FTL syntax.

``` western
<code-item> ::= <symbol> | <string> |
                '[' | ']' | '{' | '}' | '<' | '>' | '(' | ')' |
                '!' | '&' | '~' | '@' | 
                ';' | ',' | ':' | '.'
<symbol> ::= <integer> | <identifier>
<integer> ::= <digit>+
<identifier> ::= <common_identifier> | <builtin_identifier>
<common_identifier> ::= <alphabetic> (<alphabetic> | '_' | <digit>)*
<builtin_identifier> ::= '_' <digit>+ 
```

The precedence of the various operators outlined above is specified as
part of the syntax for an expression:

``` western
<expression> ::= (<index> '=')* <invocation>
<invocation> ::= (<substitution> ['!']*)+ 
<substitution> ::= <retrieval>+
<retrieval> ::= <closure>['.' <index>]*
<closure> ::= (<base> [':' | '::'])* <base>
<base> ::= '(' <expression> ')' |
           <code> |
           <id_environment> |
           <identifier> |
           <vector> |
           <type_literal>
           
<code> ::= '{' <expression_list> '}'
<expression_list> ::= <expression> (';' <expression>)* [';']
<id_environment> ::= '[' [<binding_list> | <binding_list> ',' <unbound_list> | <unbound_list>] ']' 
<binding_list> ::= [<binding> (',' <binding>)*]  
<unbound_list> ::= <index> (',' <index>)*
<binding> ::= <index> '=' <invocation><vector> ::= '<' <implied_series> | <series> '>'
<index> ::= <identifier> | <type_literal>
<type_literal> ::= <integer> |
                   <string> 
<series> ::= <expression>*
<implied_series> ::= [<expression> [<expression>]] '..' [<expression>]
```

## Compilation and Semantics

The semantics of the language can partly be specified by describing how
an expression that is an equivalent canonical representation of a code
value can be obtained (i.e. how a code value can be "compiled").  This
specification requires the premise of a small number of built-in
closures:

  - **substitute**:  Closure x Value -\> Closure  
    Returns a closure in which the first unbound variable of the
    argument closure is bound to the value

  - **cat**: Closure x Closure -\> Value  
    Evaluates both closures (which must have no unbound variables) and
    returns the value of the second

  - **eval**: Closure -\> Value  
    Evaluates the closure and returns its value

  - **push**: Env x Env -\> Env  
    Returns an environment in which the names, value mappings specified
    in both environments are available and in which any mapping in the
    second takes precedence when its name is also specified in the first

  - **lookup**: Env x Value -\> Value  
    Returns the value associated with the argument value (interpreted as
    a name) in the environment provided

  - **assign**: Env x Value x Value -\> Env  
    Returns the environment in which the mapping named by the first
    value now maps on to the new (second) value provided

The algorithm is specified as a series of "rewrite" rules that can be
used to convert a code value into a "simpler" expression involving code
values until the simplest (a code value with only a single symbol in it)
can be replaced by the use of the **lookup** function above.  
  
As a simple example of compilation observe that

``` western
{ function! }
```

(in the execution environment) could be "compiled" to the replacement

``` western
eval function
```

because this is a closure value that has yet to be executed.  The
expressions are equivalent because both of the following would yield the
same result:

``` western
{ function! }!
eval function!
```

The rules use 'E' to specify an execution environment, and 'X' and 'Y'
to specify symbols parsing to the syntax relevant to the operator each
rule deals with.  They are as follows:

1.  E::{\<index\>} =\> **lookup** E \<index\>

2.  E::{ X::Y } =\> (**eval** E::{X})::{Y}

3.  E::{ X:Y } =\> (**eval** (**push** E (**eval** E::{X})))::{Y}

4.  E::{ X.Y } =\> **eval** ((**eval** E::{X})::{Y})

5.  E::{ X Y } =\> **substitute** E::{X} E::{Y}

6.  E::{ X\! } =\> **eval** E::{X}

7.  E::{ X=Y } =\> **cat** (**assign** E X E::{Y}) E::{Y}

8.  E::{ X;Y } =\> **cat** E::{X} E::{Y}

Simplification rules can be applied during this process when the
environment is known to be constant or when the values associated with
names in environments are known to be constant.  If both compilation and
simplification is applied to expressions the cost of execution can be
significantly decreased.

## <span id="mozTocId878738"></span>Techniques

This section illustrates the expressive power of the language so far
introduced. In particular it shows how many constructs present in other
languages can be emulated.

### <span id="mozTocId431017"></span>Function calls

Closures have an obvious relation to procedures/procedures - the unbound
symbols act as formal parameters and the code becomes the body of the
procedure.  In effect functions are curried - closure values being
available at intermediate stages when not all of the arguments have been
provided.  For example

``` western
square = [n]: {mul n n !}
length = [width, height]: { sqroot (add (square width !) (square height !) !) ! }
```

Notice that

``` western
square 5 
```

simply returns the code in the binding that would evaluate 5 times 5 if
it were executed.  If its value were printed it would be

``` western
[n=5]: {mul n n !}
```

(Note: in the implementation, there is no primitive that will print the
value in this format. Printing the value as “\[\]:{mul n n \!}”, where
only unbound variables are shown, is more convenient because of the size
of the environment associated with a typical binding.)

whereas printing the expression

``` western
square 5 !
```

would produce the answer

``` western
25
```

similarly

``` western
width4length = length 4
```

simply assigns a binding which is similar to a function that adds four
squared to its argument:

``` western
[width=4, height]: { sqroot (add (square width !) (square height !) !) ! } 
```

The value of

``` western
width4length 3 !
```

would be

``` western
5
```

### <span id="mozTocId146722"></span>Records

Closure bound variables can be used as the names in a record.  for
example

``` western
point = [x=15, y=32]
```

The fields can be accessed as follows:

``` western
radius = length point.x point.y !
```

### <span id="mozTocId434879"></span>If .. then .. else

Conditional execution of a single element of code code is provided using
boolean expressions returning TRUE or FALSE (as described above).  

``` western
too_big { 
  print "not small enough" !;
}!
```

If the given code returns one of the values TRUE or FALSE, a compound
boolean expression can be built up in which each item in the chain is
executed only if previous ones have evaluated to TRUE:

``` western
<true_or_false> {<boolean_expression>}! {<boolean_expression>}! ... {<boolean_expression>}!
```

Thus this construct can be used as a conditionally executed "and".  
The standard environment also includes a function "invert" that returns
FALSE only if its argument is TRUE and returns FALSE otherwise.  When
combined with the "and" functionality above this provides a sufficient
basis for the evaluation of any boolean expression.  
It would also be possible to select between two (or more) elements of
code by using the numeric values 0 and 1 to represent a condition in a
function that takes the condition and two code values as arguments and
choses which code value to execute depending on the numeric argument.
 One such definition might be:

``` western
if = [condition, if_part, else_part]: { 
      condition != FALSE {if_part!}!; 
      condition == FALSE {else_part!}!; 
}
```

which enables code to be written such as

``` western
if (more radius 1 !) {
    print "here already\n" !
}{
    print "travelling some way\n" !
}!
```

Such a definition is not required, however, since the standard
environment in FTL includes an "if" function which takes a TRUE or FALSE
value as an argument.

### Switch Statements

Given the semantics of TRUE and FALSE it is possible to create a
switch-like construct such as:

``` western
switch = [val, valcodevec, default]:{
         if (inenv valcodevec val!) {valcodevec.(vec)!} {default!}!
}
```

(where “inenv env value\!” is TRUE when the value is bound in env) which
enables code such as this to be written:

``` western
switch n <
    1 = {echo "n is one"!},
    2 = {echo "n is two"!},
    3 = {echo "n is three"!}
> {echo "n is unknown"!}!;
```

### <span id="mozTocId937909"></span>For loops

Repetitive execution could be provided recursively by a function that
keeps decrementing its counter by one and applying the counter to an
argument block of code, e.g.

``` western
forn = [n, code]: {
         more n 0 ! {
             forn (sub n 1 !) code ! 
         }!;
         code n !;
       }
```

which enables code to be written such as

``` western
forn 4 [n]: {
  print n!; print " squared is "!; print squared n ! ; print "\n"!;
}!
```

which will print out the squares from 0 to 4.

Such a definition is not required, however, since the standard
environment in FTL includes an "for" function which takes an environment
and a closure value as an argument and applies each binding in the
environment to the closure value.

### <span id="mozTocId737869"></span>Objects and their methods

A record that combines both internal state and a number of methods that
interact with that state can be constructed using a closure.  For
example:

``` western
vect = [x,y]:{
    echo "New point"!;
    [   print = []:{
            echo "<\$x,\$y>"!
        },
        xcoord = []:{x},
        ycoord = []:{y},
        add = [pt]:{
            vect (add x (pt.xcoord!)!) (add y (pt.ycoord!)!)!
        },
        sub = [pt]:{
            vect (sub x (pt.xcoord!)!) (sub y (pt.ycoord!)!)!
        }
    ]
}
p = vect 4 5!;
p.print!;
q = vect 2 1!;
(p.sub q!).print!
```

would print out "New point", "\<4,5\>", "New point" and then "New
point", "\<2,4\>".  There are some things to note

  - The "methods" must be closures, not code objects (hence the syntax
    "\[\]:" preceding the code values), only closures inherit the
    environment at the point of their definition.

  - The "object" returned has the arguments, x and y, from point bound
    into it but does not contain them in the record returned: using p.x
    and p.y would return an error because x and y are not parts of this
    record (this is why the methods xcoord and ycoord were defined).

  - (However, because x and y are bound into the methods it is possible
    to use p.add.x and p.add.y for example\!)

  - If there is any initialization for the object to be performed it can
    be done in a code part to the definition of "point".

### Argument Passing

Although not built in to the language, methods of parameter passing can
be emulated as follows.

#### Call by value

In call by value an expression is evaluated and the resulting value is
bound in to the closure invocation.

  - example argument syntax: (add a b\!)

  - example argument read in closure body when bound as "x": x

#### Call by name

In call by name the expression passed in to the invocation is
re-evaluated in the context of the invocation on each use.

  - example argument syntax: {add a b\!}

  - example argument read in closure body when bound as "x": (x\!)

#### Call by reference - in

In call-by-reference "in", the variable passed in to the invocation is
read indirectly on on each use.

  - example argument syntax: \[\]:{a}

  - example argument read in closure body when bound as "x": (x\!)

#### Call by reference in out

In call by-reference "in out", the variable passed in to the invocation
is read indirectly on each use and can also be written to indirectly.

  - example argument syntax: \[get=\[\]:{a}, set=\[x\]:{a=x}\]

  - example argument read in closure body when bound as "x": (x.get\!)

  - example argument write in closure body when bound as "x": x.set 3\!
