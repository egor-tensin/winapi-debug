PDB repository
==============

Want to conduct a post-mortem analysis, but all you've got is a bunch of PDB
files and a call stack?
Say no more!
With the unique set of tools provided by this project, you'll never feel
hopeless again.
Do all kinds of crazy shit like resolving symbol names from their addresses &
vice versa, etc.

Prerequisites
-------------

* Boost.Filesystem
* Boost.Program_options
* Boost.System

Building
--------

Create the build files using CMake and build using Visual Studio.
For example, using Visual Studio 2015 Update 3 (targetting x86-64 and using
static Boost libraries):

```
> cd
C:\workspace\build\pdb-repo\msvc\x64

> cmake -G "Visual Studio 14 2015 Win64"                ^
    -D BOOST_ROOT=C:\workspace\third-party\boost_1_61_0 ^
    -D Boost_USE_STATIC_LIBS=ON                         ^
    -D Boost_USE_STATIC_RUNTIME=ON                      ^
    C:\workspace\personal\pdb-repo
...

> cmake --build . --config release -- /m
...
```

License
-------

Distributed under the MIT License.
See [LICENSE.txt] for details.

[LICENSE.txt]: LICENSE.txt
