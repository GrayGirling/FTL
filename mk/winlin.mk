# Makefile setup to abstract differences between Linux and Windows 

# This makefile uses GCC make syntax, but it can be parsed on windows by a
# native Windows32 version of gmake, a cygwin version of make; or, on a unix
# machine by a normal make.  In addition the make file should be able to be
# used both from the environment provided by a standard windows cmd.exe or a
# windows-native or a cygwin bash.

# If the make variable NATIVEWORD is not set to 'yes' compilations will be in
# for a 32-bit instruction set, even on 64-bit hosts.

this_mk := $(lastword $(MAKEFILE_LIST))
dir_mk := $(dir $(this_mk))

include $(dir_mk)make-type.mk

ifeq ($(OS),Windows_NT)
    # we may update this later in the makefile
    need-unix-shell := false
    ifeq ($(CC),cc)
       CC := cl
    endif
    OS_DEFINES := -DWIN32=1
endif

ifeq ($(OS),Linux)
    ifeq ($(CC),cc)
       CC := gcc
    endif
    OS_DEFINES := -DLINUX=1
endif

# $(info OS is $(OS), filename-style is $(filename-style), CC is $(CC))
empty:=
space:= $(empty) $(empty)

vcvars := $(dir_mk)vcvars

winfile = $(subst /,\,$1)

ifeq ($(filename-style),windows)
    ifeq ($(OS),Windows_NT)
        # put required cl compiler environment details in vcvars.mk
        # $(info run vcvars_to_cygwin.bat \> vcvars.mk)
	ignore := $(shell "$(dir_mk)vcvars_to_cygwin.bat" \> "$(dir_mk)vcvars.mk")
	CC-bin := bash "$(dir_mk)runcl.sh"
        LINK-bin := bash "$(dir_mk)runlink.sh"
	MKLIB-bin := bash "$(dir_mk)runlib.sh"
        winfile = $1
    endif
endif

ifeq ($(filename-style),unix)
    ifeq ($(OS),Windows_NT)
        # put required cl compiler environment details in vcvars.mk
        # $(info run "$(dir_mk)vcvars_to_cygwin.bat" \> "$(dir_mk)vcvars.mk")
        ignore := $(shell "$(dir_mk)vcvars_to_cygwin.bat" \> "$(dir_mk)vcvars.mk")
	CC-bin := "$(dir_mk)runcl.sh"
        LINK-bin := "$(dir_mk)runlink.sh"
	MKLIB-bin := "$(dir_mk)runlib.sh"
        winfile = $(if $(or $(findstring :,$1),$(findstring /cygdrive/,$1)),$(shell cygpath -w "$1"),$(subst /,\,$1))
    endif
    need-unix-shell = true
endif

ifeq ($(OS),Linux)
    utoos-file = $1
endif

ifeq ($(OS),Windows_NT)
    utoos-file = $(call winfile,$1)
    ifeq ($(shell-style),windows)
        ignore := $(shell $(call shell-redir-esc,bash -c "sed 's/\\/\\\\/g' \< $(dir_mk)vcvars.mk \> $(dir_mk)vcvars.sh"))
    else
        ignore := $(shell $(call shell-redir-esc,sed 's/\/\\/g' < $(dir_mk)vcvars.mk > $(dir_mk)vcvars.sh))
    endif
endif

winlin_tmps := $(dir_mk)vcvars.mk $(dir_mk)vcvars.sh

$(info Make for $(OS) with $(filename-style)-style filenames & an $(shell-escapes) $(shell-style)-style shell)
# $(info ...shell redirection $(shell-redir-escapes) and shell quotes escaped with $(call shell-escquote,"))


# Setup:
#
# macros
#    ccfile - make file name suitable for the compiler $(CC)
#    compile-c - compile C source arguments $1 to $@
#    link-c - link object files and (compiler) arguments to unshared lib $@
#    link-c-dll - link object files and (compiler) arguments to shared lib $@
#    lib-dll - libraries needed to use DLLs

ifeq ($(OS),Linux)
    dll :=.so
    exe := 
    obj :=.o
    lib :=.a
else
    dll := .dll
    exe := .exe
    obj := .obj
    lib := .lib
endif

ifeq ($(findstring gcc,$(CC)),gcc)
    # setup for gcc
    CCDEBUG := -g -ggdb
    ifeq ($(NATIVEWORD),yes)
        CC-32-bit := 
    else
        CC-32-bit := -m32
    endif
    lib-dll := -ldl

    ccfile = $1
    picopt :=
    ifneq ($(findstring mingw,$(CC)),mingw)
        picopt := -fPIC
    endif
    compiler = $(CC) $(call files-esc,$(CC-32-bit) $(CCDEBUG) $(GLOBDEFINES) $(if $2,$2,-o $@) $1)
    compile-c = $(call compiler,$(picopt) $(CFLAGS) $1,$2)
    compile-cc = $(call compiler,-x c++ $(picopt) $(CCFLAGS) $1 -lstdc++,$2)
    #compile-link-c = $(call compile-c,$1 $2)
    #compile-link-cc = $(call compile-cc,$1 $(addprefix -l,$2))
    link-c = $(call compiler, $(LDFLAGS) $1)
    link-c-dll = $(call compiler,-shared $(LDFLAGS) $1)
    mklib-c = ar rcs $@ $1
    mkdepend-c = $(CC) -MM $(CFLAGS) $1 | sed 's,$(*F).o[ :],$(@:.d=.o) $@ :,' >$@
