/*
 * Copyright (c) 2014, Broadcom Inc.
 * Copyright (c) 2005-2017, Gray Girling
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

/*! @author  Gray Girling
 *  @file libftl_elf.c
 *  @brief ELF functions for FTL
 *         This utility provides some FTL functions to manipulate ELF files
 *         based on the BSD-licensed libelf API.
 */



/*****************************************************************************
 *                                                                           *
 *          Configuration                                                    *
 *                                                                           *
 *****************************************************************************/


#ifndef USE_LIB_BCM
#define USE_LIB_BCM 0 // if using the old hand-built Broadcom ELF libraries
#endif

#ifndef USE_LIB_ELF
#define USE_LIB_ELF 0 // if using the 'standard' GNU libelf - usually lib GPLed
/* e.g. available when using Debian Linux after apt install libelf-dev */
#endif

#ifndef USE_LIB_GELF
#define USE_LIB_GELF 0 // if using the BSD licensed libgelf
/* e.g. available when using the BSD sources for free libelf */
#endif


// Normalize USE_LIB* (to be either 0 or 1)

#if USE_LIB_BCM
#define USE_LIB_BCM 1
#endif

#if USE_LIB_ELF
#define USE_LIB_ELF 1
#endif

#if USE_LIB_GELF
#define USE_LIB_GELF 1
#endif

#if (USE_LIB_BCM+USE_LIB_ELF+USE_LIB_GELF) <= 0
#error The ELF library to be used is not defined
#endif
#if (USE_LIB_BCM+USE_LIB_ELF+USE_LIB_GELF) > 1
#error Too many ELF libraries have been specified 
#endif



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
#include <assert.h>

#include "ftl.h"
#include "ftl_internal.h"
#include "ftl_elf.h"       /* our library */

#if __GNUC__
#include <err.h>
#endif

#include <fcntl.h>

#if USE_LIB_BCM
/* These headers are ghastly .. they give us sys/cdefs.h which can do nasty
   things to C when using cl
*/
#undef __volatile
#include "libelf_outer.h" /* library help functions */
#include "libelf.h"
#include "gelf.h"
/* Try to undo the damage sys/cdefs.h might have done */
#undef volatile
#undef const
#undef signed
#undef inline
#define LIB_NAME_GELF 1
#endif


#if USE_LIB_ELF
#include "libelf.h"
#define LIB_NAME_GELF 0
#endif

#if USE_LIB_GELF
#include "gelf.h"
#define LIB_NAME_GELF 1
#endif





/*****************************************************************************
 *                                                                           *
 *          Library Independence                                             *
 *          ====================                                             *
 *                                                                           *
 *****************************************************************************/




#if LIB_NAME_GELF
#define ELF32_Ehdr GElf_Ehdr
#define ELF32_Phdr GElf_Phdr
#define ELF32_getehdr gelf_getehdr
#define ELF32_getphdr_ix gelf_getphdr
#define ELF_getclass(id, elf) gelf_getclass(elf)
#endif

#if !LIB_NAME_GELF
#define ELF32_Ehdr Elf32_Ehdr
#define ELF32_Phdr Elf32_Phdr
#define ELF32_getehdr(elf, ehdr) elf32_getehdr(elf)

/*! Return ELFCLASSNONE, ELFCLASS32, ELFCLASS64
 *     @param ident     bytes returned by elf_getident()
 */
int ELF_getclass (char *ident, Elf *elf)
{
    int class = ELFCLASSNONE;
    if (elf != NULL && ident != NULL && elf_kind(elf) == ELF_K_ELF)
    {
        class = ident[EI_CLASS];
    }
    return class;
}

ELF32_Phdr *ELF32_getphdr_ix(Elf *elf, int n, ELF32_Phdr *out_phdr)
{
    ELF32_Phdr *result = NULL;
    if (elf != NULL && out_phdr != NULL)
    {
        Elf32_Phdr *phdr_array = elf32_getphdr(elf);
        if (phdr_array != NULL)
        {
            memcpy(out_phdr, &phdr_array[n], sizeof(*out_phdr));
            result = out_phdr;
        }
    }
    return result;
}


#endif








/*****************************************************************************
 *                                                                           *
 *          File System  				                     *
 *          ===========                                                      *
 *                                                                           *
 *****************************************************************************/




#include <unistd.h>



// FD operations
#define fe_open(name,flags,mode) open(name, O_RDONLY)
#define fe_close                 close
#define fe_lseek(fd, offset)     lseek(fd, offset, SEEK_SET)
#define fe_read                  read





/*****************************************************************************
 *                                                                           *
 *          Load File  				                             *
 *          =========                                                        *
 *                                                                           *
 *****************************************************************************/






