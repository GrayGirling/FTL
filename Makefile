CC=gcc

$(info Building for OSARCH $(OSARCH))

ifeq ($(OSARCH),Darwin)
    LIBS_READLINE=
    DEFS_READLINE=
    INCS_READLINE=
    #LIBS_READLINE=-lreadline -lhistory
    #DEFS_READLINE=-DUSE_READLINE
    #INCS_READLINE=-I /opt/local/include/readline -I /opt/local/include

    CFLAGS_CC=-Wno-tautological-compare
else
    LIBS_READLINE=-lreadline -lhistory
    DEFS_READLINE=-DUSE_READLINE
    INCS_READLINE=-I /usr/include/readline

    CFLAGS_CC=
endif

DEFINES=$(DEFS_READLINE)
LIBS=$(LIBS_READLINE) -ldl
INCLUDES=-I include $(INCS_READLINE)

CFLAGS=$(CFLAGS_CC) -g $(DEFINES) $(INCLUDES)

FTLEXTS = ftlext-test.so

FTLLIB_OBJS = libftl.o filenames.o libdyn.o
FTL_OBJS = ftl.o $(FTLLIB_OBJS)
PENV_OBJS = penv.o $(FTLLIB_OBJS)

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

cscope:
	cscope -b -R -p3 lib/*.c include/*.h tools/*.c </dev/null

clean:
	rm -f ftl penv $(FTL_OBJS) $(FTLEXTS)
