/*
 * Copyright (c) 2005-2009, Solarflare Communications Inc.
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

/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  cgg
**  \brief  tests/ftl	New control plane testing framework
**   \date  2005/08
** </L5_PRIVATE>
*//*
\**************************************************************************/






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
#include <errno.h>
#include <ctype.h>

#include "ftl.h"

#ifdef USE_FTLLIB_ELF
#include "ftl_elf.h"
#endif

#ifdef USE_FTLLIB_XML
#include "ftl_xml.h"
#endif





/*****************************************************************************
 *                                                                           *
 *          Configuration                                                    *
 *          =============						     *
 *                                                                           *
 *****************************************************************************/




#define VERSION_MAJ 1
#define VERSION_MIN 1

#define ENV_PATH "FTL_PATH"
#define ENV_PROLOG "FTL_PROLOG"

/* #define TEST_FTLSTRUCT */
/* #define USE_READLINE */
/* #define USE_FTLLIB_ELF */
/* #define USE_FTLLIB_XML */

#define FTL_DIR_OPTIONS  "opt"
#define FTL_DIR_CMDS_XML "xml"   // used if USE_FTLLIB_XML is set
#define FTL_DIR_CMDS_ELF "elf"   // used if USE_FTLLIB_ELF is set

#define APP_ARGC_MAX 128

#define STATIC_INLINE static inline

#define CATDELIM(a, delim, b) #a  delim  #b
#define VERSIONSTR(max, min) CATDELIM(max, ".", min)
#define VERSION VERSIONSTR(VERSION_MAJ, VERSION_MIN) 


#define EXIT_OK           0
#define EXIT_BAD_APPOPT   1   /* bad application option */
#define EXIT_BAD_INIT     2   /* memory initialization eror */
#define EXIT_BAD_FTLINIT  3   /* couldn't initialize FTL parser */
#define EXIT_BAD_PRLGFILE 4   /* couldn't execute prolog file */
#define EXIT_BAD_PRLGSTR  5   /* couldn't execute prolog string */
#define EXIT_BAD_FTLOPT   6   /* prolog FTL options parsing failed */
#define EXIT_BAD_CMDFILE  7   /* couldn't find command file */
#define EXIT_BAD_CMDFRUN  8   /* execution of command file failed */
#define EXIT_BAD_FTLCMDS  9   /* execution of command line FTL failed */





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

#define DO(x) x
#define IGNORE(x) 

/* #define DEBUGGC   DO */
/* #define DEBUG_CLI DO */

#ifdef NDEBUG
#undef DEBUGGC
#undef DEBUG_CLI
#endif

#ifndef DEBUGGC
#define DEBUGGC      IGNORE
#endif

#ifndef DEBUG_CLI
#define DEBUG_CLI    IGNORE
#endif

/* #define DPRINTF ci_log */
#define DPRINTF printf


#define CODEID "ftl"






/*****************************************************************************
 *                                                                           *
 *          Command Arguments						     *
 *          =================					             *
 *                                                                           *
 *****************************************************************************/







