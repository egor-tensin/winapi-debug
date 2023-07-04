include prelude.mk

TOOLSET ?= mingw
PLATFORM ?= auto
CONFIGURATION ?= Debug
BOOST_VERSION ?= 1.65.0
BOOST_LIBRARIES := --with-filesystem --with-program_options --with-test
CMAKE_FLAGS ?=

this_dir  := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
src_dir   := $(this_dir)
build_dir := $(this_dir)build
boost_dir := $(build_dir)/boost
cmake_dir := $(build_dir)/cmake
DESTDIR   ?= $(build_dir)/install

$(eval $(call noexpand,TOOLSET))
$(eval $(call noexpand,PLATFORM))
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
	cd cmake && python3 -m project.boost.download \
		--cache '$(call escape,$(build_dir))' \
		-- \
		'$(call escape,$(BOOST_VERSION))' \
		'$(call escape,$(boost_dir))'

.PHONY: deps
deps: $(boost_dir)/
	cd cmake && python3 -m project.boost.build \
		--toolset '$(call escape,$(TOOLSET))' \
		--platform '$(call escape,$(PLATFORM))' \
		--configuration '$(call escape,$(CONFIGURATION))' \
		-- \
		'$(call escape,$(boost_dir))' \
		$(BOOST_LIBRARIES)

.PHONY: build
build:
	cd cmake && python3 -m project.build \
		--toolset '$(call escape,$(TOOLSET))' \
		--platform '$(call escape,$(PLATFORM))' \
		--configuration '$(call escape,$(CONFIGURATION))' \
		--install '$(call escape,$(DESTDIR))' \
		--boost '$(call escape,$(boost_dir))' \
		-- \
		'$(call escape,$(src_dir))' \
		'$(call escape,$(cmake_dir))' \
		$(CMAKE_FLAGS)

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
