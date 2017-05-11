CC=gcc

$(info Building for OSARCH $(OSARCH))

ARCH=$(OSARCH)
ifeq ($(OSARCH),Darwin)
    ARCH=osx
endif
ifeq ($(OSARCH),osx64)
    ARCH=osx
endif

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
ifneq ($(ARCH),cygwin64)

    LIBS_READLINE=-lreadline -lhistory
    DEFS_READLINE=-DUSE_READLINE
    INCS_READLINE=-I /usr/include/readline
    OBJS_READLINE=

    CFLAGS_CC=-Wint-conversion
else
    CC=mingw32-gcc
    LIB_DYNLIB=
    LIB_SOCKET=-lws2_32
endif
endif

DEFINES := $(DEFS_READLINE)
LIBS := $(LIBS_READLINE) $(LIB_DYNLIB) $(LIB_SOCKET)
INCLUDES := -I include $(INCS_READLINE)
CFLAGS_CC := $(CFLAGS_CC) -Wall $(CC_OPT_DEBUGSYMS)

CFLAGS := $(CFLAGS_CC) $(DEFINES) $(INCLUDES)

FTLEXTS := ftlext-test.so

FTLLIB_OBJS := libftl.o filenames.o libdyn.o $(OBJS_READLINE)
FTL_OBJS := ftl.o $(FTLLIB_OBJS)
PENV_OBJS := penv.o $(FTLLIB_OBJS)

vpath %.c tools:lib
vpath %.h include

#all:	ftl libs cscope
all:	ftl cscope

install: ftl
	cp ftl ~/cmd/$(OSARCH)/

%.so: %.c
	$(CC) $(CFLAGS) -o $@ -shared $<

ftlext-test.c: ftl.h ftl_internal.h ftlext.h 

libftl.c: ftl.h ftl_internal.h ftlext.h filenames.h libdyn.h Makefile

filenames.c: ftl.h filenames.h Makefile

libdyn.c: libdyn.h filenames.h Makefile

libs: $(FTLEXTS)

ftl: $(FTL_OBJS) ftl.h ftl_internal.h Makefile
	$(CC) $(CFLAGS) -o $@ $(FTL_OBJS) $(LIBS)

penv: $(PENV_OBJS) ftl.h ftl_internal.h Makefile
	$(CC) $(CFLAGS) -o $@ $(PENV_OBJS) $(LIBS)

ifeq ($(HAS_CSCOPE),1)
cscope:
	cscope -b -R -p3 lib/*.c include/*.h tools/*.c </dev/null
else
cscope:

endif

test:
	tests/check -a

clean:
	rm -f ftl penv $(FTL_OBJS) $(FTLEXTS)
