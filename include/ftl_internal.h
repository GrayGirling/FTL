/*
 * Copyright (c) 2005-2009, Solarflare Communications Inc.
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
**  \brief  Furtle (FTL) header for internal extension
**   \date  2008/09
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef _FTL_INTERNAL_H
#define _FTL_INTERNAL_H

#include "ftl.h"

/*          Configuration                      		                     */

/* #define FTL_VAL_HAS_LINENO */



#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define STATIC_INLINE static __inline
#else
#define STATIC_INLINE static inline
#endif

#define FTL_MALLOC malloc
#define FTL_FREE   free


/*          Character Sinks              		                     */


typedef bool /*full*/ putc_fn_t(charsink_t *sink, int ch);

struct charsink_s
{   putc_fn_t *putc;
} /* charsink_t */;


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

typedef int value_print_fn_t(outchar_t *out,
			     const value_t *root, const value_t *value);

typedef const value_t *value_parse_fn_t(const char **ref_line,
                                        const value_t *this_cmd,
                                        parser_state_t *state);

typedef int value_cmp_fn_t(const value_t *v1, const value_t *v2);

struct value_s
{   struct value_s *link;
    struct value_s *heap_next;  /**< next value allocated in heap */
    int heap_version;		    /**< last heap version this was a member of */
#ifdef FTL_VAL_HAS_LINENO
    int lineno;                 /**< used only for debugging */
#endif
    unsigned char local;        /**< if local this is not free for collection */
    unsigned char on_heap;      /**< if set don't delete it with kind->del */
    const value_type_t *kind;	/**< type of this value */
} /* value_t */;

/*! Initialize a value data structure
 *    @param  val      value data structure to be initialized
 *    @param  kind     pointer to value_type_t type value
 *    @param  on_heap  value is not to be deleted explicitly
 */
extern /*internal*/ value_t *
(value_init)(value_t *val, type_t kind, bool on_heap);


/*! To try to isolate uncommitted values (i.e. ones that are valid but which
 *  should not be garbage collected yet, we use the notion of a value being
 *  'local' (local == not reachable from garbage collection root).  Most new
 *  values are allocated 'local' and should be unset as soon as they have
 *  either been used in the garbage-collection-safe tree, or known not to be
 *  required, they can be un-localed.
 *  Ideally every new value will be un-localed before it leaves the scope of the
 *  routine that consumes it.
 *  The hope here is to enable the garbage collector to be run relatively
 *  asynchronously (e.g. when memory is low).
 *  Note that local values will never be collected by the garbage collector if
 *  they are not unlocalled, so that any which are still local at the time of
 *  an exception (which uses longjmp) may never be retrieved.
 */
STATIC_INLINE void
_value_unlocal(const value_t *val, int lineno)
{   /* we will need to discard the const - it applies only to the real content
       of the value - not its garbage collection status
       NOTE: this is potentially dangerous if a const value has been placed in
             read-only memory
    */
    /* if (NULL != val) allow value to be garbage collected as usual */
    if (NULL != val)
    {	/*IGNORE(if (!val->local)
	       fprintf(stderr, "%s: line %5d - value %p "
		       "not local made unlocal\n",
		       codeid(), lineno, val););*/
	((value_t *)val)->local = FALSE;
    }
}

#define value_unlocal(val) _value_unlocal(val, __LINE__)

/* candidate function for value_delete_fn_t for a simple value_t */
extern void
value_delete_alloced(value_t *value);


extern void
value_delete(value_t **ref_val);

/*          Types 					                     */

typedef int type_name_fn_t();


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

extern type_id_t type_id_new(void);

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
type_clone(bool on_heap,
           type_id_t cloned_type_id, const char *name,
           value_print_fn_t *print_fn, value_parse_fn_t *parse_fn,
           value_cmp_fn_t *compare, value_delete_fn_t *delete_fn,
           value_markver_fn_t *mark_version);

extern value_t *
type_new(bool on_heap,
         const char *name,
         value_print_fn_t *print_fn, value_parse_fn_t *parse_fn,
         value_cmp_fn_t *compare_fn,
         value_delete_fn_t *delete_fn,
         value_markver_fn_t *mark_version_fn);

/*          Directories					                     */

/* create a new name-value binding - name and value assumed to have
   permanent values (the name is not in the directory)
*/
typedef bool dir_add_fn_t(dir_t *dir, const value_t *name,
			  const value_t *value);

typedef const value_t **dir_lookup_fn_t(dir_t *dir, const value_t *name);

typedef unsigned dir_count_fn_t(dir_t *dir);

typedef const value_t *dir_get_fn_t(dir_t *dir, const value_t *name);

/* result is from first enumfn to return non NULL */
typedef void *dir_forall_fn_t(dir_t *dir, dir_enum_fn_t *enumfn, void *arg);

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


#ifdef __cplusplus
}
#endif


#endif /* _FTL_INTERNAL_H */
