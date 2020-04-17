CC:=gcc


# CONFIGURATION (these can be specified on the 'make' command line)

ftl2=no
use_elf=yes
use_xml=yes
use_json=yes
elf_lib_type=ELF
force_native=no
ndebug=no

# Hint: if you are using windows 10 under WSL and you want to buld a Windows
#       (non-WSL) executable try
#          make use_elf=no force_native=yes
#       To test the compiler system try
#          make force_native=yes hi
#          cp hi.exe /mnt/c/TEMP
#          /mnt/c/TEMP/hi.exe
#       (Native binaries have to execute from a Windows file system)
#       Warning running windows binaries from the bash command line separates
#          output sent to stdout and to stderr - so errors are reported very
#          late
#          Advice: Run native programs in a native shell, not a *nix one

# To make commands that run under WSL (but not natively) use
#          make use_elf=no force_native=no
# To make commands under WSL that run natively under windows use
#          make use_elf=no force_native=yes
# (On Ubuntu this may require package gcc-mingw-w64)


# OS CUSTOMIZATION

ARCH=$(OSARCH)
ifeq ($(OSARCH),Darwin)
    ARCH=osx
endif
ifeq ($(OSARCH),osx64)
    ARCH=osx
endif
ifeq ($(OSARCH),linux64)
    ARCH=linux
    KERNEL=$(shell uname -r)
    KERNEL_NONMS=$(KERNEL:-Microsoft=)
    #$(info KERNEL=$(KERNEL) KERNEL_NONMS=$(KERNEL_NONMS))
    ifneq ($(KERNEL),$(KERNEL_NONMS))
        #$(info Windows WSL Kernel detected)
        ARCH=winlinux
    endif
endif
ifeq ($(ARCH),cygwin64)
    ARCH=cygwin
endif

INSTALL_DIR=$(HOME)/cmd/$(OSARCH)

NATIVE:=yes
NATIVEARCH:=$(ARCH)
ifeq ($(ARCH),cygwin)
    NATIVE:=no
    NATIVEARCH:=windows
    NATIVE_INSTALL_DIR:=/cygdrive/c/TEMP
endif
ifeq ($(ARCH),winlinux)
    # may need e.g. ubuntu package mingw-w64
    NATIVE:=no
    NATIVEARCH:=windows
    NATIVE_INSTALL_DIR:=/mnt/c/TEMP
endif

ifeq ($(force_native),yes)
    BUILDARCH=$(NATIVEARCH)
    INSTALL_DIR=$(NATIVE_INSTALL_DIR)
else
    BUILDARCH=$(ARCH)
endif

# LIBRARY SELECTION

HAS_CSCOPE=0
LIB_DYNLIB=-ldl
LIB_SOCKET=
CC_OPT_DEBUGSYMS=-O0 -g

LIBS_READLINE=
DEFS_READLINE=
INCS_READLINE=
OBJS_READLINE=

LIBS_LIBELF=-lelf
DEFS_LIBELF=
INCS_LIBELF=
OBJS_LIBELF=

EXE :=
OBJ := .o

ifeq ($(BUILDARCH),osx)
    LIBS_READLINE=
    DEFS_READLINE=-DUSE_LINENOISE
    INCS_READLINE=
    OBJS_READLINE=linenoise$(OBJ)
    #LIBS_READLINE=-lreadline -lhistory
    #DEFS_READLINE=-DUSE_READLINE
    #INCS_READLINE=-I /opt/local/include/readline -I /opt/local/include
    #OBJS_READLINE=
    INCS_LIBELF=-I /opt/local/include/libelf -I /opt/local/include
    LIBS_LIBELF=-L /opt/local/lib -lelf

    CC_OPT_DEBUGSYMS=-O0 -g3 
    CFLAGS_CC=-Wno-tautological-compare
else
ifeq ($(BUILDARCH),windows)
    ifeq ($(NATIVE),no)
        EXE:=.exe
        OBJ:=.obj
        # CC:=mingw32-gcc
	LIBS_READLINE=
        #DEFS_READLINE=-DUSE_LINENOISE
	INCS_READLINE=
        #OBJS_READLINE=linenoise$(OBJ)
	LIB_DYNLIB=
	LIB_SOCKET=-lws2_32
        #CC:=i686-w64-mingw32-gcc# for 32-bit executable
	CC:=x86_64-w64-mingw32-gcc# for 64-bit executable
    endif
else
    # assume posix-like native environmnent
    LIBS_READLINE=-lreadline -lhistory
    DEFS_READLINE=-DUSE_READLINE
    INCS_READLINE=-I /usr/include/readline
    OBJS_READLINE=

    #CFLAGS_CC=-Wint-conversion
endif
endif

$(info Building for OSARCH $(OSARCH) using $(CC) (building for $(BUILDARCH)))

# default build target
#all:	ftl$(FTLVER) libs cscope
all:	hi$(EXE) ftl$(FTLVER)$(EXE) cscope

