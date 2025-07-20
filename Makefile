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
ZSH_COMPLETION ?= /usr/share/zsh/functions/Completion/Zsh/_todoos

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

.PHONY: build ln-bin zsh-autocomplete zsh-aliases install show-gen clean distclean cmake-config test

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

install: ln-bin zsh-autocomplete zsh-aliases

upgrade:
	@mkdir -p ~/.cache/oceanspy/todoos/
	@echo "currentList: default" > ~/.cache/oceanspy/todoos/cache.conf

ln-bin:
	@echo "==> Installing symlink $(BINDIR)/$(BINARY_NAME) -> $(abspath $(BUILD_DIR)/$(BINARY_NAME))"
	@ln -sf $(abspath $(BUILD_DIR)/$(BINARY_NAME)) $(BINDIR)/$(BINARY_NAME)
	@echo "Bin shortcut created!"

zsh-autocomplete:
	@echo "==> Installing zsh autocompletion to $(ZSH_COMPLETION)"
	@cp Linux/Zsh/_todoos $(ZSH_COMPLETION)
	@echo "Autocompletion installed!"
	@echo "Please restart your terminal or run 'omz reload' to apply the changes."

zsh-aliases:
	@echo "==> Installing zsh aliases"
	@cp Linux/Zsh/zsh_todoos_aliases ~/.zsh_todoos_aliases
	@cp Linux/Zsh/zsh_todoos_sync ~/.zsh_todoos_sync
	@echo ""
	@echo "=============="
	@echo ">>> Add this line to your zshrc files to enable shortcuts:"
	@echo "source ~/.zsh_todoos_aliases"
	@echo "=============="

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

.PHONY: distclean
distclean: .distclean
	@true