static void 
usage(const char* msg)
{   if (NULL != msg)
	fprintf(stderr, "error: %s\n", msg);

    fprintf(stderr, "\nusage:\n");
    fprintf(stderr, "  "CODEID" [-r <randseed>] [-e|-ne] [-c <commands> | [-f] <cmdfile>] "
	    "[[--] <script arg>...]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "     [-f] <cmdfile>     - read commands from this file instead of the console\n");
    fprintf(stderr, "     -c \"cmd;cmd;...\"   - execute initial commands\n");
    fprintf(stderr, "     -r <n>             - set random seed\n");
    fprintf(stderr, "     -[n]e | --[no]emit - "
		    "[don't] echo executed commands\n");
    fprintf(stderr, "     -q | --quiet       - "
	            "don't report unnecessary info\n");
    exit(1);
}







static const char *
parse_args(int argc, char **argv,
	   int *out_argc, const char **out_argv, size_t out_argv_len,
	   const char **ref_cmd, const char **ref_input,
	   bool *out_echo, bool *out_do_prolog, bool *out_quiet)
{   int argn = 1;
    int out_argn = 0;
    const char *err = NULL;
    bool localargs = TRUE;

    if (out_argv_len > 0)
        out_argv[out_argn++] = argv[0];

    while (argn < argc)
    {   const char *arg = argv[argn];

	if (localargs)
	{   if ((parse_key(&arg, "-h") || parse_key(&arg, "--help")) &&
		parse_empty(&arg))
	    {   err = "help requested";
	    } else
	    if ((parse_key(&arg, "-c") || parse_key(&arg, "--cmd")) &&
		parse_empty(&arg))
	    {   if (++argn < argc)
		    *ref_cmd = argv[argn];
		else
		    err = "ran out of arguments";
	    } else
	    if ((parse_key(&arg, "-f") || parse_key(&arg, "--file")) &&
		parse_empty(&arg))
	    {   if (++argn < argc)
		{   *ref_input = argv[argn];
		} else
		    err = "ran out of arguments";
	    } else
	    if ((parse_key(&arg, "-r") || parse_key(&arg, "--randomseed")) &&
		parse_empty(&arg))
	    {   if (++argn < argc)
		{   unsigned int seed = (unsigned)strtoul(argv[argn], NULL, 0);
		    IGNORE(DPRINTF("seed: %d\n", seed));
		    srand(seed);
		} else
		    err = "ran out of arguments";
	    } else
	    if ((parse_key(&arg, "-q") || parse_key(&arg, "--quiet")) &&
		parse_empty(&arg))
		*out_quiet = TRUE;
	    else
	    if ((parse_key(&arg, "-e") || parse_key(&arg, "--emit")) &&
		parse_empty(&arg))
		*out_echo = TRUE;
	    else
	    if ((parse_key(&arg, "-ne") || parse_key(&arg, "--noemit") ||
                 parse_key(&arg, "--noecho")) &&
		parse_empty(&arg))
		*out_echo = TRUE;
	    else
            if ((parse_key(&arg, "-np") || parse_key(&arg, "--noprolog")) &&
		parse_empty(&arg))
		*out_do_prolog = FALSE;
	    else
	    if (parse_key(&arg, "--") && parse_empty(&arg))
		localargs = FALSE;
	    else
	    {   localargs = FALSE;
                if ((size_t)out_argn < out_argv_len)
                    out_argv[out_argn++] = argv[argn];
                else
                    err = "too many options";
            }
#if 0
            // take first non-keyed argument as --file input
	    if (NULL == *ref_input)
	    {   *ref_input = argv[argn];
		localargs = FALSE; /* subsequent args are for the script */
	    } else
		err = "unknown option";
#endif
	} else
	{   if ((size_t)out_argn < out_argv_len)
		out_argv[out_argn++] = argv[argn];
	    else
	        err = "too many options";
	}
	argn++;
    }

    *out_argc = out_argn;
    
    return err;
}







/*****************************************************************************
 *                                                                           *
 *          Dummy Application						     *
 *          =================				                     *
 *                                                                           *
 *****************************************************************************/







#ifndef TEST_FTLSTRUCT

#define cmds_ftl(state) TRUE

#define cmds_ftl_end(state)

#else



/* Example use of the FTL_* macros provided in the header to create FTL type
 * definitions that can be used to acccess specific C datastructures from FTL.
 * These definitions include the embedding of one type inside another.
 */

typedef unsigned char mymac_t[6];

typedef struct {
    int a;
    int b;
    short n[4];
} subtest_t;

typedef subtest_t subtest_array_t[4];

typedef struct {
    unsigned fa;
    number_t fc;
    subtest_t fd;
    subtest_array_t x;
    char fb;
} test_t;

typedef test_t testarray_t[3];

/* Definitions of the types above.
 *
 * Note: with a little more macro-magic you could make macro definitions
 *       similar to those below that will expand either to what is below or
 *       the C type declarations themselves.
 *       This would eliminate the current redundancy in the type definitions.
 */

/* FTL definition for array [6] of given type (unsigned char) for mymac_t */
#define ARRAY_MYMAC(ctx) \
    FTL_TARRAY_BEGIN(ctx, mymac_t)                      \
    FTL_TARRAY_INT(ctx, mymac_t, unsigned char, 6)	\
    FTL_TARRAY_END(ctx)                                 \


/* FTL definition for struct type subtest_t with fields a and b and n[4] */
#define STRUCT_SUBTEST(ctx)					\
    FTL_TSTRUCT_BEGIN(ctx, subtest_t,)				\
    FTL_TFIELD_INT(ctx, subtest_t, int, a)			\
    FTL_TFIELD_INT(ctx, subtest_t, int, b)			\
    FTL_TFIELD_ARRAYOFINT(ctx, subtest_t, short, n, 4)	        \
    FTL_TSTRUCT_END(ctx)


/* FTL definition for array [4] of given type (subtest_array_t) for subtest_t */
#define ARRAY_SUBTEST(ctx) \
    FTL_TARRAY_BEGIN(ctx, subtest_array_t)                      \
    FTL_TARRAY_STRUCT(ctx, subtest_array_t, subtest_t, 4)	\
    FTL_TARRAY_END(ctx)						\


/* FTL definition for struct type test_t with fields fa, fb, fc, fd and x[4] */
#define STRUCT_TEST(ctx) \
    FTL_TSTRUCT_BEGIN(ctx, test_t, )				\
    FTL_TFIELD_INT(ctx, test_t, unsigned, fa)    		\
    FTL_TFIELD_INT(ctx, test_t, char, fb) 			\
    FTL_TFIELD_INT(ctx, test_t, number_t, fc)    		\
    FTL_TFIELD_STRUCT(ctx, test_t, subtest_t, fd)		\
    FTL_TFIELD_ARRAYOFSTRUCT(ctx, test_t, subtest_t, x, 4)      \
    FTL_TSTRUCT_END(ctx)


/* FTL definition for array [2] of given type (type_t) for testarray_t */
#define ARRAY_TEST(ctx) \
    FTL_TARRAY_BEGIN(ctx, testarray_t)				\
    FTL_TARRAY_STRUCT(ctx, testarray_t, test_t, 2)		\
    FTL_TARRAY_END(ctx)						\


/* Use the macros above to generate the structures and field access functions
 * needed to create accessors for the types they describe.
 */
FTL_DECLARE(ARRAY_MYMAC)
FTL_DECLARE(STRUCT_SUBTEST)
FTL_DECLARE(ARRAY_SUBTEST)
FTL_DECLARE(STRUCT_TEST)
FTL_DECLARE(ARRAY_TEST)

static void data_init(void)
{   /* allocate resources needed for the FTL structure definitions */
    FTL_DEFINE(ARRAY_MYMAC)
    FTL_DEFINE(STRUCT_SUBTEST)
    FTL_DEFINE(ARRAY_SUBTEST)
    FTL_DEFINE(STRUCT_TEST)
    FTL_DEFINE(ARRAY_TEST)
}

static void data_end(void)
{   /* release resources allocated to the FTL structure definitions */
    FTL_UNDEFINE(ARRAY_MYMAC)
    FTL_UNDEFINE(STRUCT_SUBTEST)
    FTL_UNDEFINE(ARRAY_SUBTEST)
    FTL_UNDEFINE(STRUCT_TEST)
    FTL_UNDEFINE(ARRAY_TEST)
}


/* example areas of memory we will create FTL values to acccess */
static test_t       test_val;
static mymac_t      mymac_val;
static testarray_t  array_val;



static bool
cmds_ftl(parser_state_t *state)
{   dir_t *cmds = parser_env(state);

    data_init();

    memset(&test_val, 0xff, sizeof(test_val));
    IGNORE(printf("%s: test val at %p\n", codeid(), &test_val););
    
    /* create read-only variable for test_t test_val */
    mod_add_dir(cmds, "t1", dir_cstruct_new(&FTL_TSPEC(test_t),
					    /*is_const*/TRUE, &test_val));
    
    /* create read-write variable for test_t test_val */
    mod_add_dir(cmds, "t2", dir_cstruct_new(&FTL_TSPEC(test_t),
					    /*is_const*/FALSE, &test_val));

    /* create variable to access array mymac_t mymac_val */
    mod_add_dir(cmds, "mac6", dir_carray_new(&FTL_TSPEC(mymac_t),
					     /*is_const*/ FALSE, &mymac_val,
					     FTL_ARRAY_STRIDE(mymac_val)));
    
    /* create variable to access array testarray_t array_val */
    mod_add_dir(cmds, "vec3", dir_carray_new(&FTL_TSPEC(testarray_t), 
					     /*is_const*/ FALSE, &array_val,
					     FTL_ARRAY_STRIDE(array_val)));

    return TRUE;
}




static void
cmds_ftl_end(parser_state_t *state)
{   data_end();
}


#endif






/*****************************************************************************
 *                                                                           *
 *          FTL Code						             *
 *          ========                                                         *
 *                                                                           *
 *****************************************************************************/




const char prolog_text[] =  /* ends with 'text end - penv_text' comment */
#include "ftl_fns.str"
    ;




/*****************************************************************************
 *                                                                           *
 *          Other FTL LIbraries						     *
 *          ===================					             *
 *                                                                           *
 *****************************************************************************/





/*! Load standard libraries, including those that are compile-time options
 */
static bool ftl_libs_init(parser_state_t *state)
{
    bool ok = TRUE;
    
    DEBUG_CLI(fprintf(stderr, "%s: adding tool commands\n", CODEID););
    if (!cmds_ftl(state))
    {   fprintf(stderr, "%s: failed to load application-specific "
                "commands\n", CODEID);
        ok = FALSE;
    }

#ifdef USE_FTLLIB_ELF
    if (ok)
    {   dir_t *cmds = parser_root(state);
        dir_t *elf_cmds = dir_id_new();
        DEBUG_CLI(fprintf(stderr, "%s: adding ELF commands\n", CODEID););
        if (cmds_elf(state, elf_cmds))
            mod_add_dir(cmds, FTL_DIR_CMDS_ELF, elf_cmds);
        else
        {   fprintf(stderr, "%s: failed to load ELF commands\n", CODEID);
            ok = FALSE;
        }
    }
#endif

#ifdef USE_FTLLIB_XML
    if (ok)
    {   dir_t *cmds = parser_root(state);
        dir_t *xml_cmds = dir_id_new();
        DEBUG_CLI(fprintf(stderr, "%s: adding XML commands\n", CODEID););
        if (cmds_xml(state, elf_cmds))
            mod_add_dir(cmds, FTL_DIR_CMDS_XML, xml_cmds);
        else
        {   fprintf(stderr, "%s: failed to load XML commands\n", CODEID);
            ok = FALSE;
        }
    }
#endif

    return ok;
}



static void ftl_libs_end(parser_state_t *state)
{
#ifdef USE_FTLLIB_ELF
    cmds_elf_end(state);
#endif

#ifdef USE_FTLLIB_XML
    cmds_xml_end(state);
#endif

    cmds_ftl_end(state);
}






/*****************************************************************************
 *                                                                           *
 *          Main Program						     *
 *          ============					             *
 *                                                                           *
 *****************************************************************************/





typedef struct optresult_s
{
    bool badopt;
} optresult_t;




/* register the value resvaling from the execution of a prolog option handling
 * routine
 */
static bool option_result(parser_state_t *state, const char *cmd,
                          const value_t *resval, void *arg)
{   bool ok = TRUE;
    optresult_t *result_arg = (optresult_t *)arg;

#if 0
    printf("OPTION %s result: ", cmd);
    value_fprint(stdout, dir_value(parser_root(state)), resval);
    fprintf(stdout, "\n");
#endif
        
    if (resval == value_false)
        result_arg->badopt = TRUE;
    
    return ok;
}




/*! Read a prolog file from a string
 */
static int /*rc*/ read_prolog(parser_state_t *state,
                              const char *sourceid, const char *prolog_file,
                              const char *prolog_text)
{
    const char *tool_path = getenv(ENV_PATH);
    int exit_rc = EXIT_OK;

    if (NULL != prolog_file && prolog_file[0] != '\0')
    {   charsource_t *fin =
           charsource_file_path_new(tool_path, prolog_file,
                                    strlen(prolog_file));
        if (NULL == fin)
            printf("%s: can't find file '%s' for %s on path\n",
                  codeid(), prolog_file, sourceid);
        else
        {
            if (NULL == parser_expand_exec(state, fin, NULL, NULL,
                                            /*no_locals*/TRUE))
            {  fprintf(stderr,
                       "%s: attempted execution of file \"%s\" for %s failed\n",
                       codeid(), prolog_file, sourceid);
               exit_rc = EXIT_BAD_PRLGFILE;
            }
            //charsource_delete(&fin);
        }
    } else

    {   charsource_t *prolog =
            charsource_cstring_new(sourceid, prolog_text, strlen(prolog_text));
        DEBUG_CLI(printf("%s: checking prolog '%s'\n", codeid(), sourceid););
        if (NULL == prolog)
            printf("%s: couldn't open text prolog\n", codeid());
        else
        {
            if (NULL == parser_expand_exec(state, prolog, NULL, NULL,
                                            /*no_locals*/TRUE))
            {  fprintf(stderr, "%s: attempted execution of "
                       "text prolog '%s' failed\n", codeid(), sourceid);
               exit_rc = EXIT_BAD_PRLGSTR;
            }
            DEBUG_CLI(printf("%s: prolog '%s' expanded\n",
                             codeid(), sourceid););
            //charsource_delete(&prolog);
        }
    }
    return exit_rc;
}



extern int
main(int argc, char **argv)
{   const char *err;
    const char *init = NULL;
    const char *cmd_file = NULL;
    const char *app_argv[APP_ARGC_MAX];
    int app_argc;
    bool echo_lines = FALSE;
    bool do_prolog = TRUE;
    FILE *echo_log = stdout;
    bool quiet = FALSE;
    int exit_rc = EXIT_OK;
    
    ftl_init();

    err = parse_args(argc, argv,&app_argc, &app_argv[0], APP_ARGC_MAX,
		     &init, &cmd_file, &echo_lines, &do_prolog, &quiet);
                 
    if (NULL != err)
    {   usage(err);
        exit_rc = EXIT_BAD_APPOPT;
    }
    else
    {   parser_state_t *state = parser_state_new(dir_id_new());

	if (NULL == state)
            exit_rc = EXIT_BAD_INIT;
        else
	{   parser_echo_setlog(state, echo_lines? echo_log: NULL, "> %s\n");
	    cmds_generic(state, app_argc, &app_argv[0]);
            DEBUG_CLI(printf("%s: opened internal commands\n", codeid()););
	    if (!ftl_libs_init(state))
            {
                exit_rc = EXIT_BAD_FTLINIT;
            }
            else
	    {	const char **opt_argv = &app_argv[1];
                int opt_argc = app_argc-1;
                bool do_args = opt_argc > 0;

                if (do_prolog)
                {   exit_rc = read_prolog(state, "prolog", getenv(ENV_PROLOG),
                                          &prolog_text[0]);
                }

                if (exit_rc == EXIT_OK && do_args)
                {   // parse --<option> ..
                    const value_t *opt_parse_dirval =
                        dir_string_get(parser_env(state), FTL_DIR_OPTIONS);
                    
                    DEBUG_CLI(fprintf(stderr, "%s: %sparsing options%s\n",
                                      codeid(),
                                      opt_parse_dirval == NULL? "not ":"",
                                      opt_parse_dirval == NULL?
                                      " (no " FTL_DIR_OPTIONS " obj)":""););
                    if (opt_parse_dirval != NULL)
                    {
                        optresult_t arg;
                        arg.badopt = FALSE;

                        if (!argv_opt_cli(state, CODEID, /*execpath*/NULL,
                                          &opt_argv, &opt_argc,
                                          (dir_t *)opt_parse_dirval,
                                          &option_result, (void *)&arg))
                            exit_rc = EXIT_BAD_FTLOPT;
                        else
                            if (arg.badopt)
                                exit_rc = EXIT_BAD_FTLOPT;
                                
                    }
                    do_args = opt_argc > 0;
                }

                if (exit_rc == EXIT_OK)
                {
                    DEBUG_CLI(fprintf(stderr, "%s: init commandline commands\n",
                                       codeid()););
                    if (NULL != cmd_file)
                    {   charsource_t *fin =
                            charsource_file_path_new(getenv(ENV_PATH), cmd_file,
                                                     strlen(cmd_file));
                        if (NULL == fin)
                        {   printf("%s: can't find '%s' on path\n",
                                   CODEID, cmd_file);
                            exit_rc = EXIT_BAD_CMDFILE;
                        } else
                        if (NULL == parser_expand_exec(state, fin, NULL, NULL,
                                                       /*no_locals*/TRUE))
                        {   fprintf(stderr, "%s: attempted execution of "
                                    "file \"%s\" failed\n",
                                    codeid(), cmd_file);
                            exit_rc = EXIT_BAD_CMDFRUN;
                        }
                    } else if (do_args)
                    {
                        DEBUG_CLI(fprintf(stderr,
                                          "%s: executing %d cmd line args\n",
                                           codeid(), app_argc););
                        if (!argv_cli(state, codeid(), getenv(ENV_PATH),
                                      opt_argv, opt_argc))
                            exit_rc = EXIT_BAD_FTLCMDS;
                    } else
                    {   if (!quiet)
                        {   int ftlmaj, ftlmin, ftldebug;
                            ftl_version(&ftlmaj, &ftlmin, &ftldebug);
                            printf("%s: v%s:%d.%d%s\n",
                                   CODEID, VERSION, ftlmaj, ftlmin,
#ifdef NDEBUG
                                   ""
#else
                                   " (debug)"
#endif
                           );
                        }
                        cli(state, init, CODEID);

                        if (!quiet)
                            printf("%s: finished\n", CODEID);
                    }
                }
	        ftl_libs_end(state);
	    }
	    parser_state_free(state);
	}
    }
    
    DEBUG_CLI(printf(CODEID " ending FTL\n"););
    ftl_end();
    DEBUG_CLI(printf(CODEID " exit rc %d\n", exit_rc););
    return exit_rc;
}




/*! \cidoxg_end */

/*
 * Local variables:
 *  c-basic-offset: 4
 *  c-indent-level: 4
 *  tab-width: 8
 * End:
 */
