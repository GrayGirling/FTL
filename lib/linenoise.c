/* linenoise.c -- VERSION 1.0.cgg
 *
 * Guerrilla line editing library against the idea that a line editing lib
 * needs to be 20,000 lines of C code.
 *
 * You can find the latest source code at:
 *
 *   http://github.com/antirez/linenoise
 *
 * Does a number of crazy assumptions that happen to be true in 99.9999% of
 * the 2010 UNIX computers around.
 *
 * ------------------------------------------------------------------------
 *
 * Copyright (c) 2010-2014, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2010-2013, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 * Modified by Gray Girling
 * Copyright (c) 2016-2019, Gray Girling <graygirling at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Modified by NXP
 * Copyright 2019 NXP
 *
 * ------------------------------------------------------------------------
 *
 * References:
 * - http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
 * - http://www.3waylabs.com/nw/WWW/products/wizcon/vt220.html
 *
 * Todo list:
 * - Filter bogus Ctrl+<char> combinations.
 * - Win32 support
 *
 * Bloat:
 * - History search like Ctrl+r in readline?
 *
 * List of escape sequences used by this program, we do everything just
 * with three sequences. In order to be so cheap we may have some
 * flickering effect with some slow terminal, but the lesser sequences
 * the more compatible.
 *
 * EL (Erase Line)
 *    Sequence: ESC [ n K
 *    Effect: if n is 0 or missing, clear from cursor to end of line
 *    Effect: if n is 1, clear from beginning of line to cursor
 *    Effect: if n is 2, clear entire line
 *
 * CUF (CUrsor Forward)
 *    Sequence: ESC [ n C
 *    Effect: moves cursor forward n chars
 *
 * CUB (CUrsor Backward)
 *    Sequence: ESC [ n D
 *    Effect: moves cursor backward n chars
 *
 * The following is used to get the terminal width if getting
 * the width with the TIOCGWINSZ ioctl fails
 *
 * DSR (Device Status Report)
 *    Sequence: ESC [ 6 n
 *    Effect: reports the current cusor position as ESC [ n ; m R
 *            where n is the row and m is the column
 *
 * When multi line mode is enabled, we also use an additional escape
 * sequence. However multi line editing is disabled by default.
 *
 * CUU (Cursor Up)
 *    Sequence: ESC [ n A
 *    Effect: moves cursor up of n chars.
 *
 * CUD (Cursor Down)
 *    Sequence: ESC [ n B
 *    Effect: moves cursor down of n chars.
 *
 * When linenoiseClearScreen() is called, two additional escape sequences
 * are used in order to clear the screen and position the cursor at home
 * position.
 *
 * CUP (Cursor position)
 *    Sequence: ESC [ H
 *    Effect: moves the cursor to upper left corner
 *
 * ED (Erase display)
 *    Sequence: ESC [ 2 J
 *    Effect: clear the whole screen
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>

#ifdef WIN32

#include <windows.h>
#include <io.h>

static DWORD term_mode_out = 0;
static DWORD term_mode_in = 0; /* input & output console modes to restore */

/* Windows POSIX'y funcitons */
#define snprintf _snprintf
#define strcasecmp _stricmp
#define strdup _strdup

#define LNDEBUG_LOG "lndebug.txt"

typedef HANDLE ttyio_t;  /* console I/O handle - a HANDLE */

static int tty_isatty(ttyio_t fd)
{
    return fd != NULL;
}
static int _tty_write(ttyio_t handle, const void *buf, size_t bytes)
{
    DWORD written = -1;

    if (WriteConsole(handle, buf, bytes, &written, NULL))
        return written;
    else
        return -1;
}
static int _tty_read(ttyio_t handle, void *buf, size_t bytes)
{
    DWORD read = -1;

    if (ReadConsole(handle, buf, bytes, &read, NULL))
        return read;
    else
        return -1;
}
static ttyio_t tty_stdin(oid)
{
    return GetStdHandle(STD_INPUT_HANDLE);
}
static ttyio_t tty_stdout(void)
{
    return GetStdHandle(STD_OUTPUT_HANDLE);
}

#else

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

static struct termios orig_termios; /* In order to restore at exit.*/

#define LNDEBUG_LOG "/tmp/lndebug.txt"

typedef int ttyio_t;  /* console I/O handle - a file descriptor */

#define tty_isatty(hand) (isatty(hand))
#define _tty_write(hand, buf, len) write(hand, buf, len)
#define _tty_read(hand, buf, len)  read(hand, buf, len)
#define tty_stdin() (STDIN_FILENO)
#define tty_stdout() (STDOUT_FILENO)

#endif

#include "linenoise.h"

#define LINENOISE_DEFAULT_HISTORY_MAX_LEN 100
#define LINENOISE_MAX_LINE 4096
static char *unsupported_term[] = {"dumb","cons25","emacs",NULL};
static linenoiseCompletionCallback *completionCallback = NULL;

static int rawmode = 0; /* For atexit() function to check if restore is needed*/
static int mlmode = 0;  /* Multi line mode. Default is single line. */
static int atexit_registered = 0; /* Register atexit just 1 time. */
static int history_max_len = LINENOISE_DEFAULT_HISTORY_MAX_LEN;
static int history_len = 0;
static char **history = NULL;

/* The linenoiseState structure represents the state during line editing.
 * We pass this state to functions implementing specific editing
 * functionalities. */
