/*
 * Copyright (c) 2005-2009, Solarflare Communications Inc.
 * Copyright (c) 2014, Broadcom Inc.
 * Copyright (c) 2005-2021, Gray Girling
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
**  \brief  Furtle (FTL) legacy header
**   \date  2005/08
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef _FTL_LEGACY_H
#define _FTL_LEGACY_H

/* This header uses the current API as the basis for functions that were
   available in older versions of FTL.

   In new code it is best to avoid the use of these, now deprecated, functions.
*/
#include "ftl_api.h"


/*          Values                                       */

#define value_print_detail(out, root, val, detailed)        \
    value_state_print_detail(root_state,out,root,val,detailed)

#define value_print(out, root, val) \
    value_state_print_detail(root_state,out,root,val,/*detail*/FALSE)

#define value_fprint_detail(out, root, val, detailed) \
    value_state_fprint_detail(root_state,out,root,val,/*detail*/FALSE)
    
#define value_fprint(out, root, val) \
    value_fprint_detail(out,root,val,/*detail*/FALSE)

/*          _new and _lnew                                   */

/*   Allocated values are initially allocated on a "locals" list, which is used
 *   during garbage collection to prevent over zealous collection.
 *   Legacy calls are of the form
 *          value_<something>_new(args...)
 *   The correct local pool to use for a given thread is the one associated
 *   with the parser state, paser_locals(state), so most newer code should use
 *          value_<something>_lnew(state, args...)
 *   which will provide the correct local pool found in the state.
 *
 *  To allow asynchronous garbage collection these values should be 
 *  \c value_unlocal'ed once they go out of the scope in to which they were
 *  allocated.
 *
 *  Note: functions calling <something>_lnew and return it for the purpose of
 *  new value creation do not need to call \c value_unlocal().  Such functions
 *  will ideally use "_lnew" postfix in their name.
 *
 *  Values allocated using _lnew functions should not be free'd directly using
 *  either FTL_FREE() or free()
 *
 *  In terms of other parts of the code:
 *       functions and commands:
 *            - assume all inputs (for functions) are possibly re-used by
 *              the caller (so don't unlocal them)
 *            - when calling them always assume that the output is local
 *       parse functions creating a value (parse_<x>>)
 *            - assume the output value is local
 *       operators:
 *            - assume functions do not unlocal their arguments
 *            - assume returned values are local
 */


/*          Integer Values                                   */

#define value_int_new(n) value_int_lnew(root_state, n)

#define value_uint_new(n) value_uint_lnew(root_state, n)

#define value_int_update(ref_val, n) value_int_lupdate(root_state, ref_val, n)


/*          IP Address Values                                    */

#define value_ipaddr_new(ip) value_ipaddr_lnew(root_state, ip)

#define value_ipaddr_new_quad(a, b, c, d)               \
    value_ipaddr_quad_lnew(root_state, a, b, c, d)

#define parse_ipaddr(ref_line, out_ipaddr)                              \
    parsew_ipaddr(ref_line, &(*ref_line)[strlen(*(ref_line))], out_ipaddr)


/*           MAC Address Values                                  */

#define value_macaddr_new_sextet(a, b, c, d, e, f)                  \
    value_macaddr_sextet_lnew(root_state, a, b, c, d, e, f)

/*          Boolean Values                                   */


/*          String Values                                    */

#define value_string_new(string, len) value_string_lnew(root_state, string, len)

#define value_string_new_measured(_string) \
    value_string_new(_string, strlen(_string)) /*also deprecated*/

#define value_string_alloc_new(len, out_string) \
    value_string_alloc_lnew(root_state, len, out_string) 

#define value_wcstring_new(_wcstring, _wchars) \
    value_wcstring_lnew(state, _wcstring, _wchars)

#define value_cstring_new(_string, _len)                     \
    value_cstring_lnew(state, _string, _len)

#define value_cstring_new_measured(_string)      \
    value_cstring_lnew(root_state, _string, strlen(_string))

