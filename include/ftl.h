/*
 * Copyright (c) 2005-2009, Solarflare Communications Inc.
 * Copyright (c) 2014, Broadcom Inc.
 * Copyright (c) 2005-2016, Gray Girling
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

/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  cgg
**  \brief  Furtle (FTL) header
**   \date  2005/08
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef _FTL_H
#define _FTL_H

#include <stdarg.h> /* for va_list */
#include <wchar.h>  /* for wchar_t */
#include <limits.h> /* for MB_LEN_MAX */

#ifdef __cplusplus
extern "C" {
#endif

/* should be in stdbool.h, but we can't count on it being there */
#ifndef __bool_true_false_are_defined
#define bool  int
#define true  1
#define false 0
#define __bool_true_false_are_defined 1
#endif

#ifndef TRUE
#define TRUE  true
#endif

#ifndef FALSE
#define FALSE false
#endif

typedef int wbool; /* Wide BOOLEAN - for boolean pointers */
/* It is possible in some scenarios for calling code and called code not to
 * agree on the type used for 'bool' and for the compiler not to be able to see
 * this.  This is rarely a problem - except when one of the functions here
 * returns a pointer to a bool when an unexpectedly large area of memory might
 * be updated. */
    
    

/*          O/S Independence                                     */

#ifdef _WIN32

/* The following two should mimic the Windows.h types */
typedef void *PVOID;
typedef PVOID HANDLE;
typedef HANDLE thread_os_t;
#define THREAD_OS_BAD 0

/*          Numbers                                          */

#ifdef __GNUC__ /* e.g. mingw? */
typedef long long number_t;
typedef unsigned long long unumber_t;
#else
typedef __int64 number_t;
typedef unsigned __int64 unumber_t;
#endif

#define NUMBER(digits) digits##ll
#define UNUMBER(digits) digits##ull
/* printf formats */
#define F_NUMBER_T "I64d"
#define FX_UNUMBER_T "I64x"
#define FXC_UNUMBER_T "I64X"
#define FO_UNUMBER_T "I64o"
#define F_UNUMBER_T "I64u"
#define F_U32_T "u"
#define F_S32_T ""

typedef unsigned int   ftl_u32_t;
typedef signed int     ftl_s32_t;

typedef unsigned char  ftl_u8_t;
typedef signed char    ftl_s8_t;
typedef unsigned short ftl_u16_t;
typedef signed short   ftl_s16_t;
typedef unumber_t      ftl_u64_t;
typedef number_t       ftl_s64_t;

#else /* assume OSX/Linux */

#include <pthread.h> /* for threads */
typedef pthread_t thread_os_t;
#define THREAD_OS_BAD 0

#if defined(__ia64__) || defined(__x86_64__)
#define _SIZEOF_LONG 8
#endif

#ifndef _SIZEOF_LONG
#define _SIZEOF_LONG 4
#endif

#if _SIZEOF_LONG == 8

typedef long number_t;
typedef unsigned long unumber_t;
typedef unsigned int  ftl_u32_t;
typedef signed int    ftl_s32_t;

#define NUMBER(digits) digits##l
#define UNUMBER(digits) digits##ul
/* printf formats */
#define F_NUMBER_T "ld"
#define FX_UNUMBER_T "lx"
#define FXC_UNUMBER_T "lX"
#define FO_UNUMBER_T "lo"
#define F_UNUMBER_T "lu"
#define F_U32_T "u"
#define F_S32_T ""

#else

typedef long long number_t;
typedef unsigned long long unumber_t;
typedef unsigned long ftl_u32_t;
typedef signed long   ftl_s32_t;

#define NUMBER(digits) digits##ll
#define UNUMBER(digits) digits##ull
/* printf formats */
#define F_NUMBER_T "lld"
#define FX_UNUMBER_T "llx"
#define FXC_UNUMBER_T "llX"
#define FO_UNUMBER_T "llo"
#define F_UNUMBER_T "llu"
#define F_U32_T "lu"
#define F_S32_T "l"

#endif /* _SIZEOF_LONG == 8 */

typedef unsigned char  ftl_u8_t;
typedef signed char    ftl_s8_t;
typedef unsigned short ftl_u16_t;
typedef signed short   ftl_s16_t;
typedef unumber_t      ftl_u64_t;
typedef number_t       ftl_s64_t;

#endif


/*          O/S Independence - Threads                               */

#define THREAD_DEFAULT_SIZE (64<<10)

typedef unsigned thread_main_fn_t(void *arg);

typedef struct
{   thread_main_fn_t *main;
} thread_work_t;

extern thread_os_t
thread_new(thread_main_fn_t *main, thread_work_t *work, size_t stacksize);

extern int /* rc */
thread_rc(thread_os_t thread);

extern bool
thread_active(thread_os_t thread);



extern thread_os_t
thread_self(void);


/*          O/S Independence - Time                              */

extern void
sleep_ms(unsigned long milliseconds);

/*          O/S Independence - File Path                             */

/*! Open \c name on the directory path \c path writing the full name
 *  of the name (including a prefix from the directory path) to \c namebuf
 */
extern FILE *
fopen_onpath(const char *path, const char *name, size_t namelen,
             const char *mode, char *namebuf, size_t buflen);


/*          O/S Independence - Wide characters                           */

#ifdef MB_LEN_MAX
#define FTL_MB_LEN_MAX MB_LEN_MAX
#endif

#ifdef _WIN32
#ifndef FTL_MB_LEN_MAX
/* can't use MB_CUR_MAX because it is not constant */
#define FTL_MB_LEN_MAX 10
#endif
#endif

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



/*          Code ID                                              */

/*! Return code ID string */
extern const char *
codeid(void);

/*! Return upper-cased code ID string */
extern const char *
codeid_uc(void);

extern void
codeid_set(const char *codeid);

/*          Character Sinks                                  */

typedef struct charsink_s charsink_t;


extern bool /*full*/
charsink_putc(charsink_t *sink, int ch);

extern int /*bytes*/
charsink_putwc(charsink_t *sink, wchar_t ch);

extern int
charsink_write(charsink_t *sink, const char *buf, size_t len);

extern bool /*ok*/
charsink_flush(charsink_t *sink);

extern int
charsink_vsprintf(charsink_t *sink, const char *format, va_list args);

extern int
charsink_sprintf(charsink_t *sink, const char *format, ...);

/*          Parser printing                                                  */

typedef charsink_t outchar_t;

#define outchar_printf charsink_sprintf
#define outchar_write  charsink_write
#define outchar_putc   charsink_putc

#if 0
typedef FILE outchar_t;
#define outchar_printf fprintf
#define outchar_putc(out, ch) fputc(ch, out)
#endif



/*          Output Streams                                   */

extern charsink_t *
charsink_stream_new(FILE *out);

extern void
charsink_stream_delete(charsink_t **ref_sink);

/*          Output Containers                                    */

extern charsink_t *
charsink_string_new(void);

extern void
charsink_string_delete(charsink_t **ref_sink);

extern charsink_t *
charsink_fixstring_new(char *str, size_t len);

extern void
charsink_fixstring_delete(charsink_t **ref_sink);

extern void
charsink_string_buf(charsink_t *sink, const char **out_buf, size_t *out_len);

/*          Character Sources                            */

typedef struct charsource_s charsource_t;

/*! read the next character from source and returns it as an unsigned char cast
 *  to an int, or EOF on end of file or error
 */    
extern int
charsource_getc(charsource_t *source);

/*! end use of the charsource for reading
 */
extern void
charsource_close(charsource_t *source);

/*! reclaim resources allocated to the charsource
 *  Does nothing if *ref_source == NULL.  Always succeeds.
 *  Updates *ref_source to NULL.
 */
extern void
charsource_delete(charsource_t **ref_source);


/*          File-based Character Sources                     */

/*! data source is an already-open FILE * - closed on exit iff autoclose */
extern charsource_t *
charsource_stream_new(FILE *stream, const char *name, bool autoclose);

/*! data source a named file - closed on exit */
extern charsource_t *
charsource_file_new(const char *name);

/*! data source a named file - closed on exit */
extern charsource_t *
charsource_file_path_new(const char *path, const char *name, size_t namelen);

/*          Buffer-based Character Sources                   */

/*! data source is allocated copy of input string */
extern charsource_t *
charsource_string_new(const char *name, const char *string, size_t len);

/*! data source is the (constant) input string */
extern charsource_t *
charsource_cstring_new(const char *name, const char *string, size_t len);

/*          Prompting Character Sources                          */

/*! data source is a console on which input is prompted  */
extern charsource_t *
charsource_prompting_new(FILE *consolein, FILE *consoleout,
             const char *prompt);

/*          Readline Character Sources                           */

/*! data source is a "readline" console on which input is prompted  */
extern charsource_t *
charsource_readline_new(const char *prompt);

/*          Charsource Stacks                            */

typedef charsource_t *instack_t;

extern instack_t *
instack_init(instack_t *ref_stack);

extern const char *
instack_source(instack_t stack);

extern int
instack_lineno(instack_t stack);

extern void
instack_push(instack_t *ref_stack, charsource_t *source);

extern charsource_t *
instack_pop(instack_t *ref_stack);

extern bool /* not empty */
instack_popdel(instack_t *ref_stack);

extern int
instack_getc(instack_t *ref_stack);

/*          Command Line Source                          */

typedef struct linesource_s linesource_t;

extern void
linesource_save(linesource_t *lines, linesource_t *saved);

extern void
linesource_restore(linesource_t *lines, linesource_t *saved);

extern void
linesource_push(linesource_t *lines, charsource_t *source);

extern charsource_t *
linesource_pop(linesource_t *lines);

/*! Replace the current instack of char sources with an empty stack
 *  Return the previous stack
 */
extern charsource_t *
linesource_drain(linesource_t *lines);
    
extern const char *
linesource_source(linesource_t *lines);

extern int
linesource_lineno(linesource_t *lines);

extern bool
linesource_eof(linesource_t *source);

extern void
linesource_read(linesource_t *source, charsink_t *line);

extern void
linesource_pushline(linesource_t *lines, const char *name,
                    const char *cmd_str);

extern int
vreport(charsink_t *sink, linesource_t *source,
        const char *format, va_list ap);

extern int
report(charsink_t *sink, linesource_t *source, const char *format, ...);

/* continuation line */
extern int
report_line(charsink_t *sink, linesource_t *source, const char *format, ...);


/*          Memory Display                                   */


/*! Print out an area of memory in bytes, half-words or words */
extern void
mem_dump(const void *buf, unsigned addr, int entries, int sz_ln2,
         const char *addr_format, int addr_unit_size, bool with_chars);

/*! Print out an area of memory in bytes, half-words or words */
extern unsigned /* no of units different */
mem_dumpdiff(const void *buf1, const void *buf2, unsigned addr, int units,
             int sz_ln2, const char *addr_format, int addr_unit_size,
             bool with_chars);


/*          Values                                       */

typedef struct value_s value_t;
typedef const struct value_type_s *type_t;


extern const char *
value_type_name(const value_t *val);

/* true iff value is non-null with the required type */
extern bool
value_type_equal(const value_t *val, type_t kind);

/* as value_type_equal but provides an error message to stderr if wrong */
extern bool
value_istype(const value_t *val, type_t kind);

extern int
value_print_detail(outchar_t *out, const value_t *root, const value_t *val,
                   bool detailed);
    
#define value_print(out, root, val) \
    value_print_detail(out,root,val,/*detail*/FALSE)

extern int
value_cmp(const value_t *v1, const value_t *v2);

extern int
value_fprint_detail(FILE *out, const value_t *root, const value_t *val,
                    bool detailed);
    
#define value_fprint(out, root, val) \
    value_fprint_detail(out,root,val,/*detail*/FALSE)

#define VALUE_SHOW_RT(msg, root, val) \
{   printf("%s", msg);                \
    value_fprint(stdout, root, val);  \
    printf("\n");                     \
}

#define VALUE_SHOW_DR(msg, dir, val) \
        VALUE_SHOW_RT(msg, dir_value(dir), val)

#define VALUE_SHOW_ST(msg, state, val) \
        VALUE_SHOW_DR(msg, parser_env(state), val)

#define VALUE_SHOW(msg, val) VALUE_SHOW_RT(msg, NULL, val)

/*          Type Values                                      */


typedef struct value_type_s value_type_t;
typedef int type_id_t;

extern type_t type_type;

extern const char *
type_name(type_t kind);


/*          NULL Values                                      */

extern type_t type_null;
extern value_t value_null;

extern const value_t *
value_nl(const value_t *value);

/*          Integer Values                                   */

extern type_t type_int;
extern const value_t *value_zero;
extern const value_t *value_one;

extern value_t *
value_int_new(number_t number);

extern value_t *
value_uint_new(unumber_t number);

extern void
value_int_update(const value_t **ref_value, number_t n);

extern number_t
value_int_number(const value_t *value);

/*          IP Address Values                                    */

extern type_t type_ipaddr;

typedef unsigned char addr_ip_t[4];

extern int
ipaddr_fprint(FILE *out, const addr_ip_t *ip);

extern const value_t *
value_ipaddr_new(addr_ip_t *ref_ipaddr);

extern value_t *
value_ipaddr_new_quad(int a, int b, int c, int d);

extern void
value_ipaddr_get(const value_t *value, addr_ip_t *out_ipaddr);

extern bool
parse_ipaddr(const char **ref_line, addr_ip_t *out_ipaddr);

#define ipaddr_net32(ref_ip)                      \
        (((((unsigned char *)(ref_ip)))[3]<<24) | \
         ((((unsigned char *)(ref_ip)))[2]<<16) | \
         ((((unsigned char *)(ref_ip)))[1]<<8)  | \
         ((((unsigned char *)(ref_ip)))[0]) )

/*           Address Values                                  */

extern type_t type_macaddr;
typedef unsigned char addr_mac_t[6];

extern int
macaddr_fprint(FILE *out, const addr_mac_t *mac);

extern const value_t *
value_macaddr_new(addr_mac_t *ref_macaddr);

extern value_t *
value_macaddr_new_sextet(int a, int b, int c, int d, int e, int f);

extern void
value_macaddr_get(const value_t *value, unsigned char *ref_macaddr);

extern bool
parse_macaddr(const char **ref_line, addr_mac_t *out_macaddr);

/*          Boolean Values                                   */

extern const value_t *value_true;
extern const value_t *value_false;

#define value_bool(val) ((val)?value_true: value_false)
#define value_is_bool(val) ((val) == value_true || (val) == value_false)

extern bool value_istype_bool(const value_t *val);


/*          String Values                                    */

extern type_t type_string;
/* make a new string - taking a copy of the string area */
extern value_t *
value_string_new(const char *string, size_t len);

#define value_string_new_measured(_string) \
        value_string_new(_string, strlen(_string))

/*! data source is newly allocated buffer returned for initialization */
extern value_t *
value_string_alloc_new(size_t len, char **out_string);

/*! make a new string - converted from unicode string */
extern value_t *
value_wcstring_new(const wchar_t *wcstring, size_t string_wchars);

/*! make new constant string - out of existing string storage
 *  Use only when you know that the storage has static scope.
 */
extern value_t *
value_cstring_new(const char *string, size_t len);

#define value_cstring_new_measured(_string) \
        value_cstring_new(_string, strlen(_string))

extern value_t *
value_substring_new(const value_t *string, size_t offset, size_t len);

extern void
value_string_update(const value_t **ref_value, const char *str);

extern bool
value_string_get(const value_t *value, const char **out_buf, size_t *out_len);

/* create a new string that will be followed by '\0' */
extern const value_t *
value_string_get_terminated(const value_t *value, const char **out_buf,
                            size_t *out_len);

extern const char *
value_string_chars(const value_t *string);

/*          Code body Values                                     */

extern type_t type_code;

extern value_t *
value_code_new(const value_t *string, const char *defsource, int lineno);

extern bool
value_code_buf(const value_t *value, const char **out_buf, size_t *out_len);


/*          Stream Values                                                    */

extern type_t type_stream;

typedef void
stream_close_fn_t(value_t *stream);

typedef void
stream_sink_close_fn_t(charsink_t *sink);

typedef void
stream_sink_delete_fn_t(charsink_t **ref_sink);

extern void
value_stream_close(value_t *value);

extern bool
value_stream_sink(const value_t *value, charsink_t **out_sink);

extern bool
value_stream_source(const value_t *value, charsource_t **out_source);

extern bool
value_stream_takesource(value_t *value, charsource_t **out_source);

/*          Filing System Stream Values                                      */

extern value_t *
value_stream_openfile_new(FILE *file, bool autoclose,
              const char *name, bool read, bool write);

extern value_t *
value_stream_file_new(const char *name, bool binary, bool read, bool write);

extern value_t *
value_stream_file_path_new(const char *path, const char *name, size_t namelen,
               bool binary, bool read, bool write,
                       char *namebuf, size_t buflen);

/*          Socket Stream Values                                             */

extern value_t *
value_stream_opensocket_new(int fd, bool autoclose,
                           const char *name, bool read, bool write);

extern value_t *
value_stream_socket_connect_new(const char *protocol, const char *address,
                                bool read, bool write);

extern value_t *
value_stream_socket_listen_new(const char *protocol, const char *name,
                               bool read, bool write);


/*          Socket Stream Values                                             */

extern value_t *
value_stream_instring_new(const char *name, const char *string, size_t len);

extern value_t *
value_stream_outstring_new(void);

extern value_t *
value_stream_outmem_new(char *str, size_t len);

/*          Directories                                      */

extern type_t type_dir;

typedef struct dir_s dir_t;

typedef const void *dir_lock_state_t;

/*! function called when enumerating a directory - once for each entry
 *  (enumeration will cease with the provided value if the return value is not
 *   NULL)
 */
typedef void *dir_enum_fn_t(dir_t *dir, const value_t *name,
                const value_t *value, void *arg);

extern value_t *
dir_value(dir_t *dir);

extern dir_lock_state_t *
dir_lock(dir_t *dir, dir_lock_state_t *old_lock);

extern bool
dir_islocked(dir_t *dir);

extern bool
dir_set(dir_t *dir, const value_t *name, const value_t *value);

extern const value_t *
dir_get(dir_t *dir, const value_t *name);

extern bool
dir_int_set(dir_t *dir, int index, const value_t *value);

/* Set copied name to value in directory */
extern bool
dir_string_set(dir_t *dir, const char *name, const value_t *value);

/* Set statically allocated name to value in directory */
extern bool
dir_cstring_set(dir_t *dir, const char *name, const value_t *value);

extern const value_t *
dir_int_get(dir_t *dir, int n);

extern const value_t *
dir_string_get(dir_t *dir, const char *name);

extern const value_t *
dir_stringl_get(dir_t *dir, const char *name, size_t namelen);

extern bool
dir_enumerable(dir_t *dir);

/* invoke enumfn for each entry in dir providing given argument */
extern void *
dir_forall(dir_t *dir, dir_enum_fn_t *enumfn, void *arg);

extern unsigned
dir_count(dir_t *dir);

extern int
dir_fprint(FILE *out, const value_t *root, dir_t *dir);

#define DIR_SHOW_RT(msg, root, dir) \
{   printf("%s", msg);                \
    dir_fprint(stdout, root, dir);    \
    printf("\n");                     \
}

#define DIR_SHOW_DR(msg, root, dir) \
        DIR_SHOW_RT(msg, dir_value(root), dir)

#define DIR_SHOW_ST(msg, state, dir) \
        DIR_SHOW_DR(msg, parser_env(state), dir)

#define DIR_SHOW(msg, dir) DIR_SHOW_RT(msg, NULL, dir)


/*          Identifier Directories                               */

extern dir_t *
dir_id_new(void);

/*          Integer vector Directories                               */

extern dir_t *
dir_vec_new(void);

/*          Composed Directories                                 */

extern dir_t *
dir_join_new(dir_t *index_dir, dir_t *value_dir);

/*          Field of Array/Structure Definition                          */

typedef struct field_s field_t;

typedef enum
{   field_kind_val = 1,
    field_kind_struct,
    field_kind_union,
    field_kind_array
} field_kind_t;

typedef void field_get_fn_t(const void *s, const value_t **ref_cached);
typedef void field_set_fn_t(void *s, const value_t *val);

extern void
field_init(field_t *ref_field, field_kind_t kind,
           field_set_fn_t *set, field_get_fn_t *get);

extern void
field_noset(void *mem, const value_t *val);

/*          Structure Directories                                */

typedef struct struct_field_s struct_field_t;

typedef struct struct_spec_s struct_spec_t;
struct struct_spec_s {
    struct_field_t *fields;
    struct_field_t **end_fields;
} /* struct_spec_t */;

extern void
struct_spec_init(struct_spec_t *spec);

extern void
struct_spec_end(struct_spec_t *spec);

extern void
struct_spec_add_field(struct_spec_t *spec, field_kind_t kind,
              const char *name,
              field_set_fn_t *set, field_get_fn_t *get);


#define FTL_DECLARE(struct_macro) struct_macro(DECL)
#define FTL_DEFINE(struct_macro)  struct_macro(DEF)
#define FTL_UNDEFINE(struct_macro)  struct_macro(UNDEF)

#define FTL_TSPEC(type) _ftlspec__##type

#define _FTL_STRUCT_BEGIN_DECL(_spec, tag) \
        static struct_spec_t _spec;
#define _FTL_STRUCT_BEGIN_DEF(_spec, tag) \
        struct_spec_init(&_spec);
#define _FTL_STRUCT_BEGIN_UNDEF(_spec, tag) \
        struct_spec_end(&_spec);
#define FTL_STRUCT_BEGIN(_ctx, _spec, tag) _FTL_STRUCT_BEGIN_##_ctx(_spec, tag)
#define FTL_TSTRUCT_BEGIN(_ctx, _type, tag) \
        FTL_STRUCT_BEGIN(_ctx, FTL_TSPEC(_type), tag)


#define _FTL_STRUCT_END_DECL()
#define _FTL_STRUCT_END_DEF()
#define _FTL_STRUCT_END_UNDEF()
#define FTL_STRUCT_END(_ctx) _FTL_STRUCT_END_##_ctx()
#define FTL_TSTRUCT_END(_ctx) FTL_STRUCT_END(_ctx)

#define FTL_UNION_BEGIN     FTL_STRUCT_BEGIN
#define FTL_TUNION_BEGIN    FTL_TSTRUCT_BEGIN
#define FTL_UNION_END       FTL_STRUCT_END
#define FTL_TUNION_END      FTL_TSTRUCT_END

#define FTL_VARS_BEGIN(ctx, spec) \
                            FTL_STRUCT_BEGIN(ctx, spec, )
#define FTL_VARS_END        FTL_STRUCT_END


#define _FTL_FIELD_INT_DECL(spec, stype, ftype, field)              \
    static void stype##__##field##__get(const void *mem,                \
                                        const value_t **ref_cached)     \
    {   value_int_update(ref_cached, ((stype *)mem)->field); }      \
    static void stype##__##field##__set(void *mem, const value_t *val)  \
    {   ((stype *)mem)->field = (ftype)value_int_number(val); }
