FTL
===

Language Design
---------------

The cornerstone construct in FTL is a closure, which can be specified using the following syntax (which will be extended below)
    <closure> ::= '[' <symbol> ']' '::' <code-expression>

where &lt;code&gt; is a normal expression that evaluates to a code value, which could be a literal value such as
    <code> ::= '{' <code-item>* '}'

The &lt;code-items&gt; in the code primarily consist of symbols whose meaning is determined by the environment in which the code is executed.  FTL implements an environment as an ordered set of "bound" symbol-value pairs in which a value can be found from a given symbol; and an "unbound" symbol which has not yet been assigned a value.
When an argument is applied to a closure a substitution of the unbound symbol with the value given by &lt;expression&gt; is effected:
    <substitution> ::=  <closure> <expression>

in which the resulting closure value &lt;substitution&gt; incorporates an environment in which the unbound &lt;symbol&gt; in the &lt;closure&gt; is paired with the value of the &lt;expression&gt; .  When the closure is subsequently invoked
    <invocation> := <closure-expression> '!'

the code of the closure is interpreted in this environment so that the bound values are available within the code.
So, conceptually a closure value is composed of three parts
1.  an ordered environment stack in which the most recently entered symbol values take priority over former values for the same symbol when they are sought
2.  an optional unbound symbol
3.  a code value

As a generalization of the simple &lt;closure&gt; syntax above the part of the syntax before '<span style="font-family: monospace;">::</span>'
```
'[' <symbol> ']'
```

is treated as a literal environment that may be generalized to an &lt;environment-expression&gt; - the '<span style="font-family: monospace;">::</span>' being an operator that binds an environment to some code, or - more generally - extends the environment in an existing closure with the environment given.
It is often the case that the execution environment at the point of closure definition is required as part of the closure's own environment.  This can be achieved by using the '<span style="font-family: monospace;">:</span>' operator in place of '<span style="font-family: monospace;">::</span>'.  That is, a closure formed using
    <environment-expression> '::' <code-expression>

creates a closure with whose environment is exactly what is specified in the &lt;environment-expression&gt;, but
    <environment-expression> ':' <code-expression>

also includes the current environment at the point of definition plus the environment in &lt;environment-expression&gt;.
Thus the full syntax for a closure expression is:
```
<closure> ::= (<environment-expression> [ '::' | ':' ])* <code-expression>
```

where either expression may involve literals, brackets, substitution, invocation etc.  One option for an &lt;environment-expression&gt; is a &lt;closure&gt;, which, when used in this way refers only to the environment part of the closure.
<span id="mozTocId409702" class="mozTocH2"></span>Syntactic Sugar
-----------------------------------------------------------------

The above specification is sufficient to provide all the language's, typing, data structuring and code control primitives as well as its object-orientated features.  However in order to reduce the complexity of the code there is syntax to simplify certain idioms.
### <span id="mozTocId280832" class="mozTocH3"></span>Creating Bindings

This syntax
     '[' <symbol1> ']' '::' '[' <symbol2> ']' '::' <code-expression>

(in which '<span style="font-family: monospace;">::</span>' binds more tightly to the right) can be replaced by:
    '[' <symbol1>, <symbol2> ']' '::' <code-expression>

and in general any number of unbound symbols can be placed in an environment.  Note, that values associated with <span style="font-family: monospace;">&lt;symbol2&gt;</span> take precidence over those of <span style="font-family: monospace;">&lt;symbol1&gt;</span> when both symbols have the same name.  The first substitution applied to these expressions will bind <span style="font-family: monospace;">&lt;symbol1&gt;</span>.
This syntax
     '[' <symbol> ']' '::' <code-expression> <value>

can be replaced by:
    '[' <symbol> '=' <value> ']' '::' <code-expression>

and in general
    '[' <sym_1> ']' '::' ... '::' '[' <sym_m> ']' '::' <code-expression> <val_1>  ... <val_n>