#define value_substring_new(string, offset, len) \
    value_substring_lnew(root_state, string, offset, len)

#define value_string_update(ref_value, str) \
    value_string_lupdate(root_state, ref_value, str)

#define value_string_get_terminated(value, out_buf, out_len) \
    value_string_nulterm_lnew(root_state, value, out_buf, out_len)

/*          Code body Values                                     */

#define value_code_new(string, sourcename, lineno) \
    value_code_lnew(root_state, string, sourcename, lineno)


/*          Stream Values                                                    */


/*          Filing System Stream Values                                      */

#define value_stream_openfile_new(file, autoclose, name, read, write) \
    value_stream_openfile_lnew(root_state, file, autoclose, name, read, write)

#define value_stream_file_new(name, binary, read, write) \
    value_stream_file_lnew(root_state, name, binary, read, write)

#define value_stream_file_path_new(path, name, namelen, binary, \
                                   read, write, namebuf, buflen) \
    value_stream_file_path_lnew(root_state, path, name, namelen, binary, \
                                read, write, namebuf, buflen)
    

/*          Socket Stream Values                                             */

#define value_stream_opensocket_new(fd, autoclose, name, read, write) \
    value_stream_opensocket_lnew(root_state, fd, autoclose, name, read, write)

#define value_stream_socket_connect_new(protocol, address, read, write) \
    value_stream_socket_connect_lnew(root_state, protocol, address, read, write)

#define value_stream_socket_listen_new(protocol, name, read, write) \
    value_stream_socket_listen_lnew(root_state, protocol, name, read, write)


/*          Socket Stream Values                                             */

#define value_stream_instring_new(name, string, len) \
    value_stream_instring_new(root_state, name, string, len)

#define value_stream_outstring_new() value_stream_outstring_lnew(root_state)

#define value_stream_outmem_new(str, len) \
    value_stream_outmem_lnew(root_state, str, len)


/*          Directories                                      */

#define dir_set(dir, name, value) dir_lset(dir, root_state, name, value)

#define dir_int_set(dir, name, value) \
    dir_int_lset(dir, root_state, name, value)

#define dir_string_set(dir, name, value) \
    dir_string_lset(dir, root_state, name, value)

#define dir_cstring_set(dir, name, value) \
    dir_cstring_lset(dir, root_state, name, value)

#define dir_forall(dir, enumfn, arg) \
        dir_state_forall(dir, root_state, enumfn, arg)

#define dir_count(dir) \
        dir_state_forall(dir, root_state)

#define dir_fprint(out,root,dir) dir_state_fprint(root_state,out,root,dir)
    
#define DIR_SHOW_RT(msg, root, dir) \
        DIR_STATE_SHOW_RT(root_state, msg, root, dir)

#define DIR_SHOW(msg, dir) \
        DIR_STATE_SHOW_DR(root_state, msg, NULL, dir)


/*          Identifier Directories                               */

#define dir_id_new() dir_id_lnew(root_state) /* deprecated */

/*          Integer vector Directories                               */

#define dir_vec_new() dir_vec_lnew(root_state) /* deprecated */

/*          Composed Directories                                 */

#define dir_join_new(index_dir, value_dir) \
    dir_join_lnew(root_state, index_dir, value_dir)


/*          Field of Array/Structure Definition                          */


/*          Structure Directories                                */

#define dir_struct_cast_new(spec, is_const, ref, ref_struct) \
    dir_struct_cast_lnew(root_state, spec, is_const, ref, ref_struct)

#define dir_vars(spec, const, ref) dir_vars_lnew(root_state, spec, const, ref)

#define dir_cstruct_new(spec, is_const, static_struct) \
    dir_cstruct_lnew(root_state, spec, is_const, static_struct)

#define dir_struct_update_new(ref_value, spec, is_const, structmem)     \
    dir_struct_update_lnew(root_state, ref_value, spec, is_const, structmem)


/*          Array Directories                                */

