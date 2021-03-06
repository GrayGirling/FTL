/*
 * Copyright (c) 2005-2009, Solarflare Communications Inc.
 * Copyright (c) 2005-2021, Gray Girling
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
**  \brief  Furtle (FTL) header for internal extension
**   \date  2008/09
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef _FTL_INTERNAL_H
#define _FTL_INTERNAL_H

#include "ftl_api.h"

/*          Configuration                      		                     */

/* #define FTL_VAL_HAS_LINENO */



#ifdef __cplusplus
extern "C" {
#endif


#define FTL_MALLOC malloc
#define FTL_FREE   free


/*          Character Sinks              		                     */


typedef bool /*full*/ putc_fn_t(charsink_t *sink, int ch);
typedef bool /*full*/ flush_fn_t(charsink_t *sink);
typedef bool /*writeable*/ ready_fn_t(charsink_t *sink);

struct charsink_s
{   putc_fn_t *putc;    /* output a character */
    flush_fn_t *flush;  /* force data end to end */
    ready_fn_t *ready;  /* false if next write will cause I/O delay  */
} /* charsink_t */;


extern charsink_t *
charsink_init(charsink_t *sink, putc_fn_t *putc, flush_fn_t *flush,
              ready_fn_t *ready);

    
typedef struct charsink_string_s
{   charsink_t sink;
    char *charvec;
    size_t n;
    size_t maxn;
} charsink_string_t;

extern charsink_t *
charsink_string_init(charsink_string_t *charbuf);

extern void
charsink_string_close(charsink_t *sink);


/*          Values 					                     */

typedef void value_delete_fn_t(value_t *value);

/* set the heap version of that values dependent on this value belong to */
typedef void value_markver_fn_t(const value_t *value, int version);

typedef int value_print_fn_t(parser_state_t *state, outchar_t *out,
                             const value_t *root, const value_t *value, 
                             bool detailed);
/*< Note: legacy version of this function did not include the state
 */

/*! Parse a nul-terminated string into a value
 */
typedef const value_t *value_parse_fn_t(const char **ref_line, 
                                        const value_t *this_cmd,
                                        parser_state_t *state);

typedef int value_cmp_fn_t(const value_t *v1, const value_t *v2);

struct value_s
{   /* local chain for garbage collection */
    struct value_s *loc_next;   /**< ref to next value on chain */
    struct value_s **loc_last_ref; /**< ref to last value's "loc_next" link */
    /* main value fields */
    struct value_s *link;       /**< multipurpose "next" link */
    struct value_s *heap_next;  /**< next value allocated in heap */
    int heap_version;	        /**< last heap version this was a member of */
#ifdef FTL_VAL_HAS_LINENO
    int lineno;                 /**< used only for debugging */
#endif
    const value_type_t *kind;	/**< type of this value */
    unsigned char on_heap;      /**< if set don't delete it with kind->del */
} /* value_t */;

/*! Initialize a value data structure
 *    @param  val      value data structure to be initialized
 *    @param  kind     pointer to value_type_t type value
 *    @param  on_heap  value is not to be deleted explicitly
 */
extern /*internal*/ value_t *
(value_init)(value_t *val, type_t kind, bool on_heap);

/*! Allocate storage for a value_t, which is initially placed on the
 *  thread's local values list (so it doesn't get garbage collected)
 *  Note: these values should, once initialized be attached to other
 *  ultimately non-local variables using HOME()
 */
extern value_t *value_malloc_newl(parser_state_t *state, size_t size);

    
/* candidate function for value_delete_fn_t for a simple value_t */
extern void
value_delete_alloced(value_t *value);


extern void
value_delete(value_t **ref_val);

/*          Types 					                     */

typedef int type_name_fn_t(void);


struct value_type_s
{
    value_t val;
    type_id_t id;               /**< of FTL parent type */
    const char *name;           /**< of FTL parent type */
    value_print_fn_t *print;    /**< convert value into string */
    value_parse_fn_t *parse;    /**< convert string into value */
    value_delete_fn_t *del;     /**< delete this + uniquely owned parts */
    value_markver_fn_t *mark_version; /**< for garbage collection */
    value_cmp_fn_t *compare;    /**< used when this type is first to compare */
} /* value_type_t */;


extern /*internal*/ value_t *
type_init(value_type_t *kind, bool on_heap,
          type_id_t type_id, const char *name,
          value_print_fn_t *val_print_fn, value_parse_fn_t *val_parse_fn,
          value_cmp_fn_t *val_compare_fn,
          value_delete_fn_t *val_delete_fn,
          value_markver_fn_t *val_mark_version_fn);

/*! determine whether other values are referred to in values of this type
 */
extern bool type_values_simple(type_t kind);

extern value_t *
type_state_clone(parser_state_t *state, bool on_heap,
                 type_id_t cloned_type_id, const char *name,
                 value_print_fn_t *print_fn, value_parse_fn_t *parse_fn,
                 value_cmp_fn_t *compare_fn, value_delete_fn_t *delete_fn,
                 value_markver_fn_t *mark_version);

#define type_clone(on_heap, clone_type_id, name, print_fn, parse_fn,    \
                   compare_fn, delete_fn, mark_version) \
    type_state_clone(state, on_heap, clone_type_id, name, \
                     print_fn, parse_fn, compare_fn, delete_fn, mark_version) 


