sys help
#run <line> - execute system <line>
#uid <user> - return the UID of the named user
#time - system calendar time in seconds
#localtime <time> - broken down local time
#utctime <time> - broken down UTC time
#localtimef <format> <time> - formatted local time
#utctimef <format> <time> - formatted UTC time
enter [ e=echo, h=help, x=exit, o=1, t=2 ]
help
#e <whole line> - prints the line out
#h [all] [<cmd>] - prints information about commands
#x - abandon all command inputs
help all
#e <whole line> - prints the line out
#h [all] [<cmd>] - prints information about commands
#x - abandon all command inputs
#o - int value
#t - int value
help exit
#exit - abandon all command inputs
sys help time
#time - system calendar time in seconds
help sys.time
#sys.time - system calendar time in seconds
enter [fn = [_help="- my function"]:{echo _help!}, env = [x="variable"], var = "variable" ]
help
#fn - my function
#env help - show subcommands
help all
#fn - my function
#env <subcommand> - commands:
#    x - string value
#var - string value
env help
env help all
#x - string value
