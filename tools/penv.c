/*
 * Copyright (c) 2005-2009, Solarflare Communications Inc.
 * Copyright (C) 2011, Gray Girling
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
** \author  cgg
**  \brief  tools/penv	Permanent environment 
**   \date  2011/06
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
#include "ftl_internal.h"




/*****************************************************************************
 *                                                                           *
 *          Configuration                                                    *
 *          =============						     *
 *                                                                           *
 *****************************************************************************/




#define VERSION_MAJ 0
#define VERSION_MIN 1

#define ENV_PATH "PENV_PATH"

#define APP_ARGC_MAX 64

#define STATIC_INLINE static inline

#define CATDELIM(a, delim, b) #a  delim  #b
#define VERSIONSTR(max, min) CATDELIM(max, ".", min)
#define VERSION VERSIONSTR(VERSION_MAJ, VERSION_MIN) 

#define PENV_FILE_MAXLEN 256




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


#define CODEID "penv"






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
    fprintf(stderr, "     --noprofile | -np  - don't run internal setup code\n");
    fprintf(stderr, "     -[n]e | --[no]echo - "
		    "[don't] echo executed commands\n");
    fprintf(stderr, "     -q | --quiet       - "
	            "don't report unnecessary info\n");
    exit(1);
}







static const char *
parse_args(int argc, const char **argv,
	   int *out_argc, const char **out_argv, size_t out_argv_len,
	   const char **ref_cmd, const char **ref_input, bool* out_doprofile,
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
	    if ((parse_key(&arg, "-np") || parse_key(&arg, "--noprofile")) &&
		parse_empty(&arg))
		*out_doprofile = FALSE;
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
 *          Dir File Representations 			                     *
 *          =========================  			                     *
 *                                                                           *
 *****************************************************************************/





/* Functions for reading and writing a given file representation of a
 * directory */



typedef void *repn_enum_fn(const char *name, const char *value, void *arg);

typedef bool repn_parse_fn(charsource_t *fin,
                           repn_enum_fn *enum_fn, void *enum_arg);
typedef bool repn_out_init_fn(charsink_t *fout);
typedef bool repn_out_binding_fn(charsink_t *fout,
                                 const char *key, size_t keylen,
                                 const char *val, size_t vallen);
typedef bool repn_out_end_fn(charsink_t *fout);


typedef struct file_rep_s {
   repn_parse_fn *parse;
   repn_out_init_fn *out_init;
   repn_out_binding_fn *out_binding;
   repn_out_end_fn *out_end;
} file_rep_t;









/*****************************************************************************
 *                                                                           *
 *          Name-value File Representation      	                     *
 *          ==============================		                     *
 *                                                                           *
 *****************************************************************************/






/* In this representation the name and value must be strings but the value must
   not contain a space or a newline and the value must not contain a newline.
   The layout is simply to place each name and value on each line of the text
   file as:
         <name><space><value><newline>
   There is a limit on the length of the line so constructed which is
   PENV_FILE_MAXLEN octets.
*/





static bool
nvline_parse(charsource_t *fin, repn_enum_fn *fn, void *dir_enum_arg)
{   int ch;
    char linebuf[PENV_FILE_MAXLEN];
    char *p = &linebuf[0];
    char *pend = p + sizeof(linebuf);
    char *key = p;
    char *value = NULL;
    int badline = 0;
	
    while (EOF != (ch = charsource_getc(fin)))
    {   if (value == NULL && isspace(ch))
        {   ch = '\0';
            value = p+1;
        }
        if (ch == '\n')
        {   *p = '\0';
            if (NULL != (*fn)(key, value, dir_enum_arg))
               badline++;
            p = &linebuf[0];
            key = p;
            value = NULL;
        } else if (p < pend-1)
            *p++ = ch;
        else
            badline++;
    }

    return badline == 0;
}





static bool nvline_out_init(charsink_t *fout)
{
    return TRUE;
}

   



static bool nvline_out_binding(charsink_t *fout,
                               const char *key, size_t keylen,
                               const char *val, size_t vallen)
{   bool ok = (NULL == strchr(key, '\n')) && (NULL == strchr(key, ' ')) &&
              (NULL == strchr(val, '\n'));
    /* TODO: the above is wrong for non-null terminated strings */
    if (ok)
    {
        charsink_write(fout, key, keylen);
        charsink_putc(fout, ' ');
        charsink_write(fout, val, vallen);
        charsink_putc(fout, '\n');
    }    
    return ok;
}





static bool nvline_out_end(charsink_t *fout)
{
    return TRUE;
}





static file_rep_t nvline_repn = {
   &nvline_parse,
   &nvline_out_init,
   &nvline_out_binding,
   &nvline_out_end
};


   




/*****************************************************************************
 *                                                                           *
 *          Permanent Env Directories			                     *
 *          =========================			                     *
 *                                                                           *
 *****************************************************************************/




#if 0




typedef struct 
{   dir_id_t cache;    /* cache of values */
    file_rep_t *repn;  /* represenation */
    value_t *stream;   /* read/write stream */
} dir_penv_t;






#define dir_penv_dir(dirstruct) dir_id_dir(&((dirstruct)->get_cache))
#define dir_penv_value(dirstruct) dir_value(dir_penv_dir(dirstruct))






static void *
file_rep_marshal_enum(dir_t *dir,
                      const value_t *name, const value_t *value, void *arg)
{   charsink_t *fout = (charsink_t *)arg;
    file_rep_t *repn = penvdir->repn;
    
    
}




static bool 
dir_penv_marshal(parser_state_t *state, dir_penv_t *penvdir,
                 const char* filename)
{
    value_t *stream_out = value_stream_file_new(filename, /*read*/FALSE,
                                                /*write*/TRUE);
    bool ok = stream_out != NULL;

    if (ok) {
        file_rep_t repn = penvdir->repn;
        charsink_t *fout;
        bool ok = value_stream_sink(stream_out, &fout);

        if (ok && repn->out_init(fout)) {

            dir_state_forall(penvdir, state,
                             &file_rep_marshal_enum, (void *)stream_out);

            if (!repn->out_end(fout))
                ok = FALSE;
        }
        value_stream_close(stream_out);
    }
    return ok;
}






static void *
file_rep_marshal_enum(dir_t *dir,
                      const value_t *name, const value_t *value, void *arg)
{   charsource_t *fin = (charsource_t *)arg;
    file_rep_t repn = penvdir->repn;
    
    
}




static bool
file_rep_unmarshal(parser_state_t *state, file_rep_t *, value_stream_t *stream)
    value_t *stream_in = value_stream_file_new(filename, /*read*/TRUE,
                                               /*write*/FALSE);
    bool ok = stream_in != NULL;

    if (ok) {
        charsource_t *fin;
        bool ok = value_stream_source(stream_in, &fin);

        dir_state_forall(state, penvdir,
                         &file_rep_unmarshal_enum, (void *)stream_in);
        value_stream_close(stream_in);
    }
    return ok;
}