static const value_t *
fn_kind(const value_t *this_fn, parser_state_t *state)
{
   const value_t *resval = NULL;
   value_t *strval = (value_t *)parser_builtin_arg(state, 1);
   const char *filename;
   size_t filenamelen;

   if (value_string_get(strval, &filename, &filenamelen))
   {
      int fd = fe_open(filename, "rb", 0);

      if (fd < 0)
         parser_report(state, "couldn't open ELF file to read - %s\n",filename);
      else {
         Elf *e = elf_begin(fd, ELF_C_READ, NULL);

         if (e == NULL)
            parser_report(state, "couldn't initialize ELF library - %s\n",
                          elf_errmsg(-1));
         else {
            Elf_Kind ek = elf_kind(e);

            switch (ek) {
               case ELF_K_AR:
                  resval = value_string_new_measured("archive");
                  break;
               case ELF_K_ELF:
                  resval = value_string_new_measured("elf");
                  break;
               case ELF_K_NONE:
                  resval = value_string_new_measured("data");
                  break;
               default:
                  resval = &value_null;
            }

            (void)elf_end(e);
         }
         (void)fe_close(fd);
      }
   } else
       parser_report_help(state, this_fn);

   return resval;
}





typedef const value_t *
elf_hdr_fn_t(const value_t *this_fn, parser_state_t *state,
             const char *filename, int binfd,
             Elf *e, ELF32_Ehdr *ehdr, void *fn_arg);


static const value_t *
genfn_with_elfhdr(const value_t *this_fn, parser_state_t *state, int file_arg,
                  elf_hdr_fn_t *elf_fn, void *fn_arg)
{
   const value_t *retval = NULL;
   value_t *strval = (value_t *)parser_builtin_arg(state, file_arg);
   const char *filename;
   size_t filenamelen;

   if (value_string_get(strval, &filename, &filenamelen))
   {
      int fd = fe_open(filename, "rb", 0);

      if (fd < 0)
         parser_report(state, "couldn't open ELF file to read - %s\n",filename);
      else {
         Elf *e = elf_begin(fd, ELF_C_READ, NULL);

         if (e == NULL)
            parser_report(state, "couldn't initialize ELF library - %s\n",
                          elf_errmsg(-1));
         else {
            Elf_Kind ek = elf_kind(e);
            if (ek != ELF_K_ELF)
               parser_report(state, "not an ELF file - %s\n", elf_errmsg(-1));
            else {
               ELF32_Ehdr ehdr;
               if (ELF32_getehdr(e, &ehdr) == NULL)
                  parser_report(state, "no execution header in ELF file - %s\n",
                                elf_errmsg(-1));
               else  {
                  retval = (*elf_fn)(this_fn, state, filename, fd,
                                     e, &ehdr, fn_arg);
               }
            }

            (void)elf_end(e);
         }
         (void)fe_close(fd);
      }
   } else
       parser_report_help(state, this_fn);

   return retval;
}



static const value_t *
elf_get_ehdr(const value_t *this_fn, parser_state_t *state,
             const char *filename, int binfd,
             Elf *e, ELF32_Ehdr *ehdr, void *arg)
{
   char *id = elf_getident(e, NULL);
   int eclass = ELF_getclass(id, e);
   dir_t *einfo = dir_id_new();
   dir_t *edir = dir_id_new();
   size_t n;

   if (eclass == ELFCLASS32)
      dir_string_set(einfo, "class", value_int_new(32));
   else if (eclass == ELFCLASS64)
      dir_string_set(einfo, "class", value_int_new(64));
   else {
      dir_string_set(einfo, "class", &value_null);
      if (eclass != ELFCLASSNONE)
         parser_report(state, "ELF file has unknown class %d - %s\n",
                       eclass, elf_errmsg(-1));
   }

   if (id == NULL)
      parser_report(state, "ELF file has unknown identity - %s\n",
                    elf_errmsg(-1));
   else
      dir_string_set(einfo, "id",
                     value_string_new(id, EI_NIDENT));

   dir_string_set(einfo, "ehdr", dir_value(edir));

   dir_string_set(edir,"type"   , value_int_new(ehdr->e_type));
   dir_string_set(edir,"machine", value_int_new(ehdr->e_machine));
   dir_string_set(edir,"version", value_int_new(ehdr->e_version));
   dir_string_set(edir,"entry"  , value_int_new(ehdr->e_entry));
   dir_string_set(edir,"phoff"  , value_int_new(ehdr->e_phoff));
   dir_string_set(edir,"shoff"  , value_int_new(ehdr->e_shoff));
   dir_string_set(edir,"flags"  , value_int_new(ehdr->e_flags));
   dir_string_set(edir,"ehsize" , value_int_new(ehdr->e_ehsize));
   dir_string_set(edir,"phentsize",
                  value_int_new(ehdr->e_phentsize));
   dir_string_set(edir,"shentsize",
                  value_int_new(ehdr->e_shentsize));

   if (elf_getphdrnum(e, &n) != 0)
      parser_report(state,"ELF file has unknown number of segments - %s\n",
                    elf_errmsg(-1));
   else
      dir_string_set(einfo, "progsects", value_int_new(n));

   if (elf_getshdrnum(e, &n) != 0)
      parser_report(state, "%s: ELF file has unknown number of sections - %s\n",
                    elf_errmsg(-1));
   else
      dir_string_set(einfo, "sections", value_int_new(n));

   if (elf_getshdrstrndx(e, &n) != 0)
      parser_report(state, "%s: ELF file has no section names - %s\n",
                    elf_errmsg(-1));
   else
      dir_string_set(einfo, "shdrstrndx", value_int_new(n));

   return dir_value(einfo);
}




