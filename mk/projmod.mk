# This provides some support for referring to subtrees managed using the
# trivial module standard documented at
# http://confluence.broadcom.com/x/mZhtCg

# defines a function
#    $(call projmod-dir,<modulename>) - directory containing <modulename>
#                                       empty if DIR_PROJMOD not defined
#    $(call shell-escape,<command>)  - add escapes for shell execution string
#    $(call shell-filestr,<file>) - expands to the text of <file>
#
# and variables
#    shell-sep   - the character separating directory name from its
#                  subdirectoryname in a file occuring in a native shell
#                  executable

ifndef __projmod_included
__projmod_included := T

ifeq ($(strip $(words $(subst \,\ ,$(shell echo \\\\)))),4)
   shell-escape = $1
else
   shell-escape = $(subst \,\\,$1)
endif

ifeq ($(shell echo $$SHELL),$$SHELL) # distinguish windows from unix shells
   shell-sep := $(and \)
   shell-filestr = $(shell type $(call shell-escape,$1))
else
   shell-sep := /
   shell-filestr = $(shell cat $(call shell-escape,$1))
endif

projmod-dir = $(and $(DIR_PROJMOD),$(call shell-filestr,$(DIR_PROJMOD)$(shell-sep)$1$(shell-sep)dir.txt))

endif