static void
dir_penv_delete(value_t *value)
{   dir_penv_t *penvdir = (dir_penv_t *)value;
    if (NULL != penvdir->stream)
    {   free(penvdir->stream);
	penvdir->stream = NULL;
    }
    penvdir->ref = (const value_t *)NULL;
    dir_id_delete(dir_id_value(&penvdir->cache));
}
    



static void
dir_penv_markver(const value_t *value, int heap_version)
{   dir_penv_t *penvdir = (dir_penv_t *)value;
    IGNORE(printf("%s: marking penv dir %p ref %p\n",
	          codeid(), penvdir, penvdir->ref);)
    value_mark_version((value_t *)penvdir->ref, heap_version);
    dir_id_markver(dir_id_value(&penvdir->cache), heap_version);
}





static void
dir_penv_init(dir_penv_t *penvdir, file_rep_t *file_representation)
{   dir_t *dir = dir_penv_dir(structdir);
    dir_id_init(&penvdir->cache);
    penvdir->repn = file_representation;
    penvidr->stream = NULL;
    dir_value(dir)->delete_fn = &dir_penv_delete;
    dir_value(dir)->mark_version = &dir_penv_markver;
}






extern dir_t *
dir_penv_lnew(parser_state_t *state, file_rep_t *file_representation)
{   dir_penv_t *penvdir =
        (dir_penv_t *)value_local_alloc(state, sizeof(dir_penv_t));

    if (NULL != penvdir)
       dir_penv_init(penvdir, file_representation);

    return &penvdir->dir;
}




#endif






/*****************************************************************************
 *                                                                           *
 *          PENV Functions						     *
 *          ==============				                     *
 *                                                                           *
 *****************************************************************************/







typedef struct 
{   file_rep_t *repn;      /* represenation */
    charsink_t *stream;    /* write stream */
} file_rep_enum_t;





static void *
file_rep_marshal_enum(dir_t *dir,
                      const value_t *name, const value_t *value, void *arg)
{   file_rep_enum_t *enum_arg = (file_rep_enum_t *)arg;
    charsink_t *fout = enum_arg->stream;
    file_rep_t *repn = enum_arg->repn;
    const char *namestr;
    size_t namelen;
    const char *valstr;
    size_t vallen;
    bool ok = FALSE;

    if (value_string_get(name, &namestr, &namelen) &&
        value != &value_null &&
        value_string_get(value, &valstr, &vallen))
    {
        ok = repn->out_binding(fout, namestr, namelen, valstr, vallen);
    }   

    return NULL;
}






/*! write a directory to a stream */
/* This function may cause a garbage collection */
static const value_t *
fn_dir_marshal(const value_t *this_fn, parser_state_t *state)
{   file_rep_t *repn = &nvline_repn;
    value_t *varstream = (value_t *)parser_builtin_arg(state, 1);
    value_t *dirval = (value_t *)parser_builtin_arg(state, 2);
    dir_t *dir;
    const value_t *val = &value_null;
    charsink_t *fout;
    bool ok = FALSE;

    if (value_as_dir(dirval, &dir) &&
        value_stream_sink(varstream, &fout))
    {
        if (repn->out_init(fout)) {
            file_rep_enum_t enum_args;

            enum_args.stream = fout;
            enum_args.repn = repn;

            dir_state_forall(state, dir,
                             &file_rep_marshal_enum, (void *)&enum_args);

            ok = repn->out_end(fout);
        }
        value_stream_close(varstream);
    }
    return ok? value_true: value_false;
}






static void *
file_rep_unmarshal_enum(const char *name, const char *value, void *arg)
{   dir_t *dir = (dir_t *)arg;

    dir_set(dir,
            value_string_new_measured(name), value_string_new_measured(value));
    
    return NULL;
}





/*! read a directory from a stream */
static const value_t *
fn_dir_unmarshal(const value_t *this_fn, parser_state_t *state)
{   file_rep_t *repn = &nvline_repn;
    value_t *varstream = (value_t *)parser_builtin_arg(state, 1);
    dir_t *dir = dir_id_new();
    const value_t *val = &value_null;
    charsource_t *fin;
        
    if (value_stream_source(varstream, &fin))
    {   repn->parse(fin, &file_rep_unmarshal_enum, (void *)dir);
        value_stream_close(varstream);
        val = dir_value(dir);
    }
    
    return val;
}