struct linenoiseState {
    ttyio_t ifd;        /* Terminal input handle. */
    ttyio_t ofd;        /* Terminal output handle. */
    char *buf;          /* Edited line buffer. */
    size_t buflen;      /* Edited line buffer size. */
    const char *prompt; /* Prompt to display. */
    size_t plen;        /* Prompt length. */
    size_t pos;         /* Current cursor position. */
    size_t oldpos;      /* Previous refresh cursor position. */
    size_t len;         /* Current edited line length. */
    size_t cols;        /* Number of columns in terminal. */
    size_t maxrows;     /* Maximum num of rows used so far (multiline mode) */
    int history_index;  /* The history index we are currently editing. */
};

enum KEY_ACTION{
	KEY_NULL = 0,	        /* NULL */
	KEY_CTRL_A = 1,         /* Ctrl+a */
	KEY_CTRL_B = 2,         /* Ctrl-b */
	KEY_CTRL_C = 3,         /* Ctrl-c */
	KEY_CTRL_D = 4,         /* Ctrl-d */
	KEY_CTRL_E = 5,         /* Ctrl-e */
	KEY_CTRL_F = 6,         /* Ctrl-f */
	KEY_CTRL_H = 8,         /* Ctrl-h backspace */
	KEY_TAB = 9,            /* Tab */
	KEY_CTRL_K = 11,        /* Ctrl+k */
	KEY_CTRL_L = 12,        /* Ctrl+l */
	KEY_ENTER = 13,         /* Enter */
	KEY_CTRL_N = 14,        /* Ctrl-n */
	KEY_CTRL_P = 16,        /* Ctrl-p */
	KEY_CTRL_T = 20,        /* Ctrl-t */
	KEY_CTRL_U = 21,        /* Ctrl+u */
	KEY_CTRL_W = 23,        /* Ctrl+w */
	KEY_ESC = 27,           /* Escape */
	KEY_DELETE = 127        /* Delete */
};

static void linenoiseAtExit(void);
int linenoiseHistoryAdd(const char *line);
static void refreshLine(struct linenoiseState *l);

/* Debugging macro. */
#if 0
FILE *lndebug_fp = NULL;
#define lndebug_raw(...) \
    do { \
        if (lndebug_fp == NULL) \
            printf("open "LNDEBUG_LOG"\n"); \
            lndebug_fp = fopen(LNDEBUG_LOG,"a"); \
        if (lndebug_fp != NULL) {                 \
            fprintf(lndebug_fp, __VA_ARGS__); \
            fflush(lndebug_fp); \
        } else { printf("failled to open "LNDEBUG_LOG"\n"); fprintf(lndebug_fp, __VA_ARGS__);     \
        } \
    } while (0)
#define lndebug(...) \
    do { \
        if (lndebug_fp == NULL) { \
            lndebug_fp = fopen(LNDEBUG_LOG,"a"); \
            fprintf(lndebug_fp, \
            "[%d %d %d] p: %d, rows: %d, rpos: %d, max: %d, oldmax: %d\n", \
            (int)l->len,(int)l->pos,(int)l->oldpos,plen,rows,rpos, \
            (int)l->maxrows,old_rows); \
        } \
        fprintf(lndebug_fp, ", " __VA_ARGS__); \
        fflush(lndebug_fp); \
    } while (0)
#else
#define lndebug_raw(fmt, ...)
#define lndebug(fmt, ...)
#endif

#if 0
static void buf_str(FILE *out, const void *buf, size_t len)
{   const char *chars = (const char *)buf;
    while (len-- > 0) {
        char ch = *chars++;
        if (ch == '"' || ch == '\'' || ch == '\\')
            fprintf(out, "\\%c", ch);
        else if (isprint(ch))
            fputc(ch, out);
        else
            fprintf(out, "\\x%02x", ch);
    }
    fflush(out);
}
#endif

#if 0
static int tty_write(ttyio_t handle, const void *buf, size_t bytes)
{
    lndebug_raw("out: [%d] \"", bytes);
    buf_str(lndebug_fp, buf, bytes);
    lndebug_raw("\"\n");
    return _tty_write(handle, buf, bytes);
}
#else
#define tty_write  _tty_write
#endif

#if 0
static int tty_read(ttyio_t handle, void *buf, size_t bytes)
{
    int rc = _tty_read(handle, buf, bytes);
    lndebug_raw("in: [%d] \"", rc);
    buf_str(lndebug_fp, buf, bytes);
    lndebug_raw("\"\n");
    return rc;
    
}
#else
#define tty_read  _tty_read
#endif


/* ======================= Low level terminal handling ====================== */

/* Set if to use or not the multi line mode. */
void linenoiseSetMultiLine(int ml) {
    mlmode = ml;
}

/* Return true if the terminal name is in the list of terminals we know are
 * not able to understand basic escape sequences. */
static int isUnsupportedTerm(void) {
    char *term = getenv("TERM");
    int j;

    if (term == NULL) return 0;
    for (j = 0; unsupported_term[j]; j++)
        if (!strcasecmp(term,unsupported_term[j])) return 1;
    return 0;
}

static int getCursorPosition(ttyio_t ifd, ttyio_t ofd);

#ifdef WIN32

/* Select console I/O if not already selected */
int linenoiseConsoleInit(void)
{
    /* Note - when called from some shells, in particular from 'cmd'
       stdin and stdout are already connected to the console device on
       windows.
       In other shells (e.g. cygwin bash) this is not the case and most
       of the console management calls will fail with 'Invalid handle' - so
       we assign the console stream explicitly here.
    */
    FILE *con_in = freopen("CONIN$", "r+", stdin);
    FILE *con_out = freopen("CONOUT$", "w+", stdout);
    
    lndebug_raw("stdin %sredirected to console\n",
                con_in==NULL?"not ":"");
    lndebug_raw("stdout %sredirected to console\n",
                con_out==NULL?"not ":"");
    return con_in != NULL && con_out != NULL? 0: ENOTTY; 
}

