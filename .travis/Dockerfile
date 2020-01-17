FROM ubuntu:focal

RUN apt-get update -yq && \
    apt-get install -yq --no-install-recommends \
        # Used to download Boost:
        ca-certificates \
        # Used to bootstrap Boost:
        g++             \
        # Build scripts:
        python3         \
        # Build tools:
        cmake           \
        make            \
        mingw-w64

ARG travis_boost_version=1.65.0
ARG platform=x86
ARG configuration=Debug

# We're on Travis, that's right.
ENV TRAVIS=1
ENV TRAVIS_BUILD_DIR=/usr/src
COPY [".", "$TRAVIS_BUILD_DIR"]
WORKDIR "$TRAVIS_BUILD_DIR"

RUN ./cmake/boost/build/ci/travis.py \
        --link static \
        -- \
        target-os=windows \
        --user-config="$TRAVIS_BUILD_DIR/cmake/boost/toolchains/mingw-w64-$platform.jam" \
        --with-filesystem \
        --with-program_options && \
    ./cmake/cmake/build/ci/travis.py \
        --install "$HOME/install" \
        -- \
        -D CMAKE_SYSTEM_NAME=Windows \
        -D "CMAKE_TOOLCHAIN_FILE=$TRAVIS_BUILD_DIR/cmake/cmake/toolchains/mingw-w64-$platform.cmake" \
        -D "BOOST_ROOT=$HOME/boost_1_65_0" \
        -D "BOOST_LIBRARYDIR=$HOME/boost_1_65_0/stage/$platform/$configuration/lib"
