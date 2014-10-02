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

/*! \cidoxg_tests_cplane */





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




/*****************************************************************************
 *                                                                           *
 *          Configuration                                                    *
 *          =============						     *
 *                                                                           *
 *****************************************************************************/




#define VERSION_MAJ 1
#define VERSION_MIN 0

#define ENV_PATH "FTL_PATH"

/* #define TEST_FTLSTRUCT */
/* #define USE_READLINE */

#define APP_ARGC_MAX 64

#ifndef IN_CPLANE

#define STATIC_INLINE static inline
#define cplane_assert assert

#else

#ifndef USER_LIVE
#define __ci_ul_driver__  /* we are using the user-mode driver */
#endif /* USER_LIVE */
#include <ci/tools.h>
#define STATIC_INLINE ci_inline
#define cplane_assert ci_assert

#endif /* IN_CPLANE */


#define CATDELIM(a, delim, b) #a  delim  #b
#define VERSIONSTR(max, min) CATDELIM(max, ".", min)
#define VERSION VERSIONSTR(VERSION_MAJ, VERSION_MIN) 





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

/* #define DEBUGGC DO */

#ifdef NDEBUG
#undef DEBUGGC
#endif

#ifndef DEBUGGC
#define DEBUGGC      IGNORE
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
    fprintf(stderr, "     -[n]e | --[no]echo - "
		    "[don't] echo executed commands\n");
    fprintf(stderr, "     -q | --quiet       - "
	            "don't report unnecessary info\n");
    exit(1);
}







static const char *
parse_args(int argc, char **argv,
	   int *out_argc, const char **out_argv, size_t out_argv_len,
	   const char **ref_cmd, const char **ref_input,
	   bool *out_echo, bool *out_quiet)
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
		{   unsigned int seed = strtoul(argv[argn], NULL, 0);
		    IGNORE(DPRINTF("seed: %d\n", seed));
		    srand(seed);
		} else
		    err = "ran out of arguments";
	    } else
	    if ((parse_key(&arg, "-q") || parse_key(&arg, "--quiet")) &&
		parse_empty(&arg))
		*out_quiet = TRUE;
	    else
	    if ((parse_key(&arg, "-e") || parse_key(&arg, "--echo")) &&
		parse_empty(&arg))
		*out_echo = TRUE;
	    else
	    if ((parse_key(&arg, "-ne") || parse_key(&arg, "--noecho")) &&
		parse_empty(&arg))
		*out_echo = TRUE;
	    else
	    if (parse_key(&arg, "--") && parse_empty(&arg))
		localargs = FALSE;
	    else
	    {   out_argv[out_argn++] = argv[argn];
                localargs = FALSE;
            }
#if 0
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


#define ARRAY_MYMAC(ctx) \
    FTL_TARRAY_BEGIN(ctx, mymac_t)                      \
    FTL_TARRAY_INT(ctx, mymac_t, unsigned char, 6)	\
    FTL_TARRAY_END(ctx)                                 \


#define STRUCT_SUBTEST(ctx)					\
    FTL_TSTRUCT_BEGIN(ctx, subtest_t,)				\
    FTL_TFIELD_INT(ctx, subtest_t, int, a)			\
    FTL_TFIELD_INT(ctx, subtest_t, int, b)			\
	FTL_TFIELD_ARRAYOFINT(ctx, subtest_t, short, n, 4)	\
    FTL_TSTRUCT_END(ctx)


#define ARRAY_SUBTEST(ctx) \
    FTL_TARRAY_BEGIN(ctx, subtest_array_t)                      \
    FTL_TARRAY_STRUCT(ctx, subtest_array_t, subtest_t, 4)	\
    FTL_TARRAY_END(ctx)						\


#define STRUCT_TEST(ctx) \
    FTL_TSTRUCT_BEGIN(ctx, test_t, )				\
    FTL_TFIELD_INT(ctx, test_t, unsigned, fa)    		\
    FTL_TFIELD_INT(ctx, test_t, char, fb) 			\
    FTL_TFIELD_INT(ctx, test_t, number_t, fc)    		\
    FTL_TFIELD_STRUCT(ctx, test_t, subtest_t, fd)		\
    FTL_TFIELD_ARRAYOFSTRUCT(ctx, test_t, subtest_t, x, 4)      \
    FTL_TSTRUCT_END(ctx)