static int enableRawMode(ttyio_t hand_tty_in, ttyio_t hand_tty_out) {
    int rc = 0;
    
    if (!tty_isatty(hand_tty_in)) {
        lndebug_raw("not TTY\n");
        goto fatal;
    }
    if (!GetConsoleMode(hand_tty_out, &term_mode_out)) {
        int rc = GetLastError();
        lndebug_raw("no output console mode - hand %d rc 0x%X\n",
                    hand_tty_out, rc);
        goto fatal;
    }
    if (!GetConsoleMode(hand_tty_in,  &term_mode_in)) {
        int rc = GetLastError();
        lndebug_raw("no input console mode - hand %d (invalid %d) rc 0x%X\n",
                    hand_tty_in, INVALID_HANDLE_VALUE, rc);
        goto fatal;
    }
    
    if (!atexit_registered) {
        atexit(linenoiseAtExit);
        atexit_registered = 1;
    }
    
    if (/* enter 'raw' input mkode - turn off processed, line, and echo */
        !SetConsoleMode(hand_tty_in, term_mode_in & ~ENABLE_LINE_INPUT &
                        ~ENABLE_ECHO_INPUT)) {
        rc = GetLastError();
        lndebug_raw("can't set raw input 0x%x - hand %d rc 0x%x\n",
                    term_mode_in & ~ENABLE_LINE_INPUT & ~ENABLE_ECHO_INPUT,
                    hand_tty_in, rc);
    } 
    if (/* enter 'raw' output mode - turn off processed output */
        !SetConsoleMode(hand_tty_out,
                        term_mode_out & ~ENABLE_PROCESSED_OUTPUT)) {
        rc = GetLastError();
        lndebug_raw("can't set raw output 0x%x - hand %d rc 0x%x\n",
                    term_mode_out & ~ENABLE_PROCESSED_OUTPUT, hand_tty_out, rc);
    } 
    if (!FlushConsoleInputBuffer(hand_tty_in)) {
        rc = GetLastError();
        lndebug_raw("can't flush raw input - hand %d rc 0x%x\n",
                    hand_tty_in, rc);
    }
    if (rc == 0) {
        lndebug_raw("in Raw mode\n"); 
        rawmode = 1;
    }
    
    return 0;

fatal:
    errno = ENOTTY;
    return -1;
}

static void disableRawMode(ttyio_t ifd, ttyio_t ofd) {
    (void)SetConsoleMode(ifd,  term_mode_in);
    (void)SetConsoleMode(ofd, term_mode_out);
    lndebug_raw("out of Raw mode\n"); 
}

/* Try to get the number of columns in the current terminal, or assume 80
 * if it fails. */
static int getColumns(ttyio_t ifd, ttyio_t ofd) {
    CONSOLE_SCREEN_BUFFER_INFO screen_info;

    if (GetConsoleScreenBufferInfo(ofd, &screen_info)) {
        lndebug_raw("console width %d\n", screen_info.dwSize.X);
        return screen_info.dwSize.X;
    } else {
        int rc = GetLastError();
        lndebug_raw("no console width - rc 0x%x\n", rc);
        return 80;
    }
}


#else

#define FLUSH_INPUT_ON_RAW_CHANGE 0

#if FLUSH_INPUT_ON_RAW_CHANGE
#define LINENOISE_TCSACHANGE TCSAFLUSH
#else
#define LINENOISE_TCSACHANGE TCSADRAIN
#endif

/* Select console I/O if not already selected */
int linenoiseConsoleInit(void)
{   return 0;
}

/* Raw mode: 1960 magic shit. */
static int enableRawMode(ttyio_t ifd, ttyio_t ofd) {
    struct termios raw;

    if (!isatty(ifd)) goto fatal;
    if (!atexit_registered) {
        atexit(linenoiseAtExit);
        atexit_registered = 1;
    }
    if (tcgetattr(ifd,&orig_termios) == -1) goto fatal;

    raw = orig_termios;  /* modify the original mode */
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - echoing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* control chars - set return condition: min number of bytes and timer.
     * We want read to return every single byte, without timeout. */
    raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0; /* 1 byte, no timer */

    /* put terminal in raw mode after flushing */
    if (tcsetattr(ifd,LINENOISE_TCSACHANGE,&raw) < 0) goto fatal;
    rawmode = 1;
    return 0;

fatal:
    errno = ENOTTY;
    return -1;
}

static void disableRawMode(ttyio_t ifd, ttyio_t ofd) {
    /* Don't even check the return value as it's too late. */
    if (rawmode && tcsetattr(ifd,LINENOISE_TCSACHANGE,&orig_termios) != -1)
        rawmode = 0;
}

/* Try to get the number of columns in the current terminal, or assume 80
 * if it fails. */