#define dir_carray_new(spec, is_const, static_array, stride) \
    dir_carray_lnew(root_state, spec, is_const, static_array, stride)

#define dir_array_new(spec, is_const, malloc_array, stride) \
    dir_array_lnew(root_state, spec, is_const, malloc_array, stride)

#define dir_array_cast(spec, is_const, ref, ref_array, stride)         \
    dir_array_cast_lnew(root_state, spec, is_const, ref, ref_array, stride)

#define dir_array_string(spec, is_const, string, stride)                \
    dir_array_string_lnew(root_state, spec, is_const, string, stride)

#define dir_array_update(ref_value, spec, is_const, arraymem, stride) \
    dir_array_update_lnew(root_state, ref_value, spec, is_const, \
                          arraymem, stride)

/*          String Argument vector Directories                           */

#define dir_argvec_new(argc, argv) dir_argvec_lnew(root_state, argc, argv)

/*          Integer Series Directories                               */

#define dir_series_new(first, inc, last) \
    dir_series_lnew(root_state, first, inc, last)

/*          System Env Directories                               */

#define dir_sysenv_new() dir_sysenv_lnew(root_state) 


/*          Stacked Directory Directories                                */

#define dir_state_new() dir_state_lnew(root_state)

extern dir_stack_t *
dir_stack_copy_lnew(parser_state_t *state, dir_stack_t *old);

/*! Deprecated: Legacy use only - don't use in new code */
#define dir_stack_copy(old) dir_stack_copy_lnew(root_state, old)



/*          Closure Environments                                 */

#define value_env_new() value_env_lnew(root_state)

#define value_env_newpushdir(newdir, env_end, unbound) \
    value_env_pushdir_lnew(root_state, newdir, unbound)

#define value_env_pushenv(env, newenv, env_end) \
        value_env_pushenvdir(root_state, env, newenv, env_end)

#define value_env_bind(envval, value)                   \
    value_env_bind_lnew(root_state, envval, value) 



/*          Closure Values                                   */

#define value_closure_new(code, env) value_closure_lnew(root_state, code, env)

#define value_closure_fn_new(code, env, autorun) \
    value_closure_fn_lnew(root_state, code, env, autorun)

#define value_closure_pushenv(value, env, env_end) \
    value_closure_spushenv(root_state, value, env, env_end) 

#define value_closure_bind(envval, value) \
    value_closure_bind_lnew(root_state, envval, value)


/*          Transfer Functions                               */


/*          Coroutine Values                                 */

#define value_coroutine_new(root, env, opdefs) \
    value_coroutine_lnew(root_state, root, env, opdefs)


/*          Parser State                                 */

#define parser_state_new(root) parser_state_lnew(root_state, root)


/*          Type Values                                      */


/*          Handle Types and Values                                          */

#define value_handle_new(handle, handle_type, closefn, autoclose) \
    value_handle_lnew(root_state, handle, handle_type, closefn, autoclose)


/*          Dynamic Directories                                 */



/*          Line Parsing                             */


#define parse_prefix(ref_line, prefix, len) \
    parsew_prefix(ref_line, &(*ref_line)[strlen(*(ref_line))]prefix, len)

#define parse_ending(ref_line, key) \
    parsew_ending(ref_line, strlen(*ref_line), key, strlen(key))

#define parse_hex(ref_line, out_int) \
    parsew_hex(ref_line, &(*ref_line)[strlen(*(ref_line))], out_int)

#define parse_octal(ref_line, out_int) \
    parsew_octal(ref_line, &(*ref_line)[strlen(*(ref_line))], out_int)

#define parse_hex_width(ref_line, width, out_int) \
    parsew_hex_width(ref_line, &(*ref_line)[strlen(*(ref_line))], \
                     width, out_int)

#define parse_int_val(ref_line, out_int) \
    parsew_int_val(ref_line, &(*ref_line)[strlen(*(ref_line))], out_int)

