/*
 * Copyright (c) 2015, NXP Semiconductors Inc.
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
** brief   Framework for Testing Command-line Library - XML support
** date    Jul 2015
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
#include "ftl_xml.h"          // our header


/*****************************************************************************
 *                                                                           *
 *          Configuration                                                    *
 *                                                                           *
 *****************************************************************************/






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



typedef struct {
    const char *name;
    unsigned unicode;
} xml_escape_entry_t;


static const xml_escape_entry_t xml_escape[] =
{
    { "Aacute", 193 },
    { "aacute", 225 },
    { "Acirc", 194 },
    { "acirc", 226 },
    { "acute", 180 },
    { "AElig", 198 },
    { "aelig", 230 },
    { "Agrave", 192 },
    { "agrave", 224 },
    { "alefsym", 8501 },
    { "Alpha", 913 },
    { "alpha", 945 },
    { "amp", 38 },
    { "and", 8743 },
    { "ang", 8736 },
    { "apos", 39 },
    { "Aring", 197 },
    { "aring", 229 },
    { "asymp", 8776 },
    { "Atilde", 195 },
    { "atilde", 227 },
    { "Auml", 196 },
    { "auml", 228 },
    { "bdquo", 8222 },
    { "Beta", 914 },
    { "beta", 946 },
    { "brvbar", 166 },
    { "bull", 8226 },
    { "cap", 8745 },
    { "Ccedil", 199 },
    { "ccedil", 231 },
    { "cedil", 184 },
    { "cent", 162 },
    { "Chi", 935 },
    { "chi", 967 },
    { "circ", 710 },
    { "clubs", 9827 },
    { "cong", 8773 },
    { "copy", 169 },
    { "crarr", 8629 },
    { "cup", 8746 },
    { "curren", 164 },
    { "dagger", 8224 },
    { "Dagger", 8225 },
    { "darr", 8595 },
    { "dArr", 8659 },
    { "deg", 176 },
    { "Delta", 916 },
    { "delta", 948 },
    { "diams", 9830 },
    { "divide", 247 },
    { "Eacute", 201 },
    { "eacute", 233 },
    { "Ecirc", 202 },
    { "ecirc", 234 },
    { "Egrave", 200 },
    { "egrave", 232 },
    { "empty", 8709 },
    { "emsp", 8195 },
    { "ensp", 8194 },
    { "Epsilon", 917 },
    { "epsilon", 949 },
    { "equiv", 8801 },
    { "Eta", 919 },
    { "eta", 951 },
    { "ETH", 208 },
    { "eth", 240 },
    { "Euml", 203 },
    { "euml", 235 },
    { "euro", 8364 },
    { "exist", 8707 },
    { "fnof", 402 },
    { "forall", 8704 },
    { "frac12", 189 },
    { "frac14", 188 },
    { "frac34", 190 },
    { "frasl", 8260 },
    { "Gamma", 915 },
    { "gamma", 947 },
    { "ge", 8805 },
    { "gt", 62 },
    { "harr", 8596 },
    { "hArr", 8660 },
    { "hearts", 9829 },
    { "hellip", 8230 },
    { "Iacute", 205 },
    { "iacute", 237 },
    { "Icirc", 206 },
    { "icirc", 238 },
    { "iexcl", 161 },
    { "Igrave", 204 },
    { "igrave", 236 },
    { "image", 8465 },
    { "infin", 8734 },
    { "int", 8747 },
    { "Iota", 921 },
    { "iota", 953 },
    { "iquest", 191 },
    { "isin", 8712 },
    { "Iuml", 207 },
    { "iuml", 239 },
    { "Kappa", 922 },
    { "kappa", 954 },
    { "Lambda", 923 },
    { "lambda", 955 },
    { "lang", 9001 },
    { "laquo", 171 },
    { "larr", 8592 },
    { "lArr", 8656 },
    { "lceil", 8968 },
    { "ldquo", 8220 },
    { "le", 8804 },
    { "lfloor", 8970 },
    { "lowast", 8727 },
    { "loz", 9674 },
    { "lrm", 8206 },
    { "lsaquo", 8249 },
    { "lsquo", 8216 },
    { "lt", 60 },
    { "macr", 175 },
    { "mdash", 8212 },
    { "micro", 181 },
    { "middot", 183 },
    { "minus", 8722 },
    { "Mu", 924 },
    { "mu", 956 },
    { "nabla", 8711 },
    { "nbsp", 160 },
    { "ndash", 8211 },
    { "ne", 8800 },
    { "ni", 8715 },
    { "not", 172 },
    { "notin", 8713 },
    { "nsub", 8836 },
    { "Ntilde", 209 },
    { "ntilde", 241 },
    { "Nu", 925 },
    { "nu", 957 },
    { "Oacute", 211 },
    { "oacute", 243 },
    { "Ocirc", 212 },
    { "ocirc", 244 },
    { "OElig", 338 },
    { "oelig", 339 },
    { "Ograve", 210 },
    { "ograve", 242 },
    { "oline", 8254 },
    { "Omega", 937 },
    { "omega", 969 },
    { "Omicron", 927 },
    { "omicron", 959 },
    { "oplus", 8853 },
    { "or", 8744 },
    { "ordf", 170 },
    { "ordm", 186 },
    { "Oslash", 216 },
    { "oslash", 248 },
    { "Otilde", 213 },
    { "otilde", 245 },
    { "otimes", 8855 },
    { "Ouml", 214 },
    { "ouml", 246 },
    { "para", 182 },
    { "part", 8706 },
    { "permil", 8240 },
    { "perp", 8869 },
    { "Phi", 934 },
    { "phi", 966 },
    { "Pi", 928 },
    { "pi", 960 },
    { "piv", 982 },
    { "plusmn", 177 },
    { "pound", 163 },
    { "prime", 8242 },
    { "Prime", 8243 },
    { "prod", 8719 },
    { "prop", 8733 },
    { "Psi", 936 },
    { "psi", 968 },
    { "quot", 34 },
    { "radic", 8730 },
    { "rang", 9002 },
    { "raquo", 187 },
    { "rarr", 8594 },
    { "rArr", 8658 },
    { "rceil", 8969 },
    { "rdquo", 8221 },
    { "real", 8476 },
    { "reg", 174 },
    { "rfloor", 8971 },
    { "Rho", 929 },
    { "rho", 961 },
    { "rlm", 8207 },
    { "rsaquo", 8250 },
    { "rsquo", 8217 },
    { "sbquo", 8218 },
    { "Scaron", 352 },
    { "scaron", 353 },
    { "sdot", 8901 },
    { "sect", 167 },
    { "shy", 173 },
    { "Sigma", 931 },
    { "sigma", 963 },
    { "sigmaf", 962 },
    { "sim", 8764 },
    { "spades", 9824 },
    { "sub", 8834 },
    { "sube", 8838 },
    { "sum", 8721 },
    { "sup", 8835 },
    { "sup1", 185 },
    { "sup2", 178 },
    { "sup3", 179 },
    { "supe", 8839 },
    { "szlig", 223 },
    { "Tau", 932 },
    { "tau", 964 },
    { "there4", 8756 },
    { "Theta", 920 },
    { "theta", 952 },
    { "thetasym", 977 },
    { "thinsp", 8201 },
    { "THORN", 222 },
    { "thorn", 254 },
    { "tilde", 732 },
    { "times", 215 },
    { "trade", 8482 },
    { "Uacute", 218 },
    { "uacute", 250 },
    { "uarr", 8593 },
    { "uArr", 8657 },
    { "Ucirc", 219 },
    { "ucirc", 251 },
    { "Ugrave", 217 },
    { "ugrave", 249 },
    { "uml", 168 },
    { "upsih", 978 },
    { "Upsilon", 933 },
    { "upsilon", 965 },
    { "Uuml", 220 },
    { "uuml", 252 },
    { "weierp", 8472 },
    { "Xi", 926 },
    { "xi", 958 },
    { "Yacute", 221 },
    { "yacute", 253 },
    { "yen", 165 },
    { "yuml", 255 },
    { "Yuml", 376 },
    { "Zeta", 918 },
    { "zeta", 950 },
    { "zwj", 8205 },
    { "zwnj", 8204 },
    { NULL, 0 }
};



