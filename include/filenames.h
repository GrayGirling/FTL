/*!
 * \file: filename.h
 *
 * \brief functions for handling files in an O/S independent fashion
 *
 * These functions can be used either on Windows or on Linux.
 ******************************************************************************/

#ifndef _FTL_LIBS_FILENAMES_H
#define _FTL_LIBS_FILENAMES_H

#include "ftl.h" /* for bool */

#ifdef WIN32  /**< Set the macros according to the OS */

#define OS_PATH_SEP ';'
#define OS_FS_SEP '\\'
#define OS_FS_CASE_EQ TRUE
#define OS_FS_DIR_HERE "."
#define OS_FS_NOWHERE "nul:"
#define OS_FS_EXT_LIB ".dll"
#define OS_FS_EXT_EXEC ".exe"
#define OS_FS_ABSPATH(file) ((file)[1] == ':')

#else /*! assume Linux */

#define OS_PATH_SEP ':'
#define OS_FS_SEP '/'
#define OS_FS_CASE_EQ FALSE
#define OS_FS_DIR_HERE "."
#define OS_FS_NOWHERE "/dev/null"
#define OS_FS_EXT_LIB ".so"
#define OS_FS_EXT_EXEC ""
#define OS_FS_ABSPATH(file) ((file)[0] == '/')

#endif


/***********************/
/* File System Support */
/***********************/

/*! Find a file on a PATH and return the name of a readable file
 *
 *  \param path       String of directory names separated by \c path_delim chars
 *  \param sought     Name of file to find in one of path directories
 *  \param path_delim Character separating directories in \c path
 *  \param file_sep   Character use to separate directory from file name in f/s
 *  \param name_buf   Buffer where new file name will be written
 *  \param bufsize    Number of characters available in \c name_buf
 *
 *  \return TRUE iff \c sought was found on a \c path directory as a readable
 *          file
 *
 *  Path directories that are too long for \c name_buf are ignored.
 *
 *  When the function returns TRUE \c name_buf will contain the name of a file
 *  which has been opened once for reading and then closed.
 */
extern bool
find_file_on_path(const char *path, const char *sought, char path_delim,
                  char file_sep, char *name_buf, size_t bufsize);

/*! Find a file on a operating system formatted PATH and return the name of a
 *  readable file
 *
 *  \param path       String of directory names separated by \c path_delim chars
 *  \param sought     Name of file to find in one of path directories
 *  \param name_buf   Buffer where new file name will be written
 *  \param bufsize    Number of characters available in \c name_buf
 *
 *  \return TRUE iff \c sought was found on a \c path directory as a readable
            file
 *
 *  On Windows the path is expected to contain directories separated by
 *  semicolons, on Linux the path is split by colons.  The separator between
 *  directory and file names is '/' on Linux and '\' on Windows.
 *
 *  Path directories that are too long for \c name_buf are ignored.
 *
 *  When the function returns TRUE \c name_buf will contain the name of a file
 *  which has been opened once for reading and then closed.
 */
extern bool
find_file_on_ospath(const char *path, const char *sought,
                    char *name_buf, size_t bufsize);

/*! Find the full name of the current executable.
 *
 *  \param buf        Area of memory to use (e.g. to store file name in)
 *  \param size       Size of the above area
 *
 *  \return NULL iff the name couldn't be found, else a pointer to the name
 */
extern char *file_executable(char* buf, size_t size);

/*! Find the full name of a file in the same directory as the executable
 *
 *  \param buf           Area of memory to use (e.g. to store file name in)
 *  \param size          Size of the above area
 *  \param sibling_name  Name of the file in the executable directory
 *
 *  \return NULL iff the name couldn't be found, else a pointer to the name
 */
extern char *
file_executable_sibling(char* buf, size_t size, const char *sibling_name);

/*! Type of function called for each entry in a directory
 *
 * \param enum_arg   [in] Argument to be provided to \c fn
 * \param entry      [in] Leaf name of file or subdirectory
 * \param subdir     [in] Concatenated name of file or subdirectory
 * \param examinable [in] TRUE iff we can tell what kind of object it is
 * \param is_dir     [in] TRUE iff examinable and it is a subdirectory
 *
 * Both 'entry' and 'subdir' are held in temporary storage.  If they are to
 * persist after the invocation of this function a copy must be taken.
 *
 * \return TRUE iff 
 */
typedef bool
fs_enum_fn_t(void *enum_arg, const char *entry, const char *subdir,
             bool examinable, bool is_dir);

/*!
 * This function can be used to discover the directories within a specified
 * folder. 
 *
 * \param directory [in] Pointer to the directory whose directories will
                         be returned
 * \param fn        [in] Function to be invoked for each entry in the directory
 * \param enum_arg  [in] Argument to be provided to \c fn
 *
 * \return Returns TRUE on success, FALSE on failure
 */
extern bool
fs_enum_dir(const char *directory, fs_enum_fn_t *fn, void *enum_arg);

#endif /* _FTL_LIBS_FILENAMES_H */
