/*
 * Copyright (c) 2005-2009, Solarflare Communications Inc.
 * Copyright (c) 2014, Broadcom Inc.
 * Copyright (c) 2005-2018, Gray Girling
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Solarflare nor the names of its contributors may
 *     be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* author  Gray Girling
** brief   Framework for Testing Command-line Library
** date    Jun 2015
**/

/*! \cidoxg_lib_libftl */

/* Interesting future directions
   -----------------------------

   Try to correct problems with environment pointer references (e.g. such
   as occur using parser.root in some circumstances)

   Formatted output function, fix combination of sign and leading zeroes

   Fix source command which crashes if executed inside a code item

   Garbage collection on demand - so we can support infinite loops

   Separate per-type fields from per-value fields in value representations to
   reduce their size

   Increase efficiency of memory allocation - e.g. fixed size allocation blocks
   of sufficient size to cover all common types - instead of malloc/free.

   Add value_free to remove a value (and its components) from the garbage
   collection list and do a value_delete(), so we can keep tidy as we go

   (new class!)::instance to inherit features from class. (May work now.)

   More parsing functions to support the commands (e.g. generate ftl functions
   from the parse_* functions)

   Integration of parse functions into type definitions

   Generalization of expression parser to use parse functions in the
   environment stack - so we can stop usurping the denotations of string, int,
   etc. for those types instead of new ones

   Consider <env>.{code} syntactical sugar for <env> {code}! - so we can
   enforce type parsing (e.g. ip.{168.192.4.5})

   Support a register of types so we can extend the set of types in ftl (i.e.
   from ftl scripts). Ideally introduce a new 'type' type which supports the
   same operations as the C version does, extended with a way to register a
   directory e.g. containing user-supplied operations.  Perhaps consider '<val>
   -> <user-type-dir-field>' as a way to access these operation given a value of
   a given type.

   This syntax should be used as the basis of type values.  e.g. the 'int' type
   should impose the identity int.{1} == 1, string.{"this"} == "this" and
   mac.{1:2:3:4:5:6} == macaddr {1:2:3:4:5:6}!. Make a representation for a
   type that includes a unique ID and a parse function?

   Ideally other components of a type might be the operations on it -
   e.g. int.add.  For such diadic operators how do you infer which operand
   defines the operator and avoid ambiguity?  Should there be a operator x
   type1 x type2 -> operator_function facility independent of the type
   definitions?

   The default '_parse' should be made available in the library - as the one
   that parses an FTL value.

   Support environment syntax which allows a wider range of index types -
   possibly any index type that supports comparison.

   In any event allow the syntax [1 = "one", 4 = 4, 9 = NULL] etc. so we can
   compile the denotation we currently print for vectors when they are part of
   larger environments.

   Inevitably this will have to support mixed types of index.  Because
   different types can't overlap they can be implemented as <type1 env>::<type2
   env> etc.

   Note that this kind of value can currently be created with syntax such as
   <9=NULL>::<4=4>::<1="one">, which is displayed as containing no value for
   intermediate indeces.

   Vector types should be given a base which is set to the first non-zero index.

   Support a stream read-line function (don't confuse with current parse
   readline function - which reads from input)

   Support socket-based streams

   Do something about the string representation of built-in commands and
   functions so that they have a chance of being interpreted as the same
   command when read from the string - even if the interpreter is running on a
   different machine or is of a different age.  One day we'll have to split
   this file into several parts - we need a mechanisms that will still work!

   Using the event handling mechanism:
       throw <val>,
       catch [val]:{catch code} {<code>}
   Run each interactive command line in a global catch environment that simply
   prints <val> as an error.

   Use signal handling, when available, to define additional throw sources.

   Make input stacks garbage collectable values, create a new "coroutine" type
   that contains both a current environment pointer and input stack.  Implement
   a coroutine call
       <coroutine value> = co [first]:{code}!
       coretval = cocall <coroutine> <retval>
   This may require a complete rethink of the way in which we implement
   execution (unless we rely on C co-routines).

   If we had the above would we need any additional functions to support
   sequences?  Support a stream type that reads and writes a coroutine or
   reads a sequence.

   Can we generate a stream from the execution of a system command portably?

   Consider making programmer's life easier by including an "implicit invoke"
   flag (off by default) with each function object - which invokes the function
   as soon as its last bound variable has been substituted.  Use syntax &<expr>
   to yield the function generated by <expr> with this flag unset.
   What do you do about TRUE/FALSE?

   Implement the parsing of dynamically declared operators with defined
   binding directions and priorities.  Use these for all directly executed
   parsing elements (e.g. the arithmetic and string operators).  Associate
   operator definitions with the environment.  e.g.
       operator value = op <binding-type> <priority> <function>
       [ "+" = op "xfx" 4 [a,b]{int.add a b!}!,
         "-" = op "xfx" 4 [a,b]{int.sub a b!}!]
   May need to separate the definition of an operator from the assignment
   of a function invoked when it is parsed.
   Ensure that a first class function is available to map each existing
   operator on to.

   Perhaps support an operator expansion function:
       > parse ops 1 + 3 * 8
       { add 1 (times 3 8!)! }

   Implement syntax &<operator name> to return function it maps to?  It would
   something affecting the type of binding created that persists as additional
   unbound variables are bound.  It would be possible to have a variable in the
   environment that determines which kind of binding are created by default.
   In environments with the variable set '&' would have no effect.

   Provide an annotation that makes the value of an environment name a constant

   Provide explicit functions equivalent to those implicit in the
   constructional operators in ftl: e.g. including stackenv, bind,
   substitute, invoke, lookup. To help enable compilation.

   Implement a compiler - using the rules already outlined in the white paper,
   replace execute names with constant values.  Have functions for lookup, bind,
   apply, substitute, code block (as vector of bindings) etc.

   Consider making all parsing determined by operators defined by a named value
   in the environment.  (e.g. _parse) Implement the convention that a block is
   mapped on to a string by default but otherwise is given to _parse. This
   would mean that env::{{code}} would be 'compiled' to (env._parse {code}!).
   It would also mean that env.{parse} would have this natural meaning (because
   a.b == a::{b}) - and this idiom env.{code} should be compiled as such.

   Dynamic mapping of strings on to vectors. Dynamic mapping of subvectors.
   Dynamic origin shifting.  Extend support for datatype_t and make 1st class
   value.  Implement 'cast' to achieve the above dynamic mappings.  Make it
   applicable to mem_t too.  Integrate with 'structure' definitions.

   Support for the generation of environment types that map on to specified
   C data structures - perhaps given a specification of the datastructure
   itself in C.

   File directory as an environment type.

   Functions to support automatic generation of an XML representation of
   an environment.

   Support for generating a C-section for linking into the library from a
   specification of an available C interface (a specification which is a C
   header - perhaps with restricted syntax - would be nice).

   fix 'new  [a=8, b=8]::<4,6>'
*/





/*****************************************************************************
 *                                                                           *
 *          Configuration                                                    *
 *          =============                                                    *
 *                                                                           *
 *****************************************************************************/



/* #define USE_FTL_XML */
/* #define USE_READLINE */
/* #define USE_LINENOISE */
/* #define EXIT_ON_CTRL_C */

#define VERSION_MAJ 1
#define VERSION_MIN 20

#if defined(USE_READLINE) && defined(USE_LINENOISE)
#error you can define only one of USE_READLINE and USE_LINENOISE
#endif

/*#define LOCAL_GARBAGE*/
/**< collect garbage only only if it's value is marked non-local */
/**<  If the above is defined we will pay attention to a type's 'local' marker.
 *   (otherwise is just isn't used and variables are never treated as 'local').
 *
 *   To try to isolate uncommitted values (i.e. ones that are allocated but
 *   which should not be garbage collected yet, we use the notion of a value
 *   being 'local' (local == not reachable from garbage collection root).  Most
 *   new values are allocated 'local' and should be unset as soon as they have
 *   either been used in the garbage-collection-safe tree, or known not to be
 *   required, they can be un-localed.
 *   Ideally every new value will be un-localed before it leaves the scope of
 *   the routine that consumes it.
 *   The hope here is to enable the garbage collector to be run relatively
 *   asynchronously (e.g. when memory is low).
 *   Note that local values will never be collected by the garbage collector if
 *   they are not unlocalled, so that any which are still local at the time of
 *   an exception (which uses longjmp) may never be retrieved.
 */

/*#define FTL_BOOL_ISINT*/

#define FTL_ERROR_TRAIL_LINES 4

#ifdef _WIN32

#define OS_FILE_SEP "\\"
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define HAS_REGISTRY

#else

#define OS_FILE_SEP "/"
#define HAS_UNSETENV
#define HAS_SETENV
#define HAS_OPENDIR

#endif /* _WIN32 */

#ifdef _CYGWIN_
/* #error CYGWIN */
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
/* #error MINGW */
#endif

#define ENV_FTL_RCFILE_POST "_RC"
#ifdef _WIN32
#define ENV_FTL_HOMEDIR        "USERPROFILE"
#define ENV_FTL_HOMEDIR_HOME   OS_FILE_SEP "My Documents"
#define ENV_FTL_RCFILE_DEFAULT(code_name) \
                  "%s%s", code_name, ".rc"
#define ENV_FTL_PATH(code_name) \
                  "%s%s", code_name, "_PATH"
#define ENV_PATHNAME_MAX 32
#else
#define ENV_FTL_HOMEDIR        "HOME"
#define ENV_FTL_HOMEDIR_HOME   ""
#define ENV_FTL_RCFILE_DEFAULT(code_name) \
                  "%s%s%s%s%s", ".", code_name, OS_FILE_SEP, code_name, ".rc"
#define ENV_FTL_RCFILE_DEFAULT_PRE  ("." OS_FILE_SEP)
#define ENV_FTL_RCFILE_DEFAULT_POST ".rc"
#define ENV_FTL_PATH(code_name) \
                  "%s%s", code_name, "_PATH"
#define ENV_PATHNAME_MAX 32
#endif
#define READLINE_HOME_DIR_PREFIX    "."
#define READLINE_HISTORY_EXTENSION  ".txt"
#define READLINE_HISTORY_NAME       "history"
#define READLINE_HISTORY_MAXLEN     (500)

#define BUILTIN_ARG_CH1 '_'
#define BUILTIN_ARG "_"
#define BUILTIN_HELP "_help"

#define FTLDIR_EXCEPT "exception"
#define FN_EXCEPT_SIGNAL "signal"

#define FTLDIR_PARSE  "parse"
#define FN_UNDEF_HOOK "on_badcmd"

#define OP_FN         "fn"
#define OP_ASSOC      "assoc"

#define PRINTSTACK_MAX 32
#define PATHNAME_MAX 1024 /* max characters in a full file name */

#define APP_ARGC_MAX 64

#define CATDELIM(a, delim, b) #a  delim  #b
#define VERSIONSTR(max, min) CATDELIM(max, ".", min)
#define VERSION VERSIONSTR(VERSION_MAJ, VERSION_MIN)



#define ftl_assert assert

#ifndef SYS_EPOCH_YEAR
#define SYS_EPOCH_YEAR 1900
#endif

#define FTL_LINESOURCE_NAME_MAX 64
#define FTL_ID_MAX              64
#define FTL_CMDNAME_MAX         FTL_ID_MAX
#define FTL_PRINTF_MAX          256
#define FTL_SYSENV_VAL_MAX      256
#define FTL_STRING_MAX          256
#define FTL_ARGNAMES_CACHED     8

#define HAS_VSNPRINTF
#define HAS_TOWUPPER    /* C99 function */
#define HAS_TOWLOWER    /* C99 function */
#define HAS_GETHOSTBYNAME
/* #define HAS_SOCKETS */






/*****************************************************************************
 *                                                                           *
 *          Headers                                                          *
 *          =======                                                          *
 *                                                                           *
 *****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <wchar.h>
#include <signal.h>
#include <assert.h>
#include <time.h>
#include <wctype.h>
#include <stddef.h> /* for ptrdiff_t */
#include <setjmp.h>
#include <errno.h>

#ifdef _WIN32

#include <windows.h>
#include <winerror.h>
#ifndef EINVAL
#define EINVAL WSAEINVAL
#endif
#ifndef EBADF
#define EBADF WSAEBADF
#endif

#else
#include <errno.h>
#endif


#include "libdyn.h"
#include "filenames.h"
#include "ftl.h"
#include "ftl_internal.h"
#include "ftlext.h"
#define STRING(_x) #_x

#ifdef USE_FTL_XML
#include "ftl_xml.h"
#endif

#ifdef HAS_GETHOSTBYNAME
#if defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/socket.h>        /* for AF_INET */
#endif
#endif /* HAS_GETHOSTBYNAME */



/*****************************************************************************
 *                                                                           *
 *          Debugging                                                        *
 *          =========                                                        *
 *                                                                           *
 *****************************************************************************/



#ifdef OMIT
#undef OMIT
#endif

#ifdef DO
#undef DO
#endif

#define DO(_x) _x
#define OMIT(_x)

#define FORCEDEBUG

// #define DEBUG_GC DO 
// #define DEBUG_GCU DO
// #define DEBUG_VALINIT DO 
// #define DEBUG_VALLINK DO
// #define DEBUG_CHARS DO 
// #define DEBUG_LNO DO 
// #define DEBUG_CONSOLE DO 
// #define DEBUG_EXPD DO 
// #define DEBUG_SERIES DO 
// #define DEBUG_TRACE DO
// #define DEBUG_DIR DO
// #define DEBUG_ENV DO 
// #define DEBUG_MOD DO
// #define DEBUG_SOCK DO 
// #define DEBUG_OP DO 
// #define DEBUG_RCFILE DO 
// #define DEBUG_EXECV DO
// #define DEBUG_MODEXEC DO 
// #define DEBUG_CMP DO 
// #define DEBUG_MEMDIFF DO 
// #define DEBUG_ARGV DO

#if defined(NDEBUG) && !defined(FORCEDEBUG)
#undef DEBUG_GC
#undef DEBUG_GCU
#undef DEBUG_VALINIT
#undef DEBUG_VALLINK
#undef DEBUG_CHARS
#undef DEBUG_LNO
#undef DEBUG_CONSOLE
#undef DEBUG_EXPD
#undef DEBUG_SERIES
#undef DEBUG_TRACE
#undef DEBUG_DIR
#undef DEBUG_ENV
#undef DEBUG_MOD
#undef DEBUG_SOCK
#undef DEBUG_OP
#undef DEBUG_EXECV
#undef DEBUG_MODEXEC
#undef DEBUG_RCFILE
#undef DEBUG_CMP
#undef DEBUG_MEMDIFF
#undef DEBUG_ARGV
#endif



#ifndef DEBUG_GC
#define DEBUG_GC OMIT
#endif
#ifndef DEBUG_GCU
#define DEBUG_GCU OMIT
#endif
#ifndef DEBUG_VALINIT
#define DEBUG_VALINIT OMIT
#endif
#ifndef DEBUG_VALLINK
#define DEBUG_VALLINK OMIT
#endif
#ifndef DEBUG_CHARS
#define DEBUG_CHARS OMIT
#endif
#ifndef DEBUG_LNO
#define DEBUG_LNO OMIT
#endif
#ifndef DEBUG_CONSOLE
#define DEBUG_CONSOLE OMIT
#endif
#ifndef DEBUG_EXPD
#define DEBUG_EXPD OMIT
#endif
#ifndef DEBUG_SERIES
#define DEBUG_SERIES OMIT
#endif
#ifndef DEBUG_TRACE
#define DEBUG_TRACE OMIT
#endif
#ifndef DEBUG_DIR
#define DEBUG_DIR OMIT
#endif
#ifndef DEBUG_ENV
#define DEBUG_ENV OMIT
#endif
#ifndef DEBUG_MOD
#define DEBUG_MOD OMIT
#endif
#ifndef DEBUG_SOCK
#define DEBUG_SOCK OMIT
#endif
#ifndef DEBUG_OP
#define DEBUG_OP OMIT
#endif
#ifndef DEBUG_EXECV
#define DEBUG_EXECV OMIT
#endif
#ifndef DEBUG_MODEXEC
#define DEBUG_MODEXEC OMIT
#endif
#ifndef DEBUG_RCFILE
#define DEBUG_RCFILE OMIT
#endif
#ifndef DEBUG_CMP
#define DEBUG_CMP OMIT
#endif
#ifndef DEBUG_MEMDIFF
#define DEBUG_MEMDIFF OMIT
#endif
#ifndef DEBUG_ARGV
#define DEBUG_ARGV OMIT
#endif

/* #define DPRINTF ci_log */
#define DPRINTF printf

#define CODEID_UC "FTL"
#define CODEID "ftl"
#define CODEIDNAME_MAX 64

#if DEBUG_VALINIT(1+)0 > 0 && defined(FTL_VAL_HAS_LINENO)
#error enable FTL_VAL_HAS_LINENO in ftl_internal.h for DEBUG_VALINIT
#endif



/* checking pointers */
#define PTRVALID(_var) (NULL != (_var))
#define HEAPVALID(_var) (true)

#if 0 /* defined(__APPLE__) */
#undef PTRVALID
#define PTRVALID(_var) \
    (assert((((ptrdiff_t)(_var)) & 0xFFFF000000000000) == 0),NULL != (_var))
#if 0
#undef HEAPVALID
#define HEAPVALID(_var) \
    (assert((((ptrdiff_t)(_var)) & 0xFFFFF00000000000) == 0x700000000000),\
     true)
#endif
#endif




/*****************************************************************************
 *                                                                           *
 *          O/S Dependencies                                                 *
 *          ================                                                 *
 *                                                                           *
 *****************************************************************************/







#ifndef HAS_VSNPRINTF


/* we don't have this on windows */


int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{   int len = vsprintf(str, format, ap);
    ftl_assert((size_t)len < size); /* will have done some damage */
    return len;
}




/* we don't have this on Windows */
int snprintf(char *str, size_t size, const char *format, ...)
{   /* implement it unsafely! */
    va_list args;
    int len;

    va_start(args, format);
    len = vsnprintf(str, size, format, args);
    va_end(args);

    return len;
}



#endif /* HAS_VSNPRINTF */





#ifdef HAS_TOWUPPER

#define FTL_TOWUPPER towupper

#else

/* let's hope toupper works then.. */

static wint_t ftl_towupper(wint_t wc)
{   return toupper(wc);
}

#define FTL_TOWUPPER ftl_towupper

#endif





#ifdef HAS_TOWLOWER

#define FTL_TOWLOWER towlower

#else

/* let's hope tolower works then.. */

static wint_t ftl_towlower(wint_t wc)
{   return tolower(wc);
}

#define FTL_TOWLOWER ftl_towlower

#endif





#ifdef _WIN32




#ifndef WINCE
#include <process.h>
#endif



#define putenv _putenv




extern void sleep_ms(unsigned long milliseconds)
{   Sleep(milliseconds);
}







typedef unsigned os_uid_t;

#define DEBUGUID OMIT

#define BYTESWAP_32(v)    ((((v) & 0xff000000) >> 24)|  \
                           (((v) & 0x00ff0000) >> 8) |  \
                           (((v) & 0x0000ff00) << 8) |  \
                           (((v) & 0x000000ff) << 24))


/* @TODO - Need to work out a way of getting UID's that is hash collision free
*/
static os_uid_t
sid_hash(void *sidbuf, size_t sidbufsize)
{   os_uid_t hash = 0;
    size_t hash_pos;
    os_uid_t *sid_hash = (os_uid_t *)sidbuf;

    /* Hash data from "sid" for "sidbufsize" bytes.
       Do this in chunks of (sizeof(cfg_uid_t))
       However, pay attention to endian-ness, so that hash we
       come up with is the same on all architectures. */

    for (hash_pos = 0;
         hash_pos < sidbufsize;
         sid_hash++, hash_pos += sizeof(hash))
         hash = hash ^ BYTESWAP_32(*sid_hash);

    if (hash_pos != sidbufsize)
    {   os_uid_t tmp_hash = BYTESWAP_32(*sid_hash);

        /* tmp has is now big endian, so valid bytes are at
           lowest memory address, which is *most* significant,
           so use only those bytes, and shift down
        */

        tmp_hash = tmp_hash >> (8 * (sidbufsize - hash_pos));
        hash = hash ^ tmp_hash;
    }

    return hash;
}





static bool
user_sid_hash(const char *domainname, const char *username, os_uid_t *out_uid)
{   bool ok = FALSE;
    SID_NAME_USE name_type;
    UCHAR sidbuf[256];
    DWORD sidsize = sizeof(sidbuf);
    TCHAR domain[256];
    DWORD domainsize = sizeof(domain)/sizeof(TCHAR);

    memset(&sidbuf[0], 0, sizeof(sidbuf));
    /* TODO: should use LookupAccountNameW (Unicode) or LookupAccountNameA
             (ANSI) for arbitrary character set */
    if (LookupAccountName(domainname, username,
                          &sidbuf[0], &sidsize, &domain[0], &domainsize,
                          &name_type))
    {   if (name_type == SidTypeUser)
        {   ok = TRUE;
            *out_uid = sid_hash(&sidbuf, sidsize);
        }
        DEBUGUID(else DPRINTF("%s: account name lookup of %s\\%s is not for a "
                              "user - name type %d\n",
                              codeid(),
                              domainname==NULL? "<default>": domainname,
                              username, name_type);)
    }
    DEBUGUID(else DPRINTF("%s: account name lookup of %s\\%s failed - rc %d\n",
                          codeid(), domainname==NULL? "<default>": domainname,
                          username, GetLastError());)

    return ok;
}






static bool
user_uid(const char *name, unsigned *out_uid)
{   char *backslash = strchr(name, '\\');
    const char *domain = NULL;
    char dombuf[64];

    if (NULL != backslash)
    {   size_t len = backslash - name > sizeof(dombuf)-1?
                     sizeof(dombuf)-1: backslash-name;
        memcpy(&dombuf[0], name, len);
        dombuf[len+1] = '\0';
        domain = &dombuf[0];
        name = backslash+1;
    }
    return user_sid_hash(domain, name, out_uid);
}






#if !defined(WINCE) && !defined(__GNUC__)
static DWORD WINAPI
thread_wrapper(PVOID pvParam)
{   thread_work_t *args = (thread_work_t *)pvParam;
    return (*args->main)(args);
}
#endif





/* NB: ensure that 'work' (which will probably be part of a larger structure)
       is in storage with a lifetime longer than the thread
*/
extern thread_os_t
thread_new(thread_main_fn_t *main, thread_work_t *work, size_t stacksize)
{   work->main = main;

    /* returns 0 on error - errno set */
#ifdef WINCE
    return 0; /* ?? think that WINCE uses CreateThread */
#else
#ifdef __GNUC__ /* mingw */
    return 0; /* ?? don't know if it is implemented */
#else
    {
        void *psa = NULL;    /* security attributes */
        unsigned cbStack = (unsigned)stacksize;
                                /* 0 gives same-as-main stack size */
        unsigned fdwCreate = 0; /* flag 0 = start now, not CREATE_SUSPENDED */
        unsigned thread_id = 0; /* written by _beginthreadex */

        return (thread_os_t)_beginthreadex(psa, cbStack, &thread_wrapper, work,
                                           fdwCreate, &thread_id);
    }
#endif
#endif
    /* actually returns a uintptr_t */
}







extern int /* rc */
thread_rc(thread_os_t thread)
{   DWORD rc;

    if (!GetExitCodeThread(thread, &rc))
        rc = ERROR_PATH_NOT_FOUND;  /* no such process */

    return (int)rc;
}






extern bool
thread_active(thread_os_t thread)
{   return STILL_ACTIVE == thread_rc(thread);
}





extern thread_os_t
thread_self(void)
{   return GetCurrentThread();
}




#else /* asssume Linux */


#include <sys/types.h>
#include <unistd.h> /* e.g. for usleep */
#include <pwd.h>    /* for getpwnam, struct passwd */


extern void sleep_ms(unsigned long milliseconds)
{   usleep(1000*milliseconds);
}



static bool user_uid(const char *name, unsigned *out_uid)
{   struct passwd *pwd = getpwnam(name);
    OMIT(printf("%s: lookup '%s' gives %p\n", codeid(), name, pwd);)
    if (NULL == pwd)
        return FALSE;
    else
    {   *out_uid = pwd->pw_uid;
        return TRUE;
    }
}




extern thread_os_t
thread_new(thread_main_fn_t *main, thread_work_t *work, size_t stacksize)
{   return 0;
}

extern bool
thread_active(thread_os_t thread)
{   return FALSE;
}


#endif









/*****************************************************************************
 *                                                                           *
 *          Code ID                                                          *
 *          =======                                                          *
 *                                                                           *
 *****************************************************************************/





static const char *code_name = CODEID;
static char code_name_uc[CODEIDNAME_MAX];




extern const char *
codeid(void)
{   return code_name;
}



extern const char *
codeid_uc(void)
{   return code_name_uc;
}



extern void
codeid_set(const char *codeid)
{
    code_name = codeid;

    if (NULL != codeid)
    {   size_t len = strlen(codeid);
        size_t i;

        if (len > CODEIDNAME_MAX-1)
            len = CODEIDNAME_MAX-1;

        for (i=0; i<len; i++)
            code_name_uc[i] = toupper(code_name[i]);

        code_name_uc[len+1] = '\0';
    }
}








/*****************************************************************************
 *                                                                           *
 *          Character Sinks                                                  *
 *          ===============                                                  *
 *                                                                           *
 *****************************************************************************/






/* typedef struct charsink_s {} charsink_t */;




extern bool /*full*/
charsink_putc(charsink_t *sink, int ch)
{   if (NULL != sink)
        return (*sink->putc)(sink, ch);
    else
        return FALSE;
}






extern int
charsink_write(charsink_t *sink, const char *buf, size_t len)
{   int n = 0;
    bool ok;

    if (NULL != sink)
        while (len-->0 && (ok = ((*sink->putc)(sink, *buf++))))
            n++;
    DO(else DPRINTF("%s: writing to unopened sink\n", codeid());)

    return n;
}



extern int
charsink_putwc(charsink_t *sink, wchar_t ch)
{
    int ret;

    if (NULL != sink)
    {   char mbstring[FTL_MB_LEN_MAX+1];
        size_t len;
#if 0 /* wcrtomb in windows header but not in windows library */
        mbstate_t mbstate;

        (void)wcrtomb(NULL, L'\0', &mbstate); /* reset state */
        len = wcrtomb(&mbstring[0], ch, &mbstate);
#else
        (void)wctomb(NULL, L'\0'); /* reset state */
        len = wctomb(&mbstring[0], ch);
#endif

        if ((int)len >= 0 && len < sizeof(mbstring))
        {   mbstring[len] = '\0';
            ret = charsink_write(sink, &mbstring[0], len);
        } else
            ret = -EINVAL;
        return ret;
    } else
        return -EBADF;
}






extern int
charsink_vsprintf(charsink_t *sink, const char *format, va_list args)
{   char buf[FTL_PRINTF_MAX];
    int len = vsnprintf(&buf[0], sizeof(buf), format, args);

    if (len > 0)
    {   if (len >= sizeof(buf))
        {   len = charsink_write(sink, &buf[0], sizeof(buf));
            len += charsink_write(sink, "...\n", 4);
        } else
            len = charsink_write(sink, &buf[0], len);
    }
    return len;
}




extern int
charsink_sprintf(charsink_t *sink, const char *format, ...)
{   va_list args;
    int len;

    va_start(args, format);
    len = charsink_vsprintf(sink, format, args);
    va_end(args);

    return len;
}






static charsink_t *
charsink_init(charsink_t *sink, putc_fn_t *putc)
{   sink->putc = putc;
    return sink;
}








/*****************************************************************************
 *                                                                           *
 *          Output Streams                                                   *
 *          ==============                                                   *
 *                                                                           *
 *****************************************************************************/










typedef struct
{   charsink_t sink;
    FILE *out;
} charsink_stream_t;






static bool
charsink_stream_putc(charsink_t *sink, int ch)
{   charsink_stream_t *stream = (charsink_stream_t *)sink;
    if (NULL != stream->out)
        return EOF != fputc(ch, stream->out);
    else
        return FALSE;
}








static charsink_t *
charsink_stream_init(charsink_stream_t *stream, FILE *out)
{   stream->out = out;
    return charsink_init(&stream->sink, &charsink_stream_putc);
}






STATIC_INLINE void
charsink_stream_close(charsink_t *sink)
{   /* charsink_stream_t *stream = (charsink_stream_t *)sink; */
    return; /* nothing to do */
}






extern charsink_t *
charsink_stream_new(FILE *out)
{   charsink_stream_t *stream = (charsink_stream_t *)
                                FTL_MALLOC(sizeof(charsink_stream_t));
    if (stream == NULL)
        return NULL;
    else
        return charsink_stream_init(stream, out);
}






extern void
charsink_stream_delete(charsink_t **ref_sink)
{   charsink_stream_t *stream = (charsink_stream_t *)*ref_sink;
    if (PTRVALID(stream))
    {   charsink_stream_close(*ref_sink);
        FTL_FREE(stream);
        *ref_sink = NULL;
    }
}









/*****************************************************************************
 *                                                                           *
 *          Output Sockets                                                   *
 *          ==============                                                   *
 *                                                                           *
 *****************************************************************************/










typedef struct
{   charsink_t sink;
    int fd;
    int send_flags;
} charsink_socket_t;






static bool
charsink_socket_putc(charsink_t *sink, int intch)
{   charsink_socket_t *socket = (charsink_socket_t *)sink;
    if (socket->fd >= 0)
    {   char ch = (char)intch;
        int len = (int)send(socket->fd, &ch, 1, socket->send_flags);
        return len == 1;
    } else
        return FALSE;
}








static charsink_t *
charsink_socket_init(charsink_socket_t *socket, int fd, int send_flags)
{   socket->fd = fd;
    socket->send_flags = send_flags;
    return charsink_init(&socket->sink, &charsink_socket_putc);
}






STATIC_INLINE void
charsink_socket_close(charsink_t *sink)
{   /* charsink_socket_t *socket = (charsink_socket_t *)sink; */
    return; /* nothing to do */
}






extern charsink_t *
charsink_socket_new(int fd, int send_flags)
{   charsink_socket_t *socket = (charsink_socket_t *)
                                FTL_MALLOC(sizeof(charsink_socket_t));
    if (socket == NULL)
        return NULL;
    else
        return charsink_socket_init(socket, fd, send_flags);
}






extern void
charsink_socket_delete(charsink_t **ref_sink)
{   charsink_socket_t *socket = (charsink_socket_t *)*ref_sink;
    if (PTRVALID(socket))
    {   charsink_socket_close(*ref_sink);
        FTL_FREE(socket);
        *ref_sink = NULL;
    }
}









/*****************************************************************************
 *                                                                           *
 *          Output Containers                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/





#define CHARSINK_STRING_N_INIT 8




/* typedef struct {} charsink_string_t; */




static bool
charsink_string_putc(charsink_t *sink, int ch)
{   charsink_string_t *charbuf = (charsink_string_t *)sink;
    bool ok = (NULL != charbuf);

    if (ok)
    {   char *charvec = charbuf->charvec;
        size_t index = charbuf->n;

        if (index+1 >= charbuf->maxn)  /* "+1" to allow a final \0 */
        {   size_t old_maxn = charbuf->maxn;
            size_t new_maxn = old_maxn == 0?
                              CHARSINK_STRING_N_INIT: 2*old_maxn;
            char *newchars;

            if (new_maxn < index+2)
                new_maxn = index+2;

            OMIT(printf("extend string from %d to %d (index %d)\n",
                          old_maxn, new_maxn, index);)
            newchars = (char *)FTL_MALLOC(new_maxn*sizeof(char));
            if (NULL != newchars)
            {   memcpy(newchars, charvec, old_maxn*sizeof(char));
                memset(newchars+old_maxn, 0,
                       (new_maxn-old_maxn)*sizeof(char));
                charbuf->charvec = newchars;
                charbuf->maxn = new_maxn;
                if (NULL != charvec)
                    FTL_FREE(charvec);
            } else
                ok = FALSE;
        }
        if (ok)
        {   charbuf->charvec[index] = ch;
            if (index+1 > charbuf->n)
                charbuf->n = index+1;
            OMIT(printf("%d:'%c' len %d\n", index, ch, charbuf->n);)
        }
        OMIT(else printf("Failed to add char index %d - max is %d\n",
                           index, charbuf->maxn);)
    }

    return ok;
}





extern charsink_t *
charsink_string_init(charsink_string_t *charbuf)
{   charbuf->charvec = NULL;
    charbuf->n = 0;
    charbuf->maxn = 0;
    return charsink_init(&charbuf->sink, &charsink_string_putc);
}







extern void
charsink_string_close(charsink_t *sink)
{   charsink_string_t *charbuf = (charsink_string_t *)sink;
    if (NULL != charbuf && NULL != charbuf->charvec)
    {   FTL_FREE(charbuf->charvec);
        charsink_string_init(charbuf);
    }
}








extern charsink_t *
charsink_string_new(void)
{   charsink_string_t *charbuf = (charsink_string_t *)
                                 FTL_MALLOC(sizeof(charsink_string_t));

    if (NULL != charbuf)
        return charsink_string_init(charbuf);
    else
    {   OMIT(printf("%s: no memory for a new string buffer (%d bytes)\n",
                      codeid(), (int)sizeof(charsink_string_t));)
        return NULL;
    }
}






extern void
charsink_string_delete(charsink_t **ref_sink)
{   if (NULL != *ref_sink)
    {   charsink_t *sink = *ref_sink;
        charsink_string_close(sink);
        FTL_FREE(sink);
        *ref_sink = NULL;
    }
}







static bool
charsink_fixstring_putc(charsink_t *sink, int ch)
{   charsink_string_t *charbuf = (charsink_string_t *)sink;
    bool ok = (NULL != charbuf);

    if (ok)
    {   size_t index = charbuf->n;

        if (index+1 >= charbuf->maxn) {  /* "+1" to allow a final \0 */
            ok = FALSE;
            OMIT(else printf("Failed to add char index %d - max is %d\n",
                               index, charbuf->maxn);)
        } else
        {   charbuf->charvec[index] = ch;
            if (index+1 > charbuf->n)
                charbuf->n = index+1;
            OMIT(printf("%d:'%c' len %d\n", index, ch, charbuf->n);)
        }
    }

    return ok;
}





static charsink_t *
charsink_fixstring_init(charsink_string_t *charbuf, char *str, size_t len)
{   charbuf->charvec = str;
    charbuf->n = 0;
    charbuf->maxn = len;
    return charsink_init(&charbuf->sink, &charsink_fixstring_putc);
}







static void
charsink_fixstring_close(charsink_t *sink)
{   charsink_string_t *charbuf = (charsink_string_t *)sink;
    if (NULL != charbuf && NULL != charbuf->charvec)
    {   charsink_fixstring_init(charbuf, charbuf->charvec, charbuf->maxn);
    }
}








extern charsink_t *
charsink_fixstring_new(char *str, size_t len)
{   charsink_string_t *charbuf = (charsink_string_t *)
                                 FTL_MALLOC(sizeof(charsink_string_t));

    if (NULL != charbuf)
        return charsink_fixstring_init(charbuf, str, len);
    else
    {   OMIT(printf("%s: no memory for a new fixed string buffer "
                      "(%d bytes)\n",
                      codeid(), (int)sizeof(charsink_string_t));)
        return NULL;
    }
}






extern void
charsink_fixstring_delete(charsink_t **ref_sink)
{   if (NULL != *ref_sink)
    {   charsink_t *sink = *ref_sink;
        charsink_fixstring_close(sink);
        FTL_FREE(sink);
        *ref_sink = NULL;
    }
}







extern void
charsink_string_buf(charsink_t *sink, const char **out_buf, size_t *out_len)
{   charsink_string_t *charbuf = (charsink_string_t *)sink;
    char *chars = charbuf->charvec;

    if (NULL == chars)
        *out_buf = "";
    else
        *out_buf = chars;

    *out_len = charbuf->n;
    if (NULL != chars)
        chars[charbuf->n] = '\0'; /* we always have room for one more char */
}














/*****************************************************************************
 *                                                                           *
 *          Character Sources                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/







/*! type of the function used to read the next character from source and
    returns it as an unsigned char cast to an int, or EOF on end of file or
    error.
*/
typedef int charsource_rdch_fn_t(charsource_t *source);


/*! type of the function used to read a block of characters from the charsource
 */    
typedef size_t charsource_read_fn_t(charsource_t *source,
                                    void *buf, size_t len);

/*! type of the function used to return the line number we are currently reading
 */
typedef int charsource_lineno_fn_t(charsource_t *source);


/*! type of the function used to end use reclaim resources allocated to the
 *  charsource
 */
typedef void charsource_delete_fn_t(charsource_t *source);




struct charsource_s
{   struct charsource_s *link;
    charsource_rdch_fn_t *rdch;
    charsource_read_fn_t *read;
    charsource_lineno_fn_t *linecount; /* return line number */
    charsource_delete_fn_t *del;     /* delete the charsource entirely */
    charsource_delete_fn_t *close;   /* delete any accumulated state */
    const char *name;
} /* charsource_t */;




#if 0
static int
charsource_rdch_from_read(charsource_t *source)
{
}
#endif



static size_t
charsource_read_from_rdch(charsource_t *source, void *buf, size_t len)
{   char *p = (char *)buf;
    char *endp = p+len;
    char ch;
    while (EOF != (ch = source->rdch(source)) && p < endp)
        *p++ = ch;

    return (p - endp);
}


/*! initialize a new charsource object
 *    @param source    - charsource_lineref object to be initialized
 *    @param rdch      - function to read a single character
 *    @param read      - NULL or function used to read a block of characters
 *    @param linecount_fn - NULL or function used to determine current line no
 *    @param delete_fn - NULL or function used to free resources held 
 *    @param close     - NULL or function used to free accumulatd resources
 *    @param name_format ... - sprintf arguments to create a user-friendly name
 *                       for the charsource
 *
 *  If read is NULL then rdch will be used to read a block of data.
 *  If linecount_fn is NULL then line number 0 is reported (otherwise line
 *  number is never reported)
 *  If close_fn is NULL then no action occurs when the charsource is closed.
 *  If delete_fn is NULL then no action occurs when the charsource is deleted.
 */
static void
charsource_init(charsource_t *source,
                charsource_rdch_fn_t *rdch, charsource_read_fn_t *read,
                charsource_lineno_fn_t *linecount_fn,
                charsource_delete_fn_t *delete_fn,
                charsource_delete_fn_t *close, const char *name_format, ...)
{   size_t namelen;
    char *namebuf = NULL;
    va_list args;

    va_start(args, name_format);
    namelen = vsnprintf(namebuf, 0, name_format, args);
    va_end(args);

    source->link = NULL;
    source->rdch = rdch;
    if (NULL != read)
        source->read = read;
    else
        source->read = &charsource_read_from_rdch;
    source->linecount = linecount_fn;
    source->del = delete_fn;
    source->close = close;

    if (namelen > 0)
    {   namebuf = FTL_MALLOC(namelen+1);
        source->name = namebuf;

        if (NULL != namebuf)
        {   va_start(args, name_format);
            (void)vsnprintf(namebuf, namelen+1, name_format, args);
            va_end(args);
            DEBUG_CHARS(DPRINTF("%s: chars - name '%s' -> [%d] '%s'\n",
                               codeid(), name_format, namelen, namebuf););
        }
    }
}





extern void
charsource_close(charsource_t *source)
{
    DEBUG_CHARS(DPRINTF("%s: chars - close '%s'%s\n",
                       codeid(), source == NULL? "<NULL>": source->name,
                       (NULL != source &&
                        NULL != source->close)? "": " (no fn)"););
    if (NULL != source && NULL != source->close)
        (*source->close)(source);
}





extern void
charsource_delete(charsource_t **ref_source)
{   if (NULL != *ref_source)
    {   charsource_t *source = *ref_source;

        DEBUG_CHARS(DPRINTF("%s: chars - delete '%s'%s\n",
                           codeid(), source == NULL? "<NULL>": source->name););
        charsource_close(source);

        if (NULL != source->name)
        {   FTL_FREE((void *)source->name);
            source->name = NULL;
        }

        if (NULL != source->del)
            (*source->del)(source);

        *ref_source = NULL;
    }
}





STATIC_INLINE const char *
charsource_name(charsource_t *source)
{   return source == NULL? "NULL":
           source->name == NULL? "<ERR>":
           source->name;
}





STATIC_INLINE int
charsource_lineno(charsource_t *source)
{
    int lineno = (source == NULL || source->linecount == NULL)? -1:
                 (*source->linecount)(source);
    DEBUG_LNO(printf("%s: charsource '%s' req line number %s - %d\n", codeid(),
                     source == NULL? "<none>": source->name,
                     source == NULL? "NULL source":
                     source->linecount == NULL? "no linecount":
                     "from linecount fn",
                     lineno););
    return lineno;
}





STATIC_INLINE int
charsource_getc_local(charsource_t *source)
{   if (source == NULL)
    {   DEBUG_CHARS(DPRINTF("%s: chars - '%s' getc EOF\n",
                           codeid(), source == NULL? "<NULL>": source->name););
        return EOF;
    }
    else
    {   return (*source->rdch)(source);
    }
}


extern int
charsource_getc(charsource_t *source)
{   return charsource_getc_local(source);
}


#define charsource_getc charsource_getc_local










static int
charsource_read(charsource_t *source, void *buf, size_t len)
{   if (source == NULL)
        return 0;
    else
    {   size_t actual = (*source->read)(source, buf, len);
        return (int)actual;
    }
}






/*****************************************************************************
 *                                                                           *
 *          File-based Character Sources                                     *
 *          ============================                                     *
 *                                                                           *
 *****************************************************************************/







typedef struct
{   charsource_t base;
    FILE *stream;
    int lineno;
} charsource_file_t;




STATIC_INLINE int
charsource_file_rdch(charsource_t *base_source)
{   charsource_file_t *source = (charsource_file_t *)base_source;
    int ch = getc(source->stream);
    if (ch == '\n')
        source->lineno++;
    return ch;
}




static size_t
charsource_file_read(charsource_t *base_source, void *buf, size_t len)
{   charsource_file_t *source = (charsource_file_t *)base_source;
    size_t bytes = fread(buf, 1, len, source->stream);
    char *p = (char *)buf;
    char *endp = p+bytes;
    while (p < endp)
        if (*p++ == '\n')
            source->lineno++;
    /*printf("%s: read %d bytes from %d - EOF %s\n",
             codeid(), bytes, len,
             feof(source->stream)? "TRUE": "FALSE"); */
    return bytes;
}




static int
charsource_file_linecount(charsource_t *base_source)
{   charsource_file_t *source = (charsource_file_t *)base_source;
    OMIT(fprintf(stderr, "%s: file '%s' line %d\n",
                 codeid(), base_source->name, source->lineno););
    return source->lineno;
}




static charsource_t *
charsource_stream_init(charsource_file_t *source,
                       charsource_delete_fn_t *delete_fn,
                       charsource_rdch_fn_t *rdch, charsource_read_fn_t *read,
                       const char *name, FILE *stream)
{   charsource_init(&source->base, rdch, read, &charsource_file_linecount,
                    delete_fn, /*close*/NULL,  "<%s>", name);
    source->stream = stream;
    source->lineno = 1; /* start numbering from 1 */
    return &source->base;
}





static void charsource_file_close(charsource_t *base_source)
{   charsource_file_t *source = (charsource_file_t *)base_source;
    if (PTRVALID(source->stream))
    {
        fclose(source->stream);
        source->stream = NULL;
    }
}





static void charsource_file_delete(charsource_t *source)
{   if (NULL != source)
        FTL_FREE(source);
}





static charsource_t *
charsource_file_init(charsource_file_t *source,
                     charsource_delete_fn_t *delete_fn,
                     const char *name, FILE *stream)
{   OMIT(printf("%s: new file '%s'\n", codeid(), name););
    charsource_init(&source->base,
                    &charsource_file_rdch, &charsource_file_read,
                    &charsource_file_linecount, delete_fn,
                    &charsource_file_close, "%s", name);
    source->stream = stream;
    source->lineno = 1; /* start at line 1 */
    return &source->base;
}





extern charsource_t *
charsource_stream_new(FILE *stream, const char *name, bool autoclose)
{   charsource_file_t *source = (charsource_file_t *)
                                FTL_MALLOC(sizeof(charsource_file_t));
    if (NULL != source)
    {   if (autoclose)
            return charsource_file_init(source, &charsource_file_delete, name,
                                        stream);
        else
            return charsource_stream_init(source, &charsource_file_delete,
                                          &charsource_file_rdch,
                                          &charsource_file_read, name, stream);
    } else
        return NULL;
}





extern charsource_t *
charsource_file_new(const char *name)
{   FILE *stream = fopen(name, "r");
    charsource_file_t *source = NULL;

    if (NULL != stream)
    {   source = (charsource_file_t *)
                 FTL_MALLOC(sizeof(charsource_file_t));

        if (NULL != source)
            charsource_file_init(source, &charsource_file_delete, name,
                                 stream);
        else
            fclose(stream);
    }

    return (charsource_t *)source;
}








/*! Open \c name on the directory path \c path writing the full name
 *  of the name (including a prefix from the directory path) to \c namebuf
 */
extern FILE *
fopen_onpath(const char *path, const char *name, size_t namelen,
             const char *mode, char *namebuf, size_t buflen)
{   FILE *stream = NULL;

    if (NULL == namebuf)
        stream = NULL;
    else if (OS_FS_ABSPATH(name))
    {
        if (1+namelen < buflen) {
            stream = fopen(name, mode);
            if (stream != NULL) {
                memcpy(namebuf, name, namelen);
                namebuf[namelen] = '\0';
            }
            OMIT(fprintf(stderr, "%s: open absolute '%s' %s\n",
                           codeid(), name, stream!=NULL?"OK":"FAILED"););
        } /* else buffer not big enough for this path prefix */
    } else {
        const char *pathpos;
        const char *pend;

        if (NULL == path)
            path = "";

        pathpos = &path[0];

        do {
            size_t prefixlen;

            pend = strchr(pathpos, OS_PATH_SEP);
            if (NULL == pend)
                pend = pathpos + strlen(pathpos);

            if (pend > pathpos+1 && OS_PATH_SEP == *(pend-1))
                prefixlen = pend - pathpos - 1;
            else
                prefixlen = pend - pathpos;

            if (prefixlen+1+namelen < buflen)
            {   if (0 == prefixlen)
                {   memcpy(namebuf, name, namelen);
                    namebuf[namelen] = '\0';
                } else
                {   memcpy(namebuf, pathpos, prefixlen);
                    namebuf[prefixlen] = OS_FS_SEP;
                    memcpy(namebuf+prefixlen+1, name, namelen);
                    namebuf[prefixlen+1+namelen] = '\0';
                }
                stream = fopen(namebuf, mode);
                OMIT(fprintf(stderr, "%s: open '%s' %s\n",
                               codeid(), namebuf, stream!=NULL?"OK":"FAILED"););
            } /* else buffer not big enough for this path prefix */

            if (OS_PATH_SEP == *pend)
                pathpos = pend+1;
            else
                pathpos = pend;

        } while (NULL == stream && (OS_PATH_SEP == *pend || '\0' != *pathpos));
    }

    return stream;
}







extern charsource_t *
charsource_file_path_new(const char *path, const char *name, size_t namelen)
{   char fullname[PATHNAME_MAX];
    FILE *stream = NULL;
    charsource_file_t *source = NULL;

    if (NULL != name)
        /*expand file name into fullname[] */
        stream = fopen_onpath(path, name, namelen, "r",
                              &fullname[0], sizeof(fullname));

    if (NULL != stream)
    {   source = (charsource_file_t *)
                 FTL_MALLOC(sizeof(charsource_file_t));

        if (NULL != source)
            charsource_file_init(source, &charsource_file_delete, &fullname[0],
                                 stream);
        else
            fclose(stream);
    }
    OMIT (else printf("%s: couldn't open file on path '%s' - [%d]%s\n",
                        codeid(), path, namelen, name););

    return (charsource_t *)source;
}









/*****************************************************************************
 *                                                                           *
 *          Socket-based Character Sources                                   *
 *          ==============================                                   *
 *                                                                           *
 *****************************************************************************/






#ifdef HAS_SOCKETS

#ifdef __unix__
#include <netinet/tcp.h>
#include <netinet/udp.h>
#endif

#ifdef _WIN32
#include <mstcpip.h>  /* Undoc'd SIO_* */
#include <ipexport.h>
#endif



typedef struct
{   charsource_t base;
    int fd;
    int recv_flags;
    int lineno;
} charsource_socket_t;




STATIC_INLINE int
charsource_socket_rdch(charsource_t *base_source)
{   charsource_socket_t *source = (charsource_socket_t *)base_source;
    char ch;
    int rc = recv(source->fd, &ch, 1, source->recv_flags);
    if (rc < 0)
        return EOF;
    else
    {
        if (ch == '\n')
            source->lineno++;
        return rc;
    }
}




static size_t
charsource_socket_read(charsource_t *base_source, void *buf, size_t len)
{   charsource_socket_t *source = (charsource_socket_t *)base_source;
    size_t bytes = recv(source->fd, buf, len, source->recv_flags);
    char *p = (char *)buf;
    char *endp = p+bytes;
    while (p < endp)
        if (*p++ == '\n')
            source->lineno++;
    return bytes;
}




static int
charsource_skt_linecount(charsource_t *base_source)
{   charsource_socket_t *source = (charsource_file_t *)base_source;
    return source->lineno;
}




static void charsource_socket_close(charsource_t *base_source)
{   charsource_socket_t *source = (charsource_socket_t *)base_source;
    if (source->fd >= 0)
        close(source->fd);
    source->fd = -1;
}





static void charsource_socket_delete(charsource_t *source)
{   if (NULL != source)
        FTL_FREE(source);
}





static charsource_t *
charsource_socket_init(charsource_socket_t *source,
                       charsource_delete_fn_t *delete_fn,
                       const char *name, int fd, int recv_flags)
{    charsource_init(&source->base,
                     &charsource_socket_rdch, &charsource_socket_read,
                     &charsource_socket_linecount,
                     delete_fn, &charsource_socket_close, "inet:%s", name);
     source->fd = fd;
     source->recv_flags = recv_flags;
     source->lineno = 1; /* start at line 1 */
     return &source->base;
}





extern charsource_t *
charsource_stream_skt_new(int fd, int recv_flags,,
                          const char *name, bool autoclose)
{   charsource_socket_t *source = (charsource_socket_t *)
                                  FTL_MALLOC(sizeof(charsource_socket_t));
    if (NULL != source)
    {   if (autoclose)
            return charsource_socket_init(source, &charsource_socket_delete,
                                          name, fd, recv_flags);
        else
            return charsource_stream_init(source, &charsource_socket_delete,
                                          &charsource_socket_rdch,
                                          &charsource_socket_read, name,
                                          fd, recv_flags);
    } else
        return NULL;
}





extern charsource_t *
charsource_socket_connect_new(const char *name)
{   int fd = -1;
    charsource_socket_t *source = NULL;

    if (fd >= 0)
    {   source = (charsource_socket_t *)
                 FTL_MALLOC(sizeof(charsource_socket_t));

        if (NULL != source)
            charsource_socket_init(source, &charsource_socket_delete, name,
                                   fd, recv_flags);
        else
            fclose(skt);
    }

    return (charsource_t *)source;
}



#endif







/*****************************************************************************
 *                                                                           *
 *          Buffer-based Character Sources                                   *
 *          ==============================                                   *
 *                                                                           *
 *****************************************************************************/





typedef struct
{   charsource_t base;
    const char *string;  /* pointer to the string buffer */
    const char *eos;     /* pointer 1 char past last character (e.g. to \0) */
    const char *pos;     /* current read position */
} charsource_string_t;




STATIC_INLINE int
charsource_string_rdch(charsource_t *base_source)
{   charsource_string_t *source = (charsource_string_t *)base_source;
    OMIT(fprintf(stdout, "%s: string source '%s' rdch\n",
                       codeid(), base_source->name););
    return source->string == NULL || source->pos >= source->eos? EOF:
           *source->pos++;
}




static size_t
charsource_string_read(charsource_t *base_source, void *buf, size_t len)
{   charsource_string_t *source = (charsource_string_t *)base_source;
    if (source->string == NULL || source->pos >= source->eos)
        return 0;
    else
    {   int maxlen = (int)(source->eos - source->pos);
        if ((int)len > maxlen)
        {   memcpy(buf, source->pos, maxlen);
            source->pos = source->eos;
            return maxlen;
        } else
        {   memcpy(buf, source->pos, len);
            source->pos += len;
            return len;
        }
    }
}




static int
charsource_string_linecount(charsource_t *base_source)
{   charsource_string_t *source = (charsource_string_t *)base_source;
    const char *p = source->string;
    const char *endp = source->pos;
    int lineno = 0; /* start at line number 0 */
    while (p<endp)
        if (*p++ == '\n')
            lineno++;
    OMIT(fprintf(stdout, "%s: string source '%s' char %d eval line %d\n",
                 base_source->name, codeid(), (int)(endp-p), lineno););
    return lineno;
}





/* initialize a string that may or may not be freed */
static charsource_t *
charsource_string_anyinit(charsource_string_t *source,
                          charsource_delete_fn_t *delete_fn,
                          charsource_delete_fn_t *close,
                          const char *name, const char *string, size_t len)
{   charsource_init(&source->base,
                    &charsource_string_rdch, &charsource_string_read,
                    &charsource_string_linecount, delete_fn, close,
                    "$%s", name);
    source->string = string;
    source->eos = string+len;
    source->pos = string;
    return &source->base;
}






static void charsource_string_close(charsource_t *base_source)
{   charsource_string_t *source = (charsource_string_t *)base_source;
    if (NULL != source->string)
    {   FTL_FREE((void *)source->string);
        source->string = NULL;
        source->eos = NULL;
        source->pos = NULL;
    }
}




#if defined(USE_READLINE) || defined(USE_LINENOISE)

/* sets a FTL_MALLOC'd string */
static void charsource_string_setstring(charsource_t *base_source,
                                        const char *string, size_t len)
{   charsource_string_t *source = (charsource_string_t *)base_source;

    charsource_string_close(base_source);
    if (NULL != source)
    {   source->string = string;
        source->eos = string+len;
        source->pos = string;
    }
}

#endif






/* initialize with a copy of the argument string */
STATIC_INLINE charsource_t *
charsource_string_init(charsource_string_t *source,
                       charsource_delete_fn_t *delete_fn,
                       const char *name, const char *string, size_t len)
{   char *strcopy = NULL;

    if (NULL != string)
       strcopy = (char *)FTL_MALLOC(len+1);

    if (NULL != strcopy)
    {   memcpy(strcopy, string, len);
        strcopy[len] = '\0';
    } else
        len = 0;

    return charsource_string_anyinit(source, delete_fn,
                                     &charsource_string_close,
                                     name, strcopy, len);
}





/* initialize with a constant string (not copied into malloc'd buffer */
STATIC_INLINE charsource_t *
charsource_cstring_init(charsource_string_t *source,
                        charsource_delete_fn_t *delete_fn,
                        const char *name, const char *string, size_t len)
{   return charsource_string_anyinit(source, delete_fn, /*close*/NULL,
                                     name, string, len);
}





static void charsource_string_delete(charsource_t *source)
{   if (NULL != source)
        FTL_FREE(source);
}





extern charsource_t *
charsource_string_new(const char *name, const char *string, size_t len)
{   charsource_string_t *source = NULL;

    if (NULL != string)
    {   source = (charsource_string_t *)
                 FTL_MALLOC(sizeof(charsource_string_t));
        if (NULL != source)
            charsource_string_init(source, &charsource_string_delete,
                                   name, string, len);
    }
    return (charsource_t *)source;
}





extern charsource_t *
charsource_cstring_new(const char *name, const char *string, size_t len)
{   charsource_string_t *source = NULL;

    if (NULL != string)
    {   source = (charsource_string_t *)
                 FTL_MALLOC(sizeof(charsource_string_t));
        if (NULL != source)
            charsource_cstring_init(source, &charsource_string_delete,
                                    name, string, len);
    }
    return (charsource_t *)source;
}











/*****************************************************************************
 *                                                                           *
 *          External String Reference Character Sources                      *
 *          ===========================================                      *
 *                                                                           *
 *****************************************************************************/





typedef struct
{   charsource_t base;
    const char **ref_string;  /* pointer to the string buffer pointer */
    const char *line1;        /* pointer to the initial string buffer */
    int line1no;
} charsource_lineref_t;




/* not expected to be used */
STATIC_INLINE int
charsource_lineref_rdch(charsource_t *base_source)
{   charsource_lineref_t *source = (charsource_lineref_t *)base_source;
    return source->ref_string == NULL || *source->ref_string == NULL ||
           (*source->ref_string)[0] == '\0'? EOF:
           *(*source->ref_string)++;
}






static void
charsource_lineref_close(charsource_t *base_source)
{   charsource_lineref_t *source = (charsource_lineref_t *)base_source;
    source->ref_string = NULL;
    source->line1 = NULL;
    source->line1no = -1;
}




static int
charsource_lineref_linecount(charsource_t *base_source)
{   charsource_lineref_t *source = (charsource_lineref_t *)base_source;
    if (source == NULL || source->ref_string == NULL)
        return 0;
    else
    {
        const char *endp = *source->ref_string;
        const char *p = source->line1;
        int lineno = source->line1no;

        while (p<endp)
            if (*p++ == '\n')
                lineno++;
        OMIT({
            const char *endless6 = endp-6;
            int len_beg = 6;
            int len_end = 6;
            p = source->line1;
            if (p+len_beg > endp)
                len_beg = endp-p;
            if (endless6 < p+len_beg)
            {   len_end = endp-(p+len_beg);
                endless6 = p+len_beg;
            }
            fprintf(stdout, "%s: %d chars '%.*s...%.*s'\n",
                    codeid(), (int)(endp-p), len_beg, p, len_end, endless6);
            fprintf(stdout, "%s: lineref source '%s' eval line %d\n",
                   codeid(), base_source->name, lineno);
        });
        return lineno;
    }
}



/*! initialize a string that may or may not be freed
 *    @param source    - charsource_lineref object to be initialized
 *    @param delete_fn - NULL or function used to free resources held
 *    @param rewind    - true if name/lineno is from previous location
 *    @param name      - name of the charsource where charsource defined
 *    @param lineno    - line number on which the charsource is being defined
 *    @param ref_string - string parse location from which input is being taken
 *                       (the string at this location is updated as characgters
 *                        are read)
 *  No action is taken on charsource_delete if delete_fn is NULL.
 */
STATIC_INLINE charsource_t *
charsource_lineref_init(charsource_lineref_t *source,
                        charsource_delete_fn_t *delete_fn, bool rewind,
                        const char *name, int lineno, const char **ref_string)
{
    int namelen = strlen(name);
    char lastch = namelen==0? '\0': name[namelen-1];
    if (rewind)
        charsource_init(&source->base,
                        &charsource_lineref_rdch, /*read*/NULL,
                        &charsource_lineref_linecount, delete_fn,
                        &charsource_lineref_close, "%s:+",
                        name); /* use same name as source */
    else if (lastch == '+') {
        bool endsinnum = namelen < 2? FALSE: isdigit(name[namelen-2]);
        charsource_init(&source->base,
                        &charsource_lineref_rdch, /*read*/NULL,
                        &charsource_lineref_linecount, delete_fn,
                        &charsource_lineref_close, "%.*s%d+",
                        endsinnum? namelen: namelen-1, name, lineno);
    } else
        charsource_init(&source->base,
                        &charsource_lineref_rdch, /*read*/NULL,
                        &charsource_lineref_linecount, delete_fn,
                        &charsource_lineref_close, "%s:%d",
                        name, lineno);

    OMIT(printf("%s: src lineref for '%s' line %d is '%s'\n", codeid(),
                name, lineno, &source->base.name[0]););
    source->ref_string = ref_string;
    source->line1no = rewind? lineno: 0;
    source->line1 = *ref_string;
    return &source->base;
}





#if 0
static void
charsource_lineref_delete(charsource_t *source)
{   if (NULL != source)
        FTL_FREE(source);
}





extern charsource_t *
charsource_lineref_new(const char *name, int lineno, bool rewind, const char **ref_string)
{   charsource_lineref_t *source = NULL;

    if (NULL != ref_string)
    {   source = (charsource_lineref_t *)
                 FTL_MALLOC(sizeof(charsource_lineref_t));
        if (NULL != source)
            charsource_lineref_init(source, &charsource_lineref_delete,
                                    rewind, name, lineno, ref_string);
    }
    return (charsource_t *)source;
}
#endif









/*****************************************************************************
 *                                                                           *
 *          Single Character Character Sources                               *
 *          ==================================                               *
 *                                                                           *
 *****************************************************************************/






typedef struct
{   charsource_t base;
    bool read;
    int ch;
} charsource_ch_t;





static int
charsource_ch_rdch(charsource_t *base_source)
{   charsource_ch_t *source = (charsource_ch_t *)base_source;
    if (source->read)
        return EOF;
    else
    {   source->read = TRUE;
        return source->ch;
    }
}





static int
charsource_ch_linecount(charsource_t *base_source)
{   charsource_ch_t *source = (charsource_ch_t *)base_source;
    int lineno = 0; /* start at line number 0 */
    if (source->read)
        return lineno;
    else
    {   if (source->ch == '\n')
            return lineno+1;
        else
            return lineno;
    }
}



/* initialize a string that may or may not be freed */
static charsource_t *
charsource_ch_init(charsource_ch_t *source, charsource_delete_fn_t *delete_fn,
                   int ch)
{   charsource_init(&source->base, &charsource_ch_rdch, /*read*/NULL,
                    charsource_ch_linecount, delete_fn, /*close*/NULL,
                    "<%s>", "UNRDCH");
    source->read = FALSE;
    source->ch = ch;
    return &source->base;
}






static void
charsource_ch_delete(charsource_t *source)
{   if (NULL != source)
        FTL_FREE(source);
}





extern charsource_t *
charsource_ch_new(int ch)
{   charsource_ch_t *source = (charsource_ch_t *)
                              FTL_MALLOC(sizeof(charsource_ch_t));

    if (NULL != source)
        charsource_ch_init(source, &charsource_ch_delete, ch);

    return (charsource_t *)source;
}











#if 0 /* not necessary */
/*****************************************************************************
 *                                                                           *
 *          No Character Character Sources                                   *
 *          ==============================                                   *
 *                                                                           *
 *****************************************************************************/







/* This is a pseudo character source providing no characters.
 *
 * It's only function is to record a character source and line number so that
 * we can temporarily appear to rewind the input to a former input position -
 * so as to cater for the execution of processed data already read in from the
 * main parser input, for example.
 */





typedef struct
{   charsource_t base;
    const char *rewind_source;
    int rewind_lineno;
} charsource_rewind_t;







static int
charsource_rewind_rdch(charsource_t *base_source)
{   return EOF;
    /* it's job is done once read character is called - we revert to the 'real'
       source and line number
    */
}





static int
charsource_rewind_linecount(charsource_t *base_source)
{   /* charsource_rewind_t *source = (charsource_rewind_t *)base_source; */
    /* always the stored line number */
    return -1;
}




/* initialize a string that may or may not be freed */
static charsource_t *
charsource_rewind_init(charsource_rewind_t *source,
                       charsource_delete_fn_t *delete_fn,
                       const char *rewind_source, int rewind_lineno)
{   charsource_init(&source->base, &charsource_rewind_rdch, /*read*/NULL,
                    charsource_rewind_linecount, delete_fn, /*close*/NULL,
                    "%s", rewind_source);
    source->rewind_lineno = rewind_lineno;
    return &source->base;
}






static void
charsource_rewind_delete(charsource_t *source)
{   if (NULL != source)
        FTL_FREE(source);
}





extern charsource_t *
charsource_rewind_new(const char *rewind_source, int rewind_lineno)
{   charsource_rewind_t *source = (charsource_rewind_t *)
                              FTL_MALLOC(sizeof(charsource_rewind_t));

    if (NULL != source)
        charsource_rewind_init(source, &charsource_rewind_delete,
                               rewind_source, rewind_lineno);

    return (charsource_t *)source;
}


#endif








/*****************************************************************************
 *                                                                           *
 *          Prompting Character Sources                                      *
 *          ===========================                                      *
 *                                                                           *
 *****************************************************************************/





#if !defined(USE_READLINE) && !defined(USE_LINENOISE)







typedef struct
{   charsource_file_t file_base;
    const char *prompt;
    bool prompt_needed;
    FILE *prompt_stream;
} charsource_prompting_t;






static int
charsource_prompting_rdch(charsource_t *base_source)
{   int ch;
    charsource_prompting_t *source = (charsource_prompting_t *)base_source;
    if (source->prompt_needed)
    {   fprintf(source->prompt_stream, "%s", source->prompt);
        fflush(source->prompt_stream);
        source->prompt_needed = FALSE;
    }
    ch = charsource_file_rdch(&source->file_base.base);
    if (ch == '\n')
        source->prompt_needed = TRUE;
    return ch;
}





static charsource_t *
charsource_prompting_init(charsource_prompting_t *source,
                          charsource_delete_fn_t *delete_fn,
                          FILE *consolein, FILE *consoleout, const char *prompt)
{
    OMIT(printf("%s: create prompting source\n", codeid()););
    charsource_stream_init(&source->file_base, delete_fn,
                            &charsource_prompting_rdch, /*read*/NULL,
                            "*console*", consolein);
     source->prompt_stream = consoleout;
     source->prompt_needed = TRUE;
     source->prompt = prompt;
     return &source->file_base.base;
}







static void
charsource_prompting_delete(charsource_t *base_source)
{   if (NULL != base_source)
    {   charsource_prompting_t *source = (charsource_prompting_t *)base_source;
        FTL_FREE(source);
    }
}





extern charsource_t *
charsource_prompting_new(FILE *consolein, FILE *consoleout, const char *prompt)
{   charsource_prompting_t *source =
        (charsource_prompting_t *)
        FTL_MALLOC(sizeof(charsource_prompting_t));

    if (NULL != source)
        charsource_prompting_init(source, &charsource_prompting_delete,
                                  consolein, consoleout, prompt);

    return (charsource_t *)source;
}




#define charsource_console_new(prompt) \
        charsource_prompting_new(stdin, stdout, prompt)



#endif /* USE_READLINE or USE_LINENOISE */





/*****************************************************************************
 *                                                                           *
 *          Readline History Files                                           *
 *          ======================                                           *
 *                                                                           *
 *****************************************************************************/




#ifndef USE_READLINE_HISTORY
#ifdef USE_READLINE
#define USE_READLINE_HISTORY 1
#else
#define USE_READLINE_HISTORY 0
#endif
#endif


#ifndef USE_LINENOISE_HISTORY
#ifdef USE_LINENOISE
#define USE_LINENOISE_HISTORY 1
#else
#define USE_LINENOISE_HISTORY 0
#endif
#endif



#if USE_READLINE_HISTORY
#include "readline/history.h"
#endif

#if USE_LINENOISE_HISTORY
#include "linenoise.h"
#endif



#if USE_READLINE_HISTORY || USE_LINENOISE_HISTORY

#ifdef HAS_OPENDIR
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif




typedef struct
{   char *this_history;       /* history file for this local invocation */
    char *global_history;     /* global history file (all invocations) */
    bool this_is_open;        /* history was found in the local history file */
    bool global_is_open;      /* history was found in the global history file */
} history_file_t;





static bool
history_open(history_file_t *hist)
{   const char *homedir = getenv(ENV_FTL_HOMEDIR);
    const char *history_home = codeid();
    const char *history_instance = "local";
    char hostname[255];

    hostname[0] = '\0';
    if (0 == gethostname(hostname, 255))
        history_instance = &hostname[0];

    hist->this_history = FTL_MALLOC(strlen(homedir) +
                                    strlen(ENV_FTL_HOMEDIR_HOME) +
                                    strlen(history_home) +
                                    strlen(READLINE_HOME_DIR_PREFIX) +
                                    strlen(READLINE_HISTORY_NAME) +
                                    strlen(history_instance) +
                                    strlen(READLINE_HISTORY_EXTENSION) + 4);

    hist->this_is_open = FALSE;
    hist->global_is_open = FALSE;

#ifdef HAS_OPENDIR
    if (NULL != hist->this_history)
    {
        /* Check directory exists, or try to create it */
        sprintf(hist->this_history, "%s%s"OS_FILE_SEP
                                    READLINE_HOME_DIR_PREFIX "%s",
                homedir, ENV_FTL_HOMEDIR_HOME, history_home);

        {   DIR *rldir = opendir(hist->this_history);

            if (rldir == NULL)
            {   if (0 != mkdir(hist->this_history, 0770))
                {   FTL_FREE(hist->this_history);
                    hist->this_history = NULL;
                }
            } else
                closedir(rldir);
        }
    }
#endif

    if (NULL != hist->this_history) /* dir valid and memory available */
    {
        /* Set up the actual name. */
        sprintf(hist->this_history, "%s%s"OS_FILE_SEP
                                    READLINE_HOME_DIR_PREFIX "%s" OS_FILE_SEP
                                    READLINE_HISTORY_NAME "-%s"
                                    READLINE_HISTORY_EXTENSION,
                homedir, ENV_FTL_HOMEDIR_HOME, history_home, history_instance);

        hist->this_is_open =
#if USE_READLINE_HISTORY
            (read_history(hist->this_history) == 0);
#endif
#if USE_LINENOISE_HISTORY
            (linenoiseHistoryLoad(hist->this_history) == 0);
#endif

        hist->global_history = FTL_MALLOC(strlen(hist->this_history) +
                                          strlen(homedir) +
                                          strlen(ENV_FTL_HOMEDIR_HOME) +
                                          strlen(history_home) +
                                          strlen(READLINE_HOME_DIR_PREFIX) +
                                          strlen(READLINE_HISTORY_EXTENSION)
                                          + 3);

        if (hist->global_history != NULL)
        {   sprintf(hist->global_history, "%s%s"OS_FILE_SEP
                                    READLINE_HOME_DIR_PREFIX "%s" OS_FILE_SEP
                                    READLINE_HISTORY_NAME
                                    READLINE_HISTORY_EXTENSION,
                    homedir, ENV_FTL_HOMEDIR_HOME, history_home);
        }

        if (hist->this_is_open)
            hist->global_is_open = FALSE;
        else
        {
            hist->global_is_open =
#if USE_READLINE_HISTORY
                (read_history(hist->global_history) == 0);
#endif
#if USE_LINENOISE_HISTORY
                (linenoiseHistoryLoad(hist->global_history) == 0);
#endif
        }

        return TRUE;
    } else
    {   hist->global_history = NULL;
        return FALSE;
    }
}





static void
history_close(history_file_t *hist)
{
    if (hist->this_history != NULL)
    {
#if USE_LINENOISE_HISTORY
         linenoiseHistorySave(hist->this_history);
#endif
#if USE_READLINE_HISTORY
        if (hist->this_is_open)
            write_history(hist->this_history);
        else
        {   append_history(READLINE_HISTORY_MAXLEN, hist->this_history);
            history_truncate_file(hist->this_history, READLINE_HISTORY_MAXLEN);
        }
#endif
        FTL_FREE(hist->this_history);
        hist->this_history = NULL;
        hist->this_is_open = FALSE;
    }

    if (hist->global_history != NULL)
    {
#if USE_LINENOISE_HISTORY
        linenoiseHistorySave(hist->global_history);
#endif
#if USE_READLINE_HISTORY
        if (hist->global_is_open)
            write_history(hist->global_history);
        else
        {   append_history(READLINE_HISTORY_MAXLEN, hist->global_history);
            history_truncate_file(hist->global_history,
                                  READLINE_HISTORY_MAXLEN);
        }
#endif
        FTL_FREE(hist->global_history);
        hist->global_history = NULL;
        hist->global_is_open = FALSE;
    }
}



#if USE_LINENOISE_HISTORY
static void add_history(const char *line)
{   (void)linenoiseHistoryAdd(line);
}
#endif


#else/* USE_READLINE_HISTORY */


typedef void *history_file_t ;
#define history_open(histfile) (TRUE)
#define history_close(histfile)
#define add_history(line)


#endif /* USE_READLINE_HISTORY */








/*****************************************************************************
 *                                                                           *
 *          Readline Character Sources                                       *
 *          ==========================                                       *
 *                                                                           *
 *****************************************************************************/






#ifdef USE_READLINE
#include "readline/readline.h"
#endif

#ifdef USE_LINENOISE
#include "linenoise.h"
#endif


#if defined(USE_READLINE) || defined(USE_LINENOISE)



#if 0
/* Throw terminal interrupt exception */
static bool cause_sigint(void); /* forward reference to Ctrl-C handler */
#endif



typedef struct
{   charsource_string_t string_base;
    int lineno;
    const char *prompt;
    history_file_t hist;
    bool prompt_needed;
} charsource_readline_t;






static int
charsource_readline_rdch(charsource_t *base_source)
{   if (NULL == base_source)
        return EOF;
    else
    {   charsource_readline_t *source = (charsource_readline_t *)base_source;
        int ch = charsource_string_rdch(base_source);

        if (source->prompt_needed)
        {   char *nextline;

            DEBUG_CONSOLE(DPRINTF("calling readline\n");)
#ifdef USE_READLINE
            nextline = readline(source->prompt);
#endif
#ifdef USE_LINENOISE
            do {
                errno = 0;
                nextline = linenoise(source->prompt);
#if 0
                if (nextline == NULL
#ifdef EAGAIN
                    && errno == EAGAIN
#endif
                )
                       (void)cause_sigint();
                       /* probably we won't have a try block */
#endif
            } while (nextline == NULL
#ifdef EAGAIN
                     && errno == EAGAIN
#endif
                    );

            OMIT(fprintf(stderr, "ln: %s%d %s rc %d\n",
                         nextline==NULL? "NONE " :"",
                         nextline==NULL? 0: (int)strlen(nextline),
                         feof(stdin)? "EOF": "", errno);
            );
#endif
            source->prompt_needed = FALSE;
            if (nextline != NULL)
            {   source->lineno++;
                charsource_string_setstring(base_source, nextline,
                                            strlen(nextline));
                DEBUG_CONSOLE(DPRINTF("got '%s'\n", nextline);)
                ch = charsource_string_rdch(base_source);
                if (ch == EOF)
                {   ch='\n';
                    source->prompt_needed = TRUE;
                } else
                    add_history(nextline);
                DEBUG_CONSOLE(DPRINTF("first ch: '%c'\n", ch);)
            } else
            {   charsource_string_close(base_source);
                ch = EOF;
            }
        } else
        if (ch == EOF)
        {   ch='\n';
            source->prompt_needed = TRUE;
        }
        DEBUG_CONSOLE(else DPRINTF("next ch: '%c'\n", ch);)

        return ch;
    }
}




static int
charsource_readline_linecount(charsource_t *base_source)
{   charsource_readline_t *source = (charsource_readline_t *)base_source;
    OMIT(fprintf(stderr, "%s: readline '%s' line %d\n",
                 codeid(), base_source->name, source->lineno););
    return source->lineno;
}




static charsource_t *
charsource_readline_init(charsource_readline_t *source,
                         charsource_delete_fn_t *delete_fn, const char *prompt)
{   charsource_t *rl;
    source->lineno = 1; /* start at line 1 */
    source->prompt = prompt;
    source->prompt_needed = TRUE;
#ifdef USE_LINENOISE
    linenoiseConsoleInit();
#endif
    OMIT(printf("%s: create readline init\n", codeid()););
    rl = charsource_string_init(&source->string_base, delete_fn, "*console*",
                                /* string */NULL, 0);
    source->string_base.base.rdch = &charsource_readline_rdch;
    source->string_base.base.linecount = &charsource_readline_linecount;
    (void)history_open(&source->hist);
    return rl;
}







static void
charsource_readline_delete(charsource_t *base_source)
{   if (NULL != base_source)
    {   charsource_readline_t *source = (charsource_readline_t *)base_source;
        history_close(&source->hist);
        FTL_FREE(source);
    }
}








extern charsource_t *
charsource_readline_new(const char *prompt)
{   charsource_readline_t *source =
        (charsource_readline_t *)
        FTL_MALLOC(sizeof(charsource_readline_t));

    if (NULL != source)
        charsource_readline_init(source, &charsource_readline_delete, prompt);

    return (charsource_t *)source;
}





#define charsource_console_new(prompt) \
        charsource_readline_new(prompt)




#endif /* USE_READLINE */





/*****************************************************************************
 *                                                                           *
 *          Charsource Stacks                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/







extern instack_t *
instack_init(instack_t *ref_stack)
{   *ref_stack = NULL;
    return ref_stack;
}




extern const char *
instack_source(instack_t stack)
{   return charsource_name(stack);
}




extern int
instack_lineno(instack_t stack)
{   return charsource_lineno(stack);
}




extern charsource_t *
instack_charsource(instack_t stack)
{   return stack;
}




extern void
instack_push(instack_t *ref_stack, charsource_t *source)
{   source->link = *ref_stack;
    *ref_stack = source;
}




extern charsource_t *
instack_pop(instack_t *ref_stack)
{   charsource_t *popped = *ref_stack;
    if (NULL != popped)
        *ref_stack = popped->link;
    return popped;
}




extern bool /* not empty */
instack_popdel(instack_t *ref_stack)
{   charsource_t *popped = instack_pop(ref_stack);
    if (NULL != popped)
    {   charsource_delete(&popped);
        return TRUE;
    } else
        return FALSE;
}



/*! Remove the given charsource if it is still to be found on the stack
 *  return TRUE if the charsource was found (and removed)
 */
extern bool
instack_remove(instack_t *ref_stack, charsource_t *onstack)
{   
    if (onstack == NULL)
        /* nothing to take off - aleady done */
        return TRUE;
    else
    {
        while (*ref_stack != NULL && *ref_stack != onstack)
            ref_stack = &(*ref_stack)->link;
        if (*ref_stack == onstack)
        {   /* take it out of the stack */
            *ref_stack = (*ref_stack)->link;
            return TRUE;
        } else
            return FALSE;
    }
}




extern int
instack_getc(instack_t *ref_stack)
{   int ch;

    while (EOF == (ch = charsource_getc(*ref_stack)) &&
           instack_popdel(ref_stack))
        continue;

    return ch;
}






extern int
instack_ungetc(instack_t *ref_stack, int ch)
{   charsource_t *unrdch = charsource_ch_new(ch);

    if (NULL != unrdch)
        instack_push(ref_stack, unrdch);

    return ch;
}




static int
charsource_line_print(charsource_t *src, charsink_t *sink,
                      const char *srcname, int lineno, const char *line)
{
    int n = 0;
    int srcnamelen = srcname == NULL? 0: strlen(srcname);
    char lastch = srcnamelen <= 0? '\0': srcname[srcnamelen-1];

    if (srcname == NULL)
        srcname = "<no source>";

    OMIT(fprintf(stdout, "%s: read charsource '%s' %s:%d\n", codeid(),
                 src->name, srcname, charsource_lineno(src)););
    if (NULL == sink)
    {   n += fprintf(stderr, "%s %s", codeid(), srcname);
        if (lineno >= 0)
        {
            if (lastch != '+')
                n += fprintf(stderr, ":");
            n += fprintf(stderr, "%d", lineno);
        }
        n += fprintf(stderr, "%s", line);
    } else
    {   n += charsink_sprintf(sink, "%s %s", codeid(), srcname);
        if (lineno >= 0)
        {
            if (lastch != '+')
                n += charsink_sprintf(sink, ":");
            n += charsink_sprintf(sink, "%d", lineno);
        }
        n += charsink_sprintf(sink, "%s", line);
    }
    return n;
}






static void
instack_outer(instack_t stack, 
              const char **out_outer_srcname, int *out_outer_lineno)
{   charsource_t *src = stack;
    if (NULL != src)
    {   /* print the whole stack except the outermost level */
        while (NULL != src->link)
            src = src->link;
        *out_outer_lineno = charsource_lineno(src);
        *out_outer_srcname = charsource_name(src);
    }
}





static int
instack_print(instack_t stack, charsink_t *sink)
{   charsource_t *src = stack;
    int n = 0;
    if (NULL != src)
    {   /* print the whole stack except the outermost level */
        while (NULL != src->link)
        {   n += charsource_line_print(src, sink, charsource_name(src),
                                       charsource_lineno(src), " in\n");
            src = src->link;
        }
    }
    return n;
}








/*****************************************************************************
 *                                                                           *
 *          Command Line Source                                              *
 *          ===================                                              *
 *                                                                           *
 *****************************************************************************/






typedef struct {
    int lineno;
    char posname[FTL_LINESOURCE_NAME_MAX];
} code_place_t;




STATIC_INLINE void
code_place_unset(code_place_t *place)
{   place->posname[0] = '\0';
    place->lineno = -1;
}





STATIC_INLINE void
code_place_set(code_place_t *place, const char *sourcename, int lineno)
{   size_t sourcename_len = strlen(sourcename);
    place->lineno = lineno;
    if (sourcename_len < sizeof(place->posname)-1)
        strcpy(&place->posname[0], sourcename);
    else {
        const char* prefix = "...";
        sourcename += sourcename_len - sizeof(place->posname) + 1;
        strncpy(&place->posname[0], sourcename, sizeof(place->posname));
        place->posname[sizeof(place->posname)-1] = '\0';
        memcpy(&place->posname[0], prefix, strlen(prefix));
    }
}




STATIC_INLINE bool
code_place_get(const code_place_t *place,
               const char **out_source, int *out_lineno)
{   if (place == NULL)
    {
        *out_source = "";
        *out_lineno = 0;
        return FALSE;
    }
    else
    {
        *out_source = &place->posname[0];
        *out_lineno = place->lineno;
        return TRUE;
    }
}




STATIC_INLINE bool
code_place_eq(code_place_t *place, const char *source, int lineno)
{   return source != NULL && 0 == strcmp(&place->posname[0], source) &&
           place->lineno == lineno;
}








struct linesource_s
{   instack_t in;               /* stack of charsources */
    char escape;                /* escape char - e.g. \ */
    char string;                /* string introduction char - e.g. " */
    char commandsep;            /* command separator char - e.g. ; */
    char comment;               /* comment introduction char - e.g. # */
    bool eof;                   /* true when charsources exhausted */
    code_place_t place;         /* position at start of current line */
    code_place_t report_place;  /* position of last report */
} /* linesource_t */;





static void
linesource_init(linesource_t *source)
{   instack_init(&source->in);
    source->escape = '\\';
    source->string = '\"';
    source->commandsep = ';';
    source->comment = '#';
    source->eof = FALSE;
    code_place_unset(&source->place);
    code_place_unset(&source->report_place);
}




extern void
linesource_save(linesource_t *lines, linesource_t *saved)
{   if (NULL != saved)
        memcpy(saved, lines, sizeof(linesource_t));
}





extern void
linesource_restore(linesource_t *lines, linesource_t *saved)
{   if (NULL != saved)
        memcpy(lines, saved, sizeof(linesource_t));
}





extern void
linesource_push(linesource_t *lines, charsource_t *source)
{   lines->eof = FALSE;
    instack_push(&lines->in, source);
}





extern charsource_t *
linesource_pop(linesource_t *lines)
{   return instack_pop(&lines->in);
}




/*! Remove the given charsource if it is still to be found on the stack
 *  of line sources, return TRUE if the charsource was found (and removed)
 */
extern bool
linesource_remove(linesource_t *lines, charsource_t *onstack)
{   return instack_remove(&lines->in, onstack);
}




STATIC_INLINE bool /* not empty */
linesource_popdel(linesource_t *lines)
{   return instack_popdel(&lines->in);
}





extern const char *
linesource_source(linesource_t *lines)
{   return &lines->place.posname[0];
}




STATIC_INLINE instack_t
linesource_instack(linesource_t *lines)
{   return lines->in;
}




STATIC_INLINE charsource_t *
linesource_charsource(linesource_t *lines)
{   return lines->in;
}




extern int
linesource_lineno(linesource_t *lines)
{   return lines->place.lineno;
}




extern bool
linesource_eof(linesource_t *source)
{   return source->eof;
}




static int
vreport_line_at(charsink_t *sink, linesource_t *source,
                const char *srcpos, int lineno, 
                const char *format, va_list ap)
{   if (NULL == source)
    {
        if (NULL == sink)
            return vfprintf(stderr, format, ap);
        else
            return charsink_vsprintf(sink, format, ap);
    } else {
        int n = 0;

        DEBUG_LNO(printf("%s: sink is %sset - source at %s:%d\n",
                         codeid(),NULL==sink? "un":"", srcpos, lineno););

        n += charsource_line_print(linesource_charsource(source), sink,
                                   srcpos, lineno, ": ");

        if (NULL == sink)
            return n + vfprintf(stderr, format, ap);
        else
            return n + charsink_vsprintf(sink, format, ap);
    }
}




extern int
vreport_line(charsink_t *sink, linesource_t *source, 
             const char *format, va_list ap)
{   int n;
    if (source == NULL)
        n = vreport_line_at(sink, source, NULL, -1, format, ap);
    else
        n = vreport_line_at(sink, source, linesource_source(source),
                            linesource_lineno(source), format, ap);
    return n;
}





extern int
vreport(charsink_t *sink, linesource_t *source, const char *format, va_list ap)
{   int n = 0;
    const char *report_source = NULL;
    int report_lineno = -1;

    if (source != NULL)
    {   charsource_t *src = linesource_charsource(source);
        const char *current_source = charsource_name(src);
        int current_lineno = charsource_lineno(src);
        
        instack_outer(source->in, &report_source, &report_lineno);

        /* report rest of the stack if it is different to last time */
        if (!code_place_eq(&source->report_place,
                           current_source, current_lineno))
        {
            OMIT(printf("%s: set error place %s:%d\n",
                        codeid(), sourcename, lineno););
            code_place_set(&source->report_place,
                           current_source, current_lineno);
            n += instack_print(source->in, sink);
        } else {
            /* report only the (repeated) inner position */
            report_source = current_source;
            report_lineno = current_lineno;
        }
    } 
    
    return n+vreport_line_at(sink, source, report_source, report_lineno,
                             format, ap);
}





extern int
report(charsink_t *sink, linesource_t *source, const char *format, ...)
{   va_list args;
    int len;

    va_start(args, format);
    len = vreport(sink, source, format, args);
    va_end(args);

    return len;
}





extern int
report_line(charsink_t *sink, linesource_t *source, const char *format, ...)
{   va_list args;
    int len;

    va_start(args, format);
    len = vreport_line(sink, source, format, args);
    va_end(args);

    return len;
}





STATIC_INLINE char escape_ch(char ch)
{   switch (ch)
    {   case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case 'a': return '\a';
        case 'b': return '\b';
        case 'd': return '\x7f';
        case 'f': return '\f';
        case 'v': return '\v';
        case '0': return '\0';
        default: return ch;
    }
}




/* Set the start source and line number for the next linesource_read.
*/
extern void
linesource_start_set(linesource_t *source, code_place_t *start_line)
{
    if (!source->eof)
    {   /* remember the position at the start of this line */
        const char *posname;
        int lineno;
        code_place_get(start_line, &posname, &lineno);
        DEBUG_LNO(
            printf("%s: source place %s:%d\n", codeid(), posname, lineno);
        );
        code_place_set(&source->place, posname, lineno);
    }
}



/* Reads in the whole of the next line but stores only the first maxlen-1
   bytes of it in the line buffer provided.
   The buffer is always written with a final zero (if there is a byte spare)
   The address of the start line should be set using linesource_start_set
   before invoking this: e.g. using
        code_place_t start;
        code_place_set(&start, instack_source(source->in),
                       instack_lineno(source->in);
        linesource_start_set(source, &start);
*/
extern void
linesource_read(linesource_t *source, charsink_t *line)
{   if (!source->eof)
    {   int ch = EOF;
        bool gotch = FALSE;
        bool escaped = FALSE;
        bool instring = FALSE;
        int brackets = 0;

        while ((gotch || EOF != (ch = instack_getc(&source->in))) &&
               (escaped || instring || brackets>0 ||
                (ch != source->commandsep && ch != '\n')))
        {   bool use_char = FALSE;

            gotch=FALSE;

            if (escaped)
            {   if (instring)
                    use_char = TRUE;
                else
                {   use_char = (ch != '\n');
                    if (use_char)
                        ch = escape_ch(ch);
                }
                escaped = FALSE;
            } else
            if (instring)
            {   use_char = TRUE;
                if (ch == source->string)
                    instring = FALSE;
                else if (ch == source->escape)
                    escaped = TRUE;
            } else
            if (ch == source->escape)
                escaped = TRUE;
            else
            if (ch == source->comment)
            {   while (ch != EOF && ch != '\n')
                    ch = instack_getc(&source->in);
                gotch = TRUE;
            } else
            {   use_char = TRUE;
                if (ch == source->string)
                    instring = TRUE;
                else if (ch == '<' || ch == '(' || ch == '{' || ch == '[')
                    brackets++;
                else if (ch == '>' || ch == ')' || ch == '}' || ch == ']')
                    brackets--;
            }

            if (use_char)
            {   /* printf("put %02x '%c'\n", ch, ch); */
                charsink_putc(line, ch);
            }
        }

        if (EOF == ch)
            source->eof = TRUE;
    }
}








extern void
linesource_pushline(linesource_t *lines, const char *name,
                    const char *cmd_str)
{   charsource_t *newline = charsource_cstring_new("<NL>", "\n", 1);
    charsource_t *line = charsource_cstring_new(name, cmd_str,
                                                strlen(cmd_str));
    linesource_push(lines, newline);
    linesource_push(lines, line);
}









/*****************************************************************************
 *                                                                           *
 *          Builtin Type IDs                                                 *
 *          ================                                                 *
 *                                                                           *
 *****************************************************************************/





/* deprecated: only parses the names of built-in types
 *             can not return the correct subtype of a given type
 */
extern bool
parse_type_id(const char **ref_line, type_t *out_type)
{   bool ok = TRUE;

    if (parse_key(ref_line, type_name(type_null)))
        *out_type = type_null;
    else if (parse_key(ref_line, type_name(type_type)))
        *out_type = type_type;
    else if (parse_key(ref_line, type_name(type_string)))
        *out_type = type_string;
    else if (parse_key(ref_line, type_name(type_code)))
        *out_type = type_code;
    else if (parse_key(ref_line, type_name(type_closure)))
        *out_type = type_closure;
    else if (parse_key(ref_line, type_name(type_int)))
        *out_type = type_int;
    else if (parse_key(ref_line, type_name(type_dir)))
        *out_type = type_dir;
    else if (parse_key(ref_line, type_name(type_cmd)))
        *out_type = type_cmd;
    else if (parse_key(ref_line, type_name(type_func)))
        *out_type = type_func;
    else if (parse_key(ref_line, type_name(type_stream)))
        *out_type = type_stream;
    else if (parse_key(ref_line, type_name(type_ipaddr)))
        *out_type = type_ipaddr;
    else if (parse_key(ref_line, type_name(type_macaddr)))
        *out_type = type_macaddr;
    else if (parse_key(ref_line, type_name(type_coroutine)))
        *out_type = type_coroutine;
    else if (parse_key(ref_line, type_name(type_mem)))
        *out_type = type_mem;
    else
        ok = FALSE;

    return ok;
}






STATIC_INLINE bool
type_equal(type_t kind1, type_t kind2)
{   return kind1 == kind2 || kind1->id == kind2->id;
}








/*****************************************************************************
 *                                                                           *
 *          Values                                                           *
 *          ======                                                           *
 *                                                                           *
 *****************************************************************************/






/* Garbage collection:

   The state of the garbage collection system is represented by a version
   number which is incremented on each garbage collection.

   Each value allocated is given the version number of the garbage collection
   system at the time of allocation and kept on a heap.

   Whenever a value is deleted it is removed from the heap.

   Each value may posses its own function for marking the current version
   number.  Complex values will arrange for this function to mark the current
   version into any subordinate values (etc.)

   When garbage collection occurs the next version number is generated and
   this is used to set the version number on a root value.  This should, in
   its turn, end up marking all values we are worried about.  Any values then
   found on the heap without the current version number are deleted.

   In order to allow garbage collection at an arbitrary point of execution
   some values can be marked "local" (the idea being that they are currently
   local variables in some function which have not been "attached" to the root
   object).  When garbage collection occurs all values on the stack that are
   marked local have their version updated just as the root object does -
   and so they will not be garbage collected.

   All newly allocated objects are marked as local.

   Any object incorporated into a value is un-marked as local.

   The code must ensure that all other allocated local values eventually become
   marked unlocal (when it is safe to garbage collect them).
*/




#define HEAP_VERSION_UNUSED 0x0




typedef struct
{   value_t *heap;              /**< list of values allocated from the heap */
    int version;                /**< current heap version */
} value_heap_t;



static value_heap_t value_heap;




typedef struct
{   int depth;
    const value_t *entry[PRINTSTACK_MAX];
} value_printstack_t;




static value_printstack_t prtstk;






extern /*internal*/ value_t *
(value_init)(value_t *val, type_t kind, bool on_heap)
{   val->kind = kind;
    val->link = NULL;
    DEBUG_VALLINK(DPRINTF("%p: new %s link NULL\n",
                         &val->link, value_type_name(val)););
    val->on_heap = on_heap;
    val->local = on_heap && !type_values_simple(kind);
    /* all non-constant values are initially local */
    val->heap_version = HEAP_VERSION_UNUSED;

    if (on_heap && HEAPVALID(val))
    {   /* place on value heap */
        val->heap_next = value_heap.heap;
        value_heap.heap = val;
    } else
        val->heap_next = NULL;

    DEBUG_GC(DPRINTF("New %s value %p %sdeleteable\n",
                    type_name(kind), val,  on_heap? "":"not ");)
    return val;
}





#if 0 != DEBUG_VALINIT(1+)0
extern value_t *
value_info(value_t *val, int lineno)
{   val->lineno = lineno;
    return val;
}


extern const value_t *
value_cinfo(const value_t *val, int lineno)
{   ((value_t *)/*unconst*/(val))->lineno = lineno;
    return val;
}


#define value_init(val, kind, on_heap)                               \
    value_info(value_init(val, kind, on_heap),                       \
               __LINE__)
#else

extern value_t *
value_info(value_t *val, int lineno)
{   return val;
}

#endif








STATIC_INLINE bool
value_islocal(value_t *val)
{   /* whether value is local or not */
#ifdef LOCAL_GARBAGE
    return val->local;
#else
    return FALSE;
#endif
}







STATIC_INLINE type_t
value_type(const value_t *val)
{   return val->kind;
}






extern const char *
value_type_name(const value_t *val)
{   if (NULL == val)
        return "<NoValue>";
    else
        return type_name(val->kind);
}







extern bool
value_type_equal(const value_t *val, type_t kind)
{   return PTRVALID(val) && type_equal(val->kind, kind);
}





extern bool
value_istype(const value_t *val, type_t kind)
{   if (NULL == val)
    {   fprintf(stderr, "%s: value is unset - expected %s\n",
                codeid(), type_name(kind));
        return FALSE;
    } else
    if (type_equal(val->kind, kind))
        return TRUE;
    else
    {   fprintf(stderr, "%s: value has wrong type - type is %s, expected %s\n",
                codeid(), value_type_name(val), type_name(kind));
        return FALSE;
    }
}





#if 0

STATIC_INLINE type_t
value_castable(const value_t *val, type_t kind)
{   return val->kind == kind;
}


#endif





extern int
value_print(outchar_t *out, const value_t *root, const value_t *val)
{   if (out != NULL && val != NULL && PTRVALID(val->kind) &&
        PTRVALID(val->kind->print))
    {   if (prtstk.depth >= PRINTSTACK_MAX)
            return outchar_printf(out, "...");
        else
        {   int n = prtstk.depth-1;

            while (n >= 0 && prtstk.entry[n] != val)
                n--;

            if (n >= 0)
                return outchar_printf(out, "...");
            else
            {   int len;
                prtstk.entry[prtstk.depth++] = val;
                len = (*val->kind->print)(out, root, val);
                prtstk.depth--;
                return len;
            }
        }
    } else
        return 0;
}





extern int
value_fprint(FILE *out, const value_t *root, const value_t *val)
{   charsink_stream_t charbuf;
    charsink_t *sink = charsink_stream_init(&charbuf, out);
    int n = value_print(sink, root, val);
    charsink_stream_close(sink);
    return n;
}






extern void
value_delete_alloced(value_t *value)
{   if (value != NULL && HEAPVALID(value))
    {
        FTL_FREE(value);
    }
}





/*! delete an allocated value
 *
 * Note: that normal allocations are managed by the heap.  If you call this
 * function yourself there will be a double free() when the garbage collector
 * later also tries to delete unused values.
 */
extern void
value_delete(value_t **ref_val)
{   value_t *val = *ref_val;
    if (PTRVALID(val) && val->on_heap &&
        PTRVALID(val->kind) && PTRVALID(val->kind->del))
    {   (*val->kind->del)(val);
        *ref_val = NULL;
    }
}






extern int /* <0 for less than, ==0 for equal, >0 for greater */
value_cmp(const value_t *v1, const value_t *v2)
{   if (v1 == NULL) {
        DEBUG_CMP(DPRINTF("cmp: v1 NULL v2 %sNULL\n", v2 == NULL? "":"not "););
        return v2 == NULL? 0: -1;
    } else if (v2 == NULL) {
        DEBUG_CMP(DPRINTF("cmp: v1 not NULL v2 NULL\n", v2 == NULL? "":"not "););
        return 1;
    } else if (!type_equal(v1->kind, v2->kind)) {
        DEBUG_CMP(DPRINTF("cmp: v1 (%s) v2 (%s) type differs\n",
                         type_name(v1->kind), type_name(v2->kind)););
        return v1->kind == v2->kind? 0: v1->kind > v2->kind? 1: -1;
    } else if (v1 == v2) {
        DEBUG_CMP(DPRINTF("cmp: v1 (%p:%s) v2 same address\n",
                         v1, type_name(v1->kind)););
        return 0;
    } else if (NULL == v1->kind || NULL == v1->kind->compare) {
        DEBUG_CMP(DPRINTF("cmp: v1 no comparison fn\n"););
        return /* really we want "incomparable" value */-1;
    } else {
        DEBUG_CMP(DPRINTF("cmp: v1 (%p:%s) cmp v2 (%p:%s) using fn\n",
                         v1, type_name(v1->kind), v2, type_name(v2->kind)););
        return (*v1->kind->compare)(v1, v2);
    }
}







/* Garbage collection:
 *     get next version of heap
 *     mark all reachable values with new version
 *     delete all values in the heap not marked with the new version
 */




#define value_marked(val, ver) ((val)->heap_version == (ver))



/*! Mark the value and (if it's type has a mark_version function) the other
 *  values referred to in the value.
 *  To mark a value simply record the current heap vesion in its 'heap_version'
 *  field.  Items on the heap which are not so marked can be freed by garbage
 *  collection.
 */
static void
value_mark_version(value_t *val, int heap_version)
{   if (PTRVALID(val) && !value_marked(val, heap_version))
    {   val->heap_version = heap_version;
        DEBUG_GC(DPRINTF("Mark %s value %p ver %d %s\n",
                        value_type_name(val), val, heap_version,
                        val->kind == NULL ||
                        val->kind->mark_version == NULL?"": "(recurse)");)
        /* TODO: shouldn't we mark other bits of the value even if it is not on
         * the heap too? */
        if (PTRVALID(val->kind) &&
            PTRVALID(val->kind->mark_version))
        {   (*val->kind->mark_version)(val, heap_version);
        }
    }
}





static void
value_heap_init(void)
{   value_heap.heap = (value_t *)NULL;
    value_heap.version = HEAP_VERSION_UNUSED+1;
}





static int
value_heap_nextversion(void)
{   value_heap.version++;
    DEBUG_GC(DPRINTF("Collection %d\n", value_heap.version);)
    return value_heap.version;
}





/*static*/ void
value_list_locals(void)
{   int heap_version = value_heap.version;
    value_t *val = value_heap.heap;

    while (PTRVALID(val))
    {   if (value_islocal(val) && heap_version != val->heap_version)
        {   fprintf(stderr, "%s: Local %s value %p at %d versions ago",
                    codeid(), value_type_name(val), val,
                    heap_version - val->heap_version);
            DEBUG_VALINIT(fprintf(stderr, " - line %5d: ", val->lineno););
            OMIT(VALUE_SHOW("", val););
            DO(fprintf(stderr, "\n"););
        }
        val = val->heap_next;
    }
}




static void
value_mark_local(int heap_version)
{   value_t **ref_value = &value_heap.heap;

    while (PTRVALID(*ref_value))
    {   value_t *val = *ref_value;

        if (val->heap_version != heap_version && value_islocal(val))
        {   value_mark_version(val, heap_version);
            DEBUG_GC(DPRINTF("Local %s value %p marked at ver %d\n",
                            value_type_name(val), val, heap_version);)
        }
        ref_value = &(val->heap_next);
    }
}




static void
value_heap_collect(void)
{   int heap_version = value_heap.version;
    value_t **ref_value = &value_heap.heap;

    while (PTRVALID(*ref_value))
    {   value_t *val = *ref_value;

        if (val->heap_version != heap_version)
        {   *ref_value = val->heap_next;
            val->heap_next = NULL;
            DO(if (value_islocal(val))
                   DPRINTF("%s: deleting %s local value %p ver %d (!=%d)\n",
                           codeid(), value_type_name(val), val,
                           val->heap_version, heap_version);)
            DEBUG_GC(DPRINTF("Collect %s value %p ver %d (!=%d)\n",
                            value_type_name(val), val, val->heap_version,
                            heap_version);)
            DEBUG_GCU(if (val->heap_version == HEAP_VERSION_UNUSED)
                         DPRINTF("Collect %s value %p had UNUSED ver %d "
                                 "(!=%d)\n",
                                 value_type_name(val), val, val->heap_version,
                                 heap_version);)
            value_delete(&val);
        } else
            ref_value = &(val->heap_next);
    }
}






/*****************************************************************************
 *                                                                           *
 *          Type Values                                                      *
 *          ===========                                                      *
 *                                                                           *
 *****************************************************************************/





#define ROOT_DIR_TYPE "basetype"

#define value_type_value(typeval) (&(typeval)->val)

static type_id_t type_id_generator = 0; /* last type ID generated */

static value_type_t type_type_val;
type_t type_type = &type_type_val;


extern type_id_t type_id_new(void)
{
    return ++type_id_generator;
}


static int
value_type_print(outchar_t *out, const value_t *root, const value_t *value)
{   const value_type_t *typeval = (const value_type_t *)value;
    (void)root;
    return outchar_printf(out, "$"ROOT_DIR_TYPE".%s", typeval->name);
}



/*! Compare this type (v1) with another type
 */
static int /* <0 for less than, ==0 for equal, >0 for greater */
value_type_compare(const value_t *v1, const value_t *v2)
{   if (v1 == v2)
        return 0;
    else if (v1->kind == NULL || v1->kind != type_type)
        return -1;
    else if (v2->kind == NULL || v2->kind != type_type)
        return 1;
    return ((value_type_t *)v1)->id - ((value_type_t *)v2)->id;
}



extern /*internal*/ value_t *
type_init(value_type_t *kind, bool on_heap,
          type_id_t type_id, const char *name,
          value_print_fn_t *val_print_fn, value_parse_fn_t *val_parse_fn,
          value_cmp_fn_t *val_compare_fn,
          value_delete_fn_t *val_delete_fn,
          value_markver_fn_t *val_mark_version_fn)
{   (void)value_init(value_type_value(kind), type_type, on_heap);
    kind->name = name;
    kind->id = type_id;
    kind->print = val_print_fn;
    kind->parse = val_parse_fn;
    kind->compare = val_compare_fn;
    kind->del = val_delete_fn;
    kind->mark_version = val_mark_version_fn;

    DEBUG_GC(DPRINTF("New type %s type %p (id %d) %sdeleteable\n",
                    name, kind, type_id, on_heap? "":"not "););
    return value_type_value(kind);
}





/*! determine whether other values are referred to in values of this type
 */
extern bool type_values_simple(type_t kind)
{   return !(PTRVALID(kind) &&
             PTRVALID(kind->mark_version));
}






extern value_t *
type_clone(bool on_heap,
           type_id_t cloned_type_id, const char *name,
           value_print_fn_t *print_fn, value_parse_fn_t *parse_fn,
           value_cmp_fn_t *compare_fn,
           value_delete_fn_t *delete_fn,
           value_markver_fn_t *mark_version)
{   value_type_t *typeval = (value_type_t *)FTL_MALLOC(sizeof(value_type_t));

    if (PTRVALID(typeval))
        return type_init(typeval, on_heap, cloned_type_id, name,
                         print_fn, parse_fn, compare_fn, delete_fn,
                         mark_version);
    else
        return NULL;
}




extern value_t *
type_new(bool on_heap,
         const char *name,
         value_print_fn_t *print_fn, value_parse_fn_t *parse_fn,
         value_cmp_fn_t *compare_fn,
         value_delete_fn_t *delete_fn,
         value_markver_fn_t *mark_version_fn)
{
    return type_clone(on_heap, type_id_new(), name,
                      print_fn, parse_fn, compare_fn, delete_fn,
                      mark_version_fn);
}





static void
values_type_init(void)
{
    type_id_generator = 0;
    type_init(&type_type_val, /*on_heap*/FALSE,
                    type_id_new(), "type", &value_type_print,
                    /*value_type_parse*/NULL,
                    &value_type_compare, &value_delete_alloced,
                    /*mark_version*/NULL);
}





extern const char *
type_name(type_t kind)
{   return kind == NULL? "<BAD type>": kind->name;
}







/*****************************************************************************
 *                                                                           *
 *          NULL Values                                                      *
 *          ===========                                                      *
 *                                                                           *
 *****************************************************************************/




static value_type_t type_null_val;
type_t type_null = &type_null_val;
value_t value_null;

static int
value_null_print(outchar_t *out, const value_t *root, const value_t *value)
{   (void)root;
    return outchar_printf(out, "NULL");
}


static int /* <0 for less than, ==0 for equal, >0 for greater */
value_null_compare(const value_t *v1, const value_t *v2)
{   return 0;  /* always equal */
}


static value_t *
value_null_init(value_t *value, bool on_heap)
{   return value_init(value, type_null, on_heap);
}


static void
values_null_init(void)
{
    type_init(&type_null_val, /*on_heap*/FALSE, type_id_new(), "nul",
              &value_null_print, /*parse*/NULL,
              &value_null_compare, &value_delete_alloced, /*mark*/NULL);
    value_null_init(&value_null, /*on_heap*/FALSE);
}



/* always gives a NON-zero value_t back */
extern const value_t *
value_nl(const value_t *value)
{   if (value == NULL)
         return &value_null;
     else
         return value;
}





/*****************************************************************************
 *                                                                           *
 *          Integer Values                                                   *
 *          ==============                                                   *
 *                                                                           *
 *****************************************************************************/





static value_type_t type_int_val;
type_t type_int = &type_int_val;



typedef struct
{   value_t value;           /* integer used as a value */
    number_t number;
} value_int_t;




static unumber_t int_format_bits_indec = -1;  /*bits rendered in decimal*/




#define value_int_get_fmt_bits_indec() (int_format_bits_indec)
#define value_int_set_fmt_bits_indec(v) int_format_bits_indec = v




#if 0

static number_t
value_int_setnumber(value_t *value, number_t number)
{   if (value_istype(value, type_int))
    {   value_int_t *no = (value_int_t *)value;
        no->number = number;
    }
    /* else type error */

    return number;
}

#endif






static int value_int_print(outchar_t *out,
                           const value_t *root, const value_t *value)
{   number_t n = value_int_number(value);
    number_t absn = n;
    if (absn < 0)
        absn = -absn;
    (void)root;
    if (0 != (absn & ~int_format_bits_indec))
        return outchar_printf(out, "0x%"FX_UNUMBER_T"", n);
    else
        return outchar_printf(out, "%"F_NUMBER_T"", n);
}





static int /* <0 for less than, ==0 for equal, >0 for greater */
value_int_compare(const value_t *v1, const value_t *v2)
{   number_t n = ((const value_int_t *)v1)->number -
                 ((const value_int_t *)v2)->number;
    return (n>0? 1: n<0? -1: 0);
    /* be cautious about truncating number_t to an int */
}




static value_t *
value_int_init(value_int_t *no, unumber_t number, bool on_heap)
{   no->number = number;
    return value_init(&no->value, type_int, on_heap);
}



#if 0 != DEBUG_VALINIT(1+)0
#define value_int_init(no, number, on_heap) \
    value_info((value_int_init)(no, number, on_heap), __LINE__)
#endif






extern value_t *
value_int_new(number_t number)
{   value_int_t *no = (value_int_t *)FTL_MALLOC(sizeof(value_int_t));

    if (PTRVALID(no))
        return value_int_init(no, number, /*on_heap*/TRUE);
    else
        return NULL;
}




extern value_t *
value_uint_new(unumber_t number)
{   value_int_t *no = (value_int_t *)FTL_MALLOC(sizeof(value_int_t));

    if (PTRVALID(no))
        return value_int_init(no, number, /*on_heap*/TRUE);
    else
        return NULL;
}




#if 0 != DEBUG_VALINIT(1+)0
#define value_int_new(number) \
    value_info((value_int_new)(number), __LINE__)
#endif





extern void
value_int_update(const value_t **ref_value, number_t number)
{   const value_t *val;
    OMIT(printf("%s: update int at *%p - %d\n",
                  codeid(), ref_value, (int)number););
    val = *ref_value;
    if (NULL == val || !value_type_equal(val, type_int))
    {   *ref_value = value_int_new(number);
    } else
    {   value_int_t *no = (value_int_t *)val;
        no->number = number;
    }
}







extern number_t
value_int_number(const value_t *value)
{   number_t number = 0;

    if (value_istype(value, type_int))
    {   value_int_t *no = (value_int_t *)value;
        number = no->number;
    }
    /* else type error */

    return number;
}





extern bool
parse_int(const char **ref_line, number_t *out_int)
{   int width;
    if (1 == sscanf(*ref_line, "%"F_NUMBER_T"%n", out_int, &width))
    {   *ref_line += width;
        return TRUE;
    } else
        return FALSE;
}






extern bool
parse_hex(const char **ref_line, unumber_t *out_int)
{   int width;
    if (1 == sscanf(*ref_line, "%"FX_UNUMBER_T"%n", out_int, &width))
    {   *ref_line += width;
        return TRUE;
    } else
        return FALSE;
}




extern bool
parse_octal(const char **ref_line, unumber_t *out_int)
{   int width;
    if (1 == sscanf(*ref_line, "%"FO_UNUMBER_T"%n", out_int, &width))
    {   *ref_line += width;
        return TRUE;
    } else
        return FALSE;
}




static int hexval(char ch)
{   if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        return ch - 'F' + 10;
    else
        return -1;
}




extern bool
parse_hex_width(const char **ref_line, unsigned width, unumber_t *out_int)
{   unumber_t n = 0;
    int hch;
    const char *line = *ref_line;

    while (width > 0 && (hch = hexval(*line))>=0)
    {   n = (n << 4) | hch;
        width--;
        line++;
    }

    if (width == 0)
    {   *ref_line = line;
        *out_int = n;
        return TRUE;
    } else
        return FALSE;
}





extern bool
parse_int_expr(const char **ref_line,
               parser_state_t *state, number_t *out_int);



extern bool
parse_int_val(const char **ref_line, number_t *out_int)
{   const char *line = *ref_line;
    bool ok = FALSE;
    int sign = 1;
    number_t val;

    parse_space(&line);
    if (parse_key(&line, "-"))
    {   sign = -1;
        parse_space(&line);
    }
    if (parse_key(&line, "0x") || parse_key(&line, "0X") )
        ok = parse_hex(&line, (unumber_t *)&val);
    else if (parse_key(&line, "0o") || parse_key(&line, "0O") )
        ok = parse_octal(&line, (unumber_t *)&val);
    /* else if (parse_key(&line, "0b") || parse_key(&line, "0B") ) */
    /*  ok = parse_binary(&line, (unumber_t *)&val); */
    else
        ok = parse_int(&line, &val);

    if (ok)
    {   *out_int = val * sign;
        *ref_line = line;
    }
    return ok;
}





/* This function may cause a garbage collection */
extern bool
parse_int_base(const char **ref_line, parser_state_t *state,
               number_t *out_int)
{   /* syntax: (<int expression>) | <int> */
    bool ok;
    const char *line = *ref_line;

    DEBUG_TRACE(DPRINTF("(int: '%s'\n", *ref_line);)
    
    if (parse_key(&line, "("))
    {   const value_t *intval;
        parse_space(&line);
        ok = parse_expr(&line, state, &intval) &&
             value_istype(intval, type_int) &&
             parse_space(&line) &&
             parse_key(&line, ")");
        if (ok)
            *out_int = value_int_number(intval);
    } else
        ok = parse_int_val(&line, out_int);

    if (ok)
        *ref_line = line;

    DEBUG_TRACE(DPRINTF(")int %s: '%s'\n", ok?"OK":"FAIL", *ref_line);)

    return ok;
}





#if 0
/* This function may cause a garbage collection */
static bool parse_int_prod(const char **ref_line,
                           parser_state_t *state, number_t *out_int)
{   number_t prod = 0;
    const char *save = *ref_line;
    bool ok = parse_int_base(ref_line, state, &prod);

    if (ok)
    {   number_t opnd;
        bool times;

        while (ok && parse_space(ref_line) &&
               ((times=parse_key(ref_line, "*")) ||
                parse_key(ref_line, "/")))
        {   parse_space(ref_line);
            ok = parse_int_base(ref_line, state, &opnd);

            if (times)
                prod *= opnd;
            else
                prod /= opnd;
        }
    }

    if (ok)
    {   *out_int = prod;
        return TRUE;
    } else
    {   *ref_line = save;
        return FALSE;
    }
}




/* This function may cause a garbage collection */
static bool parse_int_sum(const char **ref_line,
                          parser_state_t *state, number_t *out_int)
{   number_t sum = 0;
    const char *save = *ref_line;
    bool ok = parse_int_prod(ref_line, state, &sum);

    if (ok)
    {   number_t opnd;
        bool add;

        while (ok && parse_space(ref_line) &&
               ((add=parse_key(ref_line, "+")) ||
                parse_key(ref_line, "-")))
        {   parse_space(ref_line);
            ok = parse_int_prod(ref_line, state, &opnd);

            if (add)
                sum += opnd;
            else
                sum -= opnd;
        }
    }

    if (ok)
    {   *out_int = sum;
        return TRUE;
    } else
    {   *ref_line = save;
        return FALSE;
    }
}




/* This function may cause a garbage collection */
static bool parse_int_shift(const char **ref_line,
                            parser_state_t *state, number_t *out_int)
{   number_t accum = 0;
    const char *save = *ref_line;
    bool ok = parse_int_sum(ref_line, state, &accum);

    if (ok)
    {   number_t opnd;
        bool left;

        while (ok && parse_space(ref_line) &&
               ((left=parse_key(ref_line, "lsh")) ||
                parse_key(ref_line, "rsh")))
        {   number_t starting = accum;
            parse_space(ref_line);
            ok = parse_int_sum(ref_line, state, &opnd);

            if (opnd >= 8*sizeof(number_t))
                accum = 0;
            else if (left)
                accum = (number_t) (((unumber_t)starting) << (unsigned)opnd);
            else
                accum = (number_t) (((unumber_t)starting) >> (unsigned)opnd);

            OMIT(printf("%s: %"F_NUMBER_T" %s %"F_NUMBER_T
                          " == %"F_NUMBER_T"\n",
                          codeid(), starting, left?"lsh":"rsh", opnd, accum);)
        }
    }

    if (ok)
    {   *out_int = accum;
        return TRUE;
    } else
    {   *ref_line = save;
        return FALSE;
    }
}




/* This function may cause a garbage collection */
static bool parse_int_cmp(const char **ref_line,
                          parser_state_t *state, number_t *out_int)
{   number_t accum = 0;
    const char *save = *ref_line;
    bool ok = parse_int_shift(ref_line, state, &accum);

    if (ok)
    {   number_t opnd;
        bool lt = FALSE, le = FALSE, ge = FALSE, gt = FALSE;

        if (ok && parse_space(ref_line) &&
            ((le=parse_key(ref_line, "le")) ||
             (lt=parse_key(ref_line, "lt")) ||
             (ge=parse_key(ref_line, "ge")) ||
             (gt=parse_key(ref_line, "gt"))))
        {   parse_space(ref_line);
            ok = parse_int_shift(ref_line, state, &opnd);

            if (le)
                accum = (accum <= opnd? 1: 0);
            else if (lt)
                accum = (accum < opnd? 1: 0);
            else if (ge)
                accum = (accum >= opnd? 1: 0);
            else
                accum = (accum > opnd? 1: 0);
        }
    }

    if (ok)
    {   *out_int = accum;
        return TRUE;
    } else
    {   *ref_line = save;
        return FALSE;
    }
}




/* This function may cause a garbage collection */
static bool parse_int_eq(const char **ref_line,
                         parser_state_t *state, number_t *out_int)
{   number_t accum = 0;
    const char *save = *ref_line;
    bool ok = parse_int_cmp(ref_line, state, &accum);

    if (ok)
    {   number_t opnd;
        bool eq;

        if (ok && parse_space(ref_line) &&
            ((eq=parse_key(ref_line, "==")) ||
                 parse_key(ref_line, "!=")))
        {   parse_space(ref_line);
            ok = parse_int_cmp(ref_line, state, &opnd);

            if (eq)
                accum = (accum == opnd? 1: 0);
            else
                accum = (accum != opnd? 1: 0);
        }
    }

    if (ok)
    {   *out_int = accum;
        return TRUE;
    } else
    {   *ref_line = save;
        return FALSE;
    }
}





/* This function may cause a garbage collection */
static bool parse_int_not(const char **ref_line,
                          parser_state_t *state, number_t *out_int)
{   number_t accum = 0;
    const char *save = *ref_line;
    bool is_not = parse_space(ref_line) && parse_key(ref_line, "not") &&
                  parse_space(ref_line);
    bool ok = parse_int_eq(ref_line, state, &accum);

    if (ok)
    {   *out_int = (is_not? !accum: accum);
        return TRUE;
    } else
    {   *ref_line = save;
        return FALSE;
    }
}




/* This function may cause a garbage collection */
static bool parse_int_and(const char **ref_line,
                          parser_state_t *state, number_t *out_int)
{   number_t accum = 0;
    const char *save = *ref_line;
    bool ok = parse_int_not(ref_line, state, &accum);

    if (ok)
    {   number_t opnd;

        while (ok && parse_space(ref_line) && parse_key(ref_line, "and"))
        {   parse_space(ref_line);
            ok = parse_int_not(ref_line, state, &opnd);
            accum = accum && opnd;
        }
    }

    if (ok)
    {   *out_int = accum;
        return TRUE;
    } else
    {   *ref_line = save;
        return FALSE;
    }
}




/* This function may cause a garbage collection */
static bool parse_int_or(const char **ref_line,
                         parser_state_t *state, number_t *out_int)
{   number_t accum = 0;
    const char *save = *ref_line;
    bool ok = parse_int_and(ref_line, state, &accum);

    if (ok)
    {   number_t opnd;

        while (ok && parse_space(ref_line) && parse_key(ref_line, "or"))
        {   parse_space(ref_line);
            ok = parse_int_and(ref_line, state, &opnd);
            accum = accum || opnd;
        }
    }

    if (ok)
    {   *out_int = accum;
        return TRUE;
    } else
    {   *ref_line = save;
        return FALSE;
    }
}
#endif



/* This function may cause a garbage collection */
extern bool
parse_int_expr(const char **ref_line,
               parser_state_t *state, number_t *out_int)
{   /* return parse_int_or(ref_line, state, out_int); */
    /* return parse_int_shift(ref_line, state, out_int); */
    return parse_int_base(ref_line, state, out_int);
}





static const value_t *
value_int_parse(const char **ref_line, const value_t *this_cmd,
                parser_state_t *state)
{   number_t num;
    if (parse_int_expr(ref_line, state, &num))
        return value_int_new(num);
    else
        return &value_null;
}








/*****************************************************************************
 *                                                                           *
 *          Integer Values                                                   *
 *          ==============                                                   *
 *                                                                           *
 *****************************************************************************/





value_int_t value_int_zero;
value_int_t value_int_one;

const value_t *value_zero = &value_int_zero.value;
const value_t *value_one = &value_int_one.value;




extern void
values_int_init(void)
{
    type_init(&type_int_val, /*on_heap*/FALSE, type_id_new(), "int",
              &value_int_print, &value_int_parse,
              &value_int_compare, &value_delete_alloced, /*mark*/NULL);
    value_int_init(&value_int_zero,  0, /* on_heap */FALSE);
    value_int_init(&value_int_one,   1, /* on_heap */FALSE);
}






/*****************************************************************************
 *                                                                           *
 *          IP Address Values                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/







typedef struct
{   value_t value;           /* integer used as a value */
    addr_ip_t addr;
} value_ipaddr_t;




static value_type_t type_ipaddr_val;
type_t type_ipaddr = &type_ipaddr_val;




extern int
ipaddr_fprint(FILE *out, const addr_ip_t *ip)
{   return fprintf(out, "%u.%u.%u.%u",
                   (*ip)[0], (*ip)[1], (*ip)[2], (*ip)[3]);
}





static int
value_ipaddr_print(outchar_t *out, const value_t *root, const value_t *value)
{   const value_ipaddr_t *ip = (const value_ipaddr_t *)value;
    (void)root;
    return outchar_printf(out, "%u.%u.%u.%u",
                          (ip->addr)[0], (ip->addr)[1],
                          (ip->addr)[2], (ip->addr)[3]);
}





static int /* <0 for less than, ==0 for equal, >0 for greater */
value_ipaddr_compare(const value_t *v1, const value_t *v2)
{   return memcmp(&((const value_ipaddr_t *)v1)->addr[0],
                  &((const value_ipaddr_t *)v2)->addr[0],
                  sizeof((const value_ipaddr_t *)v1)->addr);
}




static const value_t *
value_ipaddr_parse(const char **ref_line, const value_t *this_cmd,
                   parser_state_t *state)
{   addr_ip_t ip;
    if (parse_ipaddr(ref_line, &ip))
        return value_ipaddr_new(&ip);
    else
        return &value_null;
}



static value_t *
value_ipaddr_init(value_ipaddr_t *ip, int a, int b, int c, int d, bool on_heap)
{   ip->addr[0] = a;
    ip->addr[1] = b;
    ip->addr[2] = c;
    ip->addr[3] = d;
    return value_init(&ip->value, type_ipaddr, on_heap);
}




static void
values_ipaddr_init(void)
{
    type_init(&type_ipaddr_val, /*on_heap*/FALSE, type_id_new(), "ip",
              &value_ipaddr_print, &value_ipaddr_parse,
              &value_ipaddr_compare, &value_delete_alloced, /*mark*/NULL);
}





extern value_t *
value_ipaddr_new_quad(int a, int b, int c, int d)
{   value_ipaddr_t *ip = (value_ipaddr_t *)
                         FTL_MALLOC(sizeof(value_ipaddr_t));

    if (PTRVALID(ip))
        return value_ipaddr_init(ip, a, b, c, d, /*on_heap*/TRUE);
    else
        return NULL;
}





extern const value_t *
value_ipaddr_new(addr_ip_t *ip)
{   return value_ipaddr_new_quad((*ip)[0], (*ip)[1], (*ip)[2], (*ip)[3]);
}





extern void
value_ipaddr_get(const value_t *value, addr_ip_t *out_ipaddr)
{   if (value_istype(value, type_ipaddr))
    {   value_ipaddr_t *ip = (value_ipaddr_t *)value;
        memcpy(out_ipaddr, &ip->addr, sizeof(ip->addr));
    }
    /* else type error */
}




extern bool
parse_ipaddr(const char **ref_line, addr_ip_t *out_ip)
{   const char *line = *ref_line;
    addr_ip_t ip;
    number_t ip1;
    bool ok = parse_int(&line, &ip1);

    if (ok)
    {   number_t ip2, ip3, ip4;

        if (parse_key(&line, ".") &&
            parse_int(&line, &ip2) && parse_key(&line, ".") &&
            parse_int(&line, &ip3) && parse_key(&line, ".") &&
            parse_int(&line, &ip4))
        {   ip[0] = (unsigned char)ip1;
            ip[1] = (unsigned char)ip2;
            ip[2] = (unsigned char)ip3;
            ip[3] = (unsigned char)ip4;
        } else
            ok = FALSE;
    }
#ifdef HAS_GETHOSTBYNAME
    else
    {   char ipname[128];
        if (parse_item(&line, ":@/", 3, &ipname[0], sizeof(ipname)))
        {   struct hostent *ent = gethostbyname(&ipname[0]);

            if (PTRVALID(ent) &&
                ent->h_addrtype == AF_INET &&
                ent->h_length == sizeof(addr_ip_t))
            {   addr_ip_t *retip = (addr_ip_t *)ent->h_addr;
                memcpy(&ip, retip, sizeof(ip));
                ok = TRUE;
            }
            DEBUG_SOCK(
                else if (NULL == ent)
                {   fprintf(stderr, "no IP address for %s\n", &ipname[0]);
                    /* debug on windows only
                    if (WSAGetLastError() != 0) {
                        if (WSAGetLastError() == 11001)
                            printf("Host not found...\nExiting.\n");
                        else
                            printf("error#:%ld\n", WSAGetLastError());
                    }
                    */
                } else if (ent->h_addrtype == AF_INET)
                    fprintf(stderr,
                            "address family for %s not %d (INET) it is %d\n",
                            &ipname[0], AF_INET, ent->h_addrtype);
                else if (ent->h_addrtype == AF_INET)
                    fprintf(stderr, "address size for %s not %d it is %d\n",
                            &ipname[0], sizeof(addr_ip_t), ent->h_length);
            )
        }
    }
#endif

    if (ok)
    {   *ref_line = line;
        memcpy(out_ip, &ip, sizeof(*out_ip));
        /* update only if succeeded */
    }
    return ok;
}









/*****************************************************************************
 *                                                                           *
 *          MAC Address Values                                               *
 *          ==================                                               *
 *                                                                           *
 *****************************************************************************/







typedef struct
{   value_t value;           /* integer used as a value */
    addr_mac_t addr;
} value_macaddr_t;






static value_type_t type_macaddr_val;
type_t type_macaddr = &type_macaddr_val;





extern int
macaddr_fprint(FILE *out, const addr_mac_t *mac)
{   return fprintf(out, "%02x:%02x:%02x:%02x:%02x:%02x",
                   (*mac)[0], (*mac)[1], (*mac)[2],
                   (*mac)[3], (*mac)[4], (*mac)[5]);
}





static int
value_macaddr_print(outchar_t *out, const value_t *root, const value_t *value)
{   const value_macaddr_t *mac = (const value_macaddr_t *)value;
    (void)root;
    return outchar_printf(out, "%02x:%02x:%02x:%02x:%02x:%02x",
                          (mac->addr)[0], (mac->addr)[1], (mac->addr)[2],
                          (mac->addr)[3], (mac->addr)[4], (mac->addr)[5]);
}






static int /* <0 for less than, ==0 for equal, >0 for greater */
value_macaddr_compare(const value_t *v1, const value_t *v2)
{   return memcmp(&((const value_macaddr_t *)v1)->addr[0],
                  &((const value_macaddr_t *)v2)->addr[0],
                  sizeof((const value_macaddr_t *)v1)->addr);
}




static value_t *
value_macaddr_init(value_macaddr_t *mac, int a, int b, int c,
                   int d, int e, int f,  bool on_heap)
{   mac->addr[0] = a;
    mac->addr[1] = b;
    mac->addr[2] = c;
    mac->addr[3] = d;
    mac->addr[4] = e;
    mac->addr[5] = f;
    return value_init(&mac->value, type_macaddr, on_heap);
}






extern value_t *
value_macaddr_new_sextet(int a, int b, int c, int d, int e, int f)
{   value_macaddr_t *mac = (value_macaddr_t *)
                           FTL_MALLOC(sizeof(value_macaddr_t));

    if (PTRVALID(mac))
        return value_macaddr_init(mac, a, b, c, d, e, f, /*on_heap*/TRUE);
    else
        return NULL;
}







extern const value_t *
value_macaddr_new(addr_mac_t *mac)
{   return value_macaddr_new_sextet((*mac)[0], (*mac)[1], (*mac)[2],
                                    (*mac)[3], (*mac)[4], (*mac)[5]);
}





extern void
value_macaddr_get(const value_t *value, unsigned char *out_macaddr)
{   if (value_istype(value, type_macaddr))
    {   value_macaddr_t *mac = (value_macaddr_t *)value;
        memcpy(out_macaddr, &mac->addr, sizeof(mac->addr));
    }
    /* else type error */
}





extern bool
parse_macaddr(const char **ref_line, addr_mac_t *mac_addr)
{   const char *line = *ref_line;
    unumber_t mac1 = 0, mac2 = 0, mac3 = 0, mac4 = 0, mac5 = 0, mac6 = 0;
    bool ok = parse_hex(&line, &mac1) && parse_key(&line, ":") &&
              parse_hex(&line, &mac2) && parse_key(&line, ":") &&
              parse_hex(&line, &mac3) && parse_key(&line, ":") &&
              parse_hex(&line, &mac4) && parse_key(&line, ":") &&
              parse_hex(&line, &mac5) && parse_key(&line, ":") &&
              parse_hex(&line, &mac6);
    if (ok)
    {   unsigned char *mac = &(*mac_addr)[0];
        *ref_line = line;
        mac[0] = (unsigned char)mac1;
        mac[1] = (unsigned char)mac2;
        mac[2] = (unsigned char)mac3;
        mac[3] = (unsigned char)mac4;
        mac[4] = (unsigned char)mac5;
        mac[5] = (unsigned char)mac6;
    }
    return ok;
}






static const value_t *
value_macaddr_parse(const char **ref_line, const value_t *this_cmd,
                    parser_state_t *state)
{   addr_mac_t mac;
    if (parse_macaddr(ref_line, &mac))
        return value_macaddr_new(&mac);
    else
        return &value_null;
}





static void
values_macaddr_init(void)
{
    type_init(&type_macaddr_val, /*on_heap*/FALSE, type_id_new(), "mac",
              &value_macaddr_print, &value_macaddr_parse,
              &value_macaddr_compare, &value_delete_alloced,
              /*mark*/NULL);
}








/*****************************************************************************
 *                                                                           *
 *          String Values                                                    *
 *          =============                                                    *
 *                                                                           *
 *****************************************************************************/







static value_type_t type_string_val;
type_t type_string = &type_string_val; 



typedef struct value_stringbase_s value_stringbase_t;



typedef bool
value_string_get_fn_t(const value_stringbase_t *value,
                      const char **out_buf, size_t *out_len);




struct value_stringbase_s
{   value_t value;               /* string used as a value */
    value_string_get_fn_t *get;  /* function to find string base and len */
} /* value_stringbase_t */;




#define value_stringbase_value(str) (&(str)->value)






STATIC_INLINE bool
value_stringbase_get(const value_stringbase_t *value,
                     const char **out_buf, size_t *out_len)
{   return (*value->get)(value, out_buf, out_len);
}






extern bool
value_string_get(const value_t *value, const char **out_buf, size_t *out_len)
{   bool ok = FALSE;

    *out_buf = NULL;
    *out_len = 0;

    if (value_istype(value, type_string))
    {   const value_stringbase_t *str = (const value_stringbase_t *)value;
        ok = value_stringbase_get(str, out_buf, out_len);
    }
    /* else type error */

    return ok;
}






extern void
value_string_update(const value_t **ref_value, const char *string)
{   OMIT(printf("%s: update str at *%p - '%s'\n",
                  codeid(), ref_value, string););
    /* we don't know how to update efficiently - this string may have
       sub-string pointers into it too, so always replace the string */
    if (NULL == string)
        *ref_value = &value_null;
    else
        *ref_value = value_string_new_measured(string);
}





static int
string_body_print(outchar_t *out, const char *str, size_t len)
{   int n=0;
    while (len-->0)
    {   char ch = *str++;
        switch (ch)
        {   case '"': case '\'': case '\\': case '$':
                outchar_putc(out, '\\'); outchar_putc(out, ch); n+=2; break;
            case '\n':
                outchar_putc(out, '\\'); outchar_putc(out, 'n'); n+=2; break;
            case '\r':
                outchar_putc(out, '\\'); outchar_putc(out, 'r'); n+=2; break;
            case '\t':
                outchar_putc(out, '\\'); outchar_putc(out, 't'); n+=2; break;
            case '\a':
                outchar_putc(out, '\\'); outchar_putc(out, 'a'); n+=2; break;
            case '\b':
                outchar_putc(out, '\\'); outchar_putc(out, 'b'); n+=2; break;
            case '\f':
                outchar_putc(out, '\\'); outchar_putc(out, 'f'); n+=2; break;
            case '\v':
                outchar_putc(out, '\\'); outchar_putc(out, 'v'); n+=2; break;
            case '\0':
                outchar_putc(out, '\\'); outchar_putc(out, '0'); n+=2; break;
            default:
                if (ch < 0x20 || ch >= 0x7f)
                {   outchar_printf(out, "\\x%02x", (unsigned char)ch);
                    n+=4;
                } else
                {   outchar_putc(out, ch);
                    n++;
                }
                break;
        }
    }

    return n;
}






static int
string_print(outchar_t *out, const char *str, size_t len)
{   int n;
    outchar_putc(out, '"');
    n = 2+string_body_print(out, str, len);
    outchar_putc(out, '"');
    return n;
}






static bool
string_is_id(const char *str, size_t len)
{   bool is_id = FALSE;
    if (len > 0 && (isalpha((unsigned char)str[0]) || str[0]=='_')) {
        do
            len--;
        while (len > 0 && (isalnum((unsigned char)str[len]) || str[len]=='_'));
        is_id = (len == 0);
    }
    return is_id;
}






static int
string_print_as_id(outchar_t *out, const char *str, size_t len)
{
    if (string_is_id(str, len))
        return string_body_print(out, str, len);
    else
        return string_print(out, str, len);
}






static int
value_string_print(outchar_t *out, const value_t *root, const value_t *value)
{   const char *chars;
    size_t len;
    (void)root;
    if (value_string_get(value, &chars, &len))
        return string_print(out, chars, len);
    else
        return outchar_printf(out, "NOSTR");
}






static int
value_string_print_as_id(outchar_t *out, const value_t *root,
                         const value_t *value)
{   const char *chars;
    size_t len;
    (void)root;
    if (value_string_get(value, &chars, &len))
        return string_print_as_id(out, chars, len);
    else
        return outchar_printf(out, "NOSTR");
}






static int /* <0 for less than, ==0 for equal, >0 for greater */
value_string_compare(const value_t *v1, const value_t *v2)
{   value_stringbase_t *str1 = (value_stringbase_t *)v1;
    value_stringbase_t *str2 = (value_stringbase_t *)v2;
    const char *buf1;
    const char *buf2;
    size_t len1;
    size_t len2;
    (void)value_stringbase_get(str1, &buf1, &len1);
    (void)value_stringbase_get(str2, &buf2, &len2);

    if (NULL == buf1 || NULL == buf2)
        return (NULL == buf1? 0: 1) - (NULL == buf2? 0: 1);
    else
    {   size_t minlen = len1;
        int cmp;

        if (len2 < minlen)
            minlen = len2;

        cmp = memcmp(buf1, buf2, minlen);

        return cmp == 0? (int)(len1) - (int)(len2): cmp;
    }
}









extern const char *
value_string_chars(const value_t *string)
{   const char *str;
    size_t len;
    value_string_get(string, &str, &len);
    return str;
}















/*****************************************************************************
 *                                                                           *
 *          Binary String Values                                             *
 *          ====================                                             *
 *                                                                           *
 *****************************************************************************/





static value_type_t type_stringlit_val;
/* a new implementation of type_string for malloc'd literal strings */

static value_type_t type_cstringlit_val;
/* a new implementation of type_string for static literal strings */





typedef struct
{   value_stringbase_t base;           /* base string type */
    const char *string;
    size_t len;
} value_string_t;


#define value_string_value(str) value_stringbase_value(&(str)->base)





static void
value_string_delete(value_t *value)
{   if (value_istype(value, &type_stringlit_val))
    {   value_string_t *str = (value_string_t *)value;
        if (NULL != str->string)
            FTL_FREE((void *)str->string);
        FTL_FREE(str);
    }
    /* else type error */
}






static bool
value_string_get_fn(const value_stringbase_t *value,
                    const char **out_buf, size_t *out_len)
{   value_string_t *str = (value_string_t *)value;
    *out_buf = str->string;
    *out_len = str->len;
    return TRUE;
}






STATIC_INLINE value_t *
value_cstring_init(value_string_t *str, const char *string, size_t len,
                   bool on_heap)
{   str->string = string;
    str->len = len;
    str->base.get = &value_string_get_fn;
    return value_init(&str->base.value, &type_cstringlit_val, on_heap);
}





#if 0 != DEBUG_VALINIT(1+)0
#define value_cstring_init(str, string, len, on_heap) \
    value_info((value_cstring_init)(str, string, len, on_heap), __LINE__)
#endif



/*! make new constant string - out of existing string storage
 *  Use only when you know that the storage has static scope.
 */
extern value_t *
value_cstring_new(const char *string, size_t len)
{   value_string_t *str = NULL;
    value_t *newstr = NULL;

    if (NULL != string)
    {   str = (value_string_t *)FTL_MALLOC(sizeof(value_string_t));
        if (NULL != str)
        {   newstr = value_cstring_init(str, string, len, /*on_heap*/TRUE);
        }
    }
    return newstr;
}






STATIC_INLINE value_t *
value_string_init(value_string_t *str, char *string, size_t len, bool on_heap)
{   if (NULL != string)
        string[len] = '\0';
    str->string = string;
    str->len = len;
    str->base.get = &value_string_get_fn;
    return value_init(&str->base.value, &type_stringlit_val, on_heap);
}





#if 0 != DEBUG_VALINIT(1+)0
#define value_string_init(str, string, len, on_heap) \
    value_info((value_string_init)(str, string, len, on_heap), __LINE__)
#endif





/*! make new string - allocates storage for a copy of the string provided
 *  Always ensures that there is a final '\0'.
 */
extern value_t *
value_string_new(const char *string, size_t len)
{   value_string_t *str = NULL;
    value_t *newstr = NULL;

    if (NULL != string)
    {   str = (value_string_t *)FTL_MALLOC(sizeof(value_string_t));
        if (NULL != str)
        {   char *strcopy = (char *)FTL_MALLOC(len+1);

            if (NULL != strcopy)
            {   memcpy(strcopy, string, len);
                strcopy[len] = '\0';
                newstr = value_string_init(str, strcopy, len, /*on_heap*/TRUE);
            } else
            {   FTL_FREE(str);
                str = NULL;
            }
        }
    }
    return newstr;
}





/*! make new string - allocates storage for a string of the given length
 *  and returns a pointer to it for initialization.
 *  Always ensures that there is a final '\0'.
 */
extern value_t *
value_string_alloc_new(size_t len, char **out_string)
{   value_t *newstr = NULL;

    if (PTRVALID(out_string)) {
        value_string_t *str = (value_string_t *)
                              FTL_MALLOC(sizeof(value_string_t));
        if (NULL != str)
        {   char *strcopy = (char *)FTL_MALLOC(len+1);

            *out_string = strcopy;

            if (NULL != strcopy)
            {   strcopy[len] = '\0';
                newstr = value_string_init(str, strcopy, len, /*on_heap*/TRUE);
            } else
            {   FTL_FREE(str);
                str = NULL;
            }
        }
    }
    return newstr;
}





#if 0 != DEBUG_VALINIT(1+)0
#define value_string_new(string, len) \
    value_info((value_string_new)(string, len), __LINE__)
#endif




extern value_t *
value_wcstring_new(const wchar_t *wcstr, size_t wcstr_chars)
{   value_t *newstr = NULL;

    if (NULL != wcstr)
    {   size_t len = wcstombs(NULL, wcstr, 0);
        if (len != (size_t)(-1))
        {   value_string_t *str = (value_string_t *)
                                  FTL_MALLOC(sizeof(value_string_t));
            if (NULL != str)
            {   char *strcopy = (char *)FTL_MALLOC(len+1);

                if (NULL != strcopy)
                {   size_t reallen = wcstombs(strcopy, wcstr, len+1);
                    if (reallen != (size_t)(-1))
                        newstr = value_string_init(str, strcopy, reallen,
                                                   /*on_heap*/TRUE);
                    else
                    {   FTL_FREE(str);
                        FTL_FREE(strcopy);
                        str = NULL;
                        DO(fprintf(stderr, "%s: can't read unicode "
                                   "(%d chars to %d multibyte string) "
                                   "not converted - error %d\n",
                                   codeid(), (int)wcstr_chars, (int)len+1,
                                   errno);)
                    }
                } else
                {   FTL_FREE(str);
                    str = NULL;
                }
            }
        }
        DO(else fprintf(stderr, "%s: failed to get length of"
                        "unicode string - error %d\n", codeid(), errno);)
    }
    return newstr;
}





extern const value_t *
value_string_get_terminated(const value_t *strval,
                            const char **out_buf, size_t *out_len)
{   const char *strbase;
    size_t len;
    const value_t *dstr = strval;

    if (value_string_get(strval, &strbase, &len) && (int)len >= 0) {
        /* why check for final \n? - I don't think the new string has one */
        if (strbase[len] == '\0' && (len <= 0 || strbase[len-1] == '\n')) {
            *out_buf = strbase;
            *out_len = len;
        } else {
            /* we'll need to make a new copy of the string */
            OMIT(printf("%s: copying %d-byte string\n", codeid(), len););
            dstr = value_string_new(strbase, len);
            if (NULL != dstr) {
                value_string_get(dstr, out_buf, out_len);
            }
        }
    } else
        dstr = NULL;

    return dstr;
}






/*****************************************************************************
 *                                                                           *
 *          Sub-String Values                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/







typedef struct
{   value_stringbase_t base;           /* base string type */
    value_stringbase_t *ref;           /* string referred to */
    size_t len;
    size_t offset;
} value_substring_t;


#define value_substring_value(v) value_stringbase_value(&(v)->base)



static value_type_t type_substring_val;  /*a new implementation of type_string*/




static bool
value_substring_get_fn(const value_stringbase_t *value,
                       const char **out_buf, size_t *out_len)
{   value_substring_t *str = (value_substring_t *)value;
    const char *refstr;
    size_t refsize;
    size_t offset = str->offset;
    bool ok = value_stringbase_get(str->ref, &refstr, &refsize);

    if (offset < refsize)
    {   *out_buf = refstr+offset;
        if (offset + str->len <= refsize)
            *out_len = str->len;
        else
        {   *out_len = refsize - offset;
            ok = FALSE;
        }
    } else
    {   *out_buf = refstr+refsize;
        *out_len = 0;
        ok = FALSE;
    }
    return ok;
}








static void
value_substring_markver(const value_t *value, int heap_version)
{   value_substring_t *str = (value_substring_t *)value;
    value_mark_version(value_stringbase_value(str->ref), heap_version);
}









STATIC_INLINE value_t *
value_substring_init(value_substring_t *str, value_stringbase_t *refstr,
                     size_t offset, size_t len, bool on_heap)
{   value_t *initval;
    str->ref    = refstr;
    str->offset = offset;
    str->len    = len;
    str->base.get = &value_substring_get_fn;
    initval = value_init(value_stringbase_value(&str->base),
                         &type_substring_val, on_heap);
    value_unlocal(value_stringbase_value(refstr));
    return initval;
}







#if 0 != DEBUG_VALINIT(1+)0
#define value_substring_init(str, refstr, offset, len, on_heap)         \
    value_info((value_substring_init)(str, refstr, offset, len, on_heap),  \
               __LINE__)
#endif





extern value_t *
value_substring_new(const value_t *string, size_t offset, size_t len)
{   value_substring_t *str = NULL;
    value_t *newstr = NULL;

    if (value_type_equal(string, type_string))
    {   str = (value_substring_t *)FTL_MALLOC(sizeof(value_substring_t));
        if (NULL != str)
            newstr = value_substring_init(str, (value_stringbase_t *)string,
                                          offset, len, /*on_heap*/TRUE);
    }
    return newstr;
}










/*****************************************************************************
 *                                                                           *
 *          String Values                                                    *
 *          =============                                                    *
 *                                                                           *
 *****************************************************************************/




value_string_t value_string_empty_str;

const value_t *value_string_empty = value_string_value(&value_string_empty_str);



static void
values_string_init(void)
{
    type_id_t string_type_id = type_id_new();

    value_cstring_init(&value_string_empty_str,  "", 0, /*on_heap*/FALSE);

    type_init(&type_string_val, /*on_heap*/FALSE, string_type_id, "string",
              &value_string_print, /*&value_string_parse*/NULL,
              &value_string_compare, &value_delete_alloced,
              /*mark*/NULL);

    type_init(&type_cstringlit_val, /*on_heap*/FALSE, string_type_id, "string",
              &value_string_print, /*&value_string_parse*/NULL,
              &value_string_compare, &value_delete_alloced,
              /*mark*/NULL);

    type_init(&type_stringlit_val, /*on_heap*/FALSE, string_type_id, "string",
              &value_string_print, /*&value_string_parse*/NULL,
              &value_string_compare, &value_string_delete,
              /*mark*/NULL);

    type_init(&type_substring_val, /*on_heap*/FALSE, string_type_id, "string",
              &value_string_print, /*&value_string_parse*/NULL,
              &value_string_compare, &value_delete_alloced,
              &value_substring_markver);
}





/*****************************************************************************
 *                                                                           *
 *          Code body Values                                                 *
 *          ================                                                 *
 *                                                                           *
 *****************************************************************************/







typedef struct
{   value_t value;           /* code used as a value */
    const value_t *string;   /* code body */
    code_place_t place;      /* definition location */
} value_code_t;



#define value_code_value(code) (&(code)->value)




static value_type_t type_code_val;
type_t type_code = &type_code_val;




static void
value_code_markver(const value_t *value, int heap_version)
{   value_code_t *code = (value_code_t *)value;
    value_mark_version((value_t *)/*unconst*/code->string, heap_version);
}





static void
value_code_delete(value_t *value)
{   if (value_istype(value, type_code))
    {   value_code_t *code = (value_code_t *)value;
        code->string = NULL; /* should be garbage collected */
        FTL_FREE(code);
    }
    /* else type error */
}








static int
value_code_print(outchar_t *out,
                 const value_t *root, const value_t *value)
{   const char *chars;
    size_t len;
    (void)root;
    if (value_code_buf(value, &chars, &len))
    {   int outlen = outchar_putc(out, '{');
        outlen += outchar_write(out, chars, len);
        outlen += outchar_putc(out, '}');
        return outlen;
    } else
        return outchar_printf(out, "NOCODE");
}








static int /* <0 for less than, ==0 for equal, >0 for greater */
value_code_compare(const value_t *v1, const value_t *v2)
{   return -1; /* TODO: implement me */
}




static value_t *
value_code_init(value_code_t *code, const value_t *string,
                const char *sourcename, int lineno, bool on_heap)
{   value_t *initval;
    code->string = string;
    DEBUG_LNO(printf("%s: CODE obj defined at %s:%d\n",
                     codeid(), sourcename, lineno););
    code_place_set(&code->place, sourcename, lineno);
    initval = value_init(&code->value, type_code, on_heap);
    value_unlocal(string);
    return initval;
}







extern value_t *
value_code_new(const value_t *string, const char *sourcename, int lineno)
{   value_code_t *code = NULL;

    if (value_istype(string, type_string))
    {   code = (value_code_t *)FTL_MALLOC(sizeof(value_code_t));
        if (NULL != code)
            value_code_init(code, string, sourcename, lineno, /*on_heap*/TRUE);
    }
    return code==NULL? NULL: &code->value;
}





extern bool
value_code_buf(const value_t *value, const char **out_buf, size_t *out_len)
{   bool ok = FALSE;

    *out_buf = NULL;
    *out_len = 0;

    if (value_istype(value, type_code))
    {   const value_code_t *code = (const value_code_t *)value;
        value_string_get(code->string, out_buf, out_len);
        ok = TRUE;
    }
    /* else type error */

    return ok;
}







static bool
value_code_place(const value_t *value, const char **out_posname,
                 int *out_lineno)
{   bool ok = FALSE;

    *out_posname = NULL;
    *out_lineno = 0;

    if (value_istype(value, type_code))
    {   const value_code_t *code = (const value_code_t *)value;
        *out_posname = &code->place.posname[0];
        *out_lineno  = code->place.lineno;
        DEBUG_LNO(printf("%s: CODE obj start read as %s:%d\n",
                         codeid(), *out_posname, *out_lineno););
        ok = TRUE;
    }
    /* else type error */

    return ok;
}






static void
values_code_init(void)
{
    type_id_t code_type_id = type_id_new();

    type_init(&type_code_val, /*on_heap*/FALSE, code_type_id, "code",
              &value_code_print, /*&value_code_parse*/NULL,
              &value_code_compare, &value_code_delete,
              &value_code_markver);
}








/*****************************************************************************
 *                                                                           *
 *          Stream Values                                                    *
 *          =============                                                    *
 *                                                                           *
 *****************************************************************************/










typedef struct
{   value_t value;           /* code used as a value */
    charsource_t *source;
    charsink_t *sink;
    stream_close_fn_t *close;
    stream_sink_close_fn_t *sink_close;
    stream_sink_delete_fn_t *sink_delete;
} value_stream_t;







#define value_stream_value(stream) (&(stream)->value)






static void
value_stream_delete(value_t *value)
{   if (value_istype(value, type_stream))
    {   value_stream_t *stream = (value_stream_t *)value;
        if (NULL != stream->source)
            charsource_delete(&stream->source);
        if (NULL != stream->sink && NULL != stream->sink_delete)
            (*stream->sink_delete)(&stream->sink);
        FTL_FREE(stream);
    }
    /* else type error */
}







extern void
value_stream_close(value_t *value)
{   if (value_istype(value, type_stream))
    {   value_stream_t *stream = (value_stream_t *)value;
        if (NULL != stream->source)
            charsource_close(stream->source);
        if (NULL != stream->sink && NULL != stream->sink_close)
            (*stream->sink_delete)(&stream->sink);
        if (NULL != stream->close)
            (*stream->close)(value);
    }
    /* else type error */
}








static int
value_stream_print(outchar_t *out,
                   const value_t *root, const value_t *value)
{   charsource_t *source;
    charsink_t   *sink;
    (void)root;

    if (value_stream_source(value, &source) &&
        value_stream_sink(value, &sink))
    {   int n = 0;

        n += outchar_printf(out, "$stream.");
        if (NULL == source)
            n += outchar_printf(out, "EOF");
        else
            n += outchar_printf(out, "'%s'", source->name);

        if (NULL != sink)
            n += outchar_printf(out, "");

        return n;
    }
    else
        return outchar_printf(out, "$stream.NOSTREAM");
}








static value_t *
value_stream_init(value_stream_t *stream, type_t stream_imp_type,
                  charsource_t *source,
                  charsink_t *sink,
                  stream_close_fn_t *close,
                  stream_sink_close_fn_t *sink_close,
                  stream_sink_delete_fn_t *sink_delete,
                  bool on_heap)
{   stream->source = source;
    stream->sink = sink;
    stream->close = close;
    stream->sink_delete = sink_delete;
    stream->sink_close = sink_close;
    return value_init(&stream->value, stream_imp_type, on_heap);
}










extern bool
value_stream_source(const value_t *value, charsource_t **out_source)
{   bool ok = FALSE;

    *out_source = NULL;

    if (value_istype(value, type_stream))
    {   const value_stream_t *stream = (const value_stream_t *)value;
        *out_source = stream->source;
        ok = TRUE;
    }
    /* else type error */

    return ok;
}







extern bool
value_stream_takesource(value_t *value, charsource_t **out_source)
{   bool ok = FALSE;

    *out_source = NULL;

    if (value_istype(value, type_stream))
    {   value_stream_t *stream = (value_stream_t *)value;
        *out_source = stream->source;
        stream->source = NULL;
        ok = TRUE;
    }
    /* else type error */

    return ok;
}







extern bool
value_stream_sink(const value_t *value, charsink_t **out_sink)
{   bool ok = FALSE;

    *out_sink = NULL;

    if (value_istype(value, type_stream))
    {   const value_stream_t *stream = (const value_stream_t *)value;
        *out_sink = stream->sink;
        ok = TRUE;
    }
    /* else type error */

    return ok;
}









/*****************************************************************************
 *                                                                           *
 *          File Stream Values                                               *
 *          ==================                                               *
 *                                                                           *
 *****************************************************************************/









typedef struct
{   value_stream_t stream;
    FILE *file;
} value_stream_file_t;




static value_type_t type_stream_file_val;
type_t type_stream = &type_stream_file_val;





#define value_stream_file_stream(fs) (&(fs)->stream)







static void
value_stream_openfile_close(value_t *stream)
{   value_stream_file_t *fstream = (value_stream_file_t *)stream;
    if (NULL != fstream->file)
    {   fclose(fstream->file);
        fstream->file = NULL;
    }
}









extern value_t *
value_stream_openfile_new(FILE *file, bool autoclose,
                          const char *name, bool read, bool write)
{   value_stream_file_t *fstream = (value_stream_file_t *)
                                   FTL_MALLOC(sizeof(value_stream_file_t));
    if (NULL != fstream)
    {   charsource_t *source = NULL;
        charsink_t *sink = NULL;
        stream_sink_close_fn_t *sink_close = NULL;
        stream_sink_delete_fn_t *sink_delete = NULL;
        stream_close_fn_t *close = NULL;

        if (write)
        {   sink = charsink_stream_new(file);
            sink_close = &charsink_stream_close;
            sink_delete = &charsink_stream_delete;
        }

        if (read)
            source = charsource_stream_new(file, name, /*autoclose*/FALSE);

        if (autoclose)
            close = &value_stream_openfile_close;

        fstream->file = file;
        return value_stream_init(&fstream->stream, &type_stream_file_val,
                                 source, sink, close,
                                 sink_close, sink_delete, /*on_heap*/TRUE);
    } else
        return NULL;
}









extern value_t *
value_stream_file_new(const char *name, bool binary, bool read, bool write)
{   const char *mode;
    FILE *str;

    if (binary)
       mode = (read? (write? "rwb": "rb"): (write? "wb": "b"));
    else
       mode = (read? (write? "rw": "r"): (write? "w": ""));

    str = fopen(name, mode);

    if (NULL == str)
    {   OMIT(fprintf(stderr,"%s: can't open '%s' in mode '%s'\n",
                       codeid(), name, mode););
        return NULL;
    } else
        return value_stream_openfile_new(str, /* autoclose */TRUE, name,
                                         read, write);
}






extern value_t *
value_stream_file_path_new(const char *path, const char *name, size_t namelen,
                           bool binary, bool read, bool write,
                           char *namebuf, size_t buflen)
{   const char *mode;
    FILE *str;

    if (binary)
       mode = (read? (write? "rwb": "rb"): (write? "wb": "b"));
    else
       mode = (read? (write? "rw": "r"): (write? "w": ""));

    /* expand file name into namebuf[] */
    str = fopen_onpath(path, name, namelen, mode, namebuf, buflen);

    if (NULL == str)
        return NULL;
    else
        return value_stream_openfile_new(str, /* autoclose */TRUE, name,
                                         read, write);
}









/*****************************************************************************
 *                                                                           *
 *          Socket Stream Values                                             *
 *          ====================                                             *
 *                                                                           *
 *****************************************************************************/







#ifdef HAS_SOCKETS




typedef struct
{   value_stream_t stream;
    int fd;
} value_stream_socket_t;




static value_type_t type_stream_socket_val;





#define value_stream_socket_stream(ss) (&(ss)->stream)







static void
value_stream_opensocket_close(value_t *stream)
{   value_stream_socket_t *fstream = (value_stream_socket_t *)stream;
    if (fstream->fd >= 0)
    {   close(fstream->fd);
        fstream->fd = -1;
    }
}









static bool
parse_hostport(const char **ref_line, parser_state_t *state,
               addr_ip_t *out_ip, number_t *out_port)
{   /* syntax: <host> | [:]<port> | <host>:<port>
               (host is <dotted quad>|<ip addr name>)
     */
    bool hashost = parse_ipaddr(ref_line, out_ip);
    bool hasport = FALSE;
    bool ok = TRUE;

    if (hashost)
        hasport = parse_key(ref_line, ":") &&
                  (ok = parse_int_base(ref_line, state, out_port));
    else
    {   (void)parse_key(ref_line, ":");
        hasport = parse_int_base(ref_line, state, out_port);
    }
    if (!hashost && !hasport)
        ok = FALSE;

    if (ok)
    {   /* set defaults (otherwise leave outputs alone) */
        if (!hashost)
            memset(out_ip, 0, sizeof(*out_ip));
        if (!hasport)
            *out_port = 0;
    }

    return ok;
}






static bool
parse_sockaddr(const char **ref_line, parser_state_t *state,
               struct sockaddr_in *addr)
{   addr_ip_t host;
    number_t port;
    bool ok = parse_hostport(ref_line, state, &host, &port);
    if (ok)
    {   addr->sin_addr.s_addr = ipaddr_net32(&host);
        /*printf("%s: host %X becomes %X\n",
                 codeid(), *(unsigned *)&host,
                 (unsigned)addr->sin_addr.s_addr);
        */
        addr->sin_port = htons((unsigned short)port);
    }

    if (ok)
        addr->sin_family = AF_INET;

    return ok;
}







extern value_t *
value_stream_opensocket_new(int fd, bool autoclose,
                            const char *name, bool read, bool write)
{   value_stream_socket_t *fstream = (value_stream_socket_t *)
                                     FTL_MALLOC(sizeof(value_stream_socket_t));
    if (NULL != fstream)
    {   charsource_t *source = NULL;
        charsink_t *sink = NULL;
        stream_sink_close_fn_t *sink_close = NULL;
        stream_sink_delete_fn_t *sink_delete = NULL;
        stream_close_fn_t *close = NULL;

        if (write)
        {   sink = charsink_socket_new(name, fd);
            sink_close = &charsink_stream_close;
            sink_delete = &charsink_stream_delete;
        }

        if (read)
            source = charsource_socket_new(fd, recv_flags, name,
                                           /*autoclose*/FALSE);

        if (autoclose)
            close = &value_stream_opensocket_close;

        fstream->socket = socket;
        return value_stream_init(&fstream->stream, &type_stream_socket_val,
                                 source, sink, close,
                                 sink_close, sink_delete, /*on_heap*/TRUE);
    } else
        return NULL;
}









extern value_t *
value_stream_socket_new(const char *name, bool read, bool write)
{   const char *mode = (read? (write? "rw": "r"): (write? "w": ""));
    struct sockaddr_in str = fopen(name, mode);

    if (NULL == str)
        return NULL;
    else
        return value_stream_opensocket_new(str, /* autoclose */TRUE, name,
                                           read, write);
}


#endif










/*****************************************************************************
 *                                                                           *
 *          InString Stream Values                                           *
 *          ======================                                           *
 *                                                                           *
 *****************************************************************************/









typedef struct
{   value_stream_t stream;
} value_stream_instring_t;




static value_type_t type_stream_instring_val;


#define value_stream_instring_stream(ins) (&(ins)->stream)







extern value_t *
value_stream_instring_new(const char *name, const char *string, size_t len)
{   value_stream_instring_t *instrstream =
        (value_stream_instring_t *)FTL_MALLOC(sizeof(value_stream_instring_t));

    if (NULL != instrstream)
    {   charsource_t *source = charsource_string_new(name, string, len);

        return value_stream_init(&instrstream->stream,
                                 &type_stream_instring_val,
                                 source, /*sink*/NULL,  /*close*/NULL,
                                 /*sink_close*/NULL, /*sink_delete*/NULL,
                                 /*on_heap*/TRUE);
    } else
        return NULL;
}










/*****************************************************************************
 *                                                                           *
 *          OutString Stream Values                                          *
 *          =======================                                          *
 *                                                                           *
 *****************************************************************************/









typedef struct
{   value_stream_t stream;
} value_stream_outstring_t;




static value_type_t type_stream_outstring_val;
static value_type_t type_stream_outmem_val;




#define value_stream_outstring_stream(outs) (&(outs)->stream)







extern value_t *
value_stream_outstring_new(void)
{   value_stream_outstring_t *outstrstream =
        (value_stream_outstring_t *)
        FTL_MALLOC(sizeof(value_stream_outstring_t));

    if (NULL != outstrstream)
    {   charsink_t *sink = charsink_string_new();

        return value_stream_init(&outstrstream->stream,
                                 &type_stream_outstring_val,
                                 /*source*/NULL, /*sink*/sink, /*close*/NULL,
                                 /*sink_close*/&charsink_string_close,
                                 /*sink_delete*/&charsink_string_delete,
                                 /*on_heap*/TRUE);
    } else
        return NULL;
}







extern value_t *
value_stream_outmem_new(char *str, size_t len)
{   value_stream_outstring_t *outstrstream =
        (value_stream_outstring_t *)
        FTL_MALLOC(sizeof(value_stream_outstring_t));

    if (NULL != outstrstream)
    {   charsink_t *sink = charsink_fixstring_new(str, len);

        return value_stream_init(&outstrstream->stream, &type_stream_outmem_val,
                                 /*source*/NULL, /*sink*/sink, /*close*/NULL,
                                 /*sink_close*/&charsink_fixstring_close,
                                 /*sink_delete*/&charsink_fixstring_delete,
                                 /*on_heap*/TRUE);
    } else
        return NULL;
}







extern value_t *
value_stream_outstring_string(value_stream_outstring_t *stream)
{   charsink_t *sink;
    if (value_stream_sink(
            value_stream_value(value_stream_outstring_stream(stream)), &sink))
    {   const char *buf;
        size_t len;
        charsink_string_buf(sink, &buf, &len);
        return value_string_new(buf, len);
    } else
        return NULL;
}









/*****************************************************************************
 *                                                                           *
 *          Stream Values                                                    *
 *          =============                                                    *
 *                                                                           *
 *****************************************************************************/









static void
values_stream_init(void)
{
    type_id_t stream_type_id = type_id_new();

    type_init(&type_stream_file_val, /*on_heap*/FALSE,
              stream_type_id, "stream",
              &value_stream_print, /*&value_stream_parse*/NULL,
              /*&value_stream_compare*/NULL, &value_stream_delete,
              /*&value_stream_markver*/NULL);

#ifdef HAS_SOCKETS
    type_init(&type_stream_socket_val, /*on_heap*/FALSE,
              stream_type_id, "stream",
              &value_stream_print, /*&value_stream_parse*/NULL,
              /*&value_stream_compare*/NULL, &value_stream_delete,
              /*&value_stream_markver*/NULL);
#endif

    type_init(&type_stream_instring_val, /*on_heap*/FALSE,
              stream_type_id, "stream",
              &value_stream_print, /*&value_stream_parse*/NULL,
              /*&value_stream_compare*/NULL, &value_stream_delete,
              /*&value_stream_markver*/NULL);

    type_init(&type_stream_outstring_val, /*on_heap*/FALSE,
              stream_type_id, "stream",
              &value_stream_print, /*&value_stream_parse*/NULL,
              /*&value_stream_compare*/NULL, &value_stream_delete,
              /*&value_stream_markver*/NULL);

    type_init(&type_stream_outmem_val, /*on_heap*/FALSE,
              stream_type_id, "stream",
              &value_stream_print, /*&value_stream_parse*/NULL,
              /*&value_stream_compare*/NULL, &value_stream_delete,
              /*&value_stream_markver*/NULL);

#if 0
        (stream)
        return value_stream_init(&fstream->stream, &type_stream_val,
                                 source, sink,
                                 &value_stream_delete,
                                 close, sink_close, sink_delete);
        (instring)
        return value_stream_init(&instrstream->stream,
                                 &type_stream_instream_val, source,
                                 /*sink*/NULL, &value_stream_delete,
                                 /*close*/NULL,
                                 /*sink_close*/NULL, /*sink_delete*/NULL);
        (outstring)
        return value_stream_init(&outstrstream->stream,
                                 &type_stream_outstring_val,
                                 /*source*/NULL,
                                 /*sink*/sink, &value_stream_delete,
                                 /*close*/NULL,
                                 /*sink_close*/&charsink_string_close,
                                 /*sink_delete*/&charsink_string_delete);
        (stream outmem)
        return value_stream_init(&outstrstream->stream,
                                 &type_stream_outmem_val,
                                 /*source*/NULL,
                                 /*sink*/sink, &value_stream_delete,
                                 /*close*/NULL,
                                 /*sink_close*/&charsink_fixstring_close,
                                 /*sink_delete*/&charsink_fixstring_delete);
#endif
}







/*****************************************************************************
 *                                                                           *
 *          Directories                                                      *
 *          ===========                                                      *
 *                                                                           *
 *****************************************************************************/









extern value_t *
(dir_value)(dir_t *dir)
{   return &dir->value;
}





typedef struct
{   outchar_t *out;
    const value_t *root;
    int len;
    bool first;
    bool bracketed;
    const char *delim;
    number_t index_expect;
} dir_bind_print_arg_t;







static void *
value_dir_bind_print(dir_t *dir, const value_t *name,
                     const value_t *value, void *arg)
{   dir_bind_print_arg_t *pr = (dir_bind_print_arg_t *)arg;
    outchar_t *out = pr->out;
    const value_t *root = pr->root;
    bool is_root = (PTRVALID(root) && value == root);

    if (is_root)
    {   if (pr->bracketed)
        {   pr->len += outchar_printf(out, "]::");
            pr->bracketed = FALSE;
        }
        pr->len += outchar_printf(out, "(_env)");
        return (void *)root; /* stop print */
    } else
    {
        if (!pr->bracketed)
        {   pr->len += outchar_printf(out, "[");
            pr->bracketed = TRUE;
        }
        if (pr->first)
            pr->first = FALSE;
        else
            pr->len += outchar_printf(out, pr->delim);

        if (value_type_equal(name, type_string))
            pr->len += value_string_print_as_id(out, root, name);
        else
            pr->len += value_print(out, root, name);
        pr->len += outchar_printf(out, "=");
        OMIT(pr->len += outchar_printf(out, "(%s)#%p ",
                                         value_type_name(value), value);)
        pr->len += value_print(out, root, value);

        return NULL;
    }
}








static int
dir_print(outchar_t *out, const value_t *root, const value_t *value)
{   dir_bind_print_arg_t pr;

    pr.out = out;
    pr.root = root;
    pr.len = 0;

    if (value_istype(value, type_dir))
    {   dir_t *dir = (dir_t *)value;

        pr.first  = TRUE;
        pr.bracketed  = FALSE;
        pr.delim  = ", ";

        if (NULL != dir->forall)
            (void)(*dir->forall)(dir, &value_dir_bind_print, &pr);
        else
            pr.len += outchar_printf(out, "<unenumerable-dir>");

        if (pr.bracketed)
            pr.len += outchar_printf(out, "]");
    }

    return pr.len;
}







static void *
value_dir_bind_print_name(dir_t *dir, const value_t *name,
                          const value_t *value, void *arg)
{   dir_bind_print_arg_t *pr = (dir_bind_print_arg_t *)arg;

    if (pr->first)
        pr->first = FALSE;
    else
        pr->len += outchar_printf(pr->out, pr->delim);

    if (value_type_equal(name, type_string))
        pr->len += value_string_print_as_id(pr->out, pr->root, name);
    else
        pr->len += value_print(pr->out, pr->root, name);
    return NULL;
}





/* used for debugging */
static int
dir_print_names(outchar_t *out, const value_t *root, const value_t *value)
{   dir_bind_print_arg_t pr;

    pr.out = out;
    pr.root = root;
    pr.len = 0;

    if (value_type_equal(value, type_dir))
    {   dir_t *dir = (dir_t *)value;

        pr.first  = TRUE;
        pr.delim  = ", ";

        if (NULL != dir->forall)
        {   pr.len += outchar_printf(out, "[");
            (void)(*dir->forall)(dir, &value_dir_bind_print_name, &pr);
            pr.len += outchar_printf(out, "]");
        }
    } else if (value == NULL)
    {   pr.len += outchar_printf(out, "<NULL dir>");
    } else
    {   pr.len += outchar_printf(out, "<not dir - %s>",
                                 value_type_name(value));
    }

    return pr.len;
}







extern int
dir_fprint(FILE *out, const value_t *root, dir_t *dir)
{   charsink_stream_t charbuf;
    charsink_t *sink = charsink_stream_init(&charbuf, out);
    int n = dir_print_names(sink, root, dir_value(dir));
    charsink_stream_close(sink);
    return n;
}






extern bool
dir_set(dir_t *dir, const value_t *nameval, const value_t *value)
{   bool ok = FALSE;

    DEBUG_DIR(printf("%s: dir - set name %p to %p (%s) in dir %p\n",
                    codeid(), nameval, value, value_type_name(value), dir);)
    if (PTRVALID(dir))
    {   if (PTRVALID(dir->lookup))
        {   const value_t **ref_value = (*dir->lookup)(dir, nameval);

            if (PTRVALID(ref_value))
            {   /* update existing value - leave the old value for the
                   garbage collector to pick up: someone else might have
                   a pointer to it */
                *ref_value = value;
                ok = TRUE;
            } else
            if (PTRVALID(dir->add))
            {   ok = (*dir->add)(dir, nameval, value);
                value_unlocal(nameval);
                value_unlocal(value);
            }
        } else
        if (PTRVALID(dir->add))
        {   ok = (*dir->add)(dir, nameval, value);
            value_unlocal(nameval);
            value_unlocal(value);
        }
        else
        {   fprintf(stderr, "%s: name ", codeid());
            value_fprint(stderr, /*root*/NULL, nameval);
            fprintf(stderr, " has a constant value\n");
        }
    }
    return ok;
}






extern const value_t *
dir_get(dir_t *dir, const value_t *name)
{   if (PTRVALID(dir) &&
        PTRVALID(dir->get))
    {   DEBUG_DIR(printf("%s: dir - get name %p (%s) from dir %p (%s)\n",
                        codeid(), name, value_type_name(name),
                        dir, value_type_name(dir_value(dir)));)
        return (*dir->get)(dir, name);
    } else
        return NULL;
}






static const value_t *
dir_get_from_lookup(dir_t *dir, const value_t *name)
{   const value_t **ref_value = NULL;

    /*DEBUG_DIR*/
    OMIT(printf("%s: dir - get name %p (%s) using %slookup dir %p (%s)\n",
                    codeid(), name, value_type_name(name),
                    dir==NULL || dir->lookup == NULL? "bad ": "",
                    dir, value_type_name(dir_value(dir)));)
    if (PTRVALID(dir) &&
        PTRVALID(dir->lookup))
        ref_value = (*dir->lookup)(dir, name);

    return ref_value == NULL? NULL: *ref_value;
}






#if 0
extern bool
dir_noadd(dir_t *dir, const value_t *nameval, const value_t *value)
{   fprintf(stderr, "%s: can't write to constant field ", codeid());
    value_fprint(stderr, /*root*/NULL, nameval);
    fprintf(stderr, "\n");
    return FALSE;
}
#endif






static void *
dir_for_count(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   unsigned *lv_len = (unsigned *)arg;
    if (PTRVALID(value))
        /* don't count names with value of NULL */
        (*lv_len)++;
    return NULL;
}






static unsigned
dir_count_from_forall(dir_t *dir)
{   unsigned len = 0;

    DEBUG_DIR(printf("%s: dir - count using forall dir %p\n", codeid(), dir);)
    if (PTRVALID(dir) &&
        PTRVALID(dir->forall))
        (void)(*dir->forall)(dir, &dir_for_count, &len);

    return len;
}






extern dir_lock_state_t *
dir_lock(dir_t *dir, dir_lock_state_t *old_lock)
{   if (PTRVALID(dir))
    {   dir_add_fn_t *old_addfn = dir->add;
        dir_add_fn_t *addfn = (dir_add_fn_t *)old_lock;
        dir->add = addfn;
        return (void *)old_addfn;
    } else
        return NULL;
}





extern bool
dir_islocked(dir_t *dir)
{   return dir==NULL || NULL == dir->add;
}





static int /* <0 for less than, ==0 for equal, >0 for greater */
dir_compare(const value_t *v1, const value_t *v2)
{   return -1; /* TODO: implement me */
}






extern /*internal*/ value_t *
dir_init(dir_t *dir, type_t dir_subtype,
         dir_add_fn_t *add, dir_lookup_fn_t *lookup,
         dir_get_fn_t *get, dir_forall_fn_t *forall,
         bool on_heap)
{   dir->add = add;
    dir->lookup = lookup;
    if (NULL == get && PTRVALID(lookup))
        dir->get = &dir_get_from_lookup;
    else
        dir->get = get;
    dir->forall = forall;
    dir->count = &dir_count_from_forall;
    dir->env_end = FALSE;

    return value_init(&dir->value, dir_subtype, on_heap);
}





#if 0 != DEBUG_VALINIT(1+)0
#define dir_init(dir, kind, add, lookup, get, forall, on_heap) \
    value_info((dir_init)(dir, kind, add, lookup, get, forall, on_heap), \
               __LINE__)
#endif




extern bool
dir_int_set(dir_t *dir, int index, const value_t *value)
{   value_int_t nameval;
    value_int_init(&nameval, index, /*on_heap*/FALSE);
    return dir_set(dir, &nameval.value, value);
}






extern bool
dir_string_set(dir_t *dir, const char *name, const value_t *value)
{   value_t *nameval = value_string_new_measured(name);
    return NULL == nameval? FALSE: dir_set(dir, nameval, value);
}





extern bool
dir_cstring_set(dir_t *dir, const char *name, const value_t *value)
{   value_t *nameval = value_cstring_new_measured(name);
    return NULL == nameval? FALSE: dir_set(dir, nameval, value);
}





extern const value_t *
dir_int_get(dir_t *dir, int n)
{   value_int_t nameval;
    value_int_init(&nameval, n, /*on_healp*/FALSE);
    return dir_get(dir, &nameval.value);
}




/*! Use a constant NUL-terminated string to index a directory
 */
extern const value_t *
dir_string_get(dir_t *dir, const char *name)
{   value_string_t nameval;
    value_cstring_init(&nameval, name, strlen(name), /*on_heap*/FALSE);
    return dir_get(dir, value_string_value(&nameval));
}







/*! Use a constant string (with given length) to index a directory
 */
extern const value_t *
dir_stringl_get(dir_t *dir, const char *name, size_t namelen)
{   value_string_t nameval;
    value_cstring_init(&nameval, name, namelen, /*on_heap*/FALSE);
    OMIT(
        printf("%s: look up '", codeid());
        value_fprint(stdout, /*root*/NULL, value_string_value(&nameval));
        printf("'\n");)
    return dir_get(dir, value_string_value(&nameval));
}






extern bool
dir_enumerable(dir_t *dir)
{    return (PTRVALID(dir) &&
             PTRVALID(dir->forall));
}





extern void *
dir_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{    if (PTRVALID(dir) &&
         PTRVALID(dir->forall))
        return (*dir->forall)(dir, enumfn, arg);
     else
        return NULL;
}






extern unsigned
dir_count(dir_t *dir)
{    if (PTRVALID(dir) &&
         PTRVALID(dir->count))
        return (*dir->count)(dir);
     else
        return 0;
}






#define dir_env_end(_dir) ((_dir)->env_end)

#define dir_env_end_set(_dir, _end) (_dir)->env_end = _end







/*****************************************************************************
 *                                                                           *
 *          Identifier Directories                                           *
 *          ======================                                           *
 *                                                                           *
 *****************************************************************************/







typedef struct binding_s
{   struct binding_s *link;
    value_t *name;
    const value_t *value;
} binding_t;



typedef struct
{   dir_t dir;
    binding_t *bindlist;
    binding_t **list_end;
} dir_id_t;




static value_type_t type_dir_id_val;
type_t type_dir = &type_dir_id_val;





#define dir_id_dir(dirid) (&((dirid)->dir))
#define dir_id_value(dirid) dir_value(dir_id_dir(dirid))






static void dir_id_delete(value_t *value)
{   if (value_istype(value, type_dir))
    {   dir_id_t *iddir = (dir_id_t *)value;
        binding_t *bind = iddir->bindlist;

        while (PTRVALID(bind))
        {   binding_t *doomed = bind;
            bind = bind->link;
            FTL_FREE(doomed);
        }
        FTL_FREE(value);
    }
    /* else type error */
}




static void dir_id_markver(const value_t *value, int heap_version)
{   dir_id_t *iddir = (dir_id_t *)value;
    binding_t *bind = iddir->bindlist;

    while (PTRVALID(bind))
    {   value_mark_version(bind->name, heap_version);
        value_mark_version((value_t *)/*unconst*/bind->value, heap_version);
        bind = bind->link;
    }
}





static bool
dir_id_add(dir_t *dir, const value_t *name, const value_t *value)
{   dir_id_t *iddir = (dir_id_t *)dir;
    binding_t *newbind = (binding_t *)NULL;

    if (value_istype(name, type_string))
    {   newbind = (binding_t *)FTL_MALLOC(sizeof(binding_t));

        if (PTRVALID(newbind))
        {   newbind->name = (value_t *)/*unconst*/name;
            newbind->value = value;
            newbind->link = NULL;

            *iddir->list_end = newbind;
            iddir->list_end = &newbind->link;
        }
    }
    return PTRVALID(newbind);
}





static const value_t **
dir_id_lookup(dir_t *dir, const value_t *nameval)
{   if (value_type_equal(nameval, type_string))
    {   dir_id_t *iddir = (dir_id_t *)dir;
        binding_t *bind = iddir->bindlist;
        const char *name;
        size_t namelen;
        const char *bindname;
        size_t bindnamelen;

        value_string_get(nameval, &name, &namelen);

        while (PTRVALID(bind) &&
               value_string_get(bind->name, &bindname, &bindnamelen) &&
               (bindnamelen != namelen ||
                0 != memcmp(bindname, name, namelen)))
            bind = bind->link;

        return NULL == bind? (const value_t **)NULL: &bind->value;
    } else
        return NULL;
}





static void *
dir_id_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_id_t *iddir = (dir_id_t *)dir;
    binding_t *bind = iddir->bindlist;
    void *result = NULL;

    while (PTRVALID(bind) && NULL == result)
    {   result = (*enumfn)(dir, bind->name, bind->value, arg);
        bind = bind->link;
    }

    return result;
}





static void
dir_id_init(dir_id_t *iddir, type_t dir_subtype, bool on_heap)
{

    dir_init(&iddir->dir, dir_subtype, &dir_id_add, &dir_id_lookup,
             /*get*/ NULL, &dir_id_forall, on_heap);
    iddir->bindlist = NULL;
    iddir->list_end = &iddir->bindlist;
}






extern dir_t *
dir_id_new(void)
{   dir_id_t *iddir = (dir_id_t *)FTL_MALLOC(sizeof(dir_id_t));

    if (PTRVALID(iddir))
        dir_id_init(iddir, &type_dir_id_val, /*on_heap*/TRUE);

    return &iddir->dir;
}








/*****************************************************************************
 *                                                                           *
 *          Integer vector Directories                                       *
 *          ==========================                                       *
 *                                                                           *
 *****************************************************************************/






#define DIR_VEC_N_INIT 10




typedef struct
{   dir_t dir;
    const value_t **bindvec;
    size_t n;
    size_t maxn;
} dir_vec_t;



static value_type_t type_dir_vec_val;



#define dir_vec_dir(vec) (&(vec)->dir)
#define dir_vec_value(vec) dir_value(dir_vec_dir(vec))




static void
dir_vec_delete(value_t *value)
{   if (value_istype(value, type_dir))
    {   dir_vec_t *vecdir = (dir_vec_t *)value;
        if (PTRVALID(vecdir->bindvec))
        {   FTL_FREE((value_t *)/*unconst*/vecdir->bindvec);
            vecdir->bindvec = NULL;
        }
        FTL_FREE(value);
    }
    /* else type error */
}




static void
dir_vec_markver(const value_t *value, int heap_version)
{   dir_vec_t *vecdir = (dir_vec_t *)value;
    const value_t **bind = vecdir->bindvec;

    if (PTRVALID(bind))
    {   const value_t **endbind = vecdir->bindvec + vecdir->n;

        while (bind < endbind)
        {   value_mark_version((value_t *)/*unconst*/*bind, heap_version);
            bind++;
        }
    }
}





static bool
dir_vec_int_add(dir_t *dir, size_t index, const value_t *value)
{   dir_vec_t *vecdir = (dir_vec_t *)dir;
    bool ok = PTRVALID(vecdir);

    if (ok)
    {   const value_t **bindvec = vecdir->bindvec;

        if (index >= vecdir->maxn)
        {   size_t old_maxn = vecdir->maxn;
            size_t new_maxn = old_maxn == 0? DIR_VEC_N_INIT: 2*old_maxn;
            value_t **newbind;

            if (new_maxn < index+1)
                new_maxn = index+1;

            OMIT(printf("extend vec from %d to %d (index %d)\n",
                          old_maxn, new_maxn, index);)
            newbind = (value_t **)FTL_MALLOC(new_maxn*sizeof(value_t *));
            if (PTRVALID(newbind))
            {   memcpy(newbind, bindvec, old_maxn*sizeof(value_t *));
                memset(newbind+old_maxn, 0,
                       (new_maxn-old_maxn)*sizeof(value_t *));
                vecdir->bindvec = (const value_t **)newbind;
                vecdir->maxn = new_maxn;
                if (PTRVALID(bindvec))
                    FTL_FREE((value_t **)bindvec);
            } else
                ok = FALSE;
        }
        if (ok)
        {   vecdir->bindvec[index] = value;
            if (index+1 > vecdir->n)
                vecdir->n = index+1;
        }
        OMIT(else printf("Failed to add index %d - max is %d\n",
                           (int)index, (int)vecdir->maxn);)
    }

    return ok;
}



static bool
dir_vec_add(dir_t *dir, const value_t *name, const value_t *value)
{   return (value_istype(name, type_int) &&
            dir_vec_int_add(dir, (size_t)value_int_number(name), value));
}




STATIC_INLINE const value_t **
dir_vec_int_lookup(dir_t *dir, size_t index)
{   dir_vec_t *vecdir = (dir_vec_t *)dir;
    const value_t **bindvec = vecdir->bindvec;

    if (PTRVALID(bindvec) && (int)index >= 0 && index < vecdir->n)
        return &bindvec[index];
    else
        return NULL;
}







static const value_t **
dir_vec_lookup(dir_t *dir, const value_t *name)
{   dir_vec_t *vecdir = (dir_vec_t *)dir;
    const value_t **found = NULL;
    const value_t **bindvec = vecdir->bindvec;

    if (value_type_equal(name, type_int) && PTRVALID(bindvec))
    {   size_t index = (size_t)value_int_number(name);
        if ((int)index >= 0 && index < vecdir->n)
            found = &bindvec[index];
    }

    return found;
}





static void *
dir_vec_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_vec_t *vecdir = (dir_vec_t *)dir;
    const value_t **bindvec = vecdir->bindvec;
    void *result = NULL;

    if (PTRVALID(bindvec))
    {   size_t i = 0;

        OMIT(printf("vec has %d entries\n", vecdir->n);)
        while (NULL == result && i < vecdir->n)
        {   const value_t *newint = value_int_new(i);
            result = (*enumfn)(dir, newint, bindvec[i], arg);
            value_unlocal(newint);
            i++;
        }
    }

    return result;
}





static void *
value_dir_vec_print(dir_t *dir, const value_t *name,
                    const value_t *value, void *arg)
{   dir_bind_print_arg_t *pr = (dir_bind_print_arg_t *)arg;
    number_t index = value_int_number(name); /* vectors are indexed by ints */

    if (PTRVALID(value))
    {
        if (pr->first)
            pr->first = FALSE;
        else
            pr->len += outchar_printf(pr->out, pr->delim);

        if (index != pr->index_expect)
        {   pr->len += value_print(pr->out, pr->root, name);
            pr->len += outchar_printf(pr->out, "=");
        }
        OMIT(pr->len += outchar_printf(pr->out, "(%s)#%p ",
                                         value_type_name(value), value);)
        pr->len += value_print(pr->out, pr->root, value);

        pr->index_expect = index+1;
    }
    return NULL;
}








static int
dir_vec_print(outchar_t *out, const value_t *root, const value_t *value)
{   dir_bind_print_arg_t pr;

    pr.out = out;
    pr.root = root;
    pr.len = 0;
    pr.index_expect = 0;

    if (value_istype(value, type_dir))
    {   dir_t *dir = (dir_t *)value;

        pr.first  = TRUE;
        pr.delim  = ", ";
        pr.len += outchar_printf(out, "<");

        if (PTRVALID(dir->forall))
            (void)(*dir->forall)(dir, &value_dir_vec_print, &pr);
        else
            pr.len += outchar_printf(out, "<unenumerable-dir>");

        pr.len += outchar_printf(out, ">");
    }

    return pr.len;
}





static value_t *
dir_vec_init(dir_vec_t *vecdir)
{   value_t *val = dir_init(&vecdir->dir, &type_dir_vec_val,
                            &dir_vec_add, &dir_vec_lookup,
                            /*get*/NULL, &dir_vec_forall, /*on_heap*/FALSE);
    vecdir->bindvec = NULL;
    vecdir->n = 0;
    vecdir->maxn = 0;
    return val;
}




#if 0 != DEBUG_VALINIT(1+)0
#define dir_vec_init(vecdir) value_info((dir_vec_init)(vecdir), __LINE__)
#endif




extern dir_t *
dir_vec_new(void)
{   dir_vec_t *vecdir = (dir_vec_t *)FTL_MALLOC(sizeof(dir_vec_t));

    if (PTRVALID(vecdir))
        dir_vec_init(vecdir);

    return &vecdir->dir;
}





#if 0 != DEBUG_VALINIT(1+)0
extern dir_t *
dir_info(dir_t *dir, int lineno)
{   dir_value(dir)->lineno = lineno;
    return dir;
}


#define dir_vec_new() dir_info((dir_vec_new)(),__LINE__)
#endif




typedef int compareval_fn_t(const value_t **, const value_t **);


static void
dir_vec_sort(dir_vec_t *vec, compareval_fn_t *compare)
{   const value_t **bindings = vec->bindvec;
    size_t items = vec->n;
    qsort((void *)bindings, items, sizeof(bindings[0]),
          (int (*)(const void *, const void *))(compare));
}








/*****************************************************************************
 *                                                                           *
 *          Dynamic Directories                                              *
 *          ===================                                              *
 *                                                                           *
 *****************************************************************************/





/* Dynamic directories are ones that use FTL functions to implement the main
 * functions for getting, setting and enumerating their content
 */




typedef struct
{   dir_t dir;
    parser_state_t *state; /* state for function execution */
    const value_t *get;    /* [name] - read value */
    const value_t *set;    /* [name, value] - write value */
    const value_t *getall; /* [[value, name]:{}] - enumerate names and values */
    const value_t *count;  /* [] - return number of names */
} dir_dyn_t;

/* Warning: this is not a good way to provide the state for the functions
 * there is no real guarantee that it will still be valid at the time of
 * execution.
 * Unfortunately there is no real alternative - we can't easily change all
 * the code to provide a state on every environment access.
 */




static value_type_t type_dir_dyn_val;



#define dir_dyn_dir(dirid) (&((dirid)->dir))
#define dir_dyn_value(dirid) dir_value(dir_dyn_dir(dirid))






static void dir_dyn_markver(const value_t *value, int heap_version)
{   dir_dyn_t *dyndir = (dir_dyn_t *)value;

    if (dyndir->get != &value_null)
        value_mark_version((value_t *)/*unconst*/dyndir->get, heap_version);
    if (dyndir->set != &value_null)
        value_mark_version((value_t *)/*unconst*/dyndir->set, heap_version);
    if (dyndir->getall != &value_null)
        value_mark_version((value_t *)/*unconst*/dyndir->getall, heap_version);
    if (dyndir->count != &value_null)
        value_mark_version((value_t *)/*unconst*/dyndir->count, heap_version);
}





static bool
dir_dyn_add(dir_t *dir, const value_t *name, const value_t *value)
{   dir_dyn_t *dyndir = (dir_dyn_t *)dir;
    bool ok = FALSE;

    if (PTRVALID(dyndir->set) && dyndir->set != &value_null)
    {
        const value_t *code = substitute(dyndir->set, name,
                                         dyndir->state, /*unstrict*/FALSE);
        if (PTRVALID(code)) {
            code = substitute(code, value, dyndir->state, /*unstrict*/FALSE);
            if (PTRVALID(code)) {
                const value_t *good = invoke(code, dyndir->state);
                ok = PTRVALID(good) && value_istype_bool(good) &&
                     good != value_false;
            }
        }
    }
    return ok;
}




static unsigned
dir_dyn_count(dir_t *dir)
{   dir_dyn_t *dyndir = (dir_dyn_t *)dir;
    unsigned n = 0;

    if (PTRVALID(dyndir->count) && dyndir->count != &value_null)
    {
        const value_t *countval = invoke(dyndir->count, dyndir->state);
        if (value_istype(countval, type_int)) {
            n = (unsigned)value_int_number(countval);
        }
    }
    return n;
}



static void *
dir_dyn_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_dyn_t *dyndir = (dir_dyn_t *)dir;
    const value_t *dirval = NULL;
    const value_t *val = NULL;

    if (PTRVALID(dyndir->getall) && dyndir->getall != &value_null)
        dirval = invoke(dyndir->getall, dyndir->state);

    if (PTRVALID(dirval) && dirval != &value_null &&
        value_istype(dirval, type_dir))
    {
        val = dir_forall((dir_t *)dirval, enumfn, arg);
    }
    return (void *)val;
}




static const value_t *
dir_dyn_get(dir_t *dir, const value_t *name)
{   dir_dyn_t *dyndir = (dir_dyn_t *)dir;
    const value_t *val = NULL;

    if (PTRVALID(dyndir->get) && dyndir->get != &value_null)
    {
        const value_t *code = substitute(dyndir->get, name,
                                         dyndir->state, /*unstrict*/FALSE);
        if (PTRVALID(code))
            val = invoke(code, dyndir->state);
    }
    return val;
}



static const value_t *
dir_dyn_get_using_getall(dir_t *dir, const value_t *name)
{   dir_dyn_t *dyndir = (dir_dyn_t *)dir;
    const value_t *dirval = NULL;
    const value_t *val = NULL;

    if (PTRVALID(dyndir->getall) && dyndir->getall != &value_null)
        dirval = invoke(dyndir->getall, dyndir->state);

    if (PTRVALID(dirval) && dirval != &value_null &&
        value_istype(dirval, type_dir))
    {
        val = dir_get((dir_t *)dirval, name);
    }
    return (void *)val;
}



/*! create a new dynamic environment
 *    @param dyndir    - dyn directory value to be initialized
 *    @param state     - parser state to use for function execution [!!]
 *    @param get_fn    - NULL or closure with 1 arg to retrieve name
 *    @param set_fn    - NULL or closure with 2 args to set name to value
 *    @param getall_fn - closure with 1 args to enumerate names & values
 *    @param count_fn  - NULL or code or closure with 0 args to count entries
 */
static void
dir_dyn_init(dir_dyn_t *dyndir, parser_state_t *state,
             const value_t *get_fn, const value_t *set_fn,
             const value_t *getall_fn, const value_t *count_fn)
{
    dir_init(&dyndir->dir, &type_dir_dyn_val,
             (set_fn == NULL || set_fn == &value_null)? NULL: &dir_dyn_add,
             /*lookup*/NULL,
             (get_fn == NULL || get_fn == &value_null)?
                 &dir_dyn_get_using_getall: &dir_dyn_get,
             &dir_dyn_forall, /*on_heap*/FALSE);

    if (PTRVALID(count_fn) && count_fn != &value_null)
        dyndir->dir.count = &dir_dyn_count;
    /* else will be derived from getall */

    dyndir->state = state;
    dyndir->set = set_fn;
    dyndir->get = get_fn;
    dyndir->count = count_fn;
    dyndir->getall = getall_fn;
}






extern dir_t *
dir_dyn_new(parser_state_t *state, const char *errprefix,
            const value_t *get_fn, const value_t *set_fn,
            const value_t *getall_fn, const value_t *count_fn)
{
    bool ok = TRUE;
    if (PTRVALID(get_fn) && get_fn != &value_null && (
            !value_istype(get_fn, type_closure) ||
            value_closure_argcount(get_fn) != 1)) {
        ok = FALSE;
        parser_error(state,
                     "%s'get' is not a closure with one unbound variable\n",
                     errprefix);
    }
    if (PTRVALID(set_fn) && set_fn != &value_null && (
            !value_istype(set_fn, type_closure) ||
            value_closure_argcount(set_fn) != 2)) {
        ok = FALSE;
        parser_error(state,
                     "%s'set' is not a closure with two unbound variables\n",
                     errprefix);
    }
    if (PTRVALID(count_fn) && count_fn != &value_null && (
            !value_istype(count_fn, type_closure) ||
            value_closure_argcount(count_fn) != 0)) {
        ok = FALSE;
        parser_error(state,
                     "%s'count' is not a closure with no unbound variables\n",
                     errprefix);
    }
    if (PTRVALID(getall_fn) && getall_fn != &value_null && (
            !value_istype(getall_fn, type_closure) ||
            value_closure_argcount(getall_fn) != 0)) {
        ok = FALSE;
        parser_error(state,
                     "%s'getall' is not a closure with no unbound variables\n",
                     errprefix);
    }
    if (ok) {
        dir_dyn_t *dyndir = (dir_dyn_t *)FTL_MALLOC(sizeof(dir_dyn_t));

        if (PTRVALID(dyndir))
            dir_dyn_init(dyndir, state, get_fn, set_fn, getall_fn, count_fn);

        return &dyndir->dir;
    } else
        return NULL;
}








/*****************************************************************************
 *                                                                           *
 *          Composed Directories                                             *
 *          ====================                                             *
 *                                                                           *
 *****************************************************************************/






/* Composed directories are ones that combine two other directories: and index
 * and a value directory.
 * A valiue looked up in the composed directory is first looked up in the index
 * and the resulting value is then used as the index into the value directory.
 */





typedef struct
{   dir_t dir;
    dir_t *ixdir;
    dir_t *valdir;
} dir_join_t;



#define dir_join_dir(dirid) (&((dirid)->dir))
#define dir_join_value(dirid) dir_value(dir_join_dir(dirid))



static value_type_t type_dir_join_val;




static void dir_join_delete(value_t *value)
{   if (value_istype(value, type_dir))
    {   dir_join_t *joindir = (dir_join_t *)value;
        joindir->ixdir = NULL; /* should be garbage collected */
        joindir->valdir = NULL; /* should be garbage collected */
        FTL_FREE(value);
    }
    /* else type error */
}




static void dir_join_markver(const value_t *value, int heap_version)
{   dir_join_t *joindir = (dir_join_t *)value;

    value_mark_version(dir_value(joindir->ixdir), heap_version);
    value_mark_version(dir_value(joindir->valdir), heap_version);
}





static bool
dir_join_add(dir_t *dir, const value_t *name, const value_t *value)
{   dir_join_t *joindir = (dir_join_t *)dir;
    const value_t *index = dir_get(joindir->ixdir, name);
    if (PTRVALID(index))
        return dir_set(joindir->valdir, index, value);
    else
        return FALSE;
}





static const value_t **
dir_join_lookup(dir_t *dir, const value_t *nameval)
{   dir_join_t *joindir = (dir_join_t *)dir;
    const value_t *index = dir_get(joindir->ixdir, nameval);
    const value_t **refval = NULL;

    if (PTRVALID(index))
    {
        dir_t *lookupdir = joindir->valdir;
        if (PTRVALID(lookupdir) &&
            PTRVALID(lookupdir->lookup))
            refval = (*lookupdir->lookup)(lookupdir, index);
    }

    return refval;
}




typedef struct {
    dir_join_t *joindir;
    dir_enum_fn_t *enumfn;
    void *enum_arg;
} dir_join_enum_arg_t;




static void *
dir_join_enum(dir_t *dir, const value_t *name, const value_t *index,
                 void *index_arg)
{
    dir_join_enum_arg_t *arg = (dir_join_enum_arg_t *)index_arg;
    dir_join_t *joindir = arg->joindir;

    if (PTRVALID(index))
    {
        const value_t *value = dir_get(joindir->valdir, index);
        return (*arg->enumfn)(dir_join_dir(joindir), name,
                              value, arg->enum_arg);
    }
    else
        return (*arg->enumfn)(dir_join_dir(joindir), name, NULL, arg->enum_arg);
}





static void *
dir_join_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *enum_arg)
{   dir_join_t *joindir = (dir_join_t *)dir;
    dir_join_enum_arg_t arg;

    arg.joindir  = joindir;
    arg.enumfn   = enumfn;
    arg.enum_arg = enum_arg;

    return dir_forall(joindir->ixdir, &dir_join_enum, (void *)&arg);
}





static dir_t *
dir_join_init(dir_join_t *joindir, dir_t *ixdir, dir_t *valdir)
{   dir_init(&joindir->dir, &type_dir_join_val, &dir_join_add, &dir_join_lookup,
             /*get*/ NULL, &dir_join_forall, /*on_heap*/TRUE);
    joindir->ixdir = ixdir;
    joindir->valdir = valdir;
    return &joindir->dir;
}






extern dir_t *
dir_join_new(dir_t *ixdir, dir_t *valdir)
{   dir_t *dir = NULL;

    if (PTRVALID(ixdir) &&
        PTRVALID(valdir))
    {
        dir_join_t *joindir = (dir_join_t *)
                                 FTL_MALLOC(sizeof(dir_join_t));

        if (PTRVALID(joindir))
            dir = dir_join_init(joindir, ixdir, valdir);
    }

    return dir;
}








/*****************************************************************************
 *                                                                           *
 *          Builtin Argument Name String Values                              *
 *          ===================================                              *
 *                                                                           *
 *****************************************************************************/






#if 0

/* This won't work because values are queued using their "next" field */


typedef char builtin_argname_str_t[16];
static builtin_argname_str_t builtin_argname[FTL_ARGNAMES_CACHED+1];
static value_string_t value_string_argname_val[FTL_ARGNAMES_CACHED+1];







static void
values_string_argname_init(void)
{   int i;

    for (i=0; i<FTL_ARGNAMES_CACHED+1; i++)
    {   char *argname = &(builtin_argname[i])[0];
        sprintf(argname, "%s%d", BUILTIN_ARG, i);
        OMIT(fprintf(stderr,"%s: arg '%s'[%d]\n", codeid(),
                       argname, strlen(argname)););
        (void)value_cstring_init(&value_string_argname_val[i],
                                 argname, strlen(argname),
                                 /*on_heap*/FALSE);
    }
}
#else

#define values_string_argname_init()

#endif







STATIC_INLINE value_t *
value_string_argname(unsigned argno)
{
#if 0
    if (argno <= FTL_ARGNAMES_CACHED)
        return value_string_value(&value_string_argname_val[argno]);
    else
#endif
    {   char argname[16];
        sprintf(&argname[0], "%s%d", BUILTIN_ARG, argno);
        return value_string_new_measured(argname);
    }
}







#if 0
static bool
dir_set_builtin_arg(dir_t *dir, int argno, const value_t *value)
{   char argname[16];
    (void)sprintf(&argname[0], "%s%d", BUILTIN_ARG, argno);
    return dir_string_set(dir, &argname[0], value);
}
#endif






static const value_t *
dir_get_builtin_arg(dir_t *dir, int argno)
{   char argname[16];
    const value_t *res;
    (void)sprintf(&argname[0], "%s%d", BUILTIN_ARG, argno);
    res = dir_string_get(dir, &argname[0]);
    OMIT(
        if (NULL == res)
        {   fprintf(stderr, "can't find %s in: ", &argname[0]);
            value_fprint(stderr, /*root*/NULL, dir_value(dir));
            fprintf(stderr, "\n");
        }
    )
    return res;
}












/*****************************************************************************
 *                                                                           *
 *          Structure Specifications                                         *
 *          ========================                                         *
 *                                                                           *
 *****************************************************************************/






struct field_s {
    field_kind_t field_type;
    field_set_fn_t *set;
    field_get_fn_t *get;
} /* field_t */;





extern void
field_noset(void *mem, const value_t *val)
{   fprintf(stderr, "%s: can't set a value for this field\n", codeid());
}




extern void
field_init(field_t *ref_field, field_kind_t kind,
           field_set_fn_t *set, field_get_fn_t *get)
{   ref_field->field_type = kind;
    ref_field->get = get;
    if (NULL == set)
        ref_field->set = &field_noset;
    else
        ref_field->set = set;
}





struct struct_field_s
{   struct struct_field_s *link;
    value_string_t strval;
    const char *name;
    field_t field;
} /* struct_field_t */;






extern void
struct_spec_init(struct_spec_t *spec)
{   spec->fields = NULL;
    spec->end_fields = &spec->fields;
}





extern void
struct_spec_end(struct_spec_t *spec)
{   struct_field_t *field = spec->fields;
    while (PTRVALID(field))
    {   struct_field_t *doomed = field;
        field = field->link;
        FTL_FREE(doomed);
    }
    struct_spec_init(spec);
}







static void
struct_spec_field_noset(void *mem, const value_t *val)
{   fprintf(stderr, "%s: write of %s OMITd - read only field\n",
                codeid(), type_name(value_type(val)));
}






extern void
struct_spec_add_field(struct_spec_t *spec, field_kind_t kind,
                      const char *name,
                      field_set_fn_t *set, field_get_fn_t *get)
{   struct_field_t *newfield = (struct_field_t *)
                               FTL_MALLOC(sizeof(struct_field_t));
    if (PTRVALID(newfield))
    {   value_cstring_init(&newfield->strval, name, strlen(name),
                           /*on_heap*/FALSE);
        newfield->name = name;
        if (NULL == set)
            set = &struct_spec_field_noset;
        field_init(&newfield->field, kind, set, get);
        newfield->link = NULL;

        *spec->end_fields = newfield;
        spec->end_fields = &newfield->link;
        OMIT(printf("%s: add spec field \"%s\" - get %p set %p\n",
                      codeid(), newfield->name,
                      newfield->field.get, newfield->field.set);)
    }
}






static struct_field_t *
struct_spec_find_field(struct_spec_t *spec, const char *name, size_t namelen)
{   struct_field_t *field = spec->fields;
    const char *fieldname;

    while (PTRVALID(field) &&
           (strlen(fieldname = field->name) != namelen ||
            0 != memcmp(fieldname, name, namelen)))
        field = field->link;

    return field;
}







typedef void *struct_spec_enum_fn_t(const struct_field_t *field, void *arg);

static void *
struct_spec_forall(struct_spec_t *spec, struct_spec_enum_fn_t fn, void *arg)
{   struct_field_t *field = spec->fields;
    void *result = NULL;

    while (PTRVALID(field) && NULL == result)
    {   result = (*fn)(field, arg);
        OMIT(printf("%s: spec field \"%s\" result %p\n",
                      codeid(), field->name, result);)
        field = field->link;
    }

    return result;
}












/*****************************************************************************
 *                                                                           *
 *          Structure Directories                                            *
 *          =====================                                            *
 *                                                                           *
 *****************************************************************************/







typedef struct
{   dir_id_t get_cache;
    struct_spec_t spec;
    void *structmem;
    const value_t *ref;
} dir_struct_t;




static value_type_t type_dir_struct_val;





#define dir_struct_dir(dirstruct) dir_id_dir(&((dirstruct)->get_cache))
#define dir_struct_value(dirstruct) dir_value(dir_struct_dir(dirstruct))







static void
dir_struct_delete(value_t *value)
{   dir_struct_t *structdir = (dir_struct_t *)value;
    if (PTRVALID(structdir->structmem))
    {   free(structdir->structmem);
        structdir->structmem = NULL;
    }
    structdir->ref = (const value_t *)NULL;
    dir_id_delete(dir_id_value(&structdir->get_cache));
}




static void
dir_struct_markver(const value_t *value, int heap_version)
{   dir_struct_t *structdir = (dir_struct_t *)value;
    if (PTRVALID(structdir->ref)) {
        OMIT(printf("%s: marking struct dir %p ref %p\n",
                      codeid(), structdir, structdir->ref);)
        value_mark_version((value_t *)/*unconst*/structdir->ref, heap_version);
    }
    dir_id_markver(dir_id_value(&structdir->get_cache), heap_version);
}





static bool
dir_struct_add(dir_t *dir, const value_t *nameval, const value_t *value)
{   dir_struct_t *structdir = (dir_struct_t *)dir;
    bool ok = FALSE;
    const char *name;
    size_t namelen;

    if (value_string_get(nameval, &name, &namelen))
    {   struct_field_t *field = struct_spec_find_field(&structdir->spec,
                                                       name, namelen);
        if (PTRVALID(field))
        {   (*field->field.set)(structdir->structmem, value);
            ok = TRUE;
        }
    }
    return ok;
}







static const value_t *
dir_struct_get(dir_t *dir, const value_t *nameval)
{   dir_struct_t *structdir = (dir_struct_t *)dir;
    const value_t *found = NULL;
    const char *name;
    size_t namelen;

    if (value_string_get(nameval, &name, &namelen))
    {   struct_field_t *field = struct_spec_find_field(&structdir->spec,
                                                       name, namelen);
        if (PTRVALID(field))
        {   /* get any value_t we've been saving to put return value into */
            dir_t *get_cache = dir_id_dir(&structdir->get_cache);
            const value_t **ref_value = dir_id_lookup(get_cache, nameval);

            if (NULL == ref_value)
            {   if (dir_id_add(get_cache, nameval, &value_null))
                    ref_value = dir_id_lookup(get_cache, nameval);
                if (NULL != ref_value)
                {   dir_t *subdir;
                    (*field->field.get)(structdir->structmem, ref_value);
                    found = *ref_value;
                    if (dir_islocked(dir) && value_to_dir(found, &subdir))
                        (void)dir_lock(subdir, NULL);
                }
            } else
            {   (*field->field.get)(structdir->structmem, ref_value);
                found = *ref_value;
            }
        }
    }
    return found;
}






typedef struct {
    dir_t *dir;
    dir_enum_fn_t *enumfn;
    void *arg;
    dir_id_t *get_cache;
    void *structmem;
} dir_struct_enum_args_t;





static void *
dir_spec_enum_fn(const struct_field_t *field, void *arg)
{   dir_struct_enum_args_t *args = (dir_struct_enum_args_t *)arg;
    const value_t *nameval = value_string_value(&field->strval);
    dir_t *get_cache = dir_id_dir(args->get_cache);
    const value_t **ref_value = dir_id_lookup(get_cache, nameval);
    void *result = NULL;

    if (NULL == ref_value)
    {   dir_id_add(get_cache, nameval, &value_null);
        ref_value = dir_id_lookup(get_cache, nameval);
    }
    OMIT(printf("%s: get field \"%s\" in struct %p val at %p\n",
                  codeid(), field->name, args->structmem, ref_value);)
    if (NULL != ref_value)
    {   OMIT(printf("%s: get field \"%s\" call get %p\n",
                      codeid(), field->name, field->get);)
        (*field->field.get)(args->structmem, ref_value);
        OMIT(printf("%s: got field \"%s\" of %p new val is %p\n",
                      codeid(), field->name, args->structmem, *ref_value);)
        result = (*args->enumfn)(args->dir, nameval, *ref_value, args->arg);
    }
    return result;
}






static void *
dir_struct_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_struct_t *structdir = (dir_struct_t *)dir;
    dir_struct_enum_args_t args;

    args.dir = dir;
    args.enumfn = enumfn;
    args.arg = arg;
    args.get_cache = &structdir->get_cache;
    args.structmem = structdir->structmem;

    return struct_spec_forall(&structdir->spec, &dir_spec_enum_fn,
                              (void *)&args);
}







static void
dir_struct_init(dir_struct_t *structdir, struct_spec_t *spec,
                bool is_const, void *structmem, const value_t *ref,
                bool on_heap)
{   dir_t *dir = dir_struct_dir(structdir);
    dir_id_init(&structdir->get_cache, &type_dir_struct_val, on_heap);
    /* but we're going to overwrite some */
    memcpy(&structdir->spec, spec, sizeof(structdir->spec));
    structdir->structmem = structmem;
    structdir->ref = ref;
    dir->get = &dir_struct_get;
    if (is_const)
        dir->add = NULL;
    else
        dir->add = &dir_struct_add;
    dir->lookup = NULL; /* otherwise we'll use this to do adds */
    dir->forall = &dir_struct_forall;
}






extern dir_t *
dir_cstruct_new(struct_spec_t *spec, bool is_const, void *static_struct)
{   dir_struct_t *structdir = (dir_struct_t *)FTL_MALLOC(sizeof(dir_struct_t));

    if (NULL != structdir)
        dir_struct_init(structdir, spec, is_const, static_struct, /*ref*/NULL,
                        /*on_heap*/FALSE);

    return dir_struct_dir(structdir);
}







extern dir_t *
dir_struct_new(struct_spec_t *spec, bool is_const, void *malloc_struct)
{   dir_struct_t *structdir = (dir_struct_t *)FTL_MALLOC(sizeof(dir_struct_t));

    if (NULL != structdir)
        dir_struct_init(structdir, spec, is_const, malloc_struct, /*ref*/NULL,
                        /*on_heap*/TRUE);

    return dir_struct_dir(structdir);
}







extern dir_t *
dir_struct_cast(struct_spec_t *spec, bool is_const,
                const value_t *ref, void *ref_struct)
{   dir_struct_t *structdir = (dir_struct_t *)FTL_MALLOC(sizeof(dir_struct_t));

    if (NULL != structdir)
        dir_struct_init(structdir, spec, is_const, ref_struct, ref,
                        /*on_heap*/FALSE);

    return dir_struct_dir(structdir);
}







extern void
dir_struct_update(const value_t **ref_value,
                  struct_spec_t *spec, bool is_const, void *structmem)
{   const value_t *val = *ref_value;
    if (NULL == val || !value_type_equal(val, type_dir))
        /* TODO: this should really use dir_cstruct_clone on the parent ref */
        *ref_value = dir_value(dir_cstruct_new(spec, is_const, structmem));
}











/*****************************************************************************
 *                                                                           *
 *          Data-structure Parsing                                           *
 *          ======================                                           *
 *                                                                           *
 *****************************************************************************/




#define DEBUG_DTINT OMIT




/* This is a generic memory reader, that will instantiated later */

typedef struct memreader_s memreader_t;

/* Function to retrieve bytes from an memreader
 *
 *   @param memrd   memory reader to use
 *   @param ix      index in memory to read from
 *   @param buff    location where read data is placed
 *   @param len     number of bytes to read
 *
 *   @return TRUE iff successful
 */
typedef bool
binget_fn_t(memreader_t *memrd, number_t ix, void *buff, size_t len);

struct memreader_s {
    binget_fn_t *binget;
} /* memreader_t */;




/* This is a generic data type reader, that will instantiated later */

typedef struct datatype_s datatype_t;

/*! Function to parse a data type that prefixes the area of data from *binpos
 *  to binend.
 *
 *   @param rdr          Object allowing (read) access to part of memory
 *   @param ref_binpos   Location of Index into memory where parsing occurs
 *   @param binend       Index after last byte of memory to be parsed
 *   @param dtype        Details of the datatype being parsed
 *
 *   @returns  If unsuccessful the function returns NULL otherwise
 *             *ref_binpos is updated to follow the parsed prefix.
 *
 *  Note that it is valid to pass in a NULL 'rdr'.  In this case the
 *  data structure's size should be added to ref_binpos and the function
 *  should return &value_null.
 */
typedef const value_t *
binparse_fn_t(memreader_t *rdr, unumber_t *ref_binpos, unumber_t binend,
              datatype_t *dtype);

struct datatype_s {
    binparse_fn_t *binparse;
} /* datatype_t */;




/* This is an in-store memory reader */

typedef struct {
    memreader_t memrd;  /* must occur first in the structure */
    const char *octets; /* pointer to data area in memory */
} memreader_instore_t;



static unumber_t
datatype_sizeof(datatype_t *dt)
{   unumber_t size = 0;
    if (PTRVALID(dt) &&
        PTRVALID(dt->binparse))
    {
        unumber_t ptr = 0;
        (void)dt->binparse(NULL, &ptr, (unumber_t)-1, dt);
        size = ptr;
    }
    return size;
}


static bool
instore_binget(memreader_t *rdr, number_t ix, void *buff, size_t len)
{
    memreader_instore_t *rdr_instore = (memreader_instore_t *)rdr;
    const char *ptr = &(rdr_instore->octets)[ix];
    memcpy(buff, ptr, len);
    return TRUE;
}


static memreader_t *
memreader_instore_init(memreader_instore_t *ref_rdr, const char *octets)
{
    ref_rdr->memrd.binget = &instore_binget;
    ref_rdr->octets = octets;
    return &ref_rdr->memrd;
}



/* Integer datatype parsing */


typedef struct {
    datatype_t dtype;
    unsigned bytes;
    bool little_endian;
    bool is_signed;
} datatype_int_t;




/* Parse a single octet */
static const value_t *
binparse_bits8(memreader_t *rdr, unumber_t *ref_binpos, unumber_t binend,
               datatype_t *dtype)
{   const value_t *val = NULL;
    unumber_t newpos = *ref_binpos + 1;
    if (newpos <= binend) {
        if (PTRVALID(rdr)) {
            ftl_u8_t data;
            if ((*rdr->binget)(rdr, *ref_binpos, &data, sizeof(data))) {
                datatype_int_t *inttype = (datatype_int_t *)dtype;
                ftl_u8_t n = data;
                OMIT(DPRINTF("%s: %ssigned %db %s\n",
                                    codeid(), inttype->is_signed? "":" un",
                                    8, ""););
                if (inttype->is_signed)
                    val = value_int_new((ftl_s8_t)n);
                else
                    val = value_uint_new(n);
            }
        } else
            val = &value_null;
        *ref_binpos = newpos;
    }
    return val;
}




/* Parse a double octet to a little endian integer */
static const value_t *
binparse_bits16l(memreader_t *rdr, unumber_t *ref_binpos, unumber_t binend,
                 datatype_t *dtype)
{   const value_t *val = NULL;
    unumber_t newpos = *ref_binpos + 2;
    if (newpos <= binend) {
        if (PTRVALID(rdr)) {
            ftl_u8_t data[2];
            if ((*rdr->binget)(rdr, *ref_binpos, &data[0], sizeof(data))) {
                datatype_int_t *inttype = (datatype_int_t *)dtype;
                ftl_u16_t n = (data[1] << 8) | data[0];
                OMIT(DPRINTF("%s: %ssigned %db %s\n",
                                    codeid(), inttype->is_signed? "":" un",
                                    16, "le"););
                if (inttype->is_signed)
                    val = value_int_new((ftl_s16_t)n);
                else
                    val = value_uint_new(n);
            }
        } else
            val = &value_null;
        *ref_binpos = newpos;
    }
    return val;
}




/* Parse a double octet to a big endian integer */
static const value_t *
binparse_bits16b(memreader_t *rdr, unumber_t *ref_binpos, unumber_t binend,
                 datatype_t *dtype)
{   const value_t *val = NULL;
    unumber_t newpos = *ref_binpos + 2;
    if (newpos <= binend) {
        if (PTRVALID(rdr)) {
            ftl_u8_t data[2];
            if ((*rdr->binget)(rdr, *ref_binpos, &data[0], sizeof(data))) {
                ftl_u16_t n = (data[0] << 8) | data[1];
                datatype_int_t *inttype = (datatype_int_t *)dtype;
                OMIT(DPRINTF("%s: %ssigned %db %s\n",
                                    codeid(), inttype->is_signed? "":" un",
                                    16, "be"););
                if (inttype->is_signed)
                    val = value_int_new((ftl_s16_t)n);
                else
                    val = value_uint_new(n);
            }
        } else
            val = &value_null;
        *ref_binpos = newpos;
    }
    return val;
}





/* Parse a quad octet to a little endian integer */
static const value_t *
binparse_bits32l(memreader_t *rdr, unumber_t *ref_binpos, unumber_t binend,
                 datatype_t *dtype)
{   const value_t *val = NULL;
    unumber_t newpos = *ref_binpos + 4;
    if (newpos <= binend) {
        if (PTRVALID(rdr)) {
            ftl_u8_t data[4];
            if ((*rdr->binget)(rdr, *ref_binpos, &data[0], sizeof(data))) {
                ftl_u32_t n  = (data[3] << 24) | (data[2] << 16) |
                               (data[1] << 8) | data[0];
                datatype_int_t *inttype = (datatype_int_t *)dtype;
                OMIT(DPRINTF("%s: %ssigned %db %s\n",
                                    codeid(), inttype->is_signed? "":" un",
                                    32, "le"););
                if (inttype->is_signed)
                    val = value_int_new((ftl_s32_t)n);
                else
                    val = value_uint_new(n);
            }
        } else
            val = &value_null;
        *ref_binpos = newpos;
    }
    return val;
}





/* Parse a quad octet to a big endian integer */
static const value_t *
binparse_bits32b(memreader_t *rdr, unumber_t *ref_binpos, unumber_t binend,
                 datatype_t *dtype)
{   const value_t *val = NULL;
    unumber_t newpos = *ref_binpos + 4;
    if (newpos <= binend) {
        if (PTRVALID(rdr)) {
            ftl_u8_t data[4];
            if ((*rdr->binget)(rdr, *ref_binpos, &data[0], sizeof(data))) {
                ftl_u32_t n = (data[0] << 24) | (data[1] << 16) |
                              (data[2] << 8) | data[3];
                datatype_int_t *inttype = (datatype_int_t *)dtype;
                OMIT(DPRINTF("%s: %ssigned %db %s\n",
                                    codeid(), inttype->is_signed? "":" un",
                                    32, "be"););
                if (inttype->is_signed)
                    val = value_int_new((ftl_s32_t)n);
                else
                    val = value_uint_new(n);
            }
        } else
            val = &value_null;
        *ref_binpos = newpos;
    }
    return val;
}





static datatype_t *
datatype_int_init(datatype_int_t *ref_dt, bool is_signed, unsigned bytes,
                  bool little_endian)
{
    bool ok = TRUE;

    ref_dt->bytes = bytes;
    ref_dt->little_endian = little_endian;
    ref_dt->is_signed = is_signed;

    DEBUG_DTINT(DPRINTF("%s: new %ssigned %db %s int\n",
                        codeid(), is_signed? "": "un",
                        bytes*8, little_endian?"le": "be"););
    switch (bytes)
    {
        case 1:
            ref_dt->dtype.binparse = &binparse_bits8;
            break;
        case 2:
            if (little_endian)
                ref_dt->dtype.binparse = &binparse_bits16l;
            else
                ref_dt->dtype.binparse = &binparse_bits16b;
            break;
        case 4:
            if (little_endian)
                ref_dt->dtype.binparse = &binparse_bits32l;
            else
                ref_dt->dtype.binparse = &binparse_bits32b;
            break;
        default:
            ok = FALSE;
            break;
    }

    return ok? &ref_dt->dtype: NULL;
}



static datatype_t *
datatype_int_new(bool is_signed, unsigned bytes, bool little_endian)
{
    datatype_int_t *intdt = (datatype_int_t *)
                            FTL_MALLOC(sizeof(datatype_int_t));
    datatype_t *dt = NULL;

    if (PTRVALID(intdt))
        dt = datatype_int_init(intdt, is_signed, bytes, little_endian);

    return dt;
}



/* Sequence of <datatype> datatype parsing */


typedef struct {
    datatype_t dtype;
    datatype_t *of_dtype;
    size_t entries;
} datatype_seq_t;




/* Parse a sequence of the same datatype */
static const value_t *
binparse_seq(memreader_t *rdr, unumber_t *ref_binpos, unumber_t binend,
             datatype_t *dtype)
{
    const value_t *val = NULL;
    datatype_seq_t *seqdt = (datatype_seq_t *)dtype;
    datatype_t *entry_dt = seqdt->of_dtype;
    size_t n = seqdt->entries;
    size_t entryno = 0;
    bool ok = TRUE;
    dir_t *vec = NULL;

    while (ok && entryno < n) {
        const value_t *entry =
            (*entry_dt->binparse)(rdr, ref_binpos, binend, entry_dt);
        if (entry == NULL)
            ok = FALSE;
        else {
            if (vec == NULL)
                vec = dir_vec_new();
            if (PTRVALID(vec))
                dir_int_set(vec, (int)entryno, entry);
            entryno++;
        }
    }

    if (PTRVALID(vec))
        val = dir_value(vec);

    /* if (!ok && PTRVALID(val))
        value_delete(&val); - we have no delete-from-heap function */

    return ok? (val == NULL? &value_null: val): NULL;
}



extern datatype_t *
datatype_seq_init(datatype_seq_t *ref_dt, datatype_t *of_dtype, size_t n)
{
    if (of_dtype == NULL)
        return NULL;
    else
    {
        ref_dt->of_dtype = of_dtype;
        ref_dt->entries = n;
        ref_dt->dtype.binparse = &binparse_seq;

        return &ref_dt->dtype;
    }
}




/*****************************************************************************
 *                                                                           *
 *          Dynamic vector of datatype Directories                           *
 *          ======================================                           *
 *                                                                           *
 *****************************************************************************/







typedef struct
{   dir_t dir;
    datatype_t *entry_dt; /* data type contained in sequence (to free) */
    memreader_t *rdr;     /* means to read a value at a given index (to free) */
    unumber_t first;      /* first valid index */
    unumber_t last;       /* first index after last valid index */
    unumber_t stride;     /* number of indeces consumed by entry_dt */
} dir_dynseq_t;




static value_type_t type_dir_dynseq_val;




static void dir_dynseq_delete(value_t *value)
{   if (value_istype(value, type_dir))
    {   dir_dynseq_t *dynseq = (dir_dynseq_t *)value;
        if (PTRVALID(dynseq->entry_dt))
        {   FTL_FREE(dynseq->entry_dt);
            dynseq->entry_dt = NULL;
        }
        if (PTRVALID(dynseq->rdr))
        {   FTL_FREE(dynseq->rdr);
            dynseq->rdr = NULL;
        }
        FTL_FREE(value);
    }
    /* else type error */
}



static const value_t *
dir_dynseq_get(dir_t *dir, const value_t *name)
{   dir_dynseq_t *dynseq = (dir_dynseq_t *)dir;
    const value_t *found = NULL;

    if (PTRVALID(dynseq) && value_type_equal(name, type_int))
    {   unumber_t n = value_int_number(name);
        unumber_t index = dynseq->first + n * dynseq->stride;

        if (index >= dynseq->first && index < dynseq->last)
        {
            binparse_fn_t *parse = dynseq->entry_dt->binparse;

            found = (*parse)(dynseq->rdr, &index, dynseq->last,
                             dynseq->entry_dt);
        }
    }

    return found;
}





#if 0
static const value_t **
dir_dynseq_lookup(dir_t *dir, const value_t *nameval)
{   if (value_type_equal(nameval, type_int))
    {   dir_dynseq_t *dynseq = (dir_dynseq_t *)dir;
        const value_t *found = NULL;

        if (PTRVALID(dynseq) && value_type_equal(name, type_int))
        {   unumber_t n = value_int_number(name);
            unumber_t index = dynseq->first + n * dynseq->stride;

            if (index >= dynseq->first && index < dynseq->last)
            {
                binparse_fn_t *parse = dynseq->entry_dt->binparse;

                found = (*parse)(dynseq->rdr, &index, dynseq->last,
                                 dynseq->entry_dt);
            }
        }

        return NULL == bind? (const value_t **)NULL: &found->value;
    } else
        return NULL;
}
#endif






static void *
dir_dynseq_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_dynseq_t *dynseq = (dir_dynseq_t *)dir;
    void *result = NULL;
    binparse_fn_t *parse = dynseq->entry_dt->binparse;
    unumber_t index = dynseq->first;
    number_t i = 0;
    bool ok = TRUE;

    while (ok && index < dynseq->last && NULL == result)
    {
        /*unumber_t i = index;*/
        const value_t *rdval = (*parse)(dynseq->rdr, &index, dynseq->last,
                                        dynseq->entry_dt);
        /* this updates index */
        ok = PTRVALID(rdval);
        if (ok)
        {
            value_t *ixval = value_int_new(i);
            (*enumfn)(dir, ixval, rdval, arg);
            value_unlocal(ixval);
            i++;
        }
        value_unlocal(rdval);
    }

    return result;
}






extern dir_t *
dir_dynseq_init(dir_dynseq_t *dynseq,
                datatype_t *entry_dt, memreader_t *rdr,
                unumber_t first, unumber_t last)
{   dir_t *dsdir = NULL;

    if (PTRVALID(entry_dt) &&
        PTRVALID(rdr) && last >= first)
    {
        dynseq->entry_dt = entry_dt;
        dynseq->rdr = rdr;
        dynseq->first = first;
        dynseq->last = last;
        dynseq->stride = datatype_sizeof(entry_dt);

        if (dynseq->stride > 0)
            dir_init(&dynseq->dir, &type_dir_dynseq_val,
                     /*add*/NULL, /*lookup*/NULL,
                     &dir_dynseq_get, &dir_dynseq_forall,
                     /*on_heap*/TRUE);

        dsdir = &dynseq->dir;
    }

    return dsdir;
}



/* Both entry_dt and rdr must be allocated with FTL_MALLOC */
extern dir_t *
dir_dynseq_new(datatype_t *entry_dt, memreader_t *rdr,
                       unumber_t first, unumber_t last)
{   dir_dynseq_t *dynseq = (dir_dynseq_t *)FTL_MALLOC(sizeof(dir_dynseq_t));

    if (NULL != dynseq)
        return dir_dynseq_init(dynseq, entry_dt, rdr, first, last);
    else
        return NULL;
}





#if 0  /* doesn't make sense to create a parser? */

/* Parse a sequence of the same datatype */
static const value_t *
binparse_dynseq(memreader_t *rdr, unumber_t *ref_binpos, unumber_t last,
                datatype_t *dtype)
{
    const value_t *val = NULL;
    datatype_seq_t *seqdt = (datatype_seq_t *)dtype;
    datatype_t *entry_dt = seqdt->of_dtype;
    size_t n = seqdt->entries;
    size_t entryno = 0;
    bool ok = TRUE;
    dir_t *vec = dir_seq_new(entry_dt, rdr, *ref_binpos, last);

    while (ok && entryno < n) {
        const value_t *entry =
            (*entry_dt->binparse)(rdr, ref_binpos, last, entry_dt);
        if (entry == NULL)
            ok = FALSE;
        else {
            if (vec == NULL)
                vec = dir_vec_new();
            if (PTRVALID(vec))
                dir_int_set(vec, entryno, entry);
            entryno++;
        }
    }

    if (PTRVALID(vec))
        val = dir_value(vec);

    /* if (!ok && PTRVALID(val))
        value_delete(&val); - we have no delete-from-heap function */

    return ok? (val == NULL? &value_null: val): NULL;
}



static datatype_t *
datatype_dynseq_init(datatype_seq_t *ref_dt, datatype_t *of_dtype, size_t n)
{
    if (of_dtype == NULL)
        return NULL;
    else
    {
        ref_dt->of_dtype = of_dtype;
        ref_dt->entries = n;
        ref_dt->dtype.binparse = &binparse_dynseq;

        return &ref_dt->dtype;
    }
}

#endif




/*****************************************************************************
 *                                                                           *
 *          String Argument vector Directories                               *
 *          ==================================                               *
 *                                                                           *
 *****************************************************************************/








typedef struct
{   dir_t dir;
    size_t argc;
    const char **argv;
} dir_argvec_t;






static value_type_t type_dir_argvec_val;





static const value_t *
dir_argvec_get(dir_t *dir, const value_t *name)
{   dir_argvec_t *vecdir = (dir_argvec_t *)dir;
    const value_t *found = NULL;
    const char **argv = vecdir->argv;

    if (value_istype(name, type_int) && PTRVALID(argv))
    {   size_t index = (size_t)value_int_number(name);
        if ((int)index >= 0 && index < vecdir->argc)
        {   const char *arg = argv[index];
            if (NULL == arg)
                found = &value_null;
            else
                found = value_string_new_measured(arg);
        }
    }

    return found;
}





static void *
dir_argvec_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_argvec_t *vecdir = (dir_argvec_t *)dir;
    const char **argv = vecdir->argv;
    void *result = NULL;

    if (NULL != argv)
    {   size_t i = 0;

        OMIT(printf("argv has %d entries\n", vecdir->argc);)
        while (NULL == result && i < vecdir->argc)
        {   const char *thisarg = argv[i];
            value_t *argval = &value_null;
            value_t *ival = value_int_new(i);

            if (NULL != arg)
            {   argval = value_string_new_measured(thisarg);
                result = (*enumfn)(dir, ival, argval, arg);
                value_unlocal(argval);
            } else
                result = (*enumfn)(dir, ival, argval, arg);

            value_unlocal(ival);
            i++;
        }
    }

    return result;
}






static void
dir_argvec_init(dir_argvec_t *vecdir, int argc, const char **argv)
{   dir_init(&vecdir->dir, &type_dir_argvec_val,
             /*add*/NULL, /*lookup*/NULL, &dir_argvec_get,
             &dir_argvec_forall, /*on_heap*/FALSE);
    vecdir->argc = argc;
    vecdir->argv = argv;
}






extern dir_t *
dir_argvec_new(int argc, const char **argv)
{   dir_argvec_t *vecdir = (dir_argvec_t *)FTL_MALLOC(sizeof(dir_argvec_t));

    if (NULL != vecdir)
        dir_argvec_init(vecdir, argc, argv);

    return &vecdir->dir;
}








/*****************************************************************************
 *                                                                           *
 *          Arrayof Struct Directories                                       *
 *          ==========================                                       *
 *                                                                           *
 *****************************************************************************/








typedef struct dir_array_s dir_array_t;


/* return the base of the memory area and the number of
   stride length elements in it */
typedef bool dir_array_get_fn_t(dir_array_t *array,
                                void **out_mem, size_t *out_len);





struct dir_array_s
{   dir_vec_t get_cache;
    field_t content;
    size_t stride;
    dir_array_get_fn_t *dimfn;
    void *arraymem;
    size_t len;      /* in units of stride bytes */
    const value_t *ref;
} /* dir_array_t */;





static value_type_t type_dir_array_val;




#define dir_array_dir(dirstruct) dir_vec_dir(&((dirstruct)->get_cache))
#define dir_array_value(dirstruct) dir_value(dir_array_dir(dirstruct))






extern void
array_spec_init(array_spec_t *spec)
{   spec->field = NULL;
    spec->elems = 0;
}








extern void
array_spec_end(array_spec_t *spec)
{   field_t *field = spec->field;
    if (NULL != field)
        FTL_FREE(field);
    array_spec_init(spec);
}








extern void
array_spec_set_cont(array_spec_t *spec, field_kind_t kind, size_t elems,
                    field_set_fn_t *set, field_get_fn_t *get)
{   field_t *newfield = (field_t *)FTL_MALLOC(sizeof(field_t));
    if (NULL != newfield)
    {   field_init(newfield, kind, set, get);
        spec->field = newfield;
        spec->elems = elems;
        OMIT(printf("%s: add array content - get %p set %p\n",
                       codeid(), newfield->get, newfield->set);)
    }
}









static void
dir_array_delete(value_t *value)
{   dir_array_t *arraydir = (dir_array_t *)value;
    void *arraymem;
    size_t arraylen; /* number of elements of size stride */

    if ((*arraydir->dimfn)(arraydir, &arraymem, &arraylen) &&
        NULL != arraymem)
    {   free(arraymem);
        arraydir->arraymem = NULL;
    }
    arraydir->ref = (const value_t *)NULL;
    dir_vec_delete(dir_vec_value(&arraydir->get_cache));
}





static void
dir_array_markver(const value_t *value, int heap_version)
{   dir_array_t *arraydir = (dir_array_t *)value;
    OMIT(printf("%s: marking array dir %p\n", codeid(), arraydir);)
    value_mark_version((value_t *)/*unconst*/arraydir->ref, heap_version);
    dir_vec_markver(dir_vec_value(&arraydir->get_cache), heap_version);
}







static bool
dir_array_add(dir_t *dir, const value_t *nameval, const value_t *value)
{   dir_array_t *arraydir = (dir_array_t *)dir;
    bool ok = FALSE;

    if (value_istype(nameval, type_int))
    {   int index = (int)value_int_number(nameval);
        size_t stride = arraydir->stride;
        void *arraymem;
        size_t arraylen;

        if ((*arraydir->dimfn)(arraydir, &arraymem, &arraylen))
        {   if (index < 0 || index >= (int)arraylen)
                fprintf(stderr, "%s: index %d outside array bounds <0..%d> "
                        "on write\n",
                        codeid(), index, (int)arraylen-1);
            else
            {   field_t *field = &arraydir->content;
                (*field->set)((char *)arraymem + index*stride, value);
                ok = TRUE;
            }
        }
    }
    return ok;
}







static const value_t *
dir_array_get(dir_t *dir, const value_t *nameval)
{   dir_array_t *arraydir = (dir_array_t *)dir;
    const value_t *found = NULL;

    if (value_type_equal(nameval, type_int))
    {   int index = (int)value_int_number(nameval);
        size_t stride = arraydir->stride;
        void *arraymem;
        size_t arraylen;

        if ((*arraydir->dimfn)(arraydir, &arraymem, &arraylen))
        {   if (index >= 0 && index < (int)arraylen)
            {   field_t *field = &arraydir->content;
                /* get any value_t we've been saving to put return value in */
                dir_t *get_cache = dir_vec_dir(&arraydir->get_cache);
                const value_t **ref_value = dir_vec_lookup(get_cache, nameval);

                if (NULL == ref_value)
                {   if (dir_vec_add(get_cache, nameval, &value_null))
                        ref_value = dir_vec_lookup(get_cache, nameval);
                }
                if (NULL != ref_value)
                {   (*field->get)((char *)arraymem + index*stride, ref_value);
                    found = *ref_value;
                }
            }
        }
    }
    return found;
}







static void *
dir_array_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_array_t *arraydir = (dir_array_t *)dir;
    size_t stride = arraydir->stride;
    void *result = NULL;
    void *arraymem;
    size_t arraylen;

    if ((*arraydir->dimfn)(arraydir, &arraymem, &arraylen))
    {   int index = 0;
        int maxindex = (int)(arraylen);
        dir_t *get_cache = dir_vec_dir(&arraydir->get_cache);

        while (index < maxindex && NULL == result)
        {   const value_t **ref_value = dir_vec_int_lookup(get_cache, index);
            if (NULL == ref_value)
            {   dir_vec_int_add(get_cache, index, &value_null);
                ref_value = dir_vec_int_lookup(get_cache, index);
            }
            if (NULL != ref_value)
            {   const value_t *nameval = value_int_new(index);
                field_t *field = &arraydir->content;
                (*field->get)((char *)arraymem + index*stride, ref_value);
                result = (*enumfn)(dir, nameval, *ref_value, arg);
                value_unlocal(nameval);
            }
            index++;
        }
    }
    return result;
}







static bool
dir_array_dim(dir_array_t *array, void **out_mem, size_t *out_len)
{   *out_mem = array->arraymem;
    *out_len = array->len;
    return TRUE;
}







static void
dir_array_init(dir_array_t *arraydir, array_spec_t *spec,
               bool is_const, void *arraymem, size_t stride,
               dir_array_get_fn_t *dim,
               const value_t *ref, bool on_heap)
{   dir_t *dir = dir_array_dir(arraydir);
    dir_vec_init(&arraydir->get_cache);
    /* but we're going to overwrite some */
    arraydir->stride    = stride;
    arraydir->arraymem  = arraymem;
    arraydir->len       = spec->elems;
    arraydir->ref       = ref;
    memcpy(&arraydir->content, spec->field, sizeof(arraydir->content));
    if (NULL == dim)
        arraydir->dimfn = &dir_array_dim;
    else
        arraydir->dimfn = dim;
    dir->get = &dir_array_get;
    dir->add = &dir_array_add;
    dir->lookup = NULL;
    dir->forall = &dir_array_forall;
    if (is_const)
        dir->add = NULL;
    else
        dir->add = &dir_array_add;
}





extern dir_t *
dir_carray_new(array_spec_t *spec, bool is_const,
               void *static_array, size_t stride)
{   dir_array_t *arraydir = (dir_array_t *)FTL_MALLOC(sizeof(dir_array_t));

    if (NULL != arraydir)
        dir_array_init(arraydir, spec, is_const, static_array, stride,
                       /*dim*/NULL, /*ref*/NULL, /*on_heap*/FALSE);

    return dir_array_dir(arraydir);
}







extern dir_t *
dir_array_new(array_spec_t *spec, bool is_const,
              void *malloc_array, size_t stride)
{   dir_array_t *arraydir = (dir_array_t *)FTL_MALLOC(sizeof(dir_array_t));

    if (NULL != arraydir)
        dir_array_init(arraydir, spec, is_const, malloc_array, stride,
                       /*dim*/NULL, /*ref*/NULL, /*on_heap*/TRUE);

    return dir_array_dir(arraydir);
}







extern dir_t *
dir_array_cast(array_spec_t *spec, bool is_const, const value_t *ref,
               void *ref_array, size_t stride)
{   dir_array_t *arraydir = (dir_array_t *)FTL_MALLOC(sizeof(dir_array_t));

    if (NULL != arraydir)
        dir_array_init(arraydir, spec, is_const, ref_array, stride,
                       /*dim*/NULL, ref, /*on_heap*/FALSE);

    return dir_array_dir(arraydir);
}







static bool
dir_array_string_dim(dir_array_t *array, void **out_mem, size_t *out_len)
{   const char *base;
    size_t len;
    if (value_string_get(array->ref, &base, &len))
    {   *out_mem = (void *)base;  /* warning allows writes to const area */
        *out_len = len/array->stride;
        return TRUE;
    } else
        return FALSE;
}







extern dir_t *
dir_array_string(array_spec_t *spec, bool is_const,
                 const value_t *string, size_t stride)
{   dir_t *dir = NULL;

    if (value_istype(string, type_string))
    {   dir_array_t *arraydir = (dir_array_t *)FTL_MALLOC(sizeof(dir_array_t));

        if (NULL != arraydir)
        {   dir_array_init(arraydir, spec, is_const, /*mem*/NULL, stride,
                           &dir_array_string_dim, string, /*on_heap*/TRUE);
            dir = dir_array_dir(arraydir);
        }
    }
    return dir;
}







extern void
dir_array_update(const value_t **ref_value, array_spec_t *spec,
                 bool is_const, void *arraymem, size_t stride)
{   const value_t *val = *ref_value;
    if (NULL == val || !value_type_equal(val, type_dir))
        /* TODO: this should really use dir_carray_clone on the parent ref */
        *ref_value = dir_value(dir_carray_new(spec, is_const,
                                              arraymem, stride));
}











/*****************************************************************************
 *                                                                           *
 *          Integer Series Directories                                       *
 *          ==========================                                       *
 *                                                                           *
 *****************************************************************************/






typedef struct
{   dir_t dir;
    number_t first;
    number_t inc;
    number_t last;
} dir_series_t;





static value_type_t type_dir_series_val;





static const value_t *
dir_series_get(dir_t *dir, const value_t *name)
{   dir_series_t *series = (dir_series_t *)dir;
    const value_t *found = NULL;

    if (value_type_equal(name, type_int))
    {   number_t index = (size_t)value_int_number(name);
        number_t projected = series->first + index*series->inc;
        if (series->inc > 0? projected <= series->last:
                             projected >= series->last)
            found = value_int_new(projected);
    }

    return found;
}







static void *
dir_series_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_series_t *series = (dir_series_t *)dir;
    number_t i = series->first;
    number_t n = 0;
    void *result = NULL;

    if (series->inc > 0)
    {   while (i<=series->last && NULL == result)
        {   value_t *nval = value_int_new(n);
            value_t *ival = value_int_new(i);
            (*enumfn)(dir, nval, ival, arg);
            value_unlocal(nval);
            value_unlocal(ival);
            n++;
            i += series->inc;
        }
    } else if (series->inc < 0)
    {   while (i>=series->last && NULL == result)
        {   value_t *nval = value_int_new(n);
            value_t *ival = value_int_new(i);
            (*enumfn)(dir, nval, ival, arg);
            value_unlocal(nval);
            value_unlocal(ival);
            n++;
            i += series->inc;
        }
    } else
    {   value_t *nval = value_int_new(n);
        value_t *ival = value_int_new(i);
        (*enumfn)(dir, nval, ival, arg);
        value_unlocal(nval);
        value_unlocal(ival);
    }

    return result;
}






static int
dir_series_print(outchar_t *out, const value_t *root, const value_t *value)
{   int len = 0;

    if (value_istype(value, type_dir))
    {   dir_series_t *series = (dir_series_t *)value;

        len += outchar_printf(out, "<%d", series->first);
        if ((series->last > series->first && 1 != series->inc) ||
            (series->last < series->first && -1 != series->inc))
            len += outchar_printf(out, ", %d", series->first + series->inc);

        len += outchar_printf(out, " .. %d>", series->last);
    }

    return len;
}





static dir_t *
dir_series_init(dir_series_t *series,
                number_t first, number_t inc, number_t last)
{   if (inc == 0 && first != last)
        /* impossible series */
        return NULL;

    else
    {
        /* if ((inc > 0 && first > last) ||
               (inc < 0 && first < last)) */
            /* empty series */
        series->first = first;
        series->inc = inc;
        series->last = last;

        DEBUG_SERIES(DPRINTF("series: %ld +%ld .. %ld\n", first, inc, last););

        dir_init(&series->dir, &type_dir_series_val,
                 /*add*/NULL, /*lookup*/NULL,
                 &dir_series_get, &dir_series_forall, /*on_heap*/TRUE);

        return &series->dir;
    }
}






static dir_t *
dir_series_new_vals(const value_t *firstval, const value_t *secondval,
                    const value_t *lastval)
{   dir_series_t *series;

    DEBUG_SERIES(DPRINTF("firstval %s, secondval %s, lastval %s\n",
                        value_type_name(firstval), value_type_name(secondval),
                        value_type_name(lastvalval));)

    if ((firstval == NULL || value_istype(firstval, type_int)) &&
        (secondval == NULL || value_istype(secondval, type_int)) &&
        (lastval == NULL || value_istype(lastval, type_int)))
    {   series = (dir_series_t *)FTL_MALLOC(sizeof(dir_series_t));

        if (NULL != series)
        {   /* deal with series specified as <firstval secondval .. sn>
               secondval can only be set if firstval is already set,
               sn can be set independently
            */
            number_t first = 1;
            number_t inc = 1;
            number_t last = 0x7ffffff;

            if (firstval != NULL)
            {   first = value_int_number(firstval);
                if (secondval != NULL)
                    inc = value_int_number(secondval) - first;
            }
            if (lastval != NULL)
            {   last = value_int_number(lastval);
                if (secondval == NULL)
                {   /* inc won't have been set yet */
                    if (first > last)
                        inc = -1;
                    if (first == last)
                        inc = 0;
                }
            }
            else if (inc == 0)
                last = first;
            else if (inc < 0)
                last = -last;

            return dir_series_init(series, first, inc, last);
        } else
            return NULL;
    } else
        return NULL;
}









extern dir_t *
dir_series_new(number_t first, number_t inc, number_t last)
{   dir_series_t *series = (dir_series_t *)FTL_MALLOC(sizeof(dir_series_t));

    if (NULL != series)
        return dir_series_init(series, first, inc, last);
    else
        return NULL;
}










/*****************************************************************************
 *                                                                           *
 *          System Env Directories                                           *
 *          ======================                                           *
 *                                                                           *
 *****************************************************************************/








typedef struct
{   dir_t dir;
} dir_sysenv_t;




static value_type_t type_dir_sysenv_val;




#define dir_sysenv_value(dirid) dir_value(&((dirid)->dir))






static bool
dir_sysenv_add(dir_t *dir, const value_t *name, const value_t *value)
{   /*dir_sysenv_t *sysdir = (dir_sysenv_t *)dir;*/
    bool ok = FALSE;
    const char *namestr;
    size_t namestrl;
    const char *valstr;
    size_t valstrl;

    if (value_string_get(name, &namestr, &namestrl))
    {
        if (value == &value_null)
#ifdef HAS_UNSETENV
        {   unsetenv(namestr);
            ok = TRUE;
        }
#else
        {   char envbuf[FTL_ID_MAX+2];
            if (namestrl+2 < sizeof(envbuf))
            {   memcpy(&envbuf[0], namestr, namestrl);
                envbuf[namestrl] = '=';
                envbuf[namestrl+1] = '\0';
                ok = (0 == putenv(&envbuf[0]));
            } else
                ok = FALSE;
        }
#endif
        else
        if (value_string_get(value, &valstr, &valstrl))
        {
#ifdef HAS_SETENV
            int rc = setenv(namestr, valstr, /*overwrite*/TRUE);
            ok = (rc == 0);
#else
            char envbuf[FTL_ID_MAX + FTL_SYSENV_VAL_MAX + 2];
            if (namestrl+2+valstrl < sizeof(envbuf))
            {   memcpy(&envbuf[0], namestr, namestrl);
                envbuf[namestrl] = '=';
                memcpy(&envbuf[namestrl+1], valstr, valstrl);
                envbuf[namestrl+valstrl+1] = '\0';
                ok = (0 == putenv(&envbuf[0]));
            } else
                ok = FALSE;
#endif
        }
    }
    return ok;
}






static const value_t *
dir_sysenv_get(dir_t *dir, const value_t *name)
{   const char *namestr = NULL;
    size_t namestrl;
    const value_t *val = (const value_t *)NULL;

    if (value_type_equal(name, type_string) &&
        value_string_get(name, &namestr, &namestrl))
    {   char *strval = getenv(namestr);
        if (PTRVALID(strval))
            val = value_string_new_measured(strval);
        OMIT(else printf("%s: sysenv get '%s' not in env\n",
                           codeid(), namestr);)
    }
    return val;
}







static void
dir_sysenv_init(dir_sysenv_t *sysdir)
{   dir_init(&sysdir->dir, &type_dir_sysenv_val, &dir_sysenv_add,
             /*lookup*/ NULL, /*get*/&dir_sysenv_get,
             /* forall */NULL, /*on_heap*/FALSE);
}






extern dir_t *
dir_sysenv_new(void)
{   dir_sysenv_t *sysdir = (dir_sysenv_t *)FTL_MALLOC(sizeof(dir_sysenv_t));

    if (PTRVALID(sysdir))
        dir_sysenv_init(sysdir);

    return &sysdir->dir;
}








/*****************************************************************************
 *                                                                           *
 *          Filesystem Directories                                           *
 *          ======================                                           *
 *                                                                           *
 *****************************************************************************/




#if 0



typedef struct
{   dir_t dir;
    const value_t *dirnameval;
    const char *dirname;
    size_t dirnamelen;
} dir_fs_t;




static value_type_t type_dir_fs_val;




#define dir_fs_value(dirid) dir_value(&((dirid)->dir))






static bool
dir_fs_add(dir_t *dir, const value_t *name, const value_t *value)
{   /*dir_fs_t *sysdir = (dir_fs_t *)dir;*/
    bool ok = FALSE;
    const char *namestr;
    size_t namestrl;
    const char *valstr;
    size_t valstrl;

    if (value_string_get(name, &namestr, &namestrl))
    {
        if (value == &value_null)
#ifdef HAS_UNSETENV
        {   unsetenv(namestr);
            ok = TRUE;
        }
#else
        {   char envbuf[FTL_ID_MAX+2];
            if (namestrl+2 < sizeof(envbuf))
            {   memcpy(&envbuf[0], namestr, namestrl);
                envbuf[namestrl] = '=';
                envbuf[namestrl+1] = '\0';
                ok = (0 == putenv(&envbuf[0]));
            } else
                ok = FALSE;
        }
#endif
        else
        if (value_string_get(value, &valstr, &valstrl))
        {
#ifdef HAS_SETENV
            int rc = setenv(namestr, valstr, /*overwrite*/TRUE);
            ok = (rc == 0);
#else
            char envbuf[FTL_ID_MAX + FTL_fs_VAL_MAX + 2];
            if (namestrl+2+valstrl < sizeof(envbuf))
            {   memcpy(&envbuf[0], namestr, namestrl);
                envbuf[namestrl] = '=';
                memcpy(&envbuf[namestrl+1], valstr, valstrl);
                envbuf[namestrl+valstrl+1] = '\0';
                ok = (0 == putenv(&envbuf[0]));
            } else
                ok = FALSE;
#endif
        }
    }
    return ok;
}






static const value_t *
dir_fs_get(dir_t *dir, const value_t *name)
{   const char *namestr = NULL;
    size_t namestrl;
    const value_t *val = (const value_t *)NULL;

    if (value_type_equal(name, type_string) &&
        value_string_get(name, &namestr, &namestrl))
    {   char *strval = getenv(namestr);
        if (PTRVALID(strval))
            val = value_string_new_measured(strval);
        OMIT(else printf("%s: fs get '%s' not in env\n",
                           codeid(), namestr);)
    }
    return val;
}







typedef struct {
    dir_t *dir;
    dir_enum_fn_t *enumfn;
    void *arg;
} dir_fs_enum_args_t;




static bool
dir_fs_entry(void *enum_arg, const char *entry, const char *subdir,
             bool examinable, bool is_dir)
{
    dir_fs_t *fsdir = (dir_fs_t *)args->dir;
    value_t *val;/*?*/

    result = (*args->enumfn)(args->dir, entry, value, args->arg);
}



static void *
dir_fs_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_fs_t *fsdir = (dir_fs_t *)value;
    void *result = NULL;
    dir_fs_enum_args_t arg;
    bool ok;

    arg.dir = dir;
    arg.enumfn = enumfn;
    arg.arg = arg;

    ok = fs_enum_dir(fsdir->dirname, &dir_fs_entry, &arg);

    return result;
}





static void
dir_fs_markver(const value_t *value, int heap_version)
{   dir_fs_t *fsdir = (dir_fs_t *)value;
    value_mark_version((value_t *)/*unconst*/fsdur->dirnameval, heap_version);
}





static void
dir_fs_init(dir_fs_t *fsdir, const value_t *dirnameval)
{   dir_init(&fsdir->dir, &type_dir_fs_val, &dir_fs_add, /*lookup*/ NULL,
             /*get*/&dir_fs_get, &dir_fs_forall, /*on_heap*/FALSE);
    fsdir->dirnameval = value_string_get_terminated(dirnameval,
                                                    &fsdir->dirname,
                                                    &fsdir->dirnamelen);
}






extern dir_t *
dir_fs_new(const char *dirname)
{   dir_fs_t *fsdir = (dir_fs_t *)FTL_MALLOC(sizeof(dir_fs_t));

    if (PTRVALID(fsdir))
        dir_fs_init(fsdir, dirname);

    return &fsdir->dir;
}





#endif






/*****************************************************************************
 *                                                                           *
 *          Shared Library Directories                                       *
 *          ==========================                                       *
 *                                                                           *
 *****************************************************************************/





typedef struct
{   dir_id_t dir;
    dllib_t lib;
    bool fixed;
    const value_t *libnameval;
    const char *libname;  /* redundantly implied by dirnameval */
    size_t libnamelen;    /* redundantly implied by dirnameval */
} dir_lib_t;





static value_type_t type_dir_lib_val;




#define dir_lib_dir(dirid)   dir_id_dir(&((dirid)->dir))
#define dir_lib_value(dirid) dir_id_value(&((dirid)->dir))



static void dir_lib_delete(value_t *value)
{   if (value_istype(value, type_dir))
    {   dir_lib_t *libdir = (dir_lib_t *)value;
        if (!libdir->fixed) {
           free_library(libdir->lib);
        }
        dir_id_delete(value);
    }
}




static void dir_lib_markver(const value_t *value, int heap_version)
{   dir_lib_t *libdir = (dir_lib_t *)value;
    value_mark_version((value_t *)/*unconst*/libdir->libnameval, heap_version);
    dir_id_markver(dir_lib_value(libdir), heap_version);
}



typedef struct {
    dir_t *valdir;
    dllib_t lib;
} libsym_enum_args_t;




/* called for each entry in a vector of strings */
static void *
libsym_enum(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{
    libsym_enum_args_t *args = (libsym_enum_args_t *)arg;
    const char *sym;
    size_t symlen;

    if (value_string_get(value, &sym, &symlen))
    {   libfn_t fn = lib_sym(args->lib, sym);
        if (fn != NULL)
           dir_set(args->valdir, value,
                   value_int_new((unumber_t)(ptrdiff_t)fn));
    }
    return NULL; /* continue enumeration */
}




/* Initialize a directory of symbol values held in the given dynamic library
 *   @param  libdir       - empty library directory structure to initialize
 *   @param  libfilename  - 0-terminated string containing the name of the lib
 *   @param  symbs        - vector of symbol names to be provided
 *   @param  fixed        - TRUE iff the library is not to be unloaded
 */
static dir_t *
dir_lib_init(dir_lib_t *libdir,
             const value_t *libnameval, dir_t *syms, bool fixed)
{
    libdir->lib = load_library(libdir->libname);

    if (libdir->lib != DLL_NONE) {
        libsym_enum_args_t arg;
        dir_t *dir = dir_id_dir(&libdir->dir);

        dir_id_init(&libdir->dir, &type_dir_lib_val, /*on_heap*/TRUE);
        dir->add = NULL;

        libdir->libnameval = value_string_get_terminated(libnameval,
                                                         &libdir->libname,
                                                         &libdir->libnamelen);
        libdir->fixed = fixed;

        arg.valdir = dir;
        arg.lib = libdir->lib;
        (void)dir_forall(syms, &libsym_enum, &arg);

        return dir;
    } else
        return NULL;
}





/* Create a directory of symbol values held in the given dynamic library
 *   @param  libfilename  - 0-terminated string containing the name of the lib
 *   @param  symbs        - vector of symbol names to be provided
 *   @param  fix          - TRUE iff the library is not to be unloaded
 *
 *   If \c fix is not set the library will be unloaded when the directory is
 *   garbage collected.
 */
extern dir_t *
dir_lib_new(const value_t *libfilename, dir_t *syms, bool fix)
{   dir_lib_t *libdir = (dir_lib_t *)FTL_MALLOC(sizeof(dir_lib_t));

    if (PTRVALID(libdir))
        dir_lib_init(libdir, libfilename, syms, fix);

    return dir_lib_dir(libdir);
}







/*****************************************************************************
 *                                                                           *
 *          Registry Directories                                             *
 *          ====================                                             *
 *                                                                           *
 *****************************************************************************/








#ifdef HAS_REGISTRY



#include <windows.h>



#ifdef UNICODE_REGVAL_NAMES

#define REGVAL_NAMEVAR_CH_T wchar_t

#define REGVAL_NAMEVAR_DECLARE(_namevar)                                \
    size_t _namevar##_wchars = 0;                                       \
    wchar_t *_namevar = wcstralloc(mbs, msblen, &_namevar##_wchars);    \

#define REGVAL_NAMEVAR_OPEN(_nameval, _mbs, _mbslen)                    \
    (_namevar = wcstralloc(_mbs, _msblen, &_namevar##_wchars),          \
     NULL != _namevar? ERROR_SUCCESS: ERROR_NOT_ENOUGH_MEMORY)

#define REGVAL_NAMEVAR_CLOSE(_nameval)                                  \
    FTL_FREE(_namevar)

#define REGVAL_NAMEVAR_FN(_fn) _fn##W

#else

#define REGVAL_NAMEVAR_DECLARE(_namevar)                                \
    char *_namevar##_lastpos;                                           \
    char _namevar##_lastch;                                             \
    const char *_namevar

/* This is nasty and might cause errors in strange cirucmstances */

#define REGVAL_NAMEVAR_CH_T char

#define REGVAL_NAMEVAR_OPEN(_namevar, _mbs, _mbslen)                    \
    (_namevar = _mbs, _namevar##_lastpos = (char *)&(_mbs)[_mbslen],    \
     (_namevar##_lastch = *_namevar##_lastpos) == '\0'?                 \
       (*_namevar##_lastpos = '\0', ERROR_SUCCESS): ERROR_SUCCESS)

#define REGVAL_NAMEVAR_CLOSE(_namevar)                                  \
    do {                                                                \
        if (_namevar##_lastch != '\0')                                  \
            *_namevar##_lastpos = _namevar##_lastch;                    \
    } while (0)

#define REGVAL_NAMEVAR_FN(_fn) _fn

#endif /* UNICODE_REGVAL_NAMES */

#define REGVAL_NAMEVAR_T const REGVAL_NAMEVAR_CH_T *




typedef HKEY    key_t;
typedef REGSAM  key_access_t;
typedef DWORD   key_type_t;
typedef REGVAL_NAMEVAR_CH_T key_valname_t[256];



typedef struct
{   dir_t dir;
    key_t key;
} dir_regkeyval_t;





#define dir_regkeyval_dir(keydir) (&(keydir)->dir)

#define dir_regkeyval_value(dirid) dir_value(&((dirid)->dir))




static value_type_t type_dir_regkeyval_val;





STATIC_INLINE size_t
mbsntowcs(wchar_t *dest, const char *src, size_t srclen, size_t dest_wchars)
{   /* This is nasty and might cause errors in strange cirucmstances */
    size_t n;

    if (src != NULL)
    {   char endch = src[srclen]; /* out of bounds reference */

        if (endch != '\0')
        {   ((char *)src)[srclen] = '\0';
            n =  mbstowcs(dest, src, dest_wchars);
            ((char *)src)[srclen] = endch;
        } else
            n =  mbstowcs(dest, src, dest_wchars);
    } else
        n =  mbstowcs(dest, src, dest_wchars);

    return n;
}






static wchar_t *
wcstralloc(const char *str, size_t strleng, size_t *out_wchars)
{   if (NULL == str)
        return (wchar_t *)NULL;
    else
    {   size_t n = mbsntowcs(NULL, str, strleng, 0);

        if (n == (size_t)(-1))
        {   OMIT(fprintf(stderr, "%s: unicode conversion failed for '%s'\n",
                           codeid(), str);)
            return (wchar_t *)NULL; /* conversion error */
        } else
        {   wchar_t *wcstr = (wchar_t *)FTL_MALLOC((n+1)*sizeof(wchar_t));
            if (NULL == wcstr)
                return (wchar_t *)NULL; /* out of memory */
            else
            {   size_t realn = mbsntowcs(wcstr, str, strleng, n+1);
                /* +1 converts final '\0' too */
                if (realn == (size_t)(-1))
                {   FTL_FREE(wcstr);
                    OMIT(fprintf(stderr, "%s: unicode conversion of '%s' - "
                                   "bad size %d (supposed to be %d)?\n",
                                    codeid(), str, n, realn);)
                    return (wchar_t *)NULL; /* size error? */
                } else
                {   /* updated only on success */
                    *out_wchars = realn;  /* count OMITs final L'\0; */
                    return wcstr;
                }
            }
        }
    }
}









#define regkeyval_ACCESS_READ      (KEY_READ)
#define regkeyval_ACCESS_READWRITE (KEY_READ | KEY_WRITE)


static long /* rc */
win_regkeyval_open(key_t root, const char *subkey, size_t subkeylen,
                   key_access_t access, key_t *out_key)
{   int rc;
    REGVAL_NAMEVAR_DECLARE(subkey_k);

    rc = REGVAL_NAMEVAR_OPEN(subkey_k, subkey, subkeylen);

    if (ERROR_SUCCESS == rc)
    {   rc = REGVAL_NAMEVAR_FN(RegOpenKeyEx)(root, subkey_k, /*options*/0,
                                             access, out_key);
        REGVAL_NAMEVAR_CLOSE(subkey_k);
    }

    if (rc != ERROR_SUCCESS)
        *out_key = NULL;  /* couldn't open key */

    return rc;
}






static long /* rc */
win_regkeyval_close(key_t *ref_key)
{   key_t key = *ref_key;

    OMIT(fprintf(stderr, "%s: closing key handle %p\n", codeid(), key);)
    if (key != NULL)
    {   int rc = RegCloseKey(key);
        if (rc != ERROR_SUCCESS)
        {   DO(fprintf(stderr, "%s: close of key handle %p failed - rc %d\n",
                       codeid(), key, rc);)
            return rc;
        } else
        {   *ref_key = NULL;
            return 0;
        }
    } else
        /* already closed */
        return 0;
}








static long /* rc */
win_regkeyval_get(key_t key, const char *valname, size_t valnamelen,
               key_type_t *out_type, void **out_data, size_t *out_datalen)
{   REGVAL_NAMEVAR_DECLARE(valname_k);
    long rc = REGVAL_NAMEVAR_OPEN(valname_k, valname, valnamelen);

    *out_data = NULL;
    *out_datalen = 0;

    if (ERROR_SUCCESS == rc)
    {   DWORD datalen = 0;

        rc = REGVAL_NAMEVAR_FN(RegQueryValueEx)(key,
                                                valname_k, /*reserved*/NULL,
                                                out_type, NULL, &datalen);
        if (rc == ERROR_SUCCESS ||
            rc == ERROR_MORE_DATA) /* we expect one of these */
        {   BYTE *data = FTL_MALLOC(datalen);
            if (NULL == data)
                rc = ERROR_NOT_ENOUGH_MEMORY;
            else
            {
                rc = REGVAL_NAMEVAR_FN(RegQueryValueEx)(key, valname_k,
                                                        /*reserved*/NULL,
                                                        out_type, data,
                                                        &datalen);
                if (rc != ERROR_SUCCESS)
                {   OMIT(fprintf(stderr, "%s: (real) query for reqistry "
                                   "value failed rc %d\n", codeid(), rc);)
                    FTL_FREE(data);
                } else
                {   *out_data = data;
                    *out_datalen = datalen;
                }
            }
        }
        OMIT(else fprintf(stderr, "%s: initial query for reqistry value "
                            "failed rc %d\n", codeid(), rc););

        REGVAL_NAMEVAR_CLOSE(valname_k);
    }

    return rc;
}









/* It would be more efficient if this function were updated to keep the
   data buffer for use next time - TBD */

static long /* rc */
win_regkeyval_enum_value(key_t key, unsigned long n,
                         bool *out_last,
                         key_type_t *out_type,
                         key_valname_t *name_wc, size_t *out_namechars,
                         void **out_data, size_t *out_datalen)
{   long rc;
    BYTE one_byte;
    DWORD datalen = 0;
    DWORD namechars = sizeof(key_valname_t)/sizeof(wchar_t);

    *out_data = NULL;
    *out_datalen = 0;
    *out_namechars = 0;
    *out_last = TRUE;

    /* update the values of namechars and datalen to be "big enough" */
    rc = REGVAL_NAMEVAR_FN(RegEnumValue)(key, n, &(*name_wc)[0], &namechars,
                                        /*reserved*/NULL,
                                        out_type, /*data*/&one_byte, &datalen);

    if (rc == ERROR_SUCCESS ||
        rc == ERROR_MORE_DATA) /* we expect one of these */
    {   BYTE *data = FTL_MALLOC(datalen);
        if (NULL == data)
            rc = ERROR_NOT_ENOUGH_MEMORY;
        else
        {   namechars = sizeof(key_valname_t)/sizeof(wchar_t);
            /* ... which must include the final null character */
            rc = REGVAL_NAMEVAR_FN(RegEnumValue)(
                     key, n, &(*name_wc)[0], &namechars, /*reserved*/NULL,
                     out_type, data, &datalen);
            /* weird isn't it? namechars should have been updated to be the
               number of character _not_ including the final null character
               now */
            if (rc != ERROR_SUCCESS)
            {   FTL_FREE(data);
                if (rc == ERROR_NO_MORE_ITEMS)
                    rc = ERROR_SUCCESS;
                    /* *out_last will be TRUE */
                OMIT(else
                       fprintf(stderr, "%s: (real) enum of value %d "
                               "fails rc %d - data %d bytes name %d chars\n",
                               codeid(), n, rc, datalen, namechars);)
            } else
            {   *out_data = data;
                *out_datalen = datalen;
                *out_namechars = namechars;
                *out_last = FALSE;
            }
        }
    } else
    {   if (rc == ERROR_NO_MORE_ITEMS)
            rc = ERROR_SUCCESS;
            /* *out_last will be TRUE */
        DO(else fprintf(stderr, "%s: initial enum of value %lu "
                            "fails rc %ld\n", codeid(), n, rc);)

    }
    return rc;
}






static const value_t *
value_keyinfo(key_type_t key_type, void *key_data, size_t datalen)
{   const value_t *val = NULL;
    unsigned char *data = (unsigned char *)key_data;
    dir_t *dir = dir_id_new();

    OMIT(fprintf(stderr,"%s: key type %d data size %d\n",
                   codeid(), key_type, datalen););

    if (PTRVALID(dir))
    {   unumber_t n;
        dir_cstring_set(dir, "type", value_int_new(key_type));
        switch (key_type)
        {
            case REG_DWORD_LITTLE_ENDIAN:
                n = (unumber_t)data[0]       | ((unumber_t)data[1]<<8) |
                    ((unumber_t)data[2]<<16) | ((unumber_t)data[3]<<24);
                dir_cstring_set(dir, "data", value_int_new(n));
                break;

            case REG_DWORD_BIG_ENDIAN:
                n = (unumber_t)data[3]       | ((unumber_t)data[2]<<8) |
                    ((unumber_t)data[1]<<16) | ((unumber_t)data[0]<<24);
                dir_cstring_set(dir, "data", value_int_new(n));
                break;

            case REG_QWORD_LITTLE_ENDIAN:
                n = (unumber_t)data[0]       | ((unumber_t)data[1]<<8)  |
                    ((unumber_t)data[2]<<16) | ((unumber_t)data[3]<<24) |
                    ((unumber_t)data[4]<<32) | ((unumber_t)data[5]<<40) |
                    ((unumber_t)data[6]<<48) | ((unumber_t)data[7]<<56);
                dir_cstring_set(dir, "data", value_int_new(n));
                break;

            case REG_EXPAND_SZ:
            case REG_LINK:
            case REG_SZ:
#ifdef UNICODE_REGVAL_NAMES
            {   size_t charlen = (datalen+1)/sizeof(wchar_t);
                const wchar_t data_wc = (const wchar_t *)data;
                if (data_wc[charlen-1] == '\0')
                    charlen--;
                dir_cstring_set(dir, "data",
                                value_wcstring_new(data_wc, charlen);
#else
                if (data[datalen-1] == '\0')
                    datalen--;
                dir_cstring_set(dir, "data",
                                value_string_new((const char *)data, datalen));
#endif
                break;

            case REG_MULTI_SZ:
                /* actually an array of zero terminated strings,
                   terminated by another zero - could do better */
                dir_cstring_set(dir, "data",
                                value_string_new((const char *)data, datalen));
                break;

            case REG_RESOURCE_LIST:
            case REG_RESOURCE_REQUIREMENTS_LIST:
            case REG_FULL_RESOURCE_DESCRIPTOR:
                /* don't really know what to do with these */
                dir_string_set(dir, "data",
                               value_string_new((const char *)data, datalen));
                break;

            case REG_NONE:
            case REG_BINARY:
            default:
                dir_cstring_set(dir, "data",
                                value_string_new((const char *)data, datalen));
                break;

        }
        val = dir_value(dir);
    }
    return val;
}






static const value_t *
dir_regkeyval_get(dir_t *dir, const value_t *name)
{   dir_regkeyval_t *keydir = (dir_regkeyval_t *)dir;
    const char *namestr = NULL;
    size_t namestrl;
    const value_t *val = (const value_t *)NULL;

    if (value_type_equal(name, type_string) &&
        value_string_get(name, &namestr, &namestrl))
    {   key_type_t key_type;
        void *data;
        size_t datalen;
        int rc = win_regkeyval_get(keydir->key, namestr, namestrl,
                                   &key_type, &data, &datalen);
        if (rc == ERROR_SUCCESS)
        {   val = value_keyinfo(key_type, data, datalen);
            FTL_FREE(data);
        }
    }

    return val;
}







static void *
dir_regkeyval_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_regkeyval_t *keydir = (dir_regkeyval_t *)dir;
    unsigned int n = 0;
    bool complete = FALSE;
    int rc = ERROR_SUCCESS;
    void *result = NULL;

    do {
        key_type_t key_type;
        size_t valnamechars;
        void *data;
        size_t datalen;
        key_valname_t keyval_wc;

        rc = win_regkeyval_enum_value(keydir->key, n, &complete, &key_type,
                                      &keyval_wc, &valnamechars,
                                      &data, &datalen);
        OMIT(fprintf(stderr,"%s: enum %d %scomplete rc %d\n",
                       codeid(), n, complete?"":"not ", rc);)
        (void)rc;
        if (!complete)
        {   result = (*enumfn)(dir,
#ifdef UNICODE_REGVAL_NAMES
                               value_wcstring_new(&keyval_wc[0], valnamechars),
#else
                               value_string_new(&keyval_wc[0], valnamechars),
#endif
                               value_keyinfo(key_type, data, datalen), arg);
            FTL_FREE(data);
        }
        n++;
    } while (result == NULL && !complete);

    return result;
}





STATIC_INLINE bool
dir_regkeyval_check_int(const value_t *data, const char *typename)
{   if (!value_type_equal(data, type_int))
    {   fprintf(stderr, "%s: %s \"data\" must be an integer not a %s\n",
                codeid(), typename, value_type_name(data));
        return FALSE;
    } else
        return TRUE;
}




STATIC_INLINE bool
dir_regkeyval_check_string(const value_t *data, const char *typename)
{   if (!value_type_equal(data, type_string))
    {   fprintf(stderr, "%s: %s \"data\" must be a string not a %s\n",
                codeid(), typename, value_type_name(data));
        return FALSE;
    } else
        return TRUE;
}





static int /* rc */
dir_regkeyval_set(key_t key, REGVAL_NAMEVAR_T valname_k,
                  const value_t *typeval, const value_t *data)
{   unumber_t key_type = value_int_number(typeval);
    BYTE int_data[8];
    const char *datastr;
    size_t datastrl;
    int rc = ERROR_INVALID_DATA;

    switch (key_type)
    {
        case REG_DWORD_LITTLE_ENDIAN:
            if (dir_regkeyval_check_int(data, "REG_DWORD_LITTLE_ENDIAN"))
            {   unumber_t n = value_int_number(data);
                int_data[0] = (BYTE)(n>>0)  & 0xff;
                int_data[1] = (BYTE)(n>>8)  & 0xff;
                int_data[2] = (BYTE)(n>>16) & 0xff;
                int_data[3] = (BYTE)(n>>24) & 0xff;
                rc = REGVAL_NAMEVAR_FN(RegSetValueEx)(key, valname_k,
                                                      /*reserved*/0,
                                                      (DWORD)key_type,
                                                      &int_data[0], 4);
            }
            break;

        case REG_DWORD_BIG_ENDIAN:
            if (dir_regkeyval_check_int(data, "REG_DWORD_BIG_ENDIAN"))
            {   unumber_t n = value_int_number(data);
                int_data[0] = (BYTE)(n>>24) & 0xff;
                int_data[1] = (BYTE)(n>>16) & 0xff;
                int_data[2] = (BYTE)(n>>8)  & 0xff;
                int_data[3] = (BYTE)(n>>0)  & 0xff;
                rc = REGVAL_NAMEVAR_FN(RegSetValueEx)(key, valname_k,
                                                      /*reserved*/0,
                                                      (DWORD)key_type,
                                                      &int_data[0], 4);
            }
            break;

        case REG_QWORD_LITTLE_ENDIAN:
            if (dir_regkeyval_check_int(data, "REG_QWORD_LITTLE_ENDIAN"))
            {   unumber_t n = value_int_number(data);
                int_data[0] = (BYTE)(n>>0)  & 0xff;
                int_data[1] = (BYTE)(n>>8)  & 0xff;
                int_data[2] = (BYTE)(n>>16) & 0xff;
                int_data[3] = (BYTE)(n>>24) & 0xff;
                int_data[4] = (BYTE)(n>>32) & 0xff;
                int_data[5] = (BYTE)(n>>40) & 0xff;
                int_data[6] = (BYTE)(n>>48) & 0xff;
                int_data[7] = (BYTE)(n>>56) & 0xff;
                rc = REGVAL_NAMEVAR_FN(RegSetValueEx)(key, valname_k,
                                                      /*reserved*/0,
                                                      (DWORD)key_type,
                                                      &int_data[0], 8);
            }
            break;

        case REG_EXPAND_SZ:
        case REG_LINK:
        case REG_SZ:
            if (dir_regkeyval_check_string(data, "REG_EXPAND_SZ, "
                                           "REG_LINK or REG_SZ") &&
                value_string_get(data, &datastr, &datastrl))
            {   size_t data_wchars = 0;
                wchar_t *data_wc = wcstralloc(datastr, datastrl,
                                              &data_wchars);
                if (NULL == data_wc)
                    rc = ERROR_NOT_ENOUGH_MEMORY;
                else
                {   /* data size must include terminating L'\0' */
                    if (data_wc[data_wchars] != L'\0')
                        fprintf(stderr, "%s: written UNICODE string does not "
                                "have proper termination (len %d from %d)\n",
                                codeid(), (int)data_wchars, (int)datastrl);
                    rc = REGVAL_NAMEVAR_FN(RegSetValueEx)(
                             key, valname_k, /*reserved*/0, (DWORD)key_type,
                             (const BYTE *)data_wc,
                             (DWORD)(data_wchars+1)*sizeof(wchar_t));
                    FTL_FREE(data_wc);
                }
            }
            break;

        case REG_MULTI_SZ:
            /* actually an array of zero terminated strings,
               terminated by another zero - could do better */
            if (dir_regkeyval_check_string(data, "REG_MULTI_SZ") &&
                value_string_get(data, &datastr, &datastrl))
                rc = REGVAL_NAMEVAR_FN(RegSetValueEx)(
                         key, valname_k, /*reserved*/0, (DWORD)key_type,
                         (const BYTE *)datastr, (DWORD)datastrl);
                /* should check that string ends with \0\0 */
            break;

        case REG_RESOURCE_LIST:
        case REG_RESOURCE_REQUIREMENTS_LIST:
        case REG_FULL_RESOURCE_DESCRIPTOR:
            /* don't really know what to do with these */
            if (dir_regkeyval_check_string(data, "REG_RESOURCE_LIST, "
                                           "REG_RESOURCE_REQUIREMENTS_LIST, "
                                           "or REG_FULL_RESOURCE_DESCRIPTOR")&&
                value_string_get(data, &datastr, &datastrl))
                rc = REGVAL_NAMEVAR_FN(RegSetValueEx)(
                         key, valname_k, /*reserved*/0, (DWORD)key_type,
                         (const BYTE *)datastr, (DWORD)datastrl);
            break;

        case REG_NONE:
        case REG_BINARY:
            if (dir_regkeyval_check_string(data, "REG_BINARY or REG_NONE") &&
                value_string_get(data, &datastr, &datastrl))
                rc = REGVAL_NAMEVAR_FN(RegSetValueEx)(
                         key, valname_k, /*reserved*/0, (DWORD)key_type,
                         (const BYTE *)datastr, (DWORD)datastrl);
            break;

        default:
            fprintf(stderr, "%s: \"type\" %"F_UNUMBER_T" is not a "
                    "valid registry type\n",
                    codeid(), key_type);
    }
    return rc;
}






static bool
dir_regkeyval_add(dir_t *dir, const value_t *name, const value_t *value)
{   dir_regkeyval_t *keydir = (dir_regkeyval_t *)dir;
    bool ok = FALSE;
    const char *namestr;
    size_t namestrl;
    const char *valstr;
    size_t valstrl;

    if (value_string_get(name, &namestr, &namestrl))
    {   long rc = ERROR_INVALID_DATA;
        REGVAL_NAMEVAR_DECLARE(valname_k);
        long rc_k = REGVAL_NAMEVAR_OPEN(valname_k, namestr, namestrl);

        if (ERROR_SUCCESS == rc_k)
        {   if (value == &value_null)
                rc = REGVAL_NAMEVAR_FN(RegDeleteValue)(keydir->key, valname_k);
            else
            {   if (value_type_equal(value, type_dir))
                {   dir_t *dirval = (dir_t *)value;
                    const value_t *typeval = dir_string_get(dirval, "type");
                    const value_t *dataval = dir_string_get(dirval, "data");
                    if (NULL == typeval)
                        fprintf(stderr, "%s: no \"type\" in registry value\n",
                                codeid());
                    else if (NULL == dataval)
                        fprintf(stderr, "%s: no \"data\" in registry value\n",
                                codeid());
                    else if (value_istype(typeval, type_int))
                        rc = dir_regkeyval_set(keydir->key, valname_k,
                                               typeval, dataval);
                } else
                /* give a binary value if set as a string */
                if (value_string_get(value, &valstr, &valstrl))
                    rc = REGVAL_NAMEVAR_FN(RegSetValueEx)(
                             keydir->key, valname_k, /*reserved*/0, REG_BINARY,
                             (const BYTE *)valstr, (DWORD)valstrl);
            }
            REGVAL_NAMEVAR_CLOSE(valname_k);
        }
        ok = (rc == ERROR_SUCCESS);
    }
    return ok;
}







static long /* rc */
dir_regkeyval_init(dir_regkeyval_t *keydir, key_t root,
                   const char *keyname, size_t keynamelen,
                   key_access_t access, bool on_heap)
{   long rc = win_regkeyval_open(root, keyname, keynamelen, access,
                                 &keydir->key);

    if (ERROR_SUCCESS == rc)
        dir_init(&keydir->dir, &type_dir_regkeyval_val,
                 &dir_regkeyval_add, /*lookup*/ NULL,
                 &dir_regkeyval_get, &dir_regkeyval_forall, on_heap);
    OMIT(else
              fprintf(stderr, "%s: failed to create registry key environment"
                      " - rc %d\n", codeid(), rc);)
    return rc;
}







static void
dir_regkeyval_delete(value_t *value)
{   if (value_istype(value, type_dir))
    {   dir_regkeyval_t *keydir = (dir_regkeyval_t *)value;
        win_regkeyval_close(&keydir->key);
        FTL_FREE(value);
    }
    /* else type error */
}







extern dir_t *
dir_regkeyval_new(key_t root, bool writeable,
               const char *keyname, size_t keynamelen)
{   dir_regkeyval_t *keydir = (dir_regkeyval_t *)
                              FTL_MALLOC(sizeof(dir_regkeyval_t));
    bool ok = (NULL != keydir);

    if (ok)
    {   if (dir_regkeyval_init(keydir, root, keyname, keynamelen,
                               writeable? regkeyval_ACCESS_READWRITE:
                                          regkeyval_ACCESS_READ,
                               /*on_heap*/TRUE) != ERROR_SUCCESS)
        {   FTL_FREE(keydir);
            keydir = NULL;
            ok = FALSE;
        }
    }

    if (ok)
    {   return dir_regkeyval_dir(keydir);
    } else
        return NULL;
}




#endif /* HAS_REGISTRY */










/*****************************************************************************
 *                                                                           *
 *          Stacked Directory Directories                                    *
 *          =============================                                    *
 *                                                                           *
 *****************************************************************************/










/* This is a stack of directories which use the "env_end" field in a    */
/* directory to determine how long a prefix chain of directories are to */
/* be used for finding symbols.  Subsequent directories can             */
/* eventually be "popped" and will be kept from the garbage collector   */
/* but are not used to return values from.                              */



struct dir_stack_s
{   dir_t dir;
    dir_t *stack;
} /* dir_stack_t */;






#define dir_stack_value(stack) dir_value(&((stack)->dir))
#define dir_stack_dir(stack) (&((stack)->dir))






static void
dir_stack_markver(const value_t *value, int heap_version)
{   dir_stack_t *dirstack = (dir_stack_t *)value;

    if (PTRVALID(value))
    {   value_t *dir = &dirstack->stack->value;

        while (PTRVALID(dir))
        {   value_mark_version(dir, heap_version);
            dir = dir->link;
        }
    }
}







static bool
dir_stack_push_at_pos(dir_stack_pos_t pos, dir_t *newdir, bool env_end)
{   if (PTRVALID(pos) &&
        PTRVALID(newdir))
    {   DEBUG_VALLINK(DPRINTF("%p: stack push at pos in %s\n",
                             &newdir->value.link,
                             value_type_name(&newdir->value)););
        newdir->value.link = *pos;
        dir_env_end_set(newdir, env_end);
        DEBUG_DIR(DPRINTF("%s: dir_stack - set end in dir %p\n",
                         codeid(), newdir);)
        *pos = dir_value(newdir);
        value_unlocal(dir_value(newdir));
        return TRUE;
    } else
        return FALSE;
}







extern dir_stack_pos_t /*return*/
dir_stack_push(dir_stack_t *dir, dir_t *newdir, bool env_end)
{   dir_stack_pos_t return_pos;

    if (NULL != dir && NULL != newdir)
    {   return_pos = &newdir->value.link;
        DEBUG_VALLINK(DPRINTF("%p: stack push dir in %s\n",
                             &newdir->value.link,
                             value_type_name(&newdir->value)););
        newdir->value.link = dir_value(dir->stack);
        dir_env_end_set(newdir, env_end);
        dir->stack = newdir;
        value_unlocal(dir_value(newdir));
    } else
        return_pos = NULL;

    return return_pos;
}





extern dir_stack_pos_t
dir_stack_last_pos(dir_stack_t *dir)
{   if (NULL != dir && NULL != dir->stack)
        return &dir->stack->value.link;
    else
        return NULL;
}





static dir_stack_pos_t
dir_stack_top_pos(dir_stack_t *dir)
{   if (NULL != dir)
        return (void*)&dir->stack;
    else
        return NULL;
}





extern dir_stack_pos_t
dir_stack_pos_enclosing(dir_stack_t *dir, dir_t *innerdir)
{   if (NULL != innerdir)
        return &innerdir->value.link;
    else
        return NULL;
}





STATIC_INLINE dir_t *
dir_at_stack_pos(dir_stack_pos_t dir_pos)
{   if (DIR_STACK_POS_BAD != dir_pos)
        return (dir_t *)(*(dir_pos));
    else
        return NULL;
}






extern void
dir_stack_return(dir_stack_t *dir, dir_stack_pos_t pos)
{   /* we could check that pos is in the directory stack here - it always
       must be - even if we don't want to see outer environments.
       This is because we need to keep a handle on (even invisible) outer
       directories so that the garbage collector does not delete them
    */
    if (NULL != dir && NULL != pos)
        dir->stack = dir_at_stack_pos(pos);
}







static dir_t *
dir_stack_top(dir_stack_t *dir)
{   if (NULL != dir)
        return dir->stack;
    else
        return NULL;
}






extern dir_t *
dir_stack_pop(dir_stack_t *dir)
{   if (NULL != dir && NULL != dir->stack)
    {   dir_t *popped = dir->stack;
        dir->stack = (dir_t *)popped->value.link;
        return popped;
    }  else
        return NULL;
}






static bool
dir_stack_add(dir_t *basedir, const value_t *name, const value_t *value)
{   dir_stack_t *dirstack = (dir_stack_t *)basedir;
    bool ok = FALSE;

    if (NULL != dirstack)
    {   dir_t *dir = dirstack->stack;

        if (NULL != dir && PTRVALID(dir->add))
            ok = (*dir->add)(dir, name, value);
        else
        {   /* create a new dir if top directory missing or unwriteable */
            dir_stack_push(dirstack, dir_id_new(), /*env_end*/FALSE);
            dir = dirstack->stack;
            if (NULL != dir && PTRVALID(dir->add))
                ok = (*dir->add)(dir, name, value);
        }
    }
    return ok;
}





static const value_t **
dir_stack_lookup(dir_t *basedir, const value_t *name)
{   dir_stack_t *dirstack = (dir_stack_t *)basedir;
    const value_t **ref_val = NULL;;

    if (NULL != dirstack && NULL != dirstack->stack)
    {   dir_t *dir = dirstack->stack;

        ftl_assert(dir != basedir);
        DEBUG_DIR(printf("%s: dir_stack - first lookup name %p in dir %p\n",
                        codeid(), name, dir);)
        /* find a directory where this variable is set */
        /* TODO: need to take account of dirs with "get" only (e.g. sys.env) */
        while (PTRVALID(dir) &&
               (dir->lookup == NULL ||
                NULL == (ref_val = (*dir->lookup)(dir, name))))
        {   dir_t *nextdir = (dir_t *)dir->value.link;

            ftl_assert(dir != (dir_t *)dir->value.link);
            if (PTRVALID(nextdir) && dir_env_end(nextdir))
            {   DEBUG_DIR(DPRINTF("** lookup blocked **\n");)
                dir = NULL;
            } else
                dir = nextdir;

            DEBUG_DIR(printf("%s: dir_stack - next  lookup name %p in dir %p\n",
                            codeid(), name, dir);)
        }
    }
    return ref_val;
}





static const value_t *
dir_stack_get(dir_t *basedir, const value_t *name)
{   dir_stack_t *dirstack = (dir_stack_t *)basedir;
    const value_t *val = NULL;

    if (NULL != dirstack && NULL != dirstack->stack)
    {   dir_t *dir = dirstack->stack;

        if (dir != basedir)
        {   DEBUG_DIR(DPRINTF("%s: dir_stack - first get name %p (%s) "
                             "in dir %p (%s)\n",
                             codeid(), name, value_type_name(name),
                             dir, value_type_name(dir_value(dir)));)
            while (PTRVALID(dir) &&
                   (dir->get == NULL ||
                    NULL == (val = (*dir->get)(dir, name))))
            {   dir_t *nextdir = (dir_t *)dir->value.link;

                ftl_assert(dir != (dir_t *)dir->value.link);
                if (PTRVALID(nextdir) && dir_env_end(dir))
                {   DEBUG_DIR(DPRINTF("** get blocked **\n");)
                    dir = NULL;
                } else
                    dir = nextdir;

                DEBUG_DIR(printf("%s: dir_stack - next  get name %p (%s) "
                                "in dir %p (%s)\n",
                                codeid(), name, value_type_name(name),
                                dir, value_type_name(dir_value(dir)));)
            }
        } DEBUG_DIR(
            else DPRINTF("%s: dir_stack - first dir in stack %p (%s) "
                         "is itself\n",
                         codeid(), dir, value_type_name(dir_value(dir)));
        )
    }
    return val;
}





typedef struct
{   dir_t *dir;
    dir_enum_fn_t *enumfn;
    void *arg;
} dir_stack_forall_arg_t;




static value_type_t type_dir_stack_val;




/* This is tedious I know, but we have to check each name to ensure that
   it is the one in scope of the original directory                       */
static void *
dir_stack_enum(dir_t *dir, const value_t *name, const value_t *value,
               void *arg)
{   dir_stack_forall_arg_t *stackfor = (dir_stack_forall_arg_t *)arg;

    if (value == dir_get(stackfor->dir, name))
        return (*stackfor->enumfn)(dir, name, value, stackfor->arg);
    else
        return NULL; /* keep going */
}





static void *
dir_stack_forall(dir_t *basedir, dir_enum_fn_t *enumfn, void *arg)
{   dir_stack_t *dirstack = (dir_stack_t *)basedir;
    void *result = NULL;

    if (NULL != dirstack && NULL != dirstack->stack)
    {   dir_t *dir = dirstack->stack;
        dir_stack_forall_arg_t stackfor;

        stackfor.dir = basedir;
        stackfor.arg = arg;
        stackfor.enumfn = enumfn;

        while (NULL == result && PTRVALID(dir))
        {   result = dir_forall(dir, &dir_stack_enum, &stackfor);
            if (dir_env_end(dir))
                dir = NULL;
            else
                dir = (dir_t *)dir->value.link;
        }
    }

    return result;
}






static dir_t *
dir_stack_init(dir_stack_t *dirstack, type_t dir_stack_subtype, bool on_heap)
{   dir_init(&dirstack->dir, dir_stack_subtype,
             &dir_stack_add, &dir_stack_lookup, &dir_stack_get,
             &dir_stack_forall, on_heap);
    dirstack->stack = NULL;
    return &dirstack->dir;
}






#if 0 != DEBUG_VALINIT(1+)0
#define dir_stack_init(dirstack, dir_stack_subtype, on_heap) \
    dir_info((dir_stack_init)(dirstack, dir_stack_subtype, \
                              on_heap), __LINE__)
#endif






extern dir_t *
dir_stack_new(void)
{   dir_stack_t *dirstack = (dir_stack_t *)FTL_MALLOC(sizeof(dir_stack_t));

    if (NULL != dirstack)
        dir_stack_init(dirstack, &type_dir_stack_val, /*on_heap*/TRUE);

    return &dirstack->dir;
}






#if 0 != DEBUG_VALINIT(1+)0
#define dir_stack_new() \
    dir_info((dir_stack_new)(), __LINE__)
#endif






extern dir_t *
dir_stack_copyinit(dir_stack_t *dirstack, dir_stack_t *old)
{   if (NULL != dirstack)
        dirstack->stack = old->stack;

    return dir_stack_dir(dirstack);
}






extern dir_stack_t *
dir_stack_copy(dir_stack_t *old)
{   dir_stack_t *dirstack = (dir_stack_t *)FTL_MALLOC(sizeof(dir_stack_t));

    if (NULL != dirstack)
    {   dir_stack_init(dirstack, &type_dir_stack_val, /*on_heap*/TRUE);
        dir_stack_copyinit(dirstack, old);
    }
    return dirstack;
}
















/*****************************************************************************
 *                                                                           *
 *          Closure Environments                                             *
 *          ====================                                             *
 *                                                                           *
 *****************************************************************************/







struct value_env_s
{   dir_stack_t dirs;       /* stack of bound variables */
    value_t *unbound;       /* stack of unbound variables */
    /* value_t's have a 'link' field which is used to implement a stack */
} /* value_env_t */;




static value_type_t type_dir_env_val;





/* warning: these macros throw away information about the unbound variables */

#define value_env_value(env) dir_stack_value(&((env)->dirs))

#define value_env_dir(env) dir_stack_dir(&((env)->dirs))

#define value_env_dir_stack(env) &((env)->dirs)





typedef struct
{   outchar_t *out;
    const value_t *root;
    int len;
    int vals;
} dir_env_bind_print_arg_t;









static void *
value_env_bind_print(dir_t *dir, const value_t *name,
                     const value_t *value, void *arg)
{   dir_env_bind_print_arg_t *pr = (dir_env_bind_print_arg_t *)arg;
    if (pr->root == dir_value(dir))
        return dir;
    else
    {   if (pr->vals > 0)
            pr->len += outchar_printf(pr->out, ",");
        if (value_type_equal(name, type_string))
            pr->len += value_string_print_as_id(pr->out, pr->root, name);
        else
            pr->len += value_print(pr->out, pr->root, name);
        pr->len += outchar_printf(pr->out, "=");
        OMIT(pr->len += outchar_printf(pr->out, "(%s)#%p ",
                                         value_type_name(value), value);)
        pr->len += value_print(pr->out, pr->root, value);
        pr->vals++;
        return NULL;
    }
}






#if 0
static value_t *
value_list_lastbefore(const value_t *list, const value_t *end)
{   value_t *ptr = list;

    while (PTRVALID(ptr) && ptr->link != end)
        ptr = ptr->link;

    return ptr;
}
#endif






static int
value_env_print_relative(outchar_t *out,
                         const value_t *root, const value_t *value,
                         bool *out_root_relative)
{   dir_env_bind_print_arg_t pr;

    pr.out = out;
    pr.root = root;
    pr.len = 0;
    pr.vals = 0;

    if (value_istype(value, type_dir))
    {   value_env_t *envdir = (value_env_t *)value;
        dir_t *dir = value_env_dir(envdir);

        if (PTRVALID(dir->forall))
        {   value_t *unbound = envdir->unbound;
            value_t *found_root;

            pr.len += outchar_printf(out, "[");
            found_root = (*dir->forall)(dir, &value_env_bind_print, &pr);

            while (PTRVALID(unbound))
            {   if (pr.vals > 0)
                    pr.len += outchar_printf(out, ", ");
                if (value_type_equal(unbound, type_string))
                    pr.len += value_string_print_as_id(out, root, unbound);
                else
                    pr.len += value_print(out, root, unbound);
                unbound = unbound->link;
                pr.vals++;
            }

            pr.len += outchar_printf(out, "]");
            if (NULL == out_root_relative)
            {   if (PTRVALID(found_root))
                    pr.len += outchar_printf(out, ":$root");
            } else
                *out_root_relative = PTRVALID(found_root);
        }
    }

    return pr.len;
}







static int
value_env_print(outchar_t *out, const value_t *root, const value_t *value)
{   return value_env_print_relative(out, root, value, /*out_found_root*/NULL);
}








static void
value_env_unlocal(const value_env_t *envdir)
{   value_t *unbound = envdir->unbound;

    while (PTRVALID(unbound))
    {   value_unlocal(unbound);
        unbound = unbound->link;
    }
}





static void
value_env_markver(const value_t *value, int heap_version)
{   value_env_t *envdir = (value_env_t *)value;
    value_t *unbound = envdir->unbound;

    dir_stack_markver(&envdir->dirs.dir.value, heap_version);

    while (PTRVALID(unbound))
    {   value_mark_version(unbound, heap_version);
        unbound = unbound->link;
    }
}





static value_env_t *
value_env_init(value_env_t *env, bool on_heap)
{   dir_stack_init(&env->dirs, &type_dir_env_val, on_heap);
    env->unbound = NULL;

    return env;
}






extern value_env_t *
value_env_new(void)
{   value_env_t *envdir = (value_env_t *)FTL_MALLOC(sizeof(value_env_t));

    if (PTRVALID(envdir))
        return value_env_init(envdir, /*on_heap*/TRUE);
    else
        return NULL;
}






#if 0 != DEBUG_VALINIT(1+)0
#define value_env_new() \
    value_info((value_env_new)(), __LINE__)
#endif






extern value_env_t *
value_env_copy(value_env_t *envdir_from)
{   value_env_t *envdir_to = value_env_new();

    if (PTRVALID(envdir_to))
    {   dir_stack_copyinit(&envdir_to->dirs, &envdir_from->dirs);
        envdir_to->unbound = envdir_from->unbound;
    }

    return envdir_to;
}






extern void
value_env_pushdir(value_env_t *env, dir_t *newdir, bool env_end)
{   if (PTRVALID(env))
    {   OMIT(
            printf("%s: push onto stack %p %s ",
                   codeid(), &env->dirs,
                   value_type_name(dir_value(newdir)));
            DIR_SHOW("", newdir);
        )
        dir_stack_push(&env->dirs, newdir, env_end);
    }
}






/* make a new value_env dir_t with only one directory on it
 *
 *  e.g. valuable in providing a dir_t with a spare link field albeit with the
 *  same values as the pushed directory
 *
 *  Mark directory as last visible directory at the top of the list if env_end
 *  is TRUE
 */
extern value_env_t *
value_env_newpushdir(dir_t *newdir, bool env_end, value_t *unbound)
{   value_env_t *env = value_env_new();
    env->unbound = unbound;
    value_env_pushdir(env, newdir, env_end);
    return env;
}






extern bool
value_env_pushenv(value_env_t *env, value_env_t *newenv, bool env_end)
{   bool ok = TRUE;
    if (PTRVALID(env))
    {   if (PTRVALID(env->unbound))
            ok = FALSE; /* can't add to env with unbound variables */
        else
        {   dir_stack_push(&env->dirs, value_env_dir(newenv), env_end);
            env->unbound = newenv->unbound;
        }
    }
    return ok;
}





/* Add a new unbound variable to environment or to end of environment
   queue (returned by a previous invocation)
   Return position where new variables should be added
 */
extern value_t * /*pos*/
value_env_pushunbound(value_env_t *env, value_t *pos, value_t *name)
{   if (PTRVALID(name) &&
        PTRVALID(env))
    {   if (PTRVALID(pos))
        {   DEBUG_VALLINK(DPRINTF("%p: push unbound at pos in %s\n",
                                 &pos->link, value_type_name(pos)););
            pos->link = name;
        }
        else
        {   DEBUG_VALLINK(DPRINTF("%p: push unbound new in %s\n",
                                 &name->link, value_type_name(name)););
            name->link = env->unbound;
            env->unbound = name;
        }
        return name;
    } else
        return NULL;
}






STATIC_INLINE value_t *
value_env_unbound(value_env_t *env)
{   if (PTRVALID(env))
    {   return env->unbound;
    } else
        return NULL;
}







/*! return a envdir which has the first unbound variable set to the given value
 */
extern value_t *
value_env_bind(value_env_t *envdir, const value_t *value)
{   value_t *newenvdirval = NULL;
    const value_t *unbound = envdir->unbound;

    if (PTRVALID(unbound))
    {   value_env_t *newenvdir = value_env_new();
        if (PTRVALID(newenvdir))
        {   dir_t *localbind = dir_id_new();

            /* construct an environment with one fewer unbound names */
            newenvdir->unbound = unbound->link;
            newenvdir->dirs.stack = envdir->dirs.stack;

            if (PTRVALID(localbind))
            {   dir_set(localbind, unbound, value);
                OMIT(DIR_SHOW("\n** pushing env: ", localbind);
                     DIR_SHOW("\n** onto env: ", value_env_dir(newenvdir));)
                value_env_pushdir(newenvdir, localbind, /*env_end*/FALSE);
                OMIT(DIR_SHOW("\n** result: ", value_env_dir(newenvdir));)
                newenvdirval = value_env_value(newenvdir);

            } else
            {   /* will garbage collect localbind and envdir later */
                value_unlocal(value_env_value(newenvdir));
            }
        }
    }
    value_unlocal(value);  /* should be in the environment now */

    return newenvdirval;
}









#if 0 != DEBUG_VALINIT(1+)0
#define value_env_bind(envdir, value)                   \
    value_info((value_env_bind)(envdir, value), __LINE__)
#endif




/* accumulate sum of two envionments in the first */
static bool
value_env_sum(value_env_t **ref_baseenv, value_env_t *plusenv)
{   bool ok = TRUE;

    if (*ref_baseenv == NULL)
        *ref_baseenv = plusenv;

    else if (PTRVALID(plusenv))
    {
        dir_t *env = value_env_dir(plusenv);
        dir_t *baseenv_env = value_env_dir(*ref_baseenv);
        value_t *unbound = value_env_unbound(plusenv);
        value_t *baseenv_unbound = value_env_unbound(*ref_baseenv);

        DEBUG_ENV(DPRINTF("env sum: (%s+%s) + (%s+%s)\n",
                         baseenv_env==NULL?"":"dir",
                         baseenv_unbound==NULL?"":"unbound",
                         env==NULL?"":"dir", unbound==NULL?"":"unbound"););
        if (baseenv_env == NULL && baseenv_unbound == NULL)
            *ref_baseenv = value_env_copy(plusenv);
            /* nothing in the new environment! */
        else if (PTRVALID(unbound) &&
                 PTRVALID(baseenv_unbound)) {
            ok = FALSE; /* can't have unbound variables in both */
        } else {
            value_env_pushdir(*ref_baseenv, env, /*env_end*/FALSE);
            if (baseenv_unbound == NULL)
                (*ref_baseenv)->unbound = unbound;
        }
    }

    return ok;
}







/*****************************************************************************
 *                                                                           *
 *          Directory Values                                                 *
 *          ================                                                 *
 *                                                                           *
 *****************************************************************************/





static void
values_dir_init(void)
{
    type_id_t dir_type_id = type_id_new();

    /* type_dir correspond to dir_id */
    type_init(&type_dir_id_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_print, NULL /* &dir_parse */,
              &dir_compare, &dir_id_delete,
              &dir_id_markver);

    type_init(&type_dir_vec_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_vec_print, /*&value_dir_parse*/NULL,
              &dir_compare, &dir_vec_delete,
              &dir_vec_markver);

    type_init(&type_dir_dyn_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, &value_delete_alloced/* never used? */,
              &dir_dyn_markver);

    type_init(&type_dir_join_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, &dir_join_delete,
              &dir_join_markver);

    type_init(&type_dir_struct_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, &dir_struct_delete,
              &dir_struct_markver);

    type_init(&type_dir_dynseq_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_vec_print, /*&dir_parse*/NULL,
              &dir_compare, &dir_dynseq_delete,
              /*markver*/ NULL);

    type_init(&type_dir_argvec_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, &value_delete_alloced/* never used */,
              /*markver*/ NULL);

    type_init(&type_dir_array_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, &dir_array_delete, &dir_array_markver);

    type_init(&type_dir_series_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_series_print, /*&dir_parse*/NULL,
              &dir_compare, &value_delete_alloced,
              /*markver*/ NULL);

    type_init(&type_dir_sysenv_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, &value_delete_alloced/* never used?*/,
              /*markver*/ NULL);

#if 0
    type_init(&type_dir_sysenv_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, /*&dir_sysenv_delete*/NULL,
              /*&dir_sysenv_markver*/NULL);
#endif

    type_init(&type_dir_lib_val, /*on_heap*/FALSE,
              dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, &dir_lib_delete, &dir_lib_markver);

#ifdef HAS_REGISTRY
    type_init(&type_dir_regkeyval_val, /*on_heap*/FALSE,
              dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, &value_delete_alloced,
              /*markver*/ NULL);
#endif

    type_init(&type_dir_stack_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &dir_print, /*&dir_parse*/NULL,
              &dir_compare, &value_delete_alloced,
              &dir_stack_markver);

    type_init(&type_dir_env_val, /*on_heap*/FALSE, dir_type_id, "dir",
              &value_env_print, /*&dir_parse*/NULL,
              &dir_compare, &value_delete_alloced,
              &value_env_markver);
}






/*****************************************************************************
 *                                                                           *
 *          Closure Values                                                   *
 *          ==============                                                   *
 *                                                                           *
 *****************************************************************************/







typedef struct value_closure_s value_closure_t;

struct value_closure_s
{   value_t value;           /* closure used as a value */
    const value_t *code;     /* code body */
    value_env_t *env;        /* environment */
} /* value_closure_t */;



#define value_closure_value(closure) (&(closure)->value)



static value_type_t type_closure_val;
type_t type_closure = &type_closure_val;






static void
value_closure_markver(const value_t *value, int heap_version)
{   value_closure_t *closure = (value_closure_t *)value;
    value_mark_version((value_t *)/*unconst*/closure->code, heap_version);
    value_mark_version(value_env_value(closure->env), heap_version);
}






static void
value_closure_delete(value_t *value)
{   if (value_istype(value, type_closure))
    {   value_closure_t *closure = (value_closure_t *)value;
        closure->code = NULL; /* should be garbage collected */
        closure->env = NULL;  /* should be garbage collected */
        FTL_FREE(closure);
    }
    /* else type error */
}








static int value_closure_print(outchar_t *out,
                               const value_t *root, const value_t *value)
{   size_t n = 0;
    value_closure_t *closure = (value_closure_t *)value;

    /*n = outchar_printf(out, "(");*/
    if (PTRVALID(closure->env))
    {   bool relative_to_root = FALSE;
        n += value_env_print_relative(out, root, value_env_value(closure->env),
                                      &relative_to_root);
        if (NULL == closure->code)
        {   if (relative_to_root)
                n += outchar_printf(out, ":$root");
        } else {
            if (relative_to_root)
                n += outchar_printf(out, ":");
            else
                n += outchar_printf(out, "::");
        }
    }
    if (PTRVALID(closure->code))
        n += value_print(out, root, closure->code);
    /*n += outchar_printf(out, ")");*/

    return (int)n;
}








static int /* <0 for less than, ==0 for equal, >0 for greater */
value_closure_compare(const value_t *v1, const value_t *v2)
{   return -1; /* TODO: implement me */
}






static value_t *
value_closure_init(value_closure_t *closure, type_t closure_type,
                   const value_t *code, value_env_t *env, bool on_heap)
{   value_t *initval;
    closure->code = code;
    closure->env = env;
    initval = value_init(&closure->value, closure_type, on_heap);
    value_unlocal(value_env_value(env));
    value_unlocal(code);
    return initval;
}







extern value_t *
value_closure_new(const value_t *code, value_env_t *env)
{   value_t *closureval = NULL;

    if (NULL == code || value_is_codebody(code))
    {   value_closure_t *closure = (value_closure_t *)
                                   FTL_MALLOC(sizeof(value_closure_t));
        if (PTRVALID(closure))
            closureval = value_closure_init(closure, type_closure, code, env,
                                            /*on_heap*/true);
    } else
    {   fprintf(stderr, "%s: closure code has wrong type - "
                 "type is %s, expected code or builtin\n",
                 codeid(), value_type_name(code));
    }

    if (NULL == closureval)
    {   value_unlocal(code);
        value_env_unlocal(env);
    }
    
    return closureval;
}






#if 0 != DEBUG_VALINIT(1+)0
#define value_closure_new(code, env)                    \
    value_info((value_closure_new)(code, env), __LINE__)
#endif






extern value_t *
value_closure_copy(const value_t *oldclosureval)
{   value_closure_t *closure = NULL;

    if (value_istype(oldclosureval, type_closure))
    {   value_closure_t *oldclosure = (value_closure_t *)oldclosureval;

        closure = (value_closure_t *)FTL_MALLOC(sizeof(value_closure_t));

        if (PTRVALID(closure))
        {   value_closure_init(closure, type_closure, /*code*/NULL,
                               value_env_copy(oldclosure->env),
                               /*on_heap*/TRUE);
        }
    }
    return closure==NULL? NULL: &closure->value;
}







/* return a value_t for the value_env_t in the closure */
static const value_t *
value_closure_env(const value_t *value)
{   return value_istype(value, type_closure)?
           value_env_value(((value_closure_t *)value)->env): NULL;
}







static const value_t *
value_closure_unbound(const value_t *value)
{   return value_istype(value, type_closure)?
               value_env_unbound(((value_closure_t *)value)->env):
               (const value_t *)NULL;
}







/* we must be sure the value given is a closure before using this */
extern bool
value_closure_pushdir(const value_t *value, dir_t *dir, bool env_end)
{   bool ok = FALSE;
    if (PTRVALID(dir))
    {   value_closure_t *closure = (value_closure_t *)value;

        ok = TRUE;
        if (NULL == closure->env)
            closure->env = value_env_new();

        if (PTRVALID(closure->env))
            value_env_pushdir(closure->env, dir, env_end);
    }
    return ok;
}





extern bool
value_closure_pushenv(const value_t *value, value_env_t *env, bool env_end)
{   bool ok = FALSE;
    if (PTRVALID(env))
    {   value_closure_t *closure = (value_closure_t *)value;

        ok = TRUE;
        if (NULL == closure->env)
        {   closure->env = value_env_copy(env);
        } else
            ok = value_env_pushenv(closure->env, env, env_end);
    }
    OMIT(else printf("%s: NULL value onto closure failed\n", codeid());)

    return ok;
}





extern bool
value_closure_get(const value_t *value, const value_t **out_code,
                  dir_t **out_env, const value_t **out_unbound)
{   if (value_istype(value, type_closure))
    {   const value_closure_t *closure = (const value_closure_t *)value;
        *out_code = closure->code;
        *out_env  = value_env_dir(closure->env);
        *out_unbound = value_env_unbound(closure->env);
        return TRUE;
    } else
    {   *out_code = NULL;
        *out_env  = NULL;
        *out_unbound = NULL;
        return FALSE;
    }
}







extern value_t * /*pos*/
value_closure_pushunbound(value_t *value, value_t *pos, value_t *name)
{   if (value_istype(value, type_closure))
    {   value_closure_t *closure = (value_closure_t *)value;
        return value_env_pushunbound(closure->env, pos, name);
    } else
        return NULL;
}






#if 0 /* UNUSED */
static bool
value_closure_setcode(value_t *value, const value_t *code)
{   bool ok = FALSE;
    if (value_istype(value, type_closure))
    {   value_closure_t *closure = (value_closure_t *)value;
        if (NULL == closure->code)
        {   closure->code = code;
            ok = TRUE;
        }
    }
    return ok;
}
#endif







extern value_t *
value_closure_bind(const value_t *closureval, const value_t *value)
{   if (value_istype(closureval, type_closure))
    {   value_closure_t *closure = (value_closure_t *)closureval;
        return value_closure_new(closure->code,
                                 (value_env_t *)value_env_bind(closure->env,
                                                               value));
    } else
    {   value_unlocal(value);
        return NULL;
    }
}





extern int
value_closure_argcount(const value_t *closureval)
{
    const value_t *code = NULL;
    dir_t *env = NULL;
    const value_t *unbound = NULL;
    int n = 0;

    if (value_closure_get(closureval, &code, &env, &unbound))
    {
        const value_t *nameval = unbound;
        while (PTRVALID(nameval)) {
            n++;
            nameval = nameval->link;
        }
    }
    return n;
}





#if 0 != DEBUG_VALINIT(1+)0
#define value_closure_bind(closure, val)                        \
    value_info((value_closure_bind)(closure, val), __LINE__)
#endif









extern void
values_closure_init(void)
{
    type_id_t closure_type_id = type_id_new();

    type_init(&type_closure_val, /*on_heap*/FALSE,
              closure_type_id, "closure",
              &value_closure_print, /*&value_closure_parse*/NULL,
              &value_closure_compare, &value_closure_delete,
              &value_closure_markver);
}








/*****************************************************************************
 *                                                                           *
 *          Transfer Functions                                               *
 *          ==================                                               *
 *                                                                           *
 *****************************************************************************/






extern bool
value_to_dir(const value_t *val, dir_t **out_dir)
{   if (value_type_equal(val, type_dir))
    {   *out_dir = (dir_t *)val;
        return TRUE;
    } else
    if (value_type_equal(val, type_closure))
    {   const value_t *unbound;
        const value_t *code;
        dir_t *env = NULL;
        (void)value_closure_get(val, &code, &env, &unbound);
        *out_dir = env; /* NB: this may be NULL */
        OMIT(if (NULL == *out_dir)
                   printf("%s: closure has empty env dir\n", codeid());)
        return TRUE;
    } else
        return FALSE;
}






extern bool
value_as_dir(const value_t *val, dir_t **out_dir)
{   if (!value_to_dir(val, out_dir))
    {   fprintf(stderr, "%s: value has wrong type - type is %s, "
                "expected directory or closure\n",
                codeid(), value_type_name(val));
        return FALSE;
    } else
        return TRUE;
}









/*****************************************************************************
 *                                                                           *
 *          Coroutine Values                                                 *
 *          ================                                                 *
 *                                                                           *
 *****************************************************************************/








struct value_coroutine_s
{   value_t value;              /* This can be a value */
    linesource_t source;        /* current input stream of lines to interpret */
    dir_stack_t *env;           /* stack of enclosing directories defining the
                                   current environment in which values should
                                   be looked up */
    dir_t *root;                /* a directory containing main namespace */
    dir_t *opdefs;              /* operator definitions in force */
    suspend_fn_t *sleep;        /* function used to sleep for n ms */
    int errors;                 /* count of errors */
    FILE *echo_log;             /* the stream to use if commands to be echoed */
    const char *echo_fmt;       /* the format to echo a line with */
    jmp_buf *catch_pos;         /* longjmp() dest for outer catch{} */
    const value_t *catch_arg;   /* argument to exception handler */
} /* value_coroutine_t */;

/* typedef value_coroutine_t parser_state_t; */




#define value_coroutine_value(co) (&(co)->value)




static value_type_t type_coroutine_val;
type_t type_coroutine = &type_coroutine_val;





static void
value_coroutine_delete(value_t *value)
{   parser_state_t *state = (parser_state_t *)value;
    /* close source down */
    if (PTRVALID(state))
        FTL_FREE(state);
}





static void
value_coroutine_markver(const value_t *value, int heap_version)
{   parser_state_t *state = (parser_state_t *)value;
    value_mark_version(dir_stack_value(state->env), heap_version);
    value_mark_version(dir_value(state->root), heap_version);
    value_mark_version(dir_value(state->opdefs), heap_version);
}





extern charsource_t *
parser_charsource(parser_state_t *parser_state)
{   return instack_charsource(linesource_instack(&(parser_state)->source));
}




extern int
parser_lineno(parser_state_t *parser_state)
{   return instack_lineno(linesource_instack(&(parser_state)->source));
}



extern const char *
parser_source(parser_state_t *parser_state)
{   return instack_source(linesource_instack(&(parser_state)->source));
}





static int
value_coroutine_print(outchar_t *out,
                      const value_t *root, const value_t *value)
{   int outlen=0;
    if (value_istype(value, type_coroutine))
    {   parser_state_t *state = (parser_state_t *)value;
        outlen += outchar_printf(out, "$coroutine.{in=%s:%d}",
                                 parser_source(state), parser_lineno(state));
        /*outlen += value_print(out, root, dir_stack_value(state->env));
          outlen += outchar_putc(out, '}');*/
    }
    return outlen;
}






static const value_t *
value_coroutine_init(parser_state_t *state, dir_stack_t *env, dir_t *root,
                     dir_t *opdefs, bool on_heap)
{   const value_t *val =
        value_init(&state->value, type_coroutine, on_heap);
    linesource_init(&state->source);
    state->env = env;
    state->echo_log = NULL;
    state->echo_fmt = NULL;
    state->root = root;
    state->opdefs = opdefs;
    state->sleep = &sleep_ms;
    state->errors = 0;
    state->catch_pos = NULL;  /* no outer exception */
    state->catch_arg = NULL;
    parser_env_push(state, root, /*outer_visible*/FALSE);
    value_unlocal(dir_value(root));
    value_unlocal(dir_stack_value(env));
    return val;
}






extern value_coroutine_t *
value_coroutine_new(dir_t *root, dir_stack_t *env, dir_t *opdefs)
{   parser_state_t *state = (parser_state_t *)
                            FTL_MALLOC(sizeof(parser_state_t));
    if (PTRVALID(state))
        value_coroutine_init(state, env, root, opdefs, /*on_heap*/TRUE);
    return state;
}







extern void
values_coroutine_init(void)
{
    type_id_t coroutine_type_id = type_id_new();

    type_init(&type_coroutine_val, /*on_heap*/FALSE,
              coroutine_type_id, "coroutine",
              &value_coroutine_print,
              /*&value_closure_parse*/NULL,
              /*&value_coroutine_compare*/NULL,
              &value_coroutine_delete,
              &value_coroutine_markver);
}











/*****************************************************************************
 *                                                                           *
 *          Parser State                                                     *
 *          ============                                                     *
 *                                                                           *
 *****************************************************************************/








#define parser_state_value(state) value_coroutine_value(state)



extern void
parser_state_free(parser_state_t *state)
{   /* do nothing - rely on garbage collection */
}





extern parser_state_t *
parser_state_new(dir_t *root)
{   return value_coroutine_new(root, (dir_stack_t *)dir_stack_new(),
                               /*opdefs*/dir_vec_new());
}




/* forward reference */
static bool
parse_cmdlist(const char **ref_line,
              parser_state_t *state, const value_t **out_val);


extern linesource_t *
parser_linesource(parser_state_t *parser_state)
{   return (&(parser_state)->source);
}

extern dir_t *
parser_root(parser_state_t *parser_state)
{   return (parser_state)->root;
}

extern void
parser_root_set(parser_state_t *parser_state, dir_t *dir)
{   (parser_state)->root = dir;
}

extern dir_stack_t *
parser_env_stack(const parser_state_t *parser_state)
{   return (parser_state)->env;
}

extern dir_t *
parser_env(parser_state_t *parser_state)
{   return dir_stack_dir((parser_state)->env);
}

extern dir_t *
parser_env_copy(parser_state_t *parser_state)
{   dir_stack_t *newstack = dir_stack_copy((parser_state)->env);
    return dir_stack_dir(newstack);
}

extern dir_t *
parser_opdefs(const parser_state_t *parser_state)
{   return (parser_state)->opdefs;
}

extern const value_t *
parser_builtin_arg(parser_state_t *parser_state, int argno)
{   return dir_get_builtin_arg(dir_stack_dir((parser_state)->env), argno);
}


extern dir_stack_pos_t
parser_env_push(parser_state_t *parser_state, dir_t *newdir, bool outer_visible)
{   return dir_stack_push((parser_state)->env, newdir, outer_visible);
}


extern bool /* ok */
parser_env_push_at_pos(parser_state_t *parser_state, dir_stack_pos_t pos,
                       dir_t *newdir, bool outer_visible)
{   return dir_stack_push_at_pos(pos, newdir, outer_visible);
}


extern void
parser_env_return(parser_state_t *parser_state, dir_stack_pos_t pos)
{   dir_stack_return((parser_state)->env, pos);
}


extern dir_stack_pos_t
parser_env_calling_pos(const parser_state_t *parser_state)
{   return dir_stack_last_pos((parser_state)->env);
}


extern bool
parser_echoto(const parser_state_t *parser_state,
              FILE **out_log, const char **out_fmt)
{   if (PTRVALID(out_log))
        *out_log = parser_state->echo_log;
    if (PTRVALID(out_fmt))
        *out_fmt = parser_state->echo_fmt;
    return PTRVALID(parser_state->echo_log);
}


extern void
parser_echo_setlog(parser_state_t *parser_state,
                   FILE *log, const char *echo_fmt)
{   parser_state->echo_log = log;
    parser_state->echo_fmt = echo_fmt;
}

extern suspend_fn_t *
parser_suspend_get(const parser_state_t *parser_state)
{   return (parser_state)->sleep;
}

extern void
parser_suspend_set(parser_state_t *parser_state, suspend_fn_t *sleep)
{   (parser_state)->sleep = sleep;
}


/* reports error with backtrace */
extern int
charsink_parser_vreport(charsink_t *sink, parser_state_t *parser_state,
                        const char *format, va_list ap)
{   return vreport(sink,
                   NULL==parser_state? NULL: parser_linesource(parser_state),
                   format, ap);
}


/* omits initial backtrace - for report continuation lines */
extern int
charsink_parser_vreport_line(charsink_t *sink, parser_state_t *parser_state,
                             const char *format, va_list ap)
{   return
        vreport_line(sink,
                     NULL==parser_state? NULL: parser_linesource(parser_state),
                     format, ap);
}



extern int
charsink_parser_value_print(charsink_t *sink, parser_state_t *parser_state,
                            const value_t *val)
{   if (PTRVALID(sink))
        return value_print(sink, dir_value(parser_root(parser_state)), val);
    else
        return value_fprint(stderr, dir_value(parser_root(parser_state)), val);
}


/* generate initial line of error report */
extern int
parser_report(parser_state_t *parser_state, const char *format, ...)
{   va_list args;
    int len;

    va_start(args, format);
    len = parser_vreport(parser_state, format, args);
    va_end(args);

    return len;
}




/* generate subsequent lines of error report (no backtrace) */
extern int
parser_report_line(parser_state_t *parser_state, const char *format, ...)
{   va_list args;
    int len;

    va_start(args, format);
    len = parser_vreport_line(parser_state, format, args);
    va_end(args);

    return len;
}




static void
parser_error_longstring(parser_state_t *state, const char *line,
                         const char *msg)
{   const char *p  = strchr(line,'\n');
    if (p == NULL)
        parser_error(state, "%s '%s'\n", msg, line);
    else {
        int n = FTL_ERROR_TRAIL_LINES;
        parser_error(state, "%s '%.*s\n", msg, p-line, line);
        do {
            line = p+1;
            p  = strchr(line,'\n');
            if (p == NULL)
                parser_report_line(/*state*/NULL/*no pos*/, "%s'\n", line);
            else
                parser_report_line(/*state*/NULL/*no pos*/, "%.*s%s\n",
                                   p-line, line, n==1?"...":"");
        } while (--n > 0 && PTRVALID(p));
    }
}



extern int
parser_error_count(parser_state_t *parser_state)
{   return parser_state->errors;
}



STATIC_INLINE void
parser_error_new(parser_state_t *parser_state)
{   parser_state->errors++;
}



STATIC_INLINE void
parser_errors_reset(parser_state_t *parser_state, int error_count)
{   parser_state->errors = error_count;
}



extern int
parser_error(parser_state_t *parser_state, const char *format, ...)
{   va_list args;
    int len;

    va_start(args, format);
    len = parser_vreport(parser_state, format, args);
    va_end(args);
    parser_error_new(parser_state);

    return len;
}



extern const value_t *
parser_errorval(parser_state_t *parser_state, const char *format, ...)
{   va_list args;
    charsink_string_t stream;
    charsink_t *sink = charsink_string_init(&stream);
    const char *body;
    size_t blen;
    const value_t *res;

    va_start(args, format);
    (void)charsink_parser_vreport(sink, parser_state, format, args);
    va_end(args);

    charsink_string_buf(sink, &body, &blen);
    res = value_string_new(body, blen);
    charsink_string_close(sink);

    parser_error_new(parser_state);

    return res;
}



static const char *
parser_help_text(const value_t *cmd)
{   if (cmd == NULL)
        return NULL;
    else {
        type_t val_type = value_type(cmd);

        if (type_equal(val_type, type_cmd))
            return value_cmd_help(cmd);

        else if (type_equal(val_type, type_func))
            return value_func_help(cmd);

        else if (type_equal(val_type, type_closure))
        {   const value_t *code;
            dir_t *env;
            const value_t *helpval;
            const char *help;
            size_t len;
            const value_t *unbound;

            (void)value_closure_get(cmd, &code, &env, &unbound);
            helpval = dir_string_get(env, BUILTIN_HELP);
            if (NULL != helpval)
            {   value_string_get(helpval, &help, &len);
                return help;
            } else
                return NULL;
        }
        else
            return NULL;
    }
}





extern int
parser_report_help(parser_state_t *parser_state, const value_t *cmd)
{   const char *help = parser_help_text(cmd);

    if (NULL == help)
        help = "<unknown>";

    return parser_error(parser_state, "syntax - %s\n", help);
}




static void
parser_save(const parser_state_t *parser_state,
            parser_state_t *out_saved, dir_t **out_saved_stack)
{   *out_saved = *parser_state;
    *out_saved_stack = dir_stack_top(parser_env_stack(parser_state));
}




static void
parser_restore(parser_state_t *parser_state,
               const parser_state_t *saved, dir_t *saved_stack)
{   int errors = parser_state->errors;
    /*dir_stack_t *stack; */
    *parser_state = *saved;
    parser_state->errors = errors;
    /*stack = parser_env_stack(parser_state); */
    /*stack->stack = saved_stack; */
    parser_env_return(parser_state, (dir_stack_pos_t)&saved_stack);
}




extern bool
parser_throw(parser_state_t *parser_state, const value_t *exception)
{   bool ok = (PTRVALID(parser_state->catch_pos));

    if (ok) {
        parser_state->catch_arg = exception;
        longjmp(*parser_state->catch_pos, 1/* non-zero */);
    }

    return ok;
}



extern const value_t *
parser_catch_call(parser_state_t *state, parser_call_fn_t *call,
                  void *call_arg, bool *out_ok)
{
    parser_state_t saved_state;
    dir_t *saved_stack = NULL;
    jmp_buf except_place;
    int event;
    const value_t *val = &value_null;

    parser_save(state, &saved_state, &saved_stack);

    state->catch_pos = &except_place;
    state->catch_arg = NULL;

    event = setjmp(except_place);

    if (event == 0) {
        /* no event has occurred yet */
        val = (*call)(state, call_arg);

        /* restore outer values */
        state->catch_pos = saved_state.catch_pos;
        state->catch_arg = saved_state.catch_arg;
        *out_ok = TRUE;
    } else {
        /* longjmp() was called somewhere in the body of execute_code */
        val = state->catch_arg;
        parser_restore(state, &saved_state, saved_stack);
        *out_ok = FALSE;
    }
    return val;
}



static const value_t *parser_call_invoke(parser_state_t *state, void *call_arg)
{
    const value_t *code = (const value_t *)call_arg;
    return invoke(code, state);
}



extern const value_t *
parser_catch_invoke(parser_state_t *state, const value_t *code, bool *out_ok)
{
    return parser_catch_call(state, &parser_call_invoke, (void *)code, out_ok);
}





/* forward definitions */

extern bool parse_empty(const char **ref_line);
extern bool parse_space(const char **ref_line);





extern void
parser_collect(parser_state_t *state)
{   int heap_value;
    heap_value = value_heap_nextversion();
    /* should be able to drop the following soon */
    value_mark_version(parser_state_value(state), heap_value);
    OMIT(value_list_locals());
    value_mark_local(heap_value);
    value_heap_collect();
}





/* Deal with backslash line continuation and '$' variable expansion
 * output the resulting line to \c out
 * This function may cause a garbage collection
 */
extern outchar_t *
parser_expand(parser_state_t *state, outchar_t *out,
              const char *phrase, size_t len)
{   instack_t inpile;
    instack_t *in = instack_init(&inpile);
    charsource_string_t instring;
    charsource_t *inmain = charsource_string_init(&instring, /*delete*/NULL,
                                                  "<exp source>", phrase, len);
    int ch;
    int lastch = EOF;

    DEBUG_EXPD(printf("expanding '%s'[%d]\n", phrase, len);)
    instack_push(in, inmain);

    while (EOF != (ch = instack_getc(in)))
    {   if (ch == '\\' && lastch == '\\')
        {   outchar_putc(out, ch);
            ch = EOF; /* this is just to make lastch EOF next time round -
                         so it does not affect the interpretation of $ */
        } else if (ch == '$' && lastch != '\\')
        {   ch = instack_getc(in);
            if (ch == '$')
            {   /* parse $$ - escape for just '$' */
                outchar_putc(out, ch);
            } else
            {   char macname[FTL_ID_MAX+3];
                char *mac = &macname[0];
                const char *mac_r = &macname[0];
                size_t len = sizeof(macname);
                const value_t *macval = NULL;

                if (ch == '{')
                {   /* parse ${expr} - expansion of string resulting from
                     * expression */
                    ch = instack_getc(in);
                    while (ch != '}' && ch != EOF)
                    {   if (len > 1)
                        {   *mac++ = ch;
                            len--;
                        }
                        ch = instack_getc(in);
                    }
                } else
                if (ch  == '_'  || isalnum(ch))
                {   /* parse $name - macro expansion */
                    do {
                        if (len > 1)
                            *mac++ = ch;
                        ch = instack_getc(in);
                    } while (ch != EOF && (ch =='_' || isalnum(ch)));

                    instack_ungetc(in, ch);
                } else
                if (ch  == '@')
                {   /* parse $@<n> - what is this supposed to do? */
                    do {
                        if (len > 1)
                            *mac++ = ch;
                        ch = instack_getc(in);
                    } while (ch != EOF && isdigit(ch));

                    instack_ungetc(in, ch);
                }
                *mac = '\0';
                mac_r = &macname[0];
                if (parse_cmdlist(&mac_r, state, &macval))
                {   parse_space(&mac_r);
                    if (!parse_empty(&mac_r))
                        macval = NULL;
                }
                if (macval == NULL)
                    parser_report(state,
                                  "syntax error in $ expansion - \"%s\"\n",
                                  &macname[0]);
                else
                {   const char *body;
                    size_t blen;
                    charsource_t *inmac;

                    if (value_type_equal(macval, type_string))
                    {
                        /* include text as a macro - NB any '$' in the
                           expansion will themselves be treated as macros
                        */
                        value_string_get(macval, &body, &blen);
                        inmac = charsource_string_new(&macname[0], body, blen);
                        instack_push(in, inmac);
                    }
                    else
                    {   charsink_string_t charbuf;
                        charsink_t *sink = charsink_string_init(&charbuf);

                        value_print(sink, dir_value(parser_root(state)),
                                    macval);
                        charsink_string_buf(sink, &body, &blen);
                        OMIT(printf("mac body: '%s'[%d]\n", body, blen);)
                        macval = value_string_new(body, blen);
                        charsink_string_close(sink);

                        /* it's not a string - include the text literally */
                        value_string_get(macval, &body, &blen);
                        if (body != NULL)
                        {
                            const char *endbody = &body[blen];
                            while (body < endbody)
                            {   outchar_putc(out, *body);
                                body++;
                            }
                        }   
                    }
                }
            }
        } else
            outchar_putc(out, ch);

        lastch = ch;
    }

    return out;
}









/*****************************************************************************
 *                                                                           *
 *          Line Parsing                                                     *
 *          ============                                                     *
 *                                                                           *
 *****************************************************************************/








extern bool
parse_empty(const char **ref_line)
{   return **ref_line == '\0';
}




extern bool
parse_white(const char **ref_line)
{   const char *start = *ref_line;
    const char *line = start;

    while (isspace((unsigned char)*line)) {
        line++;
    }
    OMIT(printf("%s: %d white chars\n", codeid(), line-start););
    *ref_line = line;
    return line != start;
}





extern bool
parse_space(const char **ref_line)
{   (void)parse_white(ref_line);
    return TRUE;
}





extern bool
parse_key(const char **ref_line, const char *key)
{   const char *line = *ref_line;

    while (*key != '\0' && *line != '\0' && *line++ == *key)
        key++;

    if (*key == '\0')
    {
        *ref_line = line;
        return TRUE;
    } else
        return FALSE;
}





extern bool
parse_ending(const char **ref_line, const char *key)
{   const char *line = *ref_line;
    const char *end = strstr(line, key);

    if (PTRVALID(end))
    {
        *ref_line = end;
        return TRUE;
    } else
        return FALSE;
}





extern bool
parse_item(const char **ref_line, const char *delims, size_t ndelim,
           char *buf, size_t len)
{   const char *start = *ref_line;
    const char *line = start;

    if (delims == NULL)
        delims="";

    while (*line != '\0' && !isspace((unsigned char)*line) &&
           NULL == memchr(delims, *line, ndelim))
    {   if (len > 1)
        {   len--;
            *buf++ = *line;
        }
        line++;
    }

    if (len <= 0)
        return FALSE;
    else
    {   *buf = '\0';

        if (line != start)
        {   *ref_line = line;
            return TRUE;
        } else
            return FALSE;
    }
}






extern bool
parse_id(const char **ref_line, char *buf, size_t len)
{   const char *start = *ref_line;
    const char *line = start;
    char ch = *line;

#if BUILTIN_ARG_CH1 == '_'
    if (ch=='_' || isalnum((unsigned char)ch))
        do {
            if (len > 1)
            {   len--;
                *buf++ = ch;
            }
            line++;
        } while ((ch = *line) != '\0' &&
                 (ch=='_' || isalnum((unsigned char)ch)) );
#else
    if (ch==BUILTIN_ARG_CH1)
        do {
            if (len > 1)
            {   len--;
                *buf++ = ch;
            }
            line++;
        } while ((ch = *line) != '\0' && isdigit((unsigned char)ch));
    else
    if (ch=='_' || isalpha((unsigned char)ch))
        do {
            if (len > 1)
            {   len--;
                *buf++ = ch;
            }
            line++;
        } while ((ch = *line) != '\0' &&
                 (ch=='_' || isalnum((unsigned char)ch)) );
#endif

    if (len <= 0)
        return FALSE;
    else
    {   *buf = '\0';

        if (line != start)
        {   *ref_line = line;
            return TRUE;
        } else
            return FALSE;
    }
}





STATIC_INLINE char
parse_string_delim(const char *line, char *out_delim)
{   bool ok = TRUE;

    if (*line == '"')
        *out_delim = '"';
    else if (*line == '\'')
        *out_delim = '\'';
    /* else if (*line == '<')
     *out_delim = '>'; */
    else
        ok = FALSE;

    return ok;
}




extern bool
parse_string(const char **ref_line, char *buf, size_t size, size_t *out_len)
{   const char *start = *ref_line;
    const char *line = start;
    char end_delim = 0;

    if (!parse_string_delim(line, &end_delim))
        return FALSE;
    else
    {   bool escape = FALSE;
        int delim_depth = 0;
        char *bufstart = buf;

        line++;
        while (*line != '\0' &&
               !(!escape && *line == end_delim && delim_depth == 0))
        {   char ch = *line;
            wchar_t wch = 0;
            bool use_char = TRUE;
            bool use_wchar = FALSE;

            if (escape)
            {   switch (ch)
                {   case 'x':
                    {   unumber_t xch = 0;
                        line++;
                        if (parse_hex_width(&line, 2, &xch))
                            ch = (char)xch;
                        else
                            use_char = FALSE;
                        line--;
                        break;
                    }
                    case 'u':
                    {   unumber_t uch = 0;
                        line++;
                        use_char = FALSE;
                        if (parse_hex_width(&line, 4, &uch))
                        {   wch = (wchar_t)uch;
                            use_wchar = TRUE;
                        }
                        line--;
                        break;
                    }
                    default:
                        ch = escape_ch(ch);
                        break;
                }
                escape = FALSE;
            } else
            {   if (ch == '\\')
                {   escape = TRUE;
                    use_char = FALSE;
                } else
                if (ch == *start)
                    delim_depth++;
                else
                if (ch == end_delim)
                    delim_depth--;
            }

            if (use_char)
            {   if (size > 1)
                {   size--;
                    *buf++ = ch;
                }
            } else if (use_wchar)
            {   char mbstring[FTL_MB_LEN_MAX+1];
                size_t len;
                (void)wctomb(NULL, L'\0'); /* reset state */
                len = wctomb(&mbstring[0], wch);

                if ((int)len >= 0 && size > len && len < sizeof(mbstring))
                {   const char *str = &mbstring[0];
                    size -= len;
                    while (len > 0)
                    {   *buf++ = *str++;
                        len--;
                    }
                }
            }
            line++;
        }

        if (size > 0)
           *buf = '\0';

        if (*line != '\0')
        {   line++;
            *ref_line = line;
            if (NULL != out_len)
                *out_len = buf - bufstart;
            return TRUE;
        } else
        {   if (NULL != out_len)
                *out_len = 0;
            return FALSE;
        }
    }
}






/* Note: do not use the string accumulated if size==0, also it will be "short"
         if out_len < size-1
*/
/* This function may cause a garbage collection */
extern bool
parse_string_base(const char **ref_line, parser_state_t *state,
                  char *buf, size_t size, size_t *out_len)
{   /* syntax: (<string expression>) | <string> */
    bool ok;
    const char *line = *ref_line;

    if (parse_key(&line, "("))
    {   const value_t *strval;
        const char *valbuf;
        size_t vallen;

        parse_space(&line);
        ok = parse_expr(&line, state, &strval) &&
             value_istype(strval, type_string) &&
             parse_space(&line) &&
             parse_key(&line, ")");

        if (ok && value_string_get(strval, &valbuf, &vallen) && vallen <= size)
        {   memcpy(buf, valbuf, vallen);
            *out_len = vallen;
        } else
            *out_len = 0;
    } else
        ok = parse_string(&line, buf, size, out_len);

    if (ok)
        *ref_line = line;

    return ok;
}





/* This function may cause a garbage collection */
extern bool
parse_string_expr(const char **ref_line, parser_state_t *state,
                  char *buf, size_t size, const value_t **out_string)
{   /* char strbuf[FTL_STRING_MAX]; */
    size_t filled_len = 0;
    if (parse_string_base(ref_line, state, buf, size, &filled_len))
    {   const char *line;
        size_t initial_size = size;
        size_t total_len = filled_len;
        bool ok = TRUE;

        line = *ref_line;

        while (ok && parse_space(&line) && parse_key(&line, "+") &&
               parse_space(&line))
        {   if (parse_string_base(&line, state,
                                  buf+total_len,
                                  total_len >= size? 0: size-total_len,
                                  &filled_len))
            {   total_len += filled_len;
                *ref_line = line;
            } else
            {   parser_error(state, "string expected after '+'\n");
                ok = FALSE;
            }
        }

        if (initial_size <= 0)
            *out_string = &value_null;
        else
        {   if (ok && total_len == initial_size-1)
                parser_error(state, "internal limit - string may be "
                             "truncated to %d bytes\n", total_len);
            *out_string = value_string_new(buf, total_len);
        }
        return ok;
    } else
        return FALSE;
}







extern bool
parse_itemstr(const char **ref_line, char *buf, size_t len)
{   return parse_string(ref_line, buf, len, NULL) ||
        parse_item(ref_line, NULL, 0, buf, len);
}








typedef struct
{   const char **ref_line;
    parser_state_t *state;
    parse_match_fn_t *parse_match_fn;
    void *parse_arg;
} enum_parse_oneof_arg_t;







static void *
parse_oneof_exec(dir_t *dir, const value_t *name, const value_t *value,
                 void *arg)
{   enum_parse_oneof_arg_t *onearg = (enum_parse_oneof_arg_t *)arg;

    return (*onearg->parse_match_fn)(onearg->ref_line, onearg->state,
                                     name, onearg->parse_arg)? (void *)value:
                                                               NULL;
}








extern bool
parse_oneof_matching(const char **ref_line, parser_state_t *state,
                     dir_t *prefixes, const value_t **out_val,
                     parse_match_fn_t *match_fn, void *match_fn_arg)
{   enum_parse_oneof_arg_t arg;
    arg.ref_line = ref_line;
    arg.state = state;
    arg.parse_match_fn = match_fn;
    arg.parse_arg = match_fn_arg;
    *out_val = dir_forall(prefixes, &parse_oneof_exec, &arg);
    return (NULL != *out_val);
}





static bool
parse_match_string(const char **ref_line, parser_state_t *state,
                   const value_t *name, void *arg)
{   if (value_type_equal(name, type_string))
    {   const char *key;
        size_t keylen;
        return value_string_get(name, &key, &keylen) &&
               parse_key(ref_line, key);
    } else
        return FALSE;
}






extern bool
parse_oneof(const char **ref_line, parser_state_t *state,
            dir_t *prefixes, const value_t **out_val)
{   return parse_oneof_matching(ref_line, state, prefixes, out_val,
                                &parse_match_string, NULL);
}





static bool
parse_match_ending(const char **ref_line, parser_state_t *state,
                   const value_t *name, void *arg)
{   if (value_type_equal(name, type_string))
    {   const char *key;
        size_t keylen;
        return value_string_get(name, &key, &keylen) &&
               parse_ending(ref_line, key);
    } else
        return FALSE;
}






extern bool
parse_one_ending(const char **ref_line, parser_state_t *state,
                 dir_t *prefixes, const value_t **out_val)
{   return parse_oneof_matching(ref_line, state, prefixes, out_val,
                                &parse_match_ending, NULL);
}










/*****************************************************************************
 *                                                                           *
 *          Operator Parsing                                                 *
 *          ================                                                 *
 *                                                                           *
 *****************************************************************************/






/* Example test code:

set printf io.fprintf io.out
# unquoted value format
set fmt.t [f,p,v]:{ if (equal "string" (typename v!)!) {v} {str v!}!}
set opecho2[strf=strf,name,l,r]::{ strf "(%t %s %t)" <l, name, r>! }
set opecho1[strf=strf,name,lr]::{ strf "(%s %t)" <name, lr>! }

set ops <>
set opdef NULL
set opset[pri, as, name, fn]:{
    parse.opset ops pri parse.assoc.(as) name fn!;
    opdef = range ops!;
}
set opeval[expr]: { parse.opeval opdef expr! }

opset 5 "xfx" "~" (opecho2 "~")
opset 5 "xfx" "+" (opecho2 "+")
opset 6 "fy" "fy" (opecho1 "fy")
opset 6 "fx" "fx" (opecho1 "fx")
opset 6 "yfy" "yfy" (opecho2 "yfy")
opset 6 "xfx" "xfx" (opecho2 "xfx")
opset 6 "xfy" "xfy" (opecho2 "xfy")
opset 6 "yfx" "yfx" (opecho2 "yfx")
opset 6 "yf" "yf" (opecho1 "yf")
opset 6 "xf" "xf" (opecho1 "xf")

set x 42

opeval {yfy}       # fails
opeval {x}         # fails?
opeval {4}
opeval {"str"}
opeval {<3>}

opeval {5 xfy 6}
opeval {5 yfx 6}
opeval {5 yfy 6}
opeval {5 xfx 6}
opeval {fx 7}
opeval {fy 7}
opeval {4 xf}
opeval {4 yf}

opeval {5 xfy 6 xfy 7} # should give (5 xfy 6) xfy 7
opeval {5 yfx 6 yfx 7} # should give 5 xfy (6 xfy 7)
opeval {5 yfy 6 yfy 7} # should fail?
opeval {5 xfx 6 xfx 7} # should fail

opeval {fx fx 7} # should fail
opeval {fy fy 7}
opeval {4 xf xf}
opeval {4 yf yf}

opeval {fx 6 xfy 7} # should fail

opeval {fy 6 xfy 7}
opeval {fy 6 yfx 7}
opeval {fy 6 yfy 7}
opeval {fy 6 xfx 7}

opeval {6 xfy fx 7}
opeval {6 yfx fx 7} # should fail
opeval {6 yfy fx 7}
opeval {6 xfx fx 7} # should fail

opset 8 "fy" "Fy" (opecho1 "Fy")
opset 8 "fx" "Fx" (opecho1 "Fx")
opset 8 "yfy" "yFy" (opecho2 "yFy")
opset 8 "xfx" "xFx" (opecho2 "xFx")
opset 8 "xfy" "xFy" (opecho2 "xFy")
opset 8 "yfx" "yFx" (opecho2 "yFx")
opset 8 "yf" "yF" (opecho1 "yF")
opset 8 "xf" "xF" (opecho1 "xF")

opeval {5 xFy 6 yfy 7 xFy 8}
opeval {5 xFy 6 xfx 7 xFy 8}

opeval {5 yFx 6 xfy 7 yFx 8}
opeval {5 yFx 6 yfx 7 yFx 8}
opeval {5 yFx 6 yfy 7 yFx 8}
opeval {5 yFx 6 xfx 7 yFx 8}

opeval {5 yFy 6 xfy 7 yFy 8}
opeval {5 yFy 6 yfx 7 yFy 8}
opeval {5 yFy 6 yfy 7 yFy 8}
opeval {5 yFy 6 xfx 7 yFy 8}

opeval {5 xFx 6 xfy 7 xFx 8}
opeval {5 xFx 6 yfx 7 xFx 8}
opeval {5 xFx 6 yfy 7 xFx 8}
opeval {5 xFx 6 xfx 7 xFx 8}

opeval {Fx 6 xfy Fx 8}
opeval {Fx 6 yfx Fx 8}
opeval {Fx 6 yfy Fx 8}
opeval {Fx 6 xfx Fx 8}

opeval {Fy 6 xfy Fy 8}
opeval {Fy 6 yfx Fy 8}
opeval {Fy 6 yfy Fy 8}
opeval {Fy 6 xfx Fy 8}

*/




static bool
parse_base(const char **ref_line, parser_state_t *state,
           const value_t **out_val);





extern bool
parse_key_always(const char **ref_line, parser_state_t *state, const char *key)
{   bool ok = parse_key(ref_line, key);
    if (!ok)
        parser_error(state, "expected '%s'\n", key);
    return ok;
}





#define ASSOC_HASLEFT   0x01
#define ASSOC_HASRIGHT  0x02
#define ASSOC_LEFTLOW   0x04
#define ASSOC_RIGHTLOW  0x08

#define ASSOC_HASLLEFT  (ASSOC_HASLEFT  | ASSOC_LEFTLOW)
#define ASSOC_HASLRIGHT (ASSOC_HASRIGHT | ASSOC_RIGHTLOW)


/* Operator definitions (as in Prolog)
      f - position of operator
      x - argument of strictly lower precidence
      y - argument of lower or equal precidence
*/


typedef enum
{   assoc_xfx=ASSOC_HASLLEFT | ASSOC_HASLRIGHT,  /* non-associative infix */
    assoc_yfy=ASSOC_HASLEFT  | ASSOC_HASRIGHT,   /* non-associative infix */
    assoc_xfy=ASSOC_HASLLEFT | ASSOC_HASRIGHT,   /* left associative infix */
    assoc_yfx=ASSOC_HASLEFT  | ASSOC_HASLRIGHT,  /* right associative infix */
    assoc_yf =ASSOC_HASLEFT,                     /* associative postfix */
    assoc_xf =ASSOC_HASLLEFT,                    /* non-associative postfix */
    assoc_fy =ASSOC_HASRIGHT,                    /* associative prefix */
    assoc_fx =ASSOC_HASLRIGHT                    /* non-associative prefix */
} op_assoc_t;   /* associativity */


/* at same precidence: a xfy b xfy c = (a xfy b) xfy c
                       a yfx b yfx c = a yfx (b yfx c)
*/


typedef struct
{   const char *name;
    op_assoc_t assoc;
} opassoc_def_t;




static opassoc_def_t op_assoc_info[] =
{  {"yfy", assoc_yfy},
   {"yfx", assoc_yfx},
   {"xfy", assoc_xfy},
   {"xfx", assoc_xfx},
   {"xf",  assoc_xf},
   {"yf",  assoc_yf},
   {"fx",  assoc_fx},
   {"fy",  assoc_fy}
};




#define assoc_prefix(assoc) \
    (((assoc) & (ASSOC_HASLEFT | ASSOC_HASRIGHT)) == ASSOC_HASRIGHT)

#define assoc_postfix(assoc) \
    (((assoc) & (ASSOC_HASLEFT | ASSOC_HASRIGHT)) == ASSOC_HASLEFT)

#define assoc_infix(assoc) \
    (((assoc) & (ASSOC_HASLEFT | ASSOC_HASRIGHT)) == \
     (ASSOC_HASLEFT | ASSOC_HASRIGHT))

#define assoc_valid(assoc) \
    (((assoc) & (ASSOC_HASLEFT | ASSOC_HASRIGHT)) != 0 && \
     (assoc) <= (ASSOC_HASLEFT | ASSOC_HASRIGHT |         \
                 ASSOC_HASLLEFT | ASSOC_HASLRIGHT))

#define assoc_name(assoc)         \
    ((assoc) == assoc_xfx? "xfx": \
     (assoc) == assoc_yfy? "yfy": \
     (assoc) == assoc_xfy? "xfy": \
     (assoc) == assoc_yfx? "yfx": \
     (assoc) == assoc_yf? "yf":   \
     (assoc) == assoc_xf? "xf":   \
     (assoc) == assoc_fy? "fy":   \
     (assoc) == assoc_fx? "fx": "?f?")



typedef int op_prec_t; /**< smaller values bind more tightly than larger ones */


typedef struct
{   const value_t *fn;  /**< equivalent functional form */
    op_assoc_t assoc;   /**< associativity */
} operator_t;


typedef struct op_state_s op_state_t;

typedef bool
parse_arg_fn_t(const char **ref_line, op_state_t *ops,
               const value_t **out_val);

struct op_state_s
{   parser_state_t *state;
    dir_t *opdefs;
    parse_arg_fn_t *parse_arg;
} /* op_state_t */;






static const value_t *
invoke_monadic(const value_t *monadic_fn, const value_t *arg,
               parser_state_t *state)
{   const value_t *bind = substitute(monadic_fn, arg, state, /*unstrict*/FALSE);

    if (NULL != bind)
        return invoke(bind, state);
    else
        return (const value_t *)NULL;
}








static const value_t *
invoke_diadic(const value_t *diadic_fn, const value_t *arg1,
              const value_t *arg2,  parser_state_t *state)
{   const value_t *bind = substitute(diadic_fn, arg1, state, /*unstrict*/FALSE);
    if (NULL != bind)
        return invoke_monadic(bind, arg2, state);
    else
        return NULL;
}







static bool
op_defs_get(op_state_t *ops, op_prec_t prec, dir_t **ref_opdefs)
{   const value_t *precops = dir_int_get(ops->opdefs, prec);
    if (value_type_equal(precops, type_dir))
    {   *ref_opdefs = (dir_t *)precops;
        return TRUE;
    } else
        return FALSE;
}





static bool
parse_op(const char **ref_line, op_state_t *ops, dir_t *opdefs,
         operator_t *ref_op)
{   const char *line = *ref_line;
    const value_t *opval = NULL;
    bool ok = parse_oneof(ref_line, ops->state, opdefs, &opval) &&
              !(isalpha((unsigned char)(*ref_line)[-1]) &&
                (isalpha((unsigned char)(*ref_line)[0]) ||
                 (*ref_line)[0] == '_'));
    /* an operator can not end on an alphabetic or '_' if there is one to
       continue */

    if (ok)
    {   ok = value_type_equal(opval, type_dir);
        if (ok)
        {   dir_t *op = (dir_t *)opval;
            const value_t *opassoc = dir_string_get(op, OP_ASSOC);

            if (value_type_equal(opassoc, type_int))
            {   ref_op->fn = dir_string_get(op, OP_FN);
                ref_op->assoc = (op_assoc_t)value_int_number(opassoc);
            } else
            {   DO(DPRINTF("%s: operator definition "OP_ASSOC
                           " is a %s not an integer\n",
                           codeid(), value_type_name(opassoc));)
                ok = FALSE;
            }
        } else
        {   DO(DPRINTF("%s: operator definition not a directory", codeid());)
        }


        if (!ok)
            *ref_line = line;
    }

    DEBUG_OP(DPRINTF("%s: op %sfound at ...%s\n", codeid(), ok? "":"not ",
                    line);)

    return ok;
}






static bool
parse_op_expr(const char **ref_line, op_state_t *ops,
              op_prec_t prec, const value_t **out_newterm)
{   /* <oparg> <op> <oparg> .... decoded correctly for precority            */
    /* parses the longest run of operators at the given op_prec_t level or  */
    /*     tighter.                                                         */
    bool ok;
    operator_t op;
    const char *line = *ref_line;
    dir_t *opdefs = NULL;     /* operator definitions at this precidence */
    const value_t *arg = NULL;

    (void)parse_space(&line);

    DEBUG_OP(DPRINTF("%s: expr prec %d ...%s\n", codeid(), prec, line););

    if (!op_defs_get(ops, prec, &opdefs))
    {   DEBUG_OP(DPRINTF("%s: lowest precidence - parse base\n", codeid()););
        ok = (*ops->parse_arg)(ref_line, ops, out_newterm);
    } else
    if (parse_op(&line, ops, opdefs, &op) && assoc_prefix(op.assoc) &&
        parse_space(&line))
    {   /* deal with prefix operators */
        DEBUG_OP(DPRINTF("%s: prec %d parse %s op - parse prefix\n",
                        codeid(), prec, assoc_name(op.assoc)););
        switch (op.assoc)
        {   case assoc_fx:
                ok = parse_op_expr(&line, ops, prec+1, out_newterm);
                if (ok)
                {   *out_newterm = invoke_monadic(op.fn, *out_newterm,
                                                  ops->state);
                    *ref_line = line;
                }
                break;
            case assoc_fy:
                ok = parse_op_expr(&line, ops, prec, out_newterm);
                if (ok)
                {   *out_newterm = invoke_monadic(op.fn, *out_newterm,
                                                  ops->state);
                    *ref_line = line;
                }
                break;
            default:
                ok = FALSE;
                break;
        }
    } else
    {   /* try to parse a lower precidence term */
        DEBUG_OP(DPRINTF("%s: prec %d parse lower, diadic or postfix op\n",
                        codeid(), prec););
        if (parse_op_expr(ref_line, ops, prec+1, out_newterm) &&
            parse_space(ref_line))
        {   bool complete = FALSE;
            bool first = TRUE;

            ok = TRUE;
            line = *ref_line;

            DEBUG_OP(VALUE_SHOW_ST("left val: ", ops->state, *out_newterm);)
            DEBUG_OP(DPRINTF("%s: prec %d got low prec left - "
                            "now parse ...%s\n",
                            codeid(), prec, line););

            while (ok && !complete && parse_op(&line, ops, opdefs, &op) &&
                   parse_space(&line))
            {
                DEBUG_OP(DPRINTF("%s: prec %d parse diadic or postfix %s op\n",
                                codeid(), prec, assoc_name(op.assoc)););
                switch (op.assoc)
                {   case assoc_yf:
                        ok = TRUE;
                        /* this op has no "right hand" side to come */
                        *out_newterm = invoke_monadic(op.fn, *out_newterm,
                                                      ops->state);
                        break;
                    case assoc_xf:
                        /* this op has no "right hand" side to come */
                        ok = first;
                        if (ok)
                            *out_newterm = invoke_monadic(op.fn, *out_newterm,
                                                          ops->state);
                        break;
                    case assoc_yfx:
                        /* one argument to the right at the same precidence */
                        ok = parse_op_expr(&line, ops, prec, &arg);
                        complete = TRUE;
                        if (ok)
                            *out_newterm = invoke_diadic(op.fn, *out_newterm,
                                                         arg, ops->state);
                        break;
                    case assoc_xfx:
                        /* one argument and no others */
                        ok = parse_op_expr(&line, ops, prec+1, &arg);
                        complete = TRUE;
                        if (ok)
                            *out_newterm = invoke_diadic(op.fn, *out_newterm,
                                                         arg, ops->state);
                        break;
                    case assoc_xfy:
                        /* possibly many more arguments to the right */
                        ok = parse_op_expr(&line, ops, prec+1, &arg);
                        if (ok)
                            *out_newterm = invoke_diadic(op.fn, *out_newterm,
                                                         arg, ops->state);
                        break;
                    case assoc_yfy:
                        ok = first && parse_op_expr(&line, ops, prec, &arg);
                        complete = TRUE;
                        if (ok)
                            *out_newterm = invoke_diadic(op.fn, *out_newterm,
                                                         arg, ops->state);
                        break;
                    default:
                        complete = TRUE;
                        break;
                }
                if (ok)
                    *ref_line = line;
                first = FALSE;
            }
        } else
            ok = FALSE;
    }
    return ok;
}





/*! Parse for operators using parse_arg to parse the lowest level of
 *  precidence */
static bool
parse_opterm(const char **ref_line, parser_state_t *state,
             parse_arg_fn_t *parse_arg, dir_t *opdefs,
             const value_t **out_term)
{   /* <oparg> <op> <oparg> .... decoded correctly for precidence */
    op_state_t ops;
    ops.state = state;
    ops.opdefs = opdefs;
    ops.parse_arg = parse_arg;
    DEBUG_OP(DIR_SHOW("op defs: ", opdefs););
    return parse_op_expr(ref_line, &ops, /*prec*/0, out_term);
}








static bool
parse_op_base(const char **ref_line, op_state_t *ops, const value_t **out_val)
{   bool ok;

    if (parse_key(ref_line, "(") && parse_space(ref_line))
        ok = parse_opterm(ref_line, ops->state, ops->parse_arg,
                          ops->opdefs, out_val) &&
             parse_space(ref_line) &&
             parse_key_always(ref_line, ops->state, ")");
    else
        ok = parse_base(ref_line, ops->state, out_val);

    return ok;
}









static dir_t *
dir_opassoc(void)
{   dir_t *assoc = dir_id_new();
    int i;

    for (i=0; i < sizeof(op_assoc_info)/sizeof(op_assoc_info[0]); i++)
    {   opassoc_def_t *assoc_def = &op_assoc_info[i];
        dir_string_set(assoc, assoc_def->name,
                       value_int_new(assoc_def->assoc));
    }

    (void)dir_lock(assoc, NULL); /* prevents additions to this directory */

    return assoc;
}















/*****************************************************************************
 *                                                                           *
 *          Command Values                                                   *
 *          ==============                                                   *
 *                                                                           *
 *****************************************************************************/





typedef struct value_cmd_s value_cmd_t;




struct value_cmd_s
{   value_t value;
    const char *help;
    cmd_fn_t *exec;
    const value_t *fn_exec;
} /* value_cmd_t */;




static value_type_t type_cmd_val;
type_t type_cmd = &type_cmd_val;




static int
value_cmd_print(outchar_t *out, const value_t *root, const value_t *value)
{   int n = 0;
    (void)root;
    if (PTRVALID(value) && value_istype(value, type_cmd))
    {   value_cmd_t *cmd = (value_cmd_t *)value;
        n += outchar_printf(out, "<cmd:%p,1>", cmd->exec);
    }
    return n;
}





static void
value_cmd_markver(const value_t *value, int heap_version)
{   value_cmd_t *cmd = (value_cmd_t *)value;
    if (NULL != cmd->fn_exec)
        value_mark_version((value_t *)/*unconst*/cmd->fn_exec, heap_version);
}





STATIC_INLINE value_t *
value_cmd_init(value_cmd_t *cmd, cmd_fn_t *exec, const value_t *fn_exec,
               const char *help,  bool on_heap)
{   value_t *initval;
    cmd->exec = exec;
    cmd->help = help;
    cmd->fn_exec = fn_exec;
    initval = value_init(&cmd->value, type_cmd, on_heap);
    value_unlocal(fn_exec);
    return initval;
}





static void
value_cmd_delete(value_t *value)
{   if (PTRVALID(value))
    {   if (value_istype(value, type_cmd))
        {   value_cmd_t *cmd = (value_cmd_t *)value;
            FTL_FREE(cmd);
        }
        /* else type error */
    }
}





extern value_t *
value_cmd_new(cmd_fn_t *exec, const value_t *fn_exec, const char *help)
{   value_cmd_t *cmd = (value_cmd_t *)FTL_MALLOC(sizeof(value_cmd_t));

    if (NULL != cmd)
        return value_cmd_init(cmd, exec, fn_exec, help, /*on_heap*/TRUE);
    else
        return NULL;
}





STATIC_INLINE cmd_fn_t *
value_cmd_exec(value_cmd_t *cmd)
{   return cmd->exec;
}




extern const char *
value_cmd_help(const value_t *cmdval)
{   if (value_istype(cmdval, type_cmd))
    {   value_cmd_t *cmd = (value_cmd_t *)cmdval;
        const char *cmdhelp = cmd->help;
        const value_t *fn = cmd->fn_exec;
        if (NULL == cmdhelp && NULL != fn)
            return parser_help_text(fn);
        else
            return cmdhelp;
    } else
        return NULL;
}




static void
values_cmd_init(void)
{
    type_id_t cmd_type_id = type_id_new();

    type_init(&type_cmd_val, /*on_heap*/FALSE, cmd_type_id, "cmd",
              &value_cmd_print, /*&value_cmd_parse*/NULL,
              /*&value_cmd_compare*/NULL, &value_cmd_delete,
              &value_cmd_markver);
}






/*****************************************************************************
 *                                                                           *
 *          Function Values                                                  *
 *          ===============                                                  *
 *                                                                           *
 *****************************************************************************/





typedef struct value_func_s value_func_t;





struct value_func_s
{   value_t value;
    const char *help;
    func_fn_t *exec;
    int args;
    void *implicit;
} /* value_func_t */;





static value_type_t type_func_val;
type_t type_func = &type_func_val;




STATIC_INLINE func_fn_t *
value_func_exec(value_func_t *func)
{   return func->exec;
}






STATIC_INLINE int
value_func_args(value_func_t *func)
{   return func->args;
}






static int
value_func_print(outchar_t *out, const value_t *root, const value_t *value)
{   int n = 0;
    (void)root;
    if (PTRVALID(value) && value_istype(value, type_func))
    {   value_func_t *func = (value_func_t *)value;
        n += outchar_printf(out, "<func:%p,%d>", func->exec, func->args);
    }
    return n;
}





STATIC_INLINE value_t *
value_func_init(value_func_t *func, type_t func_type,
                func_fn_t *exec, const char *help,
                int args, void *implicit, bool on_heap)
{   func->exec = exec;
    func->help = help;
    func->args = args;
    func->implicit = implicit;
    return value_init(&func->value, func_type, on_heap);
}





static void
value_func_delete(value_t *value)
{   if (PTRVALID(value))
    {   if (value_istype(value, type_func))
        {   value_func_t *func = (value_func_t *)value;
            FTL_FREE(func);
        }
        /* else type error */
    }
}





extern value_t *
value_func_new(func_fn_t *exec, const char *help, int args, void *implicit)
{   value_func_t *func = (value_func_t *)FTL_MALLOC(sizeof(value_func_t));

    if (NULL != func)
        return value_func_init(func, &type_func_val, exec, help, args, implicit,
                               /*on_heap*/TRUE);
    else
        return NULL;
}





extern const char *
value_func_help(const value_t *func)
{   if (value_istype(func, type_func))
        return ((const value_func_t *)func)->help;
    else
        return NULL;
}






extern void *
value_func_implicit(const value_t *func)
{   if (value_istype(func, type_func))
        return ((const value_func_t *)func)->implicit;
    else
        return NULL;
}







/*****************************************************************************
 *                                                                           *
 *          LHV Function Values                                              *
 *          ===================                                              *
 *                                                                           *
 *****************************************************************************/





typedef struct value_func_lhv_s value_func_lhv_t;

struct value_func_lhv_s
{   value_func_t fn;
    const value_t *lv_name;
} /* value_func_lhv_t */;



static value_type_t type_func_lhv_val; /* type implementation */





static int
value_func_lhv_print(outchar_t *out, const value_t *root, const value_t *value)
{   int n = 0;
    (void)root;
    if (PTRVALID(value) && value_istype(value, type_func))
    {   value_func_lhv_t *lhv_func = (value_func_lhv_t *)value;
        /*value_func_t *func = &lhv_func->fn;*/
        n += outchar_printf(out, "{");
        n += value_print(out, root, lhv_func->lv_name);
        n += outchar_printf(out, "="BUILTIN_ARG"1}");
    }
    return n;
}






static void
value_func_lhv_markver(const value_t *value, int heap_version)
{   value_func_lhv_t *func = (value_func_lhv_t *)value;
    value_mark_version((value_t *)/*unconst*/func->lv_name, heap_version);
}







static const value_t *
fn_lhv_setval(const value_t *this_fn, parser_state_t *state)
{   const value_t *val = parser_builtin_arg(state, 1);
    value_func_lhv_t *func;
    const value_t *codeval = NULL;
    dir_t *dir = NULL;
    const value_t *unbound = NULL;
    dir_stack_t *stack;

    (void)value_closure_get(this_fn, &codeval, &dir, &unbound);
    OMIT(DIR_SHOW_ST("pre-return dir of @fn: ", state, dir);)

    stack = value_env_dir_stack((value_env_t *)dir);
    func = (value_func_lhv_t *)codeval;
    /* return to the calling envrionment */
    dir_stack_return(stack, dir_stack_last_pos(stack)); /* argument */
    dir_stack_return(stack, dir_stack_last_pos(stack)); /* infodir */
    OMIT(DIR_SHOW_ST("return dir of @fn: ", state, dir);
         printf("at %p\n", dir_stack_top(stack));
         printf("top is %p\n", dir_stack_top(parser_env_stack(state)));)

    if (NULL != dir)
    {   if (!dir_set(dir_stack_top(stack), func->lv_name, val))
        {   parser_error(state, "failed to set value of ");
            parser_value_print(state, func->lv_name);
            fprintf(stderr, "'\n");
        }
    }
    return val;
}





STATIC_INLINE value_t *
value_func_lhv_init(value_func_lhv_t *func, const value_t *lv_name,
                    bool on_heap)
{   value_t *initval;
    func->lv_name = lv_name;
    func->fn.exec = &fn_lhv_setval;
    func->fn.help = "- assign value to name";
    func->fn.args = 1;
    initval = value_init(&func->fn.value, &type_func_lhv_val, on_heap);
    value_unlocal(lv_name);
    return initval;
}




extern value_t *
value_func_lhv_new(const value_t *lv_name)
{   value_func_lhv_t *func = (value_func_lhv_t *)
                             FTL_MALLOC(sizeof(value_func_lhv_t));

    if (NULL != func)
        return value_func_lhv_init(func, lv_name, /*on_heap*/TRUE);
    else
        return NULL;
}







/*****************************************************************************
 *                                                                           *
 *          Function Values                                                  *
 *          ===============                                                  *
 *                                                                           *
 *****************************************************************************/




static void
values_func_init(void)
{
    type_id_t func_type_id = type_id_new();

    type_init(&type_func_val, /*on_heap*/FALSE, func_type_id, "fn",
              &value_func_print, /*&value_func_parse*/NULL,
              /*&value_func_compare*/NULL, &value_func_delete,
              /*&value_func_markver*/NULL);

    type_init(&type_func_lhv_val, /*on_heap*/FALSE, func_type_id, "fn",
              &value_func_lhv_print,
              /*&value_func_parse*/NULL,
              /*&value_func_lhv_compare*/NULL,
              &value_func_delete,
              &value_func_lhv_markver);
}








/*****************************************************************************
 *                                                                           *
 *          Memory (generic)                                                 *
 *          ================                                                 *
 *                                                                           *
 *****************************************************************************/





/* structure defining struct value_mem_s is in the header ftl_internal.h */





static int
value_mem_print(outchar_t *out, const value_t *root, const value_t *value)
{   int n = 0;
    (void)root;
    if (PTRVALID(value) && value_istype(value, type_mem))
    {   n += outchar_printf(out, "<:memory:>");
    }
    return n;
}





extern value_t *
value_mem_init(value_mem_t *mem, type_t mem_subtype,
               mem_read_fn_t *read_fn, mem_write_fn_t *write_fn,
               mem_len_able_fn_t *len_able_fn, mem_len_able_fn_t *base_able_fn,
               bool on_heap)
{   value_t *initval;
    mem->read = read_fn;
    mem->write = write_fn;
    mem->len_able = len_able_fn;
    mem->base_able = base_able_fn;
    initval = value_init(&mem->value, mem_subtype, on_heap);
    return initval;
}









/*****************************************************************************
 *                                                                           *
 *          Binary String Memory                                             *
 *          ====================                                             *
 *                                                                           *
 *****************************************************************************/





typedef struct value_mem_bin_s value_mem_bin_t;


struct value_mem_bin_s {
    value_mem_t mem;
    number_t base;
    const value_t *binstr;
};




static value_type_t type_mem_val;
type_t type_mem = &type_mem_val;

static value_type_t type_mem_shared_val;





static void
value_mem_bin_markver(const value_t *value, int heap_version)
{
    value_mem_bin_t *binmem = (value_mem_bin_t *)value;
    value_mark_version((value_t *)/*unconst*/binmem->binstr, heap_version);
}



static void
value_mem_bin_delete(value_t *value)
{   if (value != (const value_t *)NULL) {
        value_mem_bin_t *binmem = (value_mem_bin_t *)value;
        value_delete((value_t **)&binmem->binstr);
    }
    value_delete_alloced(value);
}




static bool /*rc*/
value_mem_bin_read(const value_mem_t *mem, size_t byte_index,
                   void *buf, size_t buflen, bool force_volatile)
{
    value_mem_bin_t *binmem = (value_mem_bin_t *)mem;
    const value_t *binstr = binmem->binstr;
    number_t base = binmem->base;
    const char *bin;
    size_t binlen;
    bool ok = value_string_get(binstr, &bin, &binlen) &&
              byte_index >= base &&
              byte_index + buflen <= base + binlen;

    if (ok)
        memcpy(buf, bin+byte_index-base, buflen);

    return ok;
}




static bool
value_mem_bin_write(const value_mem_t *mem, size_t byte_index,
                    const void *buf, size_t buflen)
{
    value_mem_bin_t *binmem = (value_mem_bin_t *)mem;
    const value_t *binstr = binmem->binstr;
    number_t base = binmem->base;
    const char *bin;
    size_t binlen;
    bool ok = value_string_get(binstr, &bin, &binlen) &&
              byte_index >= base &&
              byte_index + buflen <= base + binlen;

    OMIT(printf("%s: bin write %zd[%zd] in %"F_NUMBER_T"[%zd] %s\n", codeid(),
                  byte_index, buflen, base, binlen, ok? "OK": "failed"););
    if (ok)
    {   char *writeable_bin = (char *)bin;
        /* we will use this function only on writeable strings */
        memcpy(writeable_bin+byte_index-base, buf, buflen);
    }
    return ok;
}




static size_t /*len*/
value_mem_bin_len_able_ro(const value_mem_t *mem, size_t byte_index,
                          bool unable, mem_attr_map_t ability)
{
    value_mem_bin_t *binmem = (value_mem_bin_t *)mem;
    const char *bin;
    size_t binlen;
    bool ok = value_string_get(binmem->binstr, &bin, &binlen);
    size_t len = 0;

    if (ok) {
        number_t membase = binmem->base;
        if (unable) {
            if (byte_index < membase)
                len = (size_t)(membase - byte_index);
            else if (byte_index >= membase + binlen)
                len = 0; /* there is no next area */
        } else {
            if (byte_index >= membase &&
                byte_index < membase + binlen)
                len = (size_t)(membase + binlen - byte_index);
        }
        if (0 != (ability & (1 << mem_can_write)))
            len = 0; /* we can't write */

        OMIT(printf("%s: mem bin base %"F_NUMBER_T" index %zd len %zd "
                      "%sable 0x%X - gives len %zd\n",
                      codeid(), membase, byte_index, binlen, unable?"un":"",
                      ability, len););
    }
    return len;
}



static size_t /*len*/
value_mem_bin_len_able_rw(const value_mem_t *mem, size_t byte_index,
                          bool unable, mem_attr_map_t ability)
{
    value_mem_bin_t *binmem = (value_mem_bin_t *)mem;
    const char *bin;
    size_t binlen;
    bool ok = value_string_get(binmem->binstr, &bin, &binlen);
    size_t len = 0;

    if (ok) {
        number_t membase = binmem->base;
        if (unable) {
            if (byte_index < membase)
                len = (size_t)(membase - byte_index);
            else if (byte_index >= membase + binlen)
                len = 0; /* there is no next area */
        } else {
            if (byte_index >= membase &&
                byte_index < membase + binlen)
                len = (size_t)(membase + binlen - byte_index);
        }
        OMIT(printf("%s: mem bin base %"F_NUMBER_T" index %zd "
                      "len %zd %sable 0x%X - gives len %zd\n",
                      codeid(), membase, byte_index, binlen, unable?"un":"",
                      ability, len););
    }
    return len;
}



static size_t /*base*/
value_mem_bin_base_able(const value_mem_t *mem, size_t byte_index,
                        bool unable, mem_attr_map_t ability)
{
    value_mem_bin_t *binmem = (value_mem_bin_t *)mem;
    const char *bin;
    size_t binlen;
    bool ok = value_string_get(binmem->binstr, &bin, &binlen);
    size_t base = 0;

    if (ok) {
        number_t membase = binmem->base;
        if (unable) {
            if (byte_index < membase)
                base = 0;
            else if (byte_index >= membase + binlen)
                base = (size_t)(membase + binlen);
        } else {
            if (byte_index >= membase &&
                byte_index < membase + binlen)
                base = (size_t)membase;
        }
        if (0 != (ability & (1 << mem_can_write)))
            base = 0; /* we need something to indicate 'no base' */
    }
    return base;
}




extern value_t *
value_mem_bin_new(const value_t *binstr, number_t base,
                  bool sole_user, bool readonly)
{   value_mem_bin_t *binmem = (value_mem_bin_t *)
                              FTL_MALLOC(sizeof(value_mem_bin_t));

    if (PTRVALID(binmem)) {
        binmem->binstr = binstr;
        binmem->base = base;
        return value_mem_init(&binmem->mem,
                              sole_user? &type_mem_val: &type_mem_shared_val,
                              &value_mem_bin_read,
                              readonly? NULL: &value_mem_bin_write,
                              readonly? &value_mem_bin_len_able_ro:
                                        &value_mem_bin_len_able_rw,
                              &value_mem_bin_base_able, /*on_heap*/TRUE);
    } else
        return NULL;
}





extern value_t *
value_mem_bin_alloc_new(number_t base, size_t len, int memset_val,
                        char **out_block)
{   value_t *val = NULL;
    value_mem_bin_t *binmem = (value_mem_bin_t *)
                              FTL_MALLOC(sizeof(value_mem_bin_t));

    *out_block = NULL;

    if (PTRVALID(binmem)) {
        value_t *binstr = value_string_alloc_new(len, out_block);
        OMIT(printf("%s: binstr %p out_block %p\n", codeid(),
                      binstr, out_block););
        if (binstr == NULL || out_block == NULL)
        {   FTL_FREE(binmem);
        } else {
            binmem->binstr = binstr;
            binmem->base = base;
            if (memset_val != EOF)
                memset(*out_block, memset_val, len);
            val = value_mem_init(&binmem->mem,
                                 &type_mem_shared_val
                                 /*&type_mem - if sole user*/,
                                 &value_mem_bin_read,
                                 &value_mem_bin_write,
                                 &value_mem_bin_len_able_rw,
                                 &value_mem_bin_base_able, /*on_heap*/TRUE);
        }
    }
    return val;
}





/*****************************************************************************
 *                                                                           *
 *          Rebased Memory                                                   *
 *          ==============                                                   *
 *                                                                           *
 *****************************************************************************/





typedef struct value_mem_rebase_s value_mem_rebase_t;


struct value_mem_rebase_s {
    value_mem_t mem;
    number_t base;
    const value_mem_t *unbase_mem;
};


#define value_mem_rebase_value(_rmem) (value_mem_value(&(_rmem)->mem))



static value_type_t type_mem_rebase_val; /*implementation type for rebased mem*/
static value_type_t type_mem_rebase_shared_val; /* " for shared rebased mem */




static void
value_mem_rebase_markver(const value_t *value, int heap_version)
{
    value_mem_rebase_t *rbmem = (value_mem_rebase_t *)value;
    value_mark_version(
        value_mem_value((value_mem_t *)/*unconst*/rbmem->unbase_mem),
        heap_version);
}



static void
value_mem_rebase_delete(value_t *value)
{   if (value != (value_t *)NULL) {
        value_mem_rebase_t *rbmem = (value_mem_rebase_t *)value;
        value_delete((value_t **)&rbmem->unbase_mem);
    }
    value_delete_alloced(value);
}




static bool /*rc*/
value_mem_rebase_read(const value_mem_t *mem, size_t byte_index,
                      void *buf, size_t buflen, bool force_volatile)
{
    value_mem_rebase_t *rbmem = (value_mem_rebase_t *)mem;
    mem_read_fn_t *read_fn = mem->read;
    bool ok = false;

    if (PTRVALID(read_fn))
        ok = (*read_fn)(rbmem->unbase_mem, byte_index - (size_t)rbmem->base,
                        buf, buflen, force_volatile);

    return ok;
}




static bool
value_mem_rebase_write(const value_mem_t *mem, size_t byte_index,
                       const void *buf, size_t buflen)
{
    value_mem_rebase_t *rbmem = (value_mem_rebase_t *)mem;
    mem_write_fn_t *write_fn = mem->write;
    bool ok = false;

    if (write_fn != NULL)
        ok = (*write_fn)(rbmem->unbase_mem, byte_index - (size_t)rbmem->base,
                         buf, buflen);

    return ok;
}




static size_t /*len*/
value_mem_rebase_len_able(const value_mem_t *mem, size_t byte_index,
                          bool unable, mem_attr_map_t ability)
{
    value_mem_rebase_t *rbmem = (value_mem_rebase_t *)mem;
    size_t len = 0;
    mem_len_able_fn_t *len_able_fn = mem->len_able;

    if (len_able_fn != NULL)
        len = (*len_able_fn)(rbmem->unbase_mem, byte_index - (size_t)rbmem->base,
                              unable, ability);

    return len;
}



static size_t /*base*/
value_mem_rebase_base_able(const value_mem_t *mem, size_t byte_index,
                           bool unable, mem_attr_map_t ability)
{
    value_mem_rebase_t *rbmem = (value_mem_rebase_t *)mem;
    size_t base = 0;
    mem_base_able_fn_t *base_able_fn = mem->base_able;

    if (base_able_fn != NULL)
        base = (*base_able_fn)(rbmem->unbase_mem,
                               byte_index - (size_t)rbmem->base,
                               unable, ability);

    return base;
}




extern value_t *
value_mem_rebase_new(const value_t *unbase_mem_val,
                     number_t base, bool readonly, bool sole_user)
{   value_t *newmem = NULL;

    if (value_type_equal(unbase_mem_val, type_mem))
    {
        const value_mem_t *unbase_mem = (const value_mem_t *)(unbase_mem_val);
        value_mem_rebase_t *rbmem = (value_mem_rebase_t *)
                                    FTL_MALLOC(sizeof(value_mem_rebase_t));

        if (rbmem != NULL) {
            rbmem->unbase_mem = (const value_mem_t *)unbase_mem;
            /* we won't update unbase_mem unless readonly is false */
            rbmem->base = base;
            newmem = value_mem_init(&rbmem->mem,
                                    sole_user? &type_mem_rebase_val:
                                               &type_mem_rebase_shared_val,
                                    &value_mem_rebase_read,
                                    readonly? NULL: &value_mem_rebase_write,
                                    &value_mem_rebase_len_able,
                                    &value_mem_rebase_base_able,
                                    /*on_heap*/TRUE);
        }
    }
    return newmem;
}








/*****************************************************************************
 *                                                                           *
 *          Mem Values                                                       *
 *          ==========                                                       *
 *                                                                           *
 *****************************************************************************/





static void
values_mem_init(void)
{
    type_id_t mem_type_id = type_id_new();

    type_init(&type_mem_val, /*on_heap*/FALSE, mem_type_id, "mem",
              &value_mem_print, /*&value_mem_bin_parse*/NULL,
              /*&value_mem_bin_compare*/NULL, &value_mem_bin_delete,
              &value_mem_bin_markver);

    type_init(&type_mem_shared_val, /*on_heap*/FALSE, mem_type_id, "mem",
              &value_mem_print, /*&value_mem_bin_parse*/NULL,
              /*&value_mem_bin_compare*/NULL, &value_delete_alloced,
              &value_mem_bin_markver);

    type_init(&type_mem_rebase_val, /*on_heap*/FALSE, mem_type_id, "mem",
              &value_mem_print, /*&value_mem_rebase_parse*/NULL,
              /*&value_mem_rebase_compare*/NULL, &value_mem_rebase_delete,
              &value_mem_rebase_markver);

    type_init(&type_mem_rebase_shared_val, /*on_heap*/FALSE,
              mem_type_id, "mem",
              &value_mem_print, /*&value_mem_rebase_parse*/NULL,
              /*&value_mem_rebase_compare*/NULL, &value_delete_alloced,
              &value_mem_rebase_markver);
}






/*****************************************************************************
 *                                                                           *
 *          Modules                                                          *
 *          =======                                                          *
 *                                                                           *
 *****************************************************************************/







/* Standard operator precidences - NB: these must be used and be consecutive */

#define OP_PREC_OR     0
#define OP_PREC_AND    1
#define OP_PREC_NOT    2
#define OP_PREC_BITOR  3
#define OP_PREC_BITXOR 4
#define OP_PREC_BITAND 5
#define OP_PREC_BITNOT 6
#define OP_PREC_CMP    7
#define OP_PREC_SHIFT  8
#define OP_PREC_SIGN   9
#define OP_PREC_SUM    10
#define OP_PREC_PROD   11




STATIC_INLINE void
value_mod_cmd_create(const value_t *helpstrval, value_t *closure, int args)
{   dir_t *helpenv = dir_id_new();
    int i;
    value_t *argnext = NULL;

    dir_cstring_set(helpenv, BUILTIN_HELP, helpstrval);

    /* is this safe? it uses the root's next pointer ... again */
    (void)value_closure_pushdir(closure, helpenv, /*env_end*/FALSE);

    for (i=1; i<=args; i++)
        argnext = value_closure_pushunbound(closure, argnext,
                                            value_string_argname(i));
}






static value_t *
mod_add_cmd(dir_t *dir, const char *name, const char *help, value_t *cmd,
            dir_stack_t *scope)
{   value_t *made = NULL;

    if (NULL == cmd)
        fprintf(stderr, "%s: failed to create %s \"%s\"\n",
                codeid(), value_type_name(cmd), name);
    else
    {   int args = value_type_equal(cmd, type_func)?
                   value_func_args((value_func_t *)cmd): 1;
        value_t *closure = value_closure_new(cmd, value_env_new());

        if (scope != NULL)
            (void)value_closure_pushdir(closure,
                                        dir_stack_dir(dir_stack_copy(scope)),
                                        /*env_end*/FALSE);

        value_mod_cmd_create(value_string_new_measured(help),
                             closure, args);

        if (dir_string_set(dir, name, closure))
            made = closure;
        else
            fprintf(stderr, "%s: failed to add %s \"%s\"\n",
                    codeid(), value_type_name(cmd), name);
    }
    return made;
}







static void
mod_add_op(dir_t *opdefs, op_prec_t prec, op_assoc_t assoc, const char *opname,
           value_t *fn)
{   if (NULL == fn)
        fprintf(stderr, "%s: no function for operator %s\n",
                codeid(), opname);
    else if (!assoc_valid(assoc))
        fprintf(stderr, "%s: invalid associativity value %d for operator %s\n",
                codeid(), assoc, opname);
    else
    {   const value_t *precfnsval = dir_int_get(opdefs, prec);

        if (NULL == precfnsval)
        {   precfnsval = dir_value(dir_id_new());
            if (!dir_int_set(opdefs, prec, precfnsval))
                fprintf(stderr, "%s: can't create operator definitions "
                        "at precidence %d (for op %s)\n",
                        codeid(), prec, opname);
        }

        if (value_type_equal(precfnsval, type_dir))
        {   dir_t *precfns = (dir_t *)precfnsval;
            dir_t *opdefn = dir_id_new();
            value_t *opnameval = value_string_new_measured(opname);
            dir_cstring_set(opdefn, OP_FN, fn);
            dir_cstring_set(opdefn, OP_ASSOC, value_int_new(assoc));
            dir_set(precfns, opnameval, dir_value(opdefn));
        } else
            fprintf(stderr, "%s: invalid operator definitions at "
                    "precidence %d (op %s)\n",
                    codeid(), prec, opname);
    }
}






extern value_t *
mod_add(dir_t *dir, const char *name, const char *help, cmd_fn_t *exec)
{   return mod_add_cmd(dir, name, help,
                       value_cmd_new(exec, /*fn_exec*/NULL, help),
                       /*scope*/NULL);
}






extern value_t *
mod_addfn(dir_t *dir, const char *name, const char *help, func_fn_t *exec,
          int args)
{   return mod_add_cmd(dir, name, help, value_func_new(exec, help, args, NULL),
                       /*scope*/NULL);
}







extern value_t *
mod_addfn_imp(dir_t *dir, const char *name, const char *help, func_fn_t *exec,
              int args, void *implicit_args)
{   return mod_add_cmd(dir, name, help,
                       value_func_new(exec, help, args, implicit_args),
                       /*scope*/NULL);
}







static value_t *
mod_addfnscope(dir_t *dir, const char *name, const char *help, func_fn_t *exec,
               int args, dir_stack_t *scope)
{   return mod_add_cmd(dir, name, help, value_func_new(exec, help, args, NULL),
                       scope);
}







extern void
mod_add_dir(dir_t *dir, const char *name, dir_t *mod)
{   if (!dir_string_set(dir, name, &mod->value))
        fprintf(stderr, "%s: failed to add command directory \"%s\"\n",
                codeid(), name);
}





extern void
mod_add_val(dir_t *dir, const char *name, const value_t *val)
{   dir_string_set(dir, name, val);
}





extern bool
mod_parse_cmd(dir_t *dir, const char **ref_line, const value_t **out_cmd)
{   const value_t *cmd = NULL;
    char cmdname[FTL_CMDNAME_MAX+1];
    const char *start = *ref_line;

    if (parse_id(ref_line, &cmdname[0], sizeof(cmdname))) {
        DEBUG_MODEXEC(fprintf(stderr, "%s: exec - got '%s', search in %p\n",
                             codeid(), &cmdname[0], dir););
        DEBUG_MODEXEC(DIR_SHOW("dir: ", dir);)
        cmd = dir_string_get(dir, cmdname);
    }
    if (cmd != NULL)
    {   *out_cmd = cmd;
        return TRUE;
    } else
    {   *ref_line = start;  /* return to initial value */
        return FALSE;
    }
}



/*! Parse the first symbol from an array of symbols
 *    @param dir       - directory in which commands are looked up
 *    @param ref_argv  - updatable reference to 1st sym in an array of symbols
 *    @param ref_argn  - updatable reference number of syms in array
 *    @param out_val   - place to write the value looked up in dir
 *
 *    @returns TRUE iff a value was parsed and placed in *out_val
 *
 *  After successful execution the locations referred to by ref_argv
 *  and ref_argn are updated to point to the next symbol in the list.
 */
extern bool
mod_parse_argv(dir_t *dir, const char ***ref_argv, int *ref_argn,
               const value_t **out_val)
{   const value_t *cmd = NULL;
    bool ok = FALSE;

    if (*ref_argn > 0)
    {   const char *cmdname = (*ref_argv)[0];
        cmd = dir_string_get(dir, cmdname);
        ok = (cmd != NULL);
    }

    if (ok)
    {   *out_val = cmd;
        DEBUG_ARGV(printf("%s: shift argv %d '%s'\n",
                          codeid(), *ref_argn, (*ref_argv)[0]););
        (*ref_argn)--;
        (*ref_argv)++;
    }
    return ok;
}



/*! Parse an option name from the first symbol from an array of symbols
 *    @param dir       - directory in which commands are looked up
 *    @param ref_argv  - updatable reference to 1st sym in an array of symbols
 *    @param ref_argn  - updatable reference number of syms in array
 *    @param out_val   - place to write the value looked up in dir
 *
 *    @returns TRUE iff a value was parsed and placed in *out_val
 *
 *  An option name is a string beginning either '-' or '--'
 *
 *  After successful execution the locations referred to by ref_argv
 *  and ref_argn are updated to point to the next symbol in the list.
 */
extern bool
mod_parse_optargv(dir_t *dir, const char ***ref_argv, int *ref_argn,
                  const value_t **out_val)
{   const value_t *cmd = NULL;
    bool ok = FALSE;

    if (*ref_argn > 0)
    {   const char *cmdname = (*ref_argv)[0];
        if (cmdname[0] == '-') {
            cmdname++;
            if (cmdname[0] == '-')
                cmdname++;
            ok = TRUE;
            cmd = dir_string_get(dir, cmdname);
        }
    }

    if (ok)
    {   *out_val = cmd;
        DEBUG_ARGV(printf("%s: shift --argv %d '%s'\n",
                          codeid(), *ref_argn, (*ref_argv)[0]););
        (*ref_argn)--;
        (*ref_argv)++;
        return TRUE;
    } else
        return FALSE;
}



extern void *
mod_get_implicit(const value_t *this_fn)
{   const value_t *codeval = NULL;
    dir_t *dir = NULL;
    const value_t *unbound = NULL;

    if (value_closure_get(this_fn, &codeval, &dir, &unbound))
        return value_func_implicit(codeval);
    else
        return NULL;
}








/*****************************************************************************
 *                                                                           *
 *          Boolean Values                                                   *
 *          ==============                                                   *
 *                                                                           *
 *****************************************************************************/








#ifdef FTL_BOOL_ISINT


value_int_t value_int_true;
value_int_t value_int_false;

const value_t *value_true = &value_int_true.value;
const value_t *value_false = &value_int_false.value;

#define value_bool_is_false(val) (0 == value_int_number(val))
#define type_bool type_int


static value_t *
value_bool_init(value_int_t *no, bool val, bool on_heap)
{   no->number = (number_t)val;
    return value_init(&no->value, &type_int, on_heap);
}


static void
values_bool_init(void)
{   value_bool_init(&value_int_true,  TRUE,  /*on_heap*/FALSE);
    value_bool_init(&value_int_false, FALSE, /*on_heap*/FALSE);
}



#else


static value_type_t    type_bool_val;
type_t                 type_bool = &type_bool_val;

static value_func_t    value_func_true;
static value_env_t     value_argenv_true;
static value_closure_t value_closure_true;
static value_string_t  value_helpstr_true;

static value_func_t    value_func_false;
static value_env_t     value_argenv_false;
static value_closure_t value_closure_false;
static value_string_t  value_helpstr_false;

const value_t *value_true  = &value_closure_true.value;
const value_t *value_false = &value_closure_false.value;


#define value_bool_is_false(val) ((val) == value_false)
#define type_bool type_closure



static int
value_bool_print(outchar_t *out, const value_t *root, const value_t *value)
{   (void)root;
    if (value == value_false)
        return outchar_printf(out, "FALSE");
    else
        return outchar_printf(out, "TRUE");
}





static int
value_bool_cmp(const value_t *v1, const value_t *v2)
{   return (v1 == value_false) - (v2 == value_false);
}





static value_t *
value_bool_init(value_closure_t *closure, value_func_t *boolfunc,
                value_env_t *boolargenv, value_string_t *boolhelpstr,
                func_fn_t *boolfn, const char *help)
{   value_t *bfnval;
    value_t *bval;
    value_t *bhelpstr;

    bfnval = value_func_init(boolfunc, &type_func_val, boolfn, help, 1,
                             /*implicit*/FALSE, /*on_heap*/FALSE);
    (void)value_env_init(boolargenv, /*on_heap*/FALSE);
    bval = value_closure_init(closure, &type_bool_val,
                              bfnval, boolargenv, /*on_heap*/FALSE);
    bhelpstr = value_cstring_init(boolhelpstr, help, strlen(help),
                                  /*on_heap*/FALSE);

    value_mod_cmd_create(bhelpstr, bval, 1);

    return bval;
}





static const value_t *
fn_true(const value_t *this_fn, parser_state_t *state)
{   const value_t *arg = parser_builtin_arg(state, 1);
    const value_t *val = NULL;

    if (NULL != arg)
    {   /* return to the calling envrionment */
        parser_env_return(state, parser_env_calling_pos(state));
        val = invoke(arg, state);
    }

    if (val == NULL)
        val = value_false;

    return val;
}





static const value_t *
fn_false(const value_t *this_fn, parser_state_t *state)
{   /*const value_t *arg = parser_builtin_arg(state, 1);*/
    return value_false;
}






static void
values_bool_init(void)
{
    type_id_t closure_type_id = type_closure->id;

    /* bool is a varient of the "fn" type */
    type_init(&type_bool_val, /*on_heap*/FALSE, closure_type_id, "closure",
              &value_bool_print, /*&value_closure_parse*/NULL,
              &value_bool_cmp, &value_closure_delete,
              &value_closure_markver);

    value_bool_init(&value_closure_true, &value_func_true,
                    &value_argenv_true, &value_helpstr_true,
                    &fn_true,  "TRUE value");
    value_bool_init(&value_closure_false, &value_func_false,
                    &value_argenv_false, &value_helpstr_false,
                    &fn_false,  "FALSE value");
}




#endif





extern bool value_istype_bool(const value_t *val)
{   bool ok = value_is_bool(val);

    if (!ok)
    {   if (NULL == val)
            fprintf(stderr, "%s: value is unset - expected TRUE or FALSE\n",
                    codeid());
        if (type_equal(val->kind, type_bool))
            fprintf(stderr, "%s: value is not TRUE or FALSE as expected\n",
                    codeid());
        else
            fprintf(stderr, "%s: (%s) value has wrong type - "
                    "expected TRUE or FALSE\n",
                    codeid(), value_type_name(val));
    }
    return ok;
}







/*****************************************************************************
 *                                                                           *
 *          Value Parsing                                                    *
 *          =============                                                    *
 *                                                                           *
 *****************************************************************************/










/* forward references */

static bool
parse_substitution(const char **ref_line, parser_state_t *state,
                   const value_t **out_val);

extern bool
parse_index_expr(const char **ref_line, parser_state_t *state,
                 const value_t **out_val);

extern bool
parse_indexname_expr(const char **ref_line, parser_state_t *state,
                     const value_t **out_val);




STATIC_INLINE bool parse_dot(const char **ref_line)
{   const char *line = *ref_line;
    return !parse_key(&line, "..") && parse_key(ref_line, ".");
}



STATIC_INLINE bool parse_become(const char **ref_line)
{   const char *line = *ref_line;

    return !parse_key(&line, "==") && parse_key(ref_line, "=");
}



STATIC_INLINE bool parse_pling(const char **ref_line)
{   const char *line = *ref_line;

    return !parse_key(&line, "!=") && parse_key(ref_line, "!");
}



/*! Parse an invocation symbol used on the command line
 *  (the '!' character is often difficult to use on the command line in
 *   unix shells, so we allow the alternative of '^')
 */
STATIC_INLINE bool parse_argv_pling(const char **ref_line)
{   return parse_key(ref_line, "^") || parse_pling(ref_line);
}




/*! Parse the body of a environment value (following the initial '[')
 *
 * parses: [<indexname>=<substitution>,]* [<indexname>,]* ']'
 *
 * If successful this function returns a value_t in out_val which is
 * constructed from a value_env_t.
 *
 * This function may cause a garbage collection
 */
/**/extern bool
parse_env(const char **ref_line, parser_state_t *state,
          const value_t **out_val)
{   bool ok = FALSE;
    value_env_t *env = NULL;
    const value_t *index = NULL;
    bool unbound = FALSE;

    DEBUG_TRACE(DPRINTF("(env: '%s'\n", *ref_line);)
    parse_space(ref_line);

    ok = TRUE;
    if (parse_indexname_expr(ref_line, state, &index) &&
        parse_space(ref_line))
    {   const value_t *val = NULL;
        value_t *pos = NULL; /* end of unbound variable list! */

        do {
            ok = TRUE;
            if (parse_become(ref_line) && parse_space(ref_line))
            {   if (parse_substitution(ref_line, state, &val))
                {   if (NULL == env)
                    {   dir_t *iddir = dir_id_new();
                        env = value_env_new();
                        value_env_pushdir(env, iddir,
                                          /*env_end*/FALSE);
                    }
                    if (NULL != env)
                        dir_set((dir_t *)env, index, val);
                } else
                    ok = FALSE;
            } else
                unbound = TRUE;
        } while (ok && !unbound &&
                 parse_key(ref_line, ",") && parse_space(ref_line) &&
                 parse_indexname_expr(ref_line, state, &index) &&
                 parse_space(ref_line));

        OMIT(DPRINTF("env %sunbound%s: '%s'\n",
                       ok? "":"(FAIL) ",unbound?"":" (NONE)", *ref_line););

        if (ok && unbound)
        {   if (NULL == env)
                env = value_env_new();
            do {
                pos = value_env_pushunbound((value_env_t *)env,
                                             pos, (value_t *)index);
            } while (parse_key(ref_line, ",") && parse_space(ref_line) &&
                     (ok = parse_indexname_expr(ref_line, state, &index)) &&
                     parse_space(ref_line));
        }
    }
    if (ok)
    {   if (NULL == env)
            env = value_env_new();  /* empty environment */
        *out_val = (const value_t *)env;
    }

    DEBUG_TRACE(DPRINTF(")env %s: '%s'\n", ok?"OK":"FAIL", *ref_line);)

    return ok;
}





typedef bool parse_val_fn_t(const char **ref_line,
                            parser_state_t *state, const value_t **out_val);



/* This function may cause a garbage collection */
extern bool
parse_vecarg(const char **ref_line, parser_state_t *state, const char *delim,
             const value_t **out_val)
{   dir_t *vec = NULL;
    int index = 0;
    const value_t *vectmp[2];
    bool ok = TRUE;
    bool series = FALSE;
    parse_val_fn_t *expr_fn = (delim == NULL)? &parse_base:
                                               &parse_substitution;
    bool complete = FALSE;
    bool first = TRUE;
    bool series_possible = TRUE;

    DEBUG_TRACE(DPRINTF("(vecarg: '%s'\n", *ref_line);)
        
    vectmp[0] = NULL;
    vectmp[1] = NULL;

    while (ok && !complete && !series && parse_space(ref_line) &&
           !parse_empty(ref_line) && *ref_line[0] != '>')
    {   const value_t *element = NULL;
        if (index <= 2 && parse_key(ref_line, "..") && parse_space(ref_line))
            series = TRUE;
        else if (!first && NULL != delim &&
                 !parse_key(ref_line, delim) && parse_space(ref_line))
            complete = TRUE;
        else
        {   first = FALSE;
            if ((*expr_fn)(ref_line, state, &element) &&
                parse_space(ref_line))
            {   if (!series && parse_become(ref_line) && parse_space(ref_line))
                {   if (value_type_equal(element, type_int))
                    {   index = (int)value_int_number(element);
                        ok = (*expr_fn)(ref_line, state, &element) &&
                             parse_space(ref_line);
                    } else
                        fprintf(stderr, "%s: names in a vector must be an "
                                "integer not a %s\n",
                                codeid(),
                                element==NULL? "absent":
                                               value_type_name(element));
                }
                if (ok)
                {   if (index >= 0 && index < 2)
                       vectmp[index] = element;
                       /* may be part of series defn. */
                    else
                    {   if (series_possible)
                        {   /* give up on getting a series */
                            vec = dir_vec_new();
                            if (NULL != vectmp[0])
                                dir_int_set(vec, 0, vectmp[0]);
                            if (NULL != vectmp[1])
                                dir_int_set(vec, 1, vectmp[1]);
                            series_possible = FALSE;
                        }
                        dir_int_set(vec, index, element);

                        OMIT(fprintf(stderr, "set index %d to ", index);
                               value_fprint(stderr,
                                            dir_value(parser_root(state)),
                                            element);
                               fprintf(stderr, "\n");)
                    }
                    index++;
                }
            } else
                ok = FALSE;
        }
    }
    if (ok)
    {   if (series)
        {   const value_t *final = NULL;
            DEBUG_SERIES(DPRINTF("up to: '%s'\n", *ref_line);)
            if (!parse_empty(ref_line) && *ref_line[0] != '>')
                ok = (*expr_fn)(ref_line, state, &final);
            if (ok)
            {   /* we now have the first, second and last entries to
                   extrapolate from - or some of them at least! */
                dir_t *dir = dir_series_new_vals(vectmp[0], vectmp[1], final);
                *out_val = dir_value(dir);
            }
        } else
        {   if (index <= 2)
            {   vec = dir_vec_new();
                if (index > 0)
                    dir_int_set(vec, 0, vectmp[0]);
                if (index > 1)
                    dir_int_set(vec, 1, vectmp[1]);
            }
            *out_val = dir_value(vec);
        }
    }
    DEBUG_TRACE(
        DPRINTF(")vecarg %s: '%s'", ok?"OK":"FAIL", *ref_line);
        if (ok) {
            DPRINTF(" = ");
            parser_value_print(state, *out_val);
        }
        DPRINTF("\n");
    )
    return ok;
}






#define DEBUG_PARSE_INDEXNAME OMIT

/* Parse an <indexname> ::= <int> | <string> | <id>
 * This function may cause a garbage collection
 */
static bool
parse_indexname(const char **ref_line, parser_state_t *state,
                  const value_t **out_val)
{   bool ok = FALSE;
    const char *line = *ref_line;
    number_t n;
    
    DEBUG_TRACE(DPRINTF("(indexname: '%s'\n", *ref_line);)

    if (parse_int_val(&line, &n))
    {   *out_val = value_int_new(n);
        DEBUG_PARSE_INDEXNAME(printf("%s: parsed int %ld\n", codeid(), n);)
        ok = TRUE;
    } else
    {   char id[FTL_ID_MAX];
        size_t id_filled = 0;
        if (parse_string(&line, &id[0], sizeof(id), &id_filled))
        {   DEBUG_PARSE_INDEXNAME(printf("%s: parsed string '%s'\n", codeid(),
                                         &id[0]);)
            *out_val = value_string_new(&id[0], id_filled);
            ok = TRUE;
        } else if (parse_id(&line, &id[0], sizeof(id)) && parse_space(ref_line))
        {   *out_val = value_string_new_measured(&id[0]);
            DEBUG_PARSE_INDEXNAME(printf("%s: parsed ID %s\n", codeid(),
                                         &id[0]);)
            ok = TRUE;
        }
    }
    if (ok)
    {
        *ref_line = line;
    }

    DEBUG_TRACE(
        DPRINTF(")indexname %s: '%s'", ok?"OK":"FAIL", *ref_line);
        if (ok) {
            DPRINTF(" = ");
            parser_value_print(state, *out_val);
        }
        DPRINTF("\n");
    )

    return ok;
}




/* Parse an <index> ::= [<env>] | '<' <vecarg> '>' | <indexname>
 * This function may cause a garbage collection
 */
static bool
parse_index(const char **ref_line, parser_state_t *state,
            const value_t **out_val)
{   bool ok = FALSE;
    const char *line = *ref_line;
    
    DEBUG_TRACE(DPRINTF("(index: '%s'\n", *ref_line);)

    if (parse_key(&line, "["))
    {   ok = parse_env(&line, state, out_val) &&
             parse_key_always(&line, state, "]");
    } else
    if (parse_key(&line, "<"))
    {   ok = parse_vecarg(&line, state, ",", out_val) &&
             parse_key_always(&line, state, ">") && parse_space(&line);
    } else
    {
        ok = parse_indexname(&line, state, out_val);
    }

    if (ok)
    {
        *ref_line = line;
    }

    DEBUG_TRACE(
        DPRINTF(")index %s: '%s'", ok?"OK":"FAIL", *ref_line);
        if (ok) {
            DPRINTF(" = ");
            parser_value_print(state, *out_val);
        }
        DPRINTF("\n");
    )

    return ok;
}







/* This function may cause a garbage collection */
/**/extern bool
parse_index_expr(const char **ref_line, parser_state_t *state,
                 const value_t **out_val)
{   bool ok;
    if (parse_key(ref_line, "("))
    {   ok = parse_expr(ref_line, state, out_val) &&
             parse_key(ref_line, ")");
    } else
    {
        ok = parse_index(ref_line, state, out_val);
    }
    return ok;
}








/* This function may cause a garbage collection */
/**/extern bool
parse_indexname_expr(const char **ref_line, parser_state_t *state,
                     const value_t **out_val)
{   bool ok;
    if (parse_key(ref_line, "("))
    {   ok = parse_expr(ref_line, state, out_val) &&
             parse_key(ref_line, ")");
    } else
    {
        ok = parse_indexname(ref_line, state, out_val);
    }
    return ok;
}








/**/extern bool
get_index_dir(const value_t *ival, dir_t **out_parent)
{   bool ok = TRUE;

    if (NULL == ival)
        *out_parent = (dir_t *)NULL;
    else
    {   ok = value_to_dir(ival, out_parent);
        if (ok && NULL == *out_parent)
        {   /* value_to_dir TRUE only if directory or closure - can't get a
               NULL directory - so this must be a closure with no directory
               part
            */
            const value_t *code;
            const value_t *unbound;
            OMIT(printf("closure has empty env dir\n");)
            /* give closure an empty directory */
            value_closure_pushdir(ival, dir_id_new(), /*end*/FALSE);
            /* return environment with empty directory in it */
            value_closure_get(ival, &code, out_parent, &unbound);
        }
    }

    return ok;
}






extern const value_t *
dir_dot_lookup(dir_t *dir, const value_t *name); /* forward ref. */


typedef struct
{   dir_t *dir_build;   /* directory we're building from given names/values */
    dir_t *dir_env;     /* directory to look up values in */
} enum_lookup_arg_t;







static void *
enum_lookup_exec(dir_t *dir, const value_t *name, const value_t *value,
                 void *arg)
{   enum_lookup_arg_t *luarg = (enum_lookup_arg_t *)arg;
    dir_t *dir_new = luarg->dir_build;
    if (NULL == dir_new)
    {   if (value_type_equal(name, type_int))
            dir_new = dir_vec_new();
        else
            dir_new = dir_id_new();

        luarg->dir_build = dir_new;
    }
    return dir_set(dir_new, name, dir_dot_lookup(luarg->dir_env, value))?
           NULL: (void *)name;
    /* will stop on first non-null */
}






/*! evaluate a potentially complex lookup in the given directory
 *  where the index might be a directory or a closure
 */
extern const value_t *
dir_dot_lookup(dir_t *dir, const value_t *name)
{   if (name == NULL)
        return NULL;
    else
    {
        type_t nametype = value_type(name);

        OMIT(DPRINTF("%s: dir lookup name type %p - %s\n",
                   codeid(), nametype, value_type_name(name)););

        if (type_equal(nametype, type_dir))
        {   enum_lookup_arg_t luarg;

            luarg.dir_build = NULL;
            luarg.dir_env = dir;

            if (NULL == dir_forall((dir_t *)name, &enum_lookup_exec, &luarg))
            {   dir_t *newdir = luarg.dir_build;
                if (NULL == newdir)
                    newdir = dir_id_new();
                return dir_value(newdir);
            } else
                return &value_null;
        } else

        if (type_equal(nametype, type_closure))
        {   const value_t *unbound;
            const value_t *code;
            dir_t *ixdir = NULL;
            (void)value_closure_get(name, &code, &ixdir, &unbound);
            if (NULL != ixdir)
                return dir_dot_lookup(dir, dir_value(ixdir));
            else
                return name;
        }
        else
        {
            OMIT(DPRINTF("%s: dir lookup name in type %p - %s\n",
                         codeid(), value_type(dir_value(dir)),
                         value_type_name(dir_value(dir))););
            return dir_get(dir, name);
        }
    }
}






/*! evaluate simple lookup in the given directory
 *  where the name must be a string
 */
extern const value_t *
dir_dot_lookup_name(dir_t *dir, const value_t *name)
{   if (name == NULL)
        return NULL;
    else
    {
        OMIT(DPRINTF("%s: dir lookup name type %p - %s\n",
                   codeid(), value_type(name), value_type_name(name));
             DPRINTF("%s: dir lookup name in type %p - %s\n",
                     codeid(), value_type(dir_value(dir)),
                     value_type_name(dir_value(dir)));
        );
        return dir_get(dir, name);
    }
}






/*! parse a series of index expressions
 *  we have parsed <parent>., or we know the parent -
 *  we now need to parse <index>[.<index>]*
 *
 *  When completed successfully
 *     *out_parent contains the directory in which the final <index> should be
 *                 found
 *     *out_id contains the index to be looked up there
 *
 *  This function may cause a garbage collection
 */
/**/extern bool
parse_index_path(const char **ref_line, parser_state_t *state,
                 dir_t *indexed,
                 dir_t **out_parent, const value_t **out_id)
{   
    bool ok;
    dir_t *parent = indexed;
    const value_t *new_id = NULL;

    DEBUG_TRACE(DPRINTF("(index path: '%s'\n", *ref_line);)

    ok = parse_index_expr(ref_line, state, &new_id) && parse_space(ref_line);
    OMIT(printf("%s: index was %s ...%s\n",
              codeid(), ok? "before": "not at", *ref_line);)

    while (ok && parse_dot(ref_line) &&
           parse_space(ref_line))
    {   /* get parent from old parent */
        const value_t *ival = dir_dot_lookup(parent, new_id);

        if (NULL == ival)
        {   parser_error(state, "undefined index symbol '");
            parser_value_print(state, new_id);
            OMIT(printf("' in %s", value_type_name(dir_value(parent)));)
            fprintf(stderr, "'\n");
            OMIT(DIR_SHOW_ST("parent env: ", state, parent);)
            ok = FALSE;
        }

        ival = value_nl(ival);
        /* garbage collection will look after the discarded values */

        if (ok && !get_index_dir(ival, &parent))
        {   parser_error(state, "can't lookup values in a %s\n",
                         value_type_name(ival));
            ok = FALSE;
        }

        if (ok)
        {   ok = parse_index_expr(ref_line, state, &new_id) &&
                 parse_space(ref_line);
        }
    }

    if (ok)
    {   *out_parent = parent;
        *out_id = new_id;
    }

    DEBUG_TRACE(DPRINTF(")index path %s: '%s'\n", ok?"OK":"FAIL", *ref_line);)

    return ok;
}






/*! parse an indexname followed by a series of index expressions
 *  parse: <indexname>[.<index>]*
 *  We know the parent environment in which the first indexname can be sought.
 *  This function may cause a garbage collection
*/
/**/extern bool
parse_index_fullpath(const char **ref_line, parser_state_t *state,
                     dir_t *env, dir_t **out_parent, const value_t **out_id)
{   
    bool ok;
    const value_t *indexname = NULL;

    ok = parse_indexname_expr(ref_line, state, &indexname) &&
         parse_space(ref_line);
    OMIT(printf("%s: indexname was %s ...%s\n",
              codeid(), ok? "before": "not at", *ref_line);)
    if (ok)
    {
        if (parse_dot(ref_line) && parse_space(ref_line))
        {
            const value_t *ival = dir_dot_lookup_name(env, indexname);
            dir_t *firstparent = NULL;

            if (NULL == ival)
            {   parser_error(state, "undefined index symbol '");
                parser_value_print(state, indexname);
                OMIT(printf("' in %s", value_type_name(dir_value(env)));)
                fprintf(stderr, "'\n");
                OMIT(DIR_SHOW_ST("parent env: ", state, env);)
                ok = FALSE;
            }

            ival = value_nl(ival);
            /* garbage collection will look after the discarded values */

            if (ok && !get_index_dir(ival, &firstparent))
            {   parser_error(state, "can't lookup values in a %s\n",
                             value_type_name(ival));
                ok = FALSE;
            }

            if (ok)
            {
                ok = parse_index_path(ref_line, state, firstparent,
                                      out_parent, out_id);
            }
        } else
        {   *out_parent = env;
            *out_id = indexname;
        }
    }
    return ok;
}






/* This function may cause a garbage collection */
/**/extern bool
parse_lvalue(const char **ref_line, parser_state_t *state,
             dir_t *indexed,
             dir_t **out_parent, const value_t **out_id)
{   if (parse_space(ref_line) && parse_dot(ref_line))
    {   dir_t *localdir = dir_stack_top(parser_env_stack(state));
        return parse_index_fullpath(ref_line, state, localdir,
                                    out_parent, out_id);
    } else
        return parse_index_fullpath(ref_line, state, indexed,
                                    out_parent, out_id);
}





extern bool
parse_code(const char **ref_line, parser_state_t *state,
           const value_t **out_val,
           const char **out_sourcepos, int *out_lineno)
{
    bool ok = parse_key(ref_line, "{");

    if (ok)
    {   charsink_string_t charbuf;
        charsink_t *sink = charsink_string_init(&charbuf);
        const char *str;
        size_t len;
        const char *line = *ref_line;
        int brackets = 1;
        int ch;
        
        *out_sourcepos = parser_source(state); /* must include whole reference*/
        *out_lineno = parser_lineno(state);    /* linesource place no good */
        DEBUG_LNO(printf("%s: CODE obj starts at %s:%d\n",
                         codeid(), *out_sourcepos, *out_lineno););

        while ((ch = *line) != '\0' && !(brackets == 1 && ch == '}'))
        {   charsink_putc(sink, ch);
            if (ch == '{')
                brackets++;
            else if (ch == '}')
                brackets--;
            line++;
        }
        if (brackets == 1 && ch == '}')
        {   *ref_line = line+1;
            charsink_string_buf(sink, &str, &len);
            *out_val = value_string_new(str, len);
        } else
            ok = FALSE;

        charsink_string_close(sink);
    }
    return ok;
}






/* If *out_isenv is TRUE the value assigned to out_val is based on a value_env_t
 * (otherwise it might be any other type of value_t)
 * This function may cause a garbage collection
 */
static bool
parse_base_env(const char **ref_line, parser_state_t *state,
               const value_t **out_val, bool *out_isenv)
{   /* syntax: '(' <expr> ') | '[' <id-dir> ']' |
               '<' <vec> '>' | '{' <code> '}' |
               <identifier> | <string> | <integer expression>
    */
    number_t val;
    bool ok = TRUE;
    const value_t *strval;
    const char *def_source;
    int def_lineno;

    parse_space(ref_line);

    DEBUG_TRACE(DPRINTF("(base: '%s'\n", *ref_line);)
    if (NULL != out_isenv)
        *out_isenv = FALSE;

    if (parse_key(ref_line, "(") && parse_space(ref_line))
    {
        ok = parse_expr(ref_line, state, out_val) &&
             parse_space(ref_line) && parse_key_always(ref_line, state, ")");
    } else
    if (parse_key(ref_line, "["))
    {   ok = parse_env(ref_line, state, out_val) &&
             parse_key_always(ref_line, state, "]");
        if (NULL != out_isenv)
            *out_isenv = TRUE;
    } else
    if (parse_key(ref_line, "<"))
    {   ok = parse_vecarg(ref_line, state, ",", out_val) &&
             parse_key_always(ref_line, state, ">") && parse_space(ref_line);
    } else
    if (parse_code(ref_line, state, &strval, &def_source, &def_lineno)) {
        DEBUG_LNO(printf("%s: new code parsed at %s:%d\n",
                         codeid(), def_source, def_lineno);); 
        *out_val = value_code_new(strval, def_source, def_lineno);
    } else
    if (parse_int_expr(ref_line, state, &val))
        *out_val = value_int_new(val);
    else
    {   char strbuf[FTL_STRING_MAX];
        if (parse_id(ref_line, &strbuf[0], sizeof(strbuf)))
        {   const value_t *v = dir_string_get(parser_env(state), &strbuf[0]);
            if (NULL == v)
            {   parser_error(state, "undefined symbol '%s'\n", strbuf);
                DEBUG_ENV(DIR_SHOW("env: ", parser_env(state));)
                ok = FALSE;
            }
            *out_val = value_nl(v);
        } else
        if (!parse_string_expr(ref_line, state,
                               &strbuf[0], sizeof(strbuf), out_val))
            ok = FALSE;
    }

    DEBUG_TRACE(DPRINTF(")base %s: '%s'\n", ok? "OK":"FAILED", *ref_line);)

    return ok;
}






/* This function may cause a garbage collection */
static bool
parse_base(const char **ref_line, parser_state_t *state,
           const value_t **out_val)
{   return parse_base_env(ref_line, state, out_val, /*out_isenv*/NULL);
}







/* new_env is either a value_env_t if new_is_env_t is true else is a dir_t */
static bool
env_add(parser_state_t *state, value_env_t **ref_env, const value_t *new_env,
        bool new_is_env_t, bool inherit)
{   bool ok = TRUE;

    if (*ref_env == NULL && new_is_env_t && !inherit)
        *ref_env = (value_env_t *)new_env;
    else
    {   if (*ref_env == NULL)
            *ref_env = value_env_new();

        if (*ref_env != NULL && new_is_env_t)
        {   value_env_t *nenv = (value_env_t *)new_env;
            if (value_env_unbound(*ref_env) == NULL)
                (*ref_env)->unbound = value_env_unbound(nenv);
            else if (value_env_unbound(nenv) != NULL)
            {   ok = FALSE;
                parser_error(state, "can't combine two environments with "
                             "unbound variables\n");
            }
        }
        if (inherit) {
            dir_t *def_env = parser_env_copy(state);
            if (NULL != def_env)
                value_env_pushdir(*ref_env, def_env, /*env_end*/FALSE);
        }
        if (new_is_env_t)
            /* we know the new_env is a value_env_t */
            (void)value_env_pushenv(*ref_env, (value_env_t *)new_env,
                                    /*env_end*/FALSE);
        else
        {   /* new_env is a dir_t */
            dir_t *new_dir = (dir_t *)new_env;
            /* need a "copy" of dir to get a free link ptr */
            value_env_pushdir(*ref_env,
                                    value_env_dir(value_env_newpushdir(new_dir,
                                                                       FALSE,
                                                                       NULL)),
                                    /*env_end*/FALSE);
        }
    }
    /* *ref_env will not normally be set to NULL now */
    return ok;
}






/*! Parse [[<base>|<code>]:[:]]*[<base>|<code>] | <base>
 *  This function may cause a garbage collection
 */
static bool
parse_closure(const char **ref_line, parser_state_t *state,
              const value_t **out_val)
{   bool ok;
    bool is_closure = TRUE;   /* value (so far) can be represented as closure */
    bool lhs_is_env = FALSE;  /* i.e. lhs was '['...']' & value_env_t */
    bool inherit = TRUE; /* ':' not '::' */
    /* if we can express the result as a directory we use these: */
        const value_t *lhs = NULL;
        bool lhs_is_dir_nonenv = FALSE;  /* i.e. lhs is dir */
    /* otherwise if lhs_is_dir_nonenv is FALSE we use these */
        value_env_t *env = NULL; /* we are constructing this */
        const value_t *code = NULL; /* we construct this too */

    DEBUG_TRACE(DPRINTF("(env expr: '%s'\n", *ref_line);)

    ok = parse_base_env(ref_line, state, &lhs, &lhs_is_env);
    /* lhs_is_env will be true if [<id-dir>] is parsed - and lhs will be a
     * value_env_t (otherwise it could be any value_t) */

    OMIT(DPRINTF("env expr base %s: '%s'\n", ok? "OK":"FAILED", *ref_line);)

    if (ok) {
        if (value_is_codebody(lhs))
            code = lhs;
        else if (value_type_equal(lhs, type_closure)) {
            const value_closure_t *closure = (const value_closure_t *)lhs;
            code = closure->code;
            env = closure->env;
        } else if (lhs_is_env) {
            env = (value_env_t *)lhs;
        } else {
            is_closure = lhs_is_dir_nonenv = value_type_equal(lhs, type_dir);
        }
    }

    OMIT(printf("is%s closure\n", is_closure?"":" not"););
    if (!is_closure)
    {
        *out_val = lhs;
    } else
    {   bool changed = lhs_is_env;  /* we want to change to a closure */

        while (ok &&
               parse_space(ref_line) &&
               (!(inherit = !parse_key(ref_line, "::")) ||
                parse_key(ref_line, ":")) &&
               parse_space(ref_line))
        {   const value_t *rhs = NULL;
            bool rhs_is_env = FALSE;

            ok = parse_base_env(ref_line, state, &rhs, &rhs_is_env);

            if (!ok)
                parser_error(state, "right of '%s' could not be parsed\n",
                             inherit? ":": "::");

            else if (rhs == NULL)
            {   ok = FALSE;
                parser_error(state, "right of '%s' is undefined\n",
                             inherit? ":": "::");

            } else
            {   /* deal with the code value on the right hand of the ':' */

                bool rhs_is_code    = value_is_codebody(rhs);
                bool rhs_is_closure = value_type_equal(rhs, type_closure);
                bool rhs_is_dir     = value_type_equal(rhs, type_dir);
                bool promote_to_env = inherit || rhs_is_closure ||
                                      rhs_is_code || rhs_is_dir ||
                                      code != NULL;
                const value_t *rhs_code = NULL;

                changed = TRUE;


                if (inherit)
                {   dir_t *def_env = parser_env_copy(state);

                    if (NULL != def_env) {
                        value_env_t *newenv;
                        value_t *existing_unbound = NULL;
                        dir_t *argdir;

                        if (promote_to_env && lhs_is_dir_nonenv)
                        {
                            argdir = (dir_t *)lhs;
                            lhs_is_dir_nonenv = FALSE;
                            lhs_is_env = TRUE;
                        } else {
                            argdir = value_env_dir(env);
                            existing_unbound = env->unbound;
                        }

                        newenv = value_env_newpushdir(def_env, /*env_end*/FALSE,
                                                      existing_unbound);
                        /* must push to a value_env_t */
                        value_env_pushdir(newenv, argdir, /*env_end*/FALSE);
                        env = newenv;
                    }
                } else
                if (promote_to_env && lhs_is_dir_nonenv)
                {
                    /* only an environment can have state added to it */
                    env = value_env_newpushdir((dir_t *)lhs, /*env_end*/FALSE,
                                               /*unbound*/NULL);
                    lhs_is_dir_nonenv = FALSE;
                    lhs_is_env = TRUE;
                }

                if (rhs_is_code)
                    rhs_code = rhs;
                else if (rhs_is_closure || rhs_is_dir)
                {   /* we have always promoted to lhs_is_env */
                    value_env_t *rhs_env = NULL;
                    dir_t *rhs_dir = NULL;

                    if (rhs_is_closure) {
                        const value_closure_t *closure =
                            (const value_closure_t *)rhs;
                        rhs_code = closure->code;
                        /* rhs_dir is NULL */
                        rhs_env  = closure->env;
                    } else if (rhs_is_env)
                        /* rhs_dir is NULL */
                        rhs_env = (value_env_t *)rhs;
                    else
                        rhs_dir = (dir_t *)rhs;
                        /* rhs_env is NULL */

                    if (rhs_env == NULL && rhs_dir != NULL)
                    {   value_env_pushdir(env, rhs_dir, /*env_end*/FALSE);
                    } else
                    {   ok = value_env_sum(&env, rhs_env);
                        if (!ok)
                            parser_error(state,
                                         "can't join two environments with "
                                         "unbound variables - not yet "
                                         "supported\n");
                    }
                } else
                {   ok = FALSE;
                    parser_error(state,  "right of '%s' must be "
                                 "code or environment\n", inherit? ":": "::");
                }

                if (ok && rhs_code != NULL)
                {   if (code == NULL)
                        /* no code value accumulated yet */
                        code = rhs_code;
                    else
                    {   parser_error(state, "can't create a closure with "
                                     "two code values with '%s'\n",
                                     inherit? ":": "::");
                        ok = FALSE;
                    }
                }
            }
        }

        if (ok)
        {
            OMIT(DPRINTF("env expr: %sDIR - env %sNULL code %sNULL\n",
                           lhs_is_dir_nonenv? "": "not ",
                           env == NULL? "": "not ", code == NULL? "": "not "););
            if (lhs_is_dir_nonenv || !changed)
                *out_val = lhs;
            else if (env == NULL && code != NULL)
                *out_val = code;
            else if (env == NULL)
            {   parser_error(state, "failed to create a closure - "
                             "internal error\n");
                ok = FALSE;
            } else
            {   /* we have accumulated an environment and perhaps some code */

                value_t *unbound = value_env_unbound(env);

                DEBUG_ENV(DPRINTF("env expr: closure unbound %sNULL "
                                 "code %sNULL\n",
                                 NULL==unbound? "":"not ",
                                 NULL==code? "":"not "););
                /* did we parse a no colon but found a dir with unbound vars? */
                if (NULL != unbound || NULL != code)
                    *out_val = value_closure_new(code, env);
                else
                    /* don't need a closure after all */
                    *out_val = value_env_value(env);
            }
        }
    }

    if (!ok)
        *out_val = NULL;

    DEBUG_TRACE(DPRINTF(")env expr %s: '%s'\n", ok?"OK":"FAIL", *ref_line);)

    return ok;
}









/* parse [<indexname>.]*<index>
 * we have parsed <id>. - we now need to parse [<index>.]*<index> 
 * This function may cause a garbage collection
 */
static bool
parse_index_value(const char **ref_line, parser_state_t *state,
                  dir_t *indexed, const value_t **out_val)
{   dir_t *parent; /* where parent = [<id>.]*) */
    const value_t *id;
    bool ok = FALSE;

    if (parse_index_path(ref_line, state, indexed, &parent, &id))
    {   *out_val = dir_dot_lookup(parent, id);
        ok = TRUE;
        if (NULL == *out_val)
        {   parser_error(state, "index symbol undefined in parent '");
            parser_value_print(state, id);
            OMIT(printf("' in %s at %p\n",
                        value_type_name(dir_value(parent)), parent);)
            fprintf(stderr, "'\n");
            OMIT(DIR_SHOW("parent env: ", parent);)
        }
    }
    return ok;
}








/* Parse left hand value of index path in indexed object
   we have parsed @<indexed>. - we now need to parse [<index>.]*<index> 
   (where parent = <indexed>.[<indexed>.]*)
   This function may cause a garbage collection 
*/
static bool
parse_index_lhvalue(const char **ref_line, parser_state_t *state,
                    dir_t *indexed, const value_t **out_val)
{
    dir_t *parent;
    const value_t *id;
    bool ok = FALSE;

    if (parse_index_path(ref_line, state, indexed, &parent, &id))
    {   /* create a binding with a new value_func_lhv_t in it */
        value_t *lhvfn = value_func_lhv_new(id);
        if (NULL != lhvfn)
        {   const char *argname = BUILTIN_ARG"1";
            value_t *closure = value_closure_new(lhvfn, (value_env_t *)NULL);
            value_t *name = value_string_new_measured("_get");
            dir_t *infodir = dir_id_new();
            value_env_t *env = value_env_new();
            value_t *code = value_code_new(id, "<lhv>", parser_lineno(state));
            value_t *get;
            DEBUG_LNO(printf("%s: new code lhv at line %d\n",
                             codeid(), parser_lineno(state));); 
            value_env_pushdir(env, parent, /*env_end*/FALSE);
            get = value_closure_new(code, env);
            *out_val = &value_null;
            dir_set(infodir, name, get);
            (void)value_closure_pushdir(closure, parent, /*env_end*/FALSE);
            (void)value_closure_pushdir(closure, infodir, /*env_end*/FALSE);
            OMIT(DIR_SHOW_ST("@parent: ", state, parent);
                   printf("at %p\n", parent);)
            (void)value_closure_pushunbound(closure, NULL,
                                            value_string_new_measured(argname));
            *out_val = closure;
            ok = TRUE;
        }
    }
    return ok;
}








/*! Parse [@<index>|<closure>][.<index>]*
 *  This function may cause a garbage collection
 */
extern bool
parse_retrieval(const char **ref_line, parser_state_t *state,
                const value_t **out_val)
{   
    bool ok = FALSE;
    bool need_index = FALSE;
    bool is_local;
    bool is_lhv = FALSE;

    DEBUG_TRACE(DPRINTF("(lookup: '%s'\n", *ref_line););

    parse_space(ref_line);

    is_local = parse_dot(ref_line);
    if (is_local)
    {   need_index = TRUE;
    } else if ((is_lhv = parse_key(ref_line, "@")))
        /* can't take lhval of local - can't copy top directory for later */
        ok = parse_index_lhvalue(ref_line, state, parser_env_copy(state),
                                 out_val);
    else
    {   ok = parse_closure(ref_line, state, out_val) &&
             parse_space(ref_line);
        need_index = parse_dot(ref_line);
    }

    if (need_index)
    {   dir_t *env;

        parse_space(ref_line);
        if (is_local)
        {   env = dir_stack_top(parser_env_stack(state));
            ok = (NULL != env);
        } else
            ok = value_to_dir(*out_val, &env);

        if (ok)
        {   if (is_lhv)
            {   OMIT(printf("%s: ..>%s\n", codeid(), *ref_line);
                DIR_SHOW_ST("env: ", state, env);)
                /*ok = parse_index_value(ref_line, state, env, out_val);*/
                ok = parse_index_lhvalue(ref_line, state, env, out_val);
            } else
                ok = parse_index_value(ref_line, state, env, out_val);
        } else
        {   parser_error(state,
                         "left of '.' must be a directory or a closure\n");
            ok = FALSE;
        }
    }
    
    DEBUG_TRACE(
        DPRINTF(")lookup %s: '%s'", ok?"OK":"FAIL", *ref_line);
        if (ok) {
            DPRINTF(" = ");
            parser_value_print(state, *out_val);
        }
        DPRINTF("\n");
    )

    return ok;
}






static bool
parse_retrieval_base(const char **ref_line, op_state_t *ops,
                     const value_t **out_val)
{   return parse_retrieval(ref_line, ops->state, out_val);
}







/*! Parse operator expression using parse_retrieval_base to parse lowest
 *  level of precience.
 *  This function may cause a garbage collection
 */
static bool
parse_operator_expr(const char **ref_line, parser_state_t *state,
                    const value_t **out_val)
{   /* parse based on defined operator definitions */
    bool ok;
    DEBUG_TRACE(DPRINTF("(op expr: '%s'\n", *ref_line););
    ok = parse_opterm(ref_line, state, &parse_retrieval_base,
                      state->opdefs, out_val);
    DEBUG_TRACE(DPRINTF(")op expr %s: '%s'\n", ok?"OK":"FAIL", *ref_line);)
    return ok;
}





#if 0
static bool can_substitute(const value_t *code)
{   bool substitutable = false;
    if (NULL != code && value_type_equal(code, type_closure))
    {   const value_t *codeval = NULL;
        dir_t *dir = NULL;
        const value_t *unbound = NULL;

        value_closure_get(code, &codeval, &dir, &unbound);
        substitutable = (NULL != unbound);
    }
    return substitutable;
}
#endif 



 
/* if 'unstrict' is set the substitution will not fail if there are not enough
   unbound symbols for the substitution (the code value itself will be
   returned)
*/
static const value_t *
substitute_simple(const value_t *code, const value_t *arg,
                  parser_state_t *state, bool unstrict, bool *ref_auto_eval)
{   const value_t *val = NULL;

    *ref_auto_eval = FALSE;

    if (NULL != code)
    {   if (value_type_equal(code, type_closure))
        {   const value_t *codeval = NULL;
            dir_t *dir = NULL;
            const value_t *unbound = NULL;

            value_closure_get(code, &codeval, &dir, &unbound);
            if (NULL == unbound)
            {   if (unstrict)
                    val = code;
                else
                {   parser_error(state, "too many arguments (no unbound "
                                        "symbols in closure) - "
                                        "missing ';'?\n");
                    parser_value_print(state, dir_value(dir));
                    fprintf(stderr, "\n");
                }
                value_unlocal(arg);
            } else
            {   value_t *newbinding = value_closure_bind(code, arg);

                if (NULL == newbinding)
                {   parser_error(state, "can't bind symbol in closure\n");
                } else
                    val = newbinding;
            }
            if (value_type_equal(codeval, type_cmd) &&
                value_type_equal(arg, type_code))
                *ref_auto_eval = TRUE;
        } else
        {   parser_error(state, "no unbound symbols in a %s - "
                                "substitution impossible\n",
                         value_type_name(code));
            value_unlocal(arg);
        }
    }

    if (val == NULL)
        value_unlocal(code);

    return val;
}






extern const value_t *
substitute(const value_t *code, const value_t *arg,
           parser_state_t *state, bool unstrict)
{   bool auto_eval = FALSE;
    const value_t *val = substitute_simple(code, arg, state,
                                           unstrict, &auto_eval);
    /* I don't like this feature yet - it makes syntax less explicit and
       difficult to understand
    if (NULL != val && auto_eval)
        val = invoke(val, state);
     */
    return val;
}






/* This function may cause a garbage collection */
extern const value_t *
invoke(const value_t *code, parser_state_t *state)
{   const value_t *val = NULL;
    const char *placename;
    int lineno = -1;
    charsource_lineref_t line;

    if (NULL != code)
    {   if (value_type_equal(code, type_code))
        {   const char *buf;
            size_t len;

            DEBUG_MOD(DPRINTF("%s: invoke - code\n", codeid());)
            value_code_place(code, &placename, &lineno);
            value_code_buf(code, &buf, &len);
            linesource_push(parser_linesource(state),
                            charsource_lineref_init(&line, /*delete*/NULL,
                                                    /*rewind*/FALSE,
                                                    placename, lineno, &buf));
            /* will garbage collection the discarded value */
            if (!parse_cmdlist(&buf, state, &val))
            {   parser_error(state, "badly formed code\n");
                val = NULL;
            }
            linesource_pop(parser_linesource(state));
        } else
        if (value_type_equal(code, type_closure))
        {   const char *buf;
            size_t len;
            const value_t *codeval = NULL;
            const value_t *unbound = NULL;
            dir_t *dir = NULL;

            value_closure_get(code, &codeval, &dir, &unbound);
            if (NULL != unbound)
            {   parser_error(state, "can't invoke closure - "
                                    "no argument for ");
                parser_value_print(state, unbound);
                fprintf(stderr, "\n");
                /* it has unbound variables */
            } else
            if (NULL != codeval)
            {   if (value_type_equal(codeval, type_code))
                {   dir_stack_pos_t pos;

                    DEBUG_MOD(DPRINTF("%s: invoke - code closure\n", codeid());)
                    value_code_buf(codeval, &buf, &len);
                    /* we will garbage collect the discarded value */
                    if (NULL == dir)
                        dir = dir_id_new();
                    pos = parser_env_push(state, dir, /*outer_visible*/TRUE);
                    value_code_place(codeval, &placename, &lineno);
                    linesource_push(parser_linesource(state),
                                    charsource_lineref_init(&line,
                                                            /*delete*/NULL,
                                                            /*rewind*/FALSE,
                                                            placename, lineno,
                                                            &buf));
                    if (!parse_cmdlist(&buf, state, &val))
                    {   parser_error(state,
                                     "error in closure code body\n");
                    }
                    linesource_pop(parser_linesource(state));
                    parser_env_return(state, pos);
                } else
                if (value_type_equal(codeval, type_cmd))
                {   /* execute command */
                    const char *buf = NULL;
                    size_t len;
                    value_cmd_t *cmd = (value_cmd_t *)codeval;
                    const value_t *boundarg = dir_get_builtin_arg(dir, 1);

                    DEBUG_MOD(DPRINTF("%s: invoke - cmd closure\n", codeid());)
                    if (value_type_equal(boundarg, type_code))
                    {   (void)value_code_buf(boundarg, &buf, &len);
                    } else
                        (void)value_string_get(boundarg, &buf, &len);
                        /* checks boundarg is a string */
                    if (buf != NULL)
                    {   charsink_string_t expandline;
                        charsink_t *expandsink =
                            charsink_string_init(&expandline);

                        /* expand string (buf, len) into expandsink */
                        parser_expand(state, expandsink, buf, len);

                        /* execute command taking expandsink as parse input */
                        charsink_string_buf(expandsink, &buf, &len);
                        val = value_cmd_exec(cmd)(&buf, code, state);

                        charsink_string_close(expandsink);
                    }
                    else
                        val = NULL;
                } else
                if (value_type_equal(codeval, type_func))
                {   /* execute function */
                    value_func_t *fn = (value_func_t *)codeval;
                    dir_stack_pos_t pos;

                    OMIT(
                        printf("top : %p\n", parser_env_stack(state)->stack);
                        DIR_SHOW("push: ", dir);
                    )
                    pos = parser_env_push(state, dir, /*outer_visible*/TRUE);
                    DEBUG_MOD(DPRINTF("%s: invoke - fn closure with %sreturn\n",
                                     codeid(), pos==NULL?"no ":"");)
                    val = (*value_func_exec(fn))(code, state);
                    parser_env_return(state, pos);
                } else
                {   parser_error(state,
                                  "unexpected closure body - "
                                  "%s not executable\n",
                                  value_type_name(codeval));
                    val = NULL;
                }
            }
        } else
        {   parser_error(state, "a %s value is not executable:\n",
                          value_type_name(code));
            parser_value_print(state, code);
            fprintf(stderr, "\n");
            val = NULL;
        }
        value_unlocal(code);
    }

    return val;
}







extern bool value_istype_invokable(const value_t *val)
{   bool ok = FALSE;

    if (NULL == val)
        fprintf(stderr, "%s: value is unset - expected "
                "code, command or function\n",
                codeid());
    if (!value_is_invokable(val))
        fprintf(stderr, "%s: (%s) value has wrong type - "
                "expected code, command or function\n",
                codeid(), value_type_name(val));
    else
        ok = TRUE;

    return ok;
}








/*! parse up'!'* [<retrieval> '!'*]* 
 * This function may cause a garbage collection
 */
                                
static bool
parse_substitution_args(const char **ref_line, parser_state_t *state,
                        const value_t **ref_val)
{   
    bool ok = TRUE;
    const value_t *newarg = NULL;

    DEBUG_TRACE(DPRINTF("(subst args: '%s'\n", *ref_line););

    while (ok && parse_pling(ref_line) && parse_space(ref_line))
        *ref_val = invoke(*ref_val, state);

    while (ok /* && parse_key(ref_line, "~") && parse_space(ref_line) */
           && parse_operator_expr(ref_line, state, &newarg) &&
           parse_space(ref_line))
    {   const value_t *code = *ref_val;

        if (ok)
        {   *ref_val = substitute(code, newarg, state, /*unstrict*/FALSE);

            if (NULL == *ref_val)
                ok = FALSE;

            while (ok && parse_pling(ref_line) && parse_space(ref_line))
                *ref_val = invoke(*ref_val, state);
        }
    }

    if (NULL == *ref_val)
        *ref_val = &value_null;

    DEBUG_TRACE(DPRINTF(")subst args %s: '%s'\n", ok?"OK":"FAIL", *ref_line);)
    return ok;
}






/*! Parse ['^' <expr|token_as_string> '^'*]
 *  Parse token_as_string if token_strings is TRUE otherwise parse expressions.
 *  This function may cause a garbage collection
 *  When delim != 0 we are parsing the arguments of an --option starting with
 *  the first token following --option.
 */
static bool
parse_substitution_argv(const char *fnname,
                        const char ***ref_argv, int *ref_argn,
                        parser_state_t *state, const char *delim,
                        bool token_strings,
                        const value_t **ref_val)
{   /* '^'* [<retrieval> '^'*]* */
    bool ok = TRUE;
    const value_t *newarg = NULL;
    const char *line = (*ref_argv)[0];  /* treat token as a line of FTL */
    bool complete = FALSE; /* run out of requirement for arguments */
    bool pling = FALSE;
    bool goodarg = TRUE;
    bool autoexec = !token_strings;
                    /* invoke closures with no remaining arguments */

    /* execute *ref_argn if argument is pling or if it is a bound closure */
    while (ok && (
           (pling = (*ref_argn > 0 &&
                     parse_argv_pling(&line) && parse_empty(&line))) ||
           (autoexec &&
                value_type_equal(*ref_val, type_closure) &&
                NULL == value_closure_unbound(*ref_val))))
    {   if (pling)
        {
            DEBUG_ARGV(DPRINTF("%s: subst auto ! argv %d '%s'\n",
                               codeid(), *ref_argn, (*ref_argv)[0]););
            (*ref_argv)++;
            (*ref_argn)--;
            line = (*ref_argv)[0];
        } else
            complete = TRUE;
        DEBUG_MOD(DPRINTF("%s: invoke argv function\n", codeid()););
        *ref_val = invoke(*ref_val, state);
    }

    while (ok && *ref_argn > 0 && !complete &&
           (delim == NULL?
                (*ref_argv)[0][0] != '-':
                0 != strcmp(delim, (*ref_argv)[0]))
           &&
           (goodarg = token_strings?
                          NULL != (newarg = value_string_new_measured(line)):
                          parse_operator_expr(&line, state, &newarg) &&
                              parse_empty(&line))
          )
    {   const value_t *code = *ref_val;

        DEBUG_MOD(DPRINTF("%s: apply argv '%s' argument%s '%s'\n",
                         codeid(), fnname==NULL? "function": fnname,
                         token_strings? " as string": "", line););
        *ref_val = substitute(code, newarg, state, /*unstrict*/FALSE);

        if (NULL == *ref_val)
            ok = FALSE;

        DEBUG_ARGV(DPRINTF("%s: subst argv %d '%s' delim %s\n",
                           codeid(), *ref_argn, (*ref_argv)[0], delim););
        (*ref_argv)++;
        (*ref_argn)--;
        line = (*ref_argv)[0];

        while (ok && *ref_argn > 0 &&
               (delim == NULL?
                    (*ref_argv)[0][0] != '-':
                     0 != strcmp(delim, (*ref_argv)[0]))
               &&
               (
                (pling = (*ref_argn > 0 &&
                          parse_argv_pling(&line) && parse_empty(&line))) ||
                (autoexec &&
                     value_type_equal(*ref_val, type_closure) &&
                     NULL == value_closure_unbound(*ref_val))
               )
            )
        {   if (pling)
            {
                DEBUG_ARGV(DPRINTF("%s: subst post auto ! argv %d '%s'\n",
                                   codeid(), *ref_argn, (*ref_argv)[0]););
                (*ref_argv)++;
                (*ref_argn)--;
                line = (*ref_argv)[0];
            } else
                complete = TRUE;

            DEBUG_MOD(DPRINTF("%s: invoke argv '%s' function after substitute\n",
                             codeid(), fnname==NULL? "function": fnname););
            *ref_val = invoke(*ref_val, state);
            DEBUG_MOD(DPRINTF("%s: returns '%s' value\n",
                             codeid(), type_name((*ref_val)->kind)););
        }
    }

    if (!goodarg)
    {   parser_error(state, "can't parse %s argument - '%s'\n",
                     fnname==NULL? "the": fnname, line);
        ok = FALSE;
    }

    if (ok  && value_type_equal(*ref_val, type_closure))
    {
        if (NULL == value_closure_unbound(*ref_val))
        {   DEBUG_MOD(DPRINTF("%s: invoke argv %s function after substitute\n",
                             codeid(), fnname==NULL? "a": fnname););
            *ref_val = invoke(*ref_val, state);
        }
        else
        {   DEBUG_MOD(
                const value_t *code = NULL;
                dir_t *env = NULL;
                const value_t *unbound = NULL;
                if (value_closure_get(*ref_val, &code, &env, &unbound))
                {
                    int n = 0;
                    while (NULL != unbound) {
                        n++;
                        unbound = unbound->link;
                    }
                    DPRINTF("%s: argv - '%s' function still has %d args\n",
                            codeid(), fnname==NULL? "a": fnname, n);
                }
                else {
                    DPRINTF("%s: argv - '%s' function not a closure\n",
                            codeid(), fnname==NULL? "a": fnname);
                }
            );
            parser_error(state, "insufficient arguments provided for "
                         "'%s'\n",
                         fnname==NULL? "function": fnname);
            ok = FALSE;
        }
    }
    
    if (NULL == *ref_val)
        *ref_val = &value_null;

    return ok;
}





/*! Parse [<retrieval> '!'*]* 
 *  This function may cause a garbage collection
 */
static bool
parse_substitution(const char **ref_line, parser_state_t *state,
                   const value_t **out_val)
{   bool ok;
    
    DEBUG_TRACE(DPRINTF("(substitute: '%s'\n", *ref_line);)

    if (parse_operator_expr(ref_line, state, out_val) && parse_space(ref_line))
    {   OMIT(DPRINTF("substitute args: '%s'\n", *ref_line););
        ok = parse_substitution_args(ref_line, state, out_val);
    } else
    {   *out_val = &value_null;
        ok = FALSE;
    }

    DEBUG_TRACE(DPRINTF(")substitute %s: '%s'\n", ok?"OK":"FAIL", *ref_line);)

    return ok;
}






/* This function may cause a garbage collection */
extern bool
parse_expr(const char **ref_line, parser_state_t *state,
           const value_t **out_val)
{   /* [<index> = <substitution>]* | <substitution> <spaces>  */
    bool assignment = FALSE;
    bool ok = TRUE;
    const value_t *name = NULL;
    const char *line = *ref_line;
    dir_t *parent = NULL;

    DEBUG_TRACE(DPRINTF("(expr: '%s'\n", *ref_line);)
    DEBUG_ENV(DIR_SHOW("expr env: ", parser_env(state));)
    *out_val = NULL;

    while (ok && parse_space(&line) &&
           !parse_key(&line, "(") &&
           parse_lvalue(&line, state, parser_env(state), &parent, &name) &&
           parse_space(&line) &&
           parse_become(&line) && parse_space(&line))
    {   assignment = TRUE;
        /* this is wrong! */
        if (parse_substitution(&line, state, out_val) &&
            parse_space(&line))
        {
            *ref_line = line;
            if (!dir_set(parent, name, *out_val))
            {   parser_error(state, "failed to assign value to '");
                parser_value_print(state, name);
                fprintf(stderr, "'\n");
            }
        } else
            ok = FALSE;
    }

    if (!assignment)
        ok = parse_substitution(ref_line, state, out_val) &&
             parse_space(ref_line);

    DEBUG_TRACE(DPRINTF(")expr %s: '%s'\n", ok?"OK":"FAIL", *ref_line);)
    return ok;
}



/*! Parse <expr>[;[<expr>]]* 
 *  ; discards the previous value 
 *  This function may cause a garbage collection
 */
static bool
parse_cmdlist(const char **ref_line, parser_state_t *state,
              const value_t **out_val)
{   bool ok;

    parse_space(ref_line);

    DEBUG_TRACE(DPRINTF("(cmdlist: '%s'\n", *ref_line);)

    ok = (parse_empty(ref_line) || *ref_line[0]=='}' || *ref_line[0]==';')
         ||
         (parse_expr(ref_line, state, out_val) && parse_space(ref_line));

    while (ok && parse_key(ref_line, ";") && parse_space(ref_line))
    {   /* garbage collect old value eventually */
        value_unlocal(*out_val);
        *out_val = &value_null;
        ok = (parse_empty(ref_line) || *ref_line[0]=='}' || *ref_line[0]==';')
             ||
             (parse_expr(ref_line, state, out_val) &&
              parse_space(ref_line));
    }

    if (parse_space(ref_line) && !parse_empty(ref_line))
        parser_error_longstring(state, *ref_line, "error in");

    DEBUG_TRACE(DPRINTF(")cmdlist %s: '%s'\n", ok?"OK":"FAIL", *ref_line);)
    return ok;
}









/*****************************************************************************
 *                                                                           *
 *          Exceptions                                                       *
 *          ==========                                                       *
 *                                                                           *
 *****************************************************************************/



/* build some default functions that cause execptions */



/* This function may cause a garbage collection */
static const value_t *
fn_throw(const value_t *this_fn, parser_state_t *state)
{   const value_t *info = parser_builtin_arg(state, 1);
    const value_t *val = value_false;

    if (info != NULL) {
        bool thrown = parser_throw(state, info);
        val = value_bool(thrown);
        if (!thrown)
            parser_error(state,  "attempt to throw exception without an "
                         "enclosing try block\n");
    } else
        parser_report_help(state, this_fn);

    return val;
}






/* This function may cause a garbage collection */
static const value_t *
fn_catch(const value_t *this_fn, parser_state_t *state)
{   const value_t *exception_code = parser_builtin_arg(state, 1);
    const value_t *execute_code = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype_invokable(exception_code) &&
        value_istype_invokable(execute_code))
    {   bool ok = TRUE;

        parser_env_return(state, parser_env_calling_pos(state));
        val = parser_catch_invoke(state, execute_code, &ok);

        if (!ok) /* an exception was caught - returned in val */
        {
            const value_t *exception;
            exception = substitute(exception_code, val, state,
                                   /*unstrict*/FALSE);
            val = invoke(exception, state);
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}





typedef struct
{   dir_t *dir_build;
} enum_new_arg_t;






static void *
new_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   enum_new_arg_t *newarg = (enum_new_arg_t *)arg;
    dir_t *dir_new = newarg->dir_build;
    if (NULL == dir_new)
    {   if (value_type_equal(name, type_int))
            dir_new = dir_vec_new();
        else
            dir_new = dir_id_new();

        newarg->dir_build = dir_new;
    }
    return dir_set(dir_new, name, value)? NULL: (void *)name;
    /* will stop on first non-null */
}





#if 0
/* make a copy of a directory */
static dir_t *
dir_copy(dir_t *dir)
{   dir_t *newdir = NULL;
    enum_new_arg_t newarg;

    newarg.dir_build = NULL;

    if (NULL == dir_forall(dir, &new_exec, &newarg))
    {   newdir = newarg.dir_build;

        if (NULL == newdir)
            newdir = dir_id_new();

        if (dir_islocked((dir_t *)dir))
            (void)dir_lock(newdir, NULL);
    }

    return newdir;
}
#endif




/*! make an environment from the function arguments, add 'name' field
 *   and throw the result
 *   This function may cause a garbage collection
 */
static const value_t *
exception_throw(const value_t *this_fn, parser_state_t *state,
                const char *name, const char **argnames)
{   const value_t *val = NULL;
    const value_t *info = NULL;
#if 0
    dir_stack_t *stack = parser_env_stack(state);;
    dir_stack_pos_t pos = dir_stack_top_pos(stack);

    if (pos != NULL)
    {   dir_t *newenv = NULL;
        dir_t *local_env = dir_at_stack_pos(pos);
        if (NULL != local_env) {
            newenv = dir_copy(local_env);
        } else
        {
            newenv = dir_id_new();
            OMIT(fprintf(stderr, "%s: no local vars\n", codeid()););
        }
        dir_string_set(newenv, "name", value_string_new_measured(name));
        info = dir_value(local_copy);
    } OMIT(else fprintf(stderr, "%s: couldn't find stack top\n", codeid()););
#else
    dir_t *newenv = dir_id_new();
    int argn = 1;
    while (argnames[0] != NULL) {
        dir_string_set(newenv, argnames[0], parser_builtin_arg(state, argn));
        argnames++;
        argn++;
    }
    dir_string_set(newenv, "name", value_string_new_measured(name));
    info = dir_value(newenv);

#endif
    if (info != NULL) {
        bool thrown = parser_throw(state, info);
        val = value_bool(thrown);
        if (!thrown)
            parser_error(state,  "exception thrown without an "
                         "enclosing try block\n");
    } else
        parser_report_help(state, this_fn);

    return val;
}




/* Specific exception commands */


/* signal() exception */
static const value_t *
fn_exception_signal(const value_t *this_fn, parser_state_t *state)
{
    const char *args[] = {"signo", NULL}; /* expects 1 argument */
    return exception_throw(this_fn, state, "signal", args);
}




static void
cmds_generic_exception(parser_state_t *state, dir_t *cmds)
{   dir_stack_t *scope = parser_env_stack(state);
    dir_t *expt_cmds = dir_id_new();

    mod_addfnscope(cmds, "catch",
              "<excep> <do> - run <do> executing <excep> <ex> on an exception",
              &fn_catch, 2, scope);
    mod_addfnscope(cmds, "throw",
              "<value> - signal an exception with <value>, exit outer 'catch'",
              &fn_throw, 1, scope);

    mod_add_dir(cmds, FTLDIR_EXCEPT, expt_cmds);

    mod_addfn(expt_cmds, FN_EXCEPT_SIGNAL,
              FN_EXCEPT_SIGNAL" <signo> - send signal exception",
              &fn_exception_signal, 1);
}




/*****************************************************************************
 *                                                                           *
 *          Signal Handling                                                  *
 *          ===============                                                  *
 *                                                                           *
 *****************************************************************************/



#define DEBUG_SIGNAL OMIT


static bool exiting = FALSE;
static parser_state_t *global_int_state = NULL;

/* We have to define the function cause_sigint() */

#define EXIT_ON_CTRL_C


#ifdef EXIT_ON_CTRL_C


static bool throw_signal(parser_state_t *state, int signal)
{
    dir_t *root = parser_root(state);
    const value_t *exceptions =
        root==NULL? NULL: dir_string_get(root, FTLDIR_EXCEPT);
    const value_t *fn_sig =
        !value_type_equal(exceptions, type_dir)? NULL:
        dir_string_get((dir_t *)(exceptions), FN_EXCEPT_SIGNAL);
    const value_t *signal_exception =
        !value_type_equal(fn_sig, type_closure)? NULL:
        value_closure_bind(fn_sig, value_int_new(signal));

    if (signal_exception != NULL) {
        /*parser_throw(state, signal_exception);*/
        OMIT(fprintf(stderr, "%s: throwing FTL signal\n", codeid()););
        (void)invoke(signal_exception, state); /* shouldn't normally return */
        return TRUE;
    } else
    {
        fprintf(stderr, "%s: no registered FTL signal handler\n", codeid());
        return FALSE;
    }
}



#if defined(__unix__) || defined(__APPLE__)

typedef void interrupt_handler_fn(int signal);

typedef struct {
    interrupt_handler_fn *handler;
    parser_state_t *displaced_state;
} interrupt_state_t;


static void interrupt_handler(int signo)
{
    /* find state */
    DEBUG_SIGNAL(fprintf(stderr, "%s: signal %d\n", codeid(), signo););
    if (global_int_state == NULL || !throw_signal(global_int_state, signo))
        exiting = TRUE;
}


#if 0
static bool cause_sigint(void)
{   interrupt_handler(SIGINT);
    return exiting;
}
#endif



static bool handler_init(interrupt_state_t *ref_old_handler,
                         parser_state_t *int_state, int signo,
                         const char *signame)
{   interrupt_handler_fn *old;

    exiting = FALSE;

    old = signal(signo, interrupt_handler);

    DEBUG_SIGNAL(fprintf(stderr, "%s: installing %s handler %p -> last was "
                   "%p%s%s%s\n",
                   codeid(), signame, interrupt_handler, old,
                   old==SIG_ERR?" (ERR)":"", old==SIG_DFL?" (DFL)":"",
                   old==SIG_IGN?" (IGN)":"");
    );

    if (ref_old_handler != NULL) {
        ref_old_handler->displaced_state = global_int_state;
        ref_old_handler->handler = old;
    }

    if (old == SIG_ERR)
        return FALSE;
    else {
        global_int_state = int_state;
        return TRUE;
    }
}



static void handler_end(interrupt_state_t *ref_old_handler,
                        int signo)
{
    
    DEBUG_SIGNAL(interrupt_handler_fn *newi;
         fprintf(stderr, "%s: installing interrupt returned to %p\n",
                 codeid(), ref_old_handler->handler);
         fflush(stderr);
    );
    DEBUG_SIGNAL(newi =)signal(signo, ref_old_handler->handler);
    DEBUG_SIGNAL(fprintf(stderr, "%s: installing old interrupt complete - "
                        "was %p%s%s%s\n",
                        codeid(), newi,
                        newi==SIG_ERR?" (ERR)":"", newi==SIG_DFL?" (DFL)":"",
                        newi==SIG_IGN?" (IGN)":""););
    exiting = FALSE;
    global_int_state = ref_old_handler->displaced_state;
}




static bool interrupt_handler_init(interrupt_state_t *ref_old_handler,
                                   parser_state_t *int_state)
{   return handler_init(ref_old_handler, int_state, SIGINT, "interrupt");
}


static void interrupt_handler_end(interrupt_state_t *ref_old_handler)
{   handler_end(ref_old_handler, SIGINT);
}


static bool badmaths_handler_init(interrupt_state_t *ref_old_handler,
                                   parser_state_t *int_state)
{   return handler_init(ref_old_handler, int_state, SIGFPE,
                        "floating point exception");
}


static void badmaths_handler_end(interrupt_state_t *ref_old_handler)
{   handler_end(ref_old_handler, SIGFPE);
}



#endif /* __unix__ */


#ifdef _WIN32


typedef PHANDLER_ROUTINE interrupt_handler_fn;

typedef struct {
    interrupt_handler_fn *handler;
    parser_state_t *displaced_state;
} interrupt_state_t;


BOOL WINAPI interrupt_handler(DWORD dwCtrlType)
{
    if (global_int_state == NULL || !throw_signal(global_int_state, 2))
        exiting = TRUE;
    return TRUE/*handled*/;
}


#if 0
static bool cause_sigint(void)
{   return interrupt_handler(0) && exiting;
}
#endif



static bool interrupt_handler_init(interrupt_state_t *ref_old_handler,
                                   parser_state_t *int_state)
{   BOOL ok;

    exiting = FALSE;

    ok = SetConsoleCtrlHandler(&interrupt_handler, /*Add*/TRUE);

    OMIT(fprintf(stderr, "%s: installing interrupt handler %p\n",
                 codeid(), &interrupt_handler););

    if (ref_old_handler != NULL) {
        ref_old_handler->handler = NULL; /*not used in windows */
        ref_old_handler->displaced_state = global_int_state;
    }

    if (ok)
        return FALSE;
    else {
        global_int_state = int_state;
        return TRUE;
    }
}


static void interrupt_handler_end(interrupt_state_t *ref_old_handler)
{
    OMIT(BOOL ok;
         fprintf(stderr, "%s: installing interrupt returned to %p\n",
                 codeid(), ref_old_handler); fflush(stderr););
    OMIT(ok =)SetConsoleCtrlHandler(&interrupt_handler, /*Add*/FALSE);
    OMIT(fprintf(stderr, "%s: installing old interrupt complete\n",
                   codeid()););
    global_int_state = ref_old_handler->displaced_state;
    exiting = FALSE;
}



static bool badmaths_handler_init(interrupt_state_t *ref_old_handler,
                                   parser_state_t *int_state)
{   return true;
}


static void badmaths_handler_end(interrupt_state_t *ref_old_handler)
{   return;
}


#endif /* _WIN32 */


#else


/* no interrupt handling */


typedef void *interrupt_handler_fn;

typedef struct {
    interrupt_handler_fn *handler;
    parser_state_t *displaced_state;
} interrupt_state_t;

#define interrupt_handler_init(ref_old_handler, state) \
        ((void)(ref_old_handler),TRUE)
#define interrupt_handler_end(ref_old_handler)

#define badmaths_handler_init(ref_old_handler, state) \
        ((void)(ref_old_handler),TRUE)
#define badmaths_handler_end(ref_old_handler)


#if 0
static bool cause_sigint(void)
{   return FALSE; /* not handled */
}
#endif



#endif /* EXIT_ON_CTRL_C */




static void interrupt_ignore(void)
{   exiting = FALSE;
}



static bool interrupt(void)
{   if (exiting) {
        fprintf(stdout, "%s: Ctrl-C exit request detected\n", codeid());
        fflush(stdout);
    }
    return exiting;
}





/*****************************************************************************
 *                                                                           *
 *          Command Line Interpreter                                         *
 *          ========================                                         *
 *                                                                           *
 *****************************************************************************/





/* forward reference */
static const value_t *
mod_exec(const char **ref_line, parser_state_t *state);

/* forward reference */
static bool
mod_execv(const value_t **out_val,
          const char ***ref_argv, int *ref_argn, const char *delim,
          const char *execpath, dir_t *dir, parser_state_t *state);






/* This function may cause a garbage collection */
static const value_t *
mod_invoke_cmd(const char **ref_line, const value_t *value,
               parser_state_t *state)
{   DEBUG_TRACE(DPRINTF("mod exec: %s %s\n",
                       value_type_name(value), *ref_line);)
    DEBUG_ENV(VALUE_SHOW("mod exec env: ", dir_value(parser_env(state)));)

    if (value_type_equal(value, type_closure))
    {   const value_t *code;
        dir_t *env;
        const value_t *unbound;

        value_closure_get(value, &code, &env, &unbound);

        if (NULL == code)
        {   parser_error(state, "closure has no code part to execute:");
            parser_value_print(state, value);
            fprintf(stderr, "\n");
            return &value_null;
        } else
        if (value_type_equal(code, type_cmd))
        {   /* special case - commands do their own parsing */
            DEBUG_MOD(DPRINTF("%s: invoke closure code\n", codeid());)
            value = mod_invoke_cmd(ref_line, code, state);
            return value;
        } else
        {   if (parse_substitution_args(ref_line, state, &value))
            {   DEBUG_MOD(DPRINTF("%s: invoke closure non-code\n", codeid());)
                return value_nl(invoke(value, state));
            } else
            {   parser_error(state, "error in parsing closure arguments\n");
                return &value_null;
            }
        }
    } else
    if (value_type_equal(value, type_cmd))
    {   value_cmd_t *cmd = (value_cmd_t *)value;
        DEBUG_MOD(DPRINTF("%s: invoke direct cmd at '%.10s...'\n",
                         codeid(), *ref_line);)
        return (*cmd->exec)(ref_line, value, state);
    } else
    if (value_type_equal(value, type_code))
    {   const char *start = *ref_line;

        if (parse_space(ref_line) && parse_empty(ref_line))
        {   const char *buf;
            size_t len;

            DEBUG_MOD(DPRINTF("%s: invoke direct code at '%.10s..'\n",
                             codeid(), *ref_line);)
            value_code_buf(value, &buf, &len);
            OMIT(printf("executing: '%.*s'[%d]\n", len, buf, len);)
            /* will garbage collection the discarded value */
            if (parse_cmdlist(&buf, state, &value))
                return value;
            else
            {   parser_error(state, "code execution failed - %s\n", *ref_line);
                return &value_null;
            }
        } else
        {   parser_error(state, "code value can have no arguments\n");
            *ref_line = start;
            return &value_null;
        }
    } else
    if (value_type_equal(value, type_func))
    {   if (parse_substitution_args(ref_line, state, &value))
        {   DEBUG_MOD(DPRINTF("%s: invoke direct function\n", codeid());)
            return invoke(value, state);
        } else
        {   parser_error(state, "error in parsing closure arguments\n");
            return &value_null;
        }
    } else
    if (value_type_equal(value, type_dir))
    {   dir_stack_pos_t pos = parser_env_push(state, (dir_t *)value,
                                              /*outer_visible*/FALSE);
        OMIT(DPRINTF("%s: push enclosing directory %p\n", codeid(), value);)
        DEBUG_MOD(DPRINTF("%s: push enclosing directory\n", codeid());)
        value = mod_exec(ref_line, state);
        /* return to external environment */
        (void)parser_env_return(state, pos);
        DEBUG_MOD(DPRINTF("%s: popped enclosing directory\n", codeid());)
        return value;
    }
    else
        return value;
}







static size_t
argv_to_string(const char ***ref_argv, int *ref_argn, const char *delim,
               char *buf, size_t len)
{   char *bufstart = buf;
    bool complete = *ref_argn <= 0;

    while (!complete)
    {   const char *next = (*ref_argv)[0];
        size_t n;

        if (delim == NULL && next[0] == '-')
            complete = TRUE;
        else
        {
            if (delim != NULL && 0 == strcmp(delim, next))
                complete = TRUE;
            else {
                if (NULL != strchr(next, ' ') || NULL != strchr(next, '\n')  ||
                    NULL != strchr(next, '\t'))
                    n = snprintf(buf, len, "\"%s\" ", next);
                else
                    n = snprintf(buf, len, "%s ", next);

                if (n < len)
                {   len -= n;
                    buf += n;
                } else
                {   buf += len;
                    len = 0;
                }
                DEBUG_ARGV(printf("%s: shift argv string %d '%s'\n",
                                  codeid(), *ref_argn, (*ref_argv)[0]););
                (*ref_argv)++;
                (*ref_argn)--;
                complete = *ref_argn <= 0;
            }
        }
    }

    if (buf != bufstart)
    {   /* something written */
        buf[-1] = '\0';
        len--;
    } else
    {   buf[0] = '\0';
        buf++;
    }

    return buf - bufstart;
}









/*! Parse the prefix symbols from argv as arguments for the invocation of a
 *  FTL value.
 *
 *    @param ref_argv  - updatable reference to 1st sym in an array of symbols
 *    @param ref_argn  - updatable reference number of syms in array
 *    @param execpath  - directory path from which to take FTL sources
 *    @param delim     - NULL or a marker for end of sequence to execute
 *    @param value     - the value to execute
 *    @param state     - current parser state
 *
 *    @return TRUE iff a command was available (value might be NULL if the
 *            command was not found)
 *
 *  If delim is NULL we expect an --opt arg arg style of prefix to the symbols.
 *
 *  After execution the locations referred to by ref_argv and ref_argn are
 *  updated to point to the remainder of the symbol list.
 *
 *  This function may cause a garbage collection
 */
/* This function may cause a garbage collection */
static const value_t *
mod_invoke_argv(const char *fnname,
                const char ***ref_argv, int *ref_argn, const char *execpath,
                const value_t *value, const char *delim, parser_state_t *state)
{   DEBUG_TRACE(DPRINTF("mod argv exec: %s [%d]\n",
                       value_type_name(value), *ref_argn););
    DEBUG_ENV(VALUE_SHOW("mod argv exec env: ", dir_value(parser_env(state))););
    DEBUG_ARGV(DPRINTF("%s: mod argv %d '%s' delim '%s'\n",
                       codeid(), *ref_argn, (*ref_argv)[0], delim););

    if (value_type_equal(value, type_closure))
    {   const value_t *code = NULL;
        dir_t *env = NULL;
        const value_t *unbound = NULL;
        const value_t *retval = &value_null;

        value_closure_get(value, &code, &env, &unbound);

        if (NULL == code)
        {   parser_error(state, "closure has no code part to execute:");
            parser_value_print(state, value);
            fprintf(stderr, "\n");
        } else
        if (value_type_equal(code, type_cmd))
        {   /* special case - commands do their own parsing */
            DEBUG_MOD(DPRINTF("%s: invoke argv closure code\n", codeid()););
            retval = mod_invoke_argv(fnname, ref_argv, ref_argn, execpath,
                                     code, delim, state);
            /* i.e. use code below to process cmd */
        } else
        {   DEBUG_MOD(DPRINTF("%s: invoke argv closure argument\n",
                             codeid()););
            if (parse_substitution_argv(fnname, ref_argv, ref_argn, state,
                                        delim, /*token_strings*/delim==NULL,
                                        &value))
            {   retval = value;
                DEBUG_MOD(DPRINTF("%s: applied argv substitution OK\n",
                                 codeid()););
            } else
            {   parser_error(state, "error in parsing closure arguments\n");
                retval = &value_null;
            }
        }
        return retval;
    } else
    if (value_type_equal(value, type_cmd))
    {   value_cmd_t *cmd = (value_cmd_t *)value;
        char linebuf[FTL_STRING_MAX];
        const char *line = &linebuf[0];
        /*size_t len = */(void)argv_to_string(ref_argv, ref_argn, delim,
                                              &linebuf[0], sizeof(linebuf));

        DEBUG_MOD(DPRINTF("%s: invoke argv direct cmd - arg \"%s\"\n",
                         codeid(), line););
        return (*cmd->exec)(&line, value, state);
    } else
    if (value_type_equal(value, type_code))
    {   const char *buf = NULL;
        size_t len = 0;

        value_code_buf(value, &buf, &len);
        DEBUG_MOD(DPRINTF("%s: invoke argv direct code - '%.10s\n",
                         codeid(), buf);)
        OMIT(printf("executing: '%s'[%d]\n", buf, len);)
        /* will garbage collection the discarded value */
        if (parse_cmdlist(&buf, state, &value))
            return value;
        else
        {   parser_error_longstring(state, buf, "code execution failed -");
            return &value_null;
        }
    } else
    if (value_type_equal(value, type_func))
    {
        DEBUG_MOD(DPRINTF("%s: invoke argv direct fn\n", codeid()););
        if (parse_substitution_argv(fnname, ref_argv, ref_argn, state,
                                    delim, /*token_strings*/delim==NULL,
                                    &value))
        {   DEBUG_MOD(DPRINTF("%s: invoked argv direct function\n", codeid()););
            return value;
        } else
        {   parser_error(state, "error in parsing function arguments\n");
            return &value_null;
        }
    } else
    if (value_type_equal(value, type_dir))
    {   dir_stack_pos_t pos = parser_env_push(state, (dir_t *)value,
                                              /*outer_visible*/FALSE);
        OMIT(DPRINTF("%s: push enclosing directory %p\n", codeid(), value););
        DEBUG_MOD(DPRINTF("%s: push enclosing argv directory\n", codeid()););
        if (!mod_execv(&value, ref_argv, ref_argn, delim, execpath,
                       parser_env(state), state))
            value = &value_null;
        /* return to external environment */
        (void)parser_env_return(state, pos);
        DEBUG_MOD(DPRINTF("%s: popped enclosing argv directory\n", codeid());)
        return value;
    }
    else
        return value;
}






/* This function may cause a garbage collection */
static const value_t *
mod_exec(const char **ref_line, parser_state_t *state)
{   const value_t *value = NULL;
    dir_t *dir = parser_env(state);

    if (mod_parse_cmd(dir, ref_line, &value) && parse_space(ref_line)) {
        DEBUG_MODEXEC(fprintf(stderr, "%s: exec - found cmd\n", codeid()););
        return mod_invoke_cmd(ref_line, value, state);
    } else {
        /* execute command with FN_UNDEF_HOOK */
        const value_t *pcmdsval =
            dir_stringl_get(dir_stack_dir(state->env),
                            FTLDIR_PARSE, strlen(FTLDIR_PARSE));
        if (pcmdsval != NULL && value_type_equal(pcmdsval, type_dir))
        {
            dir_t *pcmds = (dir_t *)pcmdsval;
            const value_t *on_undef = dir_stringl_get(pcmds, FN_UNDEF_HOOK,
                                                      strlen(FN_UNDEF_HOOK));
            if (on_undef != NULL && on_undef != &value_null) {
                DEBUG_MODEXEC(fprintf(stderr, "%s: exec - found undef handler\n",
                                     codeid()););
                return mod_invoke_cmd(ref_line, on_undef, state);
            } else {
                DEBUG_MODEXEC(fprintf(stderr, "%s: exec - not found\n",
                                     codeid()););
                return NULL;
            }
        } else
            return NULL;
    }
}





static bool mod_parse_script(const value_t **out_val,
                             const char ***ref_argv, int *ref_argn,
                             const char *delim,
                             const char *execpath, 
                             parser_state_t *state)
{   const char *cmd_file = (*ref_argv)[0];
    charsource_t *fin =
        charsource_file_path_new(execpath, cmd_file, strlen(cmd_file));
    bool parsed_ok = FALSE;
    const value_t *value = NULL;

    DEBUG_EXECV(printf("open '%s' on path '%s' - %s\n", cmd_file, execpath,
                      NULL == fin? "failed": "OK"););
    if (NULL != fin)
    {   const char **script_argv = *ref_argv;
        int script_argn = *ref_argn;
        const value_t *orig_argvec = NULL;
        const value_t *pcmdsval =
            dir_stringl_get(dir_stack_dir(state->env), FTLDIR_PARSE,
                            strlen(FTLDIR_PARSE));
        dir_t *pcmds = NULL;

        parsed_ok = TRUE;

        (*ref_argv)++;
        (*ref_argn)--;

        /* give the rest of the line's arguments to the script */
        if (delim != NULL)
        {   while (*ref_argn > 0 && NULL != (*ref_argv)[0] &&
                   0 != strcmp((*ref_argv)[0], delim))
            {   (*ref_argv)++;
                (*ref_argn)--;
            }
        }
        else
        {   /* this case is a little more dodgy than the delimited one
               if the --optfile is the last option before normal arguments
               the normal arguments will be included unless the user makes
               use of the -- option
            */
            while (*ref_argn > 0 && NULL != (*ref_argv)[0] &&
                   (*ref_argv)[0][0] != '-')
            {   (*ref_argv)++;
                (*ref_argn)--;
            }
            if (0 != strcmp((*ref_argv)[0], "--"))
            {   (*ref_argv)++;
                (*ref_argn)--;
            }
        }
        script_argn = script_argn - *ref_argn;

        if (pcmdsval != NULL && value_type_equal(pcmdsval, type_dir)) {
            /* overwrite parse.argv with scripts arguments */
            dir_t *argvec = dir_argvec_new(script_argn, script_argv);

            pcmds = (dir_t *)pcmdsval;
            orig_argvec = dir_stringl_get(pcmds, "argv", strlen("argv"));
            mod_add_dir(pcmds, "argv", argvec);
        } else
            printf("%s: no 'parse' directory\n", codeid());

        value = parser_expand_exec(state, fin,
                                   /*cmdstr*/NULL, /*rcfile*/NULL,
                                   /*no_locals*/TRUE);
        if (NULL == value)
        {   fprintf(stderr, "%s: attempted execution of "
                    "file \"%s\" failed\n",
                    codeid(), cmd_file);
            parsed_ok = FALSE;
        }

        if (orig_argvec != NULL) {
            /* restore original arguments */
            mod_add_dir(pcmds, "argv", (dir_t *)orig_argvec);
        }
    }
    if (parsed_ok)
        *out_val = value;

    return parsed_ok;
}




/*! Parse and execute the prefix symbols from argv until the next
 *  delimiter (e.g. comma) or until '--'
 *
 *    @param out_val   - value returned by prefix symbols (NULL on error)
 *    @param ref_argv  - updatable reference to 1st sym in an array of symbols
 *    @param ref_argn  - updatable reference number of syms in array
 *    @param delim     - NULL or a marker for end of sequence to execute
 *    @param execpath  - directory path from which to take FTL sources
 *    @param state     - current parser state
 *
 *    @return TRUE iff a command was available (value might be NULL if the
 *            command was not found)
 *
 *  If delim is NULL we expect an --opt arg arg style of prefix to the symbols.
 *
 *  After execution the locations referred to by ref_argv and ref_argn are
 *  updated to point to the remainder of the symbol list.
 *
 *  This function may cause a garbage collection
 */
static bool
mod_execv(const value_t **out_val,
          const char ***ref_argv, int *ref_argn, const char *delim,
          const char *execpath, dir_t *dir, parser_state_t *state)
{   const value_t *value = NULL;
    bool parsed_ok = FALSE;
    const char *symbol = (*ref_argv)[0]; 

    DEBUG_ARGV(DPRINTF("%s: mod execv argv %d '%s' delim %s\n",
                       codeid(), *ref_argn, (*ref_argv)[0],
                       delim==NULL? "<none>": delim););
    DEBUG_EXECV(DPRINTF("%s; mod execv start: %s ...[%d] "
                        "(path %s delim '%s')\n",
                        codeid(), symbol, *ref_argn,
                        execpath == NULL? "<NONE>": execpath,
                        delim==NULL? "<unset>": delim););
    /* Parse the command first */
    if (delim != NULL? mod_parse_argv(dir, ref_argv, ref_argn, &value):
                       mod_parse_optargv(dir, ref_argv, ref_argn, &value))
    {   DEBUG_EXECV(DPRINTF("mod execv parsed %s '%s'\n",
                           value_type_name(value), symbol););
        if (value == NULL || value == &value_null)
        {   value = NULL; /* signal an error */
            parser_error(state, "%s ignored '%s'\n",
                         delim==NULL? "option": "function", symbol);
        }
        else
        {   /* delim unlikely to be NULL ... --<opt> won't usually match a file
             * name */
            parsed_ok = TRUE;
            /* execute the command with the next tokens in argv */
            value = mod_invoke_argv(symbol, ref_argv, ref_argn,
                                    execpath, value, delim, state);
        }
    }
    else
    /* try to execute the option on execpath if it exists */
    if (execpath != NULL && *ref_argn > 0)
    {
        parsed_ok = mod_parse_script(&value, ref_argv, ref_argn,
                                     delim, execpath, state);
    }
    
    if (!parsed_ok)
    {
        /* try to execute command with FN_UNDEF_HOOK */
        const value_t *pcmdsval =
            dir_stringl_get(dir_stack_dir(state->env),
                            FTLDIR_PARSE, strlen(FTLDIR_PARSE));
        if (pcmdsval != NULL && value_type_equal(pcmdsval, type_dir))
        {
            dir_t *pcmds = (dir_t *)pcmdsval;
            const value_t *on_undef = dir_stringl_get(pcmds, FN_UNDEF_HOOK,
                                                      strlen(FN_UNDEF_HOOK));
            if (on_undef != NULL && on_undef != &value_null) {
                DEBUG_MODEXEC(fprintf(stderr, "%s: exec - found undef handler\n",
                                     codeid()););
                parsed_ok = TRUE;
                value = mod_invoke_argv(symbol, ref_argv, ref_argn, execpath,
                                        on_undef, delim, state);
               
            } else {
                DEBUG_MODEXEC(fprintf(stderr, "%s: exec - not found\n",
                                     codeid()););
                value = NULL;
            }
        }        
    }
    DEBUG_EXECV(if (!parsed_ok)
                   DPRINTF("mod execv can't parse %s '%s'\n",
                            value_type_name(value), symbol););
                    
    DEBUG_EXECV(if (*ref_argn <= 0)
                   DPRINTF("mod execv end: FINISHED\n");
               else
                   DPRINTF("mod execv end: %s ...[%d]\n",
                           (*ref_argv)[0], *ref_argn);
    );
    *out_val = value;
    return parsed_ok;
}







/* This function (and all command functions) will return NULL to indicate that
   no command was found for execution
   parses:

       [<directory>]* <command-in-directory>
           (subsequent <command args> are parsed by the <command-in-directory>)

       [<directory>]* <code|binding|fn-in-directory> <vec-args>
           (vector is parsed and given as argument to command)

   This function may cause a garbage collection
*/
extern const value_t *
mod_exec_cmd(const char **ref_line, parser_state_t *state)
{   FILE *log = stdout;
    const char *fmt = NULL;
    if (parser_echoto(state, &log, &fmt))
    {   fprintf(log, fmt != NULL? fmt: "%s\n", *ref_line);
        fflush(log);
    }
    return mod_exec(ref_line, state);
}






static const value_t *mod_exec_cmd_call(parser_state_t *state, void *call_arg)
{
    const char **ref_line = (const char **)call_arg;
    return mod_exec_cmd(ref_line, state);
}


/* execute a command with an enclosing 'try' block */
static const value_t *
mod_exec_cmd_caught(const char **ref_line, parser_state_t *state, bool *ref_ok)
{   return parser_catch_call(state, &mod_exec_cmd_call,
                             (void *)ref_line, ref_ok);
}




/* make upper case copy of string */
static char *
strnuccpy(char *dest, const char *src, size_t n)
{   while (n > 0 && '\0' != *src)
    {   *dest++ = toupper(*src++);
        n--;
    }
    if (n > 0)
        *dest = '\0';

    return dest;
}






static FILE *
rcfile_run(parser_state_t *state, const char *code_name)
{   char code_name_uc[64];
    char rcfile_env[64];
    char rcfile_name_buf[256];
    const char *rcfile_name;
    FILE *rcfile = NULL;
    bool using_default = FALSE;

    strnuccpy(&code_name_uc[0], code_name, sizeof(code_name_uc));
    snprintf(&rcfile_env[0], sizeof(rcfile_env), "%s%s",
             &code_name_uc[0], ENV_FTL_RCFILE_POST);
    rcfile_name = getenv(rcfile_env);

    if (rcfile_name == NULL)
    {    rcfile_name = &rcfile_name_buf[0];
         snprintf(&rcfile_name_buf[0], sizeof(rcfile_name_buf),
                  ENV_FTL_RCFILE_DEFAULT(code_name));
         using_default = TRUE;
         DEBUG_RCFILE(DPRINTF("%s: rc file '%s'\n", code_name, rcfile_name););
    }

    if (rcfile_name != NULL)
    {   rcfile = fopen(rcfile_name, "r");
        if (NULL == rcfile)
        {   const char *homedir = getenv(ENV_FTL_HOMEDIR);
            if (NULL != homedir)
            {   char *rc_home = FTL_MALLOC(strlen(homedir)+
                                           strlen(ENV_FTL_HOMEDIR_HOME)+
                                           strlen(OS_FILE_SEP)+
                                           strlen(rcfile_name)+1);
                if (NULL != rc_home)
                {   sprintf(rc_home, "%s%s"OS_FILE_SEP"%s",
                            homedir, ENV_FTL_HOMEDIR_HOME, rcfile_name);
                    DEBUG_RCFILE(DPRINTF("%s: full rc file name '%s'\n",
                                        code_name, rc_home););
                    rcfile = fopen(rc_home, "r");
                    if (NULL == rcfile && !using_default)
                        DPRINTF("%s: can't open RC file '%s'\n",
                                code_name, rc_home);
                    FTL_FREE(rc_home);
                }
            }
            DEBUG_RCFILE(else DPRINTF("%s: no %s directory\n",
                                     code_name, ENV_FTL_HOMEDIR);)
        }
    }

    if (rcfile != NULL)
    {   charsource_t *rcstream = charsource_stream_new(rcfile, rcfile_name,
                                                       TRUE);
        linesource_push(parser_linesource(state), rcstream);
    }

    DEBUG_RCFILE(
        else
           printf("%s: didn't open rcfile %s: %s (rc %d)\n",
                  code_name, rcfile_name==NULL?"<>": rcfile_name,
                  strerror(errno), errno);)

    return rcfile;
}








static bool default_register_result(parser_state_t *state, const char *cmd,
                                    const value_t *result, void *arg)
{
    bool ok = TRUE;
    
    if (!value_type_equal(result, type_null))
    {
        /* printf("RESULT: "); */
        value_fprint(stdout,
                     dir_value(parser_root(state)), result);
        fprintf(stdout, "\n");
    }

    return ok;
}





/*! Execute argv as delimiter or option separated ftl commands
 *
 *  The argv provided is updated and provides the remaining tokens.
 *    @param state     - current parser state
 *    @param execpath  - file directory path from which to take FTL sources
 *    @param ref_argv  - updatable reference to 1st sym in an array of symbols
 *    @param ref_argn  - updatable reference number of syms in array
 *    @param delim     - NULL for --opt processing otherwise a delimiter token
 *    @param fndir     - directory to take parsing function definitions from
 *    @param with_results - function for dealing with command results
 *    @param with_results_arg - argument to provide with \c with_results
 *    @param out_value - last value created by the commands
 *    @param out_ends_with_delim - TRUE when last parsed item is the delimiter
 *
 *  If delim is NULL only the initial commands that consist of --opt style
 *  tokens are parsed (until the following --opt style entry or until --).
 *  Otherwise command sequences between delim options are parsed.
 *
 *  This function may cause a garbage collection
 */
extern bool
parser_argv_exec(parser_state_t *state, const char ***ref_argv, int *ref_argn,
                 const char *delim, const char *execpath, dir_t *fndir,
                 bool expect_no_locals,
                 register_opt_result_fn *with_results, void *with_results_arg,
                 const value_t **out_value, bool *out_ends_with_delim)
{   const value_t *val = NULL;
    linesource_t saved;
    bool executing_prefix = TRUE;
    bool syntax_ok = TRUE;

    OMIT(DIR_SHOW_ST("Current dir in exec: ", state, parser_env(state)););
    OMIT(DIR_SHOW_ST("Execution dir in exec: ", state, fndir););
    linesource_save(parser_linesource(state), &saved);
    linesource_init(parser_linesource(state));

    while (executing_prefix && *ref_argn > 0)
    {   const char *cmd;
        DEBUG_ENV(dir_t *startdir = parser_env_stack(state)->stack;);

        cmd = (*ref_argv)[0];
        DEBUG_ARGV(printf("%s: %d args left - current args '%s'\n",
                          codeid(), *ref_argn, cmd););

        if (NULL != val)
            value_unlocal(val);

        if (0 == strcmp(cmd, "--"))
        {   (*ref_argn)--;
            (*ref_argv)++;
            executing_prefix = FALSE;
        } else
        {
            DEBUG_EXECV(DPRINTF("MOD EXECV (%s) --- %s ...[%d]\n",
                               delim == NULL? "opt": delim,
                               (*ref_argv)[0], *ref_argn););
            executing_prefix = mod_execv(&val, ref_argv, ref_argn, delim,
                                         execpath, fndir, state);
            DEBUG_EXECV(DPRINTF("MOD EXECV --- %s\n",
                               executing_prefix? "OK": "FAILED"););
            if (executing_prefix && val == NULL)
                syntax_ok = FALSE;
        }

        if (out_ends_with_delim != NULL)
            *out_ends_with_delim = FALSE;

        if (executing_prefix)
        {   DEBUG_EXECV(VALUE_SHOW("MOD EXECV report val: ", val););
            if (val!= NULL)
            {   if (with_results != NULL)
                    executing_prefix = (*with_results)(state, cmd, val,
                                                       with_results_arg);
            } else
            {   parser_error(state, "unknown command '%s'\n", cmd);
                executing_prefix = FALSE;
            }

            if (delim != NULL)
            {   /* skip intervening delim (e.g. ",") arguments */
                if (*ref_argn > 0 && 0 == strcmp((*ref_argv)[0], delim)) {
                    (*ref_argn)--;
                    (*ref_argv)++;
                    if (out_ends_with_delim != NULL)
                        *out_ends_with_delim = TRUE;
                }
            }

            DEBUG_ENV(
                if (parser_env_stack(state)->stack != startdir)
                {   DEBUG_ENV(printf("%s: env update %p -> %p\n",
                                    codeid(), startdir,
                                    parser_env(state));)
                    DIR_SHOW("start:  ", startdir);
                    DIR_SHOW("end:    ", parser_env(state));
                }
            )
        }
    }

    if (expect_no_locals)
    {   value_unlocal(val);
        val = NULL; /* mustn't return garbage collected value */
        parser_collect(state);
    }

    linesource_restore(parser_linesource(state), &saved);

    /* last value returned */
    *out_value = (val == NULL? &value_null: val);
    return syntax_ok;
}






static void
parser_report_trailing(parser_state_t *state, const char *msg,
                       const char *phrase)
{
    char *p;
    ptrdiff_t width;
    int lines = 0;

    p = strchr(phrase, '\n');
    width = p == NULL? (int)strlen(phrase)+1: p - phrase;
    /*printf("%s: width of trailing line %d is %d\n", codeid(), lines, width);*/
    parser_report(state, "%s%.*s\n", msg, width, phrase);
    while (p != NULL && lines < FTL_ERROR_TRAIL_LINES) {
        phrase += width+1;
        p = strchr(phrase, '\n');
        width = p == NULL? strlen(phrase): p - phrase;
        /*printf("%s: width of trailing line %d is %d\n",
          codeid(), lines, width); */
        parser_report(state, "%.*s\n", (int)width, phrase);
        lines++;
    }
}



/* This function may cause a garbage collection */
extern const value_t *
parser_expand_exec_int(parser_state_t *state, charsource_t *source,
                       const char *cmd_str, const char *rcfile_id,
                       bool expect_no_locals, bool interactive)
{   const value_t *val = NULL;
    linesource_t saved;
    interrupt_state_t old_int_state;
    interrupt_state_t old_badmaths_state;
    FILE *resultout = interactive? stdout: NULL;

    OMIT(DIR_SHOW_ST("Current dir in exec: ", state, parser_env(state));)
    linesource_save(parser_linesource(state), &saved);
    linesource_init(parser_linesource(state));

    if (NULL != source)
        linesource_push(parser_linesource(state), source);

    if (NULL != cmd_str)
        linesource_pushline(parser_linesource(state), "<INIT>", cmd_str);

    if (NULL != rcfile_id)
        (void)rcfile_run(state, rcfile_id);

    if (interactive)
        (void)interrupt_handler_init(&old_int_state, state);
    (void)badmaths_handler_init(&old_badmaths_state, state);

    while (!linesource_eof(parser_linesource(state)))
    {
        if (interactive && interrupt()) {
            OMIT(fprintf(stderr, "-- exiting\n"););
            if (source == parser_charsource(state)) {
                fprintf(stderr, "%s: still reading initial source\n", codeid());
                interrupt_ignore();
            } else {
                /* retreat to back to the main source */
                linesource_t *linesource = parser_linesource(state);
                /*GRAY: TODO remove debugging */
                fprintf(stderr, "%s: popping input stack\n", codeid());
                while (linesource != NULL &&
                       linesource_charsource(linesource) != source)
                    (void)linesource_popdel(linesource);
                fprintf(stderr, "%s: linesource %s\n", codeid(),
                        linesource==NULL? "exhausted": "back to original");
            }
        } else
        {   const char *phrase;
            size_t len;
            charsink_string_t line;
            charsink_t *linesink = charsink_string_init(&line);
            charsink_string_t expandline;
            charsink_t *expandsink = charsink_string_init(&expandline);
            code_place_t line_start_pos;
            //charsource_rewind_t linestart_src;

            DEBUG_LNO(DPRINTF("\n%s: pre-line at %s:%d\n",
                              codeid(), parser_source(state),
                              parser_lineno(state)););
            code_place_set(&line_start_pos,
                           parser_source(state), parser_lineno(state));
            linesource_start_set(parser_linesource(state), &line_start_pos);
            /* copy line from source to linesink and get buffer */
            linesource_read(parser_linesource(state), linesink);
            /* TODO: wrap linesource_read in its own try block? to catch
             * ctrl-C? */
            charsink_string_buf(linesink, &phrase, &len);
            DEBUG_LNO(DPRINTF("%s: post-line at %s:%d - pre-expand\n",
                              codeid(), parser_source(state),
                              parser_lineno(state)););

            /* expand \ and $ in buffer writing to expandsink and get buffer */
            parser_expand(state, expandsink, phrase, len);
            charsink_string_buf(expandsink, &phrase, &len);
            charsink_string_close(linesink);

            /* now parse the text expanded into expandsink */
            parse_space(&phrase);

            if (!parse_empty(&phrase))
            {
                charsource_lineref_t phrase_src;
                charsource_t *linestart_src;
                DEBUG_ENV(dir_t *startdir = parser_env_stack(state)->stack;);
                OMIT(DPRINTF("%s: parsing line '%s'\n", codeid(), phrase););
                linestart_src = charsource_lineref_init
                                    (&phrase_src, /*del*/NULL, /*rewind*/TRUE,
                                     &line_start_pos.posname[0],
                                     line_start_pos.lineno, &phrase);
                linesource_push(parser_linesource(state), linestart_src);

                DEBUG_LNO(DPRINTF("%s: post-expand %sline now %s:%d "
                                  "(chars still %s:%d)\n",
                                  codeid(), phrase[0] == '\0'? "EMPTY ":"",
                                  &line_start_pos.posname[0],
                                  line_start_pos.lineno,
                                  parser_source(state), parser_lineno(state));
                          parser_error(state, "demo error here\n");
                    );

                if (NULL != val)
                    value_unlocal(val);

                /* parse and execute the line */
                if (interactive)
                {   bool ran_ok = TRUE;
                    val = mod_exec_cmd_caught(&phrase, state, &ran_ok);
                    if (!ran_ok)
                    {    fprintf(stderr, "%s: exception - ", codeid());
                         value_fprint(stderr, dir_value(parser_env(state)),
                                      val);
                         fprintf(stderr, "\n");
                         val = &value_null;
                    }
                } else
                    val = mod_exec_cmd(&phrase, state);

                if (val != NULL)
                {   if (!parse_empty(&phrase)) {
                        parser_report_trailing(state,
                                               "warning - trailing text ...",
                                               phrase);
                    }
                    if (!value_type_equal(val, type_null) && NULL != resultout)
                    {   value_fprint(resultout,
                                     dir_value(parser_root(state)), val);
                        fprintf(resultout, "\n");
                    }
                } else
                    parser_error(state, "unknown command '%s'\n", phrase);

                DEBUG_ENV(
                    if (parser_env_stack(state)->stack != startdir)
                    {   DEBUG_ENV(printf("%s: env update %p -> %p\n",
                                        codeid(), startdir,
                                        parser_env(state));)
                        DIR_SHOW("start:  ", startdir);
                        DIR_SHOW("end:    ", parser_env(state));
                    }
                )
                if (expect_no_locals)
                {   value_unlocal(val);
                    val = NULL; /* mustn't return garbage collected value */
                    parser_collect(state);
                }
                linesource_remove(parser_linesource(state), linestart_src);
            }
            charsink_string_close(expandsink);
        }
        OMIT(fprintf(stderr, "-- cmd over\n"););
    }

    OMIT(fprintf(stderr, "-- session over\n"););
    linesource_restore(parser_linesource(state), &saved);

    badmaths_handler_end(&old_badmaths_state);
    
    if (interactive)
        interrupt_handler_end(&old_int_state);

    return val == NULL? &value_null: val;
}








/* This function may cause a garbage collection */
extern void
cli(parser_state_t *state, const char *init_cmds, const char *code_name)
{   charsource_t *console;

    codeid_set(code_name);
    console = charsource_console_new("> ");

    (void)parser_expand_exec_int(state, console, init_cmds,
                                 /*rc_file*/code_name, /*expect_no_locals*/TRUE,
                                 /*interactive*/TRUE);
}









/* This function may cause a garbage collection */
extern bool
argv_cli_ending(parser_state_t *state, const char *code_name,
                const char *execpath, const char **argv, int argc,
                bool *out_ends_with_comma)
{   const value_t *value = NULL;
    codeid_set(code_name);
    return parser_argv_exec(state, &argv, &argc, /*delim*/",", execpath,
                            parser_env(state),
                            /* expect_no_locals */TRUE,
                            /* function dealing with results from command
                             * evaluations */&default_register_result,
                            /*with_results_arg*/NULL,
                            &value, out_ends_with_comma);
}





/* This function may cause a garbage collection */
extern bool
argv_opt_cli(parser_state_t *state, const char *code_name, const char *execpath,
             const char ***ref_argv, int *ref_argc, dir_t *fndir,
             register_opt_result_fn *with_results, void *with_results_arg)
{   const value_t *value = NULL;
    bool ends_with_delim = FALSE;
    codeid_set(code_name);
    return parser_argv_exec(state, ref_argv, ref_argc,
                            /*delimiter*/NULL/*use --opt style parsing*/,
                            execpath, fndir, /*expect_no_locals*/TRUE,
                            with_results, with_results_arg,
                            &value, &ends_with_delim);
}












/*****************************************************************************
 *                                                                           *
 *          Values - Formatted Output                                        *
 *          =========================                                        *
 *                                                                           *
 *****************************************************************************/






enum
{   ff_left = 1,    /* left justify in width - otherwise right */
    ff_zero,        /* pad with leading zeros */
    ff_sign,        /* always precede with place for a sign */
    ff_posv,        /* use positive sign as well as a negative one only */
    ff_alt          /* "alternative" format */
};


typedef int fprint_flags_t; /* bit set of enum values */



static dir_t *print_formats = NULL;






extern void
printf_addformat(type_t for_type, const char *fmtchar,
                 const char *help, func_fn_t *exec)
{   (void)for_type; /* currently OMITd */

    if (NULL == print_formats)
        print_formats = dir_id_new();

    /* arguments have the form [intflags,precision,val]:{...} */
    if (NULL != print_formats)
        mod_addfn(print_formats, fmtchar, help, exec, 3);
}






static bool
parse_format(const char **ref_line, parser_state_t *state,
             const value_t **out_fmtfn,
             fprint_flags_t *out_flags, int *out_precision, size_t *out_width)
{   /* syntax: [-][+][0][ ][#][<digits>*][.<digits>*][<fmtchar> | {<fmtexpr>}] */
    bool ok = TRUE;
    bool left = FALSE;
    bool sign = FALSE;
    bool zero = FALSE;
    bool posv = FALSE;
    bool alt  = FALSE;
    number_t precision = -1;
    number_t width = 0;
    const char *line = *ref_line;
    const value_t *fmtfn = NULL;

    while ((!left && (left = parse_key(ref_line,"-"))) ||
           (!posv && (sign = parse_key(ref_line,"+"))) ||
           (!sign && (sign = parse_key(ref_line," "))) ||
           (!zero && (zero = parse_key(ref_line,"0"))) ||
           (!alt && (alt = parse_key(ref_line,"#"))))
        continue;

    OMIT(printf("%s: %sleft %ssign %szero\n", codeid(),
                  left?"":"not ", sign?"":"not ", zero?"":"not ");)

    (void)parse_int(ref_line, &width);

    if (parse_key(ref_line,"."))
        ok = parse_int(ref_line, &precision);

    if (ok)
    {   if (parse_key(ref_line,"{"))
        {   ok = parse_expr(ref_line, state, &fmtfn) &&
                 parse_key(ref_line, "}");
            if (ok && (fmtfn == NULL || fmtfn == &value_null))
                ok = FALSE;
        } else if (isalpha((unsigned char)((*ref_line)[0])))
        {   char name[2];
            name[0] = (*ref_line)[0];
            name[1] = '\0';
            (*ref_line)++;
            fmtfn = dir_string_get(print_formats, &name[0]);
            if (fmtfn == NULL)
                ok = FALSE;
        } else
            ok = FALSE; /* no format */
    }

    if (ok)
    {   *out_fmtfn = fmtfn;
        if (left)
            zero = FALSE;
        *out_flags = (left? (1<<ff_left): 0) |
                     (posv? (1<<ff_posv): 0) |
                     (sign? (1<<ff_sign): 0) |
                     (zero? (1<<ff_zero): 0) |
                     (alt?  (1<<ff_alt): 0);
        *out_width = (int)width;
        *out_precision = (int)precision;
        return TRUE;
    } else
    {   *ref_line = line;
        return FALSE;
    }
}







static const value_t *
value_string_fmtfn(parser_state_t *state, const value_t *fmtfn,
                   fprint_flags_t flags, int precision,
                   const value_t *val)
{   /* execute fmtfn intflags precision val! to obtain string */
    const value_t *bind;
    const value_t *str = NULL;

    bind = substitute(fmtfn, value_int_new(flags &
                                           ((1<<ff_sign) | (1<<ff_zero))),
                      state, /*unstrict*/FALSE);
    if (NULL != bind)
    {   bind = substitute(bind, value_int_new(precision),
                          state, /*unstrict*/FALSE);
        if (NULL != bind)
        {   bind = substitute(bind, val, state, /*unstrict*/FALSE);
            if (NULL != bind)
                str = invoke(bind, state);
        }
    }

    return str;
}






static int
value_string_printfmt(charsink_t *sink,
                      parser_state_t *state, const value_t *fmtfn,
                      fprint_flags_t flags, int precision, size_t width,
                      const value_t *arg)
{   const value_t *str = value_string_fmtfn(state, fmtfn, flags,
                                            precision, arg);
    const char *strbuf = NULL;
    size_t strlen = 0;
    bool ok = FALSE;
    int len = 0;

    if (str == NULL)
        parser_report(state, "failed to execute format item\n");
    else if (!value_type_equal(str, type_string))
        parser_report(state, "format item returned a %s, "
                      "not a string\n",
                      value_type_name(str));
    else if (value_string_get(str, &strbuf, &strlen))
    {   ok = TRUE;
        if (strlen > width)
            len += charsink_write(sink, strbuf, strlen);
        else
        {   size_t i;
            OMIT(printf("%s: %sleft - width %d flags %d\n",
                          codeid(), 0 != (flags & (1<<ff_left))?"":"not ",
                          width, flags);)
            if (0 != (flags & (1<<ff_left)))
            {   len += charsink_write(sink, strbuf, strlen);
                for (i=0; i<width-strlen; i++)
                    len += charsink_putc(sink, ' ');
            } else
            {   char pad = (0 != (flags & (1<<ff_zero)))? '0':' ';
                for (i=0; i<width-strlen; i++)
                    len += charsink_putc(sink, pad);
                len += charsink_write(sink, strbuf, strlen);
            }
        }
    }
    return ok? len: -1;
}







extern int
value_fprintfmt(parser_state_t *state, charsink_t *sink,
                const char *format, size_t formatlen,
                dir_t *env)
{   /* syntax: .... %[[<otherfield>]]<format> .... */
    const char *endf = format+formatlen;
    const char *plain;
    int index = 0;
    int len = 0;

    while (format < endf)
    {   plain = format;
        while (format < endf && *format != '%')
            format++;
        if (format > plain)
            len += charsink_write(sink, plain, format-plain);

        if (format+1 < endf && format[1] == '%')
        {   format += 2;
            len += charsink_putc(sink, '%');
        } else
        if (format+1 < endf)
        {   const value_t *fmtfn = NULL;
            fprint_flags_t flags = 0;
            int precision = 0;
            size_t width = 0;
            const char *fmtstart = format++;  /* skip over '%' */
            const char *argname = NULL;
            int argnamelen = -1;

            if (*format == '[')
            {   argname = ++format;
                while (format < endf && '\0' != *format && ']' != *format)
                    format++;
                if (format < endf)
                {   argnamelen = (int)(format - argname);
                    format++; /* skip over terminal \0 or ] */
                }
            }

            if (parse_format(&format, state,
                             &fmtfn, &flags, &precision, &width))
            {   const value_t *arg;

                if (argnamelen >= 0)
                    arg = dir_stringl_get(env, argname, argnamelen);
                else
                {   arg = dir_int_get(env, index);
                    index++;
                }

                if (NULL == arg)
                    parser_report(state, "no value found to format - '%s'\n",
                                  fmtstart);
                else
                {   int chars = value_string_printfmt(sink, state, fmtfn,
                                                      flags, precision, width,
                                                      arg);
                    if (chars >= 0)
                        len += chars;
                }
            } else
            {   parser_report(state, "invalid format item syntax - '%s'\n",
                              fmtstart);
            }
        }
    }

    return len;
}







static void
fprint_init(void)
{    print_formats = NULL;
}






/*****************************************************************************
 *                                                                           *
 *          Commands - Closure                                               *
 *          ==================                                               *
 *                                                                           *
 *****************************************************************************/





static const value_t *
fn_closure(const value_t *this_fn, parser_state_t *state)
{   const value_t *val = &value_null;
    const value_t *inhval = parser_builtin_arg(state, 1);
    const value_t *argenvval = parser_builtin_arg(state, 2);
    const value_t *code = parser_builtin_arg(state, 3);
    bool nocode = value_type_equal(code, type_null);
    bool closure_env = value_type_equal(argenvval, type_closure);

    if (value_istype_bool(inhval) &&
        (closure_env || value_istype(argenvval, type_dir))) {
         if (nocode || value_is_codebody(code))
         {
             bool inherit = (inhval != value_false);
             value_env_t *newenvval = NULL;
             bool is_env_t;

             if (closure_env) {
                 argenvval = value_closure_env(argenvval);
                 is_env_t = TRUE;  /* argenvval is a value_env_t */
             } else
                 is_env_t = FALSE; /* argenvval is a dir_t */

             /* return to the calling environment before inheriting it */
             parser_env_return(state, parser_env_calling_pos(state));
             if (env_add(state, &newenvval, argenvval, is_env_t, inherit))
                 val = value_closure_new(nocode? NULL: code, newenvval);
         } else
            parser_error(state, "argument must be a command, function or "
                         "code object (not a %s)\n",
                         value_type_name(code));
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_bind(const value_t *this_fn, parser_state_t *state)
{   const value_t *closure = parser_builtin_arg(state, 1);
    const value_t *arg = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(closure, type_closure))
        val = value_closure_bind(closure, arg);
    else
        parser_report_help(state, this_fn);

    return val;
}



static const value_t *
fn_code(const value_t *this_fn, parser_state_t *state)
{   const value_t *closure = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(closure, type_closure))
    {
        dir_t *env = NULL;
        const value_t *unbound = NULL;

        (void)value_closure_get(closure, &val, &env, &unbound);
    } else
        parser_report_help(state, this_fn);

    return val;
}



static const value_t *
fn_context(const value_t *this_fn, parser_state_t *state)
{   const value_t *closure = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(closure, type_closure))
    {   val = value_closure_env(closure);
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_argname(const value_t *this_fn, parser_state_t *state)
{   const value_t *closure = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(closure, type_closure))
    {
        const value_t *code = NULL;
        dir_t *env = NULL;
        const value_t *unbound = NULL;

        if (value_closure_get(closure, &code, &env, &unbound) &&
            unbound != NULL)
            /* WARNING - the values returned in this vector have an _in use_
                         link field.  It could be overwritten if the value
                         were used in a context that wanted to use the link
                         This needs fixing really.
            */
            val = unbound;
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_argnames(const value_t *this_fn, parser_state_t *state)
{   const value_t *closure = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(closure, type_closure))
    {
        const value_t *code = NULL;
        dir_t *env = NULL;
        const value_t *unbound = NULL;
        dir_t *namevec = dir_vec_new();

        if (value_closure_get(closure, &code, &env, &unbound))
        {
            int n = 0;
            const value_t *nameval = unbound;
            /* WARNING - the values returned in this vector have an _in use_
                         link field.  It could be overwritten if the value
                         were used in a context that wanted to use the link
                         This needs fixing really.
            */
            while (NULL != nameval) {
                dir_int_set(namevec, n++, nameval);
                nameval = nameval->link;
            }
            val = dir_value(namevec);
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}




static void
cmds_generic_closure(parser_state_t *state, dir_t *cmds)
{   mod_addfn(cmds, "closure",
              "<bool> <dir> <code> - create closure from code and dir (inherit call context if <bool>)",
              &fn_closure, 3);
    mod_addfn(cmds, "bind",
              "<closure> <arg> - bind argument to unbound closure argument",
              &fn_bind, 2);
    mod_addfn(cmds, "code",
              "<closure> - return code component of a closure",
              &fn_code, 1);
    mod_addfn(cmds, "context",
              "<closure> - return environment component of a closure",
              &fn_context, 1);
    mod_addfn(cmds, "argname",
              "<closure> - return name of 1st argument to be bound or NULL",
              &fn_argname, 1);
    mod_addfn(cmds, "argnames",
              "<closure> - return vector of arguments to be bound",
              &fn_argnames, 1);

}





/*****************************************************************************
 *                                                                           *
 *          Commands - Coroutines                                            *
 *          =====================                                            *
 *                                                                           *
 *****************************************************************************/





#if 0

static const value_t *
fn_co(const value_t *this_fn, parser_state_t *state)
{   const value_t *coclosure = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    if (value_istype(coclosure, type_closure))
    {   value_coroutine_t *newco;

        /* return to the calling envrionment */
        parser_env_return(state, parser_env_calling_pos(state));
        newco = (value_coroutine_t *)
                value_coroutine_new(parser_root(state),
                                    parser_env_stack(state));
        val = value_coroutine_value(newco);
    }
    return val;
}








static void
cmds_generic_coroutine(parser_state_t *state, dir_t *cmds)
{   mod_addfn(cmds, "co",
              "<binding> - create coroutine from binding",
              &fn_co, 1);
}

#else


#define cmds_generic_coroutine(state, cmds)


#endif






/*****************************************************************************
 *                                                                           *
 *          Commands - Parser                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/






static const value_t *
fn_readline(const value_t *this_fn, parser_state_t *state)
{
    if (linesource_eof(parser_linesource(state)))
        return &value_null;
    else
    {   const value_t *val;
        const char *phrase;
        size_t len;
        charsink_string_t line;
        charsink_t *linesink = charsink_string_init(&line);
        charsink_string_t expandline;
        charsink_t *expandsink = charsink_string_init(&expandline);
        linesource_t *linesource = parser_linesource(state);
        code_place_t start;

        code_place_set(&start, instack_source(linesource->in),
                       instack_lineno(linesource->in));

        /* read a line of source into linesink */
        linesource_start_set(linesource, &start);
        linesource_read(linesource, linesink);

        /* expand linesink into expandsink */
        charsink_string_buf(linesink, &phrase, &len);
        parser_expand(state, expandsink, phrase, len);

        /* create string from expansion */
        charsink_string_buf(expandsink, &phrase, &len);
        val = value_string_new(phrase, len);

        charsink_string_close(linesink);
        return val;
    }
}





static const value_t *
fn_root(const value_t *this_fn, parser_state_t *state)
{   /* return to the calling envrionment */
    parser_env_return(state, parser_env_calling_pos(state));
    return dir_value(parser_root(state));
}








static const value_t *
fn_env(const value_t *this_fn, parser_state_t *state)
{   /* return to the calling envrionment */
    parser_env_return(state, parser_env_calling_pos(state));
    /* copy environment state and return it */
    return dir_value(parser_env_copy(state));
}







static const value_t *
fn_stack(const value_t *this_fn, parser_state_t *state)
{   dir_t *vec = dir_vec_new();
    dir_stack_t *stack;
    dir_stack_pos_t pos;
    int n=0;
    /* return to the calling envrionment */
    parser_env_return(state, parser_env_calling_pos(state));

    stack = parser_env_stack(state);
    pos = dir_stack_top_pos(stack);
    while (n < 100 && pos != NULL)
    {   dir_t *subdir = dir_at_stack_pos(pos);
        if (NULL != subdir)
            dir_int_set(vec, n++, dir_value(subdir));
        pos = dir_stack_pos_enclosing(stack, subdir);
    }
    return dir_value(vec);
}







static const value_t *
fn_local(const value_t *this_fn, parser_state_t *state)
{   /* return to the calling envrionment */
    parser_env_return(state, parser_env_calling_pos(state));
    /* top environment of environment stack and return it */
    return dir_value(dir_stack_top(parser_env_stack(state)));
}







static const value_t *
fn_codeid(const value_t *this_fn, parser_state_t *state)
{   const char *id = codeid();
    return value_string_new_measured(id);
}





static const value_t *
fn_source(const value_t *this_fn, parser_state_t *state)
{   const char *src = linesource_source(&state->source);
    return value_string_new_measured(src);
}






static const value_t *
fn_line(const value_t *this_fn, parser_state_t *state)
{   return value_int_new(linesource_lineno(&state->source));
}






static const value_t *
fn_errors(const value_t *this_fn, parser_state_t *state)
{   return value_int_new(parser_error_count(state));
}






static const value_t *
fn_newerror(const value_t *this_fn, parser_state_t *state)
{   parser_error_new(state);
    return &value_null;
}





static const value_t *
fn_errors_reset(const value_t *this_fn, parser_state_t *state)
{   const value_t *errcountval = parser_builtin_arg(state, 1);
    if (value_istype(errcountval, type_int))
    {   parser_errors_reset(state, (int)value_int_number(errcountval));
    } else
        parser_report_help(state, this_fn);
    return &value_null;
}






/* This function may cause a garbage collection */
static const value_t *
genfn_rdexec(const value_t *this_fn, parser_state_t *state,
             const value_t *init, charsource_t *source, bool return_env)
{
    bool noinit = value_type_equal(init, type_null);
    bool is_string = value_type_equal(init, type_string);
    bool is_code = value_type_equal(init, type_code);
    const value_t *val = &value_null;
    bool ok = FALSE;

    if (noinit || is_string || is_code)
    {   const char *initcmds = NULL;
        size_t initcmdslen = 0;

        /* we must take the charsource out of the stream value because
           it will be closed by parser_expand_exec */
        if (noinit ||
            (is_string && value_string_get(init, &initcmds, &initcmdslen)) ||
            (is_code && value_code_buf(init, &initcmds, &initcmdslen))
            )
        {   dir_stack_pos_t pos;
            const value_t *res;
            dir_t *new_dir = dir_id_new();

            OMIT(DIR_SHOW("Orig env: ", parser_env(state));)
            /* return to the calling envrionment - two steps (locals, args?) */
            parser_env_return(state, parser_env_calling_pos(state));
            parser_env_return(state, parser_env_calling_pos(state));
            /* TODO: protect variables in the current scope against garbage
                     collection */

            OMIT(DIR_SHOW("Invoke env: ", parser_env(state));)
            pos = parser_env_push(state, new_dir, /*outer_visible*/FALSE);
            OMIT(DIR_SHOW("Dir after push: ", parser_env(state));)
            res = parser_expand_exec(state, source, initcmds,
                                     /*rcfile_id*/NULL, /*no_locals*/FALSE);
            OMIT(DIR_SHOW("Final env: ", parser_env(state));)
            parser_env_return(state, pos); /* return to calling environment */
            ok = TRUE;
            if (return_env)
                val = dir_value(new_dir);
            else if (NULL != res)
                val = res;
        }
    } else
        parser_error(state, "initial command must be NULL, "
                     "a string or a code object\n");

    if (!ok)
        parser_report_help(state, this_fn);

    return val;
}







/* This function may cause a garbage collection */
static const value_t *
genfn_exec(const value_t *this_fn, parser_state_t *state, bool return_env)
{   const value_t *init = parser_builtin_arg(state, 1);
    value_t *stream = (value_t *)parser_builtin_arg(state, 2);
    bool nostream = value_type_equal(stream, type_null);
    charsource_t *source = NULL;
    const value_t *val = &value_null;

    /* we must take the charsource out of the stream value because
       it will be closed by parser_expand_exec */
    if ((nostream  || value_stream_takesource(stream, &source)))
        val = genfn_rdexec(this_fn, state, init, source, return_env);
    else
        parser_report_help(state, this_fn);

    return val;
}






/* This function may cause a garbage collection */
static const value_t *
fn_exec(const value_t *this_fn, parser_state_t *state)
{   return genfn_exec(this_fn, state, /*return_env*/FALSE);
}






/* This function may cause a garbage collection */
static const value_t *
fn_rdenv(const value_t *this_fn, parser_state_t *state)
{   return genfn_exec(this_fn, state, /*return_env*/TRUE);
}





/* This function may cause a garbage collection */
static const value_t *
fn_rdmod(const value_t *this_fn, parser_state_t *state)
{   value_t *varfilename = (value_t *)parser_builtin_arg(state, 1);
    const char *filename = NULL;
    size_t filenamelen = 0;
    const value_t *val = &value_null;

    if (value_string_get(varfilename, &filename, &filenamelen))
    {   char pathname[ENV_PATHNAME_MAX];
        const char *path;
        charsource_t *source;

        sprintf(&pathname[0], ENV_FTL_PATH(codeid_uc()));
        path = getenv(&pathname[0]);
        source = charsource_file_path_new(path, filename, filenamelen);

        if (NULL != source)
            val = genfn_rdexec(this_fn, state, /*init*/&value_null,
                               source, /*return_env*/TRUE);
        else
        {   parser_report(state,
                          "can't find module \"%s\" on %spath %s\n",
                          filename, NULL==path?"unset ":"", &pathname[0]);
        }
    }
    return val;
}






static const value_t *
fn_scan(const value_t *this_fn, parser_state_t *state)
{   const value_t *str = parser_builtin_arg(state, 1);
    dir_t *vec = dir_vec_new();
    const char *strbase;
    size_t len;
    const value_t *dstr = value_string_get_terminated(str, &strbase, &len);
    const value_t *val = &value_null;

    if (NULL != vec && NULL != dstr)
    {   dir_set(vec, value_zero, dstr);
        val = (const value_t *)vec;
    }

    return val;
}





static const value_t *
fn_scanned(const value_t *this_fn, parser_state_t *state)
{   const value_t *strvec = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(strvec, type_dir))
    {   dir_t *vec = (dir_t *)strvec;
        val = dir_get(vec, value_zero);
    }

    return val;
}





/* This function may cause a garbage collection */
static const value_t *
genfn_scan_noret(const value_t *this_fn, parser_state_t *state,
                 int argstart, bool (*fnparse)(const char **ref_line))
{   const value_t *strvec = parser_builtin_arg(state, argstart);
    const value_t *val = &value_null;

    if (value_istype(strvec, type_dir))
    {   dir_t *vec = (dir_t *)strvec;
        const value_t *str = dir_get(vec, value_zero);
        const char *strbase;
        size_t len;
        const value_t *dstr = value_string_get_terminated(str, &strbase, &len);

        if (NULL != dstr)
        {   const char *line = strbase;
            bool ok = (*fnparse)(&line);
            if (ok)
            {   val = value_true;
                /* update string argument vector */
                OMIT(
                    printf("%s: new %p[%d] -> %p[%d]\n",
                           codeid(), strbase, len, line,
                           len - (line-strbase)););
                dir_set(vec, value_zero,
                        value_string_new(line, len - (line-strbase)));
                /* old value will be garbage collected */
            } else
                val = value_false;

            /* dstr will be garbage collected if it is not str */
        } else
            parser_error(state, "vector must have a string (not a %s) "
                         "named 0\n",
                         value_type_name(str));
    }
    return val;
}





static const value_t *
fn_scan_empty(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan_noret(this_fn, state, 1, &parse_empty);
}


static const value_t *
fn_scan_white(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan_noret(this_fn, state, 1, &parse_white);
}


static const value_t *
fn_scan_space(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan_noret(this_fn, state, 1, &parse_space);
}










/* This function may cause a garbage collection */
static const value_t *
genfn_scan(const value_t *this_fn, parser_state_t *state,
           int argstart,
           const value_t *(*fnparse)(const char **ref_line,
                                     parser_state_t *state,
                                     const value_t *arg),
           const value_t *arg)
{   const value_t *setres = parser_builtin_arg(state, argstart+0);
    const value_t *strvec = parser_builtin_arg(state, argstart+1);
    const value_t *val = &value_null;

    if (value_istype(strvec, type_dir) &&
        (setres == &value_null || value_istype(setres, type_closure)))
    {   dir_t *vec = (dir_t *)strvec;
        const value_t *str = dir_get(vec, value_zero);
        const char *strbase;
        size_t len;
        const value_t *dstr = value_string_get_terminated(str, &strbase, &len);

        if (NULL != dstr)
        {   const char *line = strbase;
            const value_t *newval = (*fnparse)(&line, state, arg);
            if (&value_null != newval)
            {   val = value_true;

                dir_set(vec, value_zero,
                        value_string_new(line, len - (line-strbase)));
                /* old value will be garbage collected */

                if (setres != &value_null) {
                    setres = substitute(setres,
                                        newval == NULL? &value_null: newval,
                                        state, /*unstrict*/TRUE);
                    if (NULL != setres)
                        (void)invoke(setres, state);
                }
            } else
                val = value_false;

            /* dstr will be garbage collected if it is not str */
        } else
            parser_error(state, "parse object must have a string (not a %s) "
                         "named 0\n",
                         value_type_name(str));
    }
    return val;
}





#if 0
static const value_t *
fnparse_key(const char **ref_line, parser_state_t *state, const value_t *arg)
{   const char *key;
    size_t keylen;
    if (value_type_equal(arg, type_string) &&
        value_string_get(arg, &key, &keylen) &&
        parse_key(ref_line, key))
        return value_true;
    else
        return &value_null;
}




static const value_t *
fn_scan_key(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_key, NULL);
}
#endif




static const value_t *
fnparse_match(const char **ref_line, parser_state_t *state, const value_t *arg)
{   dir_t *prefixes = (dir_t *)arg;
    const value_t *choice = NULL;
    if (parse_oneof(ref_line, state, prefixes, &choice)) {
        return choice;
    } else
        return &value_null;
}




static const value_t *
fn_scan_match(const value_t *this_fn, parser_state_t *state)
{   const value_t *prefixes = parser_builtin_arg(state, 1);
    if (value_istype(prefixes, type_dir))
    {   dir_t *prefix_dir = (dir_t *)prefixes;
        return genfn_scan(this_fn, state, 2, &fnparse_match,
                          (void *)prefix_dir);
    } else
        return &value_null;
}




static const value_t *
fnparse_ending(const char **ref_line, parser_state_t *state, const value_t *arg)
{   dir_t *delimiters = (dir_t *)arg;
    const value_t *choice = NULL;
    if (parse_one_ending(ref_line, state, delimiters, &choice)) {
        return choice;
    } else
        return &value_null;
}




static const value_t *
fn_scan_ending(const value_t *this_fn, parser_state_t *state)
{   const value_t *delimiters = parser_builtin_arg(state, 1);
    if (value_istype(delimiters, type_dir))
    {   dir_t *prefix_dir = (dir_t *)delimiters;
        return genfn_scan(this_fn, state, 2, &fnparse_ending,
                          (void *)prefix_dir);
    } else
        return &value_null;
}




#if 0
static const value_t *
fn_scan_ops(const value_t *this_fn, parser_state_t *state)
{   const value_t *prefixes = parser_builtin_arg(state, 1);
    if (value_istype(prefixes, type_dir))
    {   dir_t *prefix_dir = (dir_t *)prefixes;
        return genfn_scan(this_fn, state, 3, &fnparse_ops,
                          (void *)prefix_dir);
    } else
        return &value_null;
}
#endif




static const value_t *
fnparse_int(const char **ref_line, parser_state_t *state, const value_t *arg)
{   number_t n;
    if (parse_int(ref_line, &n))
        return value_int_new(n);
    else
        return &value_null;
}




static const value_t *
fn_scan_int(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_int, NULL);
}





static const value_t *
fnparse_hex(const char **ref_line, parser_state_t *state, const value_t *arg)
{   unumber_t n;
    if (parse_hex(ref_line, &n))
        return value_int_new(n);
    else
        return &value_null;
}




static const value_t *
fn_scan_hex(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_hex, NULL);
}






static const value_t *
fnparse_intval(const char **ref_line, parser_state_t *state, const value_t *arg)
{   number_t n;
    if (parse_int_val(ref_line, &n))
        return value_int_new(n);
    else
        return &value_null;
}




static const value_t *
fn_scan_intval(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_intval, NULL);
}





static const value_t *
fnparse_hexw(const char **ref_line, parser_state_t *state, const value_t *arg)
{   unumber_t n;
    unsigned width = (unsigned)value_int_number(arg);
    if (parse_hex_width(ref_line, width, &n))
        return value_int_new(n);
    else
        return &value_null;
}




static const value_t *
fn_scan_hexw(const value_t *this_fn, parser_state_t *state)
{   const value_t *widthval = parser_builtin_arg(state, 1);
    if (value_istype(widthval, type_int))
        return genfn_scan(this_fn, state, 2, &fnparse_hexw, widthval);
    else
        return &value_null;
}








static const value_t *
fnparse_id(const char **ref_line, parser_state_t *state, const value_t *arg)
{   char buf[128];
    if (parse_id(ref_line, buf, sizeof(buf)))
        return value_string_new_measured(buf);
    else
        return &value_null;
}




static const value_t *
fn_scan_id(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_id, NULL);
}








static const value_t *
fnparse_string(const char **ref_line, parser_state_t *state,
               const value_t *arg)
{   char buf[128];
    size_t slen;
    if (parse_string(ref_line, buf, sizeof(buf), &slen))
        return value_string_new(buf, slen);
    else
        return &value_null;
}




static const value_t *
fn_scan_string(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_string, NULL);
}





static const value_t *
fnparse_code(const char **ref_line, parser_state_t *state,
               const value_t *arg)
{   const value_t *strval;
    const char *srcpos;
    int srcline;

    if (parse_code(ref_line, state, &strval, &srcpos, &srcline)) {
        DEBUG_LNO(printf("%s: new 'code' at %s:%d\n",
                         codeid(), srcpos, srcline);); 
        return value_code_new(strval, srcpos, srcline);
    }
    else
        return &value_null;
}




static const value_t *
fn_scan_code(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_code, NULL);
}





static const value_t *
fnparse_value(const char **ref_line, parser_state_t *state,
              const value_t *arg)
{   const value_t *out_val = NULL;
    bool is_env;

    if (parse_base_env(ref_line, state, &out_val, &is_env))
        return out_val;
    else
        return &value_null;
}





static const value_t *
fn_scan_value(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_value, NULL);
}





static const value_t *
fnparse_itemstr(const char **ref_line, parser_state_t *state,
                const value_t *arg)
{   char buf[128];
    if (parse_itemstr(ref_line, buf, sizeof(buf)))
        return value_string_new_measured(buf);
    else
        return &value_null;
}




static const value_t *
fn_scan_itemstr(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_itemstr, NULL);
}








static const value_t *
fnparse_item(const char **ref_line, parser_state_t *state, const value_t *arg)
{   const char *delimsbase = NULL;
    size_t delimslen = 0;
    char buf[128];
    if ((arg == &value_null ||
        value_string_get(arg, &delimsbase, &delimslen)) &&
        parse_item(ref_line, delimsbase, delimslen, buf, sizeof(buf)))
        return value_string_new_measured(buf);
    else
        return &value_null;
}




static const value_t *
fn_scan_item(const value_t *this_fn, parser_state_t *state)
{   const value_t *delims = parser_builtin_arg(state, 1);
    if (delims == &value_null || value_istype(delims, type_string))
        return genfn_scan(this_fn, state, 2, &fnparse_item, delims);
    else
        return &value_null;
}







static const value_t *
fn_opeval(const value_t *this_fn, parser_state_t *state)
{   /* syntax: file <name> <access> */
    const value_t *opdefvals = parser_builtin_arg(state, 1);
    const value_t *codeval = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(opdefvals, type_dir) &&
        value_istype(codeval, type_code))
    {   dir_t *opdefs = (dir_t *)opdefvals;
        const char *code;
        size_t codelen;

        if (value_code_buf(codeval, &code, &codelen))
        {   const value_t *newval;
            if (parse_opterm(&code, state, &parse_op_base, opdefs, &newval) &&
                parse_space(&code) && parse_empty(&code))

                val = newval;
            else
                parser_error_longstring(state, code,
                                        "code has trailing text -");
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_opset(const value_t *this_fn, parser_state_t *state)
{   /* syntax: file <name> <access> */
    const value_t *opdefvals = parser_builtin_arg(state, 1);
    const value_t *precval = parser_builtin_arg(state, 2);
    const value_t *assocval = parser_builtin_arg(state, 3);
    const value_t *nameval = parser_builtin_arg(state, 4);
    const value_t *fn = parser_builtin_arg(state, 5);
    const value_t *val = &value_null;

    if (value_istype(opdefvals, type_dir) &&
        value_istype(precval, type_int) &&
        value_istype(assocval, type_int) &&
        value_istype(nameval, type_string))
    {   op_assoc_t assoc = (op_assoc_t)value_int_number(assocval);

        if (!assoc_valid(assoc))
            parser_report(state, "invalid associativity value %d\n",
                          assoc);
        else
        {   dir_t *opdefs = (dir_t *)opdefvals;
            op_prec_t prec = (op_prec_t)value_int_number(precval);
            const value_t *precfnsval = dir_get(opdefs, precval);

            if (NULL == precfnsval)
            {   precfnsval = dir_value(dir_id_new());
                if (!dir_int_set(opdefs, prec, precfnsval))
                    parser_report(state, "can't create operator definitions "
                                  "at precidence %d\n",
                                  prec);
            }

            if (value_type_equal(precfnsval, type_dir))
            {   dir_t *precfns = (dir_t *)precfnsval;
                dir_t *opdefn = dir_id_new();
                dir_cstring_set(opdefn, OP_FN, fn);
                dir_cstring_set(opdefn, OP_ASSOC, assocval);
                dir_set(precfns, nameval, dir_value(opdefn));
            } else
               parser_report(state, "invalid operator definitions at "
                            "precidence %d\n",
                            prec);
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}






static void
cmds_generic_parser(parser_state_t *state, dir_t *cmds,
                    int argc, const char **argv)
{   dir_t *pcmds = dir_id_new();
    dir_t *argvec = dir_argvec_new(argc, argv);

    mod_add_dir(cmds, FTLDIR_PARSE, pcmds);
    mod_addfn(pcmds, "codeid",
              "- name of interpreter",  &fn_codeid, 0);
    mod_addfn(pcmds, "rdmod",
              "<module> - return env made executing module file on path",
              &fn_rdmod, 1);
    mod_addfn(pcmds, "exec",
              "<cmds> <stream> - return value of executing "
              "initial <cmds> then stream",
              &fn_exec, 2);
    mod_addfn(pcmds, "readline",
              "- read and expand a line from the command input",
              &fn_readline, 0);
    mod_addfn(pcmds, "rdenv",
              "<cmds> <stream> - return env made executing "
              "initial <cmds> then stream",
              &fn_rdenv, 2);
    mod_addfn(pcmds, "line",
              "- number of the line in the character source",
              &fn_line, 0);
    mod_addfn(pcmds, "source",
              "- name of the source of chars at start of the last line",
              &fn_source, 0);
    mod_addfn(pcmds, "errors",
              "- total number of errors encountered by parser",
              &fn_errors, 0);
    mod_addfn(pcmds, "errors_reset",
              "<n> - reset total number of errors to <n>",
              &fn_errors_reset, 1);
    mod_addfn(pcmds, "newerror",
              "- register the occurrance of a new error",
              &fn_newerror, 0);
    mod_addfn(pcmds, "root",
              "- return current root environment",
              &fn_root, 0);
    mod_addfn(pcmds, "env",
              "- return current invocation environment",
              &fn_env, 0);
    mod_addfn(pcmds, "local",
              "- return local current invocation directory",
              &fn_local, 0);
    mod_addfn(pcmds, "stack",
              "- return local current invocation directory stack",
              &fn_stack, 0);

    mod_addfn(pcmds, "scan",
              "<string> - return <parseobj> for string",
              &fn_scan, 1);
    mod_addfn(pcmds, "scanned",
              "<parseobj> - return text remaining in <parseobj>",
              &fn_scanned, 1);
    mod_addfn(pcmds, "scanempty",
              "<parseobj> - TRUE if no characters remain in <parseobj>",
              &fn_scan_empty, 1);
    mod_addfn(pcmds, "scanwhite",
              "<parseobj> - parse some white space in <parseobj>",
              &fn_scan_white, 1);
    mod_addfn(pcmds, "scanspace",
              "<parseobj> - parse over optional white space in <parseobj>",
              &fn_scan_space, 1);
    mod_addfn(pcmds, "scanint",
              "<@int> <parseobj> - parse integer in <parseobj>",
              &fn_scan_int, 2);
    mod_addfn(pcmds, "scanintval",
              "<@int> <parseobj> - parse signed based integer in <parseobj>",
              &fn_scan_intval, 2);
    mod_addfn(pcmds, "scanhex",
              "<@int> <parseobj> - parse hex in <parseobj>",
              &fn_scan_hex, 2);
    mod_addfn(pcmds, "scanhexw",
              "<width> <@int> <parseobj> - parse hex in <width> chars in "
              "<parseobj>, update string",
              &fn_scan_hexw, 3);
    mod_addfn(pcmds, "scanstr",
              "<@string> <parseobj> - parse single or double-quoted string "
              "in <parseobj>",
              &fn_scan_string, 2);
    mod_addfn(pcmds, "scanid",
              "<@string> <parseobj> - parse identifier in <parseobj>",
              &fn_scan_id, 2);
    mod_addfn(pcmds, "scanitemstr",
              "<@string> <parseobj> - parse item or string in <parseobj>",
              &fn_scan_itemstr, 2);
    mod_addfn(pcmds, "scancode",
              "<@string> <parseobj> - parse {code} block in <parseobj>",
              &fn_scan_code, 2);
    mod_addfn(pcmds, "scanvalue",
              "<@string> <parseobj> - parse a basic value in <parseobj>",
              &fn_scan_value, 2);
    mod_addfn(pcmds, "scanitem",
              "<delims> <@string> <parseobj> - parse item until non-blank or "
              "delimiter in <parseobj>",
              &fn_scan_item, 3);
    mod_addfn(pcmds, "scanmatch",
              "<dir> <@val> <parseobj> - parse prefix in dir in "
              "<parseobj> giving matching value",
              &fn_scan_match, 3);
    mod_addfn(pcmds, "scantomatch",
              "<dir> <@val> <parseobj> - parse up to delimiter named in dir in "
              "<parseobj> giving matching value",
              &fn_scan_ending, 3);

    mod_addfn(pcmds, "opset",
              "<opdefs> <prec> <assoc> <name> <function> - define an operator "
              "in opdefs",
              &fn_opset, 5);
    mod_addfn(pcmds, "opeval",
              "<opdefs> <code> - execute code according to operator "
              "definitions",
              &fn_opeval, 2);
    mod_add_dir(pcmds, "op", parser_opdefs(state));
    mod_add_dir(pcmds, "assoc", dir_opassoc());
    mod_add_dir(pcmds, "argv", argvec);
    mod_add_val(pcmds, FN_UNDEF_HOOK, &value_null);
}








/*****************************************************************************
 *                                                                           *
 *          Commands - System                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/







static const value_t *
cmd_system(const char **ref_line, const value_t *this_cmd,
           parser_state_t *state)
{   int rc;
    const char *str = *ref_line;
    const value_t *res = &value_null;

    OMIT(printf("system <%s>\n", str);)
    rc = system(str);
    if (rc != 0)
        res = parser_errorval(state, "system command failed - '%s' (rc %d)\n",
                              str, rc);
    *ref_line += strlen(*ref_line);

    return res;
}






static const value_t *
fn_system_rc(const value_t *this_fn, parser_state_t *state)
{   /* syntax: file <name> <access> */
    const char *str;
    size_t len;
    const value_t *strval =
          value_string_get_terminated(parser_builtin_arg(state, 1), &str, &len)

    OMIT(printf("system <%s>\n", str););

    if (strval == NULL) {
        parser_report_help(state, this_fn);
        return &value_null;
    } else
        return value_int_new(system(str));
}





static const value_t *
cmd_uid(const char **ref_line, const value_t *this_cmd,
        parser_state_t *state)
{   bool ok;
    const char *str = *ref_line;
    unsigned uid = 0;

    OMIT(printf("system <%s>\n", str);)
    ok = user_uid(str, &uid);
    *ref_line += strlen(*ref_line);

    return ok? value_int_new(uid): &value_null;
}




typedef struct {
    const value_t *dirnameval;
    dir_t *content;
} fs_ls_args_t;




static bool
fn_fs_ls_enum(void *enum_arg, const char *entry, const char *subdir,
              bool examinable, bool is_dir)
{   fs_ls_args_t *args = (fs_ls_args_t *)enum_arg;

    if (examinable) {
       dir_t *valdir = dir_id_new();
       (void)dir_cstring_set(valdir, "dir", is_dir? value_true: value_false);
       (void)dir_string_set(args->content, entry, dir_value(valdir));
    } else
       (void)dir_string_set(args->content, entry, &value_null);

    return TRUE; /* continue */
}



static const value_t *
fn_fs_ls(const value_t *this_fn, parser_state_t *state)
{   const value_t *result = NULL;

    const value_t *dirnameval = parser_builtin_arg(state, 1);

    if (value_istype(dirnameval, type_string))
    {   const char *dirname = NULL;
        size_t dirnamelen;
        const value_t *d = value_string_get_terminated(dirnameval, &dirname,
                                                       &dirnamelen);
        fs_ls_args_t arg;

        arg.dirnameval = d;
        arg.content = dir_id_new();

        if (arg.content != NULL &&
            fs_enum_dir(dirname, &fn_fs_ls_enum, &arg))
            result = dir_value(arg.content);
        else
            result = &value_null;
    } else
        parser_report_help(state, this_fn);

    return result;
}






static const value_t *
fn_fs_absname(const value_t *this_fn, parser_state_t *state)
{
    const value_t *filename = parser_builtin_arg(state, 1);
    bool is_abs = FALSE;
    bool ok = FALSE;

    if (value_istype(filename, type_string))
    {   const char *name;
        size_t namelen;

        if (value_string_get(filename, &name, &namelen)) {
            ok = TRUE;
            is_abs = OS_FS_ABSPATH(name);
        }
    }

    if (!ok)
        parser_report_help(state, this_fn);

    return is_abs? value_true: value_false;
}



static const value_t *
fn_path(const value_t *this_fn, parser_state_t *state)
{   /* syntax: file <name> <access> */
    const value_t *pathval = parser_builtin_arg(state, 1);
    const value_t *filename = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;
    bool nopath = (pathval == &value_null);
    bool ok = TRUE;

    if ((nopath || value_istype(pathval, type_string)) &&
        value_istype(filename, type_string))
    {   const char *name;
        size_t namelen;
        const char *path = NULL;
        size_t pathlen;

        if (!nopath)
            ok = value_string_get(pathval, &path, &pathlen);

        if (ok && value_string_get(filename, &name, &namelen))
        {   char fullname[PATHNAME_MAX];
            FILE *str = fopen_onpath(path, name, namelen, "r",
                                     &fullname[0], sizeof(fullname));
            /* expand file name into fullname[] */
            if (NULL != str)
            {   fclose(str);
                val = value_string_new_measured(fullname);
            }
        }
    }

    if (!ok)
        parser_report_help(state, this_fn);

    return val;
}







static const value_t *
fn_time(const value_t *this_cmd, parser_state_t *state)
{   time_t now;
    time(&now);
    return value_int_new(now);
}





static const value_t *
fn_time_gen(const value_t *this_cmd, parser_state_t *state,
            struct tm *(*fn)(const time_t *))
{   const value_t *timeval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(timeval, type_int))
    {   time_t dat = (time_t)value_int_number(timeval);
        struct tm *tinfo = (*fn)(&dat);
        dir_t *tm = dir_id_new();
        dir_cstring_set(tm, "sec",   value_int_new(tinfo->tm_sec));
        dir_cstring_set(tm, "min",   value_int_new(tinfo->tm_min));
        dir_cstring_set(tm, "hour",  value_int_new(tinfo->tm_hour));
        dir_cstring_set(tm, "mday",  value_int_new(tinfo->tm_mday));
        dir_cstring_set(tm, "mon",   value_int_new(tinfo->tm_mon));
        dir_cstring_set(tm, "year",  value_int_new(SYS_EPOCH_YEAR +
                                                   tinfo->tm_year));
        dir_cstring_set(tm, "wday",  value_int_new(tinfo->tm_wday));
        dir_cstring_set(tm, "yday",  value_int_new(tinfo->tm_yday));
        dir_cstring_set(tm, "isdst", value_int_new(tinfo->tm_isdst));
        val = dir_value(tm);
    }

    return val;
}



static const value_t *
fn_localtime(const value_t *this_fn, parser_state_t *state)
{   return fn_time_gen(this_fn, state, &localtime);
}


static const value_t *
fn_gmtime(const value_t *this_fn, parser_state_t *state)
{   return fn_time_gen(this_fn, state, &gmtime);
}




static const value_t *
fn_timef_gen(const value_t *this_cmd, parser_state_t *state,
             struct tm *(*fn)(const time_t *))
{   const value_t *fmtval = parser_builtin_arg(state, 1);
    const value_t *timeval = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(fmtval, type_string) && value_istype(timeval, type_int))
    {   const char *fmtbuf;
        size_t fmtlen;
        if (fn == NULL)
            parser_error(state, "internal error - time function NULL\n");
        else if (value_string_get(fmtval, &fmtbuf, &fmtlen))
        {   time_t dat = (time_t)value_int_number(timeval);
            struct tm *tinfo = (*fn)(&dat);
            if (tinfo == NULL) {
               parser_error(state, "internal error - time function "
                            "returns NULL\n");
               assert(tinfo != NULL);
            } else if (fmtbuf[0] == '\0') {
               val = value_cstring_new("", 0);
            } else {
               char fmtstr[256];
               size_t len;
               /* Warning: Win 7/8/10 32/64-bit strftime "%s" causes abort!
                *          as does %h, %T and %D (at least)
                *          although %Y %y %m %d %H %M %S seem OK
                */
               OMIT(printf("tm %p format '%s' buflen %d\n",
                              tinfo, fmtbuf, sizeof(fmtstr));
                      printf("tm year %d hour %d\n",
                             1900+tinfo->tm_year, tinfo->tm_hour););
               len = strftime(&fmtstr[0], sizeof(fmtstr), fmtbuf, tinfo);
               OMIT(printf("len %d\n", len););
               if ((int)len >= 0 && len <= sizeof(fmtstr))
                   val = value_string_new(&fmtstr[0], len);
            }
        }
    } else
        parser_report_help(state, this_cmd);

    return val;
}



static const value_t *
fn_localtimef(const value_t *this_fn, parser_state_t *state)
{   return fn_timef_gen(this_fn, state, &localtime);
}


static const value_t *
fn_gmtimef(const value_t *this_fn, parser_state_t *state)
{   return fn_timef_gen(this_fn, state, &gmtime);
}




#if 0
static const value_t *
cmd_getenv(const char **ref_line, const value_t *this_cmd,
           parser_state_t *state)
{   const char *envval;
    const char *str = *ref_line;
    const value_t *val = &value_null;

    OMIT(printf("getenv <%s>\n", str);)
    envval = getenv(str);
    if (envval == NULL)
        parser_error(state, "environment variable not set - '%s'\n", str);
    else
        val = value_string_new_measured(envval);

    *ref_line += strlen(*ref_line);

    return val;
}
#endif




static const value_t *
fn_lib_load(const value_t *this_fn, parser_state_t *state)
{
    const value_t *filenameval = parser_builtin_arg(state, 1);
    const value_t *symsval = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;
    const char *filename;
    size_t filenamelen;

    if (value_string_get_terminated(filenameval, &filename, &filenamelen) &&
        value_istype(symsval, type_dir))
    {   dir_t *dir = dir_lib_new(filenameval, (dir_t *)symsval, /*fix*/FALSE);
        if (dir != NULL)
            val = dir_value(dir);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_lib_extend(const value_t *this_fn, parser_state_t *state)
{
    const value_t *filenameval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    const char *filename;
    size_t filenamelen;

    if (value_string_get_terminated(filenameval, &filename, &filenamelen))
    {   dir_t *syms = dir_vec_new();
        dir_t *dir;
        const char *main_fn_name = STRING(FTL_EXTENSION_MAIN);
        const value_t *main_fnval = value_string_new_measured(main_fn_name);

        dir_int_set(syms, 0, main_fnval);
        dir = dir_lib_new(filenameval, syms, /*fix*/TRUE);
        if (dir != NULL) {
            const value_t *mainval = dir_get(dir, main_fnval);
            if (mainval == NULL)
                parser_error(state, "library symbol '%s' absent\n",
                             main_fn_name);
            else if (value_type_equal(mainval, type_null))
                parser_error(state, "library symbol '%s' undefined\n",
                             main_fn_name);
            else if (!value_type_equal(mainval, type_int))
                parser_error(state, "library symbol '%s' does not have "
                             "integer value\n",
                             main_fn_name);
            else
            {   number_t main_addr = value_int_number(mainval);
                if (main_addr == 0)
                    parser_error(state, "library symbol '%s' is NULL\n",
                             main_fn_name);
                else {
                    ftl_extension_fn *ftl_main = (ftl_extension_fn *)
                                                 (ptrdiff_t)main_addr;
                    dir_t *mod = dir_id_new();

                    bool ok = (*ftl_main)(state, mod);

                    if (ok)
                        val = dir_value(mod);
                }
            }
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}





static void
cmds_generic_sys(parser_state_t *state, dir_t *cmds)
{   dir_t *scmds = dir_id_new();
    dir_t *sysenv = dir_sysenv_new();
    dir_t *fscmds = dir_id_new();
    dir_t *libcmds = dir_id_new();
    dir_t *shcmds = dir_id_new();
    const char *osfamily = "unknown";
    static char sep[2];
    static char exec_buf[PATHNAME_MAX];
    const char *executable = file_executable(&exec_buf[0], sizeof(exec_buf));

#ifdef _WIN32
    osfamily = "windows";
#endif
#ifdef __linux__
    osfamily = "linux";
#endif
#ifdef __sun__
    osfamily = "sunos";
#endif
#ifdef __APPLE__
    osfamily = "macosx";
#endif

    sep[0] = OS_FS_SEP;
    sep[1] = '\0';
    mod_add_val(fscmds, "sep", value_string_new_measured(sep));
    mod_add_val(fscmds, "nowhere", value_string_new_measured(OS_FS_NOWHERE));
    mod_add_val(fscmds, "thisdir", value_string_new_measured(OS_FS_DIR_HERE));
    mod_add_val(fscmds, "casematch", OS_FS_CASE_EQ? value_true: value_false);
    mod_addfn(fscmds, "absname",
              "<file> - TRUE iff file has an absolute path name",
              &fn_fs_absname, 1);
    mod_addfn(fscmds, "ls",
              "<dir> - directory content as name=[dir=<bool>] pairs",
              &fn_fs_ls, 1);
    (void)dir_lock(fscmds, NULL); /* prevents additions to this directory */

    mod_addfn(libcmds, "load",
              "<lib> <sym_vec> - load dynamic lib giving directory of symbol values",
              &fn_lib_load, 2);
    mod_addfn(libcmds, "extend",
              "<ftlext> - load FTL extension returning directory of functions",
              &fn_lib_extend, 1);
    (void)dir_lock(libcmds, NULL); /* prevents additions to this directory */

    sep[0] = OS_PATH_SEP;
    sep[1] = '\0';
    mod_add_val(shcmds, "self", executable == NULL? &value_null:
                                value_string_new_measured(executable));
    mod_add_val(shcmds, "pathsep", value_string_new_measured(sep));
    mod_addfn(shcmds, "path", "<path> <file> - return name of file on path",
              &fn_path, 2);
    (void)dir_lock(shcmds, NULL); /* prevents additions to this directory */

    mod_add_dir(cmds, "sys", scmds);
    mod_add_dir(scmds, "fs", fscmds);
    mod_add_dir(scmds, "shell", shcmds);
    mod_add_dir(scmds, "lib", libcmds);
    mod_add_val(scmds, "osfamily",
                value_string_new_measured(osfamily));
    mod_add(scmds, "run", "<line> - execute system <line>",
            &cmd_system);
    mod_addfn(scmds, "runrc", "<command> - execute system command & return result code",
              &fn_system_rc, 1);
    mod_add(scmds, "uid", "<user> - return the UID of the named user",
            &cmd_uid);
    mod_addfn(scmds, "time", "- system calendar time in seconds",
              &fn_time, 0);
    mod_addfn(scmds, "localtime", "<time> - broken down local time",
              &fn_localtime, 1);
    mod_addfn(scmds, "utctime", "<time> - broken down UTC time",
              &fn_gmtime, 1);
    mod_addfn(scmds, "localtimef", "<format> <time> - formatted local time",
              &fn_localtimef, 2);
    mod_addfn(scmds, "utctimef", "<format> <time> - formatted UTC time",
              &fn_gmtimef, 2);
#if 0
    mod_add(scmds, "getenv",
            "<line> - return value of environment variable <line>",
            &cmd_getenv);
#endif
    mod_add_dir(scmds, "env", sysenv);
}













/*****************************************************************************
 *                                                                           *
 *          Commands - Stream                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/







static bool
parse_stream_access(const char **ref_line, bool *out_read, bool *out_write)
{   bool is_read = FALSE;
    bool is_write = FALSE;

    while ((is_read = parse_key(ref_line, "r")) ||
           (is_write = parse_key(ref_line, "w")))
    {   if (is_read)
            *out_read = TRUE;
        if (is_write)
            *out_write = TRUE;
    }

    return parse_empty(ref_line);
}







static const value_t *
genfn_pathfile(const value_t *this_fn, parser_state_t *state, bool binary)
{   /* syntax: file <name> <access> */
    const value_t *pathval = parser_builtin_arg(state, 1);
    const value_t *filename = parser_builtin_arg(state, 2);
    const value_t *access = parser_builtin_arg(state, 3);
    const value_t *val = &value_null;
    bool nopath = (pathval == &value_null);

    if ((nopath || value_istype(pathval, type_string)) &&
        value_istype(filename, type_string) &&
        value_istype(access, type_string))
    {   bool read = FALSE;
        bool write = FALSE;
        const char *name;
        size_t namelen;
        const char *path = NULL;
        size_t pathlen;
        bool ok = TRUE;

        if (!nopath)
            ok = value_string_get(pathval, &path, &pathlen);

        if (ok && value_string_get(access, &name, &namelen))
        {   if (parse_stream_access(&name, &read, &write))
            {   if (value_string_get(filename, &name, &namelen))
                {   char fullname[PATHNAME_MAX];
                    val = value_stream_file_path_new(path, name, namelen,
                                                     binary, read, write,
                                                     fullname,
                                                     sizeof(fullname));
                }
            } else
                parser_report(state, "stream access string must contain "
                              "'r' and 'w' only\n");
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_pathfile(const value_t *this_fn, parser_state_t *state)
{   return genfn_pathfile(this_fn, state, /*binary*/FALSE);
}



static const value_t *
fn_pathbinfile(const value_t *this_fn, parser_state_t *state)
{   return genfn_pathfile(this_fn, state, /*binary*/TRUE);
}






static const value_t *
genfn_file(const value_t *this_fn, parser_state_t *state, bool binary)
{   /* syntax: file <name> <access> */
    const value_t *filename = parser_builtin_arg(state, 1);
    const value_t *access = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(filename, type_string) &&
        value_istype(access, type_string))
    {   bool read = FALSE;
        bool write = FALSE;
        const char *name;
        size_t namelen;
        if (value_string_get(access, &name, &namelen))
        {   if (parse_stream_access(&name, &read, &write))
            {   if (value_string_get(filename, &name, &namelen))
                {   val = value_stream_file_new(name, binary, read, write);
                }
            } else
                parser_report(state, "stream access string must contain "
                              "'r' and 'w' only\n");
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_file(const value_t *this_fn, parser_state_t *state)
{  return genfn_file(this_fn, state, /*binary*/FALSE);
}



static const value_t *
fn_binfile(const value_t *this_fn, parser_state_t *state)
{  return genfn_file(this_fn, state, /*binary*/TRUE);
}






static const value_t *
fn_instring(const value_t *this_fn, parser_state_t *state)
{   /* syntax: instring <string or code> <access> */
    const value_t *string = parser_builtin_arg(state, 1);
    const value_t *access = parser_builtin_arg(state, 2);
    bool is_code = value_type_equal(string, type_code);
    const value_t *val = &value_null;

    if ((is_code || value_istype(string, type_string)) &&
        value_istype(access, type_string))
    {   bool read = FALSE;
        bool write = FALSE;
        const char *acc;
        size_t acclen;

        if (value_string_get(access, &acc, &acclen))
        {   if (parse_stream_access(&acc, &read, &write))
            {   if (!write && read)
                {   const char *strbase;
                    size_t strlen;
                    bool ok;

                    if (is_code)
                        ok = value_code_buf(string, &strbase, &strlen);
                    else
                        ok = value_string_get(string, &strbase, &strlen);

                    if (ok)
                    {   val = value_stream_instring_new(is_code? "<code-in>":
                                                                 "<string-in>",
                                                        strbase, strlen);
                    }
                }
            } else
                parser_report(state, "stream access string must contain "
                              "'r' and 'w' only\n");
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}







static const value_t *
fn_outstring(const value_t *this_fn, parser_state_t *state)
{   const value_t *code = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(code, type_closure))
    {   value_t *str = value_stream_outstring_new();
        if (NULL != str)
        {   /* return to calling environment */
            parser_env_return(state, parser_env_calling_pos(state));/*code*/
            code = substitute(code, str, state, /*unstrict*/FALSE);
            if (NULL != invoke(code, state))
                val=value_stream_outstring_string((value_stream_outstring_t *)
                                                  str);
            value_stream_close(str);
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_getc(const value_t *this_fn, parser_state_t *state)
{   /* syntax: write <name> <string> */
    const value_t *stream = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(stream, type_stream))
    {   charsource_t *source;
        if (!value_stream_source(stream, &source))
            parser_error(state, "stream not open for input\n");
        else
        {   int intch = charsource_getc(source);
            if (intch != EOF)
            {   char ch = (char)intch;
                val = value_string_new(&ch, 1);
            }
            /* else printf("getc -> EOF\n"); */
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_read(const value_t *this_fn, parser_state_t *state)
{   /* syntax: read <name> <string> */
    const value_t *stream = parser_builtin_arg(state, 1);
    const value_t *size = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(stream, type_stream) &&
        value_istype(size, type_int))
    {   charsource_t *source;
        if (!value_stream_source(stream, &source))
            parser_error(state, "stream not open for input\n");
        else
        {   size_t len = (size_t)value_int_number(size);
            char *buf = (char *)FTL_MALLOC(len);
            if (NULL == buf)
            {   parser_error(state, "can't allocate a buffer of size %d\n",
                             len);
            } else
            {   size_t actual = charsource_read(source, (char *)buf, len);
                val = value_string_new(buf, actual);
                FTL_FREE(buf);
            }
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_write(const value_t *this_fn, parser_state_t *state)
{   /* syntax: write <name> <string> */
    const value_t *stream = parser_builtin_arg(state, 1);
    const value_t *obj = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(stream, type_stream) &&
        value_istype(obj, type_string))
    {   charsink_t *sink;
        if (!value_stream_sink(stream, &sink))
            parser_error(state, "stream not open for output\n");
        else
        {   const char *buf;
            size_t len;
            if (value_string_get(obj, &buf, &len))
                val = value_int_new(charsink_write(sink, (char *)buf, len));
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}






#if 0
static const value_t *
fn_writexpand(const value_t *this_fn, parser_state_t *state)
{   /* syntax: writexp <stream> <string> */
    const value_t *stream = parser_builtin_arg(state, 1);
    const value_t *obj = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(stream, type_stream) &&
        value_istype(obj, type_string))
    {   charsink_t *sink;
        if (!value_stream_sink(stream, &sink))
            parser_error(state, "stream not open for output\n");
        else
        {   const char *buf;
            size_t len;
            if (value_string_get(obj, &buf, &len))
                parser_expand(state, sink, (char *)buf, len);
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}
#endif








static const value_t *
fn_fprintf(const value_t *this_fn, parser_state_t *state)
{   /* syntax: printf <stream> <format> <env> */
    const value_t *stream = parser_builtin_arg(state, 1);
    const value_t *obj = parser_builtin_arg(state, 2);
    const value_t *envval = parser_builtin_arg(state, 3);
    const value_t *val = &value_null;
    dir_t *env = NULL;

    if (value_istype(stream, type_stream) &&
        value_istype(obj, type_string) &&
        (value_type_equal(envval, type_null) || value_as_dir(envval, &env)))
    {   charsink_t *sink;
        if (!value_stream_sink(stream, &sink))
            parser_error(state, "stream not open for output\n");
        else
        {   const char *buf;
            size_t len;

            if (value_string_get(obj, &buf, &len))
                val = value_int_new(value_fprintfmt(state, sink,
                                                    (char *)buf, len, env));
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_stringify(const value_t *this_fn, parser_state_t *state)
{   /* syntax: file <name> <access> */
    const value_t *stream = parser_builtin_arg(state, 1);
    const value_t *obj = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(stream, type_stream))
    {   charsink_t *sink;
        if (!value_stream_sink(stream, &sink))
            parser_error(state, "stream not open for output\n");
        else
            val = value_int_new(value_print(sink, dir_value(parser_root(state)),
                                            obj));
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_close(const value_t *this_fn, parser_state_t *state)
{   /* syntax: file <name> <access> */
    const value_t *stream = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(stream, type_stream))
        value_stream_close((value_t *)stream);
    else
        parser_report_help(state, this_fn);

    return val;
}







/* Temporarily until we can do this in FTL */
static const value_t *
cmd_filetostring(const char **ref_line, const value_t *this_cmd,
                 parser_state_t *state)
{   char filename[128];
    int rc = 0;

    if (parse_itemstr(ref_line, &filename[0], sizeof(filename)) &&
        parse_space(ref_line))
    {   FILE *inchars = fopen(filename, "r");
        if (NULL == inchars)
        {   rc = errno;
            parser_error(state,
                         "couldn't open file \"%s\" to read - %s (rc %d)\n",
                         &filename[0], strerror(errno), errno);
        } else
        {   FILE *to = stdout;

            if (parse_itemstr(ref_line, &filename[0], sizeof(filename)) &&
                parse_space(ref_line))
            {   to = fopen(filename, "w");
                if (NULL == to)
                {   rc = errno;
                    parser_error(state,
                                  "couldn't open file \"%s\" to write - %s "
                                  "(rc %d)\n",
                                  &filename[0], strerror(errno), errno);
                }
            }

            if (NULL != to)
            {
                if (parse_empty(ref_line))
                {   char *linebuf = &filename[0]; /* reuse space */
                    charsink_stream_t charbuf;
                    charsink_t *sink = charsink_stream_init(&charbuf, to);

                    while (!feof(inchars))
                    {   char *line = fgets(linebuf, sizeof(filename), inchars);
                        if (NULL != line)
                        {   fprintf(to, "    ");
                            string_print(sink, line, strlen(line));
                            fprintf(to, "\n");
                        }
                    }

                    charsink_stream_close(sink);
                }

                if (to != stdout)
                    fclose(to);
            }
            fclose(inchars);
        }
    } else
        parser_error(state, "file name expected\n");

    return 0 == rc? &value_null: value_int_new(rc);
}








#if 0
static const value_t *
fn_load(const char **ref_line, const value_t *this_cmd,
         parser_state_t *state)
{   char filename[128];
    int rc = 0;

    if (parse_itemstr(ref_line, &filename[0], sizeof(filename)) &&
        parse_space(ref_line))
    {   if (NULL != to)
        {
            if (parse_empty(ref_line))
            {   char *linebuf = &filename[0]; /* reuse space */
                charsink_stream_t charbuf;
                charsink_t *sink = charsink_stream_init(&charbuf, to);

                while (!feof(inchars))
                {   char *line = charsource_read(linebuf,
                                                 sizeof(filename), inchars);
                    if (NULL != line)
                    {   charsink_write(sink, line, strlen(line));
                    }
                }

                charsink_stream_close(sink);
            }

            if (to != stdout)
                fclose(to);
        }
        fclose(inchars);
        }
    } else
        parser_error(state, "file name expected\n");

    return 0 == rc? &value_null: value_int_new(rc);
}
#endif








static const value_t *
fn_sourcetext(const value_t *this_cmd, parser_state_t *state)
{   const value_t *str = parser_builtin_arg(state, 1);
    const value_t *res = &value_null;

    if (value_istype(str, type_string))
    {   const char *buf;
        size_t len;

        if (value_string_get(str, &buf, &len))
        {   charsource_t *inchars = charsource_string_new("sourcetext",
                                                          buf, len);
            if (NULL == inchars)
                res = parser_errorval(state, "couldn't open string to read\n");
            else
                linesource_push(parser_linesource(state), inchars);
        }
    }
    return res;
}






static const value_t *
cmd_source(const char **ref_line, const value_t *this_cmd,
           parser_state_t *state)
{   char filename[PATHNAME_MAX];
    int rc = 0;

    if (parse_itemstr(ref_line, &filename[0], sizeof(filename)) &&
        parse_space(ref_line) &&
        parse_empty(ref_line))
    {   char pathname[ENV_PATHNAME_MAX];
        const char *path;
        charsource_t *inchars;

        sprintf(&pathname[0], ENV_FTL_PATH(codeid_uc()));
        path = getenv(&pathname[0]);
        inchars = charsource_file_path_new(path, filename, strlen(filename));

        if (NULL == inchars)
        {   rc = errno;
            parser_error(state,
                         "couldn't open file \"%s\" to read - %s (rc %d)\n",
                         &filename[0], strerror(errno), errno);
        } else
            linesource_push(parser_linesource(state), inchars);
    } else
        parser_error(state, "file name expected\n");

    return 0 == rc? &value_null: value_int_new(rc);
}






static const value_t *
fn_return(const value_t *this_cmd, parser_state_t *state)
{   const value_t *rc = parser_builtin_arg(state, 1);
    linesource_t *lines = parser_linesource(state);
    /*printf("%s: ending source %s\n", codeid(), linesource_source(lines));*/
    (void)linesource_popdel(lines);
    return rc;
}






static void
cmds_generic_stream(parser_state_t *state, dir_t *cmds)
{   dir_stack_t *scope = parser_env_stack(state);
    dir_t *icmds = dir_id_new();
    value_t *val_stdout =
        value_stream_openfile_new(stdout, /*autoclose*/FALSE, "stdout",
                                  /*read*/FALSE, /*write*/TRUE);
    value_t *val_stderr =
        value_stream_openfile_new(stderr, /*autoclose*/FALSE, "stderr",
                                  /*read*/FALSE, /*write*/TRUE);
    value_t *val_stdin =
        value_stream_openfile_new(stdin, /*autoclose*/FALSE, "stdin",
                                  /*read*/TRUE, /*write*/FALSE);

    mod_add_dir(cmds, "io", icmds);
    mod_add_val(icmds, "err", val_stderr);
    mod_add_val(icmds, "out", val_stdout);
    mod_add_val(icmds, "in",  val_stdin);
    mod_addfn(icmds, "file",
              "<filename> <rw> - return stream for opened file",
              &fn_file, 2);
    mod_addfn(icmds, "binfile",
              "<filename> <rw> - return stream for opened binary file",
              &fn_binfile, 2);
    mod_addfn(icmds, "pathfile",
              "<path> <filename> <rw> - return stream for opened file on path",
              &fn_pathfile, 3);
    mod_addfn(icmds, "pathbinfile",
              "<path> <filename> <rw> - return stream for opened "
              "binary file on path",
              &fn_pathbinfile, 3);
    mod_addfn(icmds, "instring",
              "<string> <rw> - return stream for reading string",
              &fn_instring, 2);
    mod_addfn(icmds, "outstring",
              "<closure> - apply string output stream to closure, "
              "return string written",
              &fn_outstring, 1);
    mod_addfn(icmds, "getc",
              "<stream> - read the next character from the stream",
              &fn_getc, 1);
    mod_addfn(icmds, "read",
              "<stream> <size> - read up to <size> bytes from stream",
              &fn_read, 2);
    mod_addfn(icmds, "write",
              "<stream> <string> - write string to stream",
              &fn_write, 2);
    mod_addfn(icmds, "fprintf",
              "<stream> <format> <env> - write formatted string to stream",
              &fn_fprintf, 3);
    mod_addfn(icmds, "stringify",
              "<stream> <expr> - write FTL representation to stream",
              &fn_stringify, 2);
    mod_addfn(icmds, "close",
              "<stream> - close stream", &fn_close, 1);
    mod_add(icmds, "filetostring",
                  "<filename> [<outfile>] - write file out as a C string",
                  &cmd_filetostring);
    mod_add(cmds, "source", "<filename> - read from file <filename>",
            &cmd_source);
    mod_addfnscope(cmds, "sourcetext",
                   "<stringexpr> - read characters from string",
                   &fn_sourcetext, 1, scope);
    mod_addfn(cmds, "return",
              "<rc> - abandon current command input returning <rc>",
              &fn_return, 1);
}











/*****************************************************************************
 *                                                                           *
 *          Commands - Nul                                                   *
 *          ==============                                                   *
 *                                                                           *
 *****************************************************************************/






static void
cmds_generic_nul(parser_state_t *state, dir_t *cmds)
{   mod_add_val(cmds, "NULL", &value_null);
}






/*****************************************************************************
 *                                                                           *
 *          Commands - Type                                                  *
 *          ===============                                                  *
 *                                                                           *
 *****************************************************************************/







static const value_t *
fn_typeof(const value_t *this_fn, parser_state_t *state)
{   const value_t *anyval = parser_builtin_arg(state, 1);
    const value_t *res = &value_null;

    if (NULL != anyval && anyval->kind != NULL)
        res = value_type_value(anyval->kind);

    return res;
}






static const value_t *
fn_typename(const value_t *this_fn, parser_state_t *state)
{   const value_t *any = parser_builtin_arg(state, 1);

    if (NULL == any)
        return &value_null;
    else
    {   const char *typename = value_type_name(any);
        return value_string_new_measured(typename);
    }
}






static const value_t *
cmd_type(const char **ref_line, const value_t *this_cmd, parser_state_t *state)
{
    const value_t *result_typeval = NULL;
    dir_t *cmds = parser_root(state);
    const value_t *typesval = cmds == NULL? NULL:
                              dir_string_get(cmds, ROOT_DIR_TYPE);
    dir_t *root_types = NULL;
    const char *typename = *ref_line;

    *ref_line += strlen(*ref_line);

    if (value_as_dir(typesval, &root_types))
    {
        dir_stack_pos_t pos = parser_env_push(state, root_types,
                                              /*outer_visible*/FALSE);
        result_typeval = dir_string_get(root_types, typename);
        if (result_typeval == NULL)
             result_typeval = &value_null;

        parser_env_return(state, pos); /* go back to original stack pos */
    } else
        parser_error(state, "can't locate types directory '%s'\n",
                     ROOT_DIR_TYPE);

    return result_typeval;
}






static const value_t *
fn_cmp(const value_t *this_fn, parser_state_t *state)
{   const value_t *v1 = parser_builtin_arg(state, 1);
    const value_t *v2 = parser_builtin_arg(state, 2);

    return value_int_new(value_cmp(v1, v2));
}






static void
cmds_generic_type(parser_state_t *state, dir_t *cmds)
{
    dir_t *types = dir_id_new();

    mod_add_dir(cmds, ROOT_DIR_TYPE, types);

    /* regiser built-in types in 'type' directory */
    mod_add_val(types, type_name(type_type), value_type_value(type_type));
    mod_add_val(types, type_name(type_null), value_type_value(type_null));
    mod_add_val(types, type_name(type_string), value_type_value(type_string));
    mod_add_val(types, type_name(type_code), value_type_value(type_code));
    mod_add_val(types, type_name(type_closure), value_type_value(type_closure));
    mod_add_val(types, type_name(type_int), value_type_value(type_int));
    mod_add_val(types, type_name(type_dir), value_type_value(type_dir));
    mod_add_val(types, type_name(type_cmd), value_type_value(type_cmd));
    mod_add_val(types, type_name(type_func), value_type_value(type_func));
    mod_add_val(types, type_name(type_stream), value_type_value(type_stream));
    mod_add_val(types, type_name(type_ipaddr), value_type_value(type_ipaddr));
    mod_add_val(types, type_name(type_macaddr), value_type_value(type_macaddr));
    mod_add_val(types, type_name(type_coroutine),
                value_type_value(type_coroutine));
    mod_add_val(types, type_name(type_mem), value_type_value(type_mem));

    mod_addfn(cmds, "typeof",
             "<expr> - returns the type of <expr>",
             &fn_typeof, 1);
    mod_addfn(cmds, "typename",
             "<expr> - returns the name of the type of <expr>",
             &fn_typename, 1);
    mod_add(cmds, "type", "<type name> - type value",  &cmd_type);
    mod_addfn(cmds, "cmp",
             "<expr> <expr> - returns integer comparing its arguments",
             &fn_cmp, 2);
}









/*****************************************************************************
 *                                                                           *
 *          Commands - Boolean                                               *
 *          ==================                                               *
 *                                                                           *
 *****************************************************************************/








static int
fn_generic_cmp(parser_state_t *state)
{   const value_t *v1 = parser_builtin_arg(state, 1);
    const value_t *v2 = parser_builtin_arg(state, 2);
    return value_cmp(v1, v2);
}




static const value_t *
fn_equal(const value_t *this_fn, parser_state_t *state)
{   return fn_generic_cmp(state) == 0? value_true: value_false;
}



static const value_t *
fn_notequal(const value_t *this_fn, parser_state_t *state)
{   return fn_generic_cmp(state) != 0? value_true: value_false;
}



static const value_t *
fn_lessthan(const value_t *this_fn, parser_state_t *state)
{   return fn_generic_cmp(state) < 0? value_true: value_false;
}



static const value_t *
fn_lessequal(const value_t *this_fn, parser_state_t *state)
{   return fn_generic_cmp(state) <= 0? value_true: value_false;
}




static const value_t *
fn_morethan(const value_t *this_fn, parser_state_t *state)
{   return fn_generic_cmp(state) > 0? value_true: value_false;
}



static const value_t *
fn_moreequal(const value_t *this_fn, parser_state_t *state)
{   return fn_generic_cmp(state) >= 0? value_true: value_false;
}



static const value_t *
fn_not(const value_t *this_fn, parser_state_t *state)
{   const value_t *v1 = parser_builtin_arg(state, 1);
    return value_cmp(v1, value_false) == 0? value_true: value_false;
}


static const value_t *
fn_and(const value_t *this_fn, parser_state_t *state)
{   const value_t *v1 = parser_builtin_arg(state, 1);
    const value_t *v2 = parser_builtin_arg(state, 2);
    return value_cmp(v1, value_false) == 0? value_false: v2;
}


static const value_t *
fn_or(const value_t *this_fn, parser_state_t *state)
{   const value_t *v1 = parser_builtin_arg(state, 1);
    const value_t *v2 = parser_builtin_arg(state, 2);
    return value_cmp(v1, value_false) == 0? v2: value_true;
}




static void
cmds_generic_bool(parser_state_t *state, dir_t *cmds)
{   value_t *op_eq = mod_addfn(cmds, "equal",
              "<val> <val> - TRUE if first <val> equal to second",
              &fn_equal, 2);
    value_t *op_ne = mod_addfn(cmds, "notequal",
              "<val> <val> - TRUE if first <val> not equal to second",
              &fn_notequal, 2);
    value_t *op_lt = mod_addfn(cmds, "less",
              "<val> <val> - TRUE if first <val> less than second",
              &fn_lessthan, 2);
    value_t *op_le = mod_addfn(cmds, "lesseq",
              "<val> <val> - TRUE if first <val> less than or equal to second",
              &fn_lessequal, 2);
    value_t *op_gt = mod_addfn(cmds, "more",
              "<val> <val> - TRUE if first <val> more than second",
              &fn_morethan, 2);
    value_t *op_ge = mod_addfn(cmds, "moreeq",
              "<val> <val> - TRUE if first <val> more than or equal to second",
              &fn_moreequal, 2);
    value_t *op_not = mod_addfn(cmds, "invert",
              "<val> - TRUE if <val> is FALSE, FALSE otherwise",
              &fn_not, 1);
    value_t *op_and = mod_addfn(cmds, "logand",
              "<val1> <val2> - FALSE if <val1> is FALSE, <val2> otherwise",
              &fn_and, 2);
    value_t *op_or = mod_addfn(cmds, "logor",
              "<val1> <val2> - TRUE if <val1> is TRUE, <val2> otherwise",
              &fn_or, 2);

    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "==", op_eq);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "!=", op_ne);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "lt", op_lt);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "_lt_", op_lt);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "le", op_le);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "_le_", op_le);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "gt", op_gt);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "_gt_", op_gt);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "ge", op_ge);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "_ge_", op_ge);

    mod_add_op(parser_opdefs(state), OP_PREC_NOT, assoc_fy,  "not", op_not);
    mod_add_op(parser_opdefs(state), OP_PREC_NOT, assoc_fy,  "_not_", op_not);
    mod_add_op(parser_opdefs(state), OP_PREC_AND, assoc_xfy, "and", op_and);
    mod_add_op(parser_opdefs(state), OP_PREC_AND, assoc_xfy, "_and_", op_and);
    mod_add_op(parser_opdefs(state), OP_PREC_OR,  assoc_xfy, "or",  op_or);
    mod_add_op(parser_opdefs(state), OP_PREC_OR,  assoc_xfy, "_or_",  op_or);

    mod_add_val(cmds, "TRUE", value_true);
    mod_add_val(cmds, "FALSE", value_false);

}






/*****************************************************************************
 *                                                                           *
 *          Commands - Integer                                               *
 *          ==================                                               *
 *                                                                           *
 *****************************************************************************/








static const value_t *
fn_int_neg(const value_t *this_fn, parser_state_t *state)
{   const value_t *nval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(nval, type_int))
    {   number_t n = value_int_number(nval);
        val = value_int_new(-n);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_add(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   number_t n1 = value_int_number(n1val);
        number_t n2 = value_int_number(n2val);
        val = value_int_new(n1+n2);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_sub(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   number_t n1 = value_int_number(n1val);
        number_t n2 = value_int_number(n2val);
        val = value_int_new(n1-n2);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_mul(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   number_t n1 = value_int_number(n1val);
        number_t n2 = value_int_number(n2val);
        val = value_int_new(n1*n2);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_div(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   number_t n1 = value_int_number(n1val);
        number_t n2 = value_int_number(n2val);
        val = value_int_new(n1/n2);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_mod(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   number_t n1 = value_int_number(n1val);
        number_t n2 = value_int_number(n2val);
        val = value_int_new(n1%n2);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_shl(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   number_t n1 = value_int_number(n1val);
        number_t n2 = value_int_number(n2val);
        number_t accum = 0;

        if (n2 <= 0)
            accum = n1;
        else if (n2 < 8*sizeof(number_t))
            accum = (number_t) (((unumber_t)n1) << (unsigned)n2);

        OMIT(printf("%s: %"F_NUMBER_T" %s %"F_NUMBER_T
                      " == %"F_NUMBER_T"\n",
                      codeid(), n1, "lsh", opnd, accum);)
        val = value_int_new(accum);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_shr(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   number_t n1 = value_int_number(n1val);
        number_t n2 = value_int_number(n2val);
        number_t accum = 0;

        if (n2 <= 0)
            accum = n1;
        else if (n2 < 8*sizeof(number_t))
            accum = (number_t) (((unumber_t)n1) >> (unsigned)n2);

        OMIT(printf("%s: %"F_NUMBER_T" %s %"F_NUMBER_T
                      " == %"F_NUMBER_T"\n",
                      codeid(), n1, "rsh", opnd, accum);)
        val = value_int_new(accum);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_bitand(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   unumber_t n1 = value_int_number(n1val);
        unumber_t n2 = value_int_number(n2val);
        val = value_int_new(n1 & n2);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_bitor(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   unumber_t n1 = value_int_number(n1val);
        unumber_t n2 = value_int_number(n2val);
        val = value_int_new(n1 | n2);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_bitxor(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *n2val = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int) && value_istype(n2val, type_int))
    {   unumber_t n1 = value_int_number(n1val);
        unumber_t n2 = value_int_number(n2val);
        val = value_int_new(n1 ^ n2);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_bitnot(const value_t *this_fn, parser_state_t *state)
{   const value_t *n1val = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(n1val, type_int))
    {   unumber_t n1 = value_int_number(n1val);
        val = value_int_new(~n1);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_rndseed(const value_t *this_fn, parser_state_t *state)
{   const value_t *seedval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    const char *seedbin = NULL;
    size_t seedlen = 0;
    bool is_int = value_istype(seedval, type_int);

    if (is_int || value_string_get(seedval, &seedbin, &seedlen))
    {
        unsigned int seed = 0;
        unsigned int *seedparts;
        int seedpartcount;
        number_t seedint;
        if (is_int)
        {   seedint = value_int_number(seedval);
            seedbin = (char *)&seedint;
            seedlen = sizeof(seedint);
        }
        seedparts = (unsigned int *)seedbin;
        seedpartcount = (int)(seedlen/sizeof(unsigned int));
        seedlen -= seedpartcount * sizeof(unsigned int);
        /* make a seed by exclusive-oring all the unsigned ints in the binary */
        while (seedpartcount-- > 0)
            seed ^= *seedparts++;
        seedbin = (char *)seedparts;
        if (seedlen > 0)
        {   unsigned int lastpart = 0;
            char *lastpartbin = (char *)&lastpart; 
            while (seedlen-- > 0)
                *lastpartbin++ = *seedbin++;
            seed ^= lastpart;
        }
	srand(seed);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_rnd(const value_t *this_fn, parser_state_t *state)
{   const value_t *upb = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(upb, type_int))
    {   number_t upbval = value_int_number(upb);
        val = value_int_new((int)(((float)upbval)*rand()/(RAND_MAX+1.0)));
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_int_set_hexbits(const value_t *this_fn, parser_state_t *state)
{   const value_t *bitsval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(bitsval, type_int))
    {   unumber_t bits = value_int_number(bitsval);
        val = value_int_new(value_int_get_fmt_bits_indec());
        value_int_set_fmt_bits_indec(bits);
    } else
        parser_report_help(state, this_fn);

    return val;
}




static void
cmds_generic_int(parser_state_t *state, dir_t *cmds)
{   value_t *op_add = mod_addfn(cmds, "add",
              "<n1> <n2> - return n1 with n2 added",  &fn_int_add, 2);
    value_t *op_sub = mod_addfn(cmds, "sub",
              "<n1> <n2> - return n1 with n2 subtracted",  &fn_int_sub, 2);
    value_t *op_mul = mod_addfn(cmds, "mul",
              "<n1> <n2> - return n1 multiplied by n2",  &fn_int_mul, 2);
    value_t *op_div = mod_addfn(cmds, "div",
              "<n1> <n2> - return n1 divided by n2",  &fn_int_div, 2);
    value_t *op_mod = mod_addfn(cmds, "mod",
              "<n1> <n2> - return n1 modulus n2",  &fn_int_mod, 2);
    value_t *op_shl = mod_addfn(cmds, "shiftl",
              "<n1> <n2> - return n1 left shifted by n2 bits",
                                &fn_int_shl, 2);
    value_t *op_shr = mod_addfn(cmds, "shiftr",
              "<n1> <n2> - return n1 right shifted by n2 bits",
                                &fn_int_shr, 2);
    value_t *op_neg = mod_addfn(cmds, "neg",
              "<n> - return negated <n>",  &fn_int_neg, 1);
    value_t *op_bitand = mod_addfn(cmds, "bitand",
              "<n1> <n2> - return bit values of <n1> anded with <n2>",
                              &fn_int_bitand, 2);
    value_t *op_bitor = mod_addfn(cmds, "bitor",
              "<n1> <n2> - return bit values of <n1> ored with <n2>",
                              &fn_int_bitor, 2);
    value_t *op_bitxor = mod_addfn(cmds, "bitxor",
              "<n1> <n2> - return bit values of <n1> exclusive ored with <n2>",
                              &fn_int_bitxor, 2);
    value_t *op_bitnot = mod_addfn(cmds, "bitnot",
              "<n> - invert each bit in <n>",
                              &fn_int_bitnot, 1);

    mod_addfn(cmds, "rnd",
              "<n> - return random number less than <n>",
              &fn_rnd, 1);
    mod_addfn(cmds, "rndseed",
              "<n> | <string> - set random seed based on argument",
              &fn_rndseed, 1);
    mod_addfn(cmds, "int_fmt_hexbits",
              "<n> - set bits that won't result in integer hex "
              "printing in hex when set",
              &fn_int_set_hexbits, 1);
    mod_add(cmds, "int",
            "<integer expr> - numeric value",  &value_int_parse);

    mod_add_op(parser_opdefs(state), OP_PREC_SIGN,   assoc_fy,  "-",   op_neg);
    mod_add_op(parser_opdefs(state), OP_PREC_PROD,   assoc_xfy, "*",   op_mul);
    mod_add_op(parser_opdefs(state), OP_PREC_PROD,   assoc_xfy, "/",   op_div);
    mod_add_op(parser_opdefs(state), OP_PREC_PROD,   assoc_xfy, "rem", op_mod);
    mod_add_op(parser_opdefs(state), OP_PREC_PROD,   assoc_xfy, "_rem_",op_mod);
    mod_add_op(parser_opdefs(state), OP_PREC_SUM,    assoc_xfy, "+",   op_add);
    mod_add_op(parser_opdefs(state), OP_PREC_SUM,    assoc_xfy, "-",   op_sub);
    mod_add_op(parser_opdefs(state), OP_PREC_SHIFT,  assoc_xfy, "shl", op_shl);
    mod_add_op(parser_opdefs(state), OP_PREC_SHIFT,  assoc_xfy, "_shl_",op_shl);
    mod_add_op(parser_opdefs(state), OP_PREC_SHIFT,  assoc_xfy, "shr", op_shr);
    mod_add_op(parser_opdefs(state), OP_PREC_SHIFT,  assoc_xfy, "_shr_",op_shr);

    mod_add_op(parser_opdefs(state), OP_PREC_BITNOT, assoc_fy,  "_bitnot_",
               op_bitnot);
    mod_add_op(parser_opdefs(state), OP_PREC_BITAND, assoc_xfy, "_bitand_",
               op_bitand);
    mod_add_op(parser_opdefs(state), OP_PREC_BITXOR, assoc_xfy, "_bitxor_",
               op_bitxor);
    mod_add_op(parser_opdefs(state), OP_PREC_BITOR,  assoc_xfy, "_bitor_",
               op_bitor);
}









/*****************************************************************************
 *                                                                           *
 *          Dumping Memory                                                   *
 *                                                                           *
 *****************************************************************************/



/*! Print out an area of memory in bytes, half-words or words */
static int /* entries consumed */
dumpline(FILE *out, const void *buffer, int entries, int sz_ln2,
         int perline, bool with_chars)
{  const unsigned char *buf = (const unsigned char *)buffer;
   const unsigned char *bufend = &buf[entries<<sz_ln2];
   int consumed = 0;
   int i;

   for (i=0; i<perline; i++) {
      if (&buf[i<<sz_ln2] < bufend) {
         unsigned long val = 0;
         memcpy(&val, &buf[i<<sz_ln2], 1<<sz_ln2);
         fprintf(out, "%0*lx ", 2<<sz_ln2, val);
         consumed++;
      } else
         fprintf(out, "%*s ", 2<<sz_ln2, "");
   }
   if (with_chars) {
      const unsigned char *cbuf = (const unsigned char *)buffer;
      fprintf(out,"   ");
      for (i=0; i<(perline<<sz_ln2); i++) {
         if (buf+i < bufend) {
            unsigned char val = cbuf[i];
            fprintf(out, "%c", isprint(val)? val: '.');
         } else
            fprintf(out, " ");
      }
   }
   return consumed;
}




/*! Print out an area of memory in bytes, half-words or words */
extern void
mem_dump(const void *buf, unsigned addr, int entries, int sz_ln2,
         const char *addr_format, int addr_unit_size, bool with_chars)
{  int perline=16>>sz_ln2;
   unsigned linebytes = perline << sz_ln2;
   FILE *out = stdout;

   while (entries > 0) {
      int entused;
      fprintf(out, addr_format, addr/addr_unit_size);
      entused = dumpline(out, buf, entries, sz_ln2, perline, with_chars);
      fprintf(out, "\n");
      entries -= entused;
      addr += linebytes;
      buf = (const void *)((const char *)buf + linebytes);
   }
}




/*! Print out an area of memory in bytes, half-words or words */
extern unsigned /* no of units different */
mem_dumpdiff(const void *buf1, const void *buf2, unsigned addr, int units,
             int sz_ln2, const char *addr_format, int addr_unit_size,
             bool with_chars)
{  int perline = 8>>sz_ln2;   /* entries per line */
   FILE *out = stdout;
   unsigned diffunits = 0;
   const char *b1 = (const char *)buf1;
   const char *b2 = (const char *)buf2;
   size_t totalbytes = units<<sz_ln2;
   const char *b1end = b1 + totalbytes;

   DEBUG_MEMDIFF(printf("%s: diff %d bytes\n", codeid(), b1end - b1););
   while (b1 < b1end) {
      int entries = 0;
      int n = 0;

      /* skip common prefix */
      while (&b1[n] < b1end && b1[n] == b2[n])
         n++;

      DEBUG_MEMDIFF(printf("%s: prefix %d bytes\n", codeid(), n););
      n = n & ~((1<<sz_ln2) - 1);  /* align down */
      b1 += n;
      b2 += n;
      addr += n;
      n = 0;
      while (&b1[n] < b1end &&
             0 != memcmp(&b1[n], &b2[n], 1<<sz_ln2)) {
         n += 1 <<sz_ln2;
      }
      entries = n >> sz_ln2;

      DEBUG_MEMDIFF(printf("%s: differ %d bytes %d units\n",
                          codeid(), n, entries););
      while (entries > 0) {
         int entused;
         int bytused;

         fprintf(out, addr_format, addr/addr_unit_size);
         entused = dumpline(out, b1, entries, sz_ln2, perline, with_chars);
         bytused = entused << sz_ln2;
         fprintf(out, " | ");
         (void)dumpline(out, b2, entries, sz_ln2, perline, with_chars);
         fprintf(out, "\n");
         entries -= entused;
         diffunits += entused;
         addr += bytused;
         DEBUG_MEMDIFF(printf("%s: consumed %d units %d bytes\n",
                             codeid(), entused, bytused););
         b1 += bytused;
         b2 += bytused;
      }
   }
   return diffunits;
}






/*****************************************************************************
 *                                                                           *
 *          Commands - String                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/








static const value_t *
fn_str(const value_t *this_fn, parser_state_t *state)
{   /* syntax: file <name> <access> */
    const value_t *obj = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (NULL != obj)
    {   charsink_string_t stream;
        charsink_t *sink = charsink_string_init(&stream);
        const char *body;
        size_t blen;

        value_print(sink, dir_value(parser_root(state)), obj);
        charsink_string_buf(sink, &body, &blen);
        val = value_string_new(body, blen);
        charsink_string_close(sink);
    } else
        parser_report_help(state, this_fn);

    return val;
}







typedef const value_t *
fn_fmt_fn_t(char *buf, size_t buflen, fprint_flags_t flags,
            int precision, const value_t *argval, parser_state_t *state);


static const value_t *
fn_fmt_generic(const value_t *this_fn, parser_state_t *state,
               fn_fmt_fn_t genformat)
{   const value_t *flagsval = parser_builtin_arg(state, 1);
    const value_t *precisionval = parser_builtin_arg(state, 2);
    const value_t *argval = parser_builtin_arg(state, 3);
    const value_t *val = &value_null;

    if (value_istype(flagsval, type_int) &&
        value_istype(precisionval, type_int))
    {   fprint_flags_t flags = (fprint_flags_t)value_int_number(flagsval);
        int precision = (int)value_int_number(precisionval);
        char numbuf[64];
        val = (*genformat)(&numbuf[0], sizeof(numbuf), flags,
                           precision, argval, state);
    }

    return val;
}






static const value_t *
genfn_fmt_d(char *buf, size_t buflen, fprint_flags_t flags, int precision,
            const value_t *argval, parser_state_t *state)
{   const value_t *val = &value_null;

    if (value_istype(argval, type_int))
    {   const char *format;
        number_t arg = value_int_number(argval);
        int len;

        if (0 != (flags & (1<<ff_sign)))
            format = "%+.*"F_NUMBER_T;
        else
            format = "%.*"F_NUMBER_T;

        len = snprintf(buf, buflen, format, precision, arg);

        if (len >= 0)
            val = value_string_new(buf, len);
    }

    return val;
}






static const value_t *
genfn_fmt_u(char *buf, size_t buflen, fprint_flags_t flags, int precision,
            const value_t *argval, parser_state_t *state)
{   const value_t *val = &value_null;

    if (value_istype(argval, type_int))
    {   const char *format;
        unumber_t arg = value_int_number(argval);
        int len;

        if (0 != (flags & (1<<ff_sign)))
            format = "%+.*"F_UNUMBER_T;
        else
            format = "%.*"F_UNUMBER_T;

        len = snprintf(buf, buflen, format, precision, arg);
        if (len >= 0)
            val = value_string_new(buf, len);
    }

    return val;
}





static const value_t *
genfn_fmt_x(char *buf, size_t buflen, fprint_flags_t flags, int precision,
            const value_t *argval, parser_state_t *state)
{   const value_t *val = &value_null;

    if (value_istype(argval, type_int))
    {   const char *format;
        unumber_t arg = value_int_number(argval);
        int len;

        if (0 != (flags & (1<<ff_sign)))
            format = "%+.*"FX_UNUMBER_T;
        else
            format = "%.*"FX_UNUMBER_T;
        len = snprintf(buf, buflen, format, precision, arg);
        if (len >= 0)
            val = value_string_new(buf, len);
    }

    return val;
}






static const value_t *
genfn_fmt_x_uc(char *buf, size_t buflen, fprint_flags_t flags, int precision,
            const value_t *argval, parser_state_t *state)
{   const value_t *val = &value_null;

    if (value_istype(argval, type_int))
    {   const char *format;
        unumber_t arg = value_int_number(argval);
        int len;

        if (0 != (flags & (1<<ff_sign)))
            format = "%+.*"FXC_UNUMBER_T;
        else
            format = "%.*"FXC_UNUMBER_T;
        len = snprintf(buf, buflen, format, precision, arg);
        if (len >= 0)
            val = value_string_new(buf, len);
    }

    return val;
}






static const value_t *
genfn_fmt_s(char *buf, size_t buflen, fprint_flags_t flags, int precision,
            const value_t *argval, parser_state_t *state)
{   const value_t *val = &value_null;
    const char *strbuf;
    size_t strsize;

    (void)buf;
    (void)buflen;

    if (value_string_get(argval, &strbuf, &strsize))
    {   if (precision > 0 && precision < (int)strsize)
            strsize = precision;
        val = value_string_new(strbuf, strsize);
    }

    return val;
}






static const value_t *
genfn_fmt_v(char *buf, size_t buflen, fprint_flags_t flags, int precision,
            const value_t *argval, parser_state_t *state)
{   const value_t *val = &value_null;
    charsink_string_t stream;
    charsink_t *sink = charsink_string_init(&stream);
    const char *strbuf;
    size_t strsize;

    (void)buf;
    (void)buflen;

    value_print(sink, dir_value(parser_root(state)), argval);
    charsink_string_buf(sink, &strbuf, &strsize);
    if (precision > 0 && precision < (int)strsize)
        strsize = precision;
    val = value_string_new(strbuf, strsize);
    charsink_string_close(sink);

    return val;
}






static const value_t *
fn_fmt_d(const value_t *this_fn, parser_state_t *state)
{   return fn_fmt_generic(this_fn, state, &genfn_fmt_d);
}


static const value_t *
fn_fmt_u(const value_t *this_fn, parser_state_t *state)
{   return fn_fmt_generic(this_fn, state, &genfn_fmt_u);
}


static const value_t *
fn_fmt_x(const value_t *this_fn, parser_state_t *state)
{   return fn_fmt_generic(this_fn, state, &genfn_fmt_x);
}


static const value_t *
fn_fmt_x_uc(const value_t *this_fn, parser_state_t *state)
{   return fn_fmt_generic(this_fn, state, &genfn_fmt_x_uc);
}


static const value_t *
fn_fmt_s(const value_t *this_fn, parser_state_t *state)
{   return fn_fmt_generic(this_fn, state, &genfn_fmt_s);
}


static const value_t *
fn_fmt_v(const value_t *this_fn, parser_state_t *state)
{   return fn_fmt_generic(this_fn, state, &genfn_fmt_v);
}








static const value_t *
fn_strf(const value_t *this_fn, parser_state_t *state)
{   const value_t *formatval = parser_builtin_arg(state, 1);
    const value_t *envval = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;
    const char *format;
    size_t formatlen = 0;
    dir_t *env;

    if (value_string_get(formatval, &format, &formatlen) &&
        value_as_dir(envval, &env))
    {   charsink_string_t strsink;
        charsink_t *sink = charsink_string_init(&strsink);
        const char *outstr;
        size_t outstrlen;

        (void)value_fprintfmt(state, sink, (char *)format,
                              formatlen, env);
        charsink_string_buf(sink, &outstr, &outstrlen);
        val = value_string_new(outstr, outstrlen);
        charsink_string_close(sink);
    } else
        parser_report_help(state, this_fn);
    return val;
}







static const value_t *
fn_strcoll(const value_t *this_fn, parser_state_t *state)
{   const value_t *str1 = parser_builtin_arg(state, 1);
    const value_t *str2 = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;
    const char *str1buf;
    const char *str2buf;
    size_t str1len;
    size_t str2len;

    if (value_string_get(str1, &str1buf, &str1len) &&
        value_string_get(str2, &str2buf, &str2len))
    {   val = value_int_new(strcoll(str1buf, str2buf));
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_strlen(const value_t *this_fn, parser_state_t *state)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(dir, type_string))
    {   size_t len = 0;
        const char *buf;
        size_t wlen = 0;
        (void)value_string_get(dir, &buf, &len);
        while ((int)len > 0 && (int)wlen >= 0)
        {   size_t mbchrs = mblen(buf, len);
            if (mbchrs > 0)
            {   wlen++;
                buf += mbchrs;
                len -= mbchrs;
            } else if (mbchrs == 0)
            {   buf++;
                len--;
            } else
                wlen = mbchrs;
        }
        val = value_int_new(wlen);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
genfn_wctrans(const value_t *this_fn, parser_state_t *state,
              wint_t (*ref_trans_fn)(wint_t wc))
{   const value_t *strval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(strval, type_string))
    {   size_t len = 0;
        const char *buf;
        charsink_string_t strsink;
        charsink_t *sink = charsink_string_init(&strsink);
        int rc = 0;

        (void)value_string_get(strval, &buf, &len);
        (void)mbtowc(NULL, NULL, 0); /* reset mb counting state */

        while (len > 0 && rc == 0)
        {   wchar_t wc = 0;
            int mbchrs = mbtowc(&wc, buf, len);
            if (mbchrs > 0)
            {   int olen = charsink_putwc(sink, (*ref_trans_fn)(wc));
                if (olen < 0)
                    rc = -olen;
                buf += mbchrs;
                len -= mbchrs;
            } else if (mbchrs == 0)
            {   int olen = charsink_putc(sink, buf[0]);
                if (olen < 0)
                    rc = -olen;
                buf++;
                len--;
            } else
                rc = EINVAL;
        }
        {   const char *outstr;
            size_t outstrlen;
            charsink_string_buf(sink, &outstr, &outstrlen);
            val = value_string_new(outstr, outstrlen);
        }
        charsink_string_close(sink);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_toupper(const value_t *this_fn, parser_state_t *state)
{   return genfn_wctrans(this_fn, state, &FTL_TOWUPPER);
}





static const value_t *
fn_tolower(const value_t *this_fn, parser_state_t *state)
{   return genfn_wctrans(this_fn, state, &FTL_TOWLOWER);
}





static const value_t *
fn_octet(const value_t *this_fn, parser_state_t *state)
{   const value_t *chval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    bool ok = FALSE;

    if (value_istype(chval, type_int))
    {   number_t chn = value_int_number(chval);
        ok = -128 <= chn && chn < 256;
        if (ok) {
            char ch[2];
            ch[0] = (char)(chn & 0xff);
            ch[1] = '\0';
            val = value_string_new(&ch[0], 1);
        }
    }
    if (!ok)
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_chr(const value_t *this_fn, parser_state_t *state)
{   const value_t *chval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    bool ok = FALSE;

    if (value_istype(chval, type_int))
    {   number_t chn = value_int_number(chval);
        char mbstring[FTL_MB_LEN_MAX+1];
        wchar_t ch = (wchar_t)chn;
        size_t len = wctomb(&mbstring[0], ch);

        if ((int)len >= 0 && len < sizeof(mbstring))
        {   ok = TRUE;
            mbstring[len] = '\0';
            val = value_string_new(&mbstring[0], len);
        }
    }
    if (!ok)
        parser_report_help(state, this_fn);

    return val;
}









static const value_t *
fn_chrcode(const value_t *this_fn, parser_state_t *state)
{   const value_t *chval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    const char *chbuf;
    size_t chsize;
    bool ok = FALSE;

    if (value_string_get(chval, &chbuf, &chsize))
    {   wchar_t ch = 0;
        size_t len;
        mbtowc(NULL, NULL, 0); /* reset shift state */
        len = mbtowc(&ch, chbuf, chsize);

        if (len > 0)
        {   ok = TRUE;
            val = value_int_new(ch);
        }
    }
    if (!ok)
        parser_report_help(state, this_fn);

    return val;
}









static const value_t *
fn_octetcode(const value_t *this_fn, parser_state_t *state)
{   const value_t *chval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    const char *chbuf;
    size_t chsize;
    bool ok = FALSE;

    if (value_string_get(chval, &chbuf, &chsize))
    {   if (chsize > 0)
        {   ok = TRUE;
            val = value_int_new((unsigned)(0xff & chbuf[0]));
        }
    }
    if (!ok)
        parser_report_help(state, this_fn);

    return val;
}




static char *
my_memmem(const char *buf, size_t len, const char *find, size_t findlen)
{
    const char *end = buf + len;
    const char *last = end - findlen;
    /**< last pos where a search for find makes sense */
    char *p = NULL; /* overall pointer result */
    char *sp = NULL; /* sub-string pointer result */

    while (NULL == p &&
           buf < last &&
           NULL != (sp = memchr(buf, find[0], end-buf)) &&
           sp <= last)
    {
        /* this is a candidate position - the first character matching
           the first character of find */
        if (0 == memcmp(sp+1, &find[1], findlen-1))
            p = sp; /* found it */
        else
            buf = sp+1;
            /* search again in the next buf substring */
    }
    return p;
}




/* find a C-NULL-terminated string in a buffer */
static char *my_strnstr(const char *buf, size_t len, const char *find)
{
    /* No nulls in sought string - so if it occurs it must always occur
       in a null-terminated section of buf.
       Note that, commonly, len is < strlen(buf).
    */
    size_t findlen = strlen(find);
    size_t buflen0 = strlen(buf); /* first section, at least */
    const char *end = buf + len;
    const char *last = end - findlen;
    /**< last pos where a search for find makes sense */
    char *p = NULL;

    if (buflen0 > len + 256)
        /* buf is just the first part of a fairly big null-teminated area
           don't use strstr */
        return my_memmem(buf, len, find, findlen);
    else {
        while (buf <= last && (NULL == (p = strstr(buf, find))))
            buf += strlen(buf)+1;

        /*printf("strnstr: %d bytes %sfound in %d bytes\n",
                  findlen, p == NULL?"not ":"", len);*/
        /* Warning - we can access areas of buf that are beyond its length */
        if (p != NULL && p + findlen > end) {
            /* printf("strnstr: search %p [%d] found %p %d, %d after last %p\n",
                   buf, len, p, p - last - findlen + len,
                   p + findlen - last, last);
            */
            p = NULL;
        }

        return p;
    }
}




/**< on linux this routine is provided as memmem */
static char *strnstrn(const char *buf, size_t len,
                      const char *find, size_t findlen)
{  size_t len0 = strlen(buf);
   size_t findlen0 = strlen(find);

   if (findlen0 == findlen) {
       if (len0 == len)
           /* both strings are 'c' string terminated */
           return strstr(buf, find);
       else
           return my_strnstr(buf, len, find);
   } else if (findlen0 < findlen) {
       /* There's a NULL in the find string */
       if (len0 == len)
           /* if the sought string has a NULL in it, and the string we're
              searching in doesn't it stands to reason that you won't find
              the sought string there */
           return NULL;
       else {
           /* There's a NULL in the find string and the string to be
              searched may or may not contain a NULL*/
           /* the null-terminated strings in the sought string must occur
              in sequence as the last, the whole... and the first
              null-terminated parts of buf */
           const char *end = buf + len;
           const char *last = end - findlen;
           /**< last pos where a search for find makes sense */
           char *p = NULL; /* overall pointer result */
           char *sp = NULL; /* sub-string pointer result */

           while (NULL == p &&
                  buf < last &&
                  NULL != (sp = my_strnstr(buf, end-buf, find)) &&
                  sp <= last &&
                  findlen0 == strlen(sp))
           {
               /* this is a candidate position - the first string of find
                  is at the end of a string in buf */
               if (0 == memcmp(sp+findlen0+1, find+findlen0+1,
                               findlen-(findlen0+1)))
                   p = sp; /* found it */
               else {
                   buf = sp+strlen(sp)+1;
                   /* search again in the next buf substring */
               }
           }
           return p;
       }
   } else {
       /* findlen0 is meaningless - the nearest NULL is past the end */

       /* oh dear - there's no fast C library routine that can help here */
       return my_memmem(buf, len, find, findlen);
   }
}



static const value_t *
fn_chop(const value_t *this_fn, parser_state_t *state)
{   const value_t *strideval = parser_builtin_arg(state, 1);
    const value_t *str = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(str, type_string) &&
        value_istype(strideval, type_int))
    {
        const char *buf;
        const char *initbuf;
        size_t buflen = 0;
        long len;
        number_t stride = value_int_number(strideval);
        dir_t *vec = dir_vec_new();
        int veclen = 0;

        (void)value_string_get(str, &initbuf, &buflen);
        len = buflen; /* make sure it's a signed variable */
        buf = initbuf;

        /*printf("stride %"F_NUMBER_T"\n", stride);*/

        if (stride > 0)
        {   /* split into individual octet substrings forward */
            while (len > 0)
            {   /*printf("vec[%d]=%p[%d,%d]\n",
                         veclen,initbuf,buf-initbuf,len);*/
                if (len < stride)
                    dir_int_set(vec, veclen++,
                                value_substring_new(str, buf-initbuf, len));
                else
                    dir_int_set(vec, veclen++,
                                value_substring_new(str, buf-initbuf,
                                                    (size_t)stride));
                buf += stride;
                len -= (size_t)stride;
            }
        } else if (stride < 0) {
            /* split into individual octet substrings going backwards */
            /* remember: stride is negative */
            buf = buf+len+stride;

            while (len > 0)
            {   if (len < -stride)
                    dir_int_set(vec, veclen++,
                                value_substring_new(str,
                                                    buf-initbuf+
                                                    ((size_t)(-stride))-len,
                                                    len));
                else
                    dir_int_set(vec, veclen++,
                                value_substring_new(str, buf-initbuf,
                                                    (size_t)(-stride)));
                buf += stride;
                len -= (size_t)(-stride);
            }
        }
        val = dir_value(vec);
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_split(const value_t *this_fn, parser_state_t *state)
{   const value_t *delim = parser_builtin_arg(state, 1);
    const value_t *str = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;
    bool byte_delim = value_type_equal(delim, type_null);

    if (value_istype(str, type_string) &&
        (byte_delim || value_istype(delim, type_string)))
    {
        const char *buf;
        const char *initbuf;
        size_t buflen = 0;
        const char *delbuf;
        size_t delimlen = 0;
        dir_t *vec = dir_vec_new();
        int veclen = 0;
        int len;

        (void)value_string_get(str, &initbuf, &buflen);
        buf = initbuf;
        len = (int)buflen; /* transfer to value we know is signed */
        if (!byte_delim)
            (void)value_string_get(delim, &delbuf, &delimlen);

        if (delimlen == 0)
        {   /* split into individual bytes/chars */
            if (byte_delim)
            {   while (len > 0)
                {   dir_int_set(vec, veclen++,
                                value_substring_new(str, buf-initbuf,1));
                    buf++;
                    len--;
                }
            } else
            {   while (len > 0)
                {   int mbchrs = mblen(buf, len);
                    if (mbchrs > 0)
                    {   dir_int_set(vec, veclen++,
                                    value_substring_new(str, buf-initbuf,
                                                        mbchrs));
                        buf += mbchrs;
                        len -= mbchrs;
                    } else if (mbchrs == 0)
                    {   dir_int_set(vec, veclen++,
                                    value_substring_new(str, buf-initbuf, 1));
                        buf++;
                        len--;
                    } else
                    {   dir_int_set(vec, veclen++, &value_null);
                        buf++;
                        len--;
                    }
                }
            }
        } else
        {   if (0 == strlen(delbuf)) /* i.e. 1st char is '\0' */
            {   /* treat delimiter string as if it were '\0' */
                delimlen = 1;
                while (len > 0)
                {   size_t prefix_len = strlen(buf); /* up to next '\0' */
                    dir_int_set(vec, veclen++,
                                value_string_new(buf, prefix_len));
                    len -= (int)(prefix_len+delimlen);
                    buf += prefix_len+delimlen;
                }
            } else
            {   while (len > 0)
                {   char *p = strnstrn(buf, len, delbuf, delimlen);
                    if (NULL == p || p-buf > len /*gone too far*/)
                    {   dir_int_set(vec, veclen++,
                                    value_substring_new(str, buf-initbuf,
                                                        len));
                        len = 0;
                    } else
                    {   size_t prefix_len = p-buf;
                        dir_int_set(vec, veclen++,
                                    value_substring_new(str, buf-initbuf,
                                                        prefix_len));
                        len -= (int)(prefix_len+delimlen);
                        buf += prefix_len+delimlen;
                        if (len == 0)
                        {   dir_int_set(vec, veclen++,
                                value_substring_new(str, buf-initbuf-delimlen,
                                                    0));
                        }
                    }
                }
            }
        }

        val = dir_value(vec);
    } else
        parser_report_help(state, this_fn);

    return val;
}




/* cast binary string argument to provided datatype */
static const value_t *
bincast(parser_state_t *state, datatype_t *dtype,
        const char *octets, size_t octet_count)
{
    const value_t *val = NULL;
    memreader_instore_t memreader;
    memreader_t *reader = memreader_instore_init(&memreader, octets);
    unumber_t binpos = 0;
    const value_t *newval;

    newval = (*dtype->binparse)(reader, &binpos, octet_count, dtype);
    OMIT(printf("%s: end pos %d - octets %d\n",
                codeid(), (int)binpos, (int)octet_count););
    if (binpos != octet_count) {
        parser_error(state, "datatype size (%d bytes) does not match "
                     "size of object to be cast (%d)\n",
                     (int)binpos, (int)octet_count);
        /* value_delete(&newval); - we have no-delete-from-heap funciton */
    } else
        val = newval;

    return val;
}





/* cast binary string argument to provided datatype */
static const value_t *fn_binsplit(const value_t *this_fn, parser_state_t *state)
{
    const value_t *val = &value_null;
    const value_t *littleval = parser_builtin_arg(state, 1);
    const value_t *signedval = parser_builtin_arg(state, 2);
    const value_t *unitbytesval = parser_builtin_arg(state, 3);
    const value_t *octetsval = parser_builtin_arg(state, 4);
    const char *octets = NULL;
    size_t octet_count = 0;

    if (value_istype(unitbytesval, type_int) &&
        value_istype_bool(littleval) &&
        value_istype_bool(signedval) &&
        value_string_get(octetsval, &octets, &octet_count))
    {
        bool little_endian = !value_bool_is_false(littleval);
        bool is_signed = !value_bool_is_false(signedval);
        size_t unitbytes = (size_t)value_int_number(unitbytesval);
        datatype_t *intdt = datatype_int_new(is_signed, (int)unitbytes,
                                             little_endian);
        if (intdt == NULL) {
            parser_error(state, "%s-endian %d-byte integers not available "
                         "for casting\n",
                         little_endian? "little": "big", (int)unitbytes);
        } else {
            size_t entries = octet_count/unitbytes;
#if 0
            datatype_seq_t seq_type;
            datatype_t *seq_intdt =
                datatype_seq_init(&seq_type, intdt, entries);
            const value_t *newval;

            OMIT(printf("%s: bincast'ing %d -> %d * %d bytes\n",
                          codeid(), (int)octet_count, (int)entries,
                          (int)unitbytes););
            newval = bincast(state, seq_intdt, octets, entries * unitbytes);
            FTL_FREE(initdt);
            if (newval != NULL)
                val = newval;
#else
            memreader_instore_t *memreader =
                (memreader_instore_t *)FTL_MALLOC(sizeof(memreader_instore_t));

            if (memreader != NULL)
            {
                dir_t *dynseq;
                memreader_t *reader =
                    memreader_instore_init(memreader, (char *)octets);
                dynseq = dir_dynseq_new(intdt, reader, /*first*/0,
                                        entries * unitbytes);
                val = dir_value(dynseq);
            } else
            {   FTL_FREE(intdt);
            }
#endif
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}








typedef struct {
    charsink_t *sink;           /* stream to write chars to */
    bool first;                 /* true on first insertion */
    const value_t *delim;       /* delimiter to insert */
    bool as_bytes;              /* integer = octet not char */
    int rc;                     /* 0 if no error so far */
} for_join_arg_t;





/* forward reference */
static void *
join_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg);







static void
join_charsink(for_join_arg_t *join_arg, const value_t *value)
{   if (value_type_equal(value, type_string))
    {   const char *valstr;
        size_t vallen;
        if (value_string_get(value, &valstr, &vallen))
        {   int len = charsink_write(join_arg->sink, valstr, vallen);
            if (len < 0)
                join_arg->rc = -len;
        }
    } else if (value_type_equal(value, type_int))
    {   if (join_arg->as_bytes)
            charsink_putc(join_arg->sink, (char)value_int_number(value));
        else
        {   int len = charsink_putwc(join_arg->sink,
                                     (wchar_t)value_int_number(value));
            if (len < 0)
                join_arg->rc = -len;
        }
    } else
    {   dir_t *dir;
        if (value_to_dir(value, &dir))
            (void)dir_forall(dir, &join_exec, (void *)join_arg);
    }
}






static void *
join_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   for_join_arg_t *join_arg = (for_join_arg_t *)arg;

    if (value_type_equal(value, type_string) ||
        value_type_equal(value, type_int))
    {   if (join_arg->first)
            join_arg->first = FALSE;
        else
            join_charsink(join_arg, join_arg->delim);
    }
    if (0 == join_arg->rc)
        join_charsink(join_arg, value);

    return join_arg->rc == 0? NULL: (void *)value;
}









static const value_t *
genfn_join(const value_t *this_fn, parser_state_t *state, bool as_bytes)
{   const value_t *delim = parser_builtin_arg(state, 1);
    const value_t *vecval = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;
    dir_t *vecdir;

    if (value_as_dir(vecval, &vecdir))
    {   charsink_string_t strsink;
        for_join_arg_t join_arg;
        const char *body;
        size_t blen;

        join_arg.first = TRUE;
        join_arg.delim = delim;
        join_arg.as_bytes = as_bytes;
        join_arg.sink = charsink_string_init(&strsink);
        join_arg.rc = 0;

        (void)dir_forall(vecdir, &join_exec, (void *)&join_arg);

        charsink_string_buf(join_arg.sink, &body, &blen);
        val = value_string_new(body, blen);
        charsink_string_close(join_arg.sink);
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_joinchr(const value_t *this_fn, parser_state_t *state)
{   return genfn_join(this_fn, state, /*as_bytes*/FALSE);
}





static const value_t *
fn_join(const value_t *this_fn, parser_state_t *state)
{   return genfn_join(this_fn, state, /*as_bytes*/TRUE);
}







static void
cmds_generic_string(parser_state_t *state, dir_t *cmds)
{   dir_stack_t *scope = parser_env_stack(state);

    print_formats = dir_id_new();
    mod_add_dir(cmds, "fmt", print_formats);

    mod_addfn(cmds,
             "str",
              "<expr> - evaluate expression and return string representation",
              &fn_str, 1);

    printf_addformat(type_int, "d", "<f> <p> <val> - %d integer format",
                     &fn_fmt_d);
    printf_addformat(type_int, "u", "<f> <p> <val> - %u unsinged format",
                     &fn_fmt_u);
    printf_addformat(type_int, "x", "<f> <p> <val> - %x hex format",
                     &fn_fmt_x);
    printf_addformat(type_int, "X", "<f> <p> <val> - %X hex format",
                     &fn_fmt_x_uc);
    printf_addformat(type_int, "s", "<f> <p> <val> - %s string format",
                     &fn_fmt_s);
    printf_addformat(type_int, "v", "<f> <p> <val> - %v value format",
                     &fn_fmt_v);

    mod_addfn(cmds,
             "strf",
              "<fmt> <env> - formatted string from values in env",
              &fn_strf, 2);
    mod_addfn(cmds, "collate",
              "<str1> <str2> - compare collating sequence of chars in strings",
              &fn_strcoll, 2);
    mod_addfn(cmds, "toupper",
              "<str> - upper case version of string",
              &fn_toupper, 1);
    mod_addfn(cmds, "tolower",
              "<str> - lower case version of string",
              &fn_tolower, 1);
    mod_addfnscope(cmds, "strlen",
              "[<string>] - number of (possibly multibyte) chars in string",
              &fn_strlen, 1, scope);
    mod_addfn(cmds, "octet",
              "<int> - returns single byte string containing given octet",
              &fn_octet, 1);
    mod_addfn(cmds, "chr",
              "<int> - returns string of (multibyte) char with given ordinal",
              &fn_chr, 1);
    mod_addfn(cmds, "octetcode",
              "<string> - returns ordinal of the first byte of string",
              &fn_octetcode, 1);
    mod_addfn(cmds, "chrcode",
              "<string> - returns ordinal of the first character of string",
              &fn_chrcode, 1);
    mod_addfn(cmds, "split",
              "<delim> <str> - make vector of strings separated by <delim>s",
              &fn_split, 2);
    mod_addfn(cmds, "binsplit",
              "<le?> <signed?> <n> <str> - make vector of <signed>> <n>-byte "
              "ints with <le?> endianness",
              &fn_binsplit, 4);
    mod_addfn(cmds, "chop",
              "<stride> <str> - make vector of strings each <stride> bytes "
              "or less",
              &fn_chop, 2);
    mod_addfn(cmds, "join",
              "<delim> <str> - join vector of octets and strings separated by "
              "<delim>s",
              &fn_join, 2);
    mod_addfn(cmds, "joinchr",
              "<delim> <str> - join vector of chars and strings separated by "
              "<delim>s",
              &fn_joinchr, 2);
    /*mod_add_dir(cmds, "root", *cmds);*/
}






/*****************************************************************************
 *                                                                           *
 *          Commands - IP Address                                            *
 *          =====================                                            *
 *                                                                           *
 *****************************************************************************/







static void
cmds_generic_ipaddr(parser_state_t *state, dir_t *cmds)
{   mod_add(cmds, "ip",
            "<IP addr> - IP address value",  &value_ipaddr_parse);
}









/*****************************************************************************
 *                                                                           *
 *          Commands - MAC Address                                           *
 *          =====================                                            *
 *                                                                           *
 *****************************************************************************/







static void
cmds_generic_macaddr(parser_state_t *state, dir_t *cmds)
{   mod_add(cmds, "mac",
            "<MAC addr> - MAC address value",  &value_macaddr_parse);
}









/*****************************************************************************
 *                                                                           *
 *          Commands - Directory                                             *
 *          ====================                                             *
 *                                                                           *
 *****************************************************************************/









/* make a copy of a value that is either a closure or a directory */
static const value_t *
dir_value_copy(const value_t *dir)
{   const value_t *val = NULL;
    const value_t *code = NULL;
    bool is_closure = value_type_equal(dir, type_closure);

    if (is_closure)
    {   const value_t *unbound;
        dir_t *env = NULL;
        value_closure_get(dir, &code, &env, &unbound);
        dir = dir_value(env);
    }

    if (value_type_equal(dir, type_dir))
    {   enum_new_arg_t newarg;

        newarg.dir_build = NULL;

        if (NULL == dir_forall((dir_t *)dir, &new_exec, &newarg))
        {   dir_t *newdir = newarg.dir_build;

            if (NULL == newdir)
                newdir = dir_id_new();

            val = dir_value(newdir);

            if (is_closure)
            {   /* create a copy of the closure */
                value_env_t *env = value_env_new();
                if (NULL != env)
                {   value_env_pushdir(env, newdir, /*end*/FALSE);
                    val = value_closure_new(code, env);
                    newdir = value_env_dir(env);
                }
            }
            if (dir_islocked((dir_t *)dir))
                (void)dir_lock(newdir, NULL);
        }
    }

    return val;
}



static const value_t *
fn_new(const value_t *this_fn, parser_state_t *state)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *val = dir_value_copy(dir);

    if (val == NULL)
        parser_report_help(state, this_fn);

    return value_nl(val);
}







static const value_t *
fn_dynenv(const value_t *this_fn, parser_state_t *state)
{
    const value_t *getall_fn = parser_builtin_arg(state, 1);
    const value_t *count_fn = parser_builtin_arg(state, 2);
    const value_t *set_fn = parser_builtin_arg(state, 3);
    const value_t *get_fn = parser_builtin_arg(state, 4);
    const value_t *val = &value_null;

    if (value_istype(getall_fn, type_closure) &&
        (count_fn == &value_null || value_istype(count_fn, type_closure)) &&
        (set_fn   == &value_null || value_istype(set_fn, type_closure)) &&
        (get_fn   == &value_null || value_istype(get_fn, type_closure)))
    {
        dir_t *newdir = dir_dyn_new(state, "dynenv - ",
                                    get_fn, set_fn, getall_fn, count_fn);
        if (newdir != NULL)
            val = dir_value(newdir);

    } else
        parser_report_help(state, this_fn);


    return val;
}




static const value_t *
fn_envjoin(const value_t *this_fn, parser_state_t *state)
{   const value_t *index_dirval = parser_builtin_arg(state, 1);
    const value_t *value_dirval = parser_builtin_arg(state, 2);
    dir_t *dir_ix = NULL;
    dir_t *dir_val = NULL;
    const value_t *val = &value_null;

    if (value_as_dir(index_dirval, &dir_ix) &&
        value_as_dir(value_dirval, &dir_val))
    {
        dir_t *newdir = dir_join_new(dir_ix, dir_val);
        if (newdir != NULL)
            val = dir_value(newdir);

    } else
        parser_report_help(state, this_fn);


    return val;
}




static const value_t *
fn_inenv(const value_t *this_fn, parser_state_t *state)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *name = parser_builtin_arg(state, 2);
    const value_t *val = NULL;
    dir_t *env;

    if (value_as_dir(dir, &env))
    {   const value_t *lookedup = dir_get(env, name);
        val = lookedup == NULL? value_false: value_true;
    } else
        val = value_false;

    return value_nl(val);
}






static const value_t *
fn_lock(const value_t *this_fn, parser_state_t *state)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    dir_t *env;

    if (value_as_dir(dir, &env))
        (void)dir_lock(env, NULL); /* stop new fields being added */
    else
        parser_report_help(state, this_fn);

    return val;
}






/*! Push a new envronment on the stack that will be used when new variables
 *  are defined.  Make previous environments "invisible" if \c env_end is
 *  FALSE
 */
static const value_t *
parser_enter_dir(const value_t *this_fn, parser_state_t *state,
                 bool outer_visible)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    dir_t *env;

    if (value_as_dir(dir, &env))
    {   dir_stack_pos_t pos = parser_env_calling_pos(state);

        /* insert the directory above our return position so that it is
           still there when we return */
        parser_env_push_at_pos(state, pos, env, outer_visible);
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_enter(const value_t *this_fn, parser_state_t *state)
{   return parser_enter_dir(this_fn, state, /*outer_visible*/FALSE);
}






static const value_t *
fn_restrict(const value_t *this_fn, parser_state_t *state)
{   return parser_enter_dir(this_fn, state, /*outer_visible*/TRUE);
}





#if 0
/* won't work because calling environment remembers the return position
   independentely of the state
 */
static const value_t *
fn_leave(const value_t *this_fn, parser_state_t *state)
{   dir_stack_pos_t calling = parser_env_calling_pos(state);
    dir_stack_pos_t outer = dir_stack_pos_enclosing(parser_env_stack(state),
                                                    dir_at_stack_pos(calling));

    if (NULL == outer)
        parser_error(state, "can't leave last environment\n");
    else
        /* we lose "leave"'s invocation environment ... when we return from
           "leave" this means that the normal return will return from an
           extra level of the stack */
        parser_env_return(state, calling);

    return &value_null;
}
#endif






typedef struct {
    dir_t *vec;                 /* vector to add next value to */
    int index;                  /* next index to use */
} for_vecgen_arg_t;




static void *
domain_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   for_vecgen_arg_t *vecgen_arg = (for_vecgen_arg_t *)arg;
    if (value != NULL)
        dir_int_set(vecgen_arg->vec, vecgen_arg->index++, name);
    return NULL;
}



static void *
range_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   for_vecgen_arg_t *vecgen_arg = (for_vecgen_arg_t *)arg;
    if (value != NULL)
        dir_int_set(vecgen_arg->vec, vecgen_arg->index++, value);
    return NULL;
}






static const value_t *
genfn_vecgen(const value_t *this_fn, parser_state_t *state,
             dir_enum_fn_t *genfn)
{   const value_t *dirval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    dir_t *dir;

    if (value_as_dir(dirval, &dir))
    {   for_vecgen_arg_t vecgen_arg;
        dir_t *vec = dir_vec_new();

        vecgen_arg.vec = vec;
        vecgen_arg.index = 0;
        (void)dir_forall(dir, genfn, (void *)&vecgen_arg);
        val = dir_value(vec);
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_domain(const value_t *this_fn, parser_state_t *state)
{   return genfn_vecgen(this_fn, state, &domain_exec);
}



static const value_t *
fn_range(const value_t *this_fn, parser_state_t *state)
{   return genfn_vecgen(this_fn, state, &range_exec);
}





typedef struct
{   parser_state_t *state;
    const value_t *cmpfn;
    dir_t *vals;
} sort_env_t;




/* NB: not thread safe */
static sort_env_t *sort_global_args = NULL;




static int
fn_sort_domcmp(const value_t **ref_v1, const value_t **ref_v2)
{   OMIT(
        if (sort_global_args == NULL)
        {   printf("%s: error - sort args empty\n", codeid());
            return 0;
        } else
        if (*ref_v1 == NULL)
        {   printf("%s: error - first sort arg empty\n", codeid());
            return (*ref_v2 == NULL)? 0: -1;
        } else
        if (*ref_v2 == NULL)
        {   printf("%s: error - second sort arg empty\n", codeid());
            return 1;
        } else
    )
    return value_cmp(*ref_v1, *ref_v2);
}




static int
fn_sort_rngcmp(const value_t **ref_v1, const value_t **ref_v2)
{   OMIT(
        if (sort_global_args == NULL)
        {   printf("%s: error - sort args empty\n", codeid());
            return 0;
        } else
        if (*ref_v1 == NULL)
        {   printf("%s: error - first sort arg empty\n", codeid());
            return (*ref_v2 == NULL)? 0: -1;
        } else
        if (*ref_v2 == NULL)
        {   printf("%s: error - second sort arg empty\n", codeid());
            return 1;
        } else
    )
    {   dir_t *vals = sort_global_args->vals;
        const value_t *v1 = dir_get(vals, *ref_v1);
        const value_t *v2 = dir_get(vals, *ref_v2);
        return value_cmp(v1, v2);
    }
}




static int
fn_sort_domcmpwith(const value_t **ref_v1, const value_t **ref_v2)
{   OMIT(
        if (sort_global_args == NULL)
        {   printf("%s: error - sort args empty\n", codeid());
            return 0;
        } else
        if (*ref_v1 == NULL)
        {   printf("%s: error - first sort arg empty\n", codeid());
            return (*ref_v2 == NULL)? 0: -1;
        } else
        if (*ref_v2 == NULL)
        {   printf("%s: error - second sort arg empty\n", codeid());
            return 1;
        } else
    )
    {   parser_state_t *state = sort_global_args->state;
        const value_t *code = sort_global_args->cmpfn;
        const value_t *ret;
        code = substitute(code, *ref_v1, state, /*unstrict*/TRUE);
        code = substitute(code, *ref_v2, state, /*unstrict*/TRUE);
        ret = invoke(code, state);

        return (int)value_int_number(ret);
    }
}




static int
fn_sort_rngcmpwith(const value_t **ref_v1, const value_t **ref_v2)
{   OMIT(
        if (sort_global_args == NULL)
        {   printf("%s: error - sort args empty\n", codeid());
             return 0;
        } else
        if (*ref_v1 == NULL)
        {   printf("%s: error - first sort arg empty\n", codeid());
            return (*ref_v2 == NULL)? 0: -1;
        } else
        if (*ref_v2 == NULL)
        {   printf("%s: error - second sort arg empty\n", codeid());
            return 1;
        } else
    )
    {   parser_state_t *state = sort_global_args->state;
        const value_t *code = sort_global_args->cmpfn;
        dir_t *vals = sort_global_args->vals;
        const value_t *v1 = dir_get(vals, *ref_v1);
        const value_t *v2 = dir_get(vals, *ref_v2);
        const value_t *ret;
        code = substitute(code, v1, state, /*unstrict*/TRUE);
        code = substitute(code, v2, state, /*unstrict*/TRUE);
        ret = invoke(code, state);

        return (int)value_int_number(ret);
    }
}




static const value_t *
fn_gensort(const value_t *this_fn, parser_state_t *state,
           compareval_fn_t *compare,
           const value_t *cmpfn, const value_t *dirval)
{   const value_t *val = &value_null;
    dir_t *dir;

    if (value_as_dir(dirval, &dir) &&
        (cmpfn == NULL || value_istype(cmpfn, type_closure)))
    {   for_vecgen_arg_t vecgen_arg;
        dir_t *vec = dir_vec_new();
        sort_env_t sort_args;
        sort_env_t *old_sort_args = sort_global_args;

        vecgen_arg.vec = vec;
        vecgen_arg.index = 0;
        (void)dir_forall(dir, &domain_exec, (void *)&vecgen_arg);
        sort_args.state = state;
        sort_args.cmpfn = cmpfn;
        sort_args.vals = dir;
        sort_global_args = &sort_args;
        dir_vec_sort((dir_vec_t *)vec, compare);
        sort_global_args = old_sort_args;
        val = dir_value(vec);
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_sortdomainby(const value_t *this_fn, parser_state_t *state)
{   return fn_gensort(this_fn, state, &fn_sort_domcmpwith,
                      /*fn*/  parser_builtin_arg(state, 1),
                      /*dir*/ parser_builtin_arg(state, 2));
}

static const value_t *
fn_sortby(const value_t *this_fn, parser_state_t *state)
{   return fn_gensort(this_fn, state, &fn_sort_rngcmpwith,
                      /*fn*/  parser_builtin_arg(state, 1),
                      /*dir*/ parser_builtin_arg(state, 2));
}


static const value_t *
fn_sortdomain(const value_t *this_fn, parser_state_t *state)
{   return fn_gensort(this_fn, state, &fn_sort_domcmp,
                      /*fn*/  NULL,
                      /*dir*/ parser_builtin_arg(state, 1));
}

static const value_t *
fn_sort(const value_t *this_fn, parser_state_t *state)
{   return fn_gensort(this_fn, state, &fn_sort_rngcmp,
                      /*fn*/  NULL,
                      /*dir*/ parser_builtin_arg(state, 1));
}






typedef struct
{   parser_state_t *state;
    const value_t *code;
    dir_t *dir_build;
} select_exec_args_t;





/* This function may cause a garbage collection */
static void *
select_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   select_exec_args_t *selectval = (select_exec_args_t *)arg;
    dir_t *codeargs = parser_env(selectval->state);

    if (NULL != codeargs && value != NULL)
    {   const value_t *code = selectval->code;
        const value_t *include;

        if (value_type_equal(code, type_closure))
        {   code = substitute(code, value, selectval->state, /*unstrict*/TRUE);
            if (NULL != code && value != NULL)
                code = substitute(code, name, selectval->state,
                                  /*unstrict*/TRUE);
        }
        include = invoke(code, selectval->state);

        if (!value_bool_is_false(include))
        {   dir_t *dir_new = selectval->dir_build;

            if (NULL == dir_new)
            {   if (value_type_equal(name, type_int))
                    dir_new = dir_vec_new();
                else
                    dir_new = dir_id_new();

                selectval->dir_build = dir_new;
            }
            dir_set(dir_new, name, value);
        }
    }

    return NULL; /* non-null would stop the enumeration here */
}





static const value_t *
fn_select(const value_t *this_fn, parser_state_t *state)
{   const value_t *code = parser_builtin_arg(state, 1);
    const value_t *dir = parser_builtin_arg(state, 2);
    dir_t *env;
    const value_t *val;
    bool is_code = value_type_equal(code, type_code);

    if (value_as_dir(dir, &env) && NULL != code &&
        (is_code || value_istype(code, type_closure)))
    {   select_exec_args_t selectval;
        dir_t *argdir = dir_id_new();
        dir_stack_pos_t pos;

        parser_env_return(state, parser_env_calling_pos(state));
        selectval.state = state;
        selectval.code = code;
        selectval.dir_build = NULL;
        OMIT(VALUE_SHOW("select beselecte environment: ",
                          dir_value(parser_env(state)));)
        pos = parser_env_push(state, argdir, /*outer_visible*/FALSE);
        OMIT(VALUE_SHOW("select environment: ",
                          dir_value(parser_env(state)));)
        (void)dir_forall(env, &select_exec, &selectval);
        (void)parser_env_return(state, pos);
        val = dir_value(selectval.dir_build);
    } else
    {   parser_report_help(state, this_fn);
        val = &value_null;
    }
    return val;
}






static const value_t *
fn_intseq(const value_t *this_fn, parser_state_t *state)
{   const value_t *val = &value_null;
    const value_t *firstval = parser_builtin_arg(state, 1);
    const value_t *incval   = parser_builtin_arg(state, 2);
    const value_t *lastval  = parser_builtin_arg(state, 3);
    bool got_firstval = !value_type_equal(firstval, type_null);
    bool got_incval   = !value_type_equal(incval, type_null);
    bool got_lastval  = !value_type_equal(lastval, type_null);

    if ((!got_firstval || value_istype(firstval, type_int)) &&
        (!got_incval   || value_istype(incval, type_int)) &&
        (!got_lastval  || value_istype(lastval, type_int)))
    {   dir_t *dir;
        number_t first = 1;
        number_t inc = 1;
        number_t last = 0x7ffffff; /* not infinity, but big */

        if (got_firstval)
            first = value_int_number(firstval);

        if (got_incval)
            inc = value_int_number(incval);

        if (got_lastval)
        {   last = value_int_number(lastval);
            if (!got_incval)
            {   if (first > last)
                    inc = -1;
                if (first == last)
                    inc = 0;
            }
        }
        else if (inc == 0)
            last = first;
        else if (inc < 0)
            last = -last;

        dir = dir_series_new(first, inc, last);
        if (NULL != dir)
            val = dir_value(dir);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_rndseq(const value_t *this_fn, parser_state_t *state)
{   const value_t *val = &value_null;
    const value_t *valsval = parser_builtin_arg(state, 1);

    if (value_istype(valsval, type_int))
    {   number_t vals = value_int_number(valsval);
        dir_t *vec = dir_vec_new();
        int i;

        for (i = 0; i < vals; i++) {
            /* get a number less than i */
            unsigned j = (int)(((float)i)*rand()/(RAND_MAX+1.0));
            /* swap this location with the value we would have put here */
            dir_int_set(vec, i, dir_int_get(vec, j));
            dir_int_set(vec, j, value_int_new(i));
        }

        val = dir_value(vec);
    } else
        parser_report_help(state, this_fn);

    return val;
}





typedef struct
{   parser_state_t *state;
    size_t index;
    size_t len;
    const value_t *vals;
    dir_t *dir_build;
} zip_exec_args_t;



/* This function may cause a garbage collection */
static void *
zip_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   zip_exec_args_t *zipval = (zip_exec_args_t *)arg;
    const value_t *zipped = zipval->vals;
    dir_t *dir_new = zipval->dir_build;
    const value_t *cont = NULL;

    if (zipval->len > 0)
    {   /* vals is a dir */
        dir_t *dir_vals = (dir_t *)zipped;
        zipped = dir_int_get(dir_vals, (int)zipval->index);
        if (zipped == NULL) {
            /*printf("%s: index %d gave nothing\n", codeid(), zipval->index);*/
            cont = name;
        }
        if (zipval->index+1 >= zipval->len)
            zipval->index = 0;
        else
            zipval->index++;
    } /* else the 'vals' argument is the one that is to be replicated */

    if (zipped != NULL && value != NULL &&
        !value_type_equal(value, type_null)) {
        if (NULL == dir_new)
        {   if (value_type_equal(value, type_int))
                dir_new = dir_vec_new();
            else
                dir_new = dir_id_new();

            zipval->dir_build = dir_new;
        }
        dir_set(dir_new, value, zipped);
    }
    return (void *)cont; /* non-null would stop the enumeration here */
}





static const value_t *
fn_zip(const value_t *this_fn, parser_state_t *state)
{   const value_t *val = &value_null;
    const value_t *domain = parser_builtin_arg(state, 1);
    const value_t *range = parser_builtin_arg(state, 2);
    dir_t *dir_domain = NULL;
    bool ok = TRUE;

    if (value_as_dir(domain, &dir_domain))
    {   zip_exec_args_t zipval;
        bool range_is_dir = value_type_equal(range, type_dir);

        zipval.state = state;
        zipval.index = 0;
        zipval.vals = range;
        zipval.dir_build = NULL;

        if (range_is_dir) {
            dir_t *dir_range = (dir_t *)range;
            zipval.len = dir_count(dir_range);
            if (zipval.len < 1)
                ok = FALSE;
        } else
            zipval.len = 0;

        if (ok) {
            value_t *lastdomval = dir_forall(dir_domain, &zip_exec, &zipval);
            if (lastdomval != NULL) {
                ok = FALSE;
            }
            val = dir_value(zipval.dir_build);
        }
    } else
        ok = FALSE;

    if (!ok)
    {   parser_report_help(state, this_fn);
    }

    return val;
}






static void
cmds_generic_dir(parser_state_t *state, dir_t *cmds)
{   dir_stack_t *scope = parser_env_stack(state);

    mod_addfnscope(cmds, "new",
              "<env> - copy all <env> values",
              &fn_new, 1, scope);
    mod_addfnscope(cmds, "lock",
              "<env> - prevent new names being added to <env>",
              &fn_lock, 1, scope);
    mod_addfn(cmds, "inenv",
              "<env> <name> - returns TRUE unless string <name> is in <env>",
              &fn_inenv, 2);
    mod_addfnscope(cmds, "enter",
              "<env> - add commands from <env> to current environment",
              &fn_enter, 1, scope);
    mod_addfnscope(cmds, "restrict",
              "<env> - restrict further commands to those in <env>",
              &fn_restrict, 1, scope);
#if 0
    mod_addfn(cmds, "leave",
              "- exit the environment last entered or restricted",
              &fn_leave, 0);
#endif
    mod_addfn(cmds, "domain",
              "<env> - generate vector of names in <env>",
              &fn_domain, 1);
    mod_addfn(cmds, "range",
              "<env> - generate vector of values in <env>",
              &fn_range, 1);
    mod_addfn(cmds, "dynenv",
              "<getall> <count> <set> <get> - dynamic env built from given "
              "functions",
              &fn_dynenv, 4);
    mod_addfn(cmds, "envjoin",
              "<env1> <env2> - composed directory indexing <env2> "
              "with <env1> values",
              &fn_envjoin, 2);
    mod_addfn(cmds, "zip",
              "<dom> <range> - generate environment with given domain and "
              "range taken from <range>",
              &fn_zip, 2);
    mod_addfnscope(cmds, "sort",
              "<env> - sorted vector of values in <env>",
              &fn_sort, 1, scope);
    mod_addfnscope(cmds, "sortdom",
              "<env> - sorted vector of names in <env>",
              &fn_sortdomain, 1, scope);
    mod_addfnscope(cmds, "sortby",
              "<cmpfn> <env> - sorted vector of values in <env> using <cmpfn>",
              &fn_sortby, 2, scope);
    mod_addfnscope(cmds, "sortdomby",
              "<cmpfn> <env> - sorted vector of names in <env> using <cmpfn>",
              &fn_sortdomainby, 2, scope);
    mod_addfn(cmds, "select",
              "<binding> <env> - subset of <env> "
              "for which <binding> returns TRUE",
              &fn_select, 2);
    mod_addfn(cmds, "intseq",
              "<first> <inc> <last> - vector of integers incrementing by <inc>",
              &fn_intseq, 3);
    mod_addfn(cmds, "rndseq",
              "<n> - vector of integers containing 0..<n>-1 in a random order",
              &fn_rndseq, 1);
}




/*****************************************************************************
 *                                                                           *
 *          Commands - Language                                              *
 *          ===================                                              *
 *                                                                           *
 *****************************************************************************/






/* forward reference */
static void *
help_show(dir_t *dir, const value_t *name, const value_t *value, void *arg);





STATIC_INLINE void
indent(int n)
{   printf("%*s", n*4, "");
}





typedef struct {
    int indent_level;
    int max_indent_level;
    bool include_vars;
} help_show_args_t;





static void
help_show_val(const char *namestr, size_t namelen, const value_t *value,
              int ind, int max_ind, bool include_vars)
{   if (value_type_equal(value, type_dir) && dir_enumerable((dir_t *)value))
    {   dir_t *dir = (dir_t *)value;
        help_show_args_t help_args;

        indent(ind);
        if (max_ind >= 0 && ind >= max_ind)
            printf("%.*s help - show subcommands\n", (int)namelen, namestr);
        else
        {   printf("%.*s <subcommand> - commands:\n",
                   (int)namelen, namestr);
            help_args.max_indent_level = max_ind;
            help_args.indent_level = ind+1;
            help_args.include_vars = include_vars;
            (void)dir_forall(dir, &help_show, (void *)&help_args);
        }
    } else
    {   const char *help = parser_help_text(value);

        if (NULL != help)
        {   indent(ind);
            printf("%.*s %s\n", (int)namelen, namestr, help);
        } else if (include_vars)
        {   indent(ind);
            printf("%.*s - %s value\n", (int)namelen, namestr,
                   value_type_name(value));
        }
    }
}





static void *
help_show(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   help_show_args_t *help_arg = (help_show_args_t *)arg;
    int ind = help_arg->indent_level;
    int max_ind = help_arg->max_indent_level;
    bool include_vars = help_arg->include_vars;

    if (value_type_equal(name, type_string))
    {   const char *namestr;
        size_t namelen;
        if (value_string_get(name, &namestr, &namelen))
            help_show_val(namestr, namelen, value, ind, max_ind, include_vars);
    }

    return NULL;
}







static const value_t *
cmd_help(const char **ref_line, const value_t *this_cmd, parser_state_t *state)
{   const value_t *val = &value_null;
    const value_t *helpval = &value_null;
    bool include_vars = FALSE;
    const char *cmd;

    parse_space(ref_line);
    cmd = *ref_line;

    if (((include_vars = parse_key(ref_line, "all")) || TRUE) &&
        (parse_expr(ref_line, state, &helpval) || TRUE) &&
        parse_space(ref_line) && parse_empty(ref_line))
    {   if (helpval != &value_null)
        {   help_show_val(cmd, strlen(cmd), helpval, 0, include_vars?-1: 0,
                          include_vars);
        } else
        {   /* the current directory will be the help command's local */
            /* variables so use the outer (calling) directory         */
            dir_t *outer =
                dir_at_stack_pos(dir_stack_top_pos(parser_env_stack(state)));
            help_show_args_t help_arg;
            OMIT(
                printf(  "env    : %p\n", parser_env(state));
                printf(  "pos    : %p\n", parser_env_calling_pos(state));
                DIR_SHOW("current: ", parser_env(state));
                DIR_SHOW("outer:   ", outer);
            )
            help_arg.indent_level = 0;
            help_arg.max_indent_level = include_vars? -1: 0;
            help_arg.include_vars = include_vars;
            (void)dir_forall(outer, &help_show, (void *)&help_arg);
        }
    }
    return val;
}







static const value_t *
fn_exit(const value_t *this_cmd, parser_state_t *state)
{   /* the top directory will be the help command's local variables */
    linesource_t *lines = parser_linesource(state);
    while (linesource_popdel(lines))
        continue;
    return &value_null;
}







static const value_t *
cmd_echo(const char **ref_line, const value_t *this_cmd, parser_state_t *state)
{   printf("%s\n", *ref_line);
    fflush(stdout);
    *ref_line += strlen(*ref_line);
    return &value_null;
}






/* This function may cause a garbage collection */
static const value_t *
cmd_eval(const char **ref_line, const value_t *this_cmd, parser_state_t *state)
{   const value_t *val = NULL;

    if (!(parse_expr(ref_line, state, &val) && parse_space(ref_line)))
    {   parser_error(state, "failed to evaluate expression\n");
        val = &value_null;
    } else if (!parse_empty(ref_line))
    {   parser_error_longstring(state, *ref_line,
                                "trailing text in expression -");
        val = &value_null;
    } /* else success */
    return val;
}






/* This function may cause a garbage collection */
static const value_t *
cmd_set(const char **ref_line, const value_t *this_cmd, parser_state_t *state)
{   const value_t *val = NULL;
    dir_t *parent;
    const value_t *id;

    if (parse_lvalue(ref_line, state, parser_env(state), &parent, &id) &&
        parse_space(ref_line) &&
        parse_expr(ref_line, state, &val) &&
        parse_space(ref_line) && parse_empty(ref_line))
    {   if (!dir_set(parent, id, val))
        {   parser_error(state, "can't set a value for ");
            parser_value_print(state, id);
            fprintf(stderr, " here\n");
        }
        val = &value_null;
    } else
    {   parser_report_help(state, this_cmd);
        val = &value_null;
    }

    return val;
}







typedef struct
{   parser_state_t *state;
    const value_t *code;
} for_exec_args_t;





/* This function may cause a garbage collection */
static void *
forall_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   for_exec_args_t *forval = (for_exec_args_t *)arg;
    dir_t *codeargs = parser_env(forval->state);

    /* don't execute for values that are NULL - warning this may mean that
       the perceived domain of an environment is larger than the range
     */
    if (NULL != codeargs && value != NULL)
    {   const value_t *code = forval->code;

        if (value_type_equal(code, type_closure))
        {   code = substitute(code, value, forval->state, /*unstrict*/TRUE);
            if (NULL != code)
                code = substitute(code, name, forval->state, /*unstrict*/TRUE);
        }
        (void)invoke(code, forval->state);
    }

    return NULL; /* non-null would stop the enumeration here */
}




/*! execute 0, 1 or 2 argument function with names and values from dir
 */
static const value_t *
fn_forall(const value_t *this_fn, parser_state_t *state)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *code = parser_builtin_arg(state, 2);
    dir_t *env;
    const value_t *val;
    bool is_code = value_type_equal(code, type_code);

    if (value_as_dir(dir, &env) && NULL != code &&
        (is_code || value_istype(code, type_closure)))
    {   for_exec_args_t forval;
        dir_t *argdir = dir_id_new();
        dir_stack_pos_t pos;

        parser_env_return(state, parser_env_calling_pos(state));
        forval.state = state;
        forval.code = code;
        OMIT(VALUE_SHOW("for before environment: ",
                          dir_value(parser_env(state)));)
        pos = parser_env_push(state, argdir, /*outer_visible*/FALSE);
        OMIT(VALUE_SHOW("for environment: ", dir_value(parser_env(state)));)
        (void)dir_forall(env, &forall_exec, &forval);
        (void)parser_env_return(state, pos);
        val = &value_null;
    } else
    {   parser_report_help(state, this_fn);
        val = &value_null;
    }
    return val;
}






/* This function may cause a garbage collection */
static void *
for_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   for_exec_args_t *forval = (for_exec_args_t *)arg;
    dir_t *codeargs = parser_env(forval->state);

    /* don't execute for values that are NULL - warning this may mean that
       the perceived domain of an environment is larger than the range
     */
    if (NULL != codeargs && value != NULL)
    {   const value_t *code = forval->code;
        code = substitute(code, value, forval->state, /*unstrict*/FALSE);
        if (code != NULL)
            (void)invoke(code, forval->state);
    }

    return NULL; /* non-null would stop the enumeration here */
}





/*! execute 1-argument function with values from dir
 */
static const value_t *
fn_for(const value_t *this_fn, parser_state_t *state)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *code = parser_builtin_arg(state, 2);
    dir_t *env;
    const value_t *val = &value_null;

    if (value_as_dir(dir, &env) && NULL != code &&
        value_istype(code, type_closure))
    {
        int n = value_closure_argcount(code);

        if (n == 1) {
            for_exec_args_t forval;
            dir_t *argdir = dir_id_new();
            dir_stack_pos_t pos;

            parser_env_return(state, parser_env_calling_pos(state));
            forval.state = state;
            forval.code = code;
            OMIT(VALUE_SHOW("for before environment: ",
                              dir_value(parser_env(state)));)
            pos = parser_env_push(state, argdir, /*outer_visible*/FALSE);
            OMIT(VALUE_SHOW("for environment: ",
                              dir_value(parser_env(state)));)
            (void)dir_forall(env, &for_exec, &forval);
            (void)parser_env_return(state, pos);
        } else
        {
            parser_error(state,
                         "for - closure must have one unbound value (not %d)\n",
                         n);
        }
    } else
    {   parser_report_help(state, this_fn);
    }
    return val;
}






/* This function may cause a garbage collection */
static const value_t *
fn_if(const value_t *this_fn, parser_state_t *state)
{   const value_t *condval = parser_builtin_arg(state, 1);
    const value_t *then_code = parser_builtin_arg(state, 2);
    const value_t *else_code = parser_builtin_arg(state, 3);
    const value_t *val = &value_null;

    if (value_istype_bool(condval))
    {   const value_t *exec = (value_bool_is_false(condval)? else_code:
                                                             then_code);

        if (NULL != exec)
        {   /* return to the calling envrionment */
            OMIT(DIR_SHOW("Orig env: ", parser_env(state));)
            parser_env_return(state, parser_env_calling_pos(state));
            OMIT(DIR_SHOW("Invoke env: ", parser_env(state));)
            val = invoke(exec, state);
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}






/* This function may cause a garbage collection */
static const value_t *
fn_while(const value_t *this_fn, parser_state_t *state)
{   const value_t *cond_code = parser_builtin_arg(state, 1);
    const value_t *do_code = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype_invokable(cond_code) && value_istype_invokable(do_code))
    {   const value_t *cond;
        bool ok = TRUE;

        parser_env_return(state, parser_env_calling_pos(state));

        while (ok &&
               value_type_equal(cond = invoke(cond_code, state), type_bool) &&
               !value_bool_is_false(cond))
        {   val = invoke(do_code, state);
            ok = (val != NULL);
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}






/* This function may cause a garbage collection */
static const value_t *
fn_do(const value_t *this_fn, parser_state_t *state)
{   const value_t *do_code = parser_builtin_arg(state, 1);
    const value_t *cond_code = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (cond_code != NULL && do_code != NULL)
    {   const value_t *cond;

        parser_env_return(state, parser_env_calling_pos(state));

        do
        {   val = invoke(do_code, state);
        } while (val != NULL &&
                 value_type_equal(cond=invoke(cond_code, state), type_bool) &&
                 !value_bool_is_false(cond));
    } else
        parser_report_help(state, this_fn);

    return val;
}






static void
cmds_generic_lang(parser_state_t *state, dir_t *cmds)
{   dir_stack_t *scope = parser_env_stack(state);

    mod_add(cmds, "help", "[all] [<cmd>] - prints information about commands",
            &cmd_help);
    mod_add(cmds, "set",
            "<name> <expr> - set value in environment",  &cmd_set);
    mod_addfn(cmds, "exit", "- abandon all command inputs", &fn_exit, 0);
    mod_addfnscope(cmds, "for",
              "<env> <binding> - execute <binding> <val> for every <env> value",
              &fn_for, 2, scope);
    mod_addfnscope(cmds, "forall",
              "<env> <binding> - execute <binding> <val> <name> for all entries in <env>",
              &fn_forall, 2, scope);
    mod_addfnscope(cmds, "if",
              "<n> <then-code> <else-code> - execute <then-code> if "
                   "<n>!=FALSE",
                   &fn_if, 3, scope);
    mod_addfnscope(cmds, "while",
              "<test> <do> - while <test> evaluates non-FALSE execute <do>",
              &fn_while, 2, scope);
    mod_addfnscope(cmds, "do",
              "<do> <test> - execute <do> while <test> evaluates "
              "to non-FALSE",
              &fn_do, 2, scope);
    mod_addfnscope(cmds, "catch",
              "<excep> <do> - run <do> executing <excep> <ex> on an exception",
              &fn_catch, 2, scope);
    mod_addfnscope(cmds, "throw",
              "<value> - signal an exception with <value>, exit outer 'catch'",
              &fn_throw, 1, scope);
}






/*****************************************************************************
 *                                                                           *
 *          Commands - Registry                                              *
 *          ===================                                              *
 *                                                                           *
 *****************************************************************************/







#ifdef HAS_REGISTRY




static bool regedit_write_allowed = FALSE;





static const value_t *
fn_regval_allow_edit(const value_t *this_fn, parser_state_t *state)
{   const value_t *allowed = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    bool is_allowed = !value_bool_is_false(allowed);

    if (is_allowed)
        regedit_write_allowed = TRUE;
    else
        regedit_write_allowed = FALSE;

    parser_report(state, "new keys will %sallow writes to the registry\n",
                  regedit_write_allowed? "":"not ");

    return val;
}





static const value_t *
genfn_regval_key(const value_t *this_fn, parser_state_t *state, key_t root)
{   const value_t *keynameval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    const char *keyname;
    size_t keynamelen;

    if (value_string_get(keynameval, &keyname, &keynamelen))
    {   dir_t *regdir = dir_regkeyval_new(root, regedit_write_allowed,
                                          keyname, keynamelen);
        if (NULL != regdir)
            val = dir_value(regdir);
    } else
        parser_report_help(state, this_fn);

    return val;
}





static const value_t *
fn_regval_getconfig(const value_t *this_fn, parser_state_t *state)
{   return genfn_regval_key(this_fn, state, HKEY_CURRENT_CONFIG);
}


static const value_t *
fn_regval_getusers(const value_t *this_fn, parser_state_t *state)
{   return genfn_regval_key(this_fn, state, HKEY_USERS);
}

static const value_t *
fn_regval_getuser(const value_t *this_fn, parser_state_t *state)
{   return genfn_regval_key(this_fn, state, HKEY_CURRENT_USER);
}

static const value_t *
fn_regval_getmachine(const value_t *this_fn, parser_state_t *state)
{   return genfn_regval_key(this_fn, state, HKEY_LOCAL_MACHINE);
}

static const value_t *
fn_regval_getroot(const value_t *this_fn, parser_state_t *state)
{   return genfn_regval_key(this_fn, state, HKEY_CLASSES_ROOT);
}









static void
cmds_generic_registry(parser_state_t *state, dir_t *cmds)
{   dir_t *reg = dir_id_new();
    dir_t *reg_val = dir_id_new();
    dir_t *reg_keytype = dir_id_new();

#if 0
    dir_t *current_config = dir_regkeyval_new(HKEY_CURRENT_CONFIG,
                                              regedit_write_allowed,
                                               /*keyname*/"", /*len*/0);
    dir_t *users          = dir_regkeyval_new(HKEY_USERS,
                                              regedit_write_allowed,
                                              /*keyname*/"", /*len*/0);
    dir_t *current_user   = dir_regkeyval_new(HKEY_CURRENT_USER,
                                              regedit_write_allowed,
                                              /*keyname*/"", /*len*/0);
    dir_t *local_machine  = dir_regkeyval_new(HKEY_LOCAL_MACHINE,
                                              regedit_write_allowed,
                                              /*keyname*/"", /*len*/0);
    dir_t *classes_root   = dir_regkeyval_new(HKEY_CLASSES_ROOT,
                                              regedit_write_allowed,
                                              /*keyname*/"", /*len*/0);

    mod_add_dir(reg, "root", classes_root);
    mod_add_dir(reg, "users", users);
    mod_add_dir(reg, "machine", local_machine);
    mod_add_dir(reg, "user", current_user);
    mod_add_dir(reg, "config", current_config);
#endif

    mod_addfn(reg_val, "HKEY_CLASSES_ROOT",
              "<key> - open key values in HKEY_CLASSES_ROOT",
              &fn_regval_getroot, 1);
    mod_addfn(reg_val, "HKEY_USERS",
              "<key> - open key values in HKEY_USERS",
              &fn_regval_getusers, 1);
    mod_addfn(reg_val, "HKEY_CURRENT_USER",
              "<key> - open key values in HKEY_CURRENT_USER",
              &fn_regval_getuser, 1);
    mod_addfn(reg_val, "HKEY_LOCAL_MACHINE",
              "<key> - open key values in HKEY_LOCAL_MACHINE",
              &fn_regval_getmachine, 1);
    mod_addfn(reg_val, "HKEY_CURRENT_CONFIG",
              "<key> - open key values in HKEY_CURRENT_CONFIG",
              &fn_regval_getconfig, 1);

    mod_add_dir(reg, "value", reg_val);
    mod_addfn(reg, "allow_edit",
              "<boolean> - enable/disable write to the registry from new keys",
              &fn_regval_allow_edit, 1);

    mod_add_val(reg_keytype, "REG_BINARY", value_int_new(REG_BINARY));
    mod_add_val(reg_keytype, "REG_DWORD", value_int_new(REG_DWORD));
    mod_add_val(reg_keytype, "REG_DWORD_LITTLE_ENDIAN",
                value_int_new(REG_DWORD_LITTLE_ENDIAN));
    mod_add_val(reg_keytype, "REG_DWORD_BIG_ENDIAN",
                value_int_new(REG_DWORD_BIG_ENDIAN));
    mod_add_val(reg_keytype, "REG_EXPAND_SZ", value_int_new(REG_EXPAND_SZ));
    mod_add_val(reg_keytype, "REG_LINK", value_int_new(REG_LINK));
    mod_add_val(reg_keytype, "REG_MULTI_SZ", value_int_new(REG_MULTI_SZ));
    mod_add_val(reg_keytype, "REG_NONE", value_int_new(REG_NONE));
    mod_add_val(reg_keytype, "REG_QWORD", value_int_new(REG_QWORD));
    mod_add_val(reg_keytype, "REG_QWORD_LITTLE_ENDIAN",
                value_int_new(REG_QWORD_LITTLE_ENDIAN));
    mod_add_val(reg_keytype, "REG_SZ", value_int_new(REG_SZ));

    mod_add_dir(reg, "key", reg_keytype);
    mod_add_dir(cmds, "reg", reg);
}




#else


#define cmds_generic_registry(state, cmds)


#endif









/*****************************************************************************
 *                                                                           *
 *          Commands - Memory                                                *
 *          ==================                                               *
 *                                                                           *
 *****************************************************************************/







static bool value_mem_cast(const value_t *val, const value_mem_t **out_mem)
{
    bool ok = false;
    if (value_type_equal(val, type_null))
        fprintf(stderr, "%s: memory object is NULL\n", codeid());
    else
        ok = value_istype(val, type_mem);

    if (ok)
        *out_mem = (const value_mem_t *)(val);
    else
        *out_mem = NULL;
    return ok;
}





static const value_t *
fn_mem_write(const value_t *this_fn, parser_state_t *state)
{   const value_t *val = value_false;
    const value_t *memval = parser_builtin_arg(state, 1);
    const value_t *ixval = parser_builtin_arg(state, 2);
    const value_t *strval = parser_builtin_arg(state, 3);
    const value_mem_t *mem;
    const char *binstr;
    size_t binstrlen;

    if (value_mem_cast(memval, &mem) &&
        value_istype(ixval, type_int) &&
        value_string_get(strval, &binstr, &binstrlen))
    {
        if (mem->write != NULL) {
            size_t ix = (size_t)value_int_number(ixval);
            size_t writeable =
                (*mem->len_able)(mem, ix, /*unable*/FALSE, 1 << mem_can_write);

            if (writeable >= binstrlen) {
                bool ok = (*mem->write)(mem, ix, binstr, binstrlen);
                if (ok)
                    val = value_true;
            }
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
genfn_mem_read(const value_t *this_fn, parser_state_t *state,
               mem_attr_map_t abilities, bool force_volatile)

{   const value_t *val = &value_null;
    const value_t *memval = parser_builtin_arg(state, 1);
    const value_t *ixval = parser_builtin_arg(state, 2);
    const value_t *lenval = parser_builtin_arg(state, 3);
    const value_mem_t *mem;

    if (value_mem_cast(memval, &mem) &&
        value_istype(ixval, type_int) &&
        value_istype(lenval, type_int))
    {
        if (mem->read != NULL) {
            size_t ix = (size_t)value_int_number(ixval);
            size_t len = (size_t)value_int_number(lenval);
            size_t bytes_able =
                (*mem->len_able)(mem, ix, /*unable*/FALSE, abilities);

            OMIT(printf("%s: need %zd got %zd bytes with ability %X\n",
                          codeid(), len, bytes_able, abilities););
            if (len > 0 && bytes_able >= len) {
                char *str = NULL;
                value_t *strval = value_string_alloc_new(len, &str);
                if (strval != NULL) {
                    bool ok = (*mem->read)(mem, ix, str, len, force_volatile);

                    if (ok)
                        val = strval;
                    else
                        value_delete(&strval);
                }
            } else if (len == 0 && bytes_able > 0)
                val = value_string_empty;
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_mem_read(const value_t *this_fn, parser_state_t *state)
{   return genfn_mem_read(this_fn, state, 1 << mem_can_read,
                          /*force_volatile*/FALSE);
}




static const value_t *
fn_mem_get(const value_t *this_fn, parser_state_t *state)
{   return genfn_mem_read(this_fn, state, 1 << mem_can_get,
                          /*force_volatile*/TRUE);
}




static bool
parse_mem_attr(const char *attrstr, size_t len, mem_attr_map_t *out_abilities)
{   mem_attr_map_t abilities = 0;
    bool ok = TRUE;
    while (ok && len-- > 0) {
        switch (attrstr[0])
        {   case 'R': case 'r': abilities |= 1 << mem_can_read; break;
            case 'G': case 'g': abilities |= 1 << mem_can_get; break;
            case 'W': case 'w': abilities |= 1 << mem_can_write; break;
            case 'C': case 'c': abilities |= 1 << mem_can_cache; break;
            default: ok = FALSE; break;
        }
        attrstr++;
    }
    *out_abilities = abilities;
    return ok;
}




static const value_t *
genfn_mem_len_able(const value_t *this_fn, parser_state_t *state, bool unable)

{   const value_t *val = NULL;
    const value_t *memval = parser_builtin_arg(state, 1);
    const value_t *ability = parser_builtin_arg(state, 2);
    const value_t *ixval = parser_builtin_arg(state, 3);
    const value_mem_t *mem;
    const char *attrstr;
    size_t strlen;
    mem_attr_map_t abilities = 0;

    if (value_mem_cast(memval, &mem) &&
        value_istype(ixval, type_int) &&
        value_string_get(ability, &attrstr, &strlen) &&
        parse_mem_attr(attrstr, strlen, &abilities))
    {
        size_t ix = (size_t)value_int_number(ixval);
        size_t bytes_able =
            (*mem->len_able)(mem, ix, unable, abilities);
        val = value_int_new(bytes_able);
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_mem_len_can(const value_t *this_fn, parser_state_t *state)
{   return genfn_mem_len_able(this_fn, state, /*unable*/FALSE);
}




static const value_t *
fn_mem_len_cant(const value_t *this_fn, parser_state_t *state)
{   return genfn_mem_len_able(this_fn, state, /*unable*/TRUE);
}




static const value_t *
genfn_mem_base_able(const value_t *this_fn, parser_state_t *state, bool unable)

{   const value_t *val = value_false;
    const value_t *memval = parser_builtin_arg(state, 1);
    const value_t *ability = parser_builtin_arg(state, 2);
    const value_t *ixval = parser_builtin_arg(state, 3);
    const value_mem_t *mem;
    const char *attrstr;
    size_t attrlen;
    mem_attr_map_t abilities = 0;

    if (value_mem_cast(memval, &mem) &&
        value_istype(ixval, type_int) &&
        value_string_get(ability, &attrstr, &attrlen) &&
        parse_mem_attr(attrstr, attrlen, &abilities))
    {
        size_t ix = (size_t)value_int_number(ixval);
        size_t baseix =
            (*mem->base_able)(mem, ix, unable, abilities);
        val = value_int_new(baseix);

    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_mem_base_can(const value_t *this_fn, parser_state_t *state)
{   return genfn_mem_base_able(this_fn, state, /*able*/TRUE);
}




static const value_t *
fn_mem_base_cant(const value_t *this_fn, parser_state_t *state)
{   return genfn_mem_base_able(this_fn, state, /*able*/FALSE);
}



static const value_t *
genfn_mem_dump(const value_t *this_fn, parser_state_t *state,
               mem_attr_map_t abilities, bool force_volatile)
{   const value_t *val = &value_null;
    const value_t *withchval = parser_builtin_arg(state, 1);
    const value_t *szval = parser_builtin_arg(state, 2);
    const value_t *memval = parser_builtin_arg(state, 3);
    const value_t *ixval = parser_builtin_arg(state, 4);
    const value_t *lenval = parser_builtin_arg(state, 5);
    const value_mem_t *mem;

    if (value_istype(szval, type_int) &&
        value_mem_cast(memval, &mem) &&
        value_istype(ixval, type_int) &&
        value_istype(lenval, type_int))
    {
        if (mem->read != NULL) {
            unsigned sz_ln2 = (unsigned)value_int_number(szval);
            if (sz_ln2 > 3)
                parser_error(state, "ln2 entry size byte length must be "
                             "less than 4 - but is %u\n", sz_ln2);
            else {
                bool with_chars = !value_bool_is_false(withchval);
                size_t ix = (size_t)value_int_number(ixval);
                size_t len = (size_t)value_int_number(lenval);
                size_t bytes_able =
                    (*mem->len_able)(mem, ix, /*unable*/FALSE, abilities);

                OMIT(printf("%s: need %zd got %zd bytes with ability %X\n",
                              codeid(), len, bytes_able, abilities););
                if (len > 0 && bytes_able >= len) {
                    char *buf = malloc(len);
                    if (buf != NULL) {
                        bool ok = (*mem->read)(mem, ix, buf, len,
                                               force_volatile);

                        if (ok) {
                            int entries = (int)(len >> sz_ln2);
                            const char *addr_format = "%08X: ";
                            mem_dump(buf, (unsigned)ix, entries, (int)sz_ln2, addr_format,
                                     /* addr_unit_size */1, with_chars);
                        }

                        free(buf);
                    }
                }
            }
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_mem_dump(const value_t *this_fn, parser_state_t *state)
{   return genfn_mem_dump(this_fn, state, 1 << mem_can_read,
                          /*force_volatile*/FALSE);
}



static const value_t *
fn_mem_bin(const value_t *this_fn, parser_state_t *state)
{   const value_t *val = value_false;
    const value_t *baseval = parser_builtin_arg(state, 1);
    const value_t *binstrval = parser_builtin_arg(state, 2);

    if (value_istype(baseval, type_int) &&
        value_istype(binstrval, type_string))
    {
        number_t base = value_int_number(baseval);
        val = value_mem_bin_new(binstrval, base,
                                /*readonly*/TRUE, /*sole_user*/FALSE);
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_mem_block(const value_t *this_fn, parser_state_t *state)
{   const value_t *val = value_false;
    const value_t *baseval = parser_builtin_arg(state, 1);
    const value_t *lenval = parser_builtin_arg(state, 2);

    if (value_istype(baseval, type_int) &&
        value_istype(lenval, type_int))
    {
        number_t base = value_int_number(baseval);
        number_t len = value_int_number(lenval);
        char *block = NULL;
        val = value_mem_bin_alloc_new(base, (size_t)len, /*memset_val*/0, &block);
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_mem_rebase(const value_t *this_fn, parser_state_t *state)
{   const value_t *val = value_false;
    const value_t *memval = parser_builtin_arg(state, 1);
    const value_t *baseval = parser_builtin_arg(state, 2);
    const value_mem_t *mem = NULL;

    if (value_mem_cast(memval, &mem) &&
        value_istype(baseval, type_int))
    {
        number_t base = value_int_number(baseval);
        val = value_mem_rebase_new(value_mem_value(mem), base,
                                   /*readonly*/FALSE, /*sole_user*/FALSE);
    } else
        parser_report_help(state, this_fn);

    return val;
}




static void
cmds_generic_mem(parser_state_t *state, dir_t *cmds)
{
    dir_t *mem = dir_id_new();

    mod_add_dir(cmds, "mem", mem);
    mod_addfn(mem, "write",
              "<mem> <ix> <str> - write binary string at index to memory",
              &fn_mem_write, 3);
    mod_addfn(mem, "read",
              "<mem> <ix> <len> - read <len> string at <ix> in memory",
              &fn_mem_read, 3);
    mod_addfn(mem, "get",
              "<mem> <ix> <len> - force read <len> string at <ix> in memory",
              &fn_mem_get, 3);
    mod_addfn(mem, "len_can",
              "<mem> [rwgc] <ix> - return length of area at <ix> that can do ops",
              &fn_mem_len_can, 3);
    mod_addfn(mem, "len_cant",
              "<mem> [rwgc] <ix> - return length of area at <ix> that can not do ops",
              &fn_mem_len_cant, 3);

    mod_addfn(mem, "base_can",
              "<mem> [rwgc] <ix> - return start of area ending at <ix> that can do ops",
              &fn_mem_base_can, 3);
    mod_addfn(mem, "base_cant",
              "<mem> [rwgc] <ix> - return start of area ending at <ix> that can do ops",
              &fn_mem_base_cant, 3);
    mod_addfn(mem, "bin",
              "<base> <string> - create mem with base index and read-only string",
              &fn_mem_bin, 2);
    mod_addfn(mem, "block",
              "<base> <len> - create block of mem with len bytes and base index",
              &fn_mem_block, 2);
    mod_addfn(mem, "rebase",
              "<mem> <base> - place <mem> at byte index <base>",
              &fn_mem_rebase, 2);
    mod_addfn(mem, "dump",
              "<+char?> <ln2entryb> <mem> <ix> <len> - dump content of memory",
              &fn_mem_dump, 5);
#if 0
    mod_addfn(mem, "seg",
              "<mem> <orig> <ix> <len> - create mem with start entries <orig> from memory)",
              &fn_mem_seg, 4);
    mod_addfn(mem, "memobj",
              "<fns> - create mem from fns read,write,get,len_attr,base_attr",
              &fn_memobj, 1);
#endif
}






/*****************************************************************************
 *                                                                           *
 *          Commands - Misc                                                  *
 *          ===============                                                  *
 *                                                                           *
 *****************************************************************************/







/* This function may cause a garbage collection */
static const value_t *
cmd_every(const char **ref_line, const value_t *this_cmd,
          parser_state_t *state)
{   const value_t *val = NULL;
    number_t duration_ms;
    bool ok = parse_int_expr(ref_line, state, &duration_ms);

    if (ok && parse_space(ref_line))
    {   bool ok = TRUE;
        do {
            const char *phrase = *ref_line;
            const value_t *val;

            val = mod_exec_cmd(&phrase, state);

            if (val != NULL)
            {   if (!parse_empty(&phrase))
                {   parser_report(state,
                                  "(every) warning - trailing text '%s'\n",
                                  phrase);
                    ok = FALSE;
                }
            } else
            {   parser_error(state,"(every) unknown command '%s'\n", phrase);
                ok = FALSE;
            }

            val = &value_null;
            parser_collect(state);

            if (ok)
                (*parser_suspend_get(state))((unsigned long)duration_ms);
        } while (ok);
    } else
    {   parser_error(state,"failed to evaluate expression\n");
        val = &value_null;
    }
    return val;
}







static const value_t *
cmd_from_fn(const char **ref_line, const value_t *this_cmd,
            parser_state_t *state)
{   const value_cmd_t *cmd = NULL;
    const value_t *val = &value_null;

    if (value_type_equal(this_cmd, type_closure))
    {   dir_t *env;
        const value_t *unbound;
        const value_t *cmdval;
        value_closure_get(this_cmd, &cmdval, &env, &unbound);
        cmd = (const value_cmd_t *)cmdval;
    } else if (value_type_equal(this_cmd, type_cmd))
        cmd = (const value_cmd_t *)this_cmd;

    if (NULL != cmd)
    {   const value_t *fn = cmd->fn_exec;

        if (NULL != fn)
        {   size_t linelen = strlen(*ref_line);
            value_t *arg = value_string_new(*ref_line, linelen);
            const value_t *bind = substitute(fn, arg, state, /*unstrict*/FALSE);
            if (NULL != bind)
            {   val = invoke(bind, state);
                if (val != NULL)
                    *ref_line += linelen;
            }
        }
    }
    return val;
}







static const value_t *
fn_cmd(const value_t *this_fn, parser_state_t *state)
{   const value_t *fnval = parser_builtin_arg(state, 1);
    const value_t *helpval = parser_builtin_arg(state, 2);
    const value_t *val = &value_null;

    if (value_istype(fnval, type_closure))
    {   value_env_t *env = value_env_new();
        dir_t *helpenv = dir_id_new();
        char argname[16];
        value_t *closure = value_closure_new(value_cmd_new(&cmd_from_fn,
                                                           fnval,
                                                           /*help*/NULL),
                                             env);
        if (NULL != helpval && helpval != &value_null)
            if (value_istype(helpval, type_string))
                dir_cstring_set(helpenv, BUILTIN_HELP, helpval);
        (void)value_closure_pushdir(closure, helpenv, /*outer_visible*/FALSE);
        sprintf(&argname[0], "%s%d", BUILTIN_ARG, 1);
        /*argnext =*/(void)value_closure_pushunbound(closure, NULL,
                                   value_string_new_measured(argname));

        val = closure;
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_sleep(const value_t *this_fn, parser_state_t *state)
{   const value_t *durationval = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;

    if (value_istype(durationval, type_int))
    {   unumber_t duration_ms = (unumber_t)
                                value_int_number(durationval);
        (*parser_suspend_get(state))((unsigned long)duration_ms);
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_len(const value_t *this_fn, parser_state_t *state)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    dir_t *env;

    if (value_to_dir(dir, &env))
        val = value_int_new(dir_count(env));
    else
    if (value_type_equal(dir, type_string))
    {   size_t len = 0;
        const char *buf;
        (void)value_string_get(dir, &buf, &len);
        val = value_int_new(len);
    } else
        parser_report_help(state, this_fn);

    return val;
}





extern void
cmds_generic(parser_state_t *state, int argc, const char **argv)
{   dir_t *cmds = parser_root(state);
    dir_stack_t *scope = parser_env_stack(state);

    cmds_generic_lang(state, cmds);
    cmds_generic_exception(state, cmds);
    cmds_generic_sys(state, cmds);
    cmds_generic_registry(state, cmds);
    if (NULL != argv)
        cmds_generic_parser(state, cmds, argc, argv);

    cmds_generic_coroutine(state, cmds);
    cmds_generic_type(state, cmds);
    cmds_generic_stream(state, cmds);
    cmds_generic_nul(state, cmds);
    cmds_generic_int(state, cmds);
    cmds_generic_bool(state, cmds);
    cmds_generic_string(state, cmds);
    cmds_generic_ipaddr(state, cmds);
    cmds_generic_macaddr(state, cmds);
    cmds_generic_closure(state, cmds);
    cmds_generic_dir(state, cmds);
    cmds_generic_mem(state, cmds);

#ifdef USE_FTL_XML
    {
        const value_t *pcmdsval =
            dir_stringl_get(dir_stack_dir(state->env),
                            FTLDIR_PARSE, strlen(FTLDIR_PARSE));
        if (pcmdsval != NULL && value_type_equal(pcmdsval, type_dir))
        {
            dir_t *pcmds = (dir_t *)pcmdsval;
            cmds_xml(state, pcmds);
        }
    }
#endif

    mod_add(cmds,
            "every", "<n> <command> - repeat command every <n> ms",
            &cmd_every);
    mod_addfn(cmds, "cmd",
              "<function> <help> - create a command from a function",
              &fn_cmd, 2);

    mod_add(cmds, "echo",
            "<whole line> - prints the line out", &cmd_echo);
    mod_add(cmds,
            "eval", "<expr> - evaluate expression", &cmd_eval);
    mod_addfnscope(cmds, "sleep",
              "<n> - sleep for <n> milliseconds",  &fn_sleep, 1, scope);
    mod_addfnscope(cmds, "len",
              "[<env>|<closure>|<string>] - number of elements in object",
              &fn_len, 1, scope);
    /*mod_add_dir(cmds, "root", *cmds);*/
}






extern void
cmds_generic_end(parser_state_t *state)
{
#ifdef USE_FTL_XML
    cmds_xml_end(state);
#endif
}







/*****************************************************************************
 *                                                                           *
 *          Library                                                          *
 *          =======                                                          *
 *                                                                           *
 *****************************************************************************/




#define strof(x) #x
#define str(x) strof(x)





extern void
ftl_version(int *out_major, int *out_minor, int *out_debug)
{   *out_major = VERSION_MAJ;
    *out_minor = VERSION_MIN;
#ifdef NDEBUG
    *out_debug = 0;
#else
    *out_debug = 1;
#endif
}





extern void
ftl_init(void)
{   value_heap_init();
    values_type_init();
    values_null_init();
    values_int_init();
    values_ipaddr_init();
    values_macaddr_init();
    values_string_init();
    values_code_init();
    values_stream_init();
    values_string_argname_init();
    values_dir_init();
    values_closure_init();
    values_coroutine_init();
    values_cmd_init();
    values_func_init();
    values_bool_init();/* must follow values_func_init */
    values_mem_init();
    fprint_init();
    OMIT(printf("FTL_MB_LEN_MAX = %d MB_LEN_MAX = %d (%s)\n",
                  FTL_MB_LEN_MAX, MB_LEN_MAX, str(MB_LEN_MAX));)
}






extern void
ftl_end(void)
{   int heap_value = value_heap_nextversion();
    (void) heap_value;    /* don't mark ANY values with it */
    value_heap_collect(); /* should collect all allocated values */
}







/*
 * Local variables:
 *  c-basic-offset: 4
 *  c-indent-level: 4
 *  tab-width: 8
 * End:
 */
