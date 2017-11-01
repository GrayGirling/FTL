#-----------------------------------------------------------------------------
#
# Default command handling
#
#-----------------------------------------------------------------------------


set unknown_command[line]:{
    printf "%s: unrecognized command line %v\n" <codeid, line>!;
    NULL
}

set parse.on_badcmd (cmd unknown_command "- unknown command handler"!)
