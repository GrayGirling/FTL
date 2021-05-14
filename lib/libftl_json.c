/*
 * Copyright (c) 2018, Gray Girling.
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
** brief   Framework for Testing Command-line Library - JSON support
** date    Jul 2018
**/




/*****************************************************************************
 *                                                                           *
 *          Headers                                                          *
 *                                                                           *
 *****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#include "ftl_api.h"          /* FTL access header */
#include "ftl_internal.h"     /* FTL extensions header */
#include "ftl_json.h"         /* our header */


/*****************************************************************************
 *                                                                           *
 *          Configuration                                                    *
 *                                                                           *
 *****************************************************************************/



#define JSON_NAME_MAX   256
#define JSON_STRING_MAX 8196



/*****************************************************************************
 *                                                                           *
 *          Debug                                                            *
 *                                                                           *
 *****************************************************************************/


#define OMIT(_x)
#define DO(_x) _x

#define DEFAULT_DEBUG OMIT

#define DEBUG_PARSE   DEFAULT_DEBUG




/*****************************************************************************
 *                                                                           *
 *          Parse Routines                                                   *
 *                                                                           *
 *****************************************************************************/



/* forward reference */
static bool
parsew_json_value(const char **ref_line, const char *lineend,
                  parser_state_t *state, const value_t **out_val);




static bool
parsew_json_object_body(const char **ref_line, const char *lineend,
                        parser_state_t *state, const value_t **out_val)
{
    /* syntax: [<string> : <value> [, <string> : <value>]*] */
    bool ok = true;
    const char *line = *ref_line;
    dir_t *dirval = dir_id_lnew(state);
    bool expect_name = FALSE;

    do
    {
        char strbuf[JSON_NAME_MAX];
        size_t namelen = 0;

        parsew_space(&line, lineend);
        if (parsew_string(&line, lineend,
                          &strbuf[0], sizeof(strbuf), &namelen))
        {   parsew_space(&line, lineend);
            if (!parsew_key(&line, lineend, ":"))
            {   parser_error(state, "expected ':' after JSON name string\n");
                ok = FALSE;
            } else {
                const value_t *val = NULL;
                parsew_space(&line, lineend);
                ok = parsew_json_value(&line, lineend, state, &val);
                parsew_space(&line, lineend);
                *ref_line = line;
                if (ok)
                {
                    value_t *nameval =
                        value_string_lnew(state, &strbuf[0], namelen);
                    if (!dir_lset(dirval, state, nameval, val))
                    {   parser_error(state, "failed to set value of JSON object "
                                     "field \"%s\"\n",
                                     &strbuf[0]);
                        /* syntax OK though so don't update 'ok' */
                    }
                    *ref_line = line;
                    expect_name = parsew_key(&line, lineend, ",");
                }
            }
        }
        else if (expect_name)
        {   parser_error(state, "expected string in JSON object after ','\n");
            ok = FALSE;
        }
    } while (ok && expect_name);

    if (ok)
        *ref_line = line;
    *out_val = dir_value(dirval);

    return ok;
}





static bool
parsew_json_array_body(const char **ref_line, const char *lineend,
                       parser_state_t *state, const value_t **out_val)
{
    /* syntax: [<value> [, <value>]*] */
    bool ok = true;
    const char *line = *ref_line;
    dir_t *dirval = dir_vec_lnew(state);
    bool expect_value = false;
    int index = 0;

    do
    {
        const value_t *val = NULL;
        parsew_space(&line, lineend);
        ok = parsew_json_value(&line, lineend, state, &val);
        parsew_space(&line, lineend);
        if (ok)
        {
            if (!dir_int_lset(dirval, state, index, val))
            {   parser_error(state, "failed to set index [%d] of JSON array\n",
                             index);
                /* syntax OK though so don't update 'ok' */
            }
            index++;
            *ref_line = line;
            expect_value = parsew_key(&line, lineend, ",");
        }
        else if (expect_value)
        {   parser_error(state, "expected value in JSON array after ','\n");
            ok = FALSE;
        }
    } while (ok && expect_value);

    if (ok)
        *ref_line = line;
    *out_val = dir_value(dirval);

    return ok;
}