static int getColumns(ttyio_t ifd, ttyio_t ofd) {
    struct winsize ws;

    if (ioctl(1, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        /* ioctl() failed. Try to query the terminal itself. */
        int start, cols;

        /* Get the initial position so we can restore it later. */
        start = getCursorPosition(ifd,ofd);
        if (start == -1) goto failed;

        /* Go to right margin and get position. */
        if (tty_write(ofd,"\x1b[999C",6) != 6) goto failed;
        cols = getCursorPosition(ifd,ofd);
        if (cols == -1) goto failed;

        /* Restore position. */
        if (cols > start) {
            char seq[32];
            snprintf(seq,32,"\x1b[%dD",cols-start);
            if (tty_write(ofd,seq,strlen(seq)) == -1) {
                /* Can't recover... */
            }
        }
        return cols;
    } else {
        return ws.ws_col;
    }

failed:
    return 80;
}

#endif

/* Use the ESC [6n escape sequence to query the horizontal cursor position
 * and return it. On error -1 is returned, on success the position of the
 * cursor. */
static int getCursorPosition(ttyio_t ifd, ttyio_t ofd) {
    char buf[32];
    int cols, rows;
    unsigned int i = 0;

    /* Report cursor location */
    if (tty_write(ofd, "\x1b[6n", 4) != 4) return -1;

    /* Read the response: ESC [ rows ; cols R */
    while (i < sizeof(buf)-1) {
        if (tty_read(ifd,buf+i,1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    /* Parse it. */
    if (buf[0] != KEY_ESC || buf[1] != '[') return -1;
    if (sscanf(buf+2,"%d;%d",&rows,&cols) != 2) return -1;
    return cols;
}

/* Clear the screen. Used to handle ctrl+l */
void linenoiseClearScreen(void) {
    if (tty_write(tty_stdout(),"\x1b[H\x1b[2J",7) <= 0) {
        /* nothing to do, just to avoid warning. */
    }
}

/* Beep, used for completion when there is nothing to complete or when all
 * the choices were already shown. */
static void linenoiseBeep(void) {
    fprintf(stderr, "\x7");
    fflush(stderr);
}

/* ============================== Completion ================================ */

/* Free a list of completion option populated by linenoiseAddCompletion(). */
static void freeCompletions(linenoiseCompletions *lc) {
    size_t i;
    for (i = 0; i < lc->len; i++)
        free(lc->cvec[i]);
    if (lc->cvec != NULL)
        free(lc->cvec);
}

/* This is an helper function for linenoiseEdit() and is called when the
 * user types the <KEY_TAB> key in order to complete the string currently in the
 * input.
 *
 * The state of the editing is encapsulated into the pointed linenoiseState
 * structure as described in the structure definition. */
static int completeLine(struct linenoiseState *ls) {
    linenoiseCompletions lc = { 0, NULL };
    long nread, nwritten;
    char c = 0;

    completionCallback(ls->buf,&lc);
    if (lc.len == 0) {
        linenoiseBeep();
    } else {
        size_t stop = 0, i = 0;

        while(!stop) {
            /* Show completion or original buffer */
            if (i < lc.len) {
                struct linenoiseState saved = *ls;

                ls->len = ls->pos = strlen(lc.cvec[i]);
                ls->buf = lc.cvec[i];
                refreshLine(ls);
                ls->len = saved.len;
                ls->pos = saved.pos;
                ls->buf = saved.buf;
            } else {
                refreshLine(ls);
            }

            nread = tty_read(ls->ifd,&c,1);
            if (nread <= 0) {
                freeCompletions(&lc);
                return -1;
            }

            switch(c) {
                case 9: /* TAB */
                    i = (i+1) % (lc.len+1);
                    if (i == lc.len) linenoiseBeep();
                    break;
                case 27: /* escape */
                    /* Re-show original buffer */
                    if (i < lc.len) refreshLine(ls);
                    stop = 1;
                    break;
                default:
                    /* Update buffer and return */
                    if (i < lc.len) {
                        nwritten = snprintf(ls->buf,ls->buflen,"%s",lc.cvec[i]);
                        ls->len = ls->pos = nwritten;
                    }
                    stop = 1;
                    break;
            }
        }
    }

    freeCompletions(&lc);
    return c; /* Return last read character */
}

/* Register a callback function to be called for tab-completion. */
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *fn) {
    completionCallback = fn;
}

/* This function is used by the callback function registered by the user
 * in order to add completion options given the input string when the
 * user typed <tab>. See the example.c source code for a very easy to
 * understand example. */
void linenoiseAddCompletion(linenoiseCompletions *lc, const char *str) {
    size_t len = strlen(str);
    char *copy, **cvec;

    copy = malloc(len+1);
    if (copy == NULL) return;
    memcpy(copy,str,len+1);
    cvec = realloc(lc->cvec,sizeof(char*)*(lc->len+1));
    if (cvec == NULL) {
        free(copy);
        return;
    }
    lc->cvec = cvec;
    lc->cvec[lc->len++] = copy;
}

/* =========================== Line editing ================================= */

/* We define a very simple "append buffer" structure, that is an heap
 * allocated string where we can append to. This is useful in order to
 * write all the escape sequences in a buffer and flush them to the standard
 * output in a single call, to avoid flickering effects. */
struct abuf {
    char *b;
    int len;
};

static void abInit(struct abuf *ab) {
    ab->b = NULL;
    ab->len = 0;
}

static void abAppend(struct abuf *ab, const char *s, size_t len) {
    char *new = realloc(ab->b,ab->len+len);

    if (new == NULL) return;
    memcpy(new+ab->len,s,len);
    ab->b = new;
    ab->len += len;
}

static void abFree(struct abuf *ab) {
    free(ab->b);
}

/* Single line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshSingleLine(struct linenoiseState *l) {
    char seq[64];
    size_t plen = strlen(l->prompt);
    ttyio_t fd = l->ofd;
    char *buf = l->buf;
    size_t len = l->len;
    size_t pos = l->pos;
    struct abuf ab;

    while((plen+pos) >= l->cols) {
        buf++;
        len--;
        pos--;
    }
    while (plen+len > l->cols) {
        len--;
    }

    abInit(&ab);
    /* Cursor to left edge */
    snprintf(seq,64,"\r");
    abAppend(&ab,seq,strlen(seq));
    /* Write the prompt and the current buffer content */
    abAppend(&ab,l->prompt,strlen(l->prompt));
    abAppend(&ab,buf,len);
    /* Erase to right */
    snprintf(seq,64,"\x1b[0K");
    abAppend(&ab,seq,strlen(seq));
    /* Move cursor to original position. */
    snprintf(seq,64,"\r\x1b[%dC", (int)(pos+plen));
    abAppend(&ab,seq,strlen(seq));
    if (tty_write(fd,ab.b,ab.len) == -1) {}
     /* Can't recover from write error. */
    abFree(&ab);
}

/* Multi line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshMultiLine(struct linenoiseState *l) {
    char seq[64];
    int plen = (int)strlen(l->prompt);
    int rows = (int)(plen+l->len+l->cols-1)/l->cols; /* rows used by current buf. */
    int rpos = (int)(plen+l->oldpos+l->cols)/l->cols; /* cursor relative row. */
    int rpos2; /* rpos after refresh. */
    int col; /* colum position, zero-based. */
    int old_rows = (int)l->maxrows;
    ttyio_t fd = l->ofd;
    int j;
    struct abuf ab;

    /* Update maxrows if needed. */
    if (rows > (int)l->maxrows) l->maxrows = rows;

    /* First step: clear all the lines used before. To do so start by
     * going to the last row. */
    abInit(&ab);
    if (old_rows-rpos > 0) {
        lndebug("go down %d", old_rows-rpos);
        snprintf(seq,64,"\x1b[%dB", old_rows-rpos);
        abAppend(&ab,seq,strlen(seq));
    }

    /* Now for every row clear it, go up. */
    for (j = 0; j < old_rows-1; j++) {
        lndebug("clear+up");
        snprintf(seq,64,"\r\x1b[0K\x1b[1A");
        abAppend(&ab,seq,strlen(seq));
    }

    /* Clean the top line. */
    lndebug("clear");
    snprintf(seq,64,"\r\x1b[0K");
    abAppend(&ab,seq,strlen(seq));

    /* Write the prompt and the current buffer content */
    abAppend(&ab,l->prompt,strlen(l->prompt));
    abAppend(&ab,l->buf,l->len);

    /* If we are at the very end of the screen with our prompt, we need to
     * emit a newline and move the prompt to the first column. */
    if (l->pos &&
        l->pos == l->len &&
        (l->pos+plen) % l->cols == 0)
    {
        lndebug("<newline>");
        abAppend(&ab,"\n",1);
        snprintf(seq,64,"\r");
        abAppend(&ab,seq,strlen(seq));
        rows++;
        if (rows > (int)l->maxrows) l->maxrows = rows;
    }

    /* Move cursor to right position. */
    rpos2 = (int)(plen+l->pos+l->cols)/l->cols; /* current cursor relative row. */
    lndebug("rpos2 %d", rpos2);

    /* Go up till we reach the expected positon. */
    if (rows-rpos2 > 0) {
        lndebug("go-up %d", rows-rpos2);
        snprintf(seq,64,"\x1b[%dA", rows-rpos2);
        abAppend(&ab,seq,strlen(seq));
    }

    /* Set column. */
    col = (plen+(int)l->pos) % (int)l->cols;
    lndebug("set col %d", 1+col);
    if (col)
        snprintf(seq,64,"\r\x1b[%dC", col);
    else
        snprintf(seq,64,"\r");
    abAppend(&ab,seq,strlen(seq));

    lndebug("\n");
    l->oldpos = l->pos;

    if (tty_write(fd,ab.b,ab.len) == -1) {}
    /* Can't recover from write error. */
    abFree(&ab);
}

/* Calls the two low level functions refreshSingleLine() or
 * refreshMultiLine() according to the selected mode. */
static void refreshLine(struct linenoiseState *l) {
    if (mlmode)
        refreshMultiLine(l);
    else
        refreshSingleLine(l);
}

/* Insert the character 'c' at cursor current position.
 *
 * On error writing to the terminal -1 is returned, otherwise 0. */
int linenoiseEditInsert(struct linenoiseState *l, char c) {
    if (l->len < l->buflen) {
        if (l->len == l->pos) {
            l->buf[l->pos] = c;
            l->pos++;
            l->len++;
            l->buf[l->len] = '\0';
            if ((!mlmode && l->plen+l->len < l->cols) /* || mlmode */) {
                /* Avoid a full update of the line in the
                 * trivial case. */
                if (tty_write(l->ofd,&c,1) == -1) return -1;
            } else {
                refreshLine(l);
            }
        } else {
            memmove(l->buf+l->pos+1,l->buf+l->pos,l->len-l->pos);
            l->buf[l->pos] = c;
            l->len++;
            l->pos++;
            l->buf[l->len] = '\0';
            refreshLine(l);
        }
    }
    return 0;
}

/* Move cursor on the left. */
void linenoiseEditMoveLeft(struct linenoiseState *l) {
    if (l->pos > 0) {
        l->pos--;
        refreshLine(l);
    }
}

/* Move cursor on the right. */
void linenoiseEditMoveRight(struct linenoiseState *l) {
    if (l->pos != l->len) {
        l->pos++;
        refreshLine(l);
    }
}

/* Move cursor to the start of the line. */
void linenoiseEditMoveHome(struct linenoiseState *l) {
    if (l->pos != 0) {
        l->pos = 0;
        refreshLine(l);
    }
}

/* Move cursor to the end of the line. */
void linenoiseEditMoveEnd(struct linenoiseState *l) {
    if (l->pos != l->len) {
        l->pos = l->len;
        refreshLine(l);
    }
}

/* Substitute the currently edited line with the next or previous history
 * entry as specified by 'dir'. */
#define LINENOISE_HISTORY_NEXT 0
#define LINENOISE_HISTORY_PREV 1
void linenoiseEditHistoryNext(struct linenoiseState *l, int dir) {
    if (history_len > 1) {
        /* Update the current history entry before to
         * overwrite it with the next one. */
        free(history[history_len - 1 - l->history_index]);
        history[history_len - 1 - l->history_index] = strdup(l->buf);
        /* Show the new entry */
        l->history_index += (dir == LINENOISE_HISTORY_PREV) ? 1 : -1;
        if (l->history_index < 0) {
            l->history_index = 0;
            return;
        } else if (l->history_index >= history_len) {
            l->history_index = history_len-1;
            return;
        }
        strncpy(l->buf,history[history_len - 1 - l->history_index],l->buflen);
        l->buf[l->buflen-1] = '\0';
        l->len = l->pos = strlen(l->buf);
        refreshLine(l);
    }
}

/* Delete the character at the right of the cursor without altering the cursor
 * position. Basically this is what happens with the "Delete" keyboard key. */
void linenoiseEditDelete(struct linenoiseState *l) {
    if (l->len > 0 && l->pos < l->len) {
        memmove(l->buf+l->pos,l->buf+l->pos+1,l->len-l->pos-1);
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(l);
    }
}

/* Backspace implementation. */
void linenoiseEditBackspace(struct linenoiseState *l) {
    if (l->pos > 0 && l->len > 0) {
        memmove(l->buf+l->pos-1,l->buf+l->pos,l->len-l->pos);
        l->pos--;
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(l);
    }
}

/* Delete the previosu word, maintaining the cursor at the start of the
 * current word. */
void linenoiseEditDeletePrevWord(struct linenoiseState *l) {
    size_t old_pos = l->pos;
    size_t diff;

    while (l->pos > 0 && l->buf[l->pos-1] == ' ')
        l->pos--;
    while (l->pos > 0 && l->buf[l->pos-1] != ' ')
        l->pos--;
    diff = old_pos - l->pos;
    memmove(l->buf+l->pos,l->buf+old_pos,l->len-old_pos+1);
    l->len -= diff;
    refreshLine(l);
}

/* This function is the core of the line editing capability of linenoise.
 * It expects 'fd' to be already in "raw mode" so that every key pressed
 * will be returned ASAP to tty_read().
 *
 * The resulting string is put into 'buf' when the user type enter, or
 * when ctrl+d is typed.
 *
 * The function returns the length of the current buffer. */
static int linenoiseEdit(ttyio_t stdin_fd, ttyio_t stdout_fd, char *buf, size_t buflen, const char *prompt)
{
    struct linenoiseState l;
    int edit_complete = 0;/*FALSE*/
    int cb_len = -1; /*current buffer length*/

    /* Populate the linenoise state that we pass to functions implementing
     * specific editing functionalities. */
    l.ifd = stdin_fd;
    l.ofd = stdout_fd;
    l.buf = buf;
    l.buflen = buflen;
    l.prompt = prompt;
    l.plen = strlen(prompt);
    l.oldpos = l.pos = 0;
    l.len = 0;
    l.cols = getColumns(stdin_fd, stdout_fd);
    l.maxrows = 0;
    l.history_index = 0;

    /* Buffer starts empty. */
    l.buf[0] = '\0';
    l.buflen--; /* Make sure there is always space for the nulterm */

    /* The latest history entry is always our current buffer, that
     * initially is just an empty string. */
    linenoiseHistoryAdd("");

    if (tty_write(l.ofd,prompt,l.plen) == -1)
    {   cb_len = -1;
        edit_complete = 1; /*TRUE*/
    } else
    while(!edit_complete) {
        char c;
        size_t nread;
        char seq[3];

        nread = tty_read(l.ifd,&c,1);
        if (nread <= 0)
        {   edit_complete = 1/*TRUE*/;
            cb_len = (int)l.len;
        }
        else
        /* Only autocomplete when the callback is set. It returns < 0 when
         * there was an error reading from fd. Otherwise it will return the
         * character that should be handled next. */
        if (c == 9 && completionCallback != NULL) {
            c = completeLine(&l);
            /* Return on errors */
            if (c < 0)
            {   edit_complete = 1/*TRUE*/;
                cb_len = (int)l.len;
            }
            /* Read next character when 0 */
            else if (c == 0) continue;
        }

        if (!edit_complete)
        {
            switch(c) {
            case KEY_ENTER:    /* enter */
                history_len--;
                free(history[history_len]);
                if (mlmode) linenoiseEditMoveEnd(&l);
                edit_complete = 1/*TRUE*/;
                cb_len = (int)l.len;
                break;
            case KEY_CTRL_C:     /* ctrl-c */
                errno = EAGAIN;
                edit_complete = 1/*TRUE*/;
                cb_len = -1;
                break;
            case KEY_DELETE:      /* delete */
            case KEY_CTRL_H:      /* backspace */
                linenoiseEditBackspace(&l);
                break;
            case KEY_CTRL_D:     /* ctrl-d, remove char at right of cursor, or if the
                                line is empty, act as end-of-file. */
                if (l.len > 0) {
                    linenoiseEditDelete(&l);
                } else {
                    history_len--;
                    free(history[history_len]);
                    edit_complete = 1;/*TRUE*/
                    cb_len = -1;
                }
                break;
            case KEY_CTRL_T:    /* ctrl-t, swaps current character with previous. */
                if (l.pos > 0 && l.pos < l.len) {
                    int aux = buf[l.pos-1];
                    buf[l.pos-1] = buf[l.pos];
                    buf[l.pos] = aux;
                    if (l.pos != l.len-1) l.pos++;
                    refreshLine(&l);
                }
                break;
            case KEY_CTRL_B:     /* ctrl-b */
                linenoiseEditMoveLeft(&l);
                break;
            case KEY_CTRL_F:     /* ctrl-f */
                linenoiseEditMoveRight(&l);
                break;
            case KEY_CTRL_P:    /* ctrl-p */
                linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_PREV);
                break;
            case KEY_CTRL_N:    /* ctrl-n */
                linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_NEXT);
                break;
            case KEY_ESC:    /* escape sequence */
                /* Read the next two bytes representing the escape sequence.
                 * Use two calls to handle slow terminals returning the two
                 * chars at different times. */
                if (tty_read(l.ifd,seq,1) == -1) break;
                if (tty_read(l.ifd,seq+1,1) == -1) break;

                /* ESC [ sequences. */
                if (seq[0] == '[') {
                    if (seq[1] >= '0' && seq[1] <= '9') {
                        /* Extended escape, read additional byte. */
                        if (tty_read(l.ifd,seq+2,1) == -1) break;
                        if (seq[2] == '~') {
                            switch(seq[1]) {
                            case '3': /* Delete key. */
                                linenoiseEditDelete(&l);
                                break;
                            }
                        }
                    } else {
                        switch(seq[1]) {
                        case 'A': /* Up */
                            linenoiseEditHistoryNext(&l,
                                                     LINENOISE_HISTORY_PREV);
                            break;
                        case 'B': /* Down */
                            linenoiseEditHistoryNext(&l,
                                                     LINENOISE_HISTORY_NEXT);
                            break;
                        case 'C': /* Right */
                            linenoiseEditMoveRight(&l);
                            break;
                        case 'D': /* Left */
                            linenoiseEditMoveLeft(&l);
                            break;
                        case 'H': /* Home */
                            linenoiseEditMoveHome(&l);
                            break;
                        case 'F': /* End*/
                            linenoiseEditMoveEnd(&l);
                            break;
                        }
                    }
                }

                /* ESC O sequences. */
                else if (seq[0] == 'O') {
                    switch(seq[1]) {
                    case 'H': /* Home */
                        linenoiseEditMoveHome(&l);
                        break;
                    case 'F': /* End*/
                        linenoiseEditMoveEnd(&l);
                        break;
                    }
                }
                break;
            default:
                if (linenoiseEditInsert(&l,c))
                {   edit_complete = 1/*TRUE*/;
                    cb_len = -1;
                }
                break;
            case KEY_CTRL_U: /* Ctrl+u, delete the whole line. */
                buf[0] = '\0';
                l.pos = l.len = 0;
                refreshLine(&l);
                break;
            case KEY_CTRL_K: /* Ctrl+k, delete from current to end of line. */
                buf[l.pos] = '\0';
                l.len = l.pos;
                refreshLine(&l);
                break;
            case KEY_CTRL_A: /* Ctrl+a, go to the start of the line */
                linenoiseEditMoveHome(&l);
                break;
            case KEY_CTRL_E: /* ctrl+e, go to the end of the line */
                linenoiseEditMoveEnd(&l);
                break;
            case KEY_CTRL_L: /* ctrl+l, clear screen */
                linenoiseClearScreen();
                refreshLine(&l);
                break;
            case KEY_CTRL_W: /* ctrl+w, delete previous word */
                linenoiseEditDeletePrevWord(&l);
                break;
            }
        }
    }
    
    return cb_len;
}