#define parse_int_expr(ref_line, state, out_int) \
    parsew_int_expr(ref_line, &(*ref_line)[strlen(*(ref_line))], out_int)

#define parse_item(ref_line, delims, ndelims, buf, len) \
    parsew_item(ref_line, &(*ref_line)[strlen(*(ref_line))], \
                delims, ndelims, buf, len)

#define parse_id(ref_line, buf, size) \
    parsew_id(ref_line, &(*ref_line)[strlen(*(ref_line))], buf, size)

#define parse_string(ref_line, buf, len, out_len) \
    parsew_string(ref_line, &(*ref_line)[strlen(*(ref_line))], \
                  buf, len, out_len)


#define parse_string_expr(ref_line, state, buf, len, out_string) \
    parsew_string_expr(ref_line, &(*ref_line)[strlen(*(ref_line))], state, \
                       buf, len, out_string)

#define parse_itemstr(ref_line, buf, size) \
    parsew_itemstr(ref_line, &(*ref_line)[strlen(*(ref_line))], buf, size)

#define parse_oneof(ref_line, state, prefixes, out_val) \
    parsew_oneof(ref_line, &(*ref_line)[strlen(*(ref_line))], state, \
                 prefixes, out_val)

#define parse_one_ending(ref_line, state, delims, out_val) \
    parsew_one_ending(ref_line, &(*ref_line)[strlen(*(ref_line))], state, \
                      delims, out_val)


/*          Command Values                               */

#define value_cmd_new(exec, fn_exec, help) \
         value_cmd_lnew(root_state, exec, fn_exec, help) 


/*          Function Values                              */

#define value_func_new(exec, help, args, implicit) \
        value_func_lnew(root_state, exec, help, args, implicit)

/*          Memory                                       */

#define value_mem_bin_new(binstr, base, sole_user, readonly) \
    value_mem_bin_lnew(root_state, binstr, base, sole_user, readonly)

#define value_mem_bin_alloc_new(base, len, memset_val, out_block) \
    value_mem_bin_alloc_lnew(root_state, base, len, memset_val, out_block)


#define value_mem_rebase_new(unbase_mem_val, base, readonly, sole_user) \
    value_mem_rebase_lnew(root_state, unbase_mem_val, base, readonly, sole_user)



/*          Modules                                          */

#define mod_add(dir, name, help, exec) \
        smod_add(root_state, dir, name, help, exec)
#define mod_addfn(dir, name, help, exec, args) \
        smod_addfn(root_state, dir, name, help, exec, args)
#define mod_addfn_imp(dir, name, help, exec, args, implicit_args) \
        smod_addfn_imp(root_state, dir, name, help, exec, args, implicit_args)
#define mod_add_dir(dir, name, mod) \
        smod_add_dir(root_state, dir, name, mod)
#define mod_add_val(dir, name, val) \
        smod_add_val(root_state, dir, name, val)

#define mod_parse_cmd(dir, ref_line, out_cmd) \
    mod_parsew_cmd(dir, ref_line, &(*ref_line)[strlen(*(ref_line))], out_cmd)


/*          Value Parsing                            */

#define parse_code(ref_line, state, out_strval, out_sourcepos, out_lineno) \
    parsew_code(ref_line, &(*ref_line)[strlen(*(ref_line))], state, \
                out_strval, out_sourcepos, out_lineno)

#define parse_retrieval(ref_line, state, out_val) \
    parse_retrieval(ref_line, &(*ref_line)[strlen(*(ref_line))], state, out_val)

#define parse_expr(ref_line, state, out_val) \
    parsew_expr(ref_line, &(*ref_line)[strlen(*(ref_line))], state, out_val)


/*          Command Line Interpreter                         */

#define mod_exec_cmd(ref_line, state) \
    mod_exec_cmdw(ref_line, &(*ref_line)[strlen(*(ref_line))], state)



/*          Generic Commands                                 */


/*          Library initialization                           */


#ifdef __cplusplus
}
#endif



#endif /* _FTL_LEGACY_H */
