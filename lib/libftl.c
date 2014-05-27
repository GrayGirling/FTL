/*
 * Copyright (c) 2005-2009, Solarflare Communications Inc.
 * Copyright (c) 2014, Broadcom Inc.
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
** date    2005/09
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

   (new class!)::instance to inherit features from class.
   
   More parsing functions to support the commands (e.g. generate ftl functions
   from the parse_* functions)

   Integration of parse functions into type definitions

   Generalization of expression parser to use parse functions in the
   environment stack - so we can stop usurping the denotations of string, int,
   etc. for those types instead of new ones

   Consider <env>.{code} syntactical sugar for <env> {code}! - so we can
   enforce type parsing (e.g. ip.{168.192.4.5})

   Support a register of types so we can extend the set of types in ftl (i.e.
   from ftl scripts)

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

   Support a stream read-line function
   
   Support socket-based streams

   Do something about the string representation of built-in commands and
   functions so that they have a chance of being interpreted as the same
   command when read from the string - even if the interpreter is running on a
   different machine or is of a different age.  One day we'll have to split
   this file into several parts - we need a mechanisms that will still work!

   Construct an event handling mechanism:
       throw <val>,
       catch {<code>} [val]:{catch code}
   where the catch code executes after the abandonment of <expr> in the event
   that it calls throw.  Run each interactive command line in a global catch
   environment that simply prints <val> as an error.

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

   Dynamic mapping of strings on to vectors. Dynamic mapping of subvectors.
   Dynamic origin shifting.

   File directory as an environment type.
   
   Windows registry as an environment type.

   Functions to support automatic generation of an XML representation of
   an environment.
   
   Support for the generation of environment types that map on to specified
   C data structures - perhaps given a specification of the datastructure
   itself in C.
   
   Support for generating a C-section for linking into the library from a
   specification of an available C interface (a specification which is a C
   header - perhaps with restricted syntax - would be nice).

*/





/*****************************************************************************
 *                                                                           *
 *          Headers                                                          *
 *          =======							     *
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
#include <limits.h> /* for MB_LEN_MAX */
#include <wctype.h>

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


#include "ftl.h"
#include "ftl_internal.h"





/*****************************************************************************
 *                                                                           *
 *          Configuration                                                    *
 *          =============						     *
 *                                                                           *
 *****************************************************************************/



/* #define USE_READLINE */
/* #define EXIT_ON_CTRL_C */

#define VERSION_MAJ 1
#define VERSION_MIN 16

/*#define LOCAL_GARBAGE*/ /* collect garbage only when it's marked non-local */

/*#define FTL_BOOL_ISINT*/

#define FTL_ERROR_TRAIL_LINES 4

#ifdef MB_LEN_MAX
#define FTL_MB_LEN_MAX MB_LEN_MAX
#endif

#ifdef _WIN32

#define OS_FILE_SEP "\\"
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#ifndef FTL_MB_LEN_MAX
/* can't use MB_CUR_MAX because it is not constant */
#define FTL_MB_LEN_MAX 10
#endif
#define HAS_REGISTRY

#else

#define OS_FILE_SEP "/"
#define HAS_UNSETENV
#define HAS_SETENV
#define HAS_OPENDIR

#endif /* _WIN32 */

#ifdef __sun__
#ifndef FTL_MB_LEN_MAX
/* can't use MB_CUR_MAX because it is not constant */
#define FTL_MB_LEN_MAX 10
#endif
#endif

#ifndef FTL_MB_LEN_MAX
/* let's hope this is a constant - if not define O/S specific defn above */
#define FTL_MB_LEN_MAX MB_CUR_MAX
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

#ifdef HAS_GETHOSTBYNAME
#ifdef _WIN32
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/socket.h>        /* for AF_INET */
#endif
#endif





/*****************************************************************************
 *                                                                           *
 *          Debugging                                                        *
 *          =========							     *
 *                                                                           *
 *****************************************************************************/



#ifdef IGNORE
#undef IGNORE
#endif

#ifdef DO
#undef DO
#endif

#define DO(_x) _x
#define IGNORE(_x) 

#define FORCEDEBUG

/* #define DEBUGGC DO */
/* #define DEBUGVALINIT DO */
/* #define DEBUGCONSOLE DO */
/* #define DEBUGEXPD DO */
/* #define DEBUGSERIES DO */
/* #define DEBUGTRACE DO */
/* #define DEBUGDIR DO */
/* #define DEBUGENV DO */
/* #define DEBUGMOD DO */
/* #define DEBUGSOCK DO */
/* #define DEBUGOP DO */
/* #define DEBUGRCFILE DO */
/* #define DEBUGEXECV DO */

#if defined(NDEBUG) && !defined(FORCEDEBUG)
#undef DEBUGGC
#undef DEBUGVALINIT
#undef DEBUGCONSOLE
#undef DEBUGEXPD
#undef DEBUGSERIES
#undef DEBUGTRACE
#undef DEBUGDIR
#undef DEBUGENV
#undef DEBUGMOD
#undef DEBUGSOCK
#undef DEBUGOP
#undef DEBUGEXECV
#undef DEBUGRCFILE
#endif



#ifndef DEBUGGC
#define DEBUGGC IGNORE
#endif
#ifndef DEBUGVALINIT
#define DEBUGVALINIT IGNORE
#endif
#ifndef DEBUGCONSOLE
#define DEBUGCONSOLE IGNORE
#endif
#ifndef DEBUGEXPD
#define DEBUGEXPD IGNORE
#endif
#ifndef DEBUGSERIES
#define DEBUGSERIES IGNORE
#endif
#ifndef DEBUGTRACE
#define DEBUGTRACE IGNORE
#endif
#ifndef DEBUGDIR
#define DEBUGDIR IGNORE
#endif
#ifndef DEBUGENV
#define DEBUGENV IGNORE
#endif
#ifndef DEBUGMOD
#define DEBUGMOD IGNORE
#endif
#ifndef DEBUGSOCK
#define DEBUGSOCK IGNORE
#endif
#ifndef DEBUGOP
#define DEBUGOP IGNORE
#endif
#ifndef DEBUGEXECV
#define DEBUGEXECV IGNORE
#endif
#ifndef DEBUGRCFILE
#define DEBUGRCFILE IGNORE
#endif

/* #define DPRINTF ci_log */
#define DPRINTF printf

#define CODEID_UC "FTL"
#define CODEID "ftl"
#define CODEIDNAME_MAX 64






/*****************************************************************************
 *                                                                           *
 *          O/S Dependencies				                     *
 *          ================			                             *
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

#define DEBUGUID IGNORE

#define BYTESWAP_32(v)    ((((v) & 0xff000000) >> 24)| 	\
			   (((v) & 0x00ff0000) >> 8) |	\
			   (((v) & 0x0000ff00) << 8) |	\
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
    IGNORE(printf("%s: lookup '%s' gives %p\n", codeid(), name, pwd);)
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
 *          Code ID				                             *
 *          =======			                                     *
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
 *          Character Sinks				                     *
 *          ===============			                             *
 *                                                                           *
 *****************************************************************************/









typedef bool /*full*/ putc_fn_t(charsink_t *sink, int ch);



struct charsink_s
{   putc_fn_t *putc;
} /* charsink_t */;




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
	wctomb(NULL, L'\0'); /* reset state */
        len = wctomb(&mbstring[0], ch);
#endif
	
	if (len >= 0 && len < sizeof(mbstring))
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
 *          Output Streams				                     *
 *          ==============			                             *
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
    if (stream != NULL)
    {   charsink_stream_close(*ref_sink);
	FTL_FREE(stream);
	*ref_sink = NULL;
    }
}









/*****************************************************************************
 *                                                                           *
 *          Output Sockets				                     *
 *          ==============			                             *
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
    if (socket != NULL)
    {   charsink_socket_close(*ref_sink);
	FTL_FREE(socket);
	*ref_sink = NULL;
    }
}









/*****************************************************************************
 *                                                                           *
 *          Output Containers				                     *
 *          =================			                             *
 *                                                                           *
 *****************************************************************************/





#define CHARSINK_STRING_N_INIT 8




typedef struct
{   charsink_t sink;
    char *charvec;
    size_t n;
    size_t maxn;
} charsink_string_t;








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

	    IGNORE(printf("extend string from %d to %d (index %d)\n",
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
	    IGNORE(printf("%d:'%c' len %d\n", index, ch, charbuf->n);)
	}
	IGNORE(else printf("Failed to add char index %d - max is %d\n",
		           index, charbuf->maxn);)
    }
    
    return ok;
}





static charsink_t *
charsink_string_init(charsink_string_t *charbuf)
{   charbuf->charvec = NULL;
    charbuf->n = 0;
    charbuf->maxn = 0;
    return charsink_init(&charbuf->sink, &charsink_string_putc);
}







