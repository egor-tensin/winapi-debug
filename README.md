PDB repository
==============

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
The following Boost libraries are required to build the project: Filesystem,
Program_options, System.
* **Customization.**
The runtime libraries are linked statically by default (when this project is
the root CMake project).
Therefore, the Boost dependencies must also link them statically.
You can link the runtime dynamically by passing `-D USE_STATIC_RUNTIME=OFF` to
`cmake`.
* **Example.**
In the example below, the project directory is
"C:\workspace\personal\pdb-repo", Boost can be found in
"C:\workspace\third-party\boost_1_61_0" and Visual Studio 2015 is used,
targeting x86-64.

      > cmake -G "Visual Studio 14 2015 Win64"                ^
          -D BOOST_ROOT=C:\workspace\third-party\boost_1_61_0 ^
          -D Boost_USE_STATIC_RUNTIME=ON                      ^
          C:\workspace\personal\pdb-repo
      ...

      > cmake --build . --config release -- /m
      ...

License
-------

Distributed under the MIT License.
See [LICENSE.txt] for details.

[LICENSE.txt]: LICENSE.txt