help:
	@echo "Makefile arguments: "
	@echo "        ftl2=yes/no"
	@echo "        use_xml=yes/no (default $(use_xml))"
	@echo "        use_json=yes/no (default $(use_json)"
	@echo "        use_elf =yes/no (default $(use_elf)"
	@echo "        elf_lib_type=ELF/GELF (default $(elf_lib_type)"
	@echo "        force_native=yes/no (default $force_native)"
	@echo "        ndebug=yes/no default $(ndebug)"
	@echo
	@echo "  ftl2         - parse ftl2 (call closures when last arg supplied)"
	@echo "  use_xml      - include commands for XML parsing"
	@echo "  use_json     - include commands for JSON parsing"
	@echo "  use_elf      - include commands for parsing ELF files"
	@echo "  elf_lib_type - the name of the ELF API to use"
	@echo "  force_native - (on win WSL) generate WIN32 code not Linux code"
	@echo "  ndebug       - don't generate debuggable binaries"
	@echo
	@echo "Makefile targets: "
	@echo "        all - makes all buildable objects"
	@echo "        clean - cleans current build"
	@echo "        install - make and copy result into installation dir "
	@echo "        test - run built in tests"
	@echo "        docs - convert primary docs to markdown (for github)"
	@echo "        help - prints this text"

DEFINES  := 
LIBS     := $(LIBS_READLINE) $(LIB_DYNLIB) $(LIB_SOCKET) $(LIB_ELF)
INCLUDES := -I include
ifeq ($(ndebug),yes)
    CC_OPT_DEBUGSYMS :=
endif
CFLAGS   := $(CFLAGS_CC) -Wall $(CC_OPT_DEBUGSYMS) $(INCLUDES)
LIBFTL_DEFS := $(DEFINES) $(DEFS_READLINE)

# DYNAMIC FTL ENVIRONMENTS

# FTLEXTS := ftlext-test.so

# TOOL CONSTRUCTION

FTLVER := 

ifeq ($(ftl2),yes)
LIBFTL_DEFS += -DFTL_AUTORUN
FTLVER := 2
endif
LIBFTL_OBJS := libftl$(FTLVER)$(OBJ) filenames$(OBJ) libdyn$(OBJ)  $(OBJS_READLINE)

FTL := ./ftl
FTL_DEFS :=
FTL_OBJS := ftl$(OBJ) $(LIBFTL_OBJS) 
FTL_LIBS := $(LIBS)

ifeq ($(ftl2),yes)
endif

PENV_DEFS := 
PENV_OBJS := penv$(OBJ) $(LIBFTL_OBJS)
PENV_LIBS := $(LIBS)

HI_OBJS := hi$(OBJ)

ifeq ($(use_elf),yes)
FTL_OBJS += libftl_elf$(OBJ)
LIBELF_DEFS = -DUSE_LIB_$(elf_lib_type)
LIBELF_INCS = $(INCS_LIBELF)
FTL_DEFS += -DUSE_FTLLIB_ELF 
FTL_LIBS += $(LIBS_LIBELF)
endif

ifeq ($(use_xml),yes)
FTL_OBJS += libftl_xml$(OBJ)
FTL_DEFS += -DUSE_FTLLIB_XML
endif

ifeq ($(use_json),yes)
FTL_OBJS += libftl_json$(OBJ)
FTL_DEFS += -DUSE_FTLLIB_JSON
endif

vpath %.c tools:lib
vpath %.h include


install: ftl$(FTLVER)$(EXE) 
	cp ftl$(FTLVER)$(EXE) "$(INSTALL_DIR)"

install-hi: hi$(EXE) 
	cp hi$(EXE) "$(INSTALL_DIR)"

%.so: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -shared $<

%$(OBJ): %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

tools/%.str: tools/%.ftl
	$(FTL) -np -q -- io filetostring $< > $@

libftl$(FTLVER)$(OBJ): libftl.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LIBFTL_DEFS) $(LIBFTL_INCS) -c -o $@ $<

libftl_elf$(OBJ): libftl_elf.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LIBELF_DEFS) $(LIBELF_INCS) -c -o $@ $<

libdyn$(OBJ): libdyn.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

filenames$(OBJ): filenames.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

penv$(OBJ): penv.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(PENV_DEFS) $(PENV_INCS) -c -o $@ $<

ftl$(OBJ): ftl.c tools/ftl_fns.str
	$(CC) $(CPPFLAGS) $(CFLAGS) $(FTL_DEFS) $(FTL_INCS) -c -o $@ $<

hi$(OBJ): hi.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

ftlext-test.c: ftl.h ftl_internal.h ftlext.h 
libftl_elf.c: ftl.h ftl_internal.h ftl_elf.h
libftl_xml.c: ftl.h ftl_internal.h ftl_xml.h
libftl_json.c: ftl.h ftl_internal.h ftl_json.h
libftl.c: ftl.h ftl_internal.h ftlext.h filenames.h libdyn.h Makefile
filenames.c: ftl.h filenames.h Makefile
libdyn.c: libdyn.h filenames.h Makefile

libs: $(FTLEXTS)

ftl$(FTLVER)$(EXE): $(FTL_OBJS) ftl.h ftl_internal.h Makefile 
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(FTL_OBJS) $(FTL_LIBS)

penv$(FTLVER)$(EXE): $(PENV_OBJS) ftl.h ftl_internal.h Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(PENV_OBJS) $(PENV_LIBS)

hi$(EXE): $(HI_OBJS) Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(HI_OBJS)

ifeq ($(HAS_CSCOPE),1)
cscope:
	cscope -b -R -p3 lib/*.c include/*.h tools/*.c </dev/null
else
cscope:

endif

test:
	tests/check -a

docs:
	make -C doc

clean:
	rm -f ftl$(FTLVER) penv$(FTLVER) hi$(FTLVER) $(FTL_OBJS) $(PENV_OBJS) $(HI_OBJS) $(FTLEXTS)
