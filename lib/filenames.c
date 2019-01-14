/*! @file: functions for handling files in an O/S independent fashion
 */


/******************************************************************************
 *
 *          Headers
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "ftl.h"        /* for codeid() and bool */
#include "filenames.h"


/******************************************************************************
 *
 *          Debugging
 *
 *****************************************************************************/

#define DO(_x) _x
#define OMIT(_x)

#define error_printf printf


/******************************************************************************
 *
 *          File System Support
 *
 *****************************************************************************/


/*! Find a file on a PATH and return the name of a readable file
 */
extern bool
find_file_on_path(const char *path, const char *sought, char path_delim,
                  char file_sep, char *name_buf, size_t bufsize)
{  bool found = FALSE;
   size_t soughtlen = strlen(sought);
   const char *ppos = path;

   while (ppos != NULL && !found) {
      size_t pathelsize;
      char *nextppos = strchr(ppos, path_delim);

      if (nextppos == NULL)
         pathelsize = strlen(ppos);
      else
         pathelsize = nextppos - ppos;

      if (soughtlen+1+pathelsize+1 <= bufsize) {
         char *leafpos;
         FILE *openfile;

         memcpy(name_buf, ppos, pathelsize);
         if (pathelsize != 0) {
            name_buf[pathelsize] = file_sep;
            leafpos = &name_buf[pathelsize+1];
         } else
            leafpos = &name_buf[0];
         memcpy(leafpos, sought, soughtlen);
         leafpos[soughtlen] = '\0';

         openfile = fopen(&name_buf[0], "r");
         found = (openfile != NULL);
         /*printf("%s: could%s open '%s'\n", found? "":"n't",
                  codid(), &name_buf[0]);*/
         if (found)
            fclose(openfile);

      } /* else just omit this path element */

      if (nextppos != NULL)
         ppos = nextppos+1;
      else
         ppos = NULL;
   }

   return found;
}

/*! Find a file on a operating system formatted PATH and return the name of a
 *  readable file
 */
extern bool
find_file_on_ospath(const char *path, const char *sought,
                    char *name_buf, size_t bufsize)
{  return find_file_on_path(path, sought, OS_PATH_SEP, OS_FS_SEP,
                            name_buf, bufsize);
}


#ifdef WIN32

#include <windows.h>

extern char *file_executable(char* buf, size_t size)
{
    char path[MAX_PATH + 1];
    DWORD len = GetModuleFileName(NULL, path, MAX_PATH + 1);

    if (len != 0 && len+1 < size) {
       memcpy(buf, &path[0], len);
       buf[len] = '\0';
       return buf;
    } else
       return NULL;
}

#elif defined(__APPLE__)

#include <mach-o/dyld.h>
 
/* another option here might be to use
 *    pid = getpid();
 *    ret = proc_pidpath (pid, pathbuf, sizeof(pathbuf));
 */

/* Warning: will return name of executable even if it is a symbolic link
 *          to something elsewhere
 */
extern char *file_executable(char* buf, size_t size)
{
    /* len must start as the size of the buffer and is updaed with the
     * size written into the buffer
     */
    int rc = _NSGetExecutablePath(buf, (uint32_t*)&size);
    if (rc == 0)
        return &buf[0];
    else
        return NULL;
}

#else

/* Linux - also works under Cygwin */

#include <sys/types.h>
#include <unistd.h>

extern char *file_executable(char* buf, size_t size)
{
   char linkname[64];    /* to hold /proc/<pid>/exe */
   pid_t pid = getpid(); /* our process ID */
   int len = snprintf(linkname, sizeof(linkname), "/proc/%i/exe", pid);
   char *file = NULL;

   if (len > 0) {
      len = readlink(linkname, buf, size); /* read symbolic link */

      /* In case of an error, leave the handling up to the caller */
      if (len >= 0) {
          if (len >= size)
             errno = ERANGE;
          else {
             buf[len] = '\0';
             file = buf;
          }
      }
   }
   return file;
}

#endif


extern char *
file_executable_sibling(char* buf, size_t size, const char *sibling_name)
{
   char *me = file_executable(buf, size);
   char *dir_end = strrchr(me, OS_FS_SEP);
   size_t ix = 0;
   size_t siblen = strlen(sibling_name);
   char *file = NULL;

   if (dir_end != NULL)
      ix = dir_end - me + 1;

   /*printf("ix %d siblen %d size %d '%s'\n", ix, siblen, size, &buf[0]);*/

   if ((ix + siblen + 1) < size) {
      memcpy(&buf[ix], sibling_name, siblen);
      buf[ix+siblen] = '\0';
      file = &buf[0];
   }

   return file;
}