static bool
parsew_json_value(const char **ref_line, const char *lineend,
                  parser_state_t *state, const value_t **out_val)
{
    /* syntax: <string> | <number> | <object> | <array> | true | false | null */
    bool ok = true;
    const char *line = *ref_line;
    number_t numb = 0;
    char stringbuf[JSON_STRING_MAX];
    size_t stringbufused = 0;

    parsew_space(&line, lineend);
    if (parsew_key(&line, lineend, "{"))
    {   ok = parsew_space(&line, lineend) &&
             parsew_json_object_body(&line, lineend, state, out_val);
        parsew_space(&line, lineend);
        if (ok && !parsew_key(&line, lineend, "}"))
        {   parser_error(state, "expected '}' at end of JSON object\n");
            ok = FALSE;
        }
        *ref_line = line;
    } else
    if (parsew_key(&line, lineend, "["))
    {   ok = parsew_space(&line, lineend) &&
             parsew_json_array_body(&line, lineend, state, out_val);
        parsew_space(&line, lineend);
        if (ok && !parsew_key(&line, lineend, "]"))
        {   parser_error(state, "expected ']' at end of JSON array\n");
            ok = FALSE;
        }
        *ref_line = line;
    } else
    if (parsew_int_val(&line, lineend, &numb))
    {   *out_val = value_int_lnew(state, numb);
    } else
    if (parsew_string(&line, lineend,
                      &stringbuf[0], sizeof(stringbuf), &stringbufused))
    {   *out_val = value_string_lnew(state, &stringbuf[0], stringbufused);
    } else
    if (parsew_key(&line, lineend, "null"))
    {   *out_val = &value_null;
    } else
    if (parsew_key(&line, lineend, "true"))
    {   *out_val = value_true;
    } else
    if (parsew_key(&line, lineend, "false"))
    {   *out_val = value_false;
    } else
        ok = FALSE;

    if (ok)
        *ref_line = line;

    return ok;
}



static const value_t *
fnparse_json_value(const char **ref_line, const char * lineend,
                   parser_state_t *state, void *arg)
{
    /* doesn't make use of arg */
    const value_t *val = NULL;
    bool ok = parsew_json_value(ref_line, lineend, state, &val);
    return ok? value_true: value_false;
}

        


static const value_t *
fnparse_json_object(const char **ref_line, const char *lineend,
                    parser_state_t *state, void *arg)
{
    /* doesn't make use of arg */
    const value_t *val = NULL;
    bool ok = parsew_json_object_body(ref_line, lineend, state, &val);
    return ok? value_true: value_false;
}

        


static const value_t *
fn_json_scan_baseval(const value_t *this_fn, parser_state_t *state)
{   return genfn_scanw_cb(this_fn, state, 1, &fnparse_json_value, NULL);
}



static const value_t *
fn_json_scan_dir(const value_t *this_fn, parser_state_t *state)
{   return genfn_scanw_cb(this_fn, state, 1, &fnparse_json_object, NULL);
}





typedef bool json_parse_fn(const char **ref_line, const char *lineend,
                           parser_state_t *state, const value_t **out_val);




static const value_t *
genfn_json_parse(const value_t *this_fn, 
                 parser_state_t *state, json_parse_fn *json_scan)
{   
    const value_t *val = &value_null;
    const value_t *jsontextval = parser_builtin_arg(state, 1);
    bool is_string = value_type_equal(jsontextval, type_string);
    bool is_code = value_type_equal(jsontextval, type_code);

    if (is_string || is_code)
    {   const char *jsontext = NULL;
        size_t jsontextlen = 0;

        if ((is_string && value_string_get(jsontextval,
                                           &jsontext, &jsontextlen)) ||
            (is_code && value_code_buf(jsontextval,
                                       &jsontext, &jsontextlen))
            )
        {
            const char *line = jsontext;
            const char *lineend = &line[jsontextlen];
            bool ok = (*json_scan)(&line, lineend, state, &val) &&
                      parsew_space(&line, lineend) &&
                      parsew_empty(&line, lineend);
            if (!ok)
            {
                size_t len = strlen(line);
                if (len > 30)
                    len = 30;
                parser_error(state, "syntax error in JSON value text: '%.*s'\n",
                             len, line);
                val = &value_null;
            }
        }
    } else
        parser_report_help(state, this_fn);

    return val;
}




static const value_t *
fn_jsonval(const value_t *this_fn, parser_state_t *state)
{   return genfn_json_parse(this_fn, state, &parsew_json_value);
}





static const value_t *
fn_json(const value_t *this_fn, parser_state_t *state)
{   return genfn_json_parse(this_fn, state, &parsew_json_object_body);
}






/*****************************************************************************
 *                                                                           *
 *          FTL JSON output commands                                         *
 *                                                                           *
 *****************************************************************************/






typedef struct
{   outchar_t *out;
    const value_t *root;
    bool pretty;
    const char *delim;
    const char *fielddelim;
    const char *indent;
    int depth;
} json_format_t;



typedef struct
{   json_format_t *fmt;
    int len;
    bool first;
    bool is_obj;
    bool bracketed;
    parser_state_t *state;
} dir_json_bind_print_arg_t;




