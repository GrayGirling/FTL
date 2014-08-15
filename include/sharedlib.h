/*! @file sharedlib.h
 *  @brief
 *   Microsoft underscore adjustment for shared library functions The _SHARED
 *   macro should be used to define any function that is to be available in a
 *   shared library.
 *
 *   This macro need only be used by the declaration of each function in the
 *   header, not the function's definition in the main body of code.
 */

#ifndef _DRIVER_LIBS_SHAREDLIB_H
#define _DRIVER_LIBS_SHAREDLIB_H

#ifndef _PRE_
#ifdef _MSC_VER
#include <windows.h>
#define _PRE_       __declspec(dllexport)
#define _POST_      _cdecl
#else
#define _PRE_
#define _POST_
#endif

#endif

#define _SHARED(return_type) _PRE_ return_type _POST_

#endif /* _DRIVER_LIBS_SHAREDLIB_H */