static const value_t *
fn_elfhdr(const value_t *this_fn, parser_state_t *state)
{   return genfn_with_elfhdr(this_fn, state, 1, &elf_get_ehdr, NULL);
}




static const value_t *
elf_get_phdrs(const value_t *this_fn, parser_state_t *state,
              const char *filename, int binfd,
              Elf *e, ELF32_Ehdr *ehdr, void *arg)
{
   const value_t *resval = NULL;
   size_t n;

   if (elf_getphdrnum(e, &n) != 0)
      parser_report(state, "ELF file has unknown number of segments - %s\n",
                    elf_errmsg(-1));
   else {
      size_t i;
      dir_t *partdir = dir_vec_new();

      for (i = 0; i < n; i++) {
         ELF32_Phdr phdr;
         dir_t *pdir = dir_id_new();
         if (ELF32_getphdr_ix(e, i, &phdr) != &phdr)
            parser_report(state,
                          "ELF segment %d unretrievable - %s\n",
                          i, elf_errmsg(-1));
         else {
            dir_string_set(pdir,"type"   , value_int_new(phdr.p_type));
            dir_string_set(pdir,"offset" , value_int_new(phdr.p_offset));
            dir_string_set(pdir,"vaddr"  , value_int_new(phdr.p_vaddr));
            dir_string_set(pdir,"paddr"  , value_int_new(phdr.p_paddr));
            dir_string_set(pdir,"filesz" , value_int_new(phdr.p_filesz));
            dir_string_set(pdir,"memsz"  , value_int_new(phdr.p_memsz));
            dir_string_set(pdir,"flags"  , value_int_new(phdr.p_flags));
            dir_string_set(pdir,"align"  , value_int_new(phdr.p_align));

            dir_int_set(partdir, i, dir_value(pdir));
         }
      }
      resval = dir_value(partdir);
   }

   return resval;
}




static const value_t *
fn_elfphdrs(const value_t *this_fn, parser_state_t *state)
{   return genfn_with_elfhdr(this_fn, state, 1, &elf_get_phdrs, NULL);
}




static const value_t * /*binding*/
value_closure_bind_2(parser_state_t *state, const value_t *fn,
                     const char *msg1, const value_t *a1,
                     const char *msg2, const value_t *a2)
{
   const value_t *code = value_closure_bind(fn, a1);

   if (NULL != code) {
      code = value_closure_bind(code, a2);
      if (code == NULL)
         parser_error(state, "ELF segment %d - couldn't apply "
                      "%s%s argument to function\n",
                      msg1==NULL? "": msg1, msg1==NULL?"1st":" (1st)");
   } else
      parser_error(state, "ELF segment %d - couldn't apply "
                   "%s%s argument to function\n",
                   msg2==NULL? "": msg2, msg2==NULL?"2nd":" (2nd)");

   return code;
}




typedef struct {
   const value_t *hdrcheckfn;
   const value_t *memzerofn;
   const value_t *memwrfn;
} elf_loadfn_args_t;




/* Invoke the argument FTL function with <addr> <buf> arguments for each of
   the ELF file's program segments */