#define _FTL_FIELD_INT_DEF(spec, stype, ftype, field)           \
    struct_spec_add_field(&spec, field_kind_val, #field,        \
              &stype##__##field##__set, &stype##__##field##__get);
#define _FTL_FIELD_INT_UNDEF(spec, stype, ftype, field)
#define FTL_FIELD_INT(_ctx, spec, stype, ftype, field)              \
        _FTL_FIELD_INT_##_ctx(spec, stype, ftype, field)
#define FTL_TFIELD_INT(_ctx, _stype, ftype, field)              \
        FTL_FIELD_INT(_ctx, FTL_TSPEC(_stype), _stype, ftype, field)


#define _FTL_FIELD_CONSTINT_DECL(spec, stype, ftype, field)         \
    static void stype##__##field##__get(const void *mem,                \
                                        const value_t **ref_cached)     \
    {   value_int_update(ref_cached, ((stype *)mem)->field); }
#define _FTL_FIELD_CONSTINT_DEF(spec, stype, ftype, field)      \
    struct_spec_add_field(&spec, field_kind_val, #field,        \
              NULL, &stype##__##field##__get);
#define _FTL_FIELD_CONSTINT_UNDEF(spec, stype, ftype, field)
#define FTL_FIELD_CONSTINT(_ctx, spec, stype, ftype, field)         \
        _FTL_FIELD_CONSTINT_##_ctx(spec, stype, ftype, field)
#define FTL_TFIELD_CONSTINT(_ctx, _stype, ftype, field)             \
        FTL_FIELD_CONSTINT(_ctx, FTL_TSPEC(_stype), _stype, ftype, field)


/* Note: these fields can be used to override a "const" field so that the
   compiler will allow us to write to it - this can be unsafe in some
   compilers.  Also the technique used can not be applied to fields that are
   based on bitfields. */
#define _FTL_FIELD_UNCONSTINT_DECL(spec, stype, ftype, field)           \
    static void stype##__##field##__get(const void *mem,                \
                                        const value_t **ref_cached)     \
    {   value_int_update(ref_cached, ((stype *)mem)->field); }      \
    static void stype##__##field##__set(void *mem, const value_t *val)  \
    {   ftype *ptr = (ftype *)&(((stype *)mem)->field);                 \
        *ptr = (ftype)value_int_number(val);                            \
    }