static int json_fmt_printnl(parser_state_t *state, json_format_t *fmt)
{   int i;
    int len = outchar_printf(fmt->out, "\n");
    for (i=0; i<fmt->depth; i++)
        len += outchar_printf(fmt->out, fmt->indent);
    return len;
}



/* forward reference */
static int
json_fmt_print(parser_state_t *state, outchar_t *out, const value_t *root,
               const value_t *value, json_format_t *fmt);



static void *
value_json_dir_bind_print(dir_t *dir, const value_t *name,
                          const value_t *value, void *arg)
{   dir_json_bind_print_arg_t *pr = (dir_json_bind_print_arg_t *)arg;
    outchar_t *out = pr->fmt->out;
    const value_t *root = pr->fmt->root;
    
    if (pr->first)
        pr->is_obj = value_type_equal(name, type_string);

    if (!pr->bracketed)
    {   pr->len += outchar_printf(out, pr->is_obj? "{": "[");
        pr->bracketed = TRUE;
        if (pr->is_obj && pr->fmt->pretty)
        {   pr->fmt->depth++;
            pr->len += json_fmt_printnl(pr->state, pr->fmt);
        }
    }
    
    if (pr->first)
        pr->first = FALSE;
    else
    {   pr->len += outchar_printf(out, pr->fmt->delim);
        if (pr->is_obj && pr->fmt->pretty)
            pr->len += json_fmt_printnl(pr->state, pr->fmt);
    }

    if (pr->is_obj)
    {
        if (value_type_equal(name, type_string))
            pr->len += json_fmt_print(pr->state, out, root, name, pr->fmt);
        else
            /* shouldn't really be here! */
            pr->len += json_fmt_print(pr->state, out, root, name, pr->fmt);

        pr->len += outchar_printf(out, pr->fmt->fielddelim);
    }
    OMIT(pr->len += outchar_printf(out, "(%s)#%p ",
                                   value_type_name(value), value););
    pr->len += json_fmt_print(pr->state, out, root, value, pr->fmt);

    return NULL;
}








static int
json_fmt_dir_print(parser_state_t *state, outchar_t *out, const value_t *root,
                   const value_t *value, json_format_t *fmt)
{   dir_json_bind_print_arg_t pr;

    pr.state = state;
    pr.len = 0;

    if (value_istype(value, type_dir))
    {   dir_t *dir = (dir_t *)value;

        pr.fmt = fmt;
        pr.first  = TRUE;
        pr.bracketed = FALSE;

        (void)dir_state_forall(dir, state, &value_json_dir_bind_print, &pr);

        if (pr.first)
        {   pr.is_obj = FALSE; /* this is arbitrary - we can't tell */
            pr.len += outchar_printf(out, pr.is_obj? "{}": "[]");
            pr.bracketed = TRUE;
            pr.first = FALSE;
        } else
        if (pr.bracketed)
        {
            if (pr.is_obj && pr.fmt->pretty)
            {   pr.fmt->depth--;
                pr.len += json_fmt_printnl(pr.state, pr.fmt);
            }
            pr.len += outchar_printf(out, pr.is_obj? "}": "]");
        }
    }

    return pr.len;
}




static int
json_fmt_print(parser_state_t *state, outchar_t *out, const value_t *root,
               const value_t *val, json_format_t *fmt)
{   int len;
 
    if (val == &value_null)
        len = outchar_printf(out, "null");
    else if (val == value_true)
        len = outchar_printf(out, "true");
    else if (val == value_false)
        len = outchar_printf(out, "false");
    else if (value_type_equal(val, type_int) ||
             value_type_equal(val, type_string))
        len = value_state_print(state, out, root, val);
    else if (value_type_equal(val, type_dir))
        len = json_fmt_dir_print(state, out, root, val, fmt);
    else
    {
        /* this type is not supported in JSON */
        len = 0;
    }
        
    return len;
}





extern int
json_state_print(parser_state_t *state, outchar_t *out, const value_t *root,
                 const value_t *val, bool pretty)
{   json_format_t fmt;

    fmt.out = out;
    fmt.root = root;
    fmt.pretty = pretty;
    fmt.delim  = pretty? ", ": ",";
    fmt.fielddelim = pretty? " : ": ":";
    fmt.indent = "    ";
    fmt.depth  = 0;

    return json_fmt_print(state, out, root, val, &fmt);
}