/* This special mode is used by linenoise in order to print scan codes
 * on screen for debugging / development purposes. It is implemented
 * by the linenoise_example program using the --keycodes option. */
void linenoisePrintKeyCodes(void) {
    char quit[4];
    ttyio_t conin  = tty_stdin();
    ttyio_t conout = tty_stdout();

    printf("Linenoise key codes debugging mode.\n"
            "Press keys to see scan codes. Type 'quit' at any time to exit.\n");
    if (enableRawMode(conin, conout) == -1) return;
    memset(quit,' ',4);
    while(1) {
        char c;
        size_t nread;

        nread = tty_read(conin,&c,1);
        if (nread <= 0) continue;
        memmove(quit,quit+1,sizeof(quit)-1); /* shift string to left. */
        quit[sizeof(quit)-1] = c; /* Insert current char on the right. */
        if (memcmp(quit,"quit",sizeof(quit)) == 0) break;

        printf("'%c' %02x (%d) (type quit to exit)\n",
            isprint(c) ? c : '?', (int)c, (int)c);
        printf("\r"); /* Go left edge manually, we are in raw mode. */
        fflush(stdout);
    }
    disableRawMode(conin, conout);
}

/* This function calls the line editing function linenoiseEdit() using
 * the STDIN file descriptor set in raw mode. */
