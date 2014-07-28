/*! @file: functions for handling calls into dynamic libraries
 *
 * These functions can be used either on Windows or on Linux.
 * On *nix dynamic libraries are called shared objects (with suffix .so)
 * On windows dynamic libraries are called Dynamically Loaded Libraries (with
 *   suffix .dll)
 */

#ifndef _DRIVERS_LIBDYNLIB_H
#define _DRIVERS_LIBDYNLIB_H

#ifdef WIN32

#include "windows.h"
typedef HINSTANCE dll_t;
#define DLL_NONE 0
#define ERRNL    /* characters needed to follow error with a new line */

#else /* assume Linux */

#include <dlfcn.h>
typedef void *dll_t;
#define DLL_NONE NULL
#define ERRNL "\n" /* characters needed to follow error with a new line */

#endif

typedef void *libfn_t;

/************************************/
/* Basic library loading and lookup */
/************************************/

/*! Load the library with the given file name
 *  \return DLL_NONE iff the library failed to load
 *  On failure \c liberror_alloc() can be used to return an error string
 */
extern dll_t load_library(const char *filepath);

/*! Unload the library */
extern int /*rc*/ free_library(dll_t lib);

/*! Find the value of a symbol exported by a loaded library */
extern libfn_t lib_sym(dll_t lib, const char *sym);

/*******************/
/* Library Locking */
/*******************/

#ifdef WIN32
typedef struct liblock_s {
   const char *name;
   HANDLE lock_mutex;
} liblock_t;
#define liblock_name(_liblock) ((_liblock)->name)
#else
/* TODO: No locking on Linux - why not? */
typedef struct liblock_s {
   const char *name;
} liblock_t;
#define liblock_name(_liblock) ((_liblock)->name)
#endif

/*! Initialize a new library lock */
extern int /* bool*/ liblock_init(liblock_t *liblock, const char *libname);

/*! Lock the library for exclusive access */
extern void liblock_lock(liblock_t *liblock);

/*! Unlock the library to allow generic access */
extern void liblock_unlock(liblock_t *liblock);


/**************************/
/* Library error handling */
/**************************/

/*! Read the error left by the last failed operation */
extern unsigned get_errno(void);

/*! Provide an allocated string representing the failure indicated by err
 *  If the result is non-NULL it should be freed using \c strerror_free
 */
extern const char *strerror_alloc(unsigned err);

/*! Free a string allocated by \c strerror_alloc */
extern void strerror_free(const char *errmsg);

/*! Provide an allocated string representing the library load/symbol lookup
 *  failure indicated by err (or by other global state)
 *  If the result is non-NULL it should be freed using \c strerror_free
 */
extern const char *liberror_alloc(unsigned err);

/*! Free a string allocated by \c liberror_alloc */
extern void liberror_free(const char *errmsg);

/**********************************/
/* Cached library symbol handling */
/**********************************/

/* These functions minimize the calls to lib_sym by caching previously
   returned values.
*/

/* Treat the following as an opaque data type */
typedef struct libfn_ref_s {
   struct libfn_ref_s *prev_known;      /* fns previously known */
   libfn_t fn;                          /* DLL function address */
} libfn_ref_t;

#define LIBFN_REF_UNCACHED {/*prev_known*/NULL, /*fn*/NULL}

/* Treat the following as an opaque data type */
typedef struct {
   dll_t lib;                         /* loaded library */
   struct libfn_ref_s *known_dll_fns; /* list of cached functions */
   char path[256];                    /* name of library file */
} libfn_cache_t;


#define libfn_cache_libname(_cache) (&(_cache)->path[0])
#define libfn_cache_lib(_cache) ((_cache)->lib)

/*! Set up a libfn_cache_t for a library in path with name \c libleaf
 *
 * On Windows the path should be a list of Windows directory names
 * delimited by ';'
 * On *nux the path should be a list of *nix directory names delimited by ':'
 *
 * The leaf name will have the extension appropriate to the operating system
 * appended, so should not be included in the text of \c libleaf.
 */
extern int /*bool*/
libfn_cache_init(libfn_cache_t *cache, const char *path, const char *libleaf);

/*! Attempt to load the library
 *  \return DLL_NONE iff the library failed to load
 */
extern dll_t
libfn_cache_load(libfn_cache_t *cache);

/*! Attempt to unload the library
 *  \return non-zero iff the library failed to unload
 */
extern int /*rc*/
libfn_cache_unload(libfn_cache_t *cache);

/*! Find the cached value of a symbol in the cache
 *  The value looked up is cached in the libfn_ref_t data structure.
 *  Typically this is a per-symbol static data structure
 */
extern libfn_t
libfn_cache_get(libfn_cache_t *cache, const char *fn_name,
                libfn_ref_t *ref_cached);

/*! Flush all the cached symbols in the cache */
extern void
libfn_cache_forget(libfn_cache_t *cache);

#endif /* _DRIVERS_LIBDYNLIB_H */