static const value_t *
gengenfn_fmt_json(char *buf, size_t buflen, fprint_flags_t flags, int precision,
                  const value_t *argval, parser_state_t *state, bool pretty)
{   const value_t *val = &value_null;
    charsink_string_t stream;
    charsink_t *sink = charsink_string_init(&stream);
    const char *strbuf;
    size_t strsize;

    (void)buf;
    (void)buflen;

    json_state_print(state, sink, dir_value(parser_root(state)),
                     argval, pretty);
    charsink_string_buf(sink, &strbuf, &strsize);
    if (precision > 0 && precision < (int)strsize)
        strsize = precision;
    val = value_string_lnew(state, strbuf, strsize);
    charsink_string_close(sink);

    return val;
}



static const value_t *
genfn_fmt_j(char *buf, size_t buflen, fprint_flags_t flags, int precision,
            const value_t *argval, parser_state_t *state)
{   return gengenfn_fmt_json(buf, buflen, flags, precision, argval, state,
                             /*pretty*/FALSE);
}



static const value_t *
fn_fmt_j(const value_t *this_fn, parser_state_t *state)
{   return fn_fmt_generic(this_fn, state, &genfn_fmt_j);
}



static const value_t *
genfn_fmt_J(char *buf, size_t buflen, fprint_flags_t flags, int precision,
            const value_t *argval, parser_state_t *state)
{   return gengenfn_fmt_json(buf, buflen, flags, precision, argval, state,
                             /*pretty*/TRUE);
}



static const value_t *
fn_fmt_J(const value_t *this_fn, parser_state_t *state)
{   return fn_fmt_generic(this_fn, state, &genfn_fmt_J);
}




static const value_t *
genfn_json_make(const value_t *this_fn, parser_state_t *state,
                FILE *out, bool pretty)
{   const value_t *inval = parser_builtin_arg(state, 1);
    const value_t *val = gengenfn_fmt_json(/*buf*/NULL, /*buflen*/0, /*flags*/0,
                                           /*precision*/0, inval, state,
                                           pretty);
    if (out != NULL)
    {   /* print the string to stream directly */
        const char *str = NULL;
        size_t strlen = 0;
        if (value_string_get(val, &str, &strlen))
            fprintf(out, "%s\n", str);
        else
            parser_error(state, "failed to find a JSON equivalent for value\n");

        val = &value_null;
    }
    return val;
}



static const value_t *
fn_json_out(const value_t *this_fn, parser_state_t *state)
{   return genfn_json_make(this_fn, state, stdout, /*pretty*/FALSE);
}



static const value_t *
fn_json_outpty(const value_t *this_fn, parser_state_t *state)
{   return genfn_json_make(this_fn, state, stdout, /*pretty*/TRUE);
}




static const value_t *
fn_json_str(const value_t *this_fn, parser_state_t *state)
{   return genfn_json_make(this_fn, state, NULL, /*pretty*/FALSE);
}



static const value_t *
fn_json_strpty(const value_t *this_fn, parser_state_t *state)
{   return genfn_json_make(this_fn, state, NULL, /*pretty*/TRUE);
}








/*****************************************************************************
 *                                                                           *
 *          FTL JSON commands                                                 *
 *                                                                           *
 *****************************************************************************/




extern bool
cmds_json(parser_state_t *state, dir_t *cmds)
{
    printf_addformat(type_int, "j", "<f> <p> <val> - %j (JSON) value format",
                     &fn_fmt_j);
    printf_addformat(type_int, "J",
                     "<f> <p> <val> - %J (pretty JSON) value format",
                     &fn_fmt_J);
    smod_addfn(state, cmds, "print",
               "<val> - print value as JSON (as %j format)",
               &fn_json_out, 1);
    smod_addfn(state, cmds, "printp",
               "<val> - pretty print value as JSON (as %J format)",
               &fn_json_outpty, 1);
    smod_addfn(state, cmds, "str",
               "<val> - return value as JSON string (as %j format)",
               &fn_json_str, 1);
    smod_addfn(state, cmds, "strp",
               "<val> - return value as pretty JSON string (as %J format)",
               &fn_json_strpty, 1);
    smod_addfn(state, cmds, "obj",
               "<code> - return FTL value from JSON object",
               &fn_json, 1);
    smod_addfn(state, cmds, "val",
               "<string> - return FTL value from JSON string",
               &fn_jsonval, 1);
    smod_addfn(state, cmds, "scanjson",
               "<@value> <parseobj> - parse JSON term value from parse object",
               &fn_json_scan_baseval, 2);
    smod_addfn(state, cmds, "scanjsonobj",
               "<@value> <parseobj> - parse JSON object body from parse object",
               &fn_json_scan_dir, 2);
    return TRUE;
}




extern void
cmds_json_end(parser_state_t *state)
{
    return;
}






 /*
 * Local variables:
 *  c-basic-offset: 4
 *  c-indent-level: 4
 *  tab-width: 8
 * End:
 */

 