static const value_t *
elf_load_with_fn(const value_t *this_fn, parser_state_t *state,
                 const char *filename, int binfd,
                 Elf *e, ELF32_Ehdr *ehdr, void *arg)
{
   elf_loadfn_args_t *fnarg = (elf_loadfn_args_t *)arg;
   const value_t *hdrcheckfn = fnarg->hdrcheckfn;
   const value_t *memwrfn = fnarg->memwrfn;
   const value_t *resval = value_false;
   size_t n;

   if (elf_getphdrnum(e, &n) != 0)
      parser_report(state, "ELF file has unknown number of segments - %s\n",
                    elf_errmsg(-1));
   else {
      size_t i;
      bool ok = FALSE;

      if (hdrcheckfn != NULL) {
         const value_t *ehdrval = elf_get_ehdr(this_fn, state, filename, binfd,
                                               e, ehdr, /*arg*/NULL);
         if (ehdrval != NULL) {
            const value_t *code = value_closure_bind(hdrcheckfn, ehdrval);

            if (code == NULL) {
               parser_error(state, "couldn't apply "
                            "header argument to check function\n");
            } else {
               const value_t *fnres = invoke(code, state);
               /* we expect this to return a TRUE/FALSE value */
               ok = (fnres == value_true);
               /*if (!ok) printf("%s: check fn returns non-TRUE\n", codeid());*/
            }
         }
      } else
         ok = TRUE;

      resval = ok? value_true: value_false;

      if (ok && memwrfn != NULL) {
         for (i = 0; ok && i < n; i++) {
            ELF32_Phdr phdr;

            if (ELF32_getphdr_ix(e, i, &phdr) != &phdr)
               parser_report(state, "ELF segment %d unretrievable - %s\n",
                             i, elf_errmsg(-1));
            else {
               long offset = (long)phdr.p_offset;
               if (phdr.p_offset != (Elf64_Off)offset)
                  parser_report(state, "ELF segment %d - "
                                 "offset does not fit in 32-bits\n", i);
               else
               if (-1 == fe_lseek(binfd, offset))
                  parser_report(state, "ELF segment %d - "
                                 "offset 0X%lX is outside file\n",
                                 i, offset);
               else
               if (phdr.p_memsz < phdr.p_filesz)
                  parser_report(state,
                                "ELF segment %d - smaller size in memory (%d)"
                                "than on file (%d) - corrupt header?\n",
                                i, phdr.p_memsz, phdr.p_filesz);
               else {
                  number_t addr = phdr.p_vaddr; /* where to load segment */
                  char *buf = NULL;
                  size_t buflen = (size_t)phdr.p_memsz;
                  value_t *addrval = value_int_new(addr);
                  value_t *dataval = value_string_alloc_new(buflen, &buf);

                  if (phdr.p_memsz != (Elf64_Xword)buflen)
                     parser_report(state, "ELF segment %d - "
                                    "size does not fit in 32-bits\n", i);
                  else
                  if (dataval == NULL)
                     parser_report(state,
                                   "ELF segment %d - no memory for "
                                   "next %d bytes\n",
                                   i, buflen);
                  else {
                     ssize_t readbytes = fe_read(binfd, buf, buflen);

                     if (readbytes < buflen)
                        parser_report(state,
                                      "ELF segment %d - "
                                      "only %d bytes of %d read\n",
                                      i, readbytes);
                     else {
                        const value_t *code;
                        ok = TRUE;

                        /* zero any uninitialized area */
                        if (phdr.p_memsz > phdr.p_filesz) {
                           size_t extra_bytes = (size_t)
                                                (phdr.p_memsz - phdr.p_filesz);
                           if ((Elf64_Xword)extra_bytes !=
                               phdr.p_memsz-phdr.p_filesz) {
                              ok = FALSE;
                              parser_report(state, "ELF segment %d - "
                                            "BSS area larger than 32-bits\n",
                                            i);
                           } else
                              memset(&buf[phdr.p_filesz], '\0', extra_bytes);
                        }

                        code = value_closure_bind_2(state, memwrfn,
                                                    "address", addrval,
                                                    "data", dataval);

                        if (NULL != code) {
                           const value_t *fnres = invoke(code, state);
                           /* we expect this to return a TRUE/FALSE value */
                           if (fnres != value_true) {
                              /*printf("%s: seg fn returns non-TRUE\n",
                                       codeid());*/
                              ok = FALSE;
                           }
                           /* we rely on garbage collection to collect the data
                            * buffer */
                        } else
                           value_delete(&dataval);
                     }
                  }
               }
            }
         }

         if (!ok)
            resval = value_false;
      }
   }
   return resval;
}




static const value_t *
fn_elfrdsegs(const value_t *this_fn, parser_state_t *state)
{  value_t *hdrcheckfn = (value_t *)parser_builtin_arg(state, 1);
   /*value_t *memzerofn = (value_t *)parser_builtin_arg(state, 1);*/
   value_t *memwrfn = (value_t *)parser_builtin_arg(state, 2);
   const value_t *retval = NULL;

   if (hdrcheckfn == &value_null)
      hdrcheckfn = NULL;

   if (memwrfn == &value_null)
      memwrfn = NULL;

   if ((hdrcheckfn == NULL || value_istype(hdrcheckfn, type_closure)) &&
       (memwrfn == NULL || value_istype(memwrfn, type_closure)))
   {
      elf_loadfn_args_t args;
      args.hdrcheckfn = hdrcheckfn;
      args.memzerofn = NULL;
      args.memwrfn = memwrfn;
      retval = genfn_with_elfhdr(this_fn, state, 3, &elf_load_with_fn, &args);
   } else
      parser_report_help(state, this_fn);

   return retval;
}