static bool
parse_xml_escape(const char **ref_line, unsigned *out_unicode)
{
    const char *line = *ref_line;
    bool ok = (*line++ == '&');
    bool got_token = FALSE;
    if (ok) {
        if (*line == '#') {
            char ch;
            unsigned unicode = 0;

            line++;
            ch = *line++;

            if (ch == 'X' || ch == 'x') {
                bool is_hex = TRUE;
                do {
                    ch = *line++;
                    if (ch <= '9' && ch >= '0')
                        unicode = (unicode << 4) | (ch - '0');
                    else if (ch <= 'f' && ch >= 'a')
                        unicode = (unicode << 4) | (ch - 'a');
                    else if (ch <= 'F' && ch >= 'A')
                        unicode = (unicode << 4) | (ch - 'A');
                    else
                        is_hex = FALSE;
                } while (is_hex);
            } else
            {   while (isdigit((unsigned char)ch))
                {   unicode = unicode * 10 + (ch - '0');
                    ch = *line++;
                }
            }
            ok = got_token = (ch == ';');
            if (ok)
                *out_unicode = unicode;
        }
        else {
            char esc[16];
            const char *name = line;
            const char *name_toolong = line + sizeof(esc);
            while (isalpha((unsigned char)*line) && line < name_toolong)
                line++;
            ok = got_token = (*line++ == ';');
            if (ok) {
                const xml_escape_entry_t *knownesc = &xml_escape[0];
                memcpy(&esc[0], name, line-1-name);
                esc[line-1-name] = '\0';
                /* printf("%s: finding esc '%s'\n", codeid(), &esc[0]); */
                while (knownesc->name != NULL &&
                       0 != strcmp(knownesc->name, &esc[0]))
                    knownesc++;
                if (knownesc->name == NULL) {
                    printf("%s: unknown XML escape '%s'\n", codeid(), &esc[0]);
                    ok = FALSE;
                } else
                    *out_unicode = knownesc->unicode;
            }
        }
    }
    if (got_token)
        /* we parse through the token, if there was one, even if we
           say we've failed to get a valid escape
        */
        *ref_line = line;
    return ok;
}



