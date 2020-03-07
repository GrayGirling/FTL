## Recommended Build Environments

This code can be built under Windows, Linux and Mac OSX. Each has a
recommended tool set that may need to be installed before building is
possible.

In brief the tools are as follows:

  - Windows - MinGW (minmalist GNU for Windows)
  - Linux - standard gcc tools from the Linux distribution
  - Mac OS X - the MacPort tools

## Windows Build Environment - MinGW

Install MinGW <http://www.mingw.org/> by downloading the installer
'mingw-get-setup.exe' from
<http://sourceforge.net/projects/mingw/files/>.

  - During installation (run and click on the 'Install' button)
  - (Keep the graphical interface.)
  - Wait for the download.
  - Click on 'Continue' button when it has finished.
  - Select 'mingw32-32-gcc-g++' and 'mingw32-base' and 'msys-base'
  - Click on menu Installation \> Apply Changes
  - Then on the Apply button in the next dialogue 'Schedule of Pending
    Actions'
  - Wait for the download to complete, then click on 'Close'

Normally this will normally install the tools at C:\\MinGW

Install Eclipse https://eclipse.org/ from its download page choosing
'Eclipse IDE for C/C++ Developers' and either the 32- or 64-bit version
as corresponding to your version of Windows.

## Linux Build Environment - Local

Each Linux distribution has a 'package manager' using that you can
easily install the tools you need as packages. The name of the
appropriate package may varay from distribution to distribution.

In general you should look for the package which provides you with the
'gcc' compiler. These are example commands that you may need to run

  - Redhat Enterprise or Centos Linux distribution: rpm -ivh gcc
  - Fedora Linux distribution: yum install gcc
  - Debian or Ubuntu Linux distribution: apt-get install gcc

Note that you will usually need to be running as 'root' when installing
new software. Often this can be achieved by putting the command 'sudo'
in front of the relevant command.

## Mac OS X Build Environment - MacPort