static const value_t *
fn_ehdr_type(const value_t *this_fn, parser_state_t *state)
{  value_t *typval = (value_t *)parser_builtin_arg(state, 1);
   const value_t *retval = NULL;

   if (value_istype(typval, type_int))
   {  unsigned typ = (unsigned)value_int_number(typval);
      const char *typestr;

      switch (typ)
      {
         case ET_NONE: typestr = "untyped"; break;
         case ET_REL: typestr = "relocatable"; break;
         case ET_EXEC: typestr = "executable"; break;
         case ET_DYN: typestr = "shared"; break;
         default:
            if (typ >= ET_LOOS && typ <= ET_HIOS)
               typestr = "os-specific";
            else if (typ >= ET_LOPROC && typ <= ET_HIPROC)
               typestr = "processor-specific";
            else
               typestr = "unknown";
            break;
      }
      retval = value_string_new_measured(typestr);
   } else
      parser_report_help(state, this_fn);

   return retval;
}




#define WITH_ELF_EF_FLAG(APPLY,_arg) \
    APPLY(EF_SPARC_LEDATA, 0x800000, little endian data, _arg)          \
    APPLY(EF_SPARC_32PLUS, 0x000100, generic V8+ features, _arg)        \
    APPLY(EF_SPARC_SUN_US1,0x000200, Sun UltraSPARC1 extensions, _arg)  \
    APPLY(EF_SPARC_HAL_R1, 0x000400, HAL R1 extensions, _arg)           \
    APPLY(EF_SPARC_SUN_US3,0x000800, Sun UltraSPARCIII extensions, _arg) \
    APPLY(EF_MIPS_NOREORDER,1, A .noreorder directive was used., _arg)  \
    APPLY(EF_MIPS_PIC, 2, Contains PIC code., _arg)                     \
    APPLY(EF_MIPS_CPIC, 4, Uses PIC calling sequence., _arg)            \
    APPLY(EF_MIPS_XGOT, 8, XGOT, _arg)                                  \
    APPLY(EF_MIPS_64BIT_WHIRL,16, 64 bit Whirl, _arg)                   \
    APPLY(EF_MIPS_ABI2, 32, ABI 2, _arg)                                \
    APPLY(EF_MIPS_ABI_ON32,64, ABI on 32-bit, _arg)                     \
    APPLY(EF_MIPS_FP64, 512, Uses FP64 (12 callee-saved, _arg)., _arg)  \
    APPLY(EF_MIPS_NAN2008, 1024, Uses IEEE 754-2008 NaN encoding., _arg) \
    APPLY(EF_PARISC_TRAPNIL,0x00010000, Trap nil pointer dereference., _arg) \
    APPLY(EF_PARISC_EXT, 0x00020000, Program uses arch. extensions., _arg) \
    APPLY(EF_PARISC_LSB, 0x00040000, Program expects little endian., _arg) \
    APPLY(EF_PARISC_WIDE, 0x00080000, Program expects wide mode., _arg) \
    APPLY(EF_PARISC_NO_KABP,0x00100000, No kernel assisted branch prediction., _arg) \
    APPLY(EF_PARISC_LAZYSWAP,0x00400000, Allow lazy swapping., _arg)    \
    APPLY(EF_ALPHA_32BIT, 1, All addresses must be < 2GB., _arg)            \
    APPLY(EF_ALPHA_CANRELAX,2, Relocations for relaxing exist., _arg)       \
    APPLY(EF_PPC_EMB, 0x80000000, PowerPC embedded flag, _arg)              \
    APPLY(EF_PPC64_ABI,3, ABI, _arg)                                        \
    APPLY(EF_ARM_RELEXEC, 0x01, RELEXEC, _arg)                              \
    APPLY(EF_ARM_HASENTRY, 0x02 , Has entry, _arg)                          \
    APPLY(EF_ARM_INTERWORK,0x04 , Interwork, _arg)                          \
    APPLY(EF_ARM_APCS_26, 0x08 , APCS 26, _arg)                             \
    APPLY(EF_ARM_APCS_FLOAT,0x10 , APCS float, _arg)                        \
    APPLY(EF_ARM_PIC, 0x20, Position independent code, _arg)                \
    APPLY(EF_ARM_ALIGN8, 0x40, 8-bit structure alignment is in use, _arg)   \
    APPLY(EF_ARM_NEW_ABI, 0x80, New ABI, _arg)                              \
    APPLY(EF_ARM_OLD_ABI, 0x100, Old ABI, _arg)                             \
    APPLY(EF_ARM_SOFT_FLOAT,0x200, Soft floating point, _arg)               \
    APPLY(EF_ARM_VFP_FLOAT,0x400, VFP floating point, _arg)                 \
    APPLY(EF_ARM_MAVERICK_FLOAT,0x800, Materick floating point, _arg)       \
    APPLY(EF_S390_HIGH_GPRS, 0x00000001, High GPRs kernel facility needed., _arg) 