static int linenoiseRaw(char *buf, size_t buflen, const char *prompt) {
    size_t count;
    ttyio_t conin  = tty_stdin();
    ttyio_t conout = tty_stdout();

    if (buflen == 0) {
        errno = EINVAL;
        return -1;
    }
    if (!tty_isatty(conin)) {
        /* Not a tty: read from file / pipe. */
        if (fgets(buf, buflen, stdin) == NULL) return -1;
        count = strlen(buf);
        if (count && buf[count-1] == '\n') {
            count--;
            buf[count] = '\0';
        }
    } else {
        /* Interactive editing. */
        if (enableRawMode(conin, conout) == -1) return -1;
        count = linenoiseEdit(conin, conout, buf, buflen, prompt);
        disableRawMode(conin, conout);
        printf("\n");
    }
    return (int)count;
}

/* The high level function that is the main API of the linenoise library.
 * This function checks if the terminal has basic capabilities, just checking
 * for a blacklist of stupid terminals, and later either calls the line
 * editing function or uses dummy fgets() so that you will be able to type
 * something even in the most desperate of the conditions. */
char *linenoise(const char *prompt) {
    char buf[LINENOISE_MAX_LINE];
    int count;

    if (isUnsupportedTerm()) {
        size_t len;

        printf("%s",prompt == NULL? "%": prompt);
        fflush(stdout);
        if (fgets(buf,LINENOISE_MAX_LINE,stdin) == NULL) return NULL;
        len = strlen(buf);
        while(len && (buf[len-1] == '\n' || buf[len-1] == '\r')) {
            len--;
            buf[len] = '\0';
        }
        return strdup(buf);
    } else {
        count = linenoiseRaw(buf,LINENOISE_MAX_LINE,prompt);
        if (count == -1) return NULL;
        return strdup(buf);
    }
}