/*****************************************************************************
 *                                                                           *
 *          PENV Application						     *
 *          ================				                     *
 *                                                                           *
 *****************************************************************************/





#define FNS_DIR "envfile"





static bool
cmd_penv(parser_state_t *state)
{   dir_t *cmds = parser_env(state);
    dir_t *pcmds = dir_id_new();
    
    mod_addfn(pcmds, "marshal",
	      "<stream> <dir> - write dir to stream",  &fn_dir_marshal, 2);
    mod_addfn(pcmds, "unmarshal",
	      "<stream> - read dir from stream",  &fn_dir_unmarshal, 1);
    mod_add_val(pcmds, "onexit", &value_null);
    
    mod_add_dir(cmds, FNS_DIR, pcmds);
    
    return TRUE;
}






static void
cmd_penv_end(parser_state_t *state)
{   dir_t *cmds = parser_env(state);
    const value_t *pcmdsval = dir_string_get(cmds, FNS_DIR);

    if (value_type_equal(pcmdsval, type_dir)) {
        dir_t *pcmds = (dir_t *)pcmdsval;
        const value_t *onexit = dir_string_get(pcmds, "onexit");
 
        if (NULL != onexit)
        {   if (&value_null != onexit)
                invoke(onexit, state);
        } else
            printf("%s: error - lost '%s.onexit' function\n", CODEID, FNS_DIR);
    } else
        printf("%s: error - lost '%s' functions\n", CODEID, FNS_DIR);

}






/*****************************************************************************
 *                                                                           *
 *          FTL Code						             *
 *          ========                                                         *
 *                                                                           *
 *****************************************************************************/




