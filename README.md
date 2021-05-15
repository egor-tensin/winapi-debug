winapi-debug
============

[![CI](https://github.com/egor-tensin/winapi-debug/actions/workflows/ci.yml/badge.svg)](https://github.com/egor-tensin/winapi-debug/actions/workflows/ci.yml)

Want to conduct a post-mortem analysis, but all you've got is a bunch of PDB
files and a call stack?
Say no more!
With the unique set of tools provided by this project, you'll never feel
hopeless again.
Do all kinds of crazy shit like resolving symbol names from their addresses &
vice versa, etc.

Building
--------

Build using CMake.
Depends on Boost.{Filesystem,Program_options,Test}.
The project is Windows-only, so building with either MSVC or MinGW-w64 is
required.

There's a Makefile with useful shortcuts to build the project in the .build/
directory along with the dependencies (defaults to building with MinGW-w64):

    make deps
    make build
    make test

License
-------

Distributed under the MIT License.
See [LICENSE.txt] for details.

[LICENSE.txt]: LICENSE.txt