extern value_t *
type_state_new(parser_state_t *state, bool on_heap,
               const char *name,
               value_print_fn_t *print_fn, value_parse_fn_t *parse_fn,
               value_cmp_fn_t *compare_fn,
               value_delete_fn_t *delete_fn,
               value_markver_fn_t *mark_version_fn);

#define type_new(on_heap, name, print_fn, parse_fn, compare_fn, \
                 delete_fn, mark_version)                   \
    type_state_clone(state, on_heap, name, \
                     print_fn, parse_fn, compare_fn, delete_fn, mark_version) 



/*          Directories					                     */

/* create a new name-value binding - name and value assumed to have
   permanent values (the name is not in the directory)
*/
typedef bool dir_add_fn_t(dir_t *dir, parser_state_t *state,
                          const value_t *name, const value_t *value);

typedef const value_t **dir_lookup_fn_t(dir_t *dir, const value_t *name);

typedef unsigned dir_count_fn_t(dir_t *dir, parser_state_t *state);

/*! Fetch a value from a directory given its name
 *  Typically the result will not be a local value, but there are directories
 *  (e.g. dynamic ones) where they will be.  
 */
typedef const value_t * /*local*/dir_get_fn_t(dir_t *dir, const value_t *name);

/* result is from first enumfn to return non NULL
 * Note: legacy version of this function did not include the state
 */
typedef void *dir_forall_fn_t(dir_t *dir, parser_state_t *state, 
                              dir_enum_fn_t *enumfn, void *arg);

struct dir_s
{   value_t value;           /* directory used as a value */
    bool env_end;            /* true when dir is not to be looked beyond */
    dir_add_fn_t    *add;    /* add a new name-value pair */
    dir_lookup_fn_t *lookup; /* find address of value */
    dir_get_fn_t    *get   ; /* find corresponding value */
    dir_forall_fn_t *forall; /* ennumerate values */
    dir_count_fn_t  *count;  /* quick count of the number of values */
} /* dir_t */;

#define dir_value(dir) (&(dir)->value)

extern /*internal*/ value_t *
dir_init(dir_t *dir, type_t dir_subtype,
         dir_add_fn_t *add_fn, dir_lookup_fn_t *lookup_fn,
	 dir_get_fn_t *get_fn, dir_forall_fn_t *forall_fn,
         bool on_heap);

/*          Memory					                     */

typedef struct value_mem_s value_mem_t;

typedef enum {
    mem_can_cache = 1, /* any access this program controls values uniquely   */
    mem_can_write,     /* writeable */
    mem_can_read,      /* readable - without causing side effects */
    mem_can_get        /* readable - even if side effect caused */
} mem_attr_t;

/*! bit map of attibutes - set of (1<<mem_attr_t) */
typedef unsigned char mem_attr_map_t;