const char penv_text[] =  /* ends with 'text end - penv_text' comment */
    "# Cached permanent environment directories\n"
    "#\n"
    "# Assuming this file is in ~/.penv/penv_fns, tThis file can be copied into a\n"
    "# long string suitable for incorporating into penv.c using the command:\n"
    "#\n"
    "#        ./penv -- io filetostring ~/.penv/penv_fns\n"
    "#\n"
    "# To test a new version use\n"
    "#\n"
    "#        penv -np -c \"source \\\"penv_fns\\\"\"\n"
    "\n"
    "# output\n"
    "\n"
    "set codeid \"penv\"\n"
    "\n"
    "set verbose FALSE\n"
    "\n"
    "set printf[fmt, args]:{\n"
    "   io.fprintf io.out fmt args!;\n"
    "}\n"
    "\n"
    "set errorf[fmt, args]:{\n"
    "   io.fprintf io.err (\"\"+(codeid)+\": \"+(fmt)+\"\\n\") args!;\n"
    "}\n"
    "\n"
    "set vecho[msg]:{\n"
    "    verbose {printf \"%s\\n\" <msg>!;}!;\n"
    "}\n"
    "\n"
    "\n"
    "# File name operations\n"
    "\n"
    "set deext[name]: { .pts = split \".\" name!; if (len pts!) gt 0 {pts.0}{\"\"}! }\n"
    "\n"
    "set depush[v]: {\n"
    "    .ln = len v!;\n"
    "    if (ln le 1) {<>} {v.<0..ln-2>}!\n"
    "}\n"
    "set objdir[objname]:{\n"
    "    join \"/\" (depush (split \"/\" objname!)!)!\n"
    "}\n"
    "set dirname[file]:{\n"
    "    join sys.fs.sep (depush (split sys.fs.sep file!)!)!\n"
    "}\n"
    "\n"
    "\n"
    "# Permanent environment files\n"
    "\n"
    "set pfile [changed=FALSE, file=NULL, dir=[]]\n"
    "\n"
    "set pcache []\n"
    "\n"
    "# return TRUE if a file with the required name exists after the call\n"
    "set touch[pf, ifthere]:{\n"
    "    #errorf \"touch %s\" <pf>!;#CGG\n"
    "    .str = io.file pf \"r\"!;\n"
    "    if (str == NULL) {\n"
    "        if NULL != (str = io.file pf \"w\"!) {\n"
    "            io.close str!; TRUE\n"
    "        }{ FALSE }!\n"
    "    }{ io.close str!; ifthere }!\n"
    "}\n"
    "\n"
    "# return TRUE if a file with the required name exists after the call\n"
    "set pfile_create[pf, ifthere]:{\n"
    "    #errorf \"create %s\" <pf>!;#CGG\n"
    "    .rc = (sys.runrc \"mkdir -p \\\"\"+(dirname pf!)+\"\\\"\"!);\n"
    "    if (rc == 0) { touch pf ifthere! }{ FALSE }!\n"
    "}\n"
    "\n"
    "\n"
    "set pfile_open [pf,verbose]: {\n"
    "   # printf \"open %v\\n\" <pf>!;\n"
    "   if (inenv pcache pf!) { pcache.(pf) } {\n"
    "      .str = io.file pf \"r\"!;\n"
    "      if (str == NULL) {\n"
    "         verbose {errorf \"can\'t open environment file \'%v\' to read\" <pf>!}!;\n"
    "         NULL\n"
    "      }{\n"
    "         .n = new pfile!;\n"
    "         n.file = pf;\n"
    "         n.dir = envfile.unmarshal str!;\n"
    "         io.close str!;\n"
    "         pcache.(pf) = n\n"
    "      }!\n"
    "   }!\n"
    "}\n"
    "\n"
    "\n"
    "# return opened object in file only if file has to be created\n"
    "set pfile_open_new[pf,verbose]:{\n"
    "    if (pfile_create pf FALSE!) { pfile_open pf verbose! } { NULL }!\n"
    "}\n"
    "\n"
    "# return opened object in file even if file has to be created\n"
    "set pfile_open_create[pf,verbose]:{\n"
    "    if (pfile_create pf TRUE!) { pfile_open pf verbose! } { NULL }!\n"
    "}\n"
    "\n"
    "\n"
    "set pfile_flush[obj]: {\n"
    "   obj.changed {\n"
    "       .str = io.file obj.file \"w\"!;\n"
    "       if (str == NULL) {\n"
    "           errorf \"can\'t open environment file \'%s\' to write\" <obj.file>!;\n"
    "       }{\n"
    "           if (envfile.marshal str obj.dir!) {\n"
    "               # errorf \"wrote %s\" <obj.file>!;#CGG\n"
    "               obj.changed = FALSE;\n"
    "           } {\n"
    "               errorf \"failed to write environment file \'%s\'\" <pf>!;\n"
    "           }!;\n"
    "           io.close str!;\n"
    "       }!\n"
    "   }!\n"
    "}\n"
    "\n"
    "\n"
    "set pcache_flush[]:{\n"
    "    forall pcache [obj]:{ pfile_flush obj! }!\n"
    "}\n"
    "\n"
    "\n"
    "# This is always called when we exit:\n"
    "set envfile.onexit[]:{ pcache_flush! }\n"
    "\n"
    "\n"
    "# Library of useful functions\n"
    "\n"
    "set tr[ch1,ch2,name]: { join ch2 (split ch1 name!)! }\n"
    "set trv[v,ch2,name]: { forall v [ch]:{ name = tr ch ch2 name! }!; name }\n"
    "\n"
    "# return the rest of v once the head has been removed\n"
    "set dequeue[v]: {\n"
    "    .ln = len v!; \n"
    "    if (ln le 1) {<>} {v.<1..ln-1>}!\n"
    "}\n"
    "\n"
    "set objbasename[objname]:{\n"
    "    .v = split \"/\" objname!;\n"
    "    .ln = len v!; \n"
    "    if (ln lt 1) {NULL} {v.(ln-1)}!\n"
    "}\n"
    "\n"
    "set typefile[tname]:{\n"
    "    join sys.fs.sep <dirname (dirname sys.env.OBJROOT!)!,\"type\",\n"
    "                     \"\"+(tname)+\".env\">!\n"
    "}\n"
    "\n"
    "set type_open[ty]:{\n"
    "    pfile_open (typefile ty!) TRUE!\n"
    "}\n"
    "\n"
    "set type_create[tname,tdescr]:{\n"
    "    .tobj = pfile_open_new (typefile tname!) TRUE!;\n"
    "    tobj != NULL {\n"
    "        tobj.dir.type = \"type\";\n"
    "        tobj.dir._name = tname;\n"
    "        tdescr != \"\" {tobj.dir.description = tdescr}!;\n"
    "        tobj.changed = TRUE;\n"
    "    }!;\n"
    "    tobj\n"
    "}\n"
    "\n"
    "set type_field[tobj,field,default]:{\n"
    "    default != \"\" {tobj.dir.(\"\"+\"_field_\"+(field)) = default;}!;\n"
    "    tobj.changed = TRUE;\n"
    "}\n"
    "\n"
    "set type_store[tobj,field,link,subtype]:{\n"
    "    type_field tobj (\"_\"+(field)+\"_vars\") link!;\n"
    "    subtype != \"\" {\n"
    "        type_field tobj (\"_\"+(field)+\"_type\") subtype!;\n"
    "    }!;\n"
    "}\n"
    "\n"
    "set beginswith[start,name]:{\n"
    "    .part = split start name!;\n"
    "    (len part!) gt 1 and part.0 == \"\"\n"
    "}\n"
    "\n"
    "\n"
    "set type_fields[tobj]:{\n"
    "   .fds=[];\n"
    "   forall tobj.dir [v,n]:{\n"
    "       .part = split \"_field_\" n!;\n"
    "       if part.0 == \"\" { fds.(part.1) = v }{}!;\n"
    "   }!;\n"
    "   fds\n"
    "}\n"
    "\n"
    "set type_instance[tobj,obj]:{\n"
    "    obj.dir = new (type_fields tobj!)!;\n"
    "    obj.changed = TRUE;\n"
    "    inenv tobj.dir \"name\"! {obj.dir._type = tobj.dir.name}!;\n"
    "    obj\n"
    "}\n"
    "\n"
    "\n"
    "set objfile_create[pf,typename]:{\n"
    "    .obj = pfile_open_create pf TRUE!;\n"
    "    NULL != obj and NULL != typename and \"\" != typename {\n"
    "        .tt = type_open typename!;\n"
    "        (NULL == tt) { errorf \"can\'t make new object of non-existant type \'%s\'\" <typename>!; }!;\n"
    "        (NULL != tt) { type_instance tt obj!; }!\n"
    "    }!;\n"
    "    obj\n"
    "}\n"
    "\n"
    "\n"
    "set path_extend[origvec,link,sel]:{\n"
    "    .linkvec = split \"/\" link!;\n"
    "    if ((len linkvec!) gt 0 {linkvec.0 == \"\"}!) {\n"
    "        .orig = \"\"+(join sys.fs.sep linkvec!)+(sel)+\".env\";\n"
    "        origvec = split sys.fs.sep orig!;\n"
    "    }{\n"
    "        #printf \"link starts %s, orig is %s\\n\" <link,join sys.fs.sep origvec!>!;\n"
    "        .found=FALSE;\n"
    "        while {(len linkvec!) gt 0 {linkvec.0 == \"..\"}!}{\n"
    "            linkvec = dequeue linkvec!;\n"
    "            origvec = depush origvec!;\n"
    "            found = TRUE;\n"
    "        }!;\n"
    "        #printf \"link ends %s, orig is %s\\n\" <join \"/\" linkvec!,join sys.fs.sep origvec!>!;\n"
    "        if found {\n"
    "            origvec = depush origvec!;\n"
    "            link = join \"/\" linkvec!;\n"
    "        }{\n"
    "            .ix = (len origvec!)-1;\n"
    "            (ix ge 0) {origvec.(ix) = deext origvec.(ix)!;}!;\n"
    "        }!;\n"
    "        #printf \"then orig is %s + %s%s\\n\" <join sys.fs.sep origvec!,link,sel>!;\n"
    "        origvec = split sys.fs.sep \"\"+(join sys.fs.sep origvec!)+(sys.fs.sep)+\n"
    "                                   (link)+(sel)+\".env\"!;\n"
    "    }!;\n"
    "    origvec\n"
    "}\n"
    "\n"
    "\n"
    "\n"
    "# typename=\"\" -> create with no name\n"
    "# else create as instance of type\n"
    "set filev_from_path[create, typename, rootvec, nv]:{\n"
    "    .cont = TRUE;\n"
    "    (len nv!) == 0 {nv = <\"\">}!;\n"
    "    .pathvec = new rootvec!;\n"
    "    while {cont} {\n"
    "        if (0==(len nv!)) { cont=FALSE; }{\n"
    "            .nme = nv.0;  nv = dequeue nv!;\n"
    "            .link = \"\"+(nme)+(sys.fs.sep);\n"
    "            .penvf = join sys.fs.sep pathvec!;\n"
    "            .obj = pfile_open penvf TRUE!;\n"
    "            (NULL == obj and create) {\n"
    "                objfile_create penvf typename!;\n"
    "                obj = pfile_open penvf TRUE!;\n"
    "            }!;\n"
    "            # echo (strf \"obj from %s = \'%v\'\" <penvf,obj>!)!;\n"
    "            if (NULL == obj) {\n"
    "                errorf \"can\'t read env file %s\" <penvf>!;\n"
    "                cont=FALSE; pathvec=NULL;\n"
    "            }{\n"
    "                .lnknme = \"_\"+(nme)+\"_vars\";\n"
    "                inenv obj.dir lnknme! { obj.dir.(lnknme) != \"\" }! {\n"
    "                    link = obj.dir.(lnknme)\n"
    "                }!;\n"
    "                .tynme = \"_\"+(nme)+\"_type\";\n"
    "                inenv obj.dir tynme! { typename = obj.dir.(tynme) }!;\n"
    "                # printf \"path base \'%s\'\\n\" <nme>!;\n"
    "                if (nme ==  \".\") {} {\n"
    "                    if (nme == \"\") { pathvec = rootvec } {\n"
    "                        if (inenv obj.dir nme!) {\n"
    "                           .sel = trv <\"/\", \":\", \",\", \".\"> \"_\" obj.dir.(nme)!;\n"
    "                           pathvec = path_extend pathvec link sel!;\n"
    "                           # echo (strf \"%s -> file \'%s\'\" <nme,join sys.fs.sep pathvec!>!)!;\n"
    "                        }{\n"
    "                            errorf \"key \'%s\' not found in env file %s\" <nme, penvf>!;\n"
    "                            cont=FALSE; pathvec=NULL;\n"
    "                        }!\n"
    "                    }!\n"
    "                }!\n"
    "            }!\n"
    "        }!\n"
    "    }!;\n"
    "    pathvec\n"
    "}\n"
    "\n"
    "\n"
    "# return NULL or a string containing the file name corresponding to the name\n"
    "# indexed from the given root\n"
    "set file_from_path[create, typename, root, name]:{\n"
    "    .pv = split sys.fs.sep root!;\n"
    "    .nv = split \"/\" name!;\n"
    "    pv = filev_from_path create typename pv nv!;\n"
    "    if (pv == NULL) {NULL} {join sys.fs.sep pv!}!\n"
    "}\n"
    "\n"
    "\n"
    "# return NULL or a string containing the file name corresponding to the name\n"
    "# indexed from OBJROOT\n"
    "set file_from_root[create, typename, name]: {\n"
    "    if (inenv sys.env \"OBJROOT\"!) {\n"
    "        file_from_path create typename sys.env.OBJROOT name!\n"
    "    } {\n"
    "        errorf \"environment variable OBJROOT unset\" <>!;\n"
    "        NULL\n"
    "    }!\n"
    "}\n"
    "\n"
    "\n"
    "set pname_file[name]:{\n"
    "    .file = file_from_root FALSE NULL name!;\n"
    "    if (file == NULL) {}{\n"
    "        printf \"%s\\n\" <file>!;\n"
    "    }!\n"
    "}\n"
    "    \n"
    "\n"
    "set pname_dir[name]:{\n"
    "    .file = file_from_root FALSE NULL name!;\n"
    "    if (file == NULL) {}{\n"
    "        printf \"%s\\n\" <dirname file!>!;\n"
    "    }!\n"
    "}\n"
    "    \n"
    "\n"
    "set pname_ls[all,name]:{\n"
    "    .file = file_from_root FALSE NULL name!;\n"
    "    if (file == NULL) {}{\n"
    "        .obj = pfile_open file FALSE!;\n"
    "        obj != NULL {\n"
    "            forall obj.dir [v,n]:{\n"
    "                if (not all {beginswith \"_\" n!}!) {} {\n"
    "                    printf \"%s %s\\n\" <n, v>!;\n"
    "                }!\n"
    "            }!\n"
    "        }!\n"
    "    }!\n"
    "}\n"
    "\n"
    "\n"
    "set pname_hasvalues[name]:{\n"
    "    .file = file_from_root FALSE NULL name!;\n"
    "    if (file == NULL) {FALSE}{\n"
    "        .obj = pfile_open file FALSE!;\n"
    "        (obj == NULL)\n"
    "    }!\n"
    "}\n"
    "\n"
    "\n"
    "# return NULL or string value set for penv name\n"
    "set pname_getval[name,verbose]:{\n"
    "    .od = objdir name!;\n"
    "    .key = objbasename name!;\n"
    "    .file = file_from_root FALSE NULL od!;\n"
    "    if (file == NULL) {NULL}{\n"
    "        .obj = pfile_open file verbose!;\n"
    "        if (obj == NULL) {NULL}{\n"
    "            if (inenv obj.dir key!) {\n"
    "                obj.dir.(key)\n"
    "            }{NULL}!\n"
    "        }!\n"
    "    }!\n"
    "}\n"
    "\n"
    "\n"
    "set pname_get[name]:{\n"
    "   .val = pname_getval name TRUE!;\n"
    "   val != NULL { printf \"%s\\n\" <val>!; }!;\n"
    "}\n"
    "\n"
    "\n"
    "set pname_set[name,valstr]:{\n"
    "    .od = objdir name!;\n"
    "    .key = objbasename name!;\n"
    "    .file = file_from_root FALSE NULL od!;\n"
    "    if (file == NULL) {}{\n"
    "        .obj = pfile_open_create file FALSE!;\n"
    "        obj == NULL {\n"
    "            pfile_create file!;\n"
    "            obj = pfile_open file TRUE!;\n"
    "        }!;\n"
    "        if (obj == NULL) {}{\n"
    "            if (inenv obj.dir key!) {\n"
    "               (obj.dir.(key) != valstr) { obj.changed = TRUE }!;\n"
    "            }{ obj.changed = TRUE;\n"
    "            }!;\n"
    "            obj.dir.(key) = valstr;\n"
    "        }!\n"
    "    }!\n"
    "}\n"
    "\n"
    "set pname_unset[name]:{\n"
    "    .od = objdir name!;\n"
    "    .key = objbasename name!;\n"
    "    .file = file_from_root FALSE NULL od!;\n"
    "    if (file == NULL) {}{\n"
    "        .obj = pfile_open file TRUE!;\n"
    "        obj != NULL {\n"
    "            inenv obj.dir key! {\n"
    "               obj.changed = (obj.dir.(key) != NULL);\n"
    "               obj.dir.(key) = NULL;\n"
    "            }!\n"
    "        }!\n"
    "    }!\n"
    "}\n"
    "\n"
    "#\n"
    "# command line argument handling\n"
    "#\n"
    "\n"
    "set split_args[line]:{\n"
    "   .left = <line>;\n"
    "   .args = <>;\n"
    "   .arg = \"\";\n"
    "   while {parse.scanwhite left!;\n"
    "          if (parse.scancode @arg left!){TRUE}{\n"
    "             parse.scanitemstr @arg left!\n"
    "          }!}{\n"
    "       args.(len args!) = arg;\n"
    "   }!;\n"
    "   if (parse.scanempty left!) {args}{\n"
    "       errorf \"unknown text on line - \'%s\'\" <line>!;\n"
    "       NULL\n"
    "   }!\n"
    "}\n"
    "       \n"
    "\n"
    "set get_argv[cmin,cmax,help,line]:{\n"
    "   .argv = split_args line!;\n"
    "   .ln = len argv!;\n"
    "   .ok = TRUE;\n"
    "   ln lt cmin {\n"
    "       ok = FALSE;\n"
    "       errorf \"too few arguments (%d), syntax - %s\" <ln,help>!;\n"
    "   }!;\n"
    "   ln gt cmax {\n"
    "       ok = FALSE;\n"
    "       errorf \"too many arguments (%d), syntax - %s\" <ln,help>!;\n"
    "   }!;\n"
    "   if ok {argv}{NULL}!\n"
    "}\n"
    "\n"
    "\n"
    "set cmd_1item[fn,help]:{\n"
    "   cmd [line]:{\n"
    "       .argv = get_argv 1 1 help line!;\n"
    "       argv != NULL {fn argv.0!}!;\n"
    "   } help!\n"
    "}\n"
    "\n"
    "set cmd_0or1item[fn,default1,help]:{\n"
    "   cmd [line]:{\n"
    "       .argv = get_argv 0 1 help line!;\n"
    "       argv != NULL {fn (if (len argv!)==0 {default1} {argv.0}!)!}!;\n"
    "   } help!\n"
    "}\n"
    "\n"
    "set cmd_2item[fn,help]:{\n"
    "   cmd [line]:{\n"
    "       .argv = get_argv 2 2 help line!;\n"
    "       argv != NULL {fn argv.0 argv.1!}!;\n"
    "   } help!\n"
    "}\n"
    "\n"
    "set cmd_2or3item[fn,default3,help]:{\n"
    "   cmd [line]:{\n"
    "       .argv = get_argv 2 3 help line!;\n"
    "       argv != NULL {fn argv.0 argv.1 (if (len argv!)==2 {default3} {argv.2}!)!}!;\n"
    "   } help!\n"
    "}\n"
    "\n"
    "set cmd_3item[fn,help]:{\n"
    "   cmd [line]:{\n"
    "       .argv = get_argv 3 3 help line!;\n"
    "       argv != NULL {fn argv.0 argv.1 argv.2!}!;\n"
    "   } help!\n"
    "}\n"
    "\n"
    "set cmd_3or4item[fn,default4,help]:{\n"
    "   cmd [line]:{\n"
    "       .argv = get_argv 3 4 help line!;\n"
    "       argv != NULL {\n"
    "           fn argv.0 argv.1 argv.2 (if (len argv!)==3 {default4} {argv.3}!)!\n"
    "       }!;\n"
    "   } help!\n"
    "}\n"
    "\n"
    "set cmd_3or4item3opt[fn,default3,help]:{\n"
    "   cmd [line]:{\n"
    "       .argv = get_argv 3 4 help line!;\n"
    "       argv != NULL {\n"
    "           if (len argv!) == 3 {\n"
    "               fn argv.0 argv.1 default3 argv.2!\n"
    "           }{\n"
    "               fn argv.0 argv.1 argv.2 argv.3!\n"
    "           }!               \n"
    "       }!;\n"
    "   } help!\n"
    "}\n"
    "\n"
    "set cmd_ls[help]:{\n"
    "   cmd [line]:{\n"
    "       .argv = get_argv 0 2 help line!;\n"
    "       argv != NULL {\n"
    "           .lsall = FALSE;\n"
    "           (len argv!)!=0 {\"-a\" == argv.0}! {\n"
    "               argv = dequeue argv!;\n"
    "               lsall=TRUE;\n"
    "           }!;\n"
    "           pname_ls lsall (if (len argv!)==0 {\"\"} {argv.0}!)!\n"
    "       }!;\n"
    "   } help!\n"
    "}\n"
    "\n"
    "\n"
    "#\n"
    "# named object values\n"
    "#\n"
    "\n"
    "set objval []\n"
    "set exec_code NULL\n"
    "\n"
    "set set_exec_code[fn]:{exec_code = fn;}\n"
    "\n"
    "set typeobj_create[obj,type,descr,code]:{\n"
    "    .tobj = type_create type descr!;\n"
    "    NULL != tobj {\n"
    "        objval.(obj) = tobj;\n"
    "        exec_code != NULL {exec_code code !}!;\n"
    "    }!\n"
    "}\n"
    "\n"
    "set typeobj_open[obj,type]:{\n"
    "    objval.(obj) = type_open type!;\n"
    "}\n"
    "\n"
    "set goodtypevar[obj]:{\n"
    "    if (inenv objval obj!) {\n"
    "        if (objval.(obj) == NULL) {\n"
    "            errorf \"using closed type object \'%v\'\" <obj>!; FALSE\n"
    "        }{ TRUE }!\n"
    "    }{ errorf \"no such type object \'%v\'\" <obj>!; FALSE}!\n"
    "}\n"
    "\n"
    "set typeobj_field[obj,field,default]:{\n"
    "    (goodtypevar obj!) {\n"
    "        type_field objval.(obj) field default!\n"
    "    }!\n"
    "}\n"
    "\n"
    "set typeobj_store[obj,field,link,type]:{\n"
    "    (goodtypevar obj!) {\n"
    "        type_store objval.(obj) field link type!\n"
    "    }!\n"
    "}\n"
    "\n"
    "set typeobj_close[obj]:{\n"
    "    (goodtypevar obj!) {\n"
    "        objval.(obj) = NULL;\n"
    "    }!\n"
    "}\n"
    "\n"
    "# create new objval by opening file referred to by \'dirname\' giving temporary\n"
    "# name \'obj\'\n"
    "set dirobj_create[obj,dirname,type]:{\n"
    "    .fname = file_from_root TRUE type dirname!;\n"
    "    NULL != fname {\n"
    "        .dobj = pfile_open_create fname FALSE!;\n"
    "        NULL == dobj { errorf \"File corresponding to %v \'%s\' can\'t be opened (%s)\" <type, dirname, fname>! }!;\n"
    "        objval.(obj) = dobj;\n"
    "        dobj != NULL\n"
    "    }!\n"
    "}\n"
    "\n"
    "set dirobj_isempty[dirname,code]:{\n"
    "    pname_hasvalues dirname! {\n"
    "        exec_code != NULL {exec_code code !}!;\n"
    "    }!\n"
    "}\n"
    "\n"
    "# use string held in obj to refer to file indicated by penv name\n"
    "set dirobj_open[obj,name]:{\n"
    "    .pdir = file_from_root FALSE NULL name!;\n"
    "    pdir != NULL { objval.(obj) = pdir }!;\n"
    "}\n"
    "\n"
    "set dirobj_isunset[name,code]:{\n"
    "    NULL == (pname_getval name FALSE!) {\n"
    "        exec_code != NULL {exec_code code !}!;\n"
    "    }!\n"
    "}\n"
    "\n"
    "set gooddirvar[obj]:{\n"
    "    if (inenv objval obj!) {\n"
    "        if (objval.(obj) == NULL) {\n"
    "            errorf \"using closed env directory object \'%v\'\" <obj>!; FALSE\n"
    "        }{ TRUE }!\n"
    "    }{ errorf \"no such env directory object \'%v\'\" <obj>!; FALSE}!\n"
    "}\n"
    "\n"
    "set dirobj_setfield[obj,field,value]:{\n"
    "    (gooddirvar obj!) {\n"
    "        .dobj = objval.(obj);\n"
    "        if (inenv dobj.dir field!) {\n"
    "           (dobj.dir.(field) != value) { dobj.changed = TRUE }!;\n"
    "        }{ dobj.changed = TRUE }!;\n"
    "        dobj.dir.(field) = value;\n"
    "    }!\n"
    "}\n"
    "\n"
    "set dirobj_close[obj]:{\n"
    "    (gooddirvar obj!) {\n"
    "        objval.(obj) = NULL;\n"
    "    }!;\n"
    "}\n"
    "\n"
    "#\n"
    "# command command set\n"
    "#\n"
    "\n"
    "set envifexists [var]:{\n"
    "    if (inenv sys.env var!) { sys.env.(var) } { \"\" }!\n"
    "}\n"
    "set HOME envifexists \"HOME\"!\n"
    "set DIR_PENV envifexists \"DIR_PENV\"!\n"
    "set penv_context_domain envifexists \"penv_context_domain\"!\n"
    "set penv_context_host envifexists \"penv_context_host\"!\n"
    "set penv_context_session envifexists \"penv_context_session\"!\n"
    "set penv_context_arch envifexists \"penv_context_arch\"!\n"
    "\n"
    "set def set # otherwise we\'ll lose it\n"
    "\n"
    "set penv [\n"
    "    file = cmd_0or1item pname_file \"\"\n"
    "        \"<penv dir> - show file containing name-value pairs\"!,\n"
    "    dir = cmd_0or1item pname_dir \"\"\n"
    "        \"<penv var> - show directory containing the value of the given name\"!,\n"
    "    ls = cmd_ls\n"
    "        \"[-a] <penv dir> - show name-value pairs in named dir\"!,\n"
    "    get = cmd_1item pname_get\n"
    "        \"<penv var> - show value associated with name\"!,\n"
    "    set = cmd_2item pname_set\n"
    "        \"<penv var> <value> - assign value to name\"!,\n"
    "    unset = cmd_1item pname_unset\n"
    "        \"<penv var> - remove value from name\"!,\n"
    "\n"
    "    type_create = cmd_3or4item3opt typeobj_create \"\"\n"
    "        \"<objname> <typename> [<description>] <code> - create & name a new type run code if new\"!,\n"
    "    type_open = cmd_2item typeobj_open\n"
    "        \"<objname> <typename> - open the environment for a type and name it\"!,\n"
    "    type_field = cmd_2or3item typeobj_field \"\"\n"
    "        \"<objname> <field> [<default>] - give named type a new field with default value\"!,\n"
    "    type_store = cmd_3or4item typeobj_store \"\"\n"
    "        \"<objname> <field> <link> [<type>] - set relative location of field penv file + default type\"!,\n"
    "    type_close = cmd_1item typeobj_close\n"
    "        \"<objname> - stop using name for a type environment object\"!,\n"
    "\n"
    "    penv_create = cmd_2or3item dirobj_create \"\"\n"
    "        \"<objname> <dirname> [<type>] - create & name a new environment directory\"!,\n"
    "    penv_isempty = cmd_2item dirobj_isempty\n"
    "        \"<objname> <code> - execute code if the environment directory does not exist\"!,\n"
    "    penv_open = cmd_2item dirobj_open\n"
    "        \"<objname> <dirname> - open the environment directory and name it\"!,\n"
    "    penv_isunset = cmd_2item dirobj_isunset\n"
    "        \"<objname> <code> - execute code if the environment variable is unset\"!,\n"
    "    obj_set = cmd_3item dirobj_setfield\n"
    "        \"<objname> <field> <value> - give named directory a new field and value\"!,\n"
    "    penv_close = cmd_1item dirobj_close\n"
    "        \"<objname> - stop using name for a environment directory object\"!,\n"
    "        \n"
    "]\n"
    "\n"
    "enter penv\n"
    "\n"
    "def penv_file file\n"
    "def penv_ls ls\n"
    "def penv_get get\n"
    "def penv_set set\n"
    "def penv_unset unset\n"
    "\n"
    "# we need to pass back a function that will execute code in this new environment\n"
    "set_exec_code [code]:{parse.rdenv code NULL!;}\n"
    "\n"
