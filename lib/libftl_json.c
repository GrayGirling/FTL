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

#include "ftl.h"              // FTL access header
#include "ftl_internal.h"     // FTL extensions header
#include "ftl_json.h"         // our header


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
parse_json_value(const char **ref_line, parser_state_t *state,
                 const value_t **out_val);




static bool
parse_json_object_body(const char **ref_line, parser_state_t *state,
                       const value_t **out_val)
{
    /* syntax: [<string> : <value> [, <string> : <value>]*] */
    bool ok = true;
    const char *line = *ref_line;
    dir_t *dirval = dir_id_new();
    bool expect_name = FALSE;

    do
    {
        char strbuf[JSON_NAME_MAX];
        size_t namelen = 0;

        parse_space(&line);
        if (parse_string(&line, &strbuf[0], sizeof(strbuf), &namelen))
        {   parse_space(&line);
            if (!parse_key(&line, ":"))
            {   parser_error(state, "expected ':' after JSON name string\n");
                ok = FALSE;
            } else {
                const value_t *val = NULL;
                parse_space(&line);
                ok = parse_json_value(&line, state, &val);
                parse_space(&line);
                *ref_line = line;
                if (ok)
                {
                    value_t *nameval = value_string_new(&strbuf[0], namelen);
                    if (!dir_set(dirval, nameval, val))
                    {   parser_error(state, "failed to set value of JSON object "
                                     "field \"%s\"\n",
                                     &strbuf[0]);
                        /* syntax OK though so don't update 'ok' */
                    }
                    *ref_line = line;
                    expect_name = parse_key(&line, ",");
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
parse_json_array_body(const char **ref_line, parser_state_t *state,
                      const value_t **out_val)
{
    /* syntax: [<value> [, <value>]*] */
    bool ok = true;
    const char *line = *ref_line;
    dir_t *dirval = dir_vec_new();
    bool expect_value = false;
    int index = 0;

    do
    {
        const value_t *val = NULL;
        parse_space(&line);
        ok = parse_json_value(&line, state, &val);
        parse_space(&line);
        if (ok)
        {
            if (!dir_int_set(dirval, index, val))
            {   parser_error(state, "failed to set index [%d] of JSON array\n",
                             index);
                /* syntax OK though so don't update 'ok' */
            }
            index++;
            *ref_line = line;
            expect_value = parse_key(&line, ",");
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
parse_json_value(const char **ref_line, parser_state_t *state,
                 const value_t **out_val)
{
    /* syntax: <string> | <number> | <object> | <array> | true | false | null */
    bool ok = true;
    const char *line = *ref_line;
    number_t numb = 0;
    char stringbuf[JSON_STRING_MAX];
    size_t stringbufused = 0;

    parse_space(&line);
    if (parse_key(&line, "{"))
    {   ok = parse_space(&line) && parse_json_object_body(&line, state, out_val);
        parse_space(&line);
        if (ok && !parse_key(&line, "}"))
        {   parser_error(state, "expected '}' at end of JSON object\n");
            ok = FALSE;
        }
        *ref_line = line;
    } else
    if (parse_key(&line, "["))
    {   ok = parse_space(&line) && parse_json_array_body(&line, state, out_val);
        parse_space(&line);
        if (ok && !parse_key(&line, "]"))
        {   parser_error(state, "expected ']' at end of JSON array\n");
            ok = FALSE;
        }
        *ref_line = line;
    } else
    if (parse_int_val(&line, &numb))
    {   *out_val = value_int_new(numb);
    } else
    if (parse_string(&line, &stringbuf[0], sizeof(stringbuf), &stringbufused))
    {   *out_val = value_string_new(&stringbuf[0], stringbufused);
    } else
    if (parse_key(&line, "null"))
    {   *out_val = &value_null;
    } else
    if (parse_key(&line, "true"))
    {   *out_val = value_true;
    } else
    if (parse_key(&line, "false"))
    {   *out_val = value_false;
    } else
        ok = FALSE;

    if (ok)
        *ref_line = line;

    return ok;
}



static const value_t *
fnparse_json_value(const char **ref_line, parser_state_t *state,
                   const value_t *arg)
{
    /* doesn't make use of arg */
    const value_t *val = NULL;
    bool ok = parse_json_value(ref_line, state, &val);
    return ok? value_true: value_false;
}

        


static const value_t *
fnparse_json_object(const char **ref_line, parser_state_t *state,
                    const value_t *arg)
{
    /* doesn't make use of arg */
    const value_t *val = NULL;
    bool ok = parse_json_object_body(ref_line, state, &val);
    return ok? value_true: value_false;
}

        


static const value_t *
fn_json_scan(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_json_value, NULL);
}



static const value_t *
fn_json_scanobj(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_json_object, NULL);
}





typedef bool json_parse_fn(const char **ref_line, parser_state_t *state,
                           const value_t **out_val);




static const value_t *
genfn_json_parse(const value_t *this_fn, parser_state_t *state,
                 json_parse_fn *json_scan)
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
            bool ok = (*json_scan)(&line, state, &val) &&
                      parse_space(&line) &&
                      parse_empty(&line);
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
{   return genfn_json_parse(this_fn, state, &parse_json_value);
}





static const value_t *
fn_json(const value_t *this_fn, parser_state_t *state)
{   return genfn_json_parse(this_fn, state, &parse_json_object_body);
}






/*****************************************************************************
 *                                                                           *
 *          FTL JSON commands                                                 *
 *                                                                           *
 *****************************************************************************/




extern bool
cmds_json(parser_state_t *state, dir_t *cmds)
{
    mod_addfn(cmds, "obj",
              "<code> - return FTL value from JSON object",
              &fn_json, 1);
    mod_addfn(cmds, "val",
              "<string> - return FTL value from JSON string",
              &fn_jsonval, 1);
    mod_addfn(cmds, "scanjson",
              "<@value> <parseobj> - parse value from JSON parse object",
              &fn_json_scan, 2);
    mod_addfn(cmds, "scanjsonobj",
              "<@value> <parseobj> - parse object body from JSON parse object",
              &fn_json_scanobj, 2);
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

 