can be replaced by
    '[' <sym_1> '=' <val_1> ',' ... ',' <sym_n> = <val_n> ',' ... <sym_m> ']' '::' <code-expression>

Note that each <span style="font-family: monospace;">&lt;val\_i&gt;</span> is still evaluated in the environment outside the closure (in which none of the <span style="font-family: monospace;">&lt;sym\_i&gt;</span> symbols have been set).   The symbols with an associated '=' will be bound symbols, and those without will be unbound.
In addition this environment syntax
    '[' <sym_1> '=' <val_1> ',' ... ',' <sym_n> = <val_n> ',' ... <sym_m> ']'

specifies a first-class element in the language that can be used as a value anywhere.  Note that no bound symbols can be specified following the first unbound symbol in the specification.
### <span id="mozTocId807267" class="mozTocH3"></span>Obtaining Bound Values in Closures

This syntax
    <closure> '::' '{' <symbol> '}' '!' 

can be replaced by
    <closure> '.' <symbol>

In addition the following syntax can be used to create a symbol value from an expression
    '<closure> '.' '(' <expressionl> ')'

where, for example, an expression returning an integer or a string is converted to a symbol before use.
### <span id="mozTocId495578" class="mozTocH3"></span>Integers

Although it would theoretically be possible to create an environment defining all the values for 32 bit integers:
    { int = { 0=<0 value>, 1=<1 value>, .... 4294967295=<4294967295 value> }:
    [ int: [.... main body of program ....] ] 
    !

this step is assumed as part of the runtime library - thus all numeric symbols are interpreted with their expected values (unless overridden by higher priority definitions). 
### <span id="mozTocId541092" class="mozTocH3"></span>Vectors

This syntax
    [ 0=expr0, 1=expr1, ... N=exprN ]

can be replaced by
    < expr0, expr1, ... exprN >

or where not all of the indeces are consecutive may be broken into subseries
    < <int_i>=expr_i, expr_i+1, ..., <int_j>=expr_j, expr_j+1..., ... >

i.e. using the general syntax
    '<' [ [<integer>=]<expression> [',' [<integer>=]<expression> ]* ] '>' 

Vectors that are simple numeric series can be represented
    '<' [<expression-int_first> [ <expression-int_second> ]] '..' [ <expression-int_last> ] '>'

where, if &lt;expression-int\_second&gt; is missing an integer one greater than &lt;expression-int\_first&gt; is used, and if &lt;expression-int\_first&gt; is missing the number 1 is used.   Thus, there would be the 42 numbers from one to forty-two indexed by the integers 0 to 41 in
    <..42>

so that the following would have value 10
    <..42>.11

### Code Concatenation

The infix operator ";" is available in a code object to ignore its left-hand value and replace it with its right (if there is one).  Thus the value of
    <expression 1> ; <expression 2> ; ... ; <expression n>

is &lt;expression n&gt;.  And the value of
    <expression 1> ; <expression 2> ; ... ;

is the special empty value "<span style="font-family: monospace;">NULL</span>" (which is therefore a name for the value of "<span style="font-family: monospace;">{}!</span>")
### Boolean Expressions

The standard environment in FTL includes two "built-in" values "<span style="font-family: monospace;">TRUE</span>" and "<span style="font-family: monospace;">FALSE</span>".  In principle they are implemented as if they had been defined:
    TRUE = [val]::{val!}
    FALSE = [val]::{FALSE}

That is, <span style="font-family: monospace;">TRUE</span> is a closure that will execute its argument and <span style="font-family: monospace;">FALSE</span> is a closure that will discard its argument and return <span style="font-family: monospace;">FALSE</span> instead.  Thus if a value that is either <span style="font-family: monospace;">TRUE</span> or <span style="font-family: monospace;">FALSE</span> is given code as an argument:
    <true_or_false> { <code> }!

the result will be either the result of the code's execution or the value <span style="font-family: monospace;">FALSE</span>.  Thus the code is conditionally executed. 
<span id="mozTocId347665" class="mozTocH2"></span>Syntactic Details
-------------------------------------------------------------------

The format of the source file is not specified since it is normal for FTL expressions to be embedded in an external application.  Typically such an application will apply such formatting conventions as:
-   discard of commented out lines (e.g. those beginning '\#')
-   concatenation of lines ending with an escape (e.g. '\\')
-   expansion of textual substitutions (e.g. base on FTL expressions) introduced with an expansion escape (e.g. '$')

The characters '\#', '\\' and '$' are not used as part of FTL syntax.
    <code-item> ::= <symbol> | <string> |
                    '[' | ']' | '{' | '}' | '<' | '>' | '(' | ')' |
                    '!' | '~' |  
                    ';' | ',' | ':' | '.'

    <symbol> ::= <integer> | <identifier>

    <integer> ::= <digit>+

    <identifier> ::= <common_identifier> | <builtin_identifier>

    <common_identifier> ::= <alphabetic> (<alphabetic> | '_' | <digit>)*

    <builtin_identifier> ::= '_' <digit>+ 

The precedence of the various operators outlined above is specified as part of the syntax for an expression:
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

Compilation and Semantics
-------------------------

The semantics of the language can partly be specified by describing how an expression that is an equivalent canonical representation of a code value can be obtained (i.e. how a code value can be "compiled").  This specification requires the premise of a small number of built-in closures:
-   <span style="font-weight: bold;">substitute</span>:  Closure x Value -&gt; Closure
    Returns a closure in which the first unbound variable of the argument closure is bound to the value
-   <span style="font-weight: bold;">cat</span>: Closure x Closure -&gt; Value
    Evaluates both closures (which must have no unbound variables) and returns the value of the second
-   <span style="font-weight: bold;">eval</span>: Closure -&gt; Value
    Evaluates the closure and returns its value
-   <span style="font-weight: bold;">push</span>: Env x Env -&gt; Env
    Returns an environment in which the names, value mappings specified in both environments are available and in which any mapping in the second takes precedence when its name is also specified in the first
-   <span style="font-weight: bold;">lookup</span>: Env x Value -&gt; Value
    Returns the value associated with the argument value (interpreted as a name) in the environment provided
-   <span style="font-weight: bold;">assign</span>: Env x Value x Value -&gt; Env
    Returns the environment in which the the mapping named by the first value now maps on to the new (second) value provided

The algorithms is specified as a series of "rewrite" rules that can be used to convert a code value into a "simpler" expression involving code values until the simplest (a code value with only a single symbol in it) can be replaced by the use of the <span style="font-weight: bold;">lookup</span> function above.
As a simple example of compilation observe that
    { function! }

(in the execution environment) could be "compiled" to the replacement
    eval function

because this is a closure value that has yet to be executed.  The expressions are equivalent because both of the following would yield the same result:
    { function! }!

    eval function!

The rules use 'E' to specify an execution environment, and 'X' and 'Y' to specify symbols parsing to the syntax relevant to the operator each rule deals with.  They are as follows:
1.  E::{&lt;index&gt;} =&gt; <span style="font-weight: bold;">lookup</span> E &lt;index&gt;
2.  E::{ X::Y } =&gt; (<span style="font-weight: bold;">eval</span> E::{X})::{Y}
3.  E::{ X:Y } =&gt; (<span style="font-weight: bold;">eval</span> (<span style="font-weight: bold;">push</span> E (<span style="font-weight: bold;">eval</span> E::{X})))::{Y}
4.  E::{ X.Y } =&gt; <span style="font-weight: bold;">eval</span> ((<span style="font-weight: bold;">eval</span> E::{X})::{Y})
5.  E::{ X Y } =&gt; <span style="font-weight: bold;">substitute</span> E::{X} E::{Y}
6.  E::{ X! } =&gt; <span style="font-weight: bold;">eval</span> E::{X}
7.  E::{ X=Y } =&gt; <span style="font-weight: bold;">cat</span> (<span style="font-weight: bold;">assign</span> E X E::{Y}) E::{Y}
8.  E::{ X;Y } =&gt; <span style="font-weight: bold;">cat</span> E::{X} E::{Y}

Simplification rules can be applied during this process when the environment is known to be constant or when the values associated with names in environments are known to be constant.  If both compilation and simplification is applied to expressions the cost of execution can be significantly decreased.
<span id="mozTocId878738" class="mozTocH2"></span>Techniques
------------------------------------------------------------

This section illustrates the expressive power of the language so far introduced.
### <span id="mozTocId431017" class="mozTocH3"></span>Function calls

Closures have an obvious relation to procedures/procedures - the unbound symbols act as formal parameters and the code becomes the body of the procedure.  In effect functions are curried - closure values being available at intermediate stages when not all of the arguments have been provided.  For example
    square = [n]: {times n n !}

    length = [width, height]: { sqroot (add (square width !) (square height !) !) ! }

Notice that
    square 5 

simply returns the code in the binding that would evaluate 5 times 5 if it were executed.  If its value were printed it would be
    [n=5]: {times n n !}

whereas printing the expression
    square 5 !

would produce the answer
    25

similarly
    width4length = length 4;

simply returns a binding which is similar to a function that adds four squared to its argument:
    [width=4, height]: { sqroot (add (square width !) (square height !) !) ! } 

The value of
    width4length 3 !

would be
    5

### <span id="mozTocId146722" class="mozTocH3"></span>Records

Closure bound variables can be used as the names in a record.  for example
    point = [x=15, y=32]

The fields can be accessed as follows:
    radius = length point.x point.y !

### <span id="mozTocId434879" class="mozTocH3"></span>If .. then .. else

Conditional execution of a single element of code code is provided using boolean expressions returning TRUE or FALSE (as described above).  
    too_big { 
        print "not small enough" !;
    }!;

If the given code returns one of the values <span style="font-family: monospace;">TRUE</span> or <span style="font-family: monospace;">FALSE</span>, a compound boolean expression can be built up in which each item in the chain is executed only if previous ones have evaluated to <span style="font-family: monospace;">TRUE</span>:
    <true_or_false> {<boolean_expression>}! {<boolean_expression>}! ... {<boolean_expression>}!

Thus this construct can be used as a conditionally executed "and".
The standard environment also includes a function "<span style="font-family: monospace;">invert</span>" that returns <span style="font-family: monospace;">FALSE</span> only if its argument is <span style="font-family: monospace;">TRUE</span> and returns <span style="font-family: monospace;">FALSE</span> otherwise.  When combined with the "and" functionality above this provides a sufficient basis for the evaluation of any boolean expression.
It would also be possible to select between two (or more) elements of code by using the numeric values 0 and 1 to represent a condition in a function that takes the condition and two code values as arguments and choses which code value to execute depending on the numeric argument.  One such definition might be:
    if = [condition, if_part, else_part]: { <if_part, else_part>.(condition) }

which enables code to be written such as
    if (max 1 radius !) {
        print "here already\n" !
    }{
        print "travelling some way\n" !
    }!

Such code is not required, however, since the standard environment in FTL includes an "if" function which takes a <span style="font-family: monospace;">TRUE</span> or <span style="font-family: monospace;">FALSE</span> value as an argument rather than an integer.
### Switch Statements

Given the semantics of <span style="font-family: monospace;">TRUE</span> and <span style="font-family: monospace;">FALSE</span> it is possible to create a switch-like construct such as:
    when = [cond,code]:{if cond {code!;FALSE} {TRUE}!}

which enables code such as this to be written:
    (when (equal n 1!) {echo "n is one"!})! 
    (when (equal n 2!) {echo "n is two"!})! 
    (when (equal n 3!) {echo "n is three"!})! 
    {echo "n is unknown"!}!;

### <span id="mozTocId937909" class="mozTocH3"></span>For loops

Repetitive execution can be provided recursively by a function that keeps decrementing its counter by one and applying the counter to an argument block of code, e.g.
    for = [first, n, code]: 
           { code n !;
             if ( gt n 0 ! ) 
             {   for first minus n 1 ! code ! 
             } {} !
           }

which enables code to be written such as
```
for 0 4 [n]: {
    print n!; print " squared is "!; print squared n ! ; print "\n"!;
}!
```

which will print out the squares from 0 to 3.
### <span id="mozTocId737869" class="mozTocH3"></span>Objects and their methods

A record that combines both internal state and a number of methods that interact with that state can be constructed using a closure.  For example:
    point = [x,y]:{
        echo "New point";
        [   print = []:{
                echo "<\$x,\$y>"!
            },
            distance = []:{
                sqrt add times x x! times y y!!! 
            },
            xcoord = []:{x},
            ycoord = []:{y},
            sub = [pt]: {
                sub x (pt.xcoord!) sub y (pt.ycoord!)!!
            }
        ]
    }

    p = point 4 5!;

    p.print!;

    q = point 2 1!;

    (p.sub q!).print!

would print out "New point", "&lt;4,5&gt;", "New point" and then "New point", "&lt;2,4&gt;".  There are some points to note
-   The "methods" must be closures, not code objects (hence the syntax "<span style="font-family: monospace;">\[\]:</span>" preceding the code values), only closures inherit the environment at the point of their definition.  If we had used <span style="font-family: monospace;">print = {echo "&lt;\\$x,\\$y&gt;"}</span> only the code body would be returned by <span style="font-family: monospace;">p.print</span> which would have no particular values of "<span style="font-family: monospace;">x</span>" or "<span style="font-family: monospace;">y</span>" embedded in it.
-   The "object" returned has the arguments, <span style="font-family: monospace;">x</span> and <span style="font-family: monospace;">y</span>, from point bound into it but does not contain them in the record returned: using <span style="font-family: monospace;">p.x</span> and <span style="font-family: monospace;">p.y</span> would be an error because <span style="font-family: monospace;">x</span> and <span style="font-family: monospace;">y</span> are not parts of this record (this is why the methods <span style="font-family: monospace;">xcoord</span> and <span style="font-family: monospace;">ycoord</span> were defined).
-   If there is any initialization for the object to be performed it can be done in a code part to the definition of "<span style="font-family: monospace;">point</span>".

### Argument Passing

Although not built in to the language, methods of parameter passing can be emulated as follows.
#### Call by value

In call by value an expression is evaluated and the resulting value is bound in to the closure invocation.
-   example argument syntax: <span style="font-family: monospace;">(plus a b!)</span>
-   example argument read in closure body when bound as "x": <span style="font-family: monospace;">x</span>

#### Call by name

In call by name the expression passed in to the invocation is re-evaluated in the context of the invocation on each use.
-   example argument syntax: <span style="font-family: monospace;">{plus a b!}</span>
-   example argument read in closure body when bound as "x": <span style="font-family: monospace;">(x!)</span>

#### Call by reference - in

In call-by-reference "in", the variable passed in to the invocation is read indirectly on on each use.
-   example argument syntax: <span style="font-family: monospace;">\[\]:{a}</span>
-   example argument read in closure body when bound as "x": <span style="font-family: monospace;">(x!)</span>

#### Call by reference in out

In call by-reference "in out", the variable passed in to the invocation is read indirectly on each use and can also be written to indirectly.
-   example argument syntax: <span style="font-family: monospace;">\[get=\[\]:{a}, set=\[x\]:{a=x}\]</span>
-   example argument read in closure body when bound as "x": <span style="font-family: monospace;">(x.get!)</span>
-   example argument write in closure body when bound as "x": <span style="font-family: monospace;">x.set 3!</span>