static const value_t *
fn_ehdr_flags(const value_t *this_fn, parser_state_t *state)
{  value_t *mcval = (value_t *)parser_builtin_arg(state, 1);
   value_t *flagsval = (value_t *)parser_builtin_arg(state, 2);
   const value_t *retval = NULL;

   if (value_istype(mcval, type_int) && value_istype(flagsval, type_int))
   {  unsigned long flags = (unsigned long)value_int_number(flagsval);
      unsigned mc = (unsigned)value_int_number(mcval);
      /* This isn't very important - none of the prefixes available are for
         VideoCore */
      const char *name_prefix =
         mc == EM_ARM? "EF_ARM_":
         mc == EM_MIPS || mc == EM_MIPS_RS3_LE || mc == EM_MIPS_X? "EF_MIPS_":
         mc == EM_PPC || mc == EM_PPC64? "EF_PPC_":
         mc == EM_SPARC || mc == EM_SPARC32PLUS? "EF_SPARC_":
         mc == EM_SPARCV9? "EF_SPARCV9_":
         "EF_XXX_";
      dir_t *vec = dir_vec_new();
      int index = 0;
      const char *flagname;

#undef   _ELF_DEFINE_EF
#define  _ELF_DEFINE_EF(name, val, msg, arg)        \
      flagname = #name;                 \
      if (val != 0 && val==(flags & val) && \
          flagname == strstr(flagname, name_prefix)) {              \
         const char *info = #msg;                                    \
         if (0 == strcmp(info, "GNU EABI extension"))                \
            info = flagname+3;                                       \
         dir_int_set(vec, index++, value_string_new_measured(info)); \
      }

      WITH_ELF_EF_FLAG(_ELF_DEFINE_EF, )

      if (flags != 0 && index == 0) {
         /* no descriptions identified */
         unsigned int bit = 0;
         char flagname[] = "flag_xxxxxxxxxxx";
         char *flagno = &flagname[5];

         while (bit < 32) {
            if (0 != (flags & (1<<bit))) {
               sprintf(flagno, "%d", bit);
               dir_int_set(vec, index++, value_string_new_measured(flagname));
            }
            bit++;
         }
      }

      retval = dir_value(vec);
   } else
      parser_report_help(state, this_fn);

   return retval;
}



#define WITH_ELF_PF_FLAG(APPLY,_arg) \
    APPLY(PF_X, (1 << 0), Segment is executable, _arg)      \
    APPLY(PF_W, (1 << 1), Segment is writable, _arg)        \
    APPLY(PF_R, (1 << 2), Segment is readable, _arg)



