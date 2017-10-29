/*! @file  ftl_elf.h
 *  @brief FTL library to provide commands to provide ELF operations
 */

#ifndef _DRIVER_FTL_ELFLIB_H
#define _DRIVER_FTL_ELFLIB_H

#include "ftl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool
cmds_elf(parser_state_t *state, dir_t *cmds);

extern void
cmds_elf_end(parser_state_t *state);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_FTL_ELFLIB_H */
