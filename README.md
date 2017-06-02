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
* Git for Windows can be used (I think), https://git-scm.com/download/win


## Documentation

You can find (web format) documentation in directory 'doc' once you have cloned
this repository.

A description of the use of the library to create C or C++ command-line
programs can be downloaded from
[here](https://github.com/GrayGirling/FTL/raw/master/doc/FTL-commandline-tutorial.html).

If used as the basis of a language see the user guide at:
[user guide](https://github.com/GrayGirling/FTL/raw/master/doc/FTL-user-guide.pdf).

A simple outline of the language is contained in:
[spec](https://github.com/GrayGirling/FTL/raw/master/doc/FTL-spec.html).

## License

This is the FTL library written by Gray Girling while at Solarflare
Communications Inc., extended at Broadcom Inc and NXP Semiconductors Inc.

It has a BSD-style license - see the LICENSE file in this directory.