/* ================================ History ================================= */

/* Free the history, but does not reset it. Only used when we have to
 * exit() to avoid memory leaks are reported by valgrind & co. */
static void freeHistory(void) {
    if (history) {
        int j;

        for (j = 0; j < history_len; j++)
            free(history[j]);
        free(history);
    }
}

/* At exit we'll try to fix the terminal to the initial conditions. */
static void linenoiseAtExit(void) {
    disableRawMode(tty_stdin(), tty_stdout());
    freeHistory();
}

/* This is the API call to add a new entry in the linenoise history.
 * It uses a fixed array of char pointers that are shifted (memmoved)
 * when the history max length is reached in order to remove the older
 * entry and make room for the new one, so it is not exactly suitable for huge
 * histories, but will work well for a few hundred of entries.
 *
 * Using a circular buffer is smarter, but a bit more complex to handle. */
int linenoiseHistoryAdd(const char *line) {
    char *linecopy;

    if (history_max_len == 0) return 0;

    /* Initialization on first call. */
    if (history == NULL) {
        history = malloc(sizeof(char*)*history_max_len);
        if (history == NULL) return 0;
        memset(history,0,(sizeof(char*)*history_max_len));
    }

    /* Don't add duplicated lines. */
    if (history_len && !strcmp(history[history_len-1], line)) return 0;

    /* Add an heap allocated copy of the line in the history.
     * If we reached the max length, remove the older line. */
    linecopy = strdup(line);
    if (!linecopy) return 0;
    if (history_len == history_max_len) {
        free(history[0]);
        memmove(history,history+1,sizeof(char*)*(history_max_len-1));
        history_len--;
    }
    history[history_len] = linecopy;
    history_len++;
    return 1;
}