#define _FTL_FIELD_UNCONSTINT_DEF(spec, stype, ftype, field)        \
    struct_spec_add_field(&spec, field_kind_val, #field,        \
              &stype##__##field##__set, &stype##__##field##__get);
#define _FTL_FIELD_UNCONSTINT_UNDEF(spec, stype, ftype, field)
#define FTL_FIELD_UNCONSTINT(_ctx, spec, stype, ftype, field)           \
        _FTL_FIELD_UNCONSTINT_##_ctx(spec, stype, ftype, field)
#define FTL_TFIELD_UNCONSTINT(_ctx, _stype, ftype, field)           \
        FTL_FIELD_UNCONSTINT(_ctx, FTL_TSPEC(_stype), _stype, ftype, field)


#define _FTL_FIELD_STRUCT_DECL(spec, stype, ftype, field, fspec)  \
    static void stype##__##field##__get(const void *mem,                \
                                        const value_t **ref_cached)     \
    {   dir_struct_update(ref_cached, &fspec, /*is_const*/FALSE,        \
                          (void *)&((stype *)mem)->field);              \
    }
#define _FTL_FIELD_STRUCT_DEF(spec, stype, ftype, field, fspec)         \
    struct_spec_add_field(&spec, field_kind_struct, #field,     \
              &field_noset, &stype##__##field##__get);
#define _FTL_FIELD_STRUCT_UNDEF(spec, stype, ftype, field, fspec)
#define FTL_FIELD_STRUCT(_ctx, spec, stype, ftype, field, fspec)    \
        _FTL_FIELD_STRUCT_##_ctx(spec, stype, ftype, field, fspec)
#define FTL_TFIELD_STRUCT(_ctx, _stype, ftype, field)           \
        FTL_FIELD_STRUCT(_ctx, FTL_TSPEC(_stype), _stype,               \
                         ftype, field, FTL_TSPEC(ftype))


#define _FTL_VAR_INT_DECL(spec, vtype, var)                     \
    static void _var__##var##__get(const void *mem,                     \
                                   const value_t **ref_cached)          \
    {   value_int_update(ref_cached, var); }                            \
    static void _var__##var##__set(void *mem, const value_t *val)   \
    {   var = (vtype)value_int_number(val); }
