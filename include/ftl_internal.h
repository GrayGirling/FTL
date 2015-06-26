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
**  \brief  Furtle (FTL) header for internal extension
**   \date  2008/09
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef _FTL_INTERNAL_H
#define _FTL_INTERNAL_H

#include "ftl.h"

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

/*          Values 					                     */
  
typedef void value_delete_fn_t(value_t *value);

/* set the heap version of that values dependent on this value belong to */
typedef void value_markver_fn_t(const value_t *value, int version);

typedef int value_print_fn_t(outchar_t *out,
			     const value_t *root, const value_t *value);

typedef int value_cmp_fn_t(const value_t *v1, const value_t *v2);

struct value_s
{   struct value_s *link;
    struct value_s *heap_next;  /*< next value allocated in heap */
    value_print_fn_t *print;
    value_delete_fn_t *del;
    value_markver_fn_t *mark_version;
    value_cmp_fn_t *compare;
    int heap_version;		/*< last heap version this was a member of */
    type_t kind;		/*< type of this value */
    unsigned char local;        /*< if local this is not free for collection */
    int lineno;                 /*< used only for debugging */
} /* value_t */;

extern /*internal*/ value_t *
(value_init)(value_t *val, type_t kind, value_print_fn_t *print_fn,
	     value_cmp_fn_t *compare_fn, value_delete_fn_t *delete_fn,
	     value_markver_fn_t *mark_version_fn);

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

extern void
value_delete(value_t **ref_val);
  
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
dir_init(dir_t *dir, dir_add_fn_t *add_fn, dir_lookup_fn_t *lookup_fn,
	 dir_get_fn_t *get_fn, dir_forall_fn_t *forall_fn,
	 value_print_fn_t *print_fn, value_delete_fn_t *delete_fn,
	 value_markver_fn_t *mark_fn);
  
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

#define value_mem_value(mem) (&(mem)->value)


extern value_t *
value_mem_init(value_mem_t *mem, value_delete_fn_t *delete_fn,
   	       value_markver_fn_t *mark_version_fn,
               mem_read_fn_t *read_fn, mem_write_fn_t *write_fn,
               mem_len_able_fn_t *len_able_fn, mem_len_able_fn_t *base_able_fn);


#ifdef __cplusplus
}
#endif


#endif /* _FTL_INTERNAL_H */
