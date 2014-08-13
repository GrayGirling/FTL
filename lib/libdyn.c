/*! @file: functions for handling calls into dynamic libraries */



/******************************************************************************
 *
 *          Headers
 *
 *****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "dynlib.h"
#include "filename.h"


/******************************************************************************
 *
 *          O/S Error Reporting Abstraction
 *
 *****************************************************************************/


#ifdef WIN32
extern unsigned get_errno(void)
{
   return (unsigned)GetLastError();
}
extern const char *strerror_alloc(unsigned err)
{
   // Retrieve the system error message for the last-error code
   // Note: LPTSTR is sometimes a pointer to wide characters and sometimes
   //       a pointer to normal characters depending on whether UNICODE is
   //       defined.

   LPTSTR lpMsgBuf;
   DWORD rc = FormatMessage(
                 FORMAT_MESSAGE_ALLOCATE_BUFFER |
                 FORMAT_MESSAGE_FROM_SYSTEM |
                 FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL,
                 err,
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 (LPTSTR) &lpMsgBuf, //This is what the doc. says to do!
                 0, NULL );

   return (const char *)(rc==0? TEXT("(unknown)"): lpMsgBuf);
}

extern void strerror_free(const char * errmsg)
{  LocalFree((LPTSTR)errmsg);
}

#else

extern unsigned get_errno(void)
{  return errno;
}
extern const char *strerror_alloc(unsigned err)
{  return strerror(err);
}
   
extern void strerror_free(const char *errmsg)
{  return;
}

#endif /*WIN32*/


/******************************************************************************
 *
 *          O/S Shared Library Abstraction
 *
 *****************************************************************************/


#ifdef WIN32

typedef HINSTANCE dll_t;

/* Basic library loading and lookup */

extern dll_t load_library(const char *path)
{  return LoadLibraryEx(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
}
extern int /*rc*/free_library(dll_t h)
{  if (FreeLibrary(h))
      return 0;
   else
      return 1;
}
extern libfn_t lib_sym(dll_t lib, const char *sym)
{   return (libfn_t)GetProcAddress(lib, sym);
}

/* Library error handling */

extern const char *liberror_alloc(unsigned err)
{   return strerror_alloc((DWORD)err);
}
extern void liberror_free(const char *errmsg)
{   strerror_free(errmsg);
}

#else

/* assume Linux */

#include <dlfcn.h>

#define __stdcall

typedef void *dll_t;

/* Basic library loading and lookup */

extern dll_t load_library(const char *path)
{  //return dlopen(path, RTLD_NOW);
   return dlopen(path,RTLD_LAZY);
}
extern int/*rc*/ free_library(dll_t lib)
{  return dlclose(lib);
}
extern libfn_t lib_sym(dll_t lib, const char *sym)
{  return dlsym(lib, sym);
}

/* Library error handling */

extern const char *liberror_alloc(unsigned err)
{   return dlerror(); /* clears the last error too */
}
extern void liberror_free(const char *errmsg)
{   return;
}
#endif



/******************************************************************************
 *
 *          Shared Library Locking
 *
 *****************************************************************************/


#ifdef WIN32

extern int /*bool*/ liblock_init(liblock_t *liblock, const char *libname)
{  liblock->name = libname;
   liblock->lock_mutex = CreateMutex(NULL, FALSE, libname);
   return (NULL != liblock->lock_mutex);
}

extern void liblock_lock(liblock_t *liblock)
{
   WaitForSingleObject(liblock->lock_mutex, INFINITE);
   //while (dll_lock) ;
}

extern void liblock_unlock(liblock_t *liblock)
{
   ReleaseMutex(liblock->lock_mutex);
}

#else /* assume Linux */

/* no locking on Linux (why not?) */

extern int /*bool*/ liblock_init(liblock_t *liblock, const char *libname)
{  return 1/*TRUE*/;
}

extern void liblock_lock(liblock_t *liblock)
{  return;
}

extern void liblock_unlock(liblock_t *liblock)
{  return;
}

#endif



/******************************************************************************
 *
 *          Cached Shared Library Function Addresses
 *
 *****************************************************************************/





/* The cache list starts with a non-null value so that all following
   cached address additions will have a non-NULL prev_unknown field once
   they have been added to the list (and NULL otherwise).
*/
static libfn_ref_t libfn_sentinal = LIBFN_REF_UNCACHED;


/*! Find library on path and initialize cache with its full name
 *  @return TRUE iff the library was found on the path
 */
extern int /*bool*/
libfn_cache_init(libfn_cache_t *cache, const char *path, const char *libleaf)
{
   int /*bool*/ ok = 1/*TRUE*/;
   
   cache->lib = DLL_NONE;
   cache->known_dll_fns = &libfn_sentinal;
   
   if (libleaf != NULL) {
      size_t leafsize = strlen(libleaf);
      char leaf_ext[64];

      if (leafsize+strlen(OS_FS_EXT_LIB)+1 > sizeof(leaf_ext))
         ok = 0/*FALSE*/;
      else if (path == NULL)
         sprintf(&cache->path[0], "%s%s", libleaf, OS_FS_EXT_LIB);
      else {
         sprintf(&leaf_ext[0], "%s%s", libleaf, OS_FS_EXT_LIB);
         ok = find_file_on_ospath(path, &leaf_ext[0],
                                  &cache->path[0], sizeof(cache->path));
      }
   } else
      cache->path[0] = '\0';

   return ok;
}


extern dll_t libfn_cache_load(libfn_cache_t *cache)
{  dll_t lib = load_library(&cache->path[0]);
   cache->lib = lib;
   return lib;
}
   

extern int /*rc*/ libfn_cache_unload(libfn_cache_t *cache)
{  int rc = 0;
   if (cache->lib != DLL_NONE)
      rc = free_library(cache->lib);
   if (rc == 0)
      cache->lib = DLL_NONE;
   return rc;
}
   

/*! read cached version of library symbol
 *
 * Uses a cached version of a value returned by lib_sym if it has
 * been found, otherwise use lib_sym to find it and caches the result.
 *
 * ref_cached must be a long-lived data area (e.g. static) because it is used
 * to form a permanent list of references that we can refer to when we want
 * to invalidate all the functions, for example.
 *
 *  \return value of DLL procedure named by c
 */
extern libfn_t libfn_cache_get(libfn_cache_t *cache, const char *fn_name,
                               libfn_ref_t *ref_cached)
{  if (ref_cached->prev_known == NULL) {
      /* add this (static) reference to the list of known DLL functions */
      ref_cached->prev_known = cache->known_dll_fns;
      cache->known_dll_fns = ref_cached;
   }
   if (ref_cached->fn == NULL) {
      ref_cached->fn = lib_sym(cache->lib, fn_name);
   }
   return ref_cached->fn;
}

/*! forget the details of all the symbols looked up from the given library */
extern void libfn_cache_forget(libfn_cache_t *cache)
{  libfn_ref_t *fnref = cache->known_dll_fns;
   while (NULL != fnref)
   {   fnref->fn = NULL;
       fnref = fnref->prev_known;
   }
}