/* text end - penv_text */
   ;




/*****************************************************************************
 *                                                                           *
 *          Main Program						     *
 *          ============					             *
 *                                                                           *
 *****************************************************************************/






extern int
main(int argc, const char **argv)
{   const char *err;
    const char *init_cmds = NULL;
    const char *cmd_file = NULL;
    const char *app_argv[APP_ARGC_MAX];
    int app_argc;
    bool echo_lines = FALSE;
    bool quiet = FALSE;
    bool do_profile = TRUE;
    
    ftl_init();
    
    if (NULL == (err = parse_args(argc, argv,
				  &app_argc, &app_argv[0], APP_ARGC_MAX,
				  &init_cmds, &cmd_file, &do_profile,
                                  &echo_lines, &quiet)))
    {   parser_state_t *state = parser_state_new(dir_id_new());

	if (NULL != state)
	{   parser_echo_set(state, echo_lines);
	    cmds_generic(state, app_argc, &app_argv[0]);
	    if (cmd_penv(state))
	    {	bool do_args = app_argc > 1;

                if (do_profile)
                {   charsource_t *prolog =
                        charsource_cstring_new("prolog", &penv_text[0],
                                               sizeof(penv_text));
                    if (NULL == prolog)
                        printf("%s: couldn't open text prolog\n", CODEID);
                    else if (NULL == parser_expand_exec(state, prolog,
                                                        NULL, NULL,
                                                        /*no_locals*/TRUE))
                    {   fprintf(stderr, "%s: attempted execution of "
                                "text prolog failed\n", codeid());
                    }
                }
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
   		    cli(state, init_cmds, CODEID);

    		    if (!quiet)
		        printf("%s: finished\n", CODEID);
	        }
	        cmd_penv_end(state);
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