endif

ifeq ($(CC),cl)
    # setup for gcc
    CCDEBUG := 
    #libs-std = gportio.lib vfw32.lib user32.lib ws2_32.lib
    libs-std = Advapi32.lib ws2_32.lib vfw32.lib user32.lib 
    ifeq ($(NATIVEWORD),yes)
        CC-32-bit :=
        # TODO: set cltype to x86 or x64 depending on native word size
        ifeq (cltype,x64)
	    # e.g. found in c:\windows\syswow64\
	    libs-std = Advapi64.lib ws2_64.lib vfw64.lib user64.lib
        endif
    else
        # no flags used in windows - just use the correct compiler!
        CC-32-bit := -m32
    endif
    lib-dll := 

    ccfile = $(call winfile,$1)
    compiler = $(CC-bin) $(call files-esc,/nologo $(CCDEBUG) $(GLOBDEFINES) $1)
    linker = $(LINK-bin) $(call files-esc,/nologo $(LDDEBUG) $1)
    warnflags = /W3 -D_CRT_SECURE_NO_WARNINGS /we4013 /we4028 /we4029 /we4505
    compile-c = $(call compiler, $(if $2,$2,/Fo$(call ccfile,$@)) /TC $(warnflags) $(CFLAGS) $1)
    compile-cc = $(call compiler, $(if $2,$2,/Fo$(call ccfile,$@)) /TP $(warnflags) $(CCFLAGS) $1)
    #compile-link-c = $(call compiler, /Fe$(call ccfile,$@) /TC $(warnflags) $(CFLAGS) $1 $2)
    #compile-link-cc = $(call compiler, /Fe$(call ccfile,$@) /TP $(warnflags) $(CCFLAGS) $1 $2)
    link-c = $(call linker, $(LDFLAGS) /OUT:$(call ccfile,$@) $1 $(libs-std) $2)
    link-c-dll = $(call linker,$(LDFLAGS) /DLL /OUT:$(call ccfile,$@) $1 $(libs-std) $2)
    mklib-c = $(MKLIB-bin) /nologo $(call files-esc,$1 -OUT:$(call ccfile,$@))
    # not implemented
    mkdepend-c = touch $@
endif

ifeq ($(compile-c),)
   $(error Unknown compiler selected - CC='$(CC)')
endif

shell-inquotes = "$(call files-esc,$1)"

# We shouldn't use these - we can no longer use one application to both
# compile and link on all platforms.  In particular, hcl386 can't link code
# when it runs on recent versions of Linux (e.g. Ubuntu 13.04)
compile-link-c = $(info Compile-Link C unimplemented - $1) (exit 1)
compile-link-cc = $(info Compile-Link C++ unimplemented - $1) (exit 1)

make-msg-text = == [$1] ==
make-message = @echo "$(call make-msg-text,$1)"

# if [ ! -f $1 ]; then echo "$1 doesn't exist"; fi; \
#               if [ ! -f $2 ]; then echo "$2 doesn't exist"; fi; \
#               if ! diff $1 $2>/dev/null; then echo "$1 and $2 differ"; fi; \
#

# called using $(call install-file,<fromfile>,<tofile>)
install-file = \
        if [ -f $1 ] && ( [ ! -f $2 ] || ! diff $1 $2 >/dev/null ); then \
	    if [ ! -f $2 ] || [ -w $2 ]; then \
		cp $1 $2; \
		echo "$(call make-msg-text,INSTALLING $2)"; \
	    else \
		echo "$(call make-msg-text,CAN'T UPDATE $2 - have you checked it out?)"; \
	    fi \
	fi

# called using $(call installcheck-file,<tofile>)
installcheck-file =  \
        if [ -d "$1" ] || ( [ -f "$1" ] && [ ! -w "$1" ] ); then \
	    echo "$(call make-msg-text,UNINSTALLABLE $1 - have you checked it out?)"; \
	    exit 1; \
	fi

installcheck-files =  \
        if [ ! -r "$1" ]; then \
	    echo "$(call make-msg-text,UNINSTALLABLE $1 - not readable)"; \
	    exit 1; \
        elif [ -d "$2" ] || ( [ -f "$2" ] && [ ! -w "$2" ] && ! diff $1 $2 >/dev/null ); then \
	    echo "$(call make-msg-text,UNINSTALLABLE $2 - have you checked it out?)"; \
	    exit 1; \
	fi

# Prefix commands with $(show) to control their output depending on whether
# V=1 is set on make line
ifeq ($(V),)
   show = @
else
   show =
endif