/* Parse white space to a new value at *out_strval, unless this is NULL
 * if the value is NULL just parse the text, but don't generate its value
 */
static bool
parse_xml_text(const char **ref_line, const value_t **out_strval)
{   charsink_string_t charbuf;
    charsink_t *sink = out_strval != NULL? charsink_string_init(&charbuf): NULL;
    const char *line = *ref_line;
    int ch;
    bool ok = FALSE;

    while ((ch = *line) != '\0' && ch != '<')
    {
        ok = TRUE;
        if (ch == '&')
        {   unsigned unicode = 0;
            if (parse_xml_escape(&line, &unicode))
            {
                int written = 0;
                char mbstring[FTL_MB_LEN_MAX+1];
                size_t len;
                (void)wctomb(NULL, L'\0'); /* reset state */
                len = wctomb(&mbstring[0], unicode);
                if ((int)len >= 0 && len < sizeof(mbstring))
                {   mbstring[len] = '\0';
                    if (sink != NULL)
                        written = charsink_write(sink, &mbstring[0], len);
                } else
                    written = -EINVAL;
                if (written != len)
                    printf("%s: failed to convert unicode \\x%04X to string - "
                           "rc %d\n",
                           codeid(), unicode, written);
                /* ch = unicode; */
            }
        } else
        {   if (sink != NULL)
                charsink_putc(sink, ch);
            line++;
        }
    }

    if (sink != NULL)
    {
        const char *str;
        size_t len;

        charsink_string_buf(sink, &str, &len);
        if (out_strval != NULL)
            *out_strval = value_string_new(str, len);
        charsink_string_close(sink);
    }

    if (ok)
        *ref_line = line;

    return ok;
}



/*! parse an XML Name
 *  Note: we handle only the ASCII 7-bit subset
 */
