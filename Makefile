CC=gcc
LIBS=-lreadline -lhistory
DEFINES=-DSTANDALONE -DUSE_READLINE
INCLUDES=-I include -I /usr/include/readline
CFLAGS=-g $(DEFINES) $(INCLUDES)

FTL_OBJS = ftl.o libftl.o 

vpath %.c tools:lib
vpath %.h include

all:	ftl cscope

install: ftl
	cp ftl ~/cmd/$(OSARCH)/

ftl: $(FTL_OBJS) ftl.h ftl_internal.h
	$(CC) $(CFLAGS) -o $@ $(FTL_OBJS) $(LIBS)

cscope:
	cscope -b -R -p3 lib/*.c include/*.h tools/*.c </dev/null

clean:
	rm -f ftl $(FTL_OBJS)
