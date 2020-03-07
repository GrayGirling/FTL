# Fun, Tiny Language

## Synopsys

FTL is provided as a library which makes it simple to create shell-style utilities that expect input in the form
```
    command arg arg
    command arg
    command
    command arg
    :
    etc.
```

The user can (optionally) add a number of additional pre-defined commands that
are sufficiently expressive to write programs in.

## Motivation

Using the library allows the user to implement just the commands necessary
without needing to implement things like:
* command line editing (depending on the build)
* concatenating lines using separated with a terminal '\'
* ignoring comment lines with an initial '#'
* variable setting and expansion
* including scripts from other files

The commands implemented can be scripted using FTL to implement more
complex commands in a way that is simpler than writing additional C/C++.

Also FTL, the language used to extend the scripts, is conceptually tiny and
quite fun to explore.

## Obtaining the code

Typically this command can be used to clone the repository into a new
directory called 'FTL':

```
git clone https://github.com/GrayGirling/FTL.git
```

## Building

The code is intended to be portable among at least Windows, Linux and MacOS
systems.

It is built in a traditional 'make' environment.  While this is
trivially available in Linux and MacOS it needs a little work to establish in
Windows.

A number of solutions are possible in windows:
* In Windows 10 (post the 2017 'Creators' edition) it is possible to use the Ubuntu Linux subsystem
* Cygwin can be downloaded and installed (selecting the mingw compilers), https://www.cygwin.com/
* MinGW can be used, http://www.mingw.org/
* Git for Windows can be used, https://git-scm.com/download/win

The code includes the main library - which you can use in your utility as well as:
* some extensions for additional sets of functions in subdirectory lib
* two example utilities: "ftl" - which just allows basic FTL programs to be run; and, "penv" - which provides an example tool (for maintaining "permanent" environment state)
* some examples of FTL functions that you may find useful in subdirectory flib


## Documentation

You can find (web format) documentation in directory 'doc' once you have cloned
this repository.

There is a description of the use of the library to create C or C++ command-line
programs [here](doc/markdown/FTL-commandline-tutorial.md).

[This](doc/markdown/FTL-build-setup.md) describes how the tool (or a new tool) can be built in different software environments (such as on Windows, MacOS or Linux).

When using FTL as the basis of a utility you may need to know what the built-in
commands and functions are - see the user guide: [here](doc/markdown/FTL-user-guide.md) (or [as PDF](doc/pdf/FTL-user-guide.pdf)).

A simple outline of the language is contained in: [spec](doc/markdown/FTL-spec.md).

## License

This is the FTL library written by Gray Girling while at Solarflare
Communications Inc., extended at Broadcom Inc and NXP Semiconductors Inc.

It has a BSD-style license - see the LICENSE file in this directory for most of
the code.

The code also includes an optional BSD-licensed line editing library (the use of
which will be controlled by your utility's Makefile).

Three options providing for line editing are available:
* no line editing (e.g. not really necessary when run from some Windows shells)
* the "standard" readline line editor, under a GPL license (which will require your program to be GPL licensed)
* the linenoise line editor, under a (non-viral) BSD license (see LICENSE-linenoise).


## Test suite

There is a test suite that requires the use of "bash" which tests many of the bult-in functions and can be used to spot regressions.  (Currently these tests work best when not used under Windows.) Help can be found using

```
test/check -h
```
