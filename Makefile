CC:=gcc


# CONFIGURATION (these can be specified on the 'make' command line)

add_elf=yes
add_xml=no
elf_lib_type=ELF
force_native=yes

# Hint: if you are using windows 10 under WSL and you want to buld a Windows
#       (non-WSL) executable try
#       make add_elf=no force_native=yes

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

NATIVE:=yes
NATIVEARCH:=$(ARCH)
ifeq ($(ARCH),cygwin)
    NATIVE:=no
    NATIVEARCH:=windows
endif
ifeq ($(ARCH),winlinux)
    # may need e.g. ubuntu package mingw-w64
    NATIVE:=no
    NATIVEARCH:=windows
endif

ifeq ($(force_native),yes)
    BUILDARCH=$(NATIVEARCH)
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

DEFINES  := 
LIBS     := $(LIBS_READLINE) $(LIB_DYNLIB) $(LIB_SOCKET) $(LIB_ELF)
INCLUDES := -I include
CFLAGS   := $(CFLAGS_CC) -Wall $(CC_OPT_DEBUGSYMS) $(INCLUDES)
LIBFTL_DEFS = $(DEFINES) $(DEFS_READLINE)
# DYNAMIC FTL ENVIRONMENTS

# FTLEXTS := ftlext-test.so

# TOOL CONSTRUCTION

FTLLIB_OBJS := libftl$(OBJ) filenames$(OBJ) libdyn$(OBJ) $(OBJS_READLINE)

FTL_DEFS := 
FTL_OBJS := ftl$(OBJ) $(FTLLIB_OBJS)
FTL_LIBS := $(LIBS)

PENV_DEFS := 
PENV_OBJS := penv$(OBJ) $(FTLLIB_OBJS)
PENV_LIBS := $(LIBS)

ifeq ($(add_elf),yes)
FTL_OBJS += libftl_elf$(OBJ)
LIBELF_DEFS = -DUSE_LIB_$(elf_lib_type)
LIBELF_INCS = $(INCS_LIBELF)
FTL_DEFS += -DUSE_FTLLIB_ELF 
FTL_LIBS += $(LIBS_LIBELF)
endif

ifeq ($(add_xml),yes)
FTL_OBJS += libftl_xml$(OBJ)
FTL_DEFS += -DUSE_FTLLIB_XML
endif

vpath %.c tools:lib
vpath %.h include

#all:	ftl libs cscope
all:	ftl$(EXE) cscope

install: ftl$(EXE)
	cp ftl$(EXE) ~/cmd/$(OSARCH)/

%.so: %.c
	$(CC) $(CFLAGS) -o $@ -shared $<

libftl$(OBJ): libftl.c
	$(CC) $(CFLAGS) $(LIBFTL_DEFS) $(LIBFTL_INCS) -c -o $@ $<

libftl_elf$(OBJ): libftl_elf.c
	$(CC) $(CFLAGS) $(LIBELF_DEFS) $(LIBELF_INCS) -c -o $@ $<

libdyn$(OBJ): libdyn.c
	$(CC) $(CFLAGS) -c -o $@ $<

filenames$(OBJ): filenames.c
	$(CC) $(CFLAGS) -c -o $@ $<

penv$(OBJ): penv.c
	$(CC) $(CFLAGS) $(PENV_DEFS) $(PENV_INCS) -c -o $@ $<

ftl$(OBJ): ftl.c
	$(CC) $(CFLAGS) $(FTL_DEFS) $(FTL_INCS) -c -o $@ $<

ftlext-test.c: ftl.h ftl_internal.h ftlext.h 

libftl_elf.c: ftl.h ftl_internal.h ftl_elf.h

libftl_xml.c: ftl.h ftl_internal.h ftl_xml.h

libftl.c: ftl.h ftl_internal.h ftlext.h filenames.h libdyn.h Makefile

filenames.c: ftl.h filenames.h Makefile

libdyn.c: libdyn.h filenames.h Makefile

libs: $(FTLEXTS)

ftl$(EXE): $(FTL_OBJS) ftl.h ftl_internal.h Makefile
	$(CC) $(CFLAGS) -o $@ $(FTL_OBJS) $(FTL_LIBS)

penv: $(PENV_OBJS) ftl.h ftl_internal.h Makefile
	$(CC) $(CFLAGS) -o $@ $(PENV_OBJS) $(PENV_LIBS)

ifeq ($(HAS_CSCOPE),1)
cscope:
	cscope -b -R -p3 lib/*.c include/*.h tools/*.c </dev/null
else
cscope:

endif

test:
	tests/check -a

clean:
	rm -f ftl penv $(FTL_OBJS) $(PENV_OBJS) $(FTLEXTS)