static void
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
    {   IGNORE(printf("%s: no memory for a new string buffer (%d bytes)\n",
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
            IGNORE(else printf("Failed to add char index %d - max is %d\n",
                               index, charbuf->maxn);)
        } else
	{   charbuf->charvec[index] = ch;
	    if (index+1 > charbuf->n)
		charbuf->n = index+1;
	    IGNORE(printf("%d:'%c' len %d\n", index, ch, charbuf->n);)
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
    {   IGNORE(printf("%s: no memory for a new fixed string buffer "
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
 *          Character Sources						     *
 *          =================					             *
 *                                                                           *
 *****************************************************************************/







/* reads the next character from source and returns it as an
   unsigned char cast to an int, or EOF on end of file or error. */
typedef int charsource_rdch_fn_t(charsource_t *source);

typedef size_t charsource_read_fn_t(charsource_t *source,
				    void *buf, size_t len);

typedef void charsource_delete_fn_t(charsource_t *source);




struct charsource_s
{   struct charsource_s *link;
    charsource_rdch_fn_t *rdch;
    charsource_read_fn_t *read;
    charsource_delete_fn_t *delete;  /* delete the charsource entirely */
    charsource_delete_fn_t *close;   /* delete any accumulated state */
    const char *name;
    int lineno;
};




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



static void
charsource_init(charsource_t *source,
		charsource_rdch_fn_t *rdch, charsource_read_fn_t *read,
		charsource_delete_fn_t *delete,
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
    source->delete = delete;
    source->close = close;
    source->lineno = 1;
    
    if (namelen > 0)
    {   namebuf = FTL_MALLOC(namelen+1);
        source->name = namebuf;
	
	if (NULL != namebuf)
	{   va_start(args, name_format);
	    (void)vsnprintf(namebuf, namelen+1, name_format, args);
	    va_end(args);
            IGNORE(printf("%s: name '%s' -> [%d] '%s'\n",
                          codeid(), name_format, namelen, namebuf););
	}
    }
}





extern void
charsource_close(charsource_t *source)
{   if (NULL != source && NULL != source->close)
	(*source->close)(source);
}





extern void
charsource_delete(charsource_t **ref_source)
{   if (NULL != *ref_source)
    {   charsource_t *source = *ref_source;

	charsource_close(source);
	
	if (NULL != source->name)
	{   FTL_FREE((void *)source->name);
	    source->name = NULL;
	}
	
	if (NULL != source->delete)
	    (*source->delete)(source);
	
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
{   return source == NULL? 0:
	   source->lineno;
}





STATIC_INLINE int
charsource_getc_local(charsource_t *source)
{   if (source == NULL)
        return EOF;
    else
    {   int ch = (*source->rdch)(source);
	if (ch == '\n')
	    source->lineno++;
	return ch;
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
	const char *p = (const char *)buf;
	const char *endp = p+actual;
	while (p < endp)
	    if (*p++ == '\n')
	        source->lineno++;
	return (int)actual;
    }
}






/*****************************************************************************
 *                                                                           *
 *          File-based Character Sources				     *
 *          ============================			             *
 *                                                                           *
 *****************************************************************************/







typedef struct
{   charsource_t base;
    FILE *stream;
} charsource_file_t;




STATIC_INLINE int
charsource_file_rdch(charsource_t *base_source)
{   charsource_file_t *source = (charsource_file_t *)base_source;
    return getc(source->stream);
}




static size_t
charsource_file_read(charsource_t *base_source, void *buf, size_t len)
{   charsource_file_t *source = (charsource_file_t *)base_source;
    size_t bytes = fread(buf, 1, len, source->stream);
    /*printf("%s: read %d bytes from %d - EOF %s\n",
             codeid(), bytes, len,
             feof(source->stream)? "TRUE": "FALSE"); */
    return bytes;
}




static charsource_t *
charsource_stream_init(charsource_file_t *source,
		       charsource_delete_fn_t *delete,
		       charsource_rdch_fn_t *rdch, charsource_read_fn_t *read,
		       const char *name, FILE *stream)
{   charsource_init(&source->base, rdch, read, delete,
		    /*close*/NULL,  "<%s>", name);
    source->stream = stream;
    return &source->base;
}







static void charsource_file_close(charsource_t *base_source)
{   charsource_file_t *source = (charsource_file_t *)base_source;
    fclose(source->stream);
    source->stream = NULL;
}





static void charsource_file_delete(charsource_t *source)
{   if (NULL != source)
        FTL_FREE(source);
}





static charsource_t *
charsource_file_init(charsource_file_t *source, charsource_delete_fn_t *delete,
		     const char *name, FILE *stream)
{    IGNORE(printf("%s: new file '%s'\n", codeid(), name););
     charsource_init(&source->base,
		     &charsource_file_rdch, &charsource_file_read, delete,
		     &charsource_file_close, "%s", name);
     source->stream = stream;
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
            IGNORE(fprintf(stderr, "%s: open absolute '%s' %s\n",
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
	        IGNORE(fprintf(stderr, "%s: open '%s' %s\n",
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
    IGNORE (else printf("%s: couldn't open file on path '%s' - [%d]%s\n",
                        codeid(), path, namelen, name););
    
    return (charsource_t *)source;
}









/*****************************************************************************
 *                                                                           *
 *          Socket-based Character Sources				     *
 *          ==============================			             *
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
} charsource_socket_t;




STATIC_INLINE int
charsource_socket_rdch(charsource_t *base_source)
{   charsource_socket_t *source = (charsource_socket_t *)base_source;
    char ch;
    return recv(source->fd, &ch, 1, source->recv_flags);
}




static size_t
charsource_socket_read(charsource_t *base_source, void *buf, size_t len)
{   charsource_socket_t *source = (charsource_socket_t *)base_source;
    return recv(source->fd, buf, len, source->recv_flags);
}




static charsource_t *
charsource_skt_init(charsource_socket_t *source,
		    charsource_delete_fn_t *delete,
		    charsource_rdch_fn_t *rdch, charsource_read_fn_t *read,
		    const char *name, int fd, int recv_flags)
{   charsource_init(&source->base, rdch, read, delete,
		    /*close*/NULL,  "inet:%s", name);
    source->fd = fd;
    source->recv_flags = recv_flags;
    return &source->base;
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
		       charsource_delete_fn_t *delete,
		       const char *name, int fd, int recv_flags)
{    charsource_init(&source->base,
		     &charsource_socket_rdch, &charsource_socket_read, delete,
		     &charsource_socket_close, "inet:%s", name);
     source->fd = fd;
     source->recv_flags = recv_flags;
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
 *          Buffer-based Character Sources				     *
 *          ==============================			             *
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




/* initialize a string that may or may not be freed */
static charsource_t *
charsource_string_anyinit(charsource_string_t *source,
		          charsource_delete_fn_t *delete,
			  charsource_delete_fn_t *close,
		          const char *name, const char *string, size_t len)
{   charsource_init(&source->base,
		    &charsource_string_rdch, &charsource_string_read,
		    delete, close, "$%s", name);
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




#ifdef USE_READLINE

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
		       charsource_delete_fn_t *delete,
		       const char *name, const char *string, size_t len)
{   char *strcopy = NULL;

    if (NULL != string)
       strcopy = (char *)FTL_MALLOC(len+1);

    if (NULL != strcopy)
    {   memcpy(strcopy, string, len);
	strcopy[len] = '\0';
    } else
	len = 0;
    
    return charsource_string_anyinit(source, delete,
				     &charsource_string_close,
				     name, strcopy, len);
}





/* initialize with a constant string (not copied into malloc'd buffer */
STATIC_INLINE charsource_t *
charsource_cstring_init(charsource_string_t *source,
		        charsource_delete_fn_t *delete,
		        const char *name, const char *string, size_t len)
{   return charsource_string_anyinit(source, delete, /*close*/NULL,
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
 *          External String Reference Character Sources			     *
 *          ===========================================		             *
 *                                                                           *
 *****************************************************************************/





typedef struct
{   charsource_t base;
    const char **ref_string;  /* pointer to the string buffer pointer */
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
}





/* initialize a string that may or may not be freed */
STATIC_INLINE charsource_t *
charsource_lineref_init(charsource_lineref_t *source,
		        charsource_delete_fn_t *delete,
		        const char *name, int lineno, const char **ref_string)
{
    /*printf("%s: src name '%s'\n", codeid(), name);*/
    charsource_init(&source->base,
		    &charsource_lineref_rdch, /*read*/NULL,
		    delete,
		    &charsource_lineref_close, "%s@%d", name, lineno);
    source->ref_string = ref_string;
    source->base.lineno = 0; /* we can't keep track of this */
    return &source->base;
}





#if 0
static void
charsource_lineref_delete(charsource_t *source)
{   if (NULL != source)
        FTL_FREE(source);
}





extern charsource_t *
charsource_lineref_new(const char *name, int lineno, const char **ref_string)
{   charsource_lineref_t *source = NULL;

    if (NULL != ref_string)
    {   source = (charsource_lineref_t *)
		 FTL_MALLOC(sizeof(charsource_lineref_t));
	if (NULL != source)
	    charsource_lineref_init(source, &charsource_lineref_delete,
				    name, lineno, ref_string);
    }
    return (charsource_t *)source;
}
#endif









/*****************************************************************************
 *                                                                           *
 *          Single Character Character Sources				     *
 *          ==================================			             *
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
    





/* initialize a string that may or may not be freed */
static charsource_t *
charsource_ch_init(charsource_ch_t *source, charsource_delete_fn_t *delete,
		   int ch)
{   charsource_init(&source->base, &charsource_ch_rdch, /*read*/NULL,
		    delete, /*close*/NULL,
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














/*****************************************************************************
 *                                                                           *
 *          Prompting Character Sources				             *
 *          ===========================			                     *
 *                                                                           *
 *****************************************************************************/





#ifndef USE_READLINE







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
  			  charsource_delete_fn_t *delete,
			  FILE *consolein, FILE *consoleout, const char *prompt)
{    charsource_stream_init(&source->file_base, delete,
			    &charsource_prompting_rdch, /*read*/NULL,
			    "*console*",
			    consolein);
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



#endif /* USE_READLINE */





/*****************************************************************************
 *                                                                           *
 *          Readline History Files				             *
 *          ======================			                     *
 *                                                                           *
 *****************************************************************************/




#ifndef USE_READLINE_HISTORY
#ifdef USE_READLINE
#define USE_READLINE_HISTORY 1
#endif
#endif


#if USE_READLINE_HISTORY




#include "history.h"

#ifdef HAS_OPENDIR
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif




typedef struct
{   char *this_history;
    char *global_history;
    bool this_is_open;
    bool global_is_open;
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

    if (NULL != hist->this_history) 
    {
	/* Set up the actual name. */
	sprintf(hist->this_history, "%s%s"OS_FILE_SEP
		                    READLINE_HOME_DIR_PREFIX "%s" OS_FILE_SEP
		                    READLINE_HISTORY_NAME "-%s"
		                    READLINE_HISTORY_EXTENSION,
		homedir, ENV_FTL_HOMEDIR_HOME, history_home, history_instance);

	hist->this_is_open = (read_history(hist->this_history) != 0);
	
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
		    READLINE_HISTORY_NAME READLINE_HISTORY_EXTENSION,
		    homedir, ENV_FTL_HOMEDIR_HOME, history_home);
	}

	if (hist->this_is_open)
	    hist->global_is_open = (read_history(hist->global_history) != 0);
	else
	    hist->global_is_open = FALSE;

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
    {	if (hist->this_is_open) 
	    write_history(hist->this_history);
	else
	{   append_history(READLINE_HISTORY_MAXLEN, hist->this_history);
	    history_truncate_file(hist->this_history, READLINE_HISTORY_MAXLEN);
	}
	FTL_FREE(hist->this_history);
	hist->this_history = NULL;
	hist->this_is_open = FALSE;
    }
    
    if (hist->global_history != NULL)
    {	if (hist->global_is_open) 
	    write_history(hist->global_history);
	else 
	{   append_history(READLINE_HISTORY_MAXLEN, hist->global_history);
	    history_truncate_file(hist->global_history,
				  READLINE_HISTORY_MAXLEN);
	}
	FTL_FREE(hist->global_history);
	hist->global_history = NULL;
	hist->global_is_open = FALSE;
    }
}



#else/* USE_READLINE_HISTORY */


typedef void *history_file_t ;
#define history_open(histfile) (TRUE)
#define history_close(histfile)
#define add_history(line)


#endif /* USE_READLINE_HISTORY */







    
/*****************************************************************************
 *                                                                           *
 *          Readline Character Sources				             *
 *          ==========================			                     *
 *                                                                           *
 *****************************************************************************/






#ifdef USE_READLINE




#include "readline.h"





typedef struct
{   charsource_string_t string_base;
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
		
	    DEBUGCONSOLE(DPRINTF("calling readline\n");)
	    nextline = readline(source->prompt);
	    source->prompt_needed = FALSE;
	    if (nextline != NULL)
	    {   charsource_string_setstring(base_source, nextline,
					    strlen(nextline));
		DEBUGCONSOLE(DPRINTF("got '%s'\n", nextline);)
		ch = charsource_string_rdch(base_source);
		if (ch == EOF)
		{   ch='\n';
		    source->prompt_needed = TRUE;
		} else
		    add_history(nextline);
		DEBUGCONSOLE(DPRINTF("first ch: '%c'\n", ch);)
	    } else
	    {   charsource_string_close(base_source);
		ch = EOF;
	    }
	} else
	if (ch == EOF)
	{   ch='\n';
	    source->prompt_needed = TRUE;
	}
	DEBUGCONSOLE(else DPRINTF("next ch: '%c'\n", ch);)
	
        return ch;
    }
}







static charsource_t *
charsource_readline_init(charsource_readline_t *source,
			 charsource_delete_fn_t *delete, const char *prompt)
{    charsource_t *rl;
     source->prompt = prompt;
     source->prompt_needed = TRUE;
     rl = charsource_string_init(&source->string_base, delete, "*console*",
				 /* string */NULL, 0);
     source->string_base.base.rdch = &charsource_readline_rdch;
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
 *          Charsource Stacks						     *
 *          =================					             *
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
instack_print(instack_t stack, charsink_t *sink)
{   charsource_t *src = stack;
    int n = 0;
    if (NULL != src)
    {   /* print all but the outermost */
	while (NULL != src->link)
	{   int lineno = charsource_lineno(src);
	    if (NULL == sink)
	    {   n += fprintf(stderr, "%s %s:", codeid(), charsource_name(src));
		if (lineno > 0)
		    n += fprintf(stderr, "%d:", lineno);
		n += fprintf(stderr, " in\n");
	    } else
	    {   n += charsink_sprintf(sink, "%s %s:", codeid(),
				      charsource_name(src));
		if (lineno > 0)
		    n += charsink_sprintf(sink, "%d:", lineno);
		n += charsink_sprintf(sink, " in\n");
	    }
	    src = src->link;
	}
    }
    return n;
}








/*****************************************************************************
 *                                                                           *
 *          Command Line Source						     *
 *          ===================				                     *
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
code_place_eq(code_place_t *place, const char *source, int lineno)
{   return source != NULL && 0 == strcmp(&place->posname[0], source) &&
	   place->lineno == lineno;
}








struct linesource_s
{   instack_t in;		/* stack of charsources */
    char escape;                /* escape char - e.g. \ */
    char string;		/* string introduction char - e.g. " */
    char commandsep;		/* command separator char - e.g. ; */
    char comment;		/* comment introduction char - e.g. # */
    bool eof;			/* true when charsources exhausted */
    code_place_t place;		/* position at start of current line */
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




extern int
vreport_line(charsink_t *sink, linesource_t *source,
             const char *format, va_list ap)
{   if (NULL == source)
    {
        if (NULL == sink)
            return vfprintf(stderr, format, ap);
        else
            return charsink_vsprintf(sink, format, ap);
    } else {
        int n = 0;

        IGNORE(printf("%s: sink is %sset - source is '%s'\n",
                      codeid(),NULL==sink? "un":"",
                      linesource_source(source)););
        if (NULL == sink)
        {   n += fprintf(stderr, "%s %s:%d: ",
                         codeid(), linesource_source(source),
                         linesource_lineno(source));
            return n + vfprintf(stderr, format, ap);
        } else
        {   int n = 0;
            n += charsink_sprintf(sink, "%s %s:%d: ",
                                  codeid(), linesource_source(source),
                                  linesource_lineno(source));
            return n + charsink_vsprintf(sink, format, ap);
        }
    }
}





extern int
vreport(charsink_t *sink, linesource_t *source, const char *format, va_list ap)
{   int n = 0;
    
    if (source != NULL)
    {
        const char *at_source = instack_source(source->in);
        int at_lineno = instack_lineno(source->in);

        /* report rest of the stack if it is different to last time */
        if (!code_place_eq(&source->report_place, at_source, at_lineno))
        {   code_place_set(&source->report_place, at_source, at_lineno);
            n += instack_print(source->in, sink);
        }
    }
    return n+vreport_line(sink, source, format, ap);
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




/* Reads in the whole of the next line but stores only the first maxlen-1
   bytes of it in the line buffer provided.
   The buffer is always written with a final zero (if there is a byte spare)
*/
extern void
linesource_read(linesource_t *source, charsink_t *line)
{   if (!source->eof)
    {   int ch = EOF;
	bool gotch = FALSE;
	bool escaped = FALSE;
	bool instring = FALSE;
	int brackets = 0;

	/* remember the position at the start of this line */
	code_place_set(&source->place, instack_source(source->in),
		       instack_lineno(source->in));
	
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
 *          Types 					                     *
 *          =====					                     *
 *                                                                           *
 *****************************************************************************/






extern const char *
type_name(type_t kind)
{   switch (kind)
    {   case type_null:
	    return "nul";
	case type_type:
	    return "type";
	case type_string:
	    return "string";
	case type_code:
	    return "code";
	case type_closure:
	    return "closure";
	case type_int:
	    return "int";
	case type_dir:
	    return "dir";
	case type_cmd:
	    return "cmd";
	case type_func:
	    return "function";
	case type_stream:
	    return "stream";
	case type_ipaddr:
	    return "ipaddr";
	case type_macaddr:
	    return "macaddr";
	case type_coroutine:
	    return "coroutine";
	default:
	    return "<UnknownType>";
    }
}





extern bool
parse_type(const char **ref_line, type_t *out_type_id)
{   bool ok = TRUE;
    
    if (parse_key(ref_line, type_name(type_null)))
	*out_type_id = type_null;
    else if (parse_key(ref_line, type_name(type_type)))
	*out_type_id = type_type;
    else if (parse_key(ref_line, type_name(type_string)))
	*out_type_id = type_string;
    else if (parse_key(ref_line, type_name(type_code)))
	*out_type_id = type_code;
    else if (parse_key(ref_line, type_name(type_closure)))
	*out_type_id = type_closure;
    else if (parse_key(ref_line, type_name(type_int)))
	*out_type_id = type_int;
    else if (parse_key(ref_line, type_name(type_dir)))
	*out_type_id = type_dir;
    else if (parse_key(ref_line, type_name(type_cmd)))
	*out_type_id = type_cmd;
    else if (parse_key(ref_line, type_name(type_func)))
	*out_type_id = type_func;
    else if (parse_key(ref_line, type_name(type_stream)))
	*out_type_id = type_stream;
    else if (parse_key(ref_line, type_name(type_ipaddr)))
	*out_type_id = type_ipaddr;
    else if (parse_key(ref_line, type_name(type_macaddr)))
	*out_type_id = type_macaddr;
    else if (parse_key(ref_line, type_name(type_coroutine)))
	*out_type_id = type_coroutine;
    else
	ok = FALSE;    

    return ok;
}






STATIC_INLINE bool
type_equal(type_t kind1, type_t kind2)
{   return kind1 == kind2;
}








/*****************************************************************************
 *                                                                           *
 *          Values 					                     *
 *          ======					                     *
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

   
   





typedef struct
{   value_t *heap;		/*< list of values allocated from the heap */
    int version;		/*< current heap version */
} value_heap_t;



static value_heap_t value_heap;




typedef struct
{   int depth;
    const value_t *entry[PRINTSTACK_MAX];
} value_printstack_t;




static value_printstack_t prtstk;






extern /*internal*/ value_t *
(value_init)(value_t *val, type_t kind, value_print_fn_t *print,
	     value_cmp_fn_t *compare, value_delete_fn_t *delete,
	     value_markver_fn_t *mark_version)
{   val->kind = kind;
    val->link = NULL;
    val->delete = delete;
    val->print = print;
    val->compare = compare;
    val->mark_version = mark_version;
    val->heap_version = 0;
    val->local = (mark_version != NULL);
    /* all non-constant values are initially local */

    if (NULL != delete)
    {   /* place on value heap */
        val->heap_next = value_heap.heap;
        value_heap.heap = val;
    } else
	val->heap_next = NULL;

    DEBUGGC(DPRINTF("New %s value %p %sdeleteable\n",
		    type_name(kind), val,  NULL==delete? "not ":"");)
    return val;
}





#if 0 != DEBUGVALINIT(1+)0
extern value_t *
value_info(value_t *val, int lineno)
{   val->lineno = lineno;
    return val;
}


extern const value_t *
value_cinfo(const value_t *val, int lineno)
{   ((value_t *)(val))->lineno = lineno;
    return val;
}


#define value_init(val, kind, print, compare, delete, mark_version)	\
    value_info(value_init(val, kind, print, compare, delete, mark_version), \
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
{   return val != NULL && type_equal(val->kind, kind);
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
{   if (out != NULL && val != NULL && val->print != NULL)
    {	if (prtstk.depth >= PRINTSTACK_MAX)
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
                len = (*val->print)(out, root, val);
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






static void
value_delete_alloced(value_t *value)
{   if (NULL != value)
    {   FTL_FREE(value);
    }
}
    





extern void
value_delete(value_t **ref_val)
{   value_t *val = *ref_val;
    if (val != NULL && val->delete != NULL)
    {   (*val->delete)(val);
	*ref_val = NULL;
    }
}






extern int /* <0 for less than, ==0 for equal, >0 for greater */
value_cmp(const value_t *v1, const value_t *v2)
{   if (v1 == NULL)
        return v2 == NULL? 0: -1;
    if (v2 == NULL)
        return 1;
    else if (v1->kind != v2->kind)
	return v1->kind - v2->kind;
    else if (v1 == v2)
	return 0;
    else if (NULL == v1->compare)
	return /* really we want "incomparable" value */-1;
    else
        return (*v1->compare)(v1, v2);
}







/* Garbage collection:
 *     get next version of heap
 *     mark all reachable values with new version
 *     delete all values in the heap not marked with the new version
 */




#define value_marked(val, ver) ((val)->heap_version == (ver))



static void
value_mark_version(value_t *val, int heap_version)
{   if (val != NULL && !value_marked(val, heap_version))
    {   val->heap_version = heap_version;
	DEBUGGC(DPRINTF("Mark %s value %p ver %d %s\n",
			value_type_name(val), val, heap_version,
			val->mark_version == NULL?"": "(recurse)");)
	if (val->mark_version != NULL)
	{   (*val->mark_version)(val, heap_version);
	}
    }
}





static void
value_heap_init(void)
{   value_heap.heap = (value_t *)NULL;
    value_heap.version = 0;
}





static int
value_heap_nextversion(void)
{   value_heap.version++;
    DEBUGGC(DPRINTF("Collection %d\n", value_heap.version);)
    return value_heap.version;
}





/*static*/ void
value_list_locals(void)
{   int heap_version = value_heap.version;
    value_t *val = value_heap.heap;

    while (val != NULL)
    {   if (value_islocal(val) && heap_version != val->heap_version)
	{   fprintf(stderr, "%s: Local %s value %p at %d versions ago",
		    codeid(), value_type_name(val), val,
		    heap_version - val->heap_version);
            DEBUGVALINIT(fprintf(stderr, " - line %5d: ", val->lineno););
	    IGNORE(VALUE_SHOW("", val););
	    DO(fprintf(stderr, "\n"););
	}
	val = val->heap_next;
    }
}




static void
value_mark_local(int heap_version)
{   value_t **ref_value = &value_heap.heap;

    while (*ref_value != NULL)
    {   value_t *val = *ref_value;

        if (val->heap_version != heap_version && value_islocal(val))
        {   value_mark_version(val, heap_version);
            DEBUGGC(DPRINTF("Local %s value %p marked at ver %d\n",
                            value_type_name(val), val, heap_version);)
        }
        ref_value = &(val->heap_next);
    }
}




static void
value_heap_collect(void)
{   int heap_version = value_heap.version;
    value_t **ref_value = &value_heap.heap;

    while (*ref_value != NULL)
    {   value_t *val = *ref_value;

	if (val->heap_version != heap_version)
	{   *ref_value = val->heap_next;
	    val->heap_next = NULL;
	    DO(if (value_islocal(val))
		   DPRINTF("%s: deleting %s local value %p ver %d (!=%d)\n",
			   codeid(), value_type_name(val), val,
			   val->heap_version, heap_version);)
	    DEBUGGC(DPRINTF("Collect %s value %p ver %d (!=%d)\n",
			    value_type_name(val), val, val->heap_version,
			    heap_version);)
	    value_delete(&val);
        } else
	    ref_value = &(val->heap_next);
    }
}






/*****************************************************************************
 *                                                                           *
 *          NULL Values					                     *
 *          =========== 				                     *
 *                                                                           *
 *****************************************************************************/




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
value_null_init(value_t *value, value_delete_fn_t *delete)
{   return value_init(value, type_null, &value_null_print, &value_null_compare,
		      delete, /*mark*/NULL);
}


static void
values_null_init(void)
{   value_null_init(&value_null, /* delete */NULL);
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
 *          Type Values					                     *
 *          =========== 				                     *
 *                                                                           *
 *****************************************************************************/





typedef struct 
{   value_t value;           /* integer used as a value */
    type_t type_id;
} value_type_t;




#define value_type_value(typeval) (&(typeval)->value)




static int
value_type_print(outchar_t *out, const value_t *root, const value_t *value)
{   const value_type_t *typeval = (const value_type_t *)value;
    (void)root;
    return outchar_printf(out, type_name(typeval->type_id));
}




static int /* <0 for less than, ==0 for equal, >0 for greater */
value_type_compare(const value_t *v1, const value_t *v2)
{   return ((const value_type_t *)v1)->type_id -
	   ((const value_type_t *)v2)->type_id;
}



static value_t *
value_type_init(value_type_t *typeval, type_t type_id,
		value_delete_fn_t *delete)
{   typeval->type_id = type_id;
    return value_init(value_type_value(typeval), type_type, &value_type_print,
		      &value_type_compare, delete, /*mark*/NULL);
}




extern value_t *
value_type_new(type_t type_id)
{   value_type_t *typeval = (value_type_t *)FTL_MALLOC(sizeof(value_type_t));
    
    if (NULL != typeval)
        return value_type_init(typeval, type_id, &value_delete_alloced);
    else
	return NULL;
}





extern bool
value_type_id(const value_t *value, type_t *out_type_id)
{   if (value_istype(value, type_type))
    {   *out_type_id = ((value_type_t *)value)->type_id;
	return TRUE;
    } else
	return FALSE;
}





static const value_t *
value_type_parse(const char **ref_line, const value_t *this_cmd,
	         parser_state_t *state)
{   type_t id;
    if (parse_type(ref_line, &id))
	return value_type_new(id);
    else
	return &value_null;
}












/*****************************************************************************
 *                                                                           *
 *          Integer Values				                     *
 *          ============== 				                     *
 *                                                                           *
 *****************************************************************************/







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
    (void)root;
    if (0 != (n & ~int_format_bits_indec))
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
value_int_init(value_int_t *no, number_t number, value_delete_fn_t *delete)
{   no->number = number;
    return value_init(&no->value, type_int, &value_int_print,
		      &value_int_compare, delete, /*mark*/NULL);
}



#if 0 != DEBUGVALINIT(1+)0
#define value_int_init(no, number, delete) \
    value_info((value_int_init)(no, number, delete), __LINE__)
#endif






extern value_t *
value_int_new(number_t number)
{   value_int_t *no = (value_int_t *)FTL_MALLOC(sizeof(value_int_t));
    
    if (NULL != no)
        return value_int_init(no, number, &value_delete_alloced);
    else
	return NULL;
}




#if 0 != DEBUGVALINIT(1+)0
#define value_int_new(number) \
    value_info((value_int_new)(number), __LINE__)
#endif





extern void
value_int_update(const value_t **ref_value, number_t number)
{   const value_t *val;
    IGNORE(printf("%s: update int at *%p - %d\n",
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
    
    DEBUGTRACE(DPRINTF("int %s: %s\n", ok?"OK":"FAIL", *ref_line);)
    
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

	    IGNORE(printf("%s: %"F_NUMBER_T" %s %"F_NUMBER_T
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
 *          Integer Values				                     *
 *          ============== 				                     *
 *                                                                           *
 *****************************************************************************/





value_int_t value_int_zero;
value_int_t value_int_one;

const value_t *value_zero = &value_int_zero.value;
const value_t *value_one = &value_int_one.value;




extern void
values_int_init(void)
{   value_int_init(&value_int_zero,  0, /* delete */NULL);
    value_int_init(&value_int_one,   1, /* delete */NULL);
}






/*****************************************************************************
 *                                                                           *
 *          IP Address Values				                     *
 *          ================= 				                     *
 *                                                                           *
 *****************************************************************************/







typedef struct 
{   value_t value;           /* integer used as a value */
    addr_ip_t addr;
} value_ipaddr_t;








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




static value_t *
value_ipaddr_init(value_ipaddr_t *ip, int a, int b, int c, int d,
		  value_delete_fn_t *delete)
{   ip->addr[0] = a;
    ip->addr[1] = b;
    ip->addr[2] = c;
    ip->addr[3] = d;
    return value_init(&ip->value, type_ipaddr, &value_ipaddr_print,
		      &value_ipaddr_compare, delete, /*mark*/NULL);
}






extern value_t *
value_ipaddr_new_quad(int a, int b, int c, int d)
{   value_ipaddr_t *ip = (value_ipaddr_t *)
			 FTL_MALLOC(sizeof(value_ipaddr_t));
    
    if (NULL != ip)
        return value_ipaddr_init(ip, a, b, c, d, &value_delete_alloced);
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
	    
	    if (NULL != ent &&
		ent->h_addrtype == AF_INET &&
		ent->h_length == sizeof(addr_ip_t))
	    {   addr_ip_t *retip = (addr_ip_t *)ent->h_addr;
		memcpy(&ip, retip, sizeof(ip));
		ok = TRUE;
	    }
	    DEBUGSOCK(
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





static const value_t *
value_ipaddr_parse(const char **ref_line, const value_t *this_cmd,
	           parser_state_t *state)
{   addr_ip_t ip;
    if (parse_ipaddr(ref_line, &ip))
	return value_ipaddr_new(&ip);
    else
	return &value_null;
}







/*****************************************************************************
 *                                                                           *
 *          MAC Address Values				                     *
 *          ================== 				                     *
 *                                                                           *
 *****************************************************************************/







typedef struct 
{   value_t value;           /* integer used as a value */
    addr_mac_t addr;
} value_macaddr_t;










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
		   int d, int e, int f,  value_delete_fn_t *delete)
{   mac->addr[0] = a;
    mac->addr[1] = b;
    mac->addr[2] = c;
    mac->addr[3] = d;
    mac->addr[4] = e;
    mac->addr[5] = f;
    return value_init(&mac->value, type_macaddr, &value_macaddr_print,
		      &value_macaddr_compare, delete, /*mark*/NULL);
}






extern value_t *
value_macaddr_new_sextet(int a, int b, int c, int d, int e, int f)
{   value_macaddr_t *mac = (value_macaddr_t *)
			   FTL_MALLOC(sizeof(value_macaddr_t));
    
    if (NULL != mac)
        return value_macaddr_init(mac, a, b, c, d, e, f,
				  &value_delete_alloced);
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
    unumber_t mac1, mac2, mac3, mac4, mac5, mac6;
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







/*****************************************************************************
 *                                                                           *
 *          String Values				                     *
 *          ============= 				                     *
 *                                                                           *
 *****************************************************************************/







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
{   IGNORE(printf("%s: update str at *%p - '%s'\n",
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
    if (len > 0 && (isalpha(str[0]) || str[0]=='_')) {
        do
            len--;
        while (len > 0 && (isalnum(str[len]) || str[len]=='_'));
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









STATIC_INLINE value_t *
value_stringbase_init(value_stringbase_t *str, value_string_get_fn_t *get,
		      value_delete_fn_t *delete,
		      value_markver_fn_t *mark_version)
{   str->get = get;
    return value_init(&str->value, type_string, &value_string_print,
		      &value_string_compare, delete, mark_version);
}





#if 0 != DEBUGVALINIT(1+)0
#define value_stringbase_init(str, get, delete, mark_version) \
    value_info((value_stringbase_init)(str, get, delete, mark_version), \
	       __LINE__)
#endif





extern const char *
value_string_chars(const value_t *string)
{   const char *str;
    size_t len;
    value_string_get(string, &str, &len);
    return str;
}






/*****************************************************************************
 *                                                                           *
 *          Binary String Values			                     *
 *          ====================			                     *
 *                                                                           *
 *****************************************************************************/







typedef struct 
{   value_stringbase_t base;           /* base string type */
    const char *string;
    size_t len;
} value_string_t;


#define value_string_value(str) value_stringbase_value(&(str)->base)






static void
value_string_delete(value_t *value)
{   if (value_istype(value, type_string))
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
		   value_delete_fn_t *delete)
{   str->string = string;
    str->len = len;
    return value_stringbase_init(&str->base, &value_string_get_fn,
				      delete, /*mark*/NULL);
}





#if 0 != DEBUGVALINIT(1+)0
#define value_cstring_init(str, string, len, delete) \
    value_info((value_cstring_init)(str, string, len, delete), __LINE__)
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
	{   newstr = value_cstring_init(str, string, len,
					&value_delete_alloced);
	}
    }
    return newstr;
}






STATIC_INLINE value_t *
value_string_init(value_string_t *str, char *string, size_t len,
		  value_delete_fn_t *delete)
{   if (NULL != string)
	string[len] = '\0';
    return value_cstring_init(str, string, len, delete);
}





#if 0 != DEBUGVALINIT(1+)0
#define value_string_init(str, string, len, delete) \
    value_info((value_string_init)(str, string, len, delete), __LINE__)
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
		newstr = value_string_init(str, strcopy, len,
					   &value_string_delete);
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

    if (out_string != NULL) {
        value_string_t *str = (value_string_t *)
                              FTL_MALLOC(sizeof(value_string_t));
        if (NULL != str)
        {   char *strcopy = (char *)FTL_MALLOC(len+1);

            *out_string = strcopy;

            if (NULL != strcopy)
            {   strcopy[len] = '\0';
                newstr = value_string_init(str, strcopy, len,
                                           &value_string_delete);
            } else
            {   FTL_FREE(str);
                str = NULL;
            }
        }
    }
    return newstr;
}





#if 0 != DEBUGVALINIT(1+)0
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
					           &value_string_delete);
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
    
    if (value_string_get(strval, &strbase, &len) && len >= 0) {
        /* why check for final \n? - I don't think the new string has one */
	if (strbase[len] == '\0' && (len <= 0 || strbase[len-1] == '\n')) {
	    *out_buf = strbase;
	    *out_len = len;
	} else {
	    /* we'll need to make a new copy of the string */
            IGNORE(printf("%s: copying %d-byte string\n", codeid(), len););
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
 *          Sub-String Values			                             *
 *          =================						     *
 *                                                                           *
 *****************************************************************************/







typedef struct 
{   value_stringbase_t base;           /* base string type */
    value_stringbase_t *ref;           /* string referred to */
    size_t len;
    size_t offset;
} value_substring_t;


#define value_substring_value(v) value_stringbase_value(&(v)->base)






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
		     size_t offset, size_t len)
{   value_t *initval;
    str->ref    = refstr;
    str->offset = offset;
    str->len    = len;
    initval = value_stringbase_init(&str->base,
				    &value_substring_get_fn,
				    &value_delete_alloced,
				    &value_substring_markver);
    value_unlocal(value_stringbase_value(refstr));
    return initval;
}







extern value_t *
value_substring_new(const value_t *string, size_t offset, size_t len)
{   value_substring_t *str = NULL;
    value_t *newstr = NULL;

    if (value_type_equal(string, type_string))
    {   str = (value_substring_t *)FTL_MALLOC(sizeof(value_substring_t));
	if (NULL != str)
	    newstr = value_substring_init(str, (value_stringbase_t *)string,
					  offset, len);
    }
    return newstr;
}










/*****************************************************************************
 *                                                                           *
 *          Code body Values				                     *
 *          ================ 				                     *
 *                                                                           *
 *****************************************************************************/







typedef struct 
{   value_t value;           /* code used as a value */
    const value_t *string;   /* code body */
    code_place_t place;      /* definition location */
} value_code_t;







static void
value_code_markver(const value_t *value, int heap_version)
{   value_code_t *code = (value_code_t *)value;
    value_mark_version((value_t *)code->string, heap_version);
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
		value_delete_fn_t *delete_fn,
                const char *sourcename, int lineno)
{   value_t *initval;
    code->string = string;
    code_place_set(&code->place, sourcename, lineno);
    initval = value_init(&code->value, type_code, &value_code_print,
		         &value_code_compare, delete_fn, &value_code_markver);
    value_unlocal(string);
    return initval;
}







extern value_t *
value_code_new(const value_t *string, const char *sourcename, int lineno)
{   value_code_t *code = NULL;

    if (value_istype(string, type_string))
    {   code = (value_code_t *)FTL_MALLOC(sizeof(value_code_t));
	if (NULL != code)
	    value_code_init(code, string, &value_code_delete,
			    sourcename, lineno);
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
	ok = TRUE;
    }
    /* else type error */

    return ok;
}












/*****************************************************************************
 *                                                                           *
 *          Stream Values					             *
 *          =============				                     *
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

	n += outchar_printf(out, "<-");
	if (NULL == source)
	    n += outchar_printf(out, "EOF");
	else
	    n += outchar_printf(out, "'%s'", source->name);
	
	if (NULL != sink)
	    n += outchar_printf(out, "->");

	return n;
    }	
    else
        return outchar_printf(out, "NOSTREAM");
}








static value_t *
value_stream_init(value_stream_t *stream, charsource_t *source,
		  charsink_t *sink,
		  value_delete_fn_t *delete,
		  stream_close_fn_t *close,
		  stream_sink_close_fn_t *sink_close,
                  stream_sink_delete_fn_t *sink_delete)
{   stream->source = source;
    stream->sink = sink;
    stream->close = close;
    stream->sink_delete = sink_delete;
    stream->sink_close = sink_close;
    return value_init(&stream->value, type_stream, &value_stream_print,
		      /*compare*/NULL, delete, /*mark*/NULL);
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
 *          File Stream Values					             *
 *          ==================				                     *
 *                                                                           *
 *****************************************************************************/









typedef struct 
{   value_stream_t stream;
    FILE *file;
} value_stream_file_t;






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
	return value_stream_init(&fstream->stream, source, sink,
				 &value_stream_delete,
			         close, sink_close, sink_delete);
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
    {   IGNORE(fprintf(stderr,"%s: can't open '%s' in mode '%s'\n",
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
 *          Socket Stream Values				             *
 *          ====================			                     *
 *                                                                           *
 *****************************************************************************/







#ifdef HAS_SOCKETS




typedef struct 
{   value_stream_t stream;
    int fd;
} value_stream_socket_t;






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
	return value_stream_init(&fstream->stream, source, sink,
				 &value_stream_delete,
			         close, sink_close, sink_delete);
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
 *          InString Stream Values				             *
 *          ======================			                     *
 *                                                                           *
 *****************************************************************************/









typedef struct 
{   value_stream_t stream;
} value_stream_instring_t;






#define value_stream_instring_stream(ins) (&(ins)->stream)







extern value_t *
value_stream_instring_new(const char *name, const char *string, size_t len)
{   value_stream_instring_t *instrstream =
	(value_stream_instring_t *)FTL_MALLOC(sizeof(value_stream_instring_t));
    
    if (NULL != instrstream)
    {   charsource_t *source = charsource_string_new(name, string, len);

	return value_stream_init(&instrstream->stream, source,
				 /*sink*/NULL, &value_stream_delete,
			         /*close*/NULL,
				 /*sink_close*/NULL, /*sink_delete*/NULL);
    } else
	return NULL;
}










/*****************************************************************************
 *                                                                           *
 *          OutString Stream Values				             *
 *          =======================			                     *
 *                                                                           *
 *****************************************************************************/









typedef struct 
{   value_stream_t stream;
} value_stream_outstring_t;






#define value_stream_outstring_stream(outs) (&(outs)->stream)







extern value_t *
value_stream_outstring_new(void)
{   value_stream_outstring_t *outstrstream =
	(value_stream_outstring_t *)
        FTL_MALLOC(sizeof(value_stream_outstring_t));
    
    if (NULL != outstrstream)
    {   charsink_t *sink = charsink_string_new();

	return value_stream_init(&outstrstream->stream,
				 /*source*/NULL,
				 /*sink*/sink, &value_stream_delete,
			         /*close*/NULL,
				 /*sink_close*/&charsink_string_close,
				 /*sink_delete*/&charsink_string_delete);
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

	return value_stream_init(&outstrstream->stream,
				 /*source*/NULL,
				 /*sink*/sink, &value_stream_delete,
			         /*close*/NULL,
				 /*sink_close*/&charsink_fixstring_close,
				 /*sink_delete*/&charsink_fixstring_delete);
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
 *          Directories					                     *
 *          ===========					                     *
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
    bool is_root = (root != NULL && value == root);

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
	IGNORE(pr->len += outchar_printf(out, "(%s)#%p ",
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
dir_set(dir_t *dir, const value_t *name, const value_t *value)
{   bool ok = FALSE;
    
    DEBUGDIR(printf("dir: set name %p value %p in dir %p\n",
		    name, value, dir);)
    if (NULL != dir)
    {   if (NULL != dir->lookup)
	{   const value_t **ref_value = (*dir->lookup)(dir, name);

	    if (NULL != ref_value)
	    {   /* update existing value - leave the old value for the
		   garbage collector to pick up: someone else might have
		   a pointer to it */
		*ref_value = value;
		ok = TRUE;
	    } else
	    if (NULL != dir->add)
	    {   ok = (*dir->add)(dir, name, value);
		value_unlocal(name);
		value_unlocal(value);
	    }
	} else
	if (NULL != dir->add)
	{   ok = (*dir->add)(dir, name, value);
	    value_unlocal(name);
	    value_unlocal(value);
	}
	else
	{   fprintf(stderr, "%s: name ", codeid());
	    value_fprint(stderr, /*root*/NULL, name);
	    fprintf(stderr, " has a constant value\n");
	}
    }
    return ok;
}






extern const value_t *
dir_get(dir_t *dir, const value_t *name)
{   if (NULL != dir && NULL != dir->get)
    {   DEBUGDIR(printf("dir: get name %p from dir %p\n", name, dir);)
	return (*dir->get)(dir, name);
    } else
        return NULL;
}






static const value_t *
dir_get_from_lookup(dir_t *dir, const value_t *name)
{   const value_t **ref_value = NULL;

    DEBUGDIR(printf("dir: get using lookup dir %p\n", dir);)
    if (NULL != dir && NULL != dir->lookup)
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
    (*lv_len)++;
    return NULL;
}






static unsigned
dir_count_from_forall(dir_t *dir)
{   unsigned len = 0;

    DEBUGDIR(printf("dir: count using forall dir %p\n", dir);)
    if (NULL != dir && NULL != dir->forall)
 	(void)(*dir->forall)(dir, &dir_for_count, &len);

    return len;
}






extern dir_lock_state_t *
dir_lock(dir_t *dir, dir_lock_state_t *old_lock)
{   if (NULL != dir)
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
dir_init(dir_t *dir, dir_add_fn_t *add, dir_lookup_fn_t *lookup,
	 dir_get_fn_t *get, dir_forall_fn_t *forall,
	 value_print_fn_t *print, value_delete_fn_t *delete,
	 value_markver_fn_t *mark)
{   dir->add = add;
    dir->lookup = lookup;
    if (NULL == get && NULL != lookup)
        dir->get = &dir_get_from_lookup;
    else
        dir->get = get;
    dir->forall = forall;
    dir->count = &dir_count_from_forall;
    dir->env_end = FALSE;
    
    return value_init(&dir->value, type_dir,
		      NULL != print? print: &dir_print,
		      &dir_compare, delete, mark);
}





#if 0 != DEBUGVALINIT(1+)0
#define dir_init(dir, add, lookup, get, forall, print, delete, mark) \
    value_info((dir_init)(dir, add, lookup, get, forall, print, delete, mark),\
	       __LINE__)
#endif




extern bool
dir_int_set(dir_t *dir, int index, const value_t *value)
{   value_int_t nameval;
    value_int_init(&nameval, index, /*delete*/NULL);
    return dir_set(dir, &nameval.value, value);
}






extern bool
dir_string_set(dir_t *dir, const char *name, const value_t *value)
{   value_t *nameval = value_string_new_measured(name);
    return NULL == nameval? FALSE: dir_set(dir, nameval, value);
}





extern const value_t *
dir_int_get(dir_t *dir, int n)
{   value_int_t nameval;
    value_int_init(&nameval, n, /*delete*/NULL);
    return dir_get(dir, &nameval.value);
}







extern const value_t *
dir_string_get(dir_t *dir, const char *name)
{   value_string_t nameval;
    value_cstring_init(&nameval, name, strlen(name), /*delete*/NULL);
    return dir_get(dir, value_string_value(&nameval));
}







extern const value_t *
dir_stringl_get(dir_t *dir, const char *name, size_t namelen)
{   value_string_t nameval;
    value_cstring_init(&nameval, name, namelen, /*delete*/NULL);
    IGNORE(
	printf("%s: look up '", codeid());
        value_fprint(stdout, /*root*/NULL, (value_t *)&nameval);
        printf("'\n");)
    return dir_get(dir, value_string_value(&nameval));
}






extern bool
dir_enumerable(dir_t *dir)
{    return (NULL != dir && NULL != dir->forall);
}





extern void *
dir_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{    if (NULL != dir && NULL != dir->forall)
        return (*dir->forall)(dir, enumfn, arg);
     else
        return NULL;
}






extern unsigned
dir_count(dir_t *dir)
{    if (NULL != dir && NULL != dir->count)
        return (*dir->count)(dir);
     else
        return 0;
}






#define dir_env_end(_dir) ((_dir)->env_end)

#define dir_env_end_set(_dir, _end) (_dir)->env_end = _end







/*****************************************************************************
 *                                                                           *
 *          Identifier Directories			                     *
 *          ======================			                     *
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






#define dir_id_dir(dirid) (&((dirid)->dir))
#define dir_id_value(dirid) dir_value(dir_id_dir(dirid))






static void dir_id_delete(value_t *value)
{   if (value_istype(value, type_dir))
    {   dir_id_t *iddir = (dir_id_t *)value;
	binding_t *bind = iddir->bindlist;

	while (NULL != bind)
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

    while (NULL != bind)
    {   value_mark_version(bind->name, heap_version);
	value_mark_version((value_t *)bind->value, heap_version);
	bind = bind->link;
    }
}





static bool
dir_id_add(dir_t *dir, const value_t *name, const value_t *value)
{   dir_id_t *iddir = (dir_id_t *)dir;
    binding_t *newbind = (binding_t *)NULL;

    if (value_istype(name, type_string))
    {	newbind = (binding_t *)FTL_MALLOC(sizeof(binding_t));

	if (newbind != NULL)
	{   newbind->name = (value_t *)name;
	    newbind->value = value;
	    newbind->link = NULL;
	    
	    *iddir->list_end = newbind;
	    iddir->list_end = &newbind->link;
	}
    }
    return NULL != newbind;
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

	while (NULL != bind &&
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

    while (NULL != bind && NULL == result)
    {   result = (*enumfn)(dir, bind->name, bind->value, arg);
	bind = bind->link;
    }

    return result;
}





static void
dir_id_init(dir_id_t *iddir)
{   dir_init(&iddir->dir, &dir_id_add, &dir_id_lookup, /*get*/ NULL,
	     &dir_id_forall, /*print*/ NULL, &dir_id_delete, &dir_id_markver);
    iddir->bindlist = NULL;
    iddir->list_end = &iddir->bindlist;
}






extern dir_t *
dir_id_new(void)
{   dir_id_t *iddir = (dir_id_t *)FTL_MALLOC(sizeof(dir_id_t));

    if (NULL != iddir)
	dir_id_init(iddir);

    return &iddir->dir;
}








/*****************************************************************************
 *                                                                           *
 *          Builtin Argument Name String Values		                     *
 *          ===================================		                     *
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
	IGNORE(fprintf(stderr,"%s: arg '%s'[%d]\n", codeid(),
		       argname, strlen(argname)););
        (void)value_cstring_init(&value_string_argname_val[i],
				 argname,
				 strlen(argname),
				 /*delete*/NULL);
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
    IGNORE(
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
 *          Structure Specifications			                     *
 *          ========================			                     *
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
    while (NULL != field)
    {   struct_field_t *doomed = field;
	field = field->link;
	FTL_FREE(doomed);
    }
    struct_spec_init(spec);
}







static void
struct_spec_field_noset(void *mem, const value_t *val)
{   fprintf(stderr, "%s: write of %s ignored - read only field\n",
	        codeid(), type_name(value_type(val)));
}






extern void
struct_spec_add_field(struct_spec_t *spec, field_kind_t kind,
		      const char *name, 
		      field_set_fn_t *set, field_get_fn_t *get)
{   struct_field_t *newfield = (struct_field_t *)
			       FTL_MALLOC(sizeof(struct_field_t));
    if (NULL != newfield)
    {   value_cstring_init(&newfield->strval, name, strlen(name),
       		           /*delete*/NULL);
        newfield->name = name;
	if (NULL == set)
	    set = &struct_spec_field_noset;
	field_init(&newfield->field, kind, set, get);
        newfield->link = NULL;
	 
        *spec->end_fields = newfield;
        spec->end_fields = &newfield->link;
	IGNORE(printf("%s: add spec field \"%s\" - get %p set %p\n",
	              codeid(), newfield->name,
		      newfield->field.get, newfield->field.set);)
    }
}






static struct_field_t *
struct_spec_find_field(struct_spec_t *spec, const char *name, size_t namelen)
{   struct_field_t *field = spec->fields;
    const char *fieldname;

    while (NULL != field &&
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

    while (NULL != field && NULL == result)
    {   result = (*fn)(field, arg);
	IGNORE(printf("%s: spec field \"%s\" result %p\n",
	              codeid(), field->name, result);)
	field = field->link;
    }

    return result;
}






		      


    


/*****************************************************************************
 *                                                                           *
 *          Structure Directories			                     *
 *          =====================			                     *
 *                                                                           *
 *****************************************************************************/







typedef struct 
{   dir_id_t get_cache;
    struct_spec_t spec;
    void *structmem;
    const value_t *ref;
} dir_struct_t;






#define dir_struct_dir(dirstruct) dir_id_dir(&((dirstruct)->get_cache))
#define dir_struct_value(dirstruct) dir_value(dir_struct_dir(dirstruct))







static void
dir_struct_delete(value_t *value)
{   dir_struct_t *structdir = (dir_struct_t *)value;
    if (NULL != structdir->structmem)
    {   free(structdir->structmem);
	structdir->structmem = NULL;
    }
    structdir->ref = (const value_t *)NULL;
    dir_id_delete(dir_id_value(&structdir->get_cache));
}
    



static void
dir_struct_markver(const value_t *value, int heap_version)
{   dir_struct_t *structdir = (dir_struct_t *)value;
    IGNORE(printf("%s: marking struct dir %p ref %p\n",
	          codeid(), structdir, structdir->ref);)
    value_mark_version((value_t *)structdir->ref, heap_version);
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
        if (NULL != field)
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
        if (NULL != field)
	{   /* get any value_t we've been saving to put return value into */
	    dir_t *get_cache = dir_id_dir(&structdir->get_cache);
	    const value_t **ref_value =	dir_id_lookup(get_cache, nameval);
	    
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
    IGNORE(printf("%s: get field \"%s\" in struct %p val at %p\n",
	          codeid(), field->name, args->structmem, ref_value);)
    if (NULL != ref_value)
    {   IGNORE(printf("%s: get field \"%s\" call get %p\n",
	              codeid(), field->name, field->get);)
	(*field->field.get)(args->structmem, ref_value);
	IGNORE(printf("%s: got field \"%s\" of %p new val is %p\n",
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
		bool is_const, void *structmem, 
		const value_t *ref, value_delete_fn_t *delete_fn)
{   dir_t *dir = dir_struct_dir(structdir);
    dir_id_init(&structdir->get_cache);
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
    if (NULL != delete_fn)
	dir_value(dir)->delete = delete_fn;
    if (NULL != ref)
	dir_value(dir)->mark_version = &dir_struct_markver;
}






extern dir_t *
dir_cstruct_new(struct_spec_t *spec, bool is_const, void *static_struct)
{   dir_struct_t *structdir = (dir_struct_t *)FTL_MALLOC(sizeof(dir_struct_t));

    if (NULL != structdir)
	dir_struct_init(structdir, spec, is_const, static_struct, 
			/*ref*/NULL, /*delete*/NULL);

    return dir_struct_dir(structdir);
}







extern dir_t *
dir_struct_new(struct_spec_t *spec, bool is_const, void *malloc_struct)
{   dir_struct_t *structdir = (dir_struct_t *)FTL_MALLOC(sizeof(dir_struct_t));

    if (NULL != structdir)
	dir_struct_init(structdir, spec, is_const, malloc_struct, 
			/*ref*/NULL, &dir_struct_delete);

    return dir_struct_dir(structdir);
}







extern dir_t *
dir_struct_cast(struct_spec_t *spec, bool is_const,
		const value_t *ref, void *ref_struct)
{   dir_struct_t *structdir = (dir_struct_t *)FTL_MALLOC(sizeof(dir_struct_t));

    if (NULL != structdir)
	dir_struct_init(structdir, spec,
			is_const, ref_struct, ref, /*delete*/NULL);

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
 *          String Argument vector Directories		                     *
 *          ==================================			             *
 *                                                                           *
 *****************************************************************************/








typedef struct 
{   dir_t dir;
    size_t argc;
    const char **argv;
} dir_argvec_t;









static const value_t *
dir_argvec_get(dir_t *dir, const value_t *name)
{   dir_argvec_t *vecdir = (dir_argvec_t *)dir;
    const value_t *found = NULL;
    const char **argv = vecdir->argv;
    
    if (value_istype(name, type_int) && NULL != argv)
    {   size_t index = (size_t)value_int_number(name);
	if (index >= 0 && index < vecdir->argc)
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
	
	IGNORE(printf("argv has %d entries\n", vecdir->argc);)
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
{   dir_init(&vecdir->dir, /*add*/NULL, /*lookup*/NULL, &dir_argvec_get,
	     &dir_argvec_forall, /*print*/NULL, /*delete*/NULL,
	     /*markver*/NULL);
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
 *          Integer vector Directories			                     *
 *          ==========================			                     *
 *                                                                           *
 *****************************************************************************/






#define DIR_VEC_N_INIT 10




typedef struct 
{   dir_t dir;
    const value_t **bindvec;
    size_t n;
    size_t maxn;
} dir_vec_t;




#define dir_vec_dir(vec) (&(vec)->dir)
#define dir_vec_value(vec) dir_value(dir_vec_dir(vec))




static void
dir_vec_delete(value_t *value)
{   if (value_istype(value, type_dir))
    {   dir_vec_t *vecdir = (dir_vec_t *)value;
	if (NULL != vecdir->bindvec)
	{   FTL_FREE((value_t *)vecdir->bindvec);
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
    
    if (NULL != bind)
    {   const value_t **endbind = vecdir->bindvec + vecdir->n;
	
        while (bind < endbind)
	{   value_mark_version((value_t *)*bind, heap_version);
	    bind++;
	}
    }
}





static bool
dir_vec_int_add(dir_t *dir, size_t index, const value_t *value)
{   dir_vec_t *vecdir = (dir_vec_t *)dir;
    bool ok = (NULL != vecdir);

    if (ok)
    {   const value_t **bindvec = vecdir->bindvec;

	if (index >= vecdir->maxn)
        {   size_t old_maxn = vecdir->maxn;
	    size_t new_maxn = old_maxn == 0? DIR_VEC_N_INIT: 2*old_maxn;
	    value_t **newbind;

	    if (new_maxn < index+1)
		new_maxn = index+1;

	    IGNORE(printf("extend vec from %d to %d (index %d)\n",
			  old_maxn, new_maxn, index);)
	    newbind = (value_t **)FTL_MALLOC(new_maxn*sizeof(value_t *));
	    if (NULL != newbind)
	    {   memcpy(newbind, bindvec, old_maxn*sizeof(value_t *));
		memset(newbind+old_maxn, 0,
		       (new_maxn-old_maxn)*sizeof(value_t *));
		vecdir->bindvec = (const value_t **)newbind;
		vecdir->maxn = new_maxn;
		if (NULL != bindvec)
		    FTL_FREE((value_t **)bindvec);
	    } else
		ok = FALSE;
	}
	if (ok)
	{   vecdir->bindvec[index] = value;
	    if (index+1 > vecdir->n)
		vecdir->n = index+1;
	}
	IGNORE(else printf("Failed to add index %d - max is %d\n",
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
    
    if (NULL != bindvec && index >= 0 && index < vecdir->n)
	return &bindvec[index];
    else
	return NULL;
}







static const value_t **
dir_vec_lookup(dir_t *dir, const value_t *name)
{   dir_vec_t *vecdir = (dir_vec_t *)dir;
    const value_t **found = NULL;
    const value_t **bindvec = vecdir->bindvec;
    
    if (value_type_equal(name, type_int) && NULL != bindvec)
    {   size_t index = (size_t)value_int_number(name);
	if (index >= 0 && index < vecdir->n)
	    found = &bindvec[index];
    }	

    return found;
}





static void *
dir_vec_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg)
{   dir_vec_t *vecdir = (dir_vec_t *)dir;
    const value_t **bindvec = vecdir->bindvec;
    void *result = NULL;

    if (NULL != bindvec)
    {   size_t i = 0;
	
	IGNORE(printf("vec has %d entries\n", vecdir->n);)
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

    if (value != NULL)
    {
	if (pr->first)
	    pr->first = FALSE;
	else
	    pr->len += outchar_printf(pr->out, pr->delim);

	if (index != pr->index_expect)
	{   pr->len += value_print(pr->out, pr->root, name);
	    pr->len += outchar_printf(pr->out, "=");
	}
	IGNORE(pr->len += outchar_printf(pr->out, "(%s)#%p ",
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
	    
	if (NULL != dir->forall)
	    (void)(*dir->forall)(dir, &value_dir_vec_print, &pr);
	else
	    pr.len += outchar_printf(out, "<unenumerable-dir>");
	    
	pr.len += outchar_printf(out, ">");
    }

    return pr.len;
}





static value_t *
dir_vec_init(dir_vec_t *vecdir)
{   value_t *val = dir_init(&vecdir->dir, &dir_vec_add, &dir_vec_lookup,
			    /*get*/NULL, &dir_vec_forall, &dir_vec_print,
			    &dir_vec_delete, &dir_vec_markver);
    vecdir->bindvec = NULL;
    vecdir->n = 0;
    vecdir->maxn = 0;
    return val;
}




#if 0 != DEBUGVALINIT(1+)0
#define dir_vec_init(vecdir) value_info((dir_vec_init)(vecdir), __LINE__)
#endif




extern dir_t *
dir_vec_new(void)
{   dir_vec_t *vecdir = (dir_vec_t *)FTL_MALLOC(sizeof(dir_vec_t));

    if (NULL != vecdir)
	dir_vec_init(vecdir);

    return &vecdir->dir;
}





#if 0 != DEBUGVALINIT(1+)0
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
 *          Arrayof Struct Directories			                     *
 *          ==========================			                     *
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
	IGNORE(printf("%s: add array content - get %p set %p\n",
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
    IGNORE(printf("%s: marking array dir %p\n", codeid(), arraydir);)
    value_mark_version((value_t *)arraydir->ref, heap_version);
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
    {	int index = 0;
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
	       const value_t *ref, value_delete_fn_t *delete)
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
    if (NULL != delete)
	dir_value(dir)->delete = delete;
    if (NULL != ref)
	dir_value(dir)->mark_version = &dir_array_markver;
}





extern dir_t *
dir_carray_new(array_spec_t *spec, bool is_const,
	       void *static_array, size_t stride)
{   dir_array_t *arraydir = (dir_array_t *)FTL_MALLOC(sizeof(dir_array_t));

    if (NULL != arraydir)
	dir_array_init(arraydir, spec, is_const, static_array, stride,
		       /*dim*/NULL, /*ref*/NULL, /*delete*/NULL);

    return dir_array_dir(arraydir);
}







extern dir_t *
dir_array_new(array_spec_t *spec, bool is_const,
	      void *malloc_array, size_t stride)
{   dir_array_t *arraydir = (dir_array_t *)FTL_MALLOC(sizeof(dir_array_t));

    if (NULL != arraydir)
	dir_array_init(arraydir, spec, is_const, malloc_array, stride,
		       /*dim*/NULL, /*ref*/NULL, &dir_array_delete);

    return dir_array_dir(arraydir);
}







extern dir_t *
dir_array_cast(array_spec_t *spec, bool is_const, const value_t *ref,
	       void *ref_array, size_t stride)
{   dir_array_t *arraydir = (dir_array_t *)FTL_MALLOC(sizeof(dir_array_t));

    if (NULL != arraydir)
	dir_array_init(arraydir, spec, is_const, ref_array, stride, 
                       /*dim*/NULL, ref, /*delete*/NULL);

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
			   &dir_array_string_dim, string, /*delete*/NULL);
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
 *          Integer Series Directories			                     *
 *          ==========================			                     *
 *                                                                           *
 *****************************************************************************/






typedef struct 
{   dir_t dir;
    number_t first;
    number_t inc;
    number_t last;
} dir_series_t;







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
{   if ((inc == 0 && first != last) ||
        (inc > 0 && first > last) ||
        (inc < 0 && first < last))
        /* impossible series */
        return NULL;
    else
    {   series->first = first;
        series->inc = inc;
        series->last = last;

        DEBUGSERIES(DPRINTF("series: %ld +%ld .. %ld\n", first, inc, last););

        dir_init(&series->dir, /*add*/NULL, /*lookup*/NULL,
                 &dir_series_get, &dir_series_forall, &dir_series_print,
                 &value_delete_alloced, /*mark*/NULL);

        return &series->dir;
    }
}






static dir_t *
dir_series_new_vals(const value_t *firstval, const value_t *secondval,
	            const value_t *lastval)
{   dir_series_t *series;

    DEBUGSERIES(DPRINTF("firstval %s, secondval %s, lastval %s\n",
	                value_type_name(firstval), value_type_name(secondval),
	                value_type_name(lastvalval));)
    
    if ((firstval == NULL || value_istype(firstval, type_int)) &&
	(secondval == NULL || value_istype(secondval, type_int)) &&
	(lastval == NULL || value_istype(lastval, type_int)))
    {	series = (dir_series_t *)FTL_MALLOC(sizeof(dir_series_t));

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
 *          System Env Directories			                     *
 *          ======================			                     *
 *                                                                           *
 *****************************************************************************/








typedef struct 
{   dir_t dir;
} dir_sysenv_t;






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
	if (NULL != strval)
            val = value_string_new_measured(strval);
        IGNORE(else printf("%s: sysenv get '%s' not in env\n",
			   codeid(), namestr);)
    }
    return val;
}







static void
dir_sysenv_init(dir_sysenv_t *sysdir)
{   dir_init(&sysdir->dir, &dir_sysenv_add, /*lookup*/ NULL,
	     /*get*/&dir_sysenv_get, /* forall */NULL,
	     /*print*/ NULL, /*delete*/NULL, /*markver*/NULL);
}






extern dir_t *
dir_sysenv_new(void)
{   dir_sysenv_t *sysdir = (dir_sysenv_t *)FTL_MALLOC(sizeof(dir_sysenv_t));

    if (NULL != sysdir)
	dir_sysenv_init(sysdir);

    return &sysdir->dir;
}








/*****************************************************************************
 *                                                                           *
 *          Registry Directories			                     *
 *          ====================			                     *
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
    char *_namevar##_lastpos;     					\
    char _namevar##_lastch;                                             \
    const char *_namevar

/* This is nasty and might cause errors in strange cirucmstances */
    
#define REGVAL_NAMEVAR_CH_T char

#define REGVAL_NAMEVAR_OPEN(_namevar, _mbs, _mbslen)                    \
    (_namevar = _mbs, _namevar##_lastpos = (char *)&(_mbs)[_mbslen],	\
     (_namevar##_lastch = *_namevar##_lastpos) == '\0'?			\
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
        {   IGNORE(fprintf(stderr, "%s: unicode conversion failed for '%s'\n",
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
		    IGNORE(fprintf(stderr, "%s: unicode conversion of '%s' - "
			           "bad size %d (supposed to be %d)?\n",
			            codeid(), str, n, realn);)
		    return (wchar_t *)NULL; /* size error? */
		} else
		{   /* updated only on success */
		    *out_wchars = realn;  /* count ignores final L'\0; */
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
    
    IGNORE(fprintf(stderr, "%s: closing key handle %p\n", codeid(), key);)
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
		{   IGNORE(fprintf(stderr, "%s: (real) query for reqistry "
			           "value failed rc %d\n", codeid(), rc);)
		    FTL_FREE(data);
		} else
		{   *out_data = data;
		    *out_datalen = datalen;
		}
	    }
	}
	IGNORE(else fprintf(stderr, "%s: initial query for reqistry value "
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
		IGNORE(else
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

    IGNORE(fprintf(stderr,"%s: key type %d data size %d\n",
	           codeid(), key_type, datalen););
    
    if (NULL != dir)
    {   unumber_t n;
	dir_string_set(dir, "type", value_int_new(key_type));
	switch (key_type)
	{
	    case REG_DWORD_LITTLE_ENDIAN:
		n = (unumber_t)data[0]       | ((unumber_t)data[1]<<8) |
		    ((unumber_t)data[2]<<16) | ((unumber_t)data[3]<<24);
		dir_string_set(dir, "data", value_int_new(n));
		break;

	    case REG_DWORD_BIG_ENDIAN:
		n = (unumber_t)data[3]       | ((unumber_t)data[2]<<8) |
		    ((unumber_t)data[1]<<16) | ((unumber_t)data[0]<<24);
		dir_string_set(dir, "data", value_int_new(n));
		break;

	    case REG_QWORD_LITTLE_ENDIAN:
		n = (unumber_t)data[0]       | ((unumber_t)data[1]<<8)  |
		    ((unumber_t)data[2]<<16) | ((unumber_t)data[3]<<24) |
		    ((unumber_t)data[4]<<32) | ((unumber_t)data[5]<<40) |
		    ((unumber_t)data[6]<<48) | ((unumber_t)data[7]<<56);
		dir_string_set(dir, "data", value_int_new(n));
		break;

	    case REG_EXPAND_SZ:
	    case REG_LINK:
	    case REG_SZ:
#ifdef UNICODE_REGVAL_NAMES
	    {   size_t charlen = (datalen+1)/sizeof(wchar_t);
		const wchar_t data_wc = (const wchar_t *)data;
		if (data_wc[charlen-1] == '\0')
		    charlen--;
		dir_string_set(dir, "data",
			       value_wcstring_new(data_wc, charlen);
#else
		if (data[datalen-1] == '\0')
		    datalen--;
		dir_string_set(dir, "data",
			       value_string_new((const char *)data, datalen));
#endif
		break;

	    case REG_MULTI_SZ:
		/* actually an array of zero terminated strings,
		   terminated by another zero - could do better */
		dir_string_set(dir, "data",
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
		dir_string_set(dir, "data",
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
	IGNORE(fprintf(stderr,"%s: enum %d %scomplete rc %d\n",
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
				codeid(), data_wchars, datastrl);
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
		   key_access_t access, value_delete_fn_t *delete)
{   long rc = win_regkeyval_open(root, keyname, keynamelen, access,
				 &keydir->key);
    
    if (ERROR_SUCCESS == rc)
        dir_init(&keydir->dir, &dir_regkeyval_add, /*lookup*/ NULL,
	         &dir_regkeyval_get, &dir_regkeyval_forall,
	         /*print*/ NULL, delete, /*markver*/NULL);
    IGNORE(else
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
			       &dir_regkeyval_delete) != ERROR_SUCCESS)
	{   FTL_FREE(keydir);
	    keydir = NULL;
	    ok = FALSE;
	}
    }
    
    if (ok)
    {	return dir_regkeyval_dir(keydir);
    } else
	return NULL;
}




#endif /* HAS_REGISTRY */










/*****************************************************************************
 *                                                                           *
 *          Stacked Directory Directories	                             *
 *          =============================				     *
 *                                                                           *
 *****************************************************************************/










/* This is a stack of directories which use the "env_end" field in a	*/
/* directory to determine how long a prefix chain of directories are to	*/
/* be used for finding symbols.  Subsequent directories can	        */
/* eventually be "popped" and will be kept from the garbage collector	*/
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
    
    if (NULL != value)
    {   value_t *dir = &dirstack->stack->value;
	
        while (NULL != dir)
	{   value_mark_version(dir, heap_version);
	    dir = dir->link;
	}
    }
}







static bool
dir_stack_push_at_pos(dir_stack_pos_t pos, dir_t *newdir, bool env_end)
{   if (NULL != pos && NULL != newdir)
    {   newdir->value.link = *pos;
	dir_env_end_set(newdir, env_end);
	DEBUGDIR(DPRINTF("dir_stack: set end in dir %p\n", newdir);)
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

	if (NULL != dir && dir->add != NULL)
	    ok = (*dir->add)(dir, name, value);
	else
	{   /* create a new dir if top directory missing or unwriteable */
	    dir_stack_push(dirstack, dir_id_new(), /*env_end*/FALSE);
	    dir = dirstack->stack;
	    if (NULL != dir && dir->add != NULL)
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
	DEBUGDIR(printf("dir_stack: first lookup name %p in dir %p\n",
			name, dir);)
	/* find a directory where this variable is set */
	/* TODO: need to take account of dirs with "get" only (e.g. sys.env) */
	while (dir != NULL && 
	       (dir->lookup == NULL ||
	        NULL == (ref_val = (*dir->lookup)(dir, name))))
	{   dir_t *nextdir = (dir_t *)dir->value.link;
	    
	    ftl_assert(dir != (dir_t *)dir->value.link);
	    if (nextdir != NULL && dir_env_end(nextdir))
	    {   DEBUGDIR(DPRINTF("** lookup blocked **\n");)
		dir = NULL;
	    } else
	        dir = nextdir;
	    
  	    DEBUGDIR(printf("dir_stack: next  lookup name %p in dir %p\n",
			    name, dir);)
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

	if (dir != basedir);
	{   DEBUGDIR(DPRINTF("dir_stack: first get name %p in dir %p\n",
			     name, dir);)
	    while (dir != NULL && 
		   (dir->get == NULL ||
		    NULL == (val = (*dir->get)(dir, name))))
	    {   dir_t *nextdir = (dir_t *)dir->value.link;

		ftl_assert(dir != (dir_t *)dir->value.link);
		if (nextdir != NULL && dir_env_end(dir))
		{   DEBUGDIR(DPRINTF("** get blocked **\n");)
		    dir = NULL;
		} else
		    dir = nextdir;

		DEBUGDIR(printf("dir_stack: next  get name %p in dir %p\n",
				name, dir);)
	    }
	} DEBUGDIR(else DPRINTF("dir_stack: first dir in stack %p is itself\n",
			        dir);)
    }    
    return val;
}





typedef struct
{   dir_t *dir;
    dir_enum_fn_t *enumfn;
    void *arg;
} dir_stack_forall_arg_t;





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
	
	while (NULL == result && dir != NULL)
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
dir_stack_init(dir_stack_t *dirstack, value_print_fn_t *print,
	       value_delete_fn_t *delete, value_markver_fn_t *mark)
{   dir_init(&dirstack->dir, &dir_stack_add, &dir_stack_lookup, &dir_stack_get,
	     &dir_stack_forall, print, delete, mark);
    dirstack->stack = NULL;
    return &dirstack->dir;
}






#if 0 != DEBUGVALINIT(1+)0
#define dir_stack_init(dirstack, print, delete, mark) \
    dir_info((dir_stack_init)(dirstack, print, delete, mark), __LINE__)
#endif






extern dir_t *
dir_stack_new(void)
{   dir_stack_t *dirstack = (dir_stack_t *)FTL_MALLOC(sizeof(dir_stack_t));

    if (NULL != dirstack)
	dir_stack_init(dirstack, /*print*/NULL, &value_delete_alloced,
		       &dir_stack_markver);

    return &dirstack->dir;
}






#if 0 != DEBUGVALINIT(1+)0
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
    {   dir_stack_init(dirstack, /*print*/NULL, &value_delete_alloced,
		       &dir_stack_markver);
	dir_stack_copyinit(dirstack, old);
    }
    return dirstack;
}
















/*****************************************************************************
 *                                                                           *
 *          Closure Environments			                     *
 *          ====================			                     *
 *                                                                           *
 *****************************************************************************/







struct value_env_s
{   dir_stack_t dirs;       /* stack of bound variables */
    value_t *unbound;       /* stack of unbound variables */
    /* value_t's have a 'link' field which is used to implement a stack */
} /* value_env_t */;





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
	IGNORE(pr->len += outchar_printf(pr->out, "(%s)#%p ",
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

    while (NULL != ptr && ptr->link != end)
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
	
	if (NULL != dir->forall)
	{   value_t *unbound = envdir->unbound;
	    value_t *found_root;
	    
	    pr.len += outchar_printf(out, "[");
	    found_root = (*dir->forall)(dir, &value_env_bind_print, &pr);
	    
	    while (NULL != unbound)
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
	    {   if (NULL != found_root)
	            pr.len += outchar_printf(out, ":root");
	    } else
		*out_root_relative = (NULL != found_root);
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
    
    while (NULL != unbound)
    {   value_unlocal(unbound);
	unbound = unbound->link;
    }
}





static void
value_env_markver(const value_t *value, int heap_version)
{   value_env_t *envdir = (value_env_t *)value;
    value_t *unbound = envdir->unbound;

    dir_stack_markver(&envdir->dirs.dir.value, heap_version);
    
    while (NULL != unbound)
    {   value_mark_version(unbound, heap_version);
	unbound = unbound->link;
    }
}





static value_env_t *
value_env_init(value_env_t *env, value_delete_fn_t *delete)
{   dir_stack_init(&env->dirs, &value_env_print, delete, &value_env_markver);
    env->unbound = NULL;

    return env;
}






extern value_env_t *
value_env_new(void)
{   value_env_t *envdir = (value_env_t *)FTL_MALLOC(sizeof(value_env_t));

    if (NULL != envdir)
	return value_env_init(envdir, &value_delete_alloced);
    else
	return NULL;
}






#if 0 != DEBUGVALINIT(1+)0
#define value_env_new() \
    value_info((value_env_new)(), __LINE__)
#endif






extern value_env_t *
value_env_copy(value_env_t *envdir_from)
{   value_env_t *envdir_to = value_env_new();

    if (NULL != envdir_to)
    {   dir_stack_copyinit(&envdir_to->dirs, &envdir_from->dirs);
	envdir_to->unbound = envdir_from->unbound;
    }
    
    return envdir_to;
}






extern void
value_env_pushdir(value_env_t *env, dir_t *newdir, bool env_end)
{   if (NULL != env)
    {   IGNORE(
	    printf("%s: push onto stack %p %s ",
		   codeid(), &env->dirs,
		   value_type_name(dir_value(newdir)));
	    DIR_SHOW("", newdir);
	)
        dir_stack_push(&env->dirs, newdir, env_end);
    }
}






/* make a value_env dir_t with only one directory on it - e.g. valuable in
   providing a dir_t with a spare link field albeit with the same values as
   the pushed directory
*/
extern dir_t *
value_env_newpushdir(dir_t *newdir, bool env_end)
{   value_env_t *env = value_env_new();
    value_env_pushdir(env, newdir, env_end);
    return value_env_dir(env);
}






extern bool
value_env_pushenv(value_env_t *env, value_env_t *newenv, bool env_end)
{   bool ok = TRUE;
    if (NULL != env)
    {   if (NULL != env->unbound)
	    ok = FALSE; /* can't add to env with unbound variables */
	else
	{   dir_stack_push(&env->dirs, value_env_dir(newenv), env_end);
	    env->unbound = newenv->unbound;
	}
    }
    return ok;
}





extern value_t * /*pos*/
value_env_pushunbound(value_env_t *env, value_t *pos, value_t *name)
{   if (NULL != name && env != NULL)
    {   if (NULL != pos)
	    pos->link = name;
	else
	{   name->link = env->unbound;
	    env->unbound = name;
	}
	return name;
    } else
	return NULL;
}






STATIC_INLINE value_t *
value_env_unbound(value_env_t *env)
{   if (NULL != env)
    {   return env->unbound;
    } else
	return NULL;
}







/* return a envdir which has the first unbound variable set to value */
extern value_t *
value_env_bind(value_env_t *envdir, const value_t *value)
{   value_t *newenvdirval = NULL;
    const value_t *unbound = envdir->unbound;

    if (NULL != unbound)
    {   newenvdirval = (value_t *)value_env_new();
	if (NULL != newenvdirval)
	{   dir_t *localbind = dir_id_new();
	    value_env_t *newenvdir = (value_env_t *)newenvdirval;

	    newenvdir->unbound = unbound->link;
	    newenvdir->dirs.stack = envdir->dirs.stack;

	    if (NULL != localbind)
	    {   dir_set(localbind, unbound, value);
		IGNORE(DIR_SHOW("\n** pushing env: ", localbind);
		       DIR_SHOW("\n** onto env: ",
				  value_env_dir(newenvdir));)
		value_env_pushdir(newenvdir, localbind, /*env_end*/FALSE);
		IGNORE(DIR_SHOW("\n** result: ",
				  value_env_dir(newenvdir));)
		
	    } else
	    {   /* will garbage collect localbind and envdir later */
		value_unlocal(value_env_value(newenvdir));
		newenvdir = NULL;
	    }
	}
    }
    value_unlocal(value);

    return newenvdirval;
}









#if 0 != DEBUGVALINIT(1+)0
#define value_env_bind(envdir, value)			\
    value_info((value_env_bind)(envdir, value), __LINE__)
#endif




/* accumulate sum of two envionments in the first */
static bool
value_env_sum(value_env_t **ref_baseenv, value_env_t *plusenv)
{   bool ok = TRUE;
    
    if (*ref_baseenv == NULL)
        *ref_baseenv = plusenv;
    
    else if (plusenv != NULL)
    {
        dir_t *env = value_env_dir(plusenv);
        dir_t *baseenv_env = value_env_dir(*ref_baseenv);
        value_t *unbound = value_env_unbound(plusenv);
        value_t *baseenv_unbound = value_env_unbound(*ref_baseenv);

        DEBUGENV(DPRINTF("env sum: (%s+%s) + (%s+%s)\n",
                         baseenv_env==NULL?"":"dir",
                         baseenv_unbound==NULL?"":"unbound",
                         env==NULL?"":"dir", unbound==NULL?"":"unbound"););
        if (baseenv_env == NULL && baseenv_unbound == NULL)
            *ref_baseenv = value_env_copy(plusenv);
            /* nothing in the new environment! */
        else if (unbound != NULL && baseenv_unbound != NULL) {
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
 *          Closure Values				                     *
 *          ============== 				                     *
 *                                                                           *
 *****************************************************************************/







typedef struct 
{   value_t value;           /* closure used as a value */
    const value_t *code;     /* code body */
    value_env_t *env;	     /* environment */
} value_closure_t;





#define value_closure_value(closure) (&(closure)->value)






static void
value_closure_markver(const value_t *value, int heap_version)
{   value_closure_t *closure = (value_closure_t *)value;
    value_mark_version((value_t *)closure->code, heap_version);
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
    if (NULL != closure->env)
    {   bool relative_to_root = FALSE;
	n += value_env_print_relative(out, root, value_env_value(closure->env),
				      &relative_to_root);
        if (NULL == closure->code)
	{   if (relative_to_root)
                n += outchar_printf(out, ":root");
        } else {
   	    if (relative_to_root)
                n += outchar_printf(out, ":");
            else
                n += outchar_printf(out, "::");
        }
    }
    if (NULL != closure->code)
        n += value_print(out, root, closure->code);
    /*n += outchar_printf(out, ")");*/
    
    return (int)n;
}








static int /* <0 for less than, ==0 for equal, >0 for greater */
value_closure_compare(const value_t *v1, const value_t *v2)
{   return -1; /* TODO: implement me */
}






static value_t *
value_closure_init(value_closure_t *closure, const value_t *code,
		   value_env_t *env, value_delete_fn_t *delete)
{   value_t *initval;
    closure->code = code;
    closure->env = env;
    initval = value_init(&closure->value, type_closure, &value_closure_print,
		         &value_closure_compare, delete,
			 &value_closure_markver);
    value_unlocal(value_env_value(env));
    value_unlocal(code);
    return initval;
}







extern value_t *
value_closure_new(const value_t *code, value_env_t *env)
{   value_closure_t *closure = NULL;

    if (NULL == code ||
	value_type_equal(code, type_code) ||
	value_type_equal(code, type_cmd) ||
	value_type_equal(code, type_func))
    {   closure = (value_closure_t *)FTL_MALLOC(sizeof(value_closure_t));
	if (NULL != closure)
	    value_closure_init(closure, code, env, &value_closure_delete);
    } else
    {   fprintf(stderr, "%s: closure code has wrong type - "
		 "type is %s, expected code or builtin\n",
	         codeid(), value_type_name(code));
    }

    if (NULL == closure)
    {   value_unlocal(code);
	value_env_unlocal(env);
	return (value_t *)NULL;
    } else
	return value_closure_value(closure);
}






#if 0 != DEBUGVALINIT(1+)0
#define value_closure_new(code, env)			\
    value_info((value_closure_new)(code, env), __LINE__)
#endif






extern value_t *
value_closure_copy(const value_t *oldclosureval)
{   value_closure_t *closure = NULL;

    if (value_istype(oldclosureval, type_closure))
    {   value_closure_t *oldclosure = (value_closure_t *)oldclosureval;
	
	closure = (value_closure_t *)FTL_MALLOC(sizeof(value_closure_t));
	
	if (NULL != closure)
	{   value_closure_init(closure, /*code*/NULL,
			       value_env_copy(oldclosure->env),
			       &value_closure_delete);
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
    if (NULL != dir)
    {   value_closure_t *closure = (value_closure_t *)value;

	ok = TRUE;
	if (NULL == closure->env)
	    closure->env = value_env_new();
	
	if (NULL != closure->env)
	    value_env_pushdir(closure->env, dir, env_end);
    }
    return ok;
}





extern bool
value_closure_pushenv(const value_t *value, value_env_t *env, bool env_end)
{   bool ok = FALSE;
    if (NULL != env)
    {   value_closure_t *closure = (value_closure_t *)value;

	ok = TRUE;
	if (NULL == closure->env)
	{   closure->env = value_env_copy(env);
	} else
	    ok = value_env_pushenv(closure->env, env, env_end);
    }
    IGNORE(else printf("%s: NULL value onto closure failed\n", codeid());)
	
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







#if 0 != DEBUGVALINIT(1+)0
#define value_closure_bind(closure, val)			\
    value_info((value_closure_bind)(closure, val), __LINE__)
#endif













/*****************************************************************************
 *                                                                           *
 *          Transfer Functions					             *
 *          ==================					             *
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
	IGNORE(if (NULL == *out_dir)
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
 *          Coroutine Values					             *
 *          ================					             *
 *                                                                           *
 *****************************************************************************/








struct value_coroutine_s
{   value_t value;
    linesource_t source;
    dir_stack_t *env;
    dir_t *root;
    dir_t *opdefs;
    suspend_fn_t *sleep;
    int errors;
    bool echo_cmds;
} /* value_coroutine_t, parser_state_t */;





#define value_coroutine_value(co) (&(co)->value)





static void
value_coroutine_delete(value_t *value)
{   parser_state_t *state = (parser_state_t *)value;
    /* close source down */
    if (NULL != state)
        FTL_FREE(state);
}




    
static void
value_coroutine_markver(const value_t *value, int heap_version)
{   parser_state_t *state = (parser_state_t *)value;
    value_mark_version((value_t *)state->env, heap_version);
    value_mark_version((value_t *)state->root, heap_version);
    value_mark_version((value_t *)state->opdefs, heap_version);
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
	outlen += outchar_printf(out, "coroutine.{in=%s:%d}",
	                         parser_source(state), parser_lineno(state));
	/*outlen += value_print(out, root, dir_stack_value(state->env));
	  outlen += outchar_putc(out, '}');*/
    }
    return outlen;
}






static void
value_coroutine_init(parser_state_t *state, value_delete_fn_t *delete,
		     dir_stack_t *env, dir_t *root, dir_t *opdefs)
{   value_init(&state->value, type_coroutine, &value_coroutine_print,
	       /*compare*/NULL, delete, &value_coroutine_markver);
    linesource_init(&state->source);
    state->env = env;
    state->echo_cmds = FALSE;
    state->root = root;
    state->opdefs = opdefs;
    state->sleep = &sleep_ms;
    state->errors = 0;
    parser_env_push(state, root, /*env_end*/FALSE);
    value_unlocal(dir_value(root));
    value_unlocal(dir_stack_value(env));
}





    
extern value_coroutine_t *
value_coroutine_new(dir_t *root, dir_stack_t *env, dir_t *opdefs)
{   parser_state_t *state = (parser_state_t *)
			    FTL_MALLOC(sizeof(parser_state_t));
    if (NULL != state)
	value_coroutine_init(state, &value_coroutine_delete,
			     env, root, opdefs);
    return state;
}








/*****************************************************************************
 *                                                                           *
 *          Parser State					             *
 *          ============					             *
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
parser_env_stack(parser_state_t *parser_state)
{   return (parser_state)->env;
}

extern dir_t *
parser_env(parser_state_t *parser_state)
{   return dir_stack_dir((parser_state)->env);
}

extern dir_t *
parser_env_copy(parser_state_t *parser_state)
{   return dir_stack_dir(dir_stack_copy((parser_state)->env));
}

extern dir_t *
parser_opdefs(parser_state_t *parser_state)
{   return (parser_state)->opdefs;
}

extern const value_t *
parser_builtin_arg(parser_state_t *parser_state, int argno)
{   return dir_get_builtin_arg(dir_stack_dir((parser_state)->env), argno);
}


extern dir_stack_pos_t
parser_env_push(parser_state_t *parser_state, dir_t *newdir, bool env_end)
{   return dir_stack_push((parser_state)->env, newdir, env_end);
}


extern bool /* ok */
parser_env_push_at_pos(parser_state_t *parser_state, dir_stack_pos_t pos,
		       dir_t *newdir, bool env_end)
{   return dir_stack_push_at_pos(pos, newdir, env_end);
}


extern void
parser_env_return(parser_state_t *parser_state, dir_stack_pos_t pos)
{   dir_stack_return((parser_state)->env, pos);
}


extern dir_stack_pos_t
parser_env_calling_pos(parser_state_t *parser_state)
{   return dir_stack_last_pos((parser_state)->env);
}


extern bool
parser_echo(parser_state_t *parser_state)
{   return (parser_state)->echo_cmds;
}


extern void
parser_echo_set(parser_state_t *parser_state, bool on)
{   (parser_state)->echo_cmds = on;
}

extern suspend_fn_t *
parser_suspend_get(parser_state_t *parser_state)
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
{   if (NULL != sink)
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
        parser_error(state, "%s '%*.s\n", msg, p-line, line);
        do {
            line = p+1;
            p  = strchr(line,'\n');
            if (p == NULL)
                parser_report_line(/*state*/NULL/*no pos*/, "%s'\n", line);
            else
                parser_report_line(/*state*/NULL/*no pos*/, "%.*s%s\n",
                                   p-line, line, n==1?"...":"");
        } while (--n > 0 && p != NULL);
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
    else switch (value_type(cmd))
    {   case type_cmd:
	    return value_cmd_help(cmd);
	    
	case type_func:
	    return value_func_help(cmd);

	case type_closure:
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

	default:
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




/* forward definitions */

extern bool parse_empty(const char **ref_line);
extern bool parse_space(const char **ref_line);





extern void
parser_collect(parser_state_t *state)
{   int heap_value;
    heap_value = value_heap_nextversion();
    /* should be able to drop the following soon */
    value_mark_version(parser_state_value(state), heap_value);
    IGNORE(value_list_locals());
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
    
    DEBUGEXPD(printf("expanding '%s'[%d]\n", phrase, len);)
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
	    {	char macname[FTL_ID_MAX+3];
		char *mac = &macname[0];
		const char *mac_r = &macname[0];
		size_t len = sizeof(macname);
		const value_t *macval = NULL;

		if (ch == '{')
		{   /* parse ${expr} */
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
		{   /* parse $name */
		    do {
			if (len > 1)
			    *mac++ = ch;
			ch = instack_getc(in);
		    } while (ch != EOF && (ch =='_' || isalnum(ch)));

		    instack_ungetc(in, ch);
		} else
		if (ch  == '@')
		{   /* parse $@<n>*/
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

		    if (!value_type_equal(macval, type_string))
		    {   charsink_string_t charbuf;
			charsink_t *sink = charsink_string_init(&charbuf);

			value_print(sink, dir_value(parser_root(state)),
				    macval);
			charsink_string_buf(sink, &body, &blen);
			IGNORE(printf("mac body: '%s'[%d]\n", body, blen);)
			macval = value_string_new(body, blen);
			charsink_string_close(sink);
		    }

		    value_string_get(macval, &body, &blen);
		    inmac = charsource_string_new(&macname[0], body, blen);
		    instack_push(in, inmac);
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
 *          Line Parsing						     *
 *          ============				                     *
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

    while (isspace(*line)) {
	line++;
    }
    IGNORE(printf("%s: %d white chars\n", codeid(), line-start););
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
parse_item(const char **ref_line, const char *delims, size_t ndelim,
	   char *buf, size_t len)
{   const char *start = *ref_line;
    const char *line = start;

    if (delims == NULL)
	delims="";

    while (*line != '\0' && !isspace(*line) &&
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
    if (ch=='_' || isalnum(ch))
	do {
	    if (len > 1)
	    {   len--;
		*buf++ = ch;
	    }
	    line++;
	} while ((ch = *line) != '\0' && (ch=='_' || isalnum(ch)) );
#else
    if (ch==BUILTIN_ARG_CH1)
	do {
	    if (len > 1)
	    {   len--;
		*buf++ = ch;
	    }
	    line++;
	} while ((ch = *line) != '\0' && isdigit(ch));
    else
    if (ch=='_' || isalpha(ch))
	do {
	    if (len > 1)
	    {   len--;
		*buf++ = ch;
	    }
	    line++;
	} while ((ch = *line) != '\0' && (ch=='_' || isalnum(ch)) );
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
		wctomb(NULL, L'\0'); /* reset state */
		len = wctomb(&mbstring[0], wch);

		if (len >= 0 && size > len && len < sizeof(mbstring))
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








/*****************************************************************************
 *                                                                           *
 *          Operator Parsing					             *
 *          ================					             *
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




static const value_t *
substitute(const value_t *code, const value_t *arg,
	   parser_state_t *state, bool try); /* forward ref */


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
} op_assoc_t;	/* associativity */


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

#define assoc_name(assoc)	  \
    ((assoc) == assoc_xfx? "xfx": \
     (assoc) == assoc_yfy? "yfy": \
     (assoc) == assoc_xfy? "xfy": \
     (assoc) == assoc_yfx? "yfx": \
     (assoc) == assoc_yf? "yf":   \
     (assoc) == assoc_xf? "xf":   \
     (assoc) == assoc_fy? "fy":   \
     (assoc) == assoc_fx? "fx": "?f?")



typedef int op_prec_t; /*< smaller values bind more tightly than larger ones */


typedef struct
{   const value_t *fn;  /*< equivalent functional form */
    op_assoc_t assoc;	/*< associativity */
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
{   const value_t *bind = substitute(monadic_fn, arg, state, /*try*/FALSE);
    
    if (NULL != bind)
	return invoke(bind, state);
    else
	return (const value_t *)NULL;
}








static const value_t *
invoke_diadic(const value_t *diadic_fn, const value_t *arg1,
	      const value_t *arg2,  parser_state_t *state)
{   const value_t *bind = substitute(diadic_fn, arg1, state, /*try*/FALSE);
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
	      !(isalpha((*ref_line)[-1]) && (isalpha((*ref_line)[0]) ||
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
    
    DEBUGOP(DPRINTF("%s: op %sfound at ...%s\n", codeid(), ok? "":"not ",
		    line);)
	
    return ok;
}
    





static bool 
parse_op_expr(const char **ref_line, op_state_t *ops, 
	      op_prec_t prec, const value_t **out_newterm)
{   /* <oparg> <op> <oparg> .... decoded correctly for precority 	    */
    /* parses the longest run of operators at the given op_prec_t level or  */
    /*     tighter.							    */
    bool ok;
    operator_t op;
    const char *line = *ref_line;
    dir_t *opdefs = NULL;     /* operator definitions at this precidence */
    const value_t *arg = NULL;

    (void)parse_space(&line);
	
    DEBUGOP(DPRINTF("%s: expr prec %d ...%s\n", codeid(), prec, line););

    if (!op_defs_get(ops, prec, &opdefs))
    {   DEBUGOP(DPRINTF("%s: lowest precidence - parse base\n", codeid()););
	ok = (*ops->parse_arg)(ref_line, ops, out_newterm);
    } else
    if (parse_op(&line, ops, opdefs, &op) && assoc_prefix(op.assoc) &&
	parse_space(&line))
    {   /* deal with prefix operators */
	DEBUGOP(DPRINTF("%s: prec %d parse %s op - parse prefix\n",
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
	DEBUGOP(DPRINTF("%s: prec %d parse lower, diadic or postfix op\n",
		        codeid(), prec););
	if (parse_op_expr(ref_line, ops, prec+1, out_newterm) &&
	    parse_space(ref_line))
	{   bool complete = FALSE;
	    bool first = TRUE;

	    ok = TRUE;
	    line = *ref_line;

            DEBUGOP(VALUE_SHOW_ST("left val: ", ops->state, *out_newterm);)
            DEBUGOP(DPRINTF("%s: prec %d got low prec left - "
			    "now parse ...%s\n",
			    codeid(), prec, line););
		
            while (ok && !complete && parse_op(&line, ops, opdefs, &op) &&
		   parse_space(&line))
	    {
              	DEBUGOP(DPRINTF("%s: prec %d parse diadic or postfix %s op\n",
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
    DEBUGOP(DIR_SHOW("op defs: ", opdefs););
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
 *          Command Values					             *
 *          ==============					             *
 *                                                                           *
 *****************************************************************************/





typedef struct value_cmd_s value_cmd_t;




struct value_cmd_s
{   value_t value;
    const char *help;
    cmd_fn_t *exec;
    const value_t *fn_exec;
} /* value_cmd_t */;





static int
value_cmd_print(outchar_t *out, const value_t *root, const value_t *value)
{   int n = 0;
    (void)root;
    if (value != NULL && value_istype(value, type_cmd))
    {   value_cmd_t *cmd = (value_cmd_t *)value;
	n += outchar_printf(out, "<cmd:%p,1>", cmd->exec);
    }
    return n;
}





static void
value_cmd_markver(const value_t *value, int heap_version)
{   value_cmd_t *cmd = (value_cmd_t *)value;
    if (NULL != cmd->fn_exec)
        value_mark_version((value_t *)cmd->fn_exec, heap_version);
}





STATIC_INLINE value_t *
value_cmd_init(value_cmd_t *cmd, cmd_fn_t *exec, const value_t *fn_exec,
	       const char *help,  value_delete_fn_t *delete)
{   value_t *initval;
    cmd->exec = exec;
    cmd->help = help;
    cmd->fn_exec = fn_exec;
    initval = value_init(&cmd->value, type_cmd, &value_cmd_print,
		         /*compare*/NULL, delete, &value_cmd_markver);
    value_unlocal(fn_exec);
    return initval;
}





static void
value_cmd_delete(value_t *value)
{   if (value != NULL)
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
        return value_cmd_init(cmd, exec, fn_exec, help, &value_cmd_delete);
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





    
/*****************************************************************************
 *                                                                           *
 *          Function Values					             *
 *          ===============					             *
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
    if (value != NULL && value_istype(value, type_func))
    {   value_func_t *func = (value_func_t *)value;
	n += outchar_printf(out, "<func:%p,%d>", func->exec, func->args);
    }
    return n;
}





STATIC_INLINE value_t *
value_func_init(value_func_t *func, func_fn_t *exec, const char *help,
		int args, void *implicit, value_delete_fn_t *delete)
{   func->exec = exec;
    func->help = help;
    func->args = args;
    func->implicit = implicit;
    return value_init(&func->value, type_func, &value_func_print,
		      /*compare*/NULL, delete, /*mark*/NULL);
}





static void
value_func_delete(value_t *value)
{   if (value != NULL)
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
        return value_func_init(func, exec, help, args, implicit,
			       &value_func_delete);
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
 *          LHV Function Values					             *
 *          ===================					             *
 *                                                                           *
 *****************************************************************************/





typedef struct value_func_lhv_s value_func_lhv_t;

struct value_func_lhv_s
{   value_func_t fn;
    const value_t *lv_name;
} /* value_func_lhv_t */;





static int
value_func_lhv_print(outchar_t *out, const value_t *root, const value_t *value)
{   int n = 0;
    (void)root;
    if (value != NULL && value_istype(value, type_func))
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
    value_mark_version((value_t *)func->lv_name, heap_version);
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
    IGNORE(DIR_SHOW_ST("pre-return dir of @fn: ", state, dir);)
	
    stack = value_env_dir_stack((value_env_t *)dir);
    func = (value_func_lhv_t *)codeval;
    /* return to the calling envrionment */
    dir_stack_return(stack, dir_stack_last_pos(stack)); /* argument */
    dir_stack_return(stack, dir_stack_last_pos(stack)); /* infodir */
    IGNORE(DIR_SHOW_ST("return dir of @fn: ", state, dir);
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
value_func_lhv_init(value_func_lhv_t *func, value_delete_fn_t *delete,
		    const value_t *lv_name)
{   value_t *initval;
    func->lv_name = lv_name;
    func->fn.exec = &fn_lhv_setval;
    func->fn.help = "- assign value to name";
    func->fn.args = 1;
    initval = value_init(&func->fn.value, type_func, &value_func_lhv_print,
		         /*compare*/NULL, delete, &value_func_lhv_markver);
    value_unlocal(lv_name);
    return initval;
}




extern value_t *
value_func_lhv_new(const value_t *lv_name)
{   value_func_lhv_t *func = (value_func_lhv_t *)
	                     FTL_MALLOC(sizeof(value_func_lhv_t));

    if (NULL != func)
        return value_func_lhv_init(func, &value_func_delete, lv_name);
    else
	return NULL;
}







    
/*****************************************************************************
 *                                                                           *
 *          Modules 					                     *
 *          =======					                     *
 *                                                                           *
 *****************************************************************************/







/* Standard operator precidences - NB: these must used and be consecutive */

#define OP_PREC_OR    0
#define OP_PREC_AND   1
#define OP_PREC_NOT   2
#define OP_PREC_CMP   3
#define OP_PREC_SHIFT 4
#define OP_PREC_SIGN  5
#define OP_PREC_SUM   6
#define OP_PREC_PROD  7




STATIC_INLINE void
value_mod_cmd_create(const value_t *helpstrval, value_t *closure, int args)
{   dir_t *helpenv = dir_id_new();
    int i;
    value_t *argnext = NULL;

    dir_string_set(helpenv, BUILTIN_HELP, helpstrval);

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
	    dir_string_set(opdefn, OP_FN, fn);
	    dir_string_set(opdefn, OP_ASSOC, value_int_new(assoc));
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

    if (parse_id(ref_line, &cmdname[0], sizeof(cmdname)))
	cmd = dir_string_get(dir, cmdname);

    if (cmd != NULL)
    {   *out_cmd = cmd;
	return TRUE;
    } else
    {   *ref_line = start;  /* return to initial value */
        return FALSE;
    }
}



extern bool
mod_parse_argv(dir_t *dir, const char ***ref_argv, int *ref_argn,
               const value_t **out_val)
{   const value_t *cmd = NULL;
    
    if (*ref_argn > 0) 
    {   const char *cmdname = (*ref_argv)[0];
        cmd = dir_string_get(dir, cmdname);
    }

    if (cmd != NULL)
    {   *out_val = cmd;
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
 *          Boolean Values				                     *
 *          ============== 				                     *
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
value_bool_init(value_int_t *no, bool val, value_delete_fn_t *delete)
{   no->number = (number_t)val;
    return value_init(&no->value, type_int, &value_int_print,
		      &value_int_compare, delete, /*mark*/NULL);
}


static void
values_bool_init(void)
{   value_bool_init(&value_int_true,  TRUE, /* delete */NULL);
    value_bool_init(&value_int_false, FALSE, /* delete */NULL);
    
}



#else



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

    bfnval = value_func_init(boolfunc, boolfn, help, 1,
			     /*implicit*/NULL, /*delete*/NULL);
    (void)value_env_init(boolargenv, /*delete*/NULL);
    bval = value_closure_init(closure, bfnval, boolargenv,
			       /*delete*/NULL);
    bhelpstr = value_cstring_init(boolhelpstr, help, strlen(help),
				  /*delete*/NULL);

    bval->print = &value_bool_print;
    bval->compare = &value_bool_cmp;
    
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
    
    if (val == NULL || val == &value_null)
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
    value_bool_init(&value_closure_true, &value_func_true,
		    &value_argenv_true, &value_helpstr_true,
		    &fn_true,  "TRUE value");
    value_bool_init(&value_closure_false, &value_func_false,
		    &value_argenv_false, &value_helpstr_false,
		    &fn_false,  "FALSE value");
}




#endif






/*****************************************************************************
 *                                                                           *
 *          Value Parsing						     *
 *          =============				                     *
 *                                                                           *
 *****************************************************************************/










/* forward references */

static bool
parse_substitution(const char **ref_line, parser_state_t *state,
		   const value_t **out_val);

extern bool
parse_index_expr(const char **ref_line, parser_state_t *state,
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
	




/* If successful this function returns a value_t in out_val which is
 * constructed from a value_env_t.
 *
 * This function may cause a garbage collection
 */
/**/extern bool
parse_env(const char **ref_line, parser_state_t *state,
	  const value_t **out_val)
{   /* after '[': [<index>=<substitution>,]* [<index>,]* ']' */
    bool ok = FALSE;
    value_env_t *env = NULL;
    const value_t *index = NULL;
    bool unbound = FALSE;

    DEBUGTRACE(DPRINTF("env: %s\n", *ref_line);)
    parse_space(ref_line);
    
    ok = TRUE;
    if (parse_index_expr(ref_line, state, &index) &&
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
		 parse_index_expr(ref_line, state, &index) &&
		 parse_space(ref_line));

        IGNORE(DPRINTF("env %sunbound%s: %s\n",
                       ok? "":"(FAIL) ",unbound?"":" (NONE)", *ref_line););

	if (ok && unbound)
	{   if (NULL == env)
		env = value_env_new();
	    do {   
		pos = value_env_pushunbound((value_env_t *)env,
					     pos, (value_t *)index);
	    } while (parse_key(ref_line, ",") && parse_space(ref_line) &&
		     (ok = parse_index_expr(ref_line, state, &index)) &&
		     parse_space(ref_line));
	}
    }
    if (ok)
    {   if (NULL == env)
	    env = value_env_new();  /* empty environment */
	*out_val = (const value_t *)env;
    }
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

			IGNORE(fprintf(stderr, "set index %d to ", index);
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
	    DEBUGSERIES(DPRINTF("up to: %s\n", *ref_line);)
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
    return ok;
}







/* This function may cause a garbage collection */
static bool
parse_index(const char **ref_line, parser_state_t *state,
	    const value_t **out_val)
{   bool ok = FALSE;
    const char *line = *ref_line;
    number_t n;

    if (parse_key(&line, "["))
    {   ok = parse_env(&line, state, out_val) &&
	     parse_key_always(&line, state, "]");
    } else
    if (parse_key(&line, "<"))
    {   ok = parse_vecarg(&line, state, ",", out_val) &&
	     parse_key_always(&line, state, ">") && parse_space(&line);
    } else
    if (parse_int_val(&line, &n))
    {   *out_val = value_int_new(n);
	IGNORE(printf("%s: parsed int %ld\n", codeid(), n);)
        ok = TRUE;
    } else
    {   char id[FTL_ID_MAX];
	size_t id_filled = 0;
	if (parse_string(&line, &id[0], sizeof(id), &id_filled))
	{   IGNORE(printf("%s: parsed string '%s'\n", codeid(), &id[0]);)
	    *out_val = value_string_new(&id[0], id_filled);
	    ok = TRUE;
	} else
	{   if (parse_id(&line, &id[0], sizeof(id)) && parse_space(ref_line))
	    {   *out_val = value_string_new_measured(&id[0]);
		IGNORE(printf("%s: parsed ID %s\n", codeid(), &id[0]);)
		ok = TRUE;
	    }
	}
    }
    if (ok)
    {
	*ref_line = line;
    }
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
	    IGNORE(printf("closure has empty env dir\n");)
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







extern const value_t *
dir_dot_lookup(dir_t *dir, const value_t *name)
{   if (name == NULL)
	return NULL;
    else
    switch (value_type(name))
    {   case type_closure:
	{   const value_t *unbound;
	    const value_t *code;
	    dir_t *ixdir = NULL;
	    (void)value_closure_get(name, &code, &ixdir, &unbound);
	    if (NULL != ixdir)
	        return dir_dot_lookup(dir, dir_value(ixdir));
	    else
		return name;
	}
	    
	case type_dir:
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
	}
            
	default:
	    return dir_get(dir, name);
    }
}


    




/* This function may cause a garbage collection */
/**/extern bool
parse_index_path(const char **ref_line, parser_state_t *state,
		 dir_t *indexed,
		 dir_t **out_parent, const value_t **out_id)
{   /* we have parsed <parent>., or we know the parent -
       we now need to parse <id>[.<id>]* */
    bool ok;
    dir_t *parent = indexed;
    const value_t *new_id = NULL;

    ok = parse_index_expr(ref_line, state, &new_id) && parse_space(ref_line);
    IGNORE(printf("%s: index was %s ...%s\n",
	          codeid(), ok? "before": "not at", *ref_line);)
    
    while (ok && parse_dot(ref_line) &&
	   parse_space(ref_line))
    {   /* get parent from old parent */
	const value_t *ival = dir_dot_lookup(parent, new_id);

	if (NULL == ival)
	{   parser_error(state, "undefined index symbol '");
	    parser_value_print(state, new_id);
	    /*printf("' in %s\n", value_type_name(parent));*/
	    fprintf(stderr, "'\n");
	    IGNORE(DIR_SHOW_ST("parent env: ", state, parent);)
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

    return ok;
}






/* This function may cause a garbage collection */
/**/extern bool
parse_lvalue(const char **ref_line, parser_state_t *state,
	     dir_t *indexed,
	     dir_t **out_parent, const value_t **out_id)
{   if (parse_space(ref_line) && parse_dot(ref_line))
    {   dir_t *localdir = dir_stack_top(parser_env_stack(state));
	return parse_index_path(ref_line, state, localdir, out_parent, out_id);
    } else
	return parse_index_path(ref_line, state, indexed, out_parent, out_id);
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
        
        *out_sourcepos = parser_source(state);
        *out_lineno = parser_lineno(state);

        /* we've always consumed the whole of the current line - so
           report the last one, as well as we can */
        if (*out_lineno > 0)
            (*out_lineno)--;

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

    DEBUGTRACE(DPRINTF("base: %s\n", *ref_line);)
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
		DEBUGENV(DIR_SHOW("env: ", parser_env(state));)
		ok = FALSE;
	    }
	    *out_val = value_nl(v);
	} else
        if (!parse_string_expr(ref_line, state,
			       &strbuf[0], sizeof(strbuf), out_val))
	    ok = FALSE;
    }

    DEBUGTRACE(DPRINTF("base %s: %s\n", ok? "OK":"FAILED", *ref_line);)
        
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
                (void)value_env_pushdir(*ref_env, def_env, /*env_end*/FALSE);
        }
        if (new_is_env_t)
            /* we know the new_env is a value_env_t */
            (void)value_env_pushenv(*ref_env, (value_env_t *)new_env,
                                    /*env_end*/FALSE);
        else
        {   /* new_env is a dir_t */
            dir_t *new_dir = (dir_t *)new_env;
            /* need a "copy" of dir to get a free link ptr */
            (void)value_env_pushdir(*ref_env,
                                    value_env_newpushdir(new_dir, FALSE),
                                    /*env_end*/FALSE);
        }
    }
    /* *ref_env will not normally be set to NULL now */
    return ok;
}







/* This function may cause a garbage collection */
static bool
parse_closure(const char **ref_line, parser_state_t *state,
	      const value_t **out_val)
{   /* [[<base>|<code>]:[:]]*[<base>|<code>] | <base> */
    bool ok;
    bool is_closure = TRUE;
    bool lhs_is_env = FALSE;  /* i.e. lhs was '['...']' & value_env_t */
    bool inherit = TRUE; /* ':' not '::' */
    /* if we can express the result as a directory we use these: */
        const value_t *lhs = NULL; 
        bool lhs_is_dir_nonenv = FALSE;  /* i.e. lhs is dir */
    /* otherwise if lhs_is_dir_nonenv is FALSE we use these */
        value_env_t *env = NULL; /* we are constructing this */
        const value_t *code = NULL; /* we construct this too */
        

    DEBUGTRACE(DPRINTF("env expr: %s\n", *ref_line);)
    
    ok = parse_base_env(ref_line, state, &lhs, &lhs_is_env);
    /* lhs_is_env will be true if [<id-dir>] is parsed - and lhs will be a
     * value_env_t (otherwise it could be any value_t) */

    IGNORE(DPRINTF("env expr base %s: %s\n", ok? "OK":"FAILED", *ref_line);)
        
    if (ok) {
        if (value_type_equal(lhs, type_code) ||
            value_type_equal(lhs, type_func) ||
            value_type_equal(lhs, type_cmd))
            code = lhs;
        else if (value_type_equal(lhs, type_closure)) {
            const value_closure_t *closure = (const value_closure_t *)lhs;
            code = closure->code;
            env = closure->env;
        } else if (lhs_is_env)
            env = (value_env_t *)lhs;
        else {
            is_closure = lhs_is_dir_nonenv = value_type_equal(lhs, type_dir);
        }
    }        

    if (!is_closure)
        *out_val = lhs;
    else
    {
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

                bool rhs_is_code    = value_type_equal(rhs, type_code) ||
                                      value_type_equal(rhs, type_cmd) ||
                                      value_type_equal(rhs, type_func);
                bool rhs_is_closure = value_type_equal(rhs, type_closure);
                bool rhs_is_dir     = value_type_equal(rhs, type_dir);
                bool promote_to_env = inherit || rhs_is_closure ||
                                      rhs_is_dir ||
                                      code != NULL;
                const value_t *rhs_code = NULL;

                if (promote_to_env && lhs_is_dir_nonenv)
                {
                    /* only an environment can have state added to it */
                    env = (value_env_t *)value_env_newpushdir((dir_t *)lhs,
                                                              /*env_end*/FALSE);
                    lhs_is_dir_nonenv = FALSE;
                    lhs_is_env = TRUE;
                }

                if (inherit)
                {   dir_t *def_env = parser_env_copy(state);
                    if (NULL != def_env)
                        (void)value_env_pushdir(env, def_env, /*env_end*/FALSE);
                }

                if (rhs_is_code)
                    rhs_code = rhs;
                else

                if (rhs_is_closure || rhs_is_dir)
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
            IGNORE(DPRINTF("env expr: %sDIR - env %sNULL code %sNULL\n",
                           lhs_is_dir_nonenv? "": "not ",
                           env == NULL? "": "not ", code == NULL? "": "not "););
            if (lhs_is_dir_nonenv)
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

                DEBUGENV(DPRINTF("env expr: closure unbound %sNULL "
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
    
    DEBUGTRACE(DPRINTF("env expr %s: %s\n", ok?"OK":"FAIL", *ref_line);)
    
    return ok;
}









/* This function may cause a garbage collection */
static bool
parse_index_value(const char **ref_line, parser_state_t *state,
		  dir_t *indexed, const value_t **out_val)
{   /* we have parsed <indexed>. - we now need to parse [<id>.]*<id> */
    /* (where parent = [<id>.]*)                                     */
    dir_t *parent;
    const value_t *id;
    bool ok = FALSE;
    
    if (parse_index_path(ref_line, state, indexed, &parent, &id))
    {   *out_val = dir_dot_lookup(parent, id);
	if (NULL != *out_val)
	    ok = TRUE;
	else
	{   parser_error(state, "index symbol undefined in parent '");
	    parser_value_print(state, id);
	    IGNORE(printf("' in %s at %p\n",
		          value_type_name(dir_value(parent)), parent);)
	    fprintf(stderr, "'\n");
	    IGNORE(DIR_SHOW("parent env: ", parent);)
	}
    }
    return ok;
}








/* This function may cause a garbage collection */
static bool
parse_index_lhvalue(const char **ref_line, parser_state_t *state,
		    dir_t *indexed, const value_t **out_val)
{   /* we have parsed @<indexed>. - we now need to parse [<id>.]*<id> */
    /* (where parent = <indexed>.[<id>.]*)                            */
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
            value_env_pushdir(env, parent, /*env_end*/FALSE);
	    get = value_closure_new(code, env);
	    *out_val = &value_null;
	    dir_set(infodir, name, get);
	    (void)value_closure_pushdir(closure, parent, /*env_end*/FALSE);
	    (void)value_closure_pushdir(closure, infodir, /*env_end*/FALSE);
	    IGNORE(DIR_SHOW_ST("@parent: ", state, parent);
	           printf("at %p\n", parent);)
	    (void)value_closure_pushunbound(closure, NULL,
					   value_string_new_measured(argname));
	    *out_val = closure;
	    ok = TRUE;
	}
    }
    return ok;
}








/* This function may cause a garbage collection */
extern bool
parse_retrieval(const char **ref_line, parser_state_t *state,
		const value_t **out_val)
{   /* [@<index>|<closure>][.<index>]* */
    bool ok = FALSE;
    bool need_index = FALSE;
    bool is_local;
    bool is_lhv = FALSE;

    DEBUGTRACE(DPRINTF("lookup: %s\n", *ref_line););

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
	    {   IGNORE(printf("%s: ..>%s\n", codeid(), *ref_line);
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
    return parse_opterm(ref_line, state, &parse_retrieval_base, state->opdefs,
			out_val);
}








/* if 'try' is set the substitution will not fail if there are not enough
   unbound symbols for the substitution (the code value itself will be
   returned)
*/
static const value_t *
substitute_simple(const value_t *code, const value_t *arg,
	          parser_state_t *state, bool try, bool *ref_auto_eval)
{   const value_t *val = NULL;

    *ref_auto_eval = FALSE;
    
    if (NULL != code)
    {   if (value_type_equal(code, type_closure))
	{   const value_t *codeval = NULL;
	    dir_t *dir = NULL;
	    const value_t *unbound = NULL;

	    value_closure_get(code, &codeval, &dir, &unbound);
	    if (NULL == unbound)
            {   if (try)
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






static const value_t *
substitute(const value_t *code, const value_t *arg,
	   parser_state_t *state, bool try)
{   bool auto_eval = FALSE;
    const value_t *val = substitute_simple(code, arg, state, try, &auto_eval);
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
    int lineno;
    charsource_lineref_t line;
    
    if (NULL != code)
    {   if (value_type_equal(code, type_code))
	{   const char *buf;
	    size_t len;
		
	    DEBUGMOD(DPRINTF("%s: invoke - code\n", codeid());)
	    value_code_place(code, &placename, &lineno);
	    linesource_push(parser_linesource(state),
			    charsource_lineref_init(&line, /*delete*/NULL,
						    placename, lineno, &buf));
	    value_code_buf(code, &buf, &len);
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

   	            DEBUGMOD(DPRINTF("%s: invoke - code closure\n", codeid());)
		    value_code_buf(codeval, &buf, &len);
		    /* we will garbage collect the discarded value */
		    if (NULL == dir)
			dir = dir_id_new();
		    pos = parser_env_push(state, dir, /*env_end*/TRUE);
	            value_code_place(codeval, &placename, &lineno);
		    linesource_push(parser_linesource(state),
				    charsource_lineref_init(&line,
							    /*delete*/NULL,
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
		    
   	            DEBUGMOD(DPRINTF("%s: invoke - cmd closure\n", codeid());)
		    if (value_type_equal(boundarg, type_code))
		    {   (void)value_code_buf(boundarg, &buf, &len);
		    } else
		        (void)value_string_get(boundarg, &buf, &len);
                        /* checks boundarg is a string */
		    if (buf != NULL)
		    {   charsink_string_t expandline;
			charsink_t *expandsink =
			    charsink_string_init(&expandline);
			parser_expand(state, expandsink, buf, len);
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

		    IGNORE(
			printf("top : %p\n", parser_env_stack(state)->stack);
		        DIR_SHOW("push: ", dir);
		    )
		    pos = parser_env_push(state, dir, /*env_end*/TRUE);
   	            DEBUGMOD(DPRINTF("%s: invoke - fn closure with %sreturn\n",
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
	{   parser_error(state, "%s expression not executable:\n",
			  value_type_name(code));
	    parser_value_print(state, code);
	    fprintf(stderr, "\n");
	    val = NULL;
	}
	value_unlocal(code);
    }

    return val;
}








/* This function may cause a garbage collection */
static bool    
parse_substitution_args(const char **ref_line, parser_state_t *state,
			const value_t **ref_val)    
{   /* '!'* [<retrieval> '!'*]* */
    bool ok = TRUE;
    const value_t *newarg = NULL;

    while (ok && parse_pling(ref_line) && parse_space(ref_line))
	*ref_val = invoke(*ref_val, state);
    
    while (ok /* && parse_key(ref_line, "~") && parse_space(ref_line) */
	   && parse_operator_expr(ref_line, state, &newarg) &&
	   parse_space(ref_line))
    {   const value_t *code = *ref_val;
	
	if (ok)
	{   *ref_val = substitute(code, newarg, state, /*try*/FALSE);
	    
	    if (NULL == *ref_val)
		ok = FALSE;
	    
	    while (ok && parse_pling(ref_line) && parse_space(ref_line))
		*ref_val = invoke(*ref_val, state);
	}
    }

    if (NULL == *ref_val)
	*ref_val = &value_null;

    return ok;
}






/* This function may cause a garbage collection */
static bool    
parse_substitution_argv(const char ***ref_argv, int *ref_argn,
                        parser_state_t *state, const value_t **ref_val)    
{   /* '!'* [<retrieval> '!'*]* */
    bool ok = TRUE;
    const value_t *newarg = NULL;
    const char *line = (*ref_argv)[0];
    bool complete = FALSE;
    bool pling = FALSE;
            
    while (ok && (
           (pling = (*ref_argn > 0 &&
                     parse_pling(&line) && parse_empty(&line))) ||
           (value_type_equal(*ref_val, type_closure) &&
                NULL == value_closure_unbound(*ref_val))))
    {   if (pling)
        {   (*ref_argv)++;
            (*ref_argn)--;
            line = (*ref_argv)[0];
        } else
            complete = TRUE;
        *ref_val = invoke(*ref_val, state);
    }
    
    while (ok && *ref_argn > 0 && !complete &&
	   parse_operator_expr(&line, state, &newarg) &&
	   parse_empty(&line))
    {   const value_t *code = *ref_val;
	
	(*ref_argv)++;
        (*ref_argn)--;
        line = (*ref_argv)[0];
        
        if (ok)
	{   
            *ref_val = substitute(code, newarg, state, /*try*/FALSE);
	    
	    if (NULL == *ref_val)
		ok = FALSE;
	    
	    while (ok && (
                   (pling = (*ref_argn > 0 &&
                             parse_pling(&line) && parse_empty(&line))) ||
                   (value_type_equal(*ref_val, type_closure) &&
                       NULL == value_closure_unbound(*ref_val))))
            {   if (pling)
                {   (*ref_argv)++;
                    (*ref_argn)--;
                    line = (*ref_argv)[0];
                } else
                    complete = TRUE;
                
		*ref_val = invoke(*ref_val, state);
            }
	}
    }

    if (NULL == *ref_val)
	*ref_val = &value_null;

    return ok;
}






/* This function may cause a garbage collection */
static bool
parse_substitution(const char **ref_line, parser_state_t *state,
		   const value_t **out_val)
{   /* [<retrieval> '!'*]* */

    DEBUGTRACE(DPRINTF("substitute: %s\n", *ref_line);)
	
    if (parse_operator_expr(ref_line, state, out_val) && parse_space(ref_line))
    {   IGNORE(DPRINTF("substitute args: %s\n", *ref_line););
	return parse_substitution_args(ref_line, state, out_val);
    } else
    {   *out_val = &value_null;
	return FALSE;
    }
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

    DEBUGTRACE(DPRINTF("expr: %s\n", *ref_line);)
    DEBUGENV(DIR_SHOW("expr env: ", parser_env(state));)
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
    return ok;
}



/* This function may cause a garbage collection */
static bool
parse_cmdlist(const char **ref_line, parser_state_t *state,
	      const value_t **out_val)
{   /* <expr>[;[<expr>]]* */
    /* ; discards the previous value */
    bool ok;

    parse_space(ref_line);
    
    DEBUGTRACE(DPRINTF("cmdlist: %s\n", *ref_line);)
    
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
                                 
    return ok;
}

    







/*****************************************************************************
 *                                                                           *
 *          Signal Handling                                                  *
 *          ===============                                                  *
 *                                                                           *
 *****************************************************************************/




static bool exiting = FALSE;


#ifdef EXIT_ON_CTRL_C

#ifdef __unix__

typedef void interrupt_handler_fn(int signal);
typedef interrupt_handler_fn *interrupt_state_t;



static void interrupt_handler(int signal)
{
   exiting = TRUE;
}



static bool interrupt_handler_init(interrupt_state_t *out_old_handler)
{   interrupt_handler_fn *old;
    
    exiting = FALSE;
    
    old = signal(SIGINT, &interrupt_handler);
    
    IGNORE(fprintf(stderr, "%s: installing interrupt handler %p -> last was "
                   "%p%s%s%s\n",
                   codeid(), &interrupt_handler, old,
                   old==SIG_ERR?" (ERR)":"", old==SIG_DFL?" (DFL)":"",
                   old==SIG_IGN?" (IGN)":"");
    );
    if (old == SIG_ERR)
        return FALSE;
    else {
        if (out_old_handler != NULL)
            *out_old_handler = old;
        return TRUE;
    }
}


static void interrupt_handler_end(interrupt_state_t old_handler)
{
    interrupt_handler_fn *newi;
    IGNORE(fprintf(stderr, "%s: installing interrupt returned to %p\n",
                   codeid(), old_handler);
           fflush(stderr);
    );
    newi = signal(SIGINT, old_handler);
    IGNORE(fprintf(stderr, "%s: installing old interrupt complete - "
                   "was %p%s%s%s\n",
                   codeid(), newi,
                   newi==SIG_ERR?" (ERR)":"", newi==SIG_DFL?" (DFL)":"",
                   newi==SIG_IGN?" (IGN)":""););
    exiting = FALSE;
}


#endif /* __unix__ */


#ifdef _WIN32

typedef PHANDLER_ROUTINE interrupt_handler_fn;
typedef interrupt_handler_fn *interrupt_state_t;



BOOL WINAPI interrupt_handler(DWORD dwCtrlType)
{
   exiting = TRUE;
   return TRUE/*handled*/;
}



static bool interrupt_handler_init(interrupt_state_t *out_old_handler)
{   BOOL ok;
    
    exiting = FALSE;
    
    ok = SetConsoleCtrlHandler(&interrupt_handler, /*Add*/TRUE);
    
    IGNORE(fprintf(stderr, "%s: installing interrupt handler %p\n",
                   codeid(), &interrupt_handler););
    if (ok)
        return FALSE;
    else {
        if (out_old_handler != NULL)
            *out_old_handler = NULL;/*not used in windows */
        return TRUE;
    }
}


static void interrupt_handler_end(interrupt_state_t old_handler)
{
    BOOL ok;
    IGNORE(fprintf(stderr, "%s: installing interrupt returned to %p\n",
                   codeid(), old_handler); fflush(stderr););
    ok = SetConsoleCtrlHandler(&interrupt_handler, /*Add*/FALSE);
    IGNORE(fprintf(stderr, "%s: installing old interrupt complete\n",
                   codeid()););
    exiting = FALSE;
}

#endif /* _WIN32 */

#else


/* no interrupt handling */

typedef void *interrupt_state_t;
#define interrupt_handler_init(out_old_handler) ((void)(out_old_handler),TRUE)
#define interrupt_handler_end(old_handler)



#endif /* EXIT_ON_CTRL_C */




static void interrupt_ignore(void)
{   exiting = FALSE;
}
    


static bool interrupt(void)
{   if (exiting) {
        fprintf(stdout, "%s: Ctrl-C - will exit\n", codeid());
        fflush(stdout);
    }
    return exiting;
}





/*****************************************************************************
 *                                                                           *
 *          Command Line Interpreter					     *
 *          ========================					     *
 *                                                                           *
 *****************************************************************************/








/* forward reference */
static const value_t *
mod_exec(const char **ref_line, parser_state_t *state);

/* forward reference */
static const value_t *
mod_execv(const char ***ref_argv, int *ref_argn, const char *execpath,
          parser_state_t *state);






/* This function may cause a garbage collection */
static const value_t *
mod_invoke_cmd(const char **ref_line, const value_t *value,
	       parser_state_t *state)
{   DEBUGTRACE(DPRINTF("mod exec: %s %s\n",
		       value_type_name(value), *ref_line);)
    DEBUGENV(VALUE_SHOW("mod exec env: ", dir_value(parser_env(state)));)

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
	    DEBUGMOD(DPRINTF("%s: invoke cmd closure\n", codeid());)
	    value = mod_invoke_cmd(ref_line, code, state);
            return value;
	} else
	{   if (parse_substitution_args(ref_line, state, &value))
	    {   DEBUGMOD(DPRINTF("%s: invoke non-cmd closure\n", codeid());)
		return value_nl(invoke(value, state));
	    } else
	    {   parser_error(state, "error in parsing closure arguments\n");
		return &value_null;
	    }
	}
    } else
    if (value_type_equal(value, type_cmd))
    {   value_cmd_t *cmd = (value_cmd_t *)value;
	DEBUGMOD(DPRINTF("%s: invoke direct cmd\n", codeid());)
	return (*cmd->exec)(ref_line, value, state);
    } else
    if (value_type_equal(value, type_code))
    {   const char *start = *ref_line;

	if (parse_space(ref_line) && parse_empty(ref_line))
	{   const char *buf;
	    size_t len;

	    DEBUGMOD(DPRINTF("%s: invoke direct code\n", codeid());)
	    value_code_buf(value, &buf, &len);
	    IGNORE(printf("executing: '%s'[%d]\n", buf, len);)
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
	{   DEBUGMOD(DPRINTF("%s: invoke direct function\n", codeid());)
	    return invoke(value, state);
        } else
        {   parser_error(state, "error in parsing closure arguments\n");
	    return &value_null;
	}
    } else
    if (value_type_equal(value, type_dir))
    {   dir_stack_pos_t pos = parser_env_push(state, (dir_t *)value,
					      /*env_end*/FALSE);
	IGNORE(DPRINTF("%s: push enclosing directory %p\n", codeid(), value);)
	DEBUGMOD(DPRINTF("%s: push enclosing directory\n", codeid());)
        value = mod_exec(ref_line, state);
	/* return to external environment */
	(void)parser_env_return(state, pos);
	DEBUGMOD(DPRINTF("%s: popped enclosing directory\n", codeid());)
	return value;
    }
    else
	return value;
}







static size_t
argv_to_string(const char ***ref_argv, int *ref_argn, char *buf, size_t len)
{  char *bufstart = buf;
   bool complete = *ref_argn <= 0;
   
   while (!complete)
   {   const char *next = (*ref_argv)[0];
       size_t n;
       
       (*ref_argv)++;
       (*ref_argn)--;
       complete = *ref_argn <= 0;

       if (0 == strcmp(next, ","))
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
                  

   






/* This function may cause a garbage collection */
static const value_t *
mod_invoke_argv(const char ***ref_argv, int *ref_argn, const char *execpath,
                const value_t *value, parser_state_t *state)
{   DEBUGTRACE(DPRINTF("mod argv exec: %s [%d]\n",
		       value_type_name(value), *ref_argn););
    DEBUGENV(VALUE_SHOW("mod argv exec env: ", dir_value(parser_env(state))););

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
	    DEBUGMOD(DPRINTF("%s: invoke cmd closure\n", codeid()););
            value = mod_invoke_argv(ref_argv, ref_argn, execpath, code, state);
            return value;
	} else
	{   if (parse_substitution_argv(ref_argv, ref_argn, state, &value))
	    {   DEBUGMOD(DPRINTF("%s: invoke non-cmd closure\n", codeid()););
                return value;
	    } else
	    {   parser_error(state, "error in parsing closure arguments\n");
		return &value_null;
	    }
	}
    } else
    if (value_type_equal(value, type_cmd))
    {   value_cmd_t *cmd = (value_cmd_t *)value;
        char linebuf[FTL_STRING_MAX];
        const char *line = &linebuf[0];
        /*size_t len = */(void)argv_to_string(ref_argv, ref_argn,
                                              &linebuf[0], sizeof(linebuf));
        
	DEBUGMOD(DPRINTF("%s: invoke direct cmd - '%s'\n", codeid(), line);)
	return (*cmd->exec)(&line, value, state);
    } else
    if (value_type_equal(value, type_code))
    {   const char *buf;
        size_t len;

        DEBUGMOD(DPRINTF("%s: invoke direct code\n", codeid());)
        value_code_buf(value, &buf, &len);
        IGNORE(printf("executing: '%s'[%d]\n", buf, len);)
        /* will garbage collection the discarded value */
        if (parse_cmdlist(&buf, state, &value))
            return value;
        else
        {   parser_error_longstring(state, buf, "code execution failed -");
            return &value_null;
        }
    } else
    if (value_type_equal(value, type_func))
    {   if (parse_substitution_argv(ref_argv, ref_argn, state, &value))
	{   DEBUGMOD(DPRINTF("%s: invoke direct function\n", codeid()););
            return value;
        } else
        {   parser_error(state, "error in parsing closure arguments\n");
	    return &value_null;
	}
    } else
    if (value_type_equal(value, type_dir))
    {   dir_stack_pos_t pos = parser_env_push(state, (dir_t *)value,
					      /*env_end*/FALSE);
	IGNORE(DPRINTF("%s: push enclosing directory %p\n", codeid(), value););
        DEBUGMOD(DPRINTF("%s: push enclosing directory\n", codeid()););
        value = mod_execv(ref_argv, ref_argn, execpath, state);
	/* return to external environment */
	(void)parser_env_return(state, pos);
	DEBUGMOD(DPRINTF("%s: popped enclosing directory\n", codeid());)
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

    if (mod_parse_cmd(dir, ref_line, &value) && parse_space(ref_line))
	return mod_invoke_cmd(ref_line, value, state);
    else {
        const value_t *pcmdsval =
            dir_stringl_get(dir_stack_dir(state->env),
                            FTLDIR_PARSE, strlen(FTLDIR_PARSE));
        if (pcmdsval != NULL && value_type_equal(pcmdsval, type_dir))
        {
            dir_t *pcmds = (dir_t *)pcmdsval;
            const value_t *on_undef = dir_stringl_get(pcmds, FN_UNDEF_HOOK,
                                                      strlen(FN_UNDEF_HOOK));
            if (on_undef != NULL && on_undef != &value_null)
                return mod_invoke_cmd(ref_line, on_undef, state);
            else
                return NULL;
        } else
            return NULL;
    } 
}







/* This function may cause a garbage collection */
static const value_t *
mod_execv(const char ***ref_argv, int *ref_argn, const char *execpath,
          parser_state_t *state)
{   const value_t *value = NULL;
    dir_t *dir = parser_env(state);
    
    DEBUGEXECV(DPRINTF("mod execv start: %s ...[%d] (path %s)\n",
	   	       (*ref_argv)[0], *ref_argn,
                       execpath == NULL? "<NONE>": execpath);)
    if (mod_parse_argv(dir, ref_argv, ref_argn, &value)) {
        DEBUGEXECV(DPRINTF("mod execv parsed %s\n",value_type_name(value)););
        value = mod_invoke_argv(ref_argv, ref_argn, execpath, value, state);
    }
        
    else if (execpath != NULL && *ref_argn > 0)
    {   const char *cmd_file = (*ref_argv)[0];
        charsource_t *fin =
            charsource_file_path_new(execpath, cmd_file, strlen(cmd_file));

        DEBUGEXECV(printf("open '%s' on path '%s' - %s\n", cmd_file, execpath,
                          NULL == fin? "failed": "OK"););
        if (NULL != fin) {
            const char **script_argv = *ref_argv;
            int script_argn = *ref_argn;
            const value_t *orig_argvec = NULL;
            const value_t *pcmdsval =
                dir_stringl_get(dir_stack_dir(state->env), FTLDIR_PARSE,
                                strlen(FTLDIR_PARSE));
            dir_t *pcmds = NULL;

            (*ref_argv)++;
            (*ref_argn)--;

            /* give the rest of the line's arguments to the script */
            while (*ref_argn > 0 && NULL != (*ref_argv)[0] &&
                   0 != strcmp((*ref_argv)[0], ",")) {
                (*ref_argv)++;
                (*ref_argn)--;
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
            }

            if (orig_argvec != NULL) {
                /* restore original arguments */
                mod_add_dir(pcmds, "argv", (dir_t *)orig_argvec);
            }
        }
    }

    DEBUGEXECV(if (*ref_argv <= 0)
                   DPRINTF("mod execv end: FINISHED\n");
               else
                   DPRINTF("mod execv end: %s ...[%d]\n",
	   	           (*ref_argv)[0], *ref_argn);
    );
    return value;
}







/* This function (and all command functions) will return NULL to indicate that
   no command was found for execution
   parses:

       [<directory>]* <command-in-directory>
           (subsequent <command args> are parsed by the <command-in-directory>)

       [<directory>]* <code|binding|fn-in-directory> <vec-args>
           (vector is parsed and given as argument to command)

   This function may cause a garbage collection */
extern const value_t *
mod_exec_cmd(const char **ref_line, parser_state_t *state)
{   if (parser_echo(state))
    {   fprintf(stderr, "%s\n", *ref_line);
	fflush(stderr);
    }
    return mod_exec(ref_line, state);
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
	 DEBUGRCFILE(DPRINTF("%s: rc file '%s'\n", code_name, rcfile_name););
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
                    DEBUGRCFILE(DPRINTF("%s: full rc file name '%s'\n",
					code_name, rc_home););
		    rcfile = fopen(rc_home, "r");
		    if (NULL == rcfile && !using_default)
			DPRINTF("%s: can't open RC file '%s'\n",
				code_name, rc_home);
		    FTL_FREE(rc_home);
		}
	    }
	    DEBUGRCFILE(else DPRINTF("%s: no %s directory\n",
		                     code_name, ENV_FTL_HOMEDIR);)
	}
    }

    if (rcfile != NULL)
    {   charsource_t *rcstream = charsource_stream_new(rcfile, rcfile_name,
						       TRUE);
	linesource_push(parser_linesource(state), rcstream);
    }
    
    DEBUGRCFILE(
	else
	   printf("%s: didn't open rcfile %s: %s (rc %d)\n",
		  code_name, rcfile_name==NULL?"<>": rcfile_name,
		  strerror(errno), errno);)
	
    return rcfile;
}








/* This function may cause a garbage collection */
extern const value_t *
parser_argv_exec(parser_state_t *state, const char **argv, int argn,
                 const char *execpath, bool expect_no_locals)
{   const value_t *val = NULL;
    linesource_t saved;

    IGNORE(DIR_SHOW_ST("Current dir in exec: ", state, parser_env(state));)
    linesource_save(parser_linesource(state), &saved);
    linesource_init(parser_linesource(state));

    while (argn > 0)
    {   const char *cmd = argv[0];
        DEBUGENV(dir_t *startdir = parser_env_stack(state)->stack;)
        if (NULL != val)
            value_unlocal(val);

        DEBUGEXECV(DPRINTF("MOD EXECV --- %s ...[%d]\n",
                           argv[0], argn);)
        val = mod_execv(&argv, &argn, execpath, state);

        DEBUGEXECV(VALUE_SHOW("MOD EXECV report val: ", val););
        if (val != NULL)
        {   if (!value_type_equal(val, type_null))
            {   value_fprint(stdout,
                             dir_value(parser_root(state)), val);
                fprintf(stdout, "\n");
            }
        } else
        {   parser_error(state, "unknown command '%s'\n", cmd);
            argn = 0;
        }

        /* skip intervening "," arguments */
        if (argn > 0 && 0 == strcmp(argv[0], ",")) {
            argn--;
            argv++;
        }

        DEBUGENV(
            if (parser_env_stack(state)->stack != startdir)
            {   DEBUGENV(printf("%s: env update %p -> %p\n",
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
    }

    linesource_restore(parser_linesource(state), &saved);

    return val == NULL? &value_null: val;
}



static void
parser_report_trailing(parser_state_t *state, const char *msg,
                       const char *phrase)
{
    char *p;
    int width;
    int lines = 0;

    p = strchr(phrase, '\n');
    width = p == NULL? strlen(phrase)+1: p - phrase;
    /*printf("%s: width of trailing line %d is %d\n", codeid(), lines, width);*/
    parser_report(state, "%s%.*s\n", msg, width, phrase);
    while (p != NULL && lines < FTL_ERROR_TRAIL_LINES) {
        phrase += width+1;
        p = strchr(phrase, '\n');
        width = p == NULL? strlen(phrase): p - phrase;
        /*printf("%s: width of trailing line %d is %d\n",
          codeid(), lines, width); */
        parser_report(state, "%.*s\n", width, phrase);
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
    interrupt_state_t old_state = NULL;
    FILE *resultout = interactive? stdout: NULL;

    IGNORE(DIR_SHOW_ST("Current dir in exec: ", state, parser_env(state));)
    linesource_save(parser_linesource(state), &saved);
    linesource_init(parser_linesource(state));

    if (NULL != source)
	linesource_push(parser_linesource(state), source);

    if (NULL != cmd_str)
	linesource_pushline(parser_linesource(state), "<INIT>", cmd_str);

    if (NULL != rcfile_id)
	(void)rcfile_run(state, rcfile_id);

    if (interactive)
        (void)interrupt_handler_init(&old_state);

    while (!linesource_eof(parser_linesource(state)))
    {   if (interactive && interrupt()) {
            if (source == parser_charsource(state)) {
                fprintf(stderr, "%s: at initial source\n", codeid());
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

            /* copy line from source to linesink and get buffer */
            linesource_read(parser_linesource(state), linesink);
            charsink_string_buf(linesink, &phrase, &len);

            /* expand \ and $ in buffer writing to expandsink and get buffer */
            parser_expand(state, expandsink, phrase, len);
            charsink_string_buf(expandsink, &phrase, &len);
            charsink_string_close(linesink);

            parse_space(&phrase);

            if (!parse_empty(&phrase))
            {   DEBUGENV(dir_t *startdir = parser_env_stack(state)->stack;)
                if (NULL != val)
                    value_unlocal(val);

                /* parse and execute the line */
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

                DEBUGENV(
                    if (parser_env_stack(state)->stack != startdir)
                    {   DEBUGENV(printf("%s: env update %p -> %p\n",
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
            }
            charsink_string_close(expandsink);
        }
    }

    linesource_restore(parser_linesource(state), &saved);

    if (interactive)
        interrupt_handler_end(old_state);

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
extern void
argv_cli(parser_state_t *state, const char *code_name, const char *execpath,
         const char **argv, int argc)
{   codeid_set(code_name);
    (void)parser_argv_exec(state, argv, argc, execpath,
                           /*expect_no_locals*/TRUE);
}












/*****************************************************************************
 *                                                                           *
 *          Values - Formatted Output			                     *
 *          =========================		                             *
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
{   (void)for_type; /* currently ignored */
    
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
    const value_t *fmtfn;

    while ((!left && (left = parse_key(ref_line,"-"))) ||
	   (!posv && (sign = parse_key(ref_line,"+"))) ||
	   (!sign && (sign = parse_key(ref_line," "))) ||
	   (!zero && (zero = parse_key(ref_line,"0"))) ||
	   (!alt && (alt = parse_key(ref_line,"#"))))
	continue;

    IGNORE(printf("%s: %sleft %ssign %szero\n", codeid(),
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
	} else if (isalpha((*ref_line)[0]))
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
		      state, /*try*/FALSE);
    if (NULL != bind)
    {   bind = substitute(bind, value_int_new(precision),
			  state, /*try*/FALSE);
	if (NULL != bind)
	{   bind = substitute(bind, val, state, /*try*/FALSE);
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
	    IGNORE(printf("%s: %sleft - width %d flags %d\n",
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
 *          ==================		                                     *
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

    if (value_istype(inhval, type_bool) &&
        (closure_env || value_istype(argenvval, type_dir))) {
         if (nocode ||
	     value_type_equal(code, type_code) ||
	     value_type_equal(code, type_cmd) ||
	     value_type_equal(code, type_func))
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
	      "<bool> <dir> <code> - create closure from code and dir (inherrit call context if <bool>)",
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
 *          Commands - Coroutines			                     *
 *          =====================		                             *
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
 *          Commands - Parser				                     *
 *          =================			                             *
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

	linesource_read(parser_linesource(state), linesink);
	charsink_string_buf(linesink, &phrase, &len);
	parser_expand(state, expandsink, phrase, len);
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
	    
	    IGNORE(DIR_SHOW("Orig env: ", parser_env(state));)
	    /* return to the calling envrionment - two steps (locals, args?) */
	    parser_env_return(state, parser_env_calling_pos(state));
	    parser_env_return(state, parser_env_calling_pos(state));
	    /* TODO: protect variables in the current scope against garbage
		     collection */
	    
	    IGNORE(DIR_SHOW("Invoke env: ", parser_env(state));)
	    pos = parser_env_push(state, new_dir, /*env_end*/FALSE);
	    IGNORE(DIR_SHOW("Dir after push: ", parser_env(state));)
	    res = parser_expand_exec(state, source, initcmds,
				     /*rcfile_id*/NULL, /*no_locals*/FALSE);
	    IGNORE(DIR_SHOW("Final env: ", parser_env(state));)
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
                IGNORE(
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
	value_istype(setres, type_closure))
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
		
		setres = substitute(setres, newval, state, /*try*/TRUE);
		if (NULL != setres)
		    (void)invoke(setres, state);
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
    
    if (parse_code(ref_line, state, &strval, &srcpos, &srcline))
        return strval;
    else
	return &value_null;
}




static const value_t *
fn_scan_code(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_code, NULL);
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
    if (value_istype(delims, type_string))
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
		dir_string_set(opdefn, OP_FN, fn);
		dir_string_set(opdefn, OP_ASSOC, assocval);
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
	      "<string> - return parse object for string",
	      &fn_scan, 1);
    mod_addfn(pcmds, "scanned",
	      "<parseobj> - return text remaining in parse object",
	      &fn_scanned, 1);
    mod_addfn(pcmds, "scanempty",
	      "<parseobj> - parse white space from parse object",
	      &fn_scan_empty, 1);
    mod_addfn(pcmds, "scanwhite",
	      "<parseobj> - parse white space from parse object",
	      &fn_scan_white, 1);
    mod_addfn(pcmds, "scanspace",
	      "<parseobj> - parse over white space from parse object",
	      &fn_scan_space, 1);
    mod_addfn(pcmds, "scanint",
	      "<@int> <parseobj> - parse integer from parse object",
	      &fn_scan_int, 2);
    mod_addfn(pcmds, "scanintval",
	      "<@int> <parseobj> - parse signed based integer from parse object",
	      &fn_scan_intval, 2);
    mod_addfn(pcmds, "scanhex",
	      "<@int> <parseobj> - parse hex from parse object",
	      &fn_scan_hex, 2);
    mod_addfn(pcmds, "scanhexw",
	      "<width> <@int> <parseobj> - parse hex in <width> chars from "
	      "parse object, update string",
	      &fn_scan_hexw, 3);
    mod_addfn(pcmds, "scanstr",
	      "<@string> <parseobj> - parse item until delimiter from "
	      "parse string",
	      &fn_scan_string, 2);
    mod_addfn(pcmds, "scanid",
	      "<@string> <parseobj> - parse identifier from parse object",
	      &fn_scan_id, 2);
    mod_addfn(pcmds, "scanitemstr",
	      "<@string> <parseobj> - parse item or string from parse object",
	      &fn_scan_itemstr, 2);
    mod_addfn(pcmds, "scancode",
	      "<@string> <parseobj> - parse {code} block from parse object",
	      &fn_scan_code, 2);
    mod_addfn(pcmds, "scanitem",
	      "<delims> <@string> <parseobj> - parse item until delimiter "
	      "from parse object",
	      &fn_scan_item, 3);
    mod_addfn(pcmds, "scanmatch",
	      "<dir> <@val> <parseobj> - parse prefix in dir from "
	      "parse object giving matching value",
	      &fn_scan_match, 3);
    
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
 *          Commands - System				                     *
 *          =================			                             *
 *                                                                           *
 *****************************************************************************/







static const value_t *
cmd_system(const char **ref_line, const value_t *this_cmd,
	   parser_state_t *state)
{   int rc;
    const char *str = *ref_line;
    const value_t *res = &value_null;
    
    IGNORE(printf("system <%s>\n", str);)
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
    
    IGNORE(printf("system <%s>\n", str););
        
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
    
    IGNORE(printf("system <%s>\n", str);)
    ok = user_uid(str, &uid);
    *ref_line += strlen(*ref_line);

    return ok? value_int_new(uid): &value_null;
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
	dir_string_set(tm, "sec",   value_int_new(tinfo->tm_sec));
	dir_string_set(tm, "min",   value_int_new(tinfo->tm_min));
	dir_string_set(tm, "hour",  value_int_new(tinfo->tm_hour));
	dir_string_set(tm, "mday",  value_int_new(tinfo->tm_mday));
	dir_string_set(tm, "mon",   value_int_new(tinfo->tm_mon));
	dir_string_set(tm, "year",  value_int_new(SYS_EPOCH_YEAR +
						  tinfo->tm_year));
	dir_string_set(tm, "wday",  value_int_new(tinfo->tm_wday));
	dir_string_set(tm, "yday",  value_int_new(tinfo->tm_yday));
	dir_string_set(tm, "isdst", value_int_new(tinfo->tm_isdst));
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
	if (value_string_get(fmtval, &fmtbuf, &fmtlen))
	{   time_t dat = (time_t)value_int_number(timeval);
	    struct tm *tinfo = (*fn)(&dat);
	    char fmtstr[128];
	    size_t len = strftime(&fmtstr[0], sizeof(fmtstr),
				  fmtbuf, tinfo);
	    if (0 != len)
	        val = value_string_new(&fmtstr[0], len);
	}
    }
    
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
    
    IGNORE(printf("getenv <%s>\n", str);)
    envval = getenv(str);
    if (envval == NULL)
	parser_error(state, "environment variable not set - '%s'\n", str);
    else
	val = value_string_new_measured(envval);
	      
    *ref_line += strlen(*ref_line);

    return val;
} 
#endif








static void
cmds_generic_sys(parser_state_t *state, dir_t *cmds)
{   dir_t *scmds = dir_id_new();
    dir_t *sysenv = dir_sysenv_new();
    dir_t *fscmds = dir_id_new();
    dir_t *shcmds = dir_id_new();
    const char *osfamily = "unknown";
    char sep[2];

#ifdef _WIN32
    osfamily = "windows";
#endif
#ifdef __linux__
    osfamily = "linux";
#endif
#ifdef __sun__
    osfamily = "sunos";
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
    (void)dir_lock(fscmds, NULL); /* prevents additions to this directory */
    
    sep[0] = OS_PATH_SEP;
    sep[1] = '\0';
    mod_add_val(shcmds, "pathsep", value_string_new_measured(sep));
    mod_addfn(shcmds, "path", "<path> <file> - return name of file on path",
	      &fn_path, 2);
    (void)dir_lock(shcmds, NULL); /* prevents additions to this directory */
    
    mod_add_dir(cmds, "sys", scmds);
    mod_add_dir(scmds, "fs", fscmds);
    mod_add_dir(scmds, "shell", shcmds);
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
 *          Commands - Stream				                     *
 *          =================						     *
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
            code = substitute(code, str, state, /*try*/FALSE);
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
	    val = value_int_new(value_print(sink,dir_value(parser_root(state)),
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
		    {	char *line = fgets(linebuf, sizeof(filename), inchars);
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
 *          Commands - Nul				                     *
 *          ==============						     *
 *                                                                           *
 *****************************************************************************/






static void
cmds_generic_nul(parser_state_t *state, dir_t *cmds)
{   mod_add_val(cmds, "NULL", &value_null);
}





    
/*****************************************************************************
 *                                                                           *
 *          Commands - Type				                     *
 *          ===============						     *
 *                                                                           *
 *****************************************************************************/







static const value_t *
fn_typeof(const value_t *this_fn, parser_state_t *state)
{   const value_t *any = parser_builtin_arg(state, 1);
    const value_t *res = &value_null;
    
    if (NULL != any)
        res = value_type_new(value_type(any));
    
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
fn_cmp(const value_t *this_fn, parser_state_t *state)
{   const value_t *v1 = parser_builtin_arg(state, 1);
    const value_t *v2 = parser_builtin_arg(state, 2);
    
    return value_int_new(value_cmp(v1, v2));
}





    
static void
cmds_generic_type(parser_state_t *state, dir_t *cmds)
{   mod_addfn(cmds, "typeof",
	     "<expr> - returns the type of <expr>",
	     &fn_typeof, 1);
    mod_addfn(cmds, "typename",
	     "<expr> - returns the name of the type of <expr>",
	     &fn_typename, 1);
    mod_addfn(cmds, "cmp",
	     "<expr> <expr> - returns integer comparing its arguments",
	     &fn_cmp, 2);
    mod_add(cmds, "type",
	    "<type name> - type value",  &value_type_parse);
}









/*****************************************************************************
 *                                                                           *
 *          Commands - Boolean			                             *
 *          ==================					             *
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
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "le", op_le);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "gt", op_gt);
    mod_add_op(parser_opdefs(state), OP_PREC_CMP, assoc_xfx, "ge", op_ge);
    
    mod_add_op(parser_opdefs(state), OP_PREC_NOT, assoc_fy,  "not", op_not);
    mod_add_op(parser_opdefs(state), OP_PREC_AND, assoc_xfy, "and", op_and);
    mod_add_op(parser_opdefs(state), OP_PREC_OR,  assoc_xfy, "or",  op_or);
    
    mod_add_val(cmds, "TRUE", value_true);
    mod_add_val(cmds, "FALSE", value_false);
    
}






/*****************************************************************************
 *                                                                           *
 *          Commands - Integer				                     *
 *          ==================						     *
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

	IGNORE(printf("%s: %"F_NUMBER_T" %s %"F_NUMBER_T
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

	IGNORE(printf("%s: %"F_NUMBER_T" %s %"F_NUMBER_T
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
    value_t *op_shl = mod_addfn(cmds, "shiftl",
	      "<n1> <n2> - return n1 left shifted by n2 bits",
				&fn_int_shl, 2);
    value_t *op_shr = mod_addfn(cmds, "shiftr",
	      "<n1> <n2> - return n1 right shifted by n2 bits",
				&fn_int_shr, 2);
    value_t *op_neg = mod_addfn(cmds, "neg",
	      "<n> - return negated <n>",  &fn_int_neg, 1);
    mod_addfn(cmds, "bitand",
	      "<n1> <n2> - return bit values of <n1> anded with <n2>",
	      &fn_int_bitand, 2);
    mod_addfn(cmds, "bitor",
	      "<n1> <n2> - return bit values of <n1> ored with <n2>",
	      &fn_int_bitor, 2);
    mod_addfn(cmds, "bitxor",
	      "<n1> <n2> - return bit values of <n1> exclusive ored with <n2>",
	      &fn_int_bitxor, 2);
    mod_addfn(cmds, "bitnot",
	      "<n> - invert each bit in <n>",
              &fn_int_bitnot, 1);
    mod_addfn(cmds, "rnd",
	      "<n> - return random number less than <n>",
              &fn_rnd, 1);
    mod_addfn(cmds, "int_fmt_hexbits",
	      "<n> - set bits that won't result in integer hex "
              "printing in hex when set",
              &fn_int_set_hexbits, 1);
    mod_add(cmds, "int",
	    "<integer expr> - numeric value",  &value_int_parse);

    mod_add_op(parser_opdefs(state), OP_PREC_SIGN,   assoc_fy,  "-",   op_neg);
    mod_add_op(parser_opdefs(state), OP_PREC_PROD,   assoc_xfy, "*",   op_mul);
    mod_add_op(parser_opdefs(state), OP_PREC_PROD,   assoc_xfy, "/",   op_div);
    mod_add_op(parser_opdefs(state), OP_PREC_SUM,    assoc_xfy, "+",   op_add);
    mod_add_op(parser_opdefs(state), OP_PREC_SUM,    assoc_xfy, "-",   op_sub);
    mod_add_op(parser_opdefs(state), OP_PREC_SHIFT,  assoc_xfy, "shl", op_shl);
    mod_add_op(parser_opdefs(state), OP_PREC_SHIFT,  assoc_xfy, "shr", op_shr);

}









/*****************************************************************************
 *                                                                           *
 *          Commands - String				                     *
 *          =================						     *
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
	while (len > 0 && wlen >= 0)
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
	
	if (len >= 0 && len < sizeof(mbstring))
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
    /*< last pos where a search for find makes sense */
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





static char *strnstr(const char *buf, size_t len, const char *find)
{
    /* No nulls in sought string - so if it occurs it must always occur
       in a null-terminated section of buf.
       Note that, commonly, len is < strlen(buf).
    */
    size_t findlen = strlen(find);
    size_t buflen = strlen(buf); /* first section, at least */
    const char *end = buf + len;
    const char *last = end - findlen;
    /*< last pos where a search for find makes sense */
    char *p = NULL;

    if (buflen > len + 256)
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




/*< on linux this routine is provided as memmem */
static char *strnstrn(const char *buf, size_t len,
                      const char *find, size_t findlen)
{  size_t len0 = strlen(buf);
   size_t findlen0 = strlen(find);

   if (findlen0 == findlen) {
       if (len0 == len)
           return strstr(buf, find);
       else
           return strnstr(buf, len, find);
   } else {
       if (len0 == len)
           /* if the sought string has a NULL in it, and the string we're
              searching in doesn't it stands to reason that you won't find the
              sought string there */
           return NULL;
       else if (findlen < findlen0) {
           /* oh dear - there's no fast C library routine that can help here */
           return my_memmem(buf, len, find, findlen);
       } else {
           /* the null-terminated strings in the sought string must occur
              in sequence as the last, the whole... and the first
              null-terminated parts of buf */
           const char *end = buf + len;
           const char *last = end - findlen;
           /*< last pos where a search for find makes sense */
           char *p = NULL; /* overall pointer result */
           char *sp = NULL; /* sub-string pointer result */

           while (NULL == p &&
                  buf < last &&
                  NULL != (sp = strnstr(buf, end-buf, find)) &&
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
        int len;
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
	    {	while (len > 0)
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









typedef struct {
    charsink_t *sink;		/* stream to write chars to */
    bool first;			/* true on first insertion */
    const value_t *delim;	/* delimiter to insert */
    bool as_bytes;		/* integer = octet not char */
    int rc;			/* 0 if no error so far */
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
 *          Commands - IP Address			                     *
 *          =====================					     *
 *                                                                           *
 *****************************************************************************/







static void
cmds_generic_ipaddr(parser_state_t *state, dir_t *cmds)
{   mod_add(cmds, "ip",
	    "<IP addr> - IP address value",  &value_ipaddr_parse);
}









/*****************************************************************************
 *                                                                           *
 *          Commands - MAC Address			                     *
 *          =====================					     *
 *                                                                           *
 *****************************************************************************/







static void
cmds_generic_macaddr(parser_state_t *state, dir_t *cmds)
{   mod_add(cmds, "mac",
	    "<MAC addr> - MAC address value",  &value_macaddr_parse);
}









/*****************************************************************************
 *                                                                           *
 *          Commands - Directory			                     *
 *          ====================					     *
 *                                                                           *
 *****************************************************************************/









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







static const value_t *
fn_new(const value_t *this_fn, parser_state_t *state)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *val = NULL;
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
    } else
        parser_report_help(state, this_fn);

    return value_nl(val);
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






/*! Push a new environment on the stack that will be used when new variables
 *  are defined.  Make previous environments "invisible" if \c env_end is
 *  FALSE
 */
static const value_t *
parser_enter_dir(const value_t *this_fn, parser_state_t *state, bool env_end)
{   const value_t *dir = parser_builtin_arg(state, 1);
    const value_t *val = &value_null;
    dir_t *env;
    
    if (value_as_dir(dir, &env))
    {   dir_stack_pos_t pos = parser_env_calling_pos(state);
	
	/* insert the directory above our return position so that it is
	   still there when we return */
	parser_env_push_at_pos(state, pos, env, env_end);
    } else
        parser_report_help(state, this_fn);

    return val;
}






static const value_t *
fn_enter(const value_t *this_fn, parser_state_t *state)
{   return parser_enter_dir(this_fn, state, /*env_end*/FALSE);
}






static const value_t *
fn_restrict(const value_t *this_fn, parser_state_t *state)
{   return parser_enter_dir(this_fn, state, /*env_end*/TRUE);
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
{   IGNORE(
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
{   IGNORE(
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
{   IGNORE(
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
	code = substitute(code, *ref_v1, state, /*try*/TRUE);
	code = substitute(code, *ref_v2, state, /*try*/TRUE);
        ret = invoke(code, state);

	return (int)value_int_number(ret);
    }
}




static int
fn_sort_rngcmpwith(const value_t **ref_v1, const value_t **ref_v2)
{   IGNORE(
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
	code = substitute(code, v1, state, /*try*/TRUE);
	code = substitute(code, v2, state, /*try*/TRUE);
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
    
    if (NULL != codeargs)
    {   const value_t *code = selectval->code;
	const value_t *include;
	
	if (value_type_equal(code, type_closure))
	{   code = substitute(code, value, selectval->state, /*try*/TRUE);
	    if (NULL != code)
		code = substitute(code, name, selectval->state, /*try*/TRUE);
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
	IGNORE(VALUE_SHOW("select beselecte environment: ",
			  dir_value(parser_env(state)));)
        pos = parser_env_push(state, argdir, /*env_end*/FALSE);
	IGNORE(VALUE_SHOW("select environment: ",
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
        zipped = dir_int_get(dir_vals, zipval->index);
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
 *          Commands - Language			                             *
 *          ===================					             *
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
	    IGNORE(
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
for_exec(dir_t *dir, const value_t *name, const value_t *value, void *arg)
{   for_exec_args_t *forval = (for_exec_args_t *)arg;
    dir_t *codeargs = parser_env(forval->state);

    /* don't execute for values that are NULL - warning this may mean that
       the perceived domain of an environment is larger than the range
     */
    if (NULL != codeargs && value != NULL)
    {   const value_t *code = forval->code;
	
	if (value_type_equal(code, type_closure))
	{   code = substitute(code, value, forval->state, /*try*/TRUE);
	    if (NULL != code)
		code = substitute(code, name, forval->state, /*try*/TRUE);
	}
        (void)invoke(code, forval->state);
    }

    return NULL; /* non-null would stop the enumeration here */
}





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
	IGNORE(VALUE_SHOW("for before environment: ",
			  dir_value(parser_env(state)));)
        pos = parser_env_push(state, argdir, /*env_end*/FALSE);
	IGNORE(VALUE_SHOW("for environment: ", dir_value(parser_env(state)));)
	(void)dir_forall(env, &for_exec, &forval);
        (void)parser_env_return(state, pos);
	val = &value_null;
    } else
    {   parser_report_help(state, this_fn);
	val = &value_null;
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
    
    if (value_istype(condval, type_bool))
    {   const value_t *exec = (value_bool_is_false(condval)? else_code:
			                                     then_code);

	if (NULL != exec)
	{   /* return to the calling envrionment */
	    IGNORE(DIR_SHOW("Orig env: ", parser_env(state));)
	    parser_env_return(state, parser_env_calling_pos(state));
	    IGNORE(DIR_SHOW("Invoke env: ", parser_env(state));)
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
    
    if (cond_code != NULL && do_code != NULL)
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
    mod_addfnscope(cmds, "forall",
	      "<env> <binding> - execute <binding> for all <env> values",
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
}






/*****************************************************************************
 *                                                                           *
 *          Commands - Registry				                     *
 *          ===================						     *
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
 *          Commands - Misc				                     *
 *          ===============						     *
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
	    const value_t *bind = substitute(fn, arg, state, /*try*/FALSE);
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
    {	value_env_t *env = value_env_new();
	dir_t *helpenv = dir_id_new();
	char argname[16];
	value_t *closure = value_closure_new(value_cmd_new(&cmd_from_fn,
							   fnval,
							   /*help*/NULL),
					     env);
	if (NULL != helpval && helpval != &value_null)
	    if (value_istype(helpval, type_string))
	        dir_string_set(helpenv, BUILTIN_HELP, helpval);
	(void)value_closure_pushdir(closure, helpenv, /*env_end*/FALSE);
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
    cmds_generic_sys(state, cmds);
    cmds_generic_registry(state, cmds);
    if (NULL != argv)
        cmds_generic_parser(state, cmds, argc, argv);
    
    cmds_generic_coroutine(state, cmds);
    cmds_generic_stream(state, cmds);
    cmds_generic_type(state, cmds);
    cmds_generic_nul(state, cmds);
    cmds_generic_int(state, cmds);
    cmds_generic_bool(state, cmds);
    cmds_generic_string(state, cmds);
    cmds_generic_ipaddr(state, cmds);
    cmds_generic_macaddr(state, cmds);
    cmds_generic_closure(state, cmds);
    cmds_generic_dir(state, cmds);
    
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







/*****************************************************************************
 *                                                                           *
 *          Library					                     *
 *          =======							     *
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
    values_null_init();
    values_int_init();
    values_string_argname_init();
    values_bool_init();
    fprint_init();
    IGNORE(printf("FTL_MB_LEN_MAX = %d MB_LEN_MAX = %d (%s)\n",
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