#define ARRAY_TEST(ctx) \
    FTL_TARRAY_BEGIN(ctx, testarray_t)				\
    FTL_TARRAY_STRUCT(ctx, testarray_t, test_t, 2)		\
    FTL_TARRAY_END(ctx)						\



FTL_DECLARE(ARRAY_MYMAC)
FTL_DECLARE(STRUCT_SUBTEST)
FTL_DECLARE(ARRAY_SUBTEST)
FTL_DECLARE(STRUCT_TEST)
FTL_DECLARE(ARRAY_TEST)

static void data_init(void)
{   FTL_DEFINE(ARRAY_MYMAC)
    FTL_DEFINE(STRUCT_SUBTEST)
    FTL_DEFINE(ARRAY_SUBTEST)
    FTL_DEFINE(STRUCT_TEST)
    FTL_DEFINE(ARRAY_TEST)
}

static void data_end(void)
{   FTL_UNDEFINE(ARRAY_MYMAC)
    FTL_UNDEFINE(STRUCT_SUBTEST)
    FTL_UNDEFINE(ARRAY_SUBTEST)
    FTL_UNDEFINE(STRUCT_TEST)
    FTL_UNDEFINE(ARRAY_TEST)
}

static test_t       test_val;
static mymac_t      mymac_val;
static testarray_t  array_val;



static bool
cmds_ftl(parser_state_t *state)
{   dir_t *cmds = parser_env(state);

    data_init();

    memset(&test_val, 0xff, sizeof(test_val));
    IGNORE(printf("%s: test val at %p\n", codeid(), &test_val);)
    mod_add_dir(cmds, "t1", dir_cstruct_new(&FTL_TSPEC(test_t),
					    /*is_const*/TRUE, &test_val));
    mod_add_dir(cmds, "t2", dir_cstruct_new(&FTL_TSPEC(test_t),
					    /*is_const*/FALSE, &test_val));
    mod_add_dir(cmds, "mac6", dir_carray_new(&FTL_TSPEC(mymac_t),
					     /*is_const*/ FALSE, &mymac_val,
					     FTL_ARRAY_STRIDE(mymac_val)));
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
 *          Main Program						     *
 *          ============					             *
 *                                                                           *
 *****************************************************************************/






extern int
main(int argc, char **argv)
{   const char *err;
    const char *init = NULL;
    const char *cmd_file = NULL;
    const char *app_argv[APP_ARGC_MAX];
    int app_argc;
    bool echo_lines = FALSE;
    bool quiet = FALSE;
    
    ftl_init();
    
    if (NULL == (err = parse_args(argc, argv,
				  &app_argc, &app_argv[0], APP_ARGC_MAX,
				  &init, &cmd_file, &echo_lines, &quiet)))
    {   parser_state_t *state = parser_state_new(dir_id_new());

	if (NULL != state)
	{   parser_echo_set(state, echo_lines);
	    cmds_generic(state, app_argc, &app_argv[0]);
	    if (cmds_ftl(state))
	    {	bool do_args = app_argc > 1;

                if (NULL != cmd_file)
		{   charsource_t *fin =
                        charsource_file_path_new(getenv(ENV_PATH),
                                                 cmd_file, strlen(cmd_file));
                    if (NULL == fin)
                        printf("%s: can't find '%s' on path\n",
                               CODEID, cmd_file);
                    else if (NULL == parser_expand_exec(state, fin, NULL, NULL,
                                                        /*no_locals*/TRUE))
		    {   fprintf(stderr, "%s: attempted execution of "
				"file \"%s\" failed\n",
				codeid(), cmd_file);
		    }
		} else if (do_args)
                {   argv_cli(state, CODEID, getenv(ENV_PATH),
                             (const char **)&app_argv[1], app_argc-1);
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
	        cmds_ftl_end(state);
	    } else
		fprintf(stderr, "%s: failed to load application-specific "
			"commands\n", CODEID);
	    parser_state_free(state);
	}
    } else
	usage(err);
    
    ftl_end();
    return 0;
}




/*! \cidoxg_end */