#define _FTL_VAR_INT_DEF(spec, vtype, var)                  \
    struct_spec_add_field(&spec, field_kind_val, #var,              \
              &_var__##var##__set, &_var__##var##__get);
#define _FTL_VAR_INT_UNDEF(spec, vtype, var)
#define FTL_VAR_INT(_ctx, spec, vtype, var)                     \
        _FTL_VAR_INT_##_ctx(spec, vtype, var)


#define _FTL_VAR_CONSTINT_DECL(spec, vtype, var)                \
    static void _var__##var##__get(const void *mem,                     \
                                   const value_t **ref_cached)          \
    {   value_int_update(ref_cached, var); }
#define _FTL_VAR_CONSTINT_DEF(spec, vtype, var)                 \
    struct_spec_add_field(&spec, field_kind_val, #var,              \
              NULL, &_var__##var##__get);
#define _FTL_VAR_CONSTINT_UNDEF(spec, vtype, var)
#define FTL_VAR_CONSTINT(_ctx, spec, vtype, var)                    \
        _FTL_VAR_CONSTINT_##_ctx(spec, vtype, var)


#define _FTL_VAR_CONSTSTR_DECL(spec, vtype, var)                \
    static void _var__##var##__get(const void *mem,                     \
                                   const value_t **ref_cached)          \
    {   value_string_update(ref_cached, var); }