#ifdef WIN32

#include <tchar.h>


extern bool fs_enum_dir(const char *directory, fs_enum_fn_t *fn, void *enum_arg)
{
   bool success = FALSE;
   size_t dirnamelen = strlen(directory);

   /* Check whether an empty directory has been given as an input */
   if (dirnamelen == 0){
      error_printf("%s: bad directory name ''\n", codeid());
   } else
   {
      HANDLE hFind;
      WIN32_FIND_DATA FindFileData;
      DWORD dwAttrs;
      char subdir[256];
      bool cont = TRUE;

      if (OS_FS_SEP == directory[dirnamelen-1])
         dirnamelen--;
      memcpy(subdir, directory, dirnamelen);

      subdir[dirnamelen+0] = OS_FS_SEP;
      subdir[dirnamelen+1] = '*';
      subdir[dirnamelen+2] = '\0';

      OMIT(printf("%s: first file '%s'\n", codeid(), subdir););
      hFind = FindFirstFile(subdir, &FindFileData);
      success = hFind != INVALID_HANDLE_VALUE;
      if (!success){
         error_printf("%s: can't open '%s' as a directory - (rc %d)\n",
                      codeid(), subdir, (int)GetLastError());
      } else
      do {
         subdir[dirnamelen] = '\0';

         if (hFind == INVALID_HANDLE_VALUE){
            error_printf("%s: can't get next file in '%s' - (rc %d)\n",
                         codeid(), subdir, (int)GetLastError());
            success = FALSE;
         } else {
            subdir[dirnamelen+0] = OS_FS_SEP;
            subdir[dirnamelen+1] = '\0';
            strcat(subdir, FindFileData.cFileName);

            dwAttrs = GetFileAttributes(subdir);
            OMIT(printf("%s: file '%s' %svalid\n", codeid(), subdir,
                   dwAttrs == INVALID_FILE_ATTRIBUTES? "in":""););
            if (dwAttrs == INVALID_FILE_ATTRIBUTES)
               cont = (*fn)(enum_arg, FindFileData.cFileName, subdir,
                            FALSE/*examinable*/, FALSE/*isdir*/);
            else
               cont = (*fn)(enum_arg, FindFileData.cFileName, subdir,
                            TRUE/*examinable*/,
                            0 != (dwAttrs & FILE_ATTRIBUTE_DIRECTORY));
        }
      } while (cont && success && FindNextFile(hFind, &FindFileData) != 0);
   }
   return success;
}

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


/*  Populates the dr_ptr parameter with a list of the directories that exist
 *  in the directory that is given as the "directory" parameter. The second
 *  parameter needs to be allocated as this pointer gets dereferenced
 */
extern bool fs_enum_dir(const char *directory, fs_enum_fn_t *fn, void *enum_arg)
{
   bool success = FALSE;
   size_t dirnamelen = strlen(directory);

   /* Check whether an empty directory has been given as an input */
   if (dirnamelen == 0){
      error_printf("%s: bad directory name ''\n", codeid());
   } else
   {
      struct dirent *dir_struct;
      int rc;
      DIR *dir_ptr;
      char subdir[256];

      success = TRUE;
      if (OS_FS_SEP == directory[dirnamelen-1])
         dirnamelen--;
      memcpy(&subdir[0], directory, dirnamelen);
      subdir[dirnamelen] = '\0';

      dir_ptr = opendir(directory); /* still valid even if ends OS_FS_SEP */

      if (dir_ptr == NULL)
      {
         error_printf("%s: can't open '%s' as a directory - %s (rc %d)\n",
                      codeid(), directory, strerror(errno), errno);
         success = FALSE;
      } else {
         bool cont = TRUE; /*continue*/

         while (cont && success && NULL != (dir_struct = readdir(dir_ptr)))
         {
            struct stat stat_ptr;

            subdir[dirnamelen+0] = OS_FS_SEP;
            subdir[dirnamelen+1] = '\0';
            strcat(subdir, dir_struct->d_name);

            rc = lstat(subdir, &stat_ptr);
            OMIT(printf("%s: file '%s' %svalid\n", codeid(), subdir,
                        rc != 0? "in":""););
            if (rc != 0)
               cont = (*fn)(enum_arg, dir_struct->d_name, subdir,
                            FALSE /*examinable*/, FALSE);
            else
               cont = (*fn)(enum_arg, dir_struct->d_name, subdir,
                            TRUE /*examinable*/,
                            S_ISDIR(stat_ptr.st_mode));
         }
      }
   }
   return success;
}

#endif
