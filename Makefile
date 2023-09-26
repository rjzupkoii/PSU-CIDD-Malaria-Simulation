mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
PWD := $(dir $(mkfile_path))
VCPKG_BASE ?=~/vcpkg
VCPKG_EXEC := $(VCPKG_BASE)/vcpkg
VCPKG_TOOLCHAIN := $(VCPKG_BASE)/scripts/buildsystems/vcpkg.cmake
TOOLCHAIN_ARG := $(if $(VCPKG_BASE),-DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN),)
BUILD_TYPE ?= Release
DEFAULT_APP_EXECUTABLE := build/bin/MaSim
# Capture the second word in MAKECMDGOALS (if it exists)
APP_EXECUTABLE ?= $(or $(word 2,$(MAKECMDGOALS)),$(DEFAULT_APP_EXECUTABLE))

.PHONY: all build b clean setup-vcpkg install-deps generate g generate-no-test help test t run r

all: build

build b:
	cmake --build build --config $(BUILD_TYPE) -j 4

test t: build
	cmake --build build --target test

run r: build 
	./$(APP_EXECUTABLE)

clean:
	rm -rf build

setup-vcpkg:
	if [ -n "$(VCPKG_BASE)" ] && [ ! -x "$(VCPKG_EXEC)" ]; then \
		git submodule update --init; \
		$(VCPKG_BASE)/bootstrap-vcpkg.sh; \
	fi

install-deps: setup-vcpkg
	[ -z "$(VCPKG_BASE)" ] || $(VCPKG_EXEC) install gsl yaml-cpp fmt libpq libpqxx sqlite3 date args cli11 gtest catch 

generate g:
	cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 . $(TOOLCHAIN_ARG)
	cp $(PWD)build/compile_commands.json $(PWD)

generate-no-test:
	cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DWITH_TESTS=0 . $(TOOLCHAIN_ARG)
	cp $(PWD)build/compile_commands.json $(PWD)

help:
	@echo "Available targets:"
	@echo "  all             : Default target, builds the project."
	@echo "  build (b)       : Build the project with specified BUILD_TYPE (default: Release)."
	@echo "  test            : Rebuild and run tests."
	@echo "  run [path]      : Rebuild and run the executable. Provide path to override default."
	@echo "  clean           : Remove the build directory."
	@echo "  setup-vcpkg     : Setup vcpkg if specified by VCPKG_BASE."
	@echo "  install-deps    : Install dependencies using vcpkg."
	@echo "  generate (g)    : Generate the build system files with optional vcpkg toolchain."
	@echo "  generate-no-test: Generate the build system without tests."
	@echo "  help            : Show this help message."