static bool
parse_xml_name(const char **ref_line, char *buf, size_t len)
{   const char *start = *ref_line;
    const char *line = start;
    char ch = *line;

    if (ch=='_' || ch==':' || isalpha((unsigned char)ch))
    {
        do {
            if (len > 1)
            {   len--;
                *buf++ = ch;
            }
            line++;
        } while ((ch = *line) != '\0' &&
                 (ch=='_' ||  ch==':' || ch=='-' || ch=='.' ||
                  isalnum((unsigned char)ch))
                );
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




/*! parse XML tag attributes
 *  Note: we handle only the ASCII 7-bit subset of the attribute names
 *        also we allow any content in the value strings - XML disallows & and <
 *        also we allow all the FTL escapes in a string, similar to C strings!
 *
 *  Parse attributes to a new id environment value at *out_strval, unless this
 *  is NULL.
 *  (The parse finishes after the white space following the last attribute.)
 *
 *  if the value is NULL just parse the text, but don't generate its value.
 */
static bool
parse_xml_attributes(const char **ref_line, const value_t **out_dirval)
{   const char *line = *ref_line;
    char attribute_name[64];
    char attribute_value[64];
    dir_t *dir = (out_dirval == NULL? NULL: dir_id_new());
    bool ok = TRUE;

    while (ok && parse_space(&line) &&
           parse_xml_name(&line, &attribute_name[0], sizeof(attribute_name))) {
        ok = FALSE;
        parse_space(&line);
        if (*line++ == '=') {
            size_t attr_len = 0;
            parse_space(&line);
            ok = parse_string(&line, &attribute_value[0],
                              sizeof(attribute_value), &attr_len);
            if (ok && dir != NULL) {
                const value_t *attrname =
                    value_string_new_measured(&attribute_name[0]);
                const value_t *attrval =
                    value_string_new(&attribute_value[0], attr_len);

                dir_set(dir, attrname, attrval);
            }
        }
    }

    if (ok && out_dirval != NULL)
        *out_dirval = dir_value(dir);

    if (ok)
        *ref_line = line;

    return ok;
}




/*! parse an XML Processing Instruction body following the initial '<'
 *  to just before the final >
 */
static bool
parse_xml_pi_body(const char **ref_line, char *buf, size_t len)
{   const char *line = *ref_line;
    bool ok = (*line++ == '?');

    if (ok) {
        bool complete = FALSE;
        do {
            char ch;
            ok = FALSE;
            while ((ch = *line) != '\0' && ch != '?')
            {
                if (len > 1)
                {   len--;
                    *buf++ = ch;
                }
                line++;
            }
            if (ch != '\0' && line[1] != '>')
            {
                if (len > 1)
                {   len--;
                    *buf++ = ch;
                }
                line++;
            }
            else
                /* ch is '?' and line[1] is '>' or ch is '\0' */
                complete = TRUE;
        } while (!complete || *line == '\0');

        if (complete && *line != '\0')
        {   ok = TRUE;
            line += 1; /* skip over the ? */
            *buf = '\0';
            *ref_line = line;
        }
    }

    return ok;
}




/*! parse an XML Comment body following the initial '<'
 *  to just before the final >
 */
static bool
parse_xml_cmt_body(const char **ref_line, char *buf, size_t len)
{   const char *line = *ref_line;
    bool ok = (*line++ == '!' && *line++ == '-' && *line++ == '-');

    if (ok) {
        bool complete = FALSE;
        do {
            char ch;
            ok = FALSE;
            while ((ch = *line) != '\0' && ch != '-')
            {
                if (len > 1)
                {   len--;
                    *buf++ = ch;
                }
                line++;
            }
            if (ch != '\0' && line[1] != '-')
            {
                if (len > 1)
                {   len--;
                    *buf++ = ch;
                }
                line++;
            }
            else
                /* ch is '-' and line[1] is '-' or ch is '\0' */
                complete = TRUE;
        } while (!complete || *line == '\0');

        if (complete && *line != '\0')
        {
            *buf = '\0';
            line += 2; /* skip over the '--' */
            ok = TRUE;
               *ref_line = line;
        }
    }

    return ok;
}




/*! Parse an XML Declaration body following the initial '<'
 *  to just before the final >
 *  This is probably too lose a definition for the declarator, which in XML
 *  always seems to be upper case alphabetics.
 */
static bool
parse_xml_decl_body(const char **ref_line, char *dec, size_t declen,
                    char *buf, size_t len)
{   const char *line = *ref_line;
    bool ok = (*line++ == '!' &&
               parse_id(&line, dec, declen) &&
               parse_space(&line));

    if (ok) {
        int indent = 1;
        char ch;
        ok = FALSE;
        while ((ch = *line) != '\0' && indent > 0)
        {
            if (ch == '<')
                indent++;
            else if (ch == '>')
                indent--;

            if (indent > 0) {
                len--;
                *buf++ = ch;
                line++;
            }
        }

        if (indent == 0)
        {
            *buf = '\0';
            ok = TRUE;
            *ref_line = line;
        }
    }

    return ok;
}




/*! parse an XML End tag following the initial '<'
 *  to just before the final >
 *  Note: we handle only the ASCII 7-bit subset of the tag name
 */
static bool
parse_xml_etag_body(const char **ref_line, char *buf, size_t len)
{   const char *line = *ref_line;
    bool ok = (*line++ == '/') && parse_xml_name(&line, buf, len);

    if (ok) {
        parse_space(&line);
        *ref_line = line;
    }
    return ok;
}




/*! parse an XML Start or Empty tag following the initial '<'
 *  to just before the final '>'
 *  Note: we handle only the ASCII 7-bit subset of the tag name
 */
static bool
parse_xml_tag_body(const char **ref_line, char *buf, size_t len,
                   bool *out_is_mttag, const value_t **out_dirval)
{   const char *line = *ref_line;
    bool ok = parse_xml_name(&line, buf, len);

    if (ok) {
        parse_space(&line);
        ok = parse_xml_attributes(&line, out_dirval);

        if ((*out_is_mttag = (*line == '/')))
            line++;
    }

    if (ok)
        *ref_line = line;

    return ok;
}





/*! Parse the next XML item and construct a binding from the xmlobj functions
 *  and the arguments from the parse
 */
static const value_t *
fnparse_xml_itemfn(const char **ref_line, parser_state_t *state, void *arg)
{   dir_t *xmlobj = (dir_t *)arg;
    const value_t *parsefn = NULL; /* to be a fn to parse the next item */
    const char *line = *ref_line;
    bool ok = (*line != '\0');
    bool action_found = FALSE;

    /* keep processing while no action has been taken */
    while (ok && !action_found) {
        char ch = *line;

        if (ch == '<') {
            char content[128];
            char declname[16];
            bool is_mt_tag = FALSE;

            line++;
            if (parse_xml_pi_body(&line, &content[0], sizeof(content))) {
                const value_t *fn_pi = dir_string_get(xmlobj, "pi");
                if (fn_pi != NULL && fn_pi != &value_null) {
                    action_found = TRUE;
                    parsefn = substitute(fn_pi,
                                         value_string_new_measured(&content[0]),
                                         state, /*unstrict*/FALSE);
                }
            }
            else if (parse_xml_cmt_body(&line, &content[0], sizeof(content))) {
                const value_t *fn_comment = dir_string_get(xmlobj, "cmt");
                if (fn_comment != NULL && fn_comment != &value_null) {
                    action_found = TRUE;
                    parsefn = substitute(fn_comment,
                                         value_string_new_measured(&content[0]),
                                         state, /*unstrict*/FALSE);
                }
            }
            else if (parse_xml_decl_body(&line,
                                         &declname[0], sizeof(declname),
                                         &content[0], sizeof(content))) {
                const value_t *fn_decl = dir_string_get(xmlobj, "decl");
                if (fn_decl != NULL && fn_decl != &value_null) {
                    parsefn = substitute(fn_decl,
                                         value_string_new_measured(
                                             &declname[0]),
                                         state, /*unstrict*/FALSE);
                    if (parsefn != NULL) {
                        action_found = TRUE;
                        parsefn = substitute(parsefn,
                                             value_string_new_measured(
                                                 &content[0]),
                                             state, /*unstrict*/FALSE);
                    }
                }
            }
            else if (parse_xml_etag_body(&line, &content[0], sizeof(content))) {
                const value_t *fn_etag = dir_string_get(xmlobj, "etag");
                if (fn_etag != NULL && fn_etag != &value_null) {
                    action_found = TRUE;
                    parsefn = substitute(fn_etag,
                                         value_string_new_measured(&content[0]),
                                         state, /*unstrict*/FALSE);
                }
            }
            else
            {
                const value_t *fn_mttag = dir_string_get(xmlobj, "mttag");
                const value_t *fn_stag  = dir_string_get(xmlobj, "stag");
                const value_t *attrs = NULL;

                if (fn_mttag == &value_null)
                    fn_mttag = NULL;
                if (fn_stag == &value_null)
                    fn_stag = NULL;

                if (parse_xml_tag_body(&line, &content[0], sizeof(content),
                                       &is_mt_tag,
                                       fn_mttag == NULL && fn_stag == NULL?
                                           NULL: &attrs))
                {
                    const value_t *tagname =
                        value_string_new_measured(&content[0]);
                    if (is_mt_tag) {
                        if (fn_mttag != NULL) {
                            parsefn = substitute(fn_mttag, tagname,
                                                 state, /*unstrict*/FALSE);
                            if (parsefn != NULL) {
                                action_found = TRUE;
                                parsefn = substitute(parsefn, attrs,
                                                     state, /*unstrict*/FALSE);
                            }
                       }
                   }
                   else {
                        if (fn_stag != NULL) {
                            parsefn = substitute(fn_stag, tagname,
                                                 state, /*unstrict*/FALSE);
                            if (parsefn != NULL) {
                                action_found = TRUE;
                                parsefn = substitute(parsefn, attrs,
                                                     state, /*unstrict*/FALSE);
                            }
                       }
                   }
                }
                else
                    ok = FALSE; /* unrecognizable '<...' */
            }

            ch = *line;
            ok = (ch == '>');
            if (ok)
                line++;
            else
            {
                /* recover by reading up to the next >, < or & */
                printf("%s: failed to parse '<' item - ignoring\n",
                       codeid());

                while ((ch = *line) != '>' && ch != '<' && ch != '&' &&
                       ch != '\0')
                    line++;
                if (ch == '>')
                    line++;
            }

        } else
        {
            const value_t *strval = NULL;
            ok = parse_xml_text(&line, &strval);
            if (ok && xmlobj != NULL) {
                const value_t *fn_text = dir_string_get(xmlobj, "text");
                if (fn_text != NULL && fn_text != &value_null) {
                    action_found = TRUE;
                    parsefn = substitute(fn_text, strval, state,
                                         /*unstrict*/FALSE);
                }
            }
        }
    }

    if (ok) {
        *ref_line = line;
        OMIT(printf("%s: success - %sNULL parse function returned\n",
                    codeid(), parsefn==NULL? "": "non-"););
        return parsefn;
    } else {
        OMIT(printf("%s: fail - sNULL parse function returned\n", codeid()););
        return &value_null;
    }
}




/*! Scan the next part of the XML input string and return a function
 *  that will handle it bound appropriate arguments.
 *  The functions to used are provided in an environment variable and
 *  have names that are bound arguments as follows:
 *
 *       pi "<processing instruction>"   - processing instr <?....?>
 *       comment "<comment string>"      - cmt <!--...-->
 *       decl "<type>" "<declaration>"   - decl <!type declaration>
 *       mttag "<tagname>" <attr_dir>    - empty tag <tag [name='val']* />
 *       stag "<tagname>" <attr_dir>     - start tag <tag [name='val']* >
 *       etag "<tagname>"                - end tag </tag [name='val']* >
 *       text "<string>"                 - non-tag text
 *
 *  If any of these names are absent, or have the value NULL, then those parts
 *  of the specification are parsed but ignored (the next element being
 *  consumed in its place)
 */
static const value_t *
fn_scan_xml_itemfn(const value_t *this_fn, parser_state_t *state)
{   const value_t *xmlobj = parser_builtin_arg(state, 1);
    if (value_istype(xmlobj, type_dir))
    {   dir_t *xmlobj_dir = (dir_t *)xmlobj;
        return genfn_scan(this_fn, state, 2, &fnparse_xml_itemfn,
                          (void *)xmlobj_dir);
    } else
        return &value_null;
}




static const value_t *
fnparse_xml_name(const char **ref_line, parser_state_t *state, void *arg)
{   char buf[128];
    if (parse_xml_name(ref_line, buf, sizeof(buf)))
        return value_string_new_measured(buf);
    else
        return &value_null;
}




static const value_t *
fn_scan_xml_name(const value_t *this_fn, parser_state_t *state)
{   return genfn_scan(this_fn, state, 1, &fnparse_xml_name, NULL);
}








/*****************************************************************************
 *                                                                           *
 *          FTL XML commands                                                 *
 *                                                                           *
 *****************************************************************************/




extern bool
cmds_xml(parser_state_t *state, dir_t *cmds)
{
    mod_addfn(cmds, "scanxmlid",
              "<@string> <parseobj> - parse XML-style name in <parseobj>",
              &fn_scan_xml_name, 2);
    mod_addfn(cmds, "scanxml",
              "<d> <@fn> <parseobj> - bind d.pi|cmt|decl|mttag|stag|etag|text "
              "to XML item",
              &fn_scan_xml_itemfn, 3);
    return TRUE;
}




extern void
cmds_xml_end(parser_state_t *state)
{
    return;
}
