#!/bin/bash

DIRECTORY="Build"
USER_HOME_DIR=$( getent passwd "$USER" | cut -d: -f6 )

if ! command -v cmake &> /dev/null
then
    echo "cmake could not be found -- Please install cmake"
    exit 1
fi

if [ ! -d "$DIRECTORY" ]; then
  mkdir -p $DIRECTORY
fi

cd $DIRECTORY

# check if ninja is available
if command -v ninja &> /dev/null
then
    echo "Ninja found, using ninja"
    echo ""
    cmake -DCMAKE_CXX_STANDARD=23 -G Ninja ..
    ninja
else
    echo "Ninja not found, using make instead"
    echo ""
    cmake -DCMAKE_CXX_STANDARD=23 ..
    make
fi

echo "Build complete!"
echo ""

# Ask to create a bin shortcut
echo "Do you want to create a bin shortcut in /usr/local/bin (require sudo)? (y/n)"
read -r response
if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]
then
    echo "Creating bin shortcut..." &&
    sudo ln -sf $PWD/todoos /usr/local/bin/todoos &&
    echo "Bin shortcut created!"
fi

echo ""

# Ask if they want to apply the oh-my-zsh autocompletion
# if user home dir is not null
if [ -z "$USER_HOME_DIR" ]; then
    echo "User home directory not found, skipping oh-my-zsh autocompletion installation."
    exit 0
fi

if [ ! -d "$USER_HOME_DIR/.oh-my-zsh" ]; then
    echo "oh-my-zsh not found, skipping oh-my-zsh autocompletion installation."
    exit 0
fi

echo "Do you want to install the oh-my-zsh autocompletion? (y/n)"
read -r response
if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]
then
    # check if completions folder exists
    if [ ! -d "$USER_HOME_DIR/.oh-my-zsh/completions" ]; then
        echo "Creating oh-my-zsh completions folder..." &&
        mkdir -p $USER_HOME_DIR/.oh-my-zsh/completions &&
        echo "oh-my-zsh completions folder created!"
    fi
    echo "Installing oh-my-zsh autocompletion..." &&
    cd .. &&
    cp Linux/Zsh/_todoos $USER_HOME_DIR/.oh-my-zsh/completions/_todoos &&
    echo "oh-my-zsh autocompletion installed!" &&
    echo "Please restart your terminal or run 'omz reload' to apply the changes."
fi