#define _FTL_VAR_CONSTSTR_DEF(spec, vtype, var)                 \
    struct_spec_add_field(&spec, field_kind_val, #var,              \
              NULL, &_var__##var##__get);
#define _FTL_VAR_CONSTSTR_UNDEF(spec, vtype, var)
#define FTL_VAR_CONSTSTR(_ctx, spec, vtype, var)                    \
        _FTL_VAR_CONSTSTR_##_ctx(spec, vtype, var)


extern dir_t *
dir_struct_new(struct_spec_t *spec, bool is_const, void *malloc_struct);

extern dir_t *
dir_struct_cast(struct_spec_t *spec, bool is_const,
        const value_t *ref, void *ref_struct);

#define dir_vars(spec, const, ref) dir_struct_cast(spec, const, ref, NULL)

extern dir_t *
dir_cstruct_new(struct_spec_t *spec, bool is_const, void *static_struct);

extern void
dir_struct_update(const value_t **ref_value,
          struct_spec_t *spec, bool is_const, void *structmem);


/*          Array Directories                                */

typedef struct
{   field_t *field;
    size_t elems;
} array_spec_t;

extern void
array_spec_init(array_spec_t *spec);

extern void
array_spec_end(array_spec_t *spec);

extern void
array_spec_set_cont(array_spec_t *spec, field_kind_t kind, size_t elems,
                    field_set_fn_t *set, field_get_fn_t *get);


#define FTL_ARRAY_ELEMS(array)  (sizeof(array)/sizeof((array)[0]))
#define FTL_ARRAY_STRIDE(array) ((char *)(&(array)[1]) - (char *)(&(array)[0]))

#define _FTL_ARRAY_BEGIN_DECL(_spec) \
        static array_spec_t _spec;
#define _FTL_ARRAY_BEGIN_DEF(_spec) \
        array_spec_init(&_spec);
#define _FTL_ARRAY_BEGIN_UNDEF(_spec) \
        array_spec_end(&_spec);
#define FTL_ARRAY_BEGIN(_ctx, _spec) \
        _FTL_ARRAY_BEGIN_##_ctx(_spec)
#define FTL_TARRAY_BEGIN(_ctx, _type) \
        FTL_ARRAY_BEGIN(_ctx, FTL_TSPEC(_type))

#define _FTL_ARRAY_END_DECL()
#define _FTL_ARRAY_END_DEF()
#define _FTL_ARRAY_END_UNDEF()
#define FTL_ARRAY_END(_ctx) _FTL_ARRAY_END_##_ctx()
#define FTL_TARRAY_END(_ctx) FTL_ARRAY_END(_ctx)

#define _FTL_ARRAY_INT_DECL(spec, stype, ctype, elems)            \
    static void stype##__get(const void *mem, const value_t **ref_cached) \
    {   value_int_update(ref_cached, ((ctype *)(mem))[0]); }          \
    static void stype##__set(void *mem, const value_t *val)           \
    {   ((ctype *)(mem))[0] = (ctype)value_int_number(val);}
#define _FTL_ARRAY_INT_DEF(spec, stype, ctype, elems)                 \
    array_spec_set_cont(&spec, field_kind_val, elems,                     \
                &stype##__set, &stype##__get);
#define _FTL_ARRAY_INT_UNDEF(spec, stype, ctype, elems)
#define FTL_ARRAY_INT(_ctx, spec, _atype, ctype, elems)               \
        _FTL_ARRAY_INT_##_ctx(spec, _atype, ctype, elems)
#define FTL_TARRAY_INT(_ctx, _atype, _ctype, elems)           \
        FTL_ARRAY_INT(_ctx, FTL_TSPEC(_atype), _atype, _ctype, elems)


#define _FTL_ARRAY_STRUCT_DECL(aspec, _atype_uid, cspec, elems)           \
    static void _atype_uid##___get(const void *mem,                       \
                   const value_t **ref_cached)            \
    {   dir_struct_update(ref_cached, &cspec, /*is_const*/FALSE,          \
                          (void *)mem);                               \
    }
#define _FTL_ARRAY_STRUCT_DEF(aspec, _atype_uid, cspec, elems)            \
    array_spec_set_cont(&aspec, field_kind_struct, elems,                 \
            &field_noset, &_atype_uid##___get);
#define _FTL_ARRAY_STRUCT_UNDEF(aspec, _atype_uid, cspec, elems)
#define FTL_ARRAY_STRUCT(_ctx, aspec, _atype, cspec, elems)               \
    _FTL_ARRAY_STRUCT_##_ctx(aspec, _atype, cspec, elems)
#define FTL_TARRAY_STRUCT(_ctx, _atype, _ctype, elems)                \
        FTL_ARRAY_STRUCT(_ctx, FTL_TSPEC(_atype), _atype,         \
                         FTL_TSPEC(_ctype), elems)

#define _FTL_FIELD_ARRAY_DECL(sspec, stype, field, fspec)                 \
    static void stype##__##field##__get(const void *mem,                  \
                                        const value_t **ref_cached)       \
    {   dir_array_update(ref_cached, &fspec, /*is_const*/FALSE,           \
             ((stype *)mem)->field,               \
             FTL_ARRAY_STRIDE(((stype *)mem)->field));    \
    }
#define _FTL_FIELD_ARRAY_DEF(sspec, stype, field, fspec)              \
    struct_spec_add_field(&sspec, field_kind_array, #field,       \
              &field_noset, &stype##__##field##__get);
#define _FTL_FIELD_ARRAY_UNDEF(sspec, stype, field, fspec)
#define FTL_FIELD_ARRAY(_ctx, sspec, _stype,  field, fspec)           \
        _FTL_FIELD_ARRAY_##_ctx(sspec, _stype, field, fspec)
#define FTL_TFIELD_ARRAY(_ctx, _stype, _ftype, field)                 \
        FTL_FIELD_ARRAY(_ctx, FTL_TSPEC(_stype), _stype,          \
            field, FTL_TSPEC(_ftype))


#define FTL_FIELD_ARRAYOFSTRUCT(_ctx, sspec, _stype, field, cspec, elems) \
        FTL_ARRAY_BEGIN(_ctx, FTL_TSPEC(_stype##__##field))       \
    FTL_ARRAY_STRUCT(_ctx, FTL_TSPEC(_stype##__##field),              \
              _stype##__##field, cspec, elems)            \
        FTL_ARRAY_END(_ctx)                                               \
    FTL_FIELD_ARRAY(_ctx, sspec, _stype, field,                       \
            FTL_TSPEC(_stype##__##field))
#define FTL_TFIELD_ARRAYOFSTRUCT(_ctx, _stype, _ctype, field, elems)      \
        FTL_FIELD_ARRAYOFSTRUCT(_ctx, FTL_TSPEC(_stype), _stype,      \
              field, FTL_TSPEC(_ctype), elems)

#define FTL_FIELD_ARRAYOFINT(_ctx, sspec, _stype, field, _ctype, elems)   \
        FTL_ARRAY_BEGIN(_ctx, FTL_TSPEC(_stype##__##field))       \
    FTL_ARRAY_INT(_ctx, FTL_TSPEC(_stype##__##field),                 \
              _stype##__##field##__array, _ctype, elems)      \
        FTL_ARRAY_END(_ctx)                                               \
    FTL_FIELD_ARRAY(_ctx, sspec, _stype, field,                       \
            FTL_TSPEC(_stype##__##field))
#define FTL_TFIELD_ARRAYOFINT(_ctx, _stype, _ctype, field, elems)     \
        FTL_FIELD_ARRAYOFINT(_ctx, FTL_TSPEC(_stype), _stype,             \
              field, _ctype, elems)


/* you may wish to use dir_lock() after the following calls */

extern dir_t *
dir_carray_new(array_spec_t *spec,bool is_const,
           void *static_array, size_t stride);

extern dir_t *
dir_array_new(array_spec_t *spec, bool is_const,
          void *malloc_array, size_t stride);

extern dir_t *
dir_array_cast(array_spec_t *spec, bool is_const,
           const value_t *ref, void *ref_array, size_t stride);

extern dir_t *
dir_array_string(array_spec_t *spec, bool is_const,
             const value_t *string, size_t stride);

extern void
dir_array_update(const value_t **ref_value, array_spec_t *spec, bool is_const,
         void *arraymem, size_t stride);

/*          String Argument vector Directories                           */

extern dir_t *
dir_argvec_new(int argc, const char **argv);

/*          Integer Series Directories                               */

extern dir_t *
dir_series_new(number_t first, number_t inc, number_t last);

/*          System Env Directories                               */

extern dir_t *
dir_sysenv_new(void);


/*          Stacked Directory Directories                                */

typedef struct dir_stack_s dir_stack_t;

typedef value_t **dir_stack_pos_t;
#define DIR_STACK_POS_BAD ((dir_stack_pos_t)NULL)

extern dir_t *
dir_stack_new(void);

extern dir_stack_t *
dir_stack_copy(dir_stack_t *old);

extern dir_stack_pos_t
dir_stack_push(dir_stack_t *dir, dir_t *newdir, bool env_end);

extern dir_stack_pos_t
dir_stack_pos_enclosing(dir_stack_t *dir, dir_t *innerdir);

extern void
dir_stack_return(dir_stack_t *dir, dir_stack_pos_t pos);

extern dir_stack_pos_t
dir_stack_last_pos(dir_stack_t *dir);



/*          Closure Environments                                 */

extern type_t type_closure;

typedef struct value_env_s value_env_t;

extern value_env_t *
value_env_new(void);

extern void
value_env_pushdir(value_env_t *env, dir_t *newdir, bool env_end);

extern value_t * /*pos*/
value_env_pushunbound(value_env_t *env, value_t *pos, value_t *name);

extern value_env_t *
value_env_newpushdir(dir_t *newdir, bool env_end, value_t *unbound);

extern bool
value_env_pushenv(value_env_t *env, value_env_t *newenv, bool env_end);

extern value_t *
value_env_bind(value_env_t *envval, const value_t *value);

/*          Closure Values                                   */

#define value_is_codebody(_val)         \
   (value_type_equal(_val, type_code) || \
    value_type_equal(_val, type_cmd)  || \
    value_type_equal(_val, type_func))

extern value_t *
value_closure_new(const value_t *code, value_env_t *env);

extern bool
value_closure_pushdir(const value_t *value, dir_t *dir, bool env_end);

extern bool
value_closure_pushenv(const value_t *value, value_env_t *env, bool env_end);

/*! return the component code, environment and unbound arguments of a closure */
extern bool
value_closure_get(const value_t *value, const value_t **out_code,
          dir_t **out_env, const value_t **out_unbound);

/*! give a closure an unbound argument */
extern value_t * /*pos*/
value_closure_pushunbound(value_t *value, value_t *pos, value_t *name);

/*! return the number of unbound arguments in a closure */
extern int
value_closure_argcount(const value_t *closureval);

/*! bind a new argument to a closure */
extern value_t *
value_closure_bind(const value_t *envval, const value_t *value);


/*          Transfer Functions                               */

extern bool
value_to_dir(const value_t *val, dir_t **out_dir);

extern bool
value_as_dir(const value_t *val, dir_t **out_dir); /* complains if uncast */

/*          Coroutine Values                                 */

extern type_t type_coroutine;

typedef struct value_coroutine_s value_coroutine_t;
typedef void suspend_fn_t(unsigned long milliseconds);

extern value_coroutine_t *
value_coroutine_new(dir_t *root, dir_stack_t *env, dir_t *opdefs);


/*          Parser State                                 */

typedef value_coroutine_t parser_state_t;

extern parser_state_t *
parser_state_new(dir_t *root);

extern void
parser_state_free(parser_state_t *state);

extern linesource_t *
parser_linesource(parser_state_t *parser_state);

extern dir_t *
parser_root(parser_state_t *parser_state);

extern dir_t *
parser_env(parser_state_t *parser_state);

extern dir_t *
parser_env_copy(parser_state_t *parser_state);

extern dir_t *
parser_opdefs(const parser_state_t *parser_state);

extern const value_t *
parser_builtin_arg(parser_state_t *parser_state, int argno);

extern dir_stack_t *
parser_env_stack(const parser_state_t *parser_state);

extern dir_stack_pos_t
parser_env_push(parser_state_t *parser_state, dir_t *newdir,
                bool outer_visible);

extern bool /* ok */
parser_env_push_at_pos(parser_state_t *parser_state, dir_stack_pos_t pos,
                       dir_t *newdir, bool outer_visible);

/* Restore environment stack back to previous (outer) level */
extern void
parser_env_return(parser_state_t *parser_state, dir_stack_pos_t pos);

/* The environment stack position when the current function was called */
extern dir_stack_pos_t
parser_env_calling_pos(const parser_state_t *parser_state);

extern bool
parser_echoto(const parser_state_t *parser_state, FILE **out_log,
              const char **out_fmt);

extern void
parser_echo_setlog(parser_state_t *parser_state, FILE *log, const char *fmt);

#define parser_echo(state) \
    (parser_echoto(state, NULL, NULL))
    
#define parser_echo_set(state, on) \
    (parser_echo_setlog(state, (on)? stderr: NULL), NULL)

extern suspend_fn_t *
parser_suspend_get(const parser_state_t *parser_state);

extern void
parser_suspend_set(parser_state_t *parser_state, suspend_fn_t *sleep);

extern int
charsink_parser_vreport(charsink_t *sink, parser_state_t *parser_state,
                    const char *format, va_list ap);

extern int
charsink_parser_vreport_line(charsink_t *sink, parser_state_t *parser_state,
                         const char *format, va_list ap);

#define parser_vreport(state, format, ap) \
        charsink_parser_vreport(NULL, state, format, ap)

#define parser_vreport_line(state, format, ap) \
        charsink_parser_vreport_line(NULL, state, format, ap)

extern int
charsink_parser_value_print(charsink_t *sink, parser_state_t *parser_state,
                const value_t *val);

#define parser_value_print(state, val)  \
        charsink_parser_value_print(NULL, state, val)

extern int
parser_report(parser_state_t *parser_state, const char *format, ...);

extern int
parser_error(parser_state_t *parser_state, const char *format, ...);

extern const value_t *
parser_errorval(parser_state_t *parser_state, const char *format, ...);

extern int
parser_error_count(parser_state_t *parser_state);

extern int
parser_report_help(parser_state_t *parser_state, const value_t *cmd);

/* throw an exception returning TRUE iff it succeeded */
extern bool
parser_throw(parser_state_t *parser_state, const value_t *exception);

typedef const value_t *parser_call_fn_t(parser_state_t *state, void *call_arg);

extern const value_t *
parser_catch_call(parser_state_t *state, parser_call_fn_t *call,
                  void *call_arg, wbool *out_ok);

/* invoke code, catching any 'throw'
 * set *out_ok TRUE iff there was no throw
 * return the result of executing code if there was no throw
 * return the thrown value if there was a throw
 */
extern const value_t *
parser_catch_invoke(parser_state_t *state, const value_t *code, wbool *out_ok);

extern void
parser_collect(parser_state_t *state);

extern outchar_t *
parser_expand(parser_state_t *state, outchar_t *out,
          const char *phrase, size_t len);


/*          Dynamic Directories                                 */

/*! create a new dynamic environment from FTL functions
 *    @param state     - parser state to use for function execution [!!]
 *    @param get_fn    - NULL or closure with 1 arg to retrieve name
 *    @param set_fn    - NULL or closure with 2 args to set name to value
 *    @param getall_fn - closure with 1 args to enumerate names & values
 *    @param count_fn  - NULL or code or closure with 0 args to count entries
 */

extern dir_t *
dir_dyn_new(parser_state_t *state, const char *errprefix,
            const value_t *get_fn, const value_t *set_fn,
            const value_t *getall_fn, const value_t *count_fn);


/*          Line Parsing                             */


/* parse the end of the string (i.e. succeed only if line empty) */
extern bool
parse_empty(const char **ref_line);

/* parse white space (succeeds only if there is some) */
extern bool
parse_white(const char **ref_line);

/* parse optional white space (always succeeds) */
extern bool
parse_space(const char **ref_line);

/* parse the prefix given by key */
extern bool
parse_key(const char **ref_line, const char *key);

/* parse the prefix ending with the key */
extern bool
parse_ending(const char **ref_line, const char *key);

/* parse a signed 64-bit decimal number */
extern bool
parse_int(const char **ref_line, number_t *out_int);

/* parse an unsigned 64-bit hexadecimal number */
extern bool
parse_hex(const char **ref_line, unumber_t *out_int);

/* parse an unsigned 64-bit octal number */
extern bool
parse_octal(const char **ref_line, unumber_t *out_int);

/* parse an unsigned 64-bit octal number taking width digits  */
extern bool
parse_hex_width(const char **ref_line, unsigned width, unumber_t *out_int);

/*! parse [-] [ 0[x|X]<nex> | 0[o|O]<octal> | <decimal> ] */
extern bool
parse_int_val(const char **ref_line, number_t *out_int);

/*! parse [<int> | ( <FTL expr> )] integer expression */
extern bool
parse_int_expr(const char **ref_line,
           parser_state_t *state, number_t *out_int);

/*! parse <non-space-non-delim>* */
extern bool
parse_item(const char **ref_line, const char *delims, size_t ndelims,
       char *buf, size_t len);

/*! parse [<alpha>|_][<alpha>|<num>|_]* */
extern bool
parse_id(const char **ref_line, char *buf, size_t size);

/*! parse single or double quoted string
 *  note: succeeds if syntax is correct even if insufficient room in the buffer
 *        including when there is no room for a terminating null (len==0)
 */
extern bool
parse_string(const char **ref_line, char *buf, size_t len, size_t *out_len);

/* note: succeeds if syntax is correct even if insufficient room in the buffer
         including when there is no room for a terminating null (len==0) */
extern bool
parse_string_expr(const char **ref_line, parser_state_t *state,
          char *buf, size_t len, const value_t **out_string);

extern bool
parse_itemstr(const char **ref_line, char *buf, size_t size);

extern bool
parse_type(const char **ref_line, type_t *out_type_id);

typedef bool
parse_match_fn_t(const char **ref_line, parser_state_t *state,
         const value_t *name, void *arg);

/*! parse one of the names in 'prefixes' and return the associated value
 *  where match_fn is used to match the name with the parse object
 */
extern bool
parse_oneof_matching(const char **ref_line, parser_state_t *state,
                 dir_t *prefixes, const value_t **out_val,
             parse_match_fn_t *match_fn, void *match_fn_arg);

/*! parse one of the names in 'prefixes' and return the associated value
 */
extern bool
parse_oneof(const char **ref_line, parser_state_t *state, dir_t *prefixes,
            const value_t **out_val);

/*! parse text ending with one of the names in 'delims' and return the
 *  associated value
 */
extern bool
parse_one_ending(const char **ref_line, parser_state_t *state, dir_t *delims,
                 const value_t **out_val);



/*          Command Values                               */

extern type_t type_cmd;

typedef const value_t *cmd_fn_t(const char **ref_line, const value_t *this_cmd,
                                parser_state_t *state);

extern value_t *
value_cmd_new(cmd_fn_t *exec, const value_t *fn_exec, const char *help);

extern const char *
value_cmd_help(const value_t *cmd);


/*          Function Values                              */

extern type_t type_func;

typedef const value_t *func_fn_t(const value_t *this_func,
                 parser_state_t *state);

extern value_t *
value_func_new(func_fn_t *exec, const char *help, int args, void *implicit);

extern void *
value_func_implicit(const value_t *func); /* deliver implicit arguments */

extern const char *
value_func_help(const value_t *func);

/*          Memory                                       */

extern type_t type_mem;

extern value_t *
value_mem_bin_new(const value_t *binstr, number_t base,
                  bool sole_user, bool readonly);

extern value_t *
value_mem_bin_alloc_new(number_t base, size_t len, int memset_val,
                        char **out_block);

extern value_t *
value_mem_rebase_new(const value_t *unbase_mem_val,
                     number_t base, bool readonly, bool sole_user);



/*          Modules                                          */

/*! Add a new command (cmd) to a module directory */
extern value_t *
mod_add(dir_t *dir, const char *name, const char *help, cmd_fn_t *exec);

/*! Add a new function to a module directory */
extern value_t *
mod_addfn(dir_t *dir, const char *name, const char *help, func_fn_t *exec,
      int args);

/*! Add a new function with bound implicit args to a module directory */
extern value_t *
mod_addfn_imp(dir_t *dir, const char *name, const char *help, func_fn_t *exec,
          int args, void *implicit_args);

/*! Add a new directory to a module directory */
extern void
mod_add_dir(dir_t *dir, const char *name, dir_t *mod);

/*! Add a new value to a module directory */
extern void
mod_add_val(dir_t *dir, const char *name, const value_t *val);

/*! Parse a value to execute taken from a given module */
extern bool
mod_parse_cmd(dir_t *dir, const char **ref_line, const value_t **out_cmd);

/*! Retrieve any implicit argument defined for this function in its module */
extern void *
mod_get_implicit(const value_t *this_fn);

/*          Value Parsing                            */



#define value_is_invokable(_val)         \
   (value_type_equal(_val, type_code) || \
    value_type_equal(_val, type_closure))

/* check for type as above and give error message in incorrect */
extern bool value_istype_invokable(const value_t *val);


/*! Create a binding providing an argument to some code (resulting in a closure)
 *  if 'unstrict' is set the substitution will not fail if there are not enough
 *  unbound symbols for the substitution (the code value itself will be
 *  returned) - i.e. unnecessary arguments will not be reported (or used)
 */
extern const value_t *
substitute(const value_t *code, const value_t *arg, parser_state_t *state,
           bool unstrict);

/*! Invoke a binding */
extern const value_t *
invoke(const value_t *code, parser_state_t *state);

extern bool
parse_int_base(const char **ref_line, parser_state_t *state,
           number_t *out_int);

extern bool
parse_code(const char **ref_line, parser_state_t *state,
           const value_t **out_strval,
           const char **out_sourcepos, int *out_lineno);

/* most complex expression built using delimiters */
extern bool
parse_retrieval(const char **ref_line, parser_state_t *state,
        const value_t **out_val);

extern bool
parse_expr(const char **ref_line, parser_state_t *state,
       const value_t **out_val);


/*          Command Line Interpreter                         */

typedef bool register_opt_result_fn(parser_state_t *state, const char *cmd,
                                    const value_t *result, void *arg);

/*! execute the command provided in a string
 *
 *  A pointer to the string is provided in ref_line and a command line
 *  is parsed and run.  The pointer is then updated to the end of the parsed
 *  text.
 */
extern const value_t *
mod_exec_cmd(const char **ref_line, parser_state_t *state);

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
                 const char *delim, const char *term,
                 const char *execpath, dir_t *fndir, bool expect_no_locals,
                 register_opt_result_fn *with_results, void *with_results_arg,
                 const value_t **out_value, wbool *out_ends_with_delim);

/*! Execute the commands provided from three sources in order
 *  This is an internal function implementing a range of others (that follow
 *  as macro definitions)
 *
 *  The parser's current instack is saved and restored once the instack provided
 *  here is completed.
 *
 *  First execute the commands in \c rcfile_id - which is found on the
 *     RC directory path - unless \c rcfile_id is NULL
 * 
 *  Second execute the commands taken from \c cmd_str - if it is not NULL
 *
 *  Third execute the commands from the the character source \c source
 *
 *  Finally garbage collect unless \c expect_no_locals is FALSE
 *
 *  These three sources are stacked on the stack of character sources and
 *  then lines are read and executed until EOF is returned (i.e. the character
 *  source stack is empty).
 *
 *  Each line is executed with \c mod_exec_cmd with exception handling in place
 *  if \c interactive is TRUE.
 *
 *  Unless /c expect_no_locals is TRUE the routine returns a value - the one
 *  resulting from the last \c mod_exec_cmd() run
 */
extern const value_t *
parser_expand_exec_int(parser_state_t *state, charsource_t *source,
               const char *cmd_str, const char *rcfile_id,
               bool expect_no_locals, bool interactive);

#define parser_expand_exec(state, source, cmd_str, rc_file_id, no_locals)     \
        parser_expand_exec_int(state, source, cmd_str, rc_file_id, no_locals, \
                               FALSE)

#define parser_expand_string_exec(state, cstring, stringlen, no_locals)       \
        parser_expand_exec(state,                                             \
               charsource_cstring_new(#cstring, cstring,          \
                                                  stringlen),                 \
                           NULL, NULL, no_locals)

#define parser_expand_file_exec(state, filename, no_locals)                   \
        parser_expand_exec(state, charsource_file_new(filename),              \
                           NULL, NULL, no_locals)

#define parser_expand_file_path_exec(state, path, filename, no_locals)        \
        parser_expand_exec(state, charsource_file_path_new(path, filename,    \
                                                           strlen(filename)), \
                           NULL, NULL, no_locals)


/*! Find charsource for an initialization file based on code name.
 *  The charsource is opened in 'autoclose' mode.
 *  If out_rcfile is NULL nothing is written to it otherwise the FILE *
 *  corresponding to the init file is returned.
 */
extern charsource_t *
charsource_rcfile(parser_state_t *state, const char *rccode_name,
                  FILE **out_rcfile);


/*! Start an interactive ftl interpreter reading rcfile initially
 *  Commands are read from the terminal and output is written to it.
 *  If the rcfile is NULL no commands are read from it.  If the init_cmds is
 *  NULL no commands are read from the RC file.
 */
extern void
cli(parser_state_t *state, const char *init_cmds, const char *rcfile);

    
/*! Run an ftl interpreter taking commands from argv
 *  Comma (,) tokens are used to separate runs of tokens that are interpreted
 *  as separate FTL commands
 *    @param state     - current parser state
 *    @param code_name - name to call the parser during this phase
 *    @param execpath  - file directory path from which to take FTL sources
 *    @param argv      - reference to 1st sym in an array of symbols
 *    @param argc      - number of syms in array
 *    @param out_ends_with_comma - TRUE when last parsed item is the delimiter
 */
extern bool
argv_cli_ending(parser_state_t *state, const char *code_name,
                const char *execpath, const char **argv, int argc,
                wbool *out_ends_with_comma);

#define argv_cli(state, code_name, execpath, argv, argc) \
        argv_cli_ending(state, code_name, execpath, argv, argc, NULL)
    

/*! Run an ftl interpreter taking commands from argv
 *  Only the initial commands that consist of --opt style tokens are parsed.
 *  as separate FTL commands.
 *  The argv provided is updated and provides the remaining tokens.
 *    @param state     - current parser state
 *    @param code_name - name to call the parser during this phase
 *    @param execpath  - file directory path from which to take FTL sources
 *    @param ref_argv  - updatable reference to 1st sym in an array of symbols
 *    @param ref_argc  - updatable reference number of syms in array
 *    @param fndir     - directory to take parsing function definitions from
 *    @param with_results - function called for the results of every option call
 */
extern bool
argv_opt_cli(parser_state_t *state, const char *code_name, const char *execpath,
             const char ***ref_argv, int *ref_argc, dir_t *fndir,
             register_opt_result_fn *with_results, void *with_results_arg);


/*          Generic Commands                                 */

extern void
cmds_generic(parser_state_t *state, int argc, const char **argv);

/*! tidy up once generic commands are no longer required */
extern void
cmds_generic_end(parser_state_t *state);

/*          Library initialization                           */

extern void
ftl_version(int *out_major, int *out_minor, int *out_debug);

extern void
ftl_init(void);

extern void
ftl_end(void);



#ifdef __cplusplus
}
#endif



#endif /* _FTL_H */