/* Set the maximum length for the history. This function can be called even
 * if there is already some history, the function will make sure to retain
 * just the latest 'len' elements if the new history length value is smaller
 * than the amount of items already inside the history. */
int linenoiseHistorySetMaxLen(int len) {
    char **new;

    if (len < 1) return 0;
    if (history) {
        int tocopy = history_len;

        new = malloc(sizeof(char*)*len);
        if (new == NULL) return 0;

        /* If we can't copy everything, free the elements we'll not use. */
        if (len < tocopy) {
            int j;

            for (j = 0; j < tocopy-len; j++) free(history[j]);
            tocopy = len;
        }
        memset(new,0,sizeof(char*)*len);
        memcpy(new,history+(history_len-tocopy), sizeof(char*)*tocopy);
        free(history);
        history = new;
    }
    history_max_len = len;
    if (history_len > history_max_len)
        history_len = history_max_len;
    return 1;
}

/* Save the history in the specified file. On success 0 is returned
 * otherwise -1 is returned. */
int linenoiseHistorySave(const char *filename) {
    FILE *fp = fopen(filename,"w");
    int j;

    if (fp == NULL) return -1;
    for (j = 0; j < history_len; j++) {
        if (history[j] == NULL) {
            fprintf(stderr, "history %d is NULL\n", j);
            history[j] = "";
        }
        fprintf(fp,"%s\n",history[j]);
    }
    fclose(fp);
    return 0;
}

/* Load the history from the specified file. If the file does not exist
 * zero is returned and no operation is performed.
 *
 * If the file exists and the operation succeeded 0 is returned, otherwise
 * on error -1 is returned. */
int linenoiseHistoryLoad(const char *filename) {
    FILE *fp = fopen(filename,"r");
    char buf[LINENOISE_MAX_LINE];

    if (fp == NULL) return -1;

    while (fgets(buf,LINENOISE_MAX_LINE,fp) != NULL) {
        char *p;

        p = strchr(buf,'\r');
        if (!p) p = strchr(buf,'\n');
        if (p) *p = '\0';
        linenoiseHistoryAdd(buf);
    }
    fclose(fp);
    return 0;
}
