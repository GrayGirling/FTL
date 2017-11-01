CC=gcc


# CONFIGURATION (these can be specified on the 'make' command line)

add_elf=yes
add_xml=no
elf_lib_type=ELF

# OS CUSTOMIZATION

$(info Building for OSARCH $(OSARCH))

ARCH=$(OSARCH)
ifeq ($(OSARCH),Darwin)
    ARCH=osx
endif
ifeq ($(OSARCH),osx64)
    ARCH=osx
endif
ifeq ($(OSARCH),linux64)
    ARCH=linux
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

ifeq ($(ARCH),osx)
    LIBS_READLINE=
    DEFS_READLINE=-DUSE_LINENOISE
    INCS_READLINE=
    OBJS_READLINE=linenoise.o
    #LIBS_READLINE=-lreadline -lhistory
    #DEFS_READLINE=-DUSE_READLINE
    #INCS_READLINE=-I /opt/local/include/readline -I /opt/local/include
    #OBJS_READLINE=

    CC_OPT_DEBUGSYMS=-O0 -g3 
    CFLAGS_CC=-Wno-tautological-compare
else
ifeq ($(ARCH),cygwin64)
    # CC=mingw32-gcc
    LIBS_READLINE=
    #DEFS_READLINE=-DUSE_LINENOISE
    INCS_READLINE=
    #OBJS_READLINE=linenoise.o
    CC=x86_64-w64-mingw32-gcc
    LIB_DYNLIB=
    LIB_SOCKET=-lws2_32
else
    LIBS_READLINE=-lreadline -lhistory
    DEFS_READLINE=-DUSE_READLINE
    INCS_READLINE=-I /usr/include/readline
    OBJS_READLINE=

    #CFLAGS_CC=-Wint-conversion
endif
endif

DEFINES  := 
LIBS     := $(LIBS_READLINE) $(LIB_DYNLIB) $(LIB_SOCKET)
INCLUDES := -I include
CFLAGS   := $(CFLAGS_CC) -Wall $(CC_OPT_DEBUGSYMS) $(INCLUDES)

LIBFTL_DEFS = $(DEFINES) $(DEFS_READLINE)
LIBFTL_INCS = $(INCLUDES) $(INCS_READLINE)

# DYNAMIC FTL ENVIRONMENTS

FTLEXTS := ftlext-test.so

# TOOL CONSTRUCTION

FTLLIB_OBJS := libftl.o filenames.o libdyn.o $(OBJS_READLINE)

FTL_DEFS := 
FTL_OBJS := ftl.o $(FTLLIB_OBJS)
FTL_LIBS := $(LIBS)

PENV_DEFS := 
PENV_OBJS := penv.o $(FTLLIB_OBJS)
PENV_LIBS := $(LIBS)

ifeq ($(add_elf),yes)
FTL_OBJS += libftl_elf.o
LIBELF_DEFS = -DUSE_LIB_$(elf_lib_type)
FTL_DEFS += -DUSE_FTLLIB_ELF 
FTL_LIBS += -lelf
endif

ifeq ($(add_xml),yes)
FTL_OBJS += libftl_xml.o
FTL_DEFS += -DUSE_FTLLIB_XML
endif

vpath %.c tools:lib
vpath %.h include

#all:	ftl libs cscope
all:	ftl cscope

install: ftl
	cp ftl ~/cmd/$(OSARCH)/

%.so: %.c
	$(CC) $(CFLAGS) -o $@ -shared $<

libftl.o: libftl.c
	$(CC) $(CFLAGS) $(LIBFTL_DEFS) $(LIBFTL_INCS) -c -o $@ $<

libftl_elf.o: libftl_elf.c
	$(CC) $(CFLAGS) $(LIBELF_DEFS) -c -o $@ $<

penv.o: penv.c
	$(CC) $(CFLAGS) $(PENV_DEFS) $(PENV_INCS) -c -o $@ $<

ftl.o: ftl.c
	$(CC) $(CFLAGS) $(FTL_DEFS) $(FTL_INCS) -c -o $@ $<

ftlext-test.c: ftl.h ftl_internal.h ftlext.h 

libftl_elf.c: ftl.h ftl_internal.h ftl_elf.h

libftl_xml.c: ftl.h ftl_internal.h ftl_xml.h

libftl.c: ftl.h ftl_internal.h ftlext.h filenames.h libdyn.h Makefile

filenames.c: ftl.h filenames.h Makefile

libdyn.c: libdyn.h filenames.h Makefile

libs: $(FTLEXTS)

ftl: $(FTL_OBJS) ftl.h ftl_internal.h Makefile
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
