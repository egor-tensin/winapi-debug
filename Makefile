# Various one-liners which I'm too lazy to remember.
# Basically a collection of really small shell scripts.

MAKEFLAGS += --no-builtin-rules --no-builtin-variables --warn-undefined-variables
unexport MAKEFLAGS
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:
.SUFFIXES:
SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c

.PHONY: DO
DO:

escape = $(subst ','\'',$(1))

define noexpand
ifeq ($$(origin $(1)),environment)
    $(1) := $$(value $(1))
endif
ifeq ($$(origin $(1)),environment override)
    $(1) := $$(value $(1))
endif
ifeq ($$(origin $(1)),command line)
    override $(1) := $$(value $(1))
endif
endef

TOOLSET ?= mingw
CONFIGURATION ?= Debug
BOOST_VERSION ?= 1.65.0
BOOST_LIBRARIES := --with-filesystem --with-program_options --with-test
CMAKE_FLAGS ?=

this_dir  := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
src_dir   := $(this_dir)
ifdef CI
build_dir := $(this_dir)../build
else
build_dir := $(this_dir).build
endif
boost_dir := $(build_dir)/boost
cmake_dir := $(build_dir)/cmake
DESTDIR   ?= $(build_dir)/install

$(eval $(call noexpand,TOOLSET))
$(eval $(call noexpand,CONFIGURATION))
$(eval $(call noexpand,BOOST_VERSION))
$(eval $(call noexpand,CMAKE_FLAGS))
$(eval $(call noexpand,DESTDIR))

.PHONY: all
all: build

.PHONY: clean
clean:
	rm -rf -- '$(call escape,$(build_dir))'

$(boost_dir)/:
	cd cmake && python3 -m project.boost.download --cache '$(call escape,$(build_dir))' -- '$(call escape,$(BOOST_VERSION))' '$(call escape,$(boost_dir))'

.PHONY: deps
ifdef CI
deps:
	cd cmake && python3 -m project.ci.boost -- $(BOOST_LIBRARIES)
else
deps: $(boost_dir)/
	cd cmake && python3 -m project.boost.build --toolset '$(call escape,$(TOOLSET))' --configuration '$(call escape,$(CONFIGURATION))' -- '$(call escape,$(boost_dir))' $(BOOST_LIBRARIES)
endif

.PHONY: build
build:
ifdef CI
	cd cmake && python3 -m project.ci.cmake --install -- $(CMAKE_FLAGS)
else
	cd cmake && python3 -m project.cmake.build --toolset '$(call escape,$(TOOLSET))' --configuration '$(call escape,$(CONFIGURATION))' --build '$(call escape,$(cmake_dir))' --install '$(call escape,$(DESTDIR))' --boost '$(call escape,$(boost_dir))' -- '$(call escape,$(src_dir))' $(CMAKE_FLAGS)
endif

.PHONY: install
install: build

.PHONY: test
test:
	cd -- '$(call escape,$(cmake_dir))' && ctest -C '$(call escape,$(CONFIGURATION))' --verbose

clang-tidy := run-clang-tidy
ifeq (1,$(shell test -e /usr/share/clang/run-clang-tidy.py && echo 1))
clang-tidy := /usr/share/clang/run-clang-tidy.py
endif

.PHONY: check
check:
	$(clang-tidy) -p '$(call escape,$(cmake_dir))' -quiet
