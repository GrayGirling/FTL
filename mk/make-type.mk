# This makefile tries to characterize the environment in which it is being run
# (unfortunately some functions it uses may be specific to gnu make)
# It ensures the following variables are set
#    OS - operating system e.g. Linux or Windows_NT
#    shell-style - the kind of commands that are executed by $(shell ...)
#         windows - CMD commands
#         unix - bash commands
#    filename-style - the kind of file names that make rules expect natively
#         windows - with device: and \ file name section separator 
#         unix - with / file name section separator
#    sep - the separator string used for files in make rules used between
#          directory name and file name
#    shell-escapes - whether shell commands need to have \ escaped
#         escaped - \ must be escaped
#         unescaped - \ need not be be escaped
#    shell-redir-escapes - whether shell commands need to have \ escaped
#                          again when their output is redirected!
#         escaped - \ must be escaped again
#         unescaped - \ need not be be escaped again
#
# and these functions
#    $(call files-esc, text-including-file-names)
#        perform any necessary escaping of single \ to \\ in
#        text-including-file-names
#    $(call shell-esc, command-line)
#        perform any necessary escaping of single \ to \\ in
#        command-line for submission to the shell
#    $(call shell-escquote, command-line)
#        perform any necessary escaping of a quote for submission to the shell
#    $(call shell-redir-esc, command-line)
#        perform any necessary escaping of single \ to \\ in
#        command-line for submission to the shell when the
#        shell output is redirected

ifndef __make-type_included
__make-type_included := T

ifeq ($(OS),)
    OS := Linux
endif

ifeq ($(OS),Windows_NT)
    # even on Windows it is still possible for the Make shell to be either a
    # unix-style shell or a windows-style shell - this test is supposed to help
    # decide: $(info Shell test string '$(shell echo $$SHELL)')
    ifeq ($(shell echo $$SHELL),$$SHELL)
	shell-style := windows
    else
	shell-style := unix
    endif

    # if our dir name has : in it we're using Windows file names, else Unix
    ifeq (:,$(findstring :,$(realpath .)))
        filename-style := windows
        # strangeness because make uses \ as end-of-line escape
        sep := $(and /)
        os-files-esc = $1
    else
        filename-style := unix
        sep := /
        os-files-esc = $(subst ",\",$(subst \,\\,$1))
    endif
endif

ifeq ($(OS),Linux)
    filename-style := unix
    shell-style := unix
    sep := /
    os-files-esc = $1
endif

_bs-count = $(strip $(words $(subst \,\ ,$1)))

# 'echo' exists in both Windows and Unix
ifeq ($(call _bs-count,////),4)
    shell-escapes := unescaped
    shell-esc = $1
else
    shell-escapes := escaped
    shell-esc = $(subst \,\\,$1)
endif

# Quotes need escaping differently in different shells
ifeq ("", $(shell echo "\\"\\""))
   shell-escquote = $(subst ",\\",$1)
else
   shell-escquote = $(subst ",\",$1)
endif

files-esc = $(call shell-escquote,$(subst \,\\,$1))

# Perversely there is sometimes an extra level of escapes in redirections
# 'more' exists in both Windows and Unix
ifeq ($(call _bs-count,$(shell $(call shell-esc,echo \\\\) > _tmp.tmp; more _tmp.tmp)),4)
    shell-redir-escapes = unescaped
    shell-redir-esc = $(call shell-esc,$1)
else
    shell-redir-escapes = escaped
    shell-redir-esc = $(call shell-escquote,$(call shell-esc,$(call shell-esc,$1)))
endif

# $(info additional escaping for shell redirections - $(shell-redir-escapes)) 

endif