static const value_t *
fn_phdr_flags(const value_t *this_fn, parser_state_t *state)
{  value_t *flagsval = (value_t *)parser_builtin_arg(state, 1);
   const value_t *retval = NULL;

   if (value_istype(flagsval, type_int))
   {  unsigned long flags = (unsigned long)value_int_number(flagsval);
      dir_t *vec = dir_vec_new();
      int index = 0;

#undef   _ELF_DEFINE_PF
#define  _ELF_DEFINE_PF(name, val, msg, _opt)                            \
      if ((val) != 0 && (val) == (flags & (val)))                   \
         dir_int_set(vec, index++, value_string_new_measured(#msg));

      WITH_ELF_PF_FLAG(_ELF_DEFINE_PF, )

      retval = dir_value(vec);
   } else
      parser_report_help(state, this_fn);

   return retval;
}





/*****************************************************************************
 *                                                                           *
 *          Load File                                   *
 *          =========                                                        *
 *                                                                           *
 *****************************************************************************/




#if 0

/* This code is kept for reference - it is a modified version of that found
   elsewhere in the VideoCore Broadcom code */

#define elf_display printf
#define elf_display printf


static bool
load_file(parser_state_t *state, void *handle, char *elf_file_name,
          ELFMEM_T *mem)
{
   Elf32_Ehdr ehdr;
   Elf32_Shdr *shdrs;
   Elf32_Phdr *phdrs;
   Elf32_Shdr *dynS;
   FILE *binfile;
   int plen, phlen;
   int largestmem = 0;
   bool ok = FALSE;

   /*
   * Open the dll for reading and check for error.
   */

   binfile = fe_open(elf_file_name, "rb", 0);

   if (binfile != NULL) {
      parser_report(state, "can't open '%s'", elf_file_name);
   } else {
      /*
      * Read ELF header and confirm that it looks correct.
      */
      if (fe_read(&ehdr, 1,sizeof(ehdr), binfile) < sizeof(ehdr)) {
         parser_report(state, "can't read ELF header of %s", elf_file_name);
      } else
      if (memcmp(ELFMAG, ehdr.e_ident, SELFMAG) != 0) {
         parser_report(state, "not a valid ELF file - %s", elf_file_name);
      } else
      /*
      * Check endianess and target...
      */
      if (ehdr.e_ident[EI_DATA] != ELFENDIAN) {
         parser_report(state, "ELF file has the wrong endianness - %s",
                       elf_file_name);
      } else
      /*
      * Check machine
      */
      if (!(ehdr.e_machine == EM_VIDEOCORE
            || ehdr.e_machine == EM_VIDEOCORE3))
      {
           parser_report(state, "ELF file contains no code - %s",
                         elf_file_name, MACHINE_NAME);
      } else
      /*
      * Confirm that it is an exe.
      * (Temporarily support loading of exe's also).
      */
      if (ehdr.e_type != ET_EXEC){
         parser_report(state, "ELF file is not an executable - %s",
                       elf_file_name);
      } else

      /*
      * Now read in the section headers
      */
      if (ehdr.e_shnum > 100000) {
         parser_report(state, "ELF file section header has silly size "
                      "(%d bytes) - %s",
                      ehdr.e_shnum, elf_file_name);
      } else {
         int pos;

         /*
         * Map each program section into memory.
         * addrMap[i] is the address of segment of phdrs[i]
         */

         /* we malloc and free this block for section headers to avoid creating
          * holes */
         plen = sizeof(Elf32_Shdr) * ehdr.e_shnum;

         shdrs = elf_malloc(plen, mem, ELFMEM_HOST);
         /*< These end up in two places becuase we need to look at this data
          *  before we know how much to malloc*/

         elf_display("%s: reading in %d section headers from 0x%x\n",
                     codeid(), ehdr.e_shnum, ehdr.e_shoff);

         /* this is just used for calculating malloc sizes required - we will
          * reload it again to keep */
         pos = fseek(binfile, ehdr.e_shoff, SEEK_SET);
         elf_display("%s: moved to %p(binfile)\n",codeid(), i, binfile);

         if (fe_read(shdrs, 1, plen, binfile) < plen) {
            parser_report(state, "%s: unable to read section headers.",
                          elf_file_name);
         } else

         /*
         * Now read in the program headers
         */
         if (ehdr.e_phnum > 1000) {
            parser_report(state,
                          "%s: program header size appears to be garbage.",
                          elf_file_name);
         } else {
            int i;

            ok = TRUE;
            /*
            * Map each program section into memory.
            * addrMap[i] is the address of segment of phdrs[i]
            */
            /* we malloc and free this block for section headers to avoid
             * creating holes */
            phlen = sizeof(Elf32_Phdr) * ehdr.e_phnum;

            phdrs = elf_malloc(plen, mem, ELFMEM_HOST);
            /*< These end up in two places becuase we need to look at this data
             *  before we know how much to malloc
             */

            elf_display("%s: reading in %d program headers from 0x%x\n",
                        codeid(), ehdr.e_phnum, ehdr.e_phoff);

            /* this is just used for calculating malloc sizes required - we
             * will reload it again to keep */
            pos = fseek(binfile, ehdr.e_phoff, SEEK_SET);

            if (fe_read(phdrs, 1, phlen, binfile) < phlen) {
               fe_close(binfile);
               parser_report(state, "unable to read program headers - %s",
                             elf_file_name);
               return 0;
            }

            for (i = 0; ok && i < ehdr.e_phnum; i++)
            {
                Elf32_Phdr *p = phdrs + i;
                char *addrp;

                elf_display("%s: [%d] Type=%d offset=%d virtual=0x%x phys=0x%x "
                            "size=%d memsize=%d flags=%d align=%d\n",
                            codeid(), i, p->p_type,p->p_offset,
                            p->p_vaddr,p->p_paddr,p->p_filesz,p->p_memsz,
                            p->p_flags,p->p_align);

                addrp = mem->datastore + p->p_vaddr;
                ok = FALSE;

                if (p->p_vaddr < 0 ||
                    p->p_vaddr+p->p_memsz >= mem->internalmem) {
                   elf_display("%s: could not fit ELF data into memory\n",
                               codeid());
                } else
                if (-1 == fseek(binfile, p->p_offset, SEEK_SET)) {
                   elf_display("%s: offset 0X%lX is outside file\n",
                               codeid(), p->p_offset);
                } else
                if (fe_read(addrp, 1, p->p_filesz, binfile) < p->p_filesz) {
                   elf_display("%s: could not read data\n", codeid());
                } else {
                   ok = TRUE;

                   if ((unsigned)p->p_memsz > p->p_filesz)
                      memset(addrp+p->p_filesz,'\0',p->p_memsz-p->p_filesz);

                   if (p->p_vaddr + p->p_memsz > largestmem)
                      largestmem = p->p_vaddr + p->p_memsz;
                }
            }

            if (ok) {
               if (mem->internal_used)
                  mem->internal_used[0] = largestmem;

               ok = FALSE;
               pos = fseek(binfile, 0, SEEK_END);
               if (-1 == pos)
                  elf_display("%s: failed to find size of ELF file\n",
                              codeid());
               else {
                  elf_data = elf_malloc(pos, mem, ELFMEM_HOST);
                  if (-1 == fseek(binfile, 0, SEEK_SET))
                     elf_display("%s: failed to rewind ELF file\n", codeid());
                  else
                  if (pos != fe_read(elf_data, 1, pos, binfile))
                     elf_display("%s: failed to read %d-byte file\n",
                                 codeid(), pos);
                  else
                     ok = TRUE;
               }

               if (ok)
                  /* Now examine debug info */
                  for (i = 0; i < ehdr.e_shnum; i++) {
                     Elf32_Shdr *p = shdrs+i;
                     char *strtable = elf_data +
                                      shdrs[ehdr.e_shstrndx].sh_offset;
                     char *name = &strtable[p->sh_name];
                     char *d = &elf_data[p->sh_offset];

                     elf_display("%s: @%d %x File %x->+%x Mem %x %s "
                                 "Flags %x Align %x\n",
                                 codeid(), pos, p->sh_type,
                                 p->sh_offset, p->sh_size,
                                 p->sh_addr, name, p->sh_flags,
                                 p->sh_addralign);

                     if (strcmp(name,".debug_info")==0)
                        elf_debuginfo = d;
                     if (strcmp(name,".debug_line")==0)
                        elf_debugline = d;
                     if (strcmp(name,".debug_frame")==0)
                        elf_debugframe = d;
                     if (strcmp(name,".debug_abbrev")==0)
                        elf_debugabbrev = d;

                     if (strcmp(name,".debug_pubnames")==0)
                     {
                        elf_debugpubnames = d;
                        elf_debugpubnameslen = p->sh_size;
                     }
                     if (strcmp(name,".debug_aranges")==0)
                        elf_debugaranges = d;
                     if (strcmp(name,".strtab")==0)
                     {
                        elf_strtab = d;
                        elf_strtablen = p->sh_size;
                     }
                     if (strcmp(name,".symtab")==0)
                     {
                        elf_symtab = d;
                        elf_symtablen = p->sh_size;
                     }
                  }
               }
            }
         }
      }
      fe_close(binfile);
   }

   return ok;
}
#endif







/*****************************************************************************
 *                                                                           *
 *          Whole Command set                                                *
 *          =================                                                *
 *                                                                           *
 *****************************************************************************/




extern bool
cmds_elf(parser_state_t *state, dir_t *cmds)
{
   bool ok = (elf_version(EV_CURRENT) != EV_NONE);

   if (!ok)
      printf("%s: ELF library version unknown - %s\n",
             codeid(), elf_errmsg(-1));
   else {
      mod_addfn(cmds, "kind", "<file> - return the kind of data in <file>",
                &fn_kind, 1);
      mod_addfn(cmds, "hdr", "<file> - the ELF header in <file>",
                &fn_elfhdr, 1);
      mod_addfn(cmds, "segments", "<file> - vector of the program headers "
                                           "in <file>",
                &fn_elfphdrs, 1);

      mod_addfn(cmds, "rdsegs", "<ckfn> <fn> <file> - "
                "apply <ckfn> to header then "
                "<fn> to each <file> segment args <addr> <string>",
                &fn_elfrdsegs, 3);
      mod_addfn(cmds, "hdrtype", "<type> - description of header type",
                &fn_ehdr_type, 1);
      mod_addfn(cmds, "hdrflags", "<mc> <flags> - vector of hdr flag "
                                  "descriptions for machine <mc>",
                &fn_ehdr_flags, 2);
      mod_addfn(cmds, "segflags", "<flags> - vector of segment header "
                                  "flag descriptions",
                &fn_phdr_flags, 1);
   }

   return ok;
}




extern void
cmds_elf_end(parser_state_t *state)
{
    return;
}
