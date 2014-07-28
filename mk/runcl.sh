#!/bin/sh

# Run the microsoft 'cl' compiler giving the appropriate arguments on the
# command line

cmd="$0"
cmd_dir="`dirname -- "$cmd"`"
cmd_dir="`(cd "$cmd_dir">/dev/null;pwd)`"
cmd_name="`basename -- "$cmd"`"

infile="$cmd_dir/vcvars.sh"

# echo "source $infile">&2
if [ ! -r "$infile" ] || ! source "$infile"; then
    echo "$cmd_name: can't source '$infile' - has it been written?">&2
    exit 1
fi

if [ -z "$VC_PATH" ]; then
    echo "$cmd_name: empty VC_PATH - not set in vcvars.mk?">&2
    rc=1
elif vcpath=`cygpath -u -p "$VC_PATH"`; then
    # we have to have a fully qualified path name to cl because when we run it
    # we will have changed the "PATH" environment variable to a Windows path
    cmd_cl=`PATH="$PATH:$vcpath" which cl`
    PATH="$VC_PATH" INCLUDE="$VC_INCLUDE" LIB="$VC_LIB" "$cmd_cl" "$@"
    rc=$?
else
    echo "$cmd_name: failed to run 'cygpath'">&2
    rc=2
fi
exit $rc

# This script is designed to be called by the makefile in the mw_plugins
# subdirectory of //tools/metaware/MetaWare/VideoCore at a point after a
# file "vcvars.mk" has been created (this contains the definitions of
# VC_PATH, VC_INCLUDE and VC_LIB ... all constructed from information provided
# by the windows batch commands vsvars32.bat or vcvars32.bat)

# Beware that this is normally called on a bash command line but will be
# expecting windows-style file names among its arguments.  Because such names
# typically include the back-slash (\) character it is likely that you will
# have to take some care in quoting the arguments properly (e.g. using \\).

# History: originally this task was undertaken in-line in the Makefile, which
# used to 'include' the file vcvars.mk.  Unfortunately this won't work when
# VC_PATH contains a hash (#) character because Make will discard any of the
# content following that (C# and F# are the main culprits here)

