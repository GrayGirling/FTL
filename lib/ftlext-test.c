/*! @file: functions for handling calls into dynamic libraries */


/******************************************************************************
 *
 *          Headers
 *
 *****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "ftlext.h"
#include "ftl_internal.h"


/******************************************************************************
 *
 *          O/S Error Reporting Abstraction
 *
 *****************************************************************************/



extern bool FTL_EXTENSION_MAIN(parser_state_t *state, dir_t *cmds)
{
    mod_add_val(cmds, "version", value_int_new(42));
}