/*! Read a block of memory
 *  @param mem             memory object to read
 *  @param byte_index      index of first byte to read
 *  @param buf             buffer where result should be stored
 *  @param buflen          number of bytes to read (and size of buffer)
 *  @param force_volatile  read even if side effect will be caused
 */
typedef bool /*ok*/
mem_read_fn_t(const value_mem_t *mem, size_t byte_index,
              void *buf, size_t buflen, bool force_volatile);


/*! Write a block of memory
 *  @param mem             memory object to read
 *  @param byte_index      index of first byte to read
 *  @param buf             buffer containing bytes to be written
 *  @param buflen          number of bytes to write (and size of buffer)
 */
typedef bool /*ok*/
mem_write_fn_t(const value_mem_t *mem, size_t byte_index,
               const void *buf, size_t buflen);


/*! Return size of next area of memory with given attributes
 *
 *  @param mem             memory object to read
 *  @param byte_index      index of first byte proposed
 *  @param unable          size area without the given ability (not with)
 *  @param ability         bitmap of memory attributes required
 *
 *  Returns a number of bytes that have the ability requested starting at the
 *  given byte index.  If the proposed byte does not have the requested
 *  ability return zero.
 *
 *  This function may return a number that is smaller than the whole following
 *  consecutive area with the given ability, but must not return zero if bytes
 *  with that attribute are available.  (This may result in some inefficiency
 *  however.)
 *
 *  For a given unable/ability setting the function must imply a block of the
 *  same size for all other byte indeces in that block.
 *
 *  Smaller-than-possible values will make larger accesses invalid.
 */
typedef size_t /*len*/
mem_len_able_fn_t(const value_mem_t *mem, size_t byte_index,
                  bool unable, mem_attr_map_t ability);


/*! Return base of an area of memory with given attributes ending at an index
 *
 *  @param mem             memory object to read
 *  @param byte_index      index of first byte proposed
 *  @param unable          base of area without the given ability (not with)
 *  @param ability         bitmap of memory attributes required
 *
 *  Returns the byte index of the start of an area incorporating the byte_index
 *  all the bytes of which have the ability requested.  If the proposed byte
 *  does not have the requested ability return zero.
 *
 *  This function may return a number that is smaller than the whole previous
 *  consecutive area with the given ability.  (This may result in some
 *  inefficiency however.)
 *
 *  For a given unable/ability setting the function must imply a block of the
 *  same size for all other byte indeces in that block.
 */
typedef size_t /*base*/
mem_base_able_fn_t(const value_mem_t *mem, size_t byte_index,
                   bool unable, mem_attr_map_t ability);



struct value_mem_s
{   value_t value;           /* memory used as a value */
    mem_read_fn_t *read;
    mem_write_fn_t *write;
    mem_len_able_fn_t *len_able;
    mem_base_able_fn_t *base_able;
} /* value_mem_t */;

#define value_mem_value(_mem) (&(_mem)->value)


extern value_t *
value_mem_init(value_mem_t *mem, type_t mem_subtype,
               mem_read_fn_t *read_fn, mem_write_fn_t *write_fn,
               mem_len_able_fn_t *len_able_fn, mem_len_able_fn_t *base_able_fn,
               bool on_heap);


/*          Scanning					                     */


#if 0
typedef bool ftl_scan_prefix_fn(const char **ref_line);

/*! Generic ftl function that just updates the parse state provided at argument
 *  'argstart'.
 *  The parsing itself is handled by the function 'fnparse' which removes a
 *  prefix from the string at <arg>.0.
 *
 *  The function will return
 *        &value_null  - if the argument has the wrong type
 *        value_false  - if fnparse returns FALSE
 *        value_true   - if fnparse returns TRUE
 *
 * Deprecated: Legacy use only - don't use in new code - use genfn_scanw_noret
 */
extern const value_t *
genfn_scan_noret(const value_t *this_fn, parser_state_t *state,
                 int argstart, ftl_scan_prefix_fn *fnparse);


