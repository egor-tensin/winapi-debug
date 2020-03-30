FROM ubuntu:focal AS base

FROM base AS builder

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

ARG boost_version=1.65.0
ARG platform=x86
ARG configuration=Debug

# We're on Travis, that's right.
ENV TRAVIS_BUILD_DIR=/usr/src

COPY ["cmake", "$TRAVIS_BUILD_DIR/cmake"]
WORKDIR "$TRAVIS_BUILD_DIR"

RUN cd cmake && \
    python3 -m project.ci.travis.boost \
        --mingw \
        -- \
        --with-filesystem \
        --with-program_options \
        --with-test

COPY [".", "$TRAVIS_BUILD_DIR"]

RUN cd cmake && \
    python3 -m project.ci.travis.cmake \
        --install /opt/pdb-repo \
        --mingw

FROM base

COPY --from=builder ["/opt/pdb-repo", "/opt/pdb-repo"]
