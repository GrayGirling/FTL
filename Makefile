CC=gcc
LIBS=-lreadline -lhistory -ldl
DEFINES=-DUSE_READLINE
INCLUDES=-I include -I /usr/include/readline
CFLAGS=-g $(DEFINES) $(INCLUDES)

FTLLIB_OBJS = libftl.o filenames.o libdyn.o
FTL_OBJS = ftl.o $(FTLLIB_OBJS)
PENV_OBJS = penv.o $(FTLLIB_OBJS)

vpath %.c tools:lib
vpath %.h include

all:	ftl cscope

install: ftl
	cp ftl ~/cmd/$(OSARCH)/

libftl.c: ftl.h ftl_internal.h filenames.h libdyn.h Makefile

filenames.c: ftl.h filenames.h Makefile

libdyn.c: libdyn.h filenames.h Makefile

ftl: $(FTL_OBJS) ftl.h ftl_internal.h Makefile
	$(CC) $(CFLAGS) -o $@ $(FTL_OBJS) $(LIBS)

penv: $(PENV_OBJS) ftl.h ftl_internal.h Makefile
	$(CC) $(CFLAGS) -o $@ $(PENV_OBJS) $(LIBS)

cscope:
	cscope -b -R -p3 lib/*.c include/*.h tools/*.c </dev/null

clean:
	rm -f ftl penv $(FTL_OBJS)
