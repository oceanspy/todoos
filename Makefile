# Makefile for the "todoos" project
# ------------------------------------------------------------
# Targets requested:
#   default (implicit) -> build (first target)
#   build              -> configure + compile in ./Build (C++23). Auto-detect Ninja.
#   ln-bin             -> sudo ln -sf Build/todoos to /usr/local/bin/todoos
#   omz-install        -> install oh-my-zsh completion (_todoos) into ~/.oh-my-zsh/completions/
#   install            -> runs ln-bin
#   test               -> runs unit tests
#
# Usage examples:
#   make                  # same as make build
#   make build            # configure+build
#   sudo make install     # just install the /usr/local/bin symlink (needs sudo)
#   make omz-install      # install the zsh completion into your home
#
# Notes:
# * Re-running "make build" will re-invoke cmake; this is harmless and ensures the
#   Build directory exists and is configured with the proper generator.
# * Generator auto-detection: if `ninja` is in PATH, use -G Ninja and build with it;
#   otherwise fall back to Unix Makefiles and build with `$(MAKE)`.
# * Override CMAKE_FLAGS on the command line if you want extra options, e.g.:
#       make build CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=Release -DTODOOS_WITH_X=ON"
#
# ------------------------------------------------------------

BUILD_DIR      ?= Build
BINARY_NAME    ?= todoos
PREFIX         ?= /usr/local
BINDIR         ?= $(PREFIX)/bin
OH_MY_ZSH_DIR  ?= $(HOME)/.oh-my-zsh
ZSH_COMPLETION ?= $(OH_MY_ZSH_DIR)/completions/_todoos

CMAKE_BASE_FLAGS ?= -DCMAKE_CXX_STANDARD=23
CMAKE_FLAGS      ?= $(CMAKE_BASE_FLAGS)

NINJA := $(shell command -v ninja 2>/dev/null)
ifeq ($(NINJA),)
  CMAKE_GENERATOR :=
  BUILD_CMD := $(MAKE) -C $(BUILD_DIR)
  MSG_GEN := Ninja not found; using make.
else
  CMAKE_GENERATOR := -G Ninja
  BUILD_CMD := $(NINJA) -C $(BUILD_DIR)
  MSG_GEN := Ninja found; using Ninja.
endif

.PHONY: build ln-bin omz-install install show-gen clean distclean cmake-config

build: show-compiler cmake-config
	@echo "==> Building $(BINARY_NAME) in $(BUILD_DIR) ..."
	@$(BUILD_CMD)
	@echo "Build complete!"

show-compiler:
	@echo "$(MSG_GEN)"

cmake-config:
	@mkdir -p $(BUILD_DIR)
	@echo "==> Configuring with CMake..."
	@cmake $(CMAKE_FLAGS) $(CMAKE_GENERATOR) -S . -B $(BUILD_DIR)

install: ln-bin

ln-bin:
	@echo "==> Installing symlink $(BINDIR)/$(BINARY_NAME) -> $(abspath $(BUILD_DIR)/$(BINARY_NAME))"
	@ln -sf $(abspath $(BUILD_DIR)/$(BINARY_NAME)) $(BINDIR)/$(BINARY_NAME)
	@echo "Bin shortcut created!"

omz-install:
	@if [ ! -d "$(OH_MY_ZSH_DIR)" ]; then \
	  echo "oh-my-zsh not found at $(OH_MY_ZSH_DIR); skipping."; \
	  exit 0; \
	fi
	@mkdir -p $(dir $(ZSH_COMPLETION))
	@echo "==> Installing oh-my-zsh autocompletion to $(ZSH_COMPLETION)"
	@cp Linux/Zsh/_todoos $(ZSH_COMPLETION)
	@echo "oh-my-zsh autocompletion installed!"
	@echo "Please restart your terminal or run 'omz reload' to apply the changes."

test:
	@./runtests.sh

clean:
	@echo "==> Cleaning build artifacts (keeping CMake configuration)."
ifeq ($(NINJA),)
	@$(MAKE) -C $(BUILD_DIR) clean || true
else
	@$(NINJA) -C $(BUILD_DIR) clean || true
endif

.distclean:
	@echo "==> Removing $(BUILD_DIR) directory."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BINDIR)/$(BINARY_NAME)

distclean: .distclean
	@true