typedef const value_t *
ftl_scan_value_fn(const char **ref_line, parser_state_t *state, void *arg);

/*! Generic FTL function that updates the parse state provided at argument
 *  'argstart' and generates an FTL value from the prefix which
 *  is returned via 
 *
 *  .... <pobj> ...
 *       |
 *       argstart
 *
 *  Normally 'argstart' is the last argument to patch other parse.<fn>s
 *
 *  The parsing itself is handled by the function 'fnparse'
 *          (*fnparse)(&line, state, arg)
 *  which can not assume that the line is nul-terminated and returns the
 *  object (value_t) parsed.
 *
  *  This returns
 *        &value_null        - if the argument has the wrong type
 *        the fnparse result - otherwise
 *
 */
extern const value_t *
genfn_scanw(const value_t *this_fn, parser_state_t *state,
            int argstart, ftl_scanw_value_fn *fnparse, void *arg);


/*! Generic FTL function that updates the parse state provided at argument
 *  'argstart'+1 and generates an FTL value from the prefix which
 *  is returned via a callback closure provided at argument 'argstart'+0.
 *
 *  .... <@setres> <pobj> ...
 *       |
 *       argstart
 *
 *  Normally 'argstart'+1 is the last argument to patch other parse.<fn>s
 *
 *  The parsing itself is handled by the function 'fnparse'
 *          (*fnparse)(&line, state, arg)
 *  which can assume that the line is nul-terminated (although this is less
 *  efficient) and returns the object (value_t) parsed.
 */
extern const value_t *
genfn_scan(const value_t *this_fn, parser_state_t *state,
           int argstart, ftl_scan_value_fn *fnparse, void *arg);
#endif

    
typedef const value_t *
ftl_scanw_value_fn(const char **ref_line, const char *lineend,
                   parser_state_t *state, void *arg);

/*! Generic FTL function that updates the parse state provided at argument
 *  'argstart'+1 and generates an FTL value from the prefix which
 *  is returned via a callback closure provided at argument 'argstart'+0.
 *
 *  .... <@setres> <pobj> ...
 *       |
 *       argstart
 *
 *  Normally 'argstart'+1 is the last argument to patch other parse.<fn>s
 *
 *  The parsing itself is handled by the function 'fnparse'
 *          (*fnparse)(&line, linelen, state, arg)
 *  which must not assume that the line is nul-terminated and will return the
 *  object (value_t) that has been parsed.
 *
 *  The function will return
 *        &value_null  - if the arguments have the wrong type
 *        value_false  - if fnparse returns NULL
 *        value_true   - if fnparse returns non-NULL
 *
 *  If value_true is returned then the parsed value will be provided as an
 *  argument to the setres closure which will then be invoked
 */
extern const value_t *
genfn_scanw_cb(const value_t *this_fn, parser_state_t *state,
               int argstart, ftl_scanw_value_fn *fnparse, void *arg);

    
/*          Printing					                     */


enum
{   ff_left = 1,    /* (-) left justify in width - otherwise right */
    ff_zero,        /* (0) pad with leading zeros */
    ff_sign,        /* ( ) always allow place for a sign */
    ff_posv,        /* (+) use positive sign as well as a negative one only */
    ff_alt          /* (#) "alternative" format */
};

typedef int fprint_flags_t; /* bit set of enum values */
    
typedef const value_t *
fn_fmt_fn_t(char *buf, size_t buflen, fprint_flags_t flags,
            int precision, const value_t *argval, parser_state_t *state);

extern const value_t *
fn_fmt_generic(const value_t *this_fn, parser_state_t *state,
               fn_fmt_fn_t genformat);
    
extern void
printf_addformat(type_t for_type, const char *fmtchar,
                 const char *help, func_fn_t *exec);



/*          Pointer Checks                                                  */


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

#define VALVALID(_val) \
    (PTRVALID(_val) && \
     ( (_val)->on_heap == TRUE || (_val)->on_heap == FALSE ) && \
       PTRVALID((_val)->kind) )



    
#ifdef __cplusplus
}
#endif


#endif /* _FTL_INTERNAL_H */
