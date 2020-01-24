PDB repository
==============

[![AppVeyor branch](https://img.shields.io/appveyor/ci/egor-tensin/pdb-repo/master?label=Visual%20Studio%20%28AppVeyor%29)](https://ci.appveyor.com/project/egor-tensin/pdb-repo/branch/master)
[![Travis (.com) branch](https://img.shields.io/travis/com/egor-tensin/pdb-repo/master?label=MinGW-w64%20%28Travis%29)](https://travis-ci.com/egor-tensin/pdb-repo)

Want to conduct a post-mortem analysis, but all you've got is a bunch of PDB
files and a call stack?
Say no more!
With the unique set of tools provided by this project, you'll never feel
hopeless again.
Do all kinds of crazy shit like resolving symbol names from their addresses &
vice versa, etc.

Building
--------

Create the build files using CMake and build the project using Visual Studio.

* **Prerequisites.**
Depends on Boost.{Filesystem,Program_options}.
* **Customization.**
The runtime libraries are linked statically by default.
Therefore, the Boost libraries must also link them statically.
You can link the runtime dynamically by passing `-D CC_STATIC_RUNTIME=OFF` to
`cmake`.
* **Example.**
Using Visual Studio 2015 (targeting x64), build & install the release version
to C:\pdb-repo:

      > cmake -G "Visual Studio 14 2015" -A x64 ^
          -D BOOST_ROOT=C:\path\to\boost_1_61_0 ^
          C:\path\to\pdb-repo
      ...

      > cmake --build . --config Release --target install -- /m
      ...

License
-------

Distributed under the MIT License.
See [LICENSE.txt] for details.

[LICENSE.txt]: LICENSE.txt
