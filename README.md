# ToDoOs

# Description

Smart CLI Application for Tasks management.

![alt text](https://github.com/oceanspy/todoos/blob/main/todoos-screenshot0.png?raw=true)
![alt text](https://github.com/oceanspy/todoos/blob/main/todoos-screenshot1.png?raw=true)

# Information

For now, the tool was only tested on Linux with ZSH and `Oh my ZSH`. We recommend you to use it to have the best experience.

Autocompletion is available for ZSH. Autocompletion for Bash is not available yet.

We also recommend you to use a terminal with a Monospace font. Meslo or Ubuntu Mono are good choices.

Don't hesitate to open an issue if you have any problem or suggestion. :-)

# Installation

## Build

### Pre-requisites

- CMake (>3.28)
- Ninja or gcc (& build-essential)
- ZSH (recommended)
- noto-fonts-emoji

### Build

#### App

```bash
git clone https://github.com/oceanspy/todoos.git
cd todoos
make
sudo make install
```

#### Autocomplete with ZSH

To get ZSH autocompletion with `oh-my-zsh`:
```
make omz-install && omz reload
```

#### Aliases & shortcuts


#### First launch & configuration 

First launch will create the required directories and files. 
You can edit the storage path and different options in `~/.oceanspy/todoos/config.conf`.

By default, lists files are stored in `~/.todoos/`.  
Configuration is stored in `~/.oceanspy/todoos/`.

## Basic usage

Show the list 
```bash
todoos 
```

Add a new item to the list
```bash
todoos add "My new item"
```

Edit an item
```bash
todoos edit {id} "My edited item"
```

Create a new list
```bash
todoos list add "My_new_list"
```

Switch to another list
```bash
todoos use "My_new_list"
```

You can change the statusEntity of an item
```bash
todoos start {id} # put the item in status "started"
```

By default, completed/cancelled items stay in the list. If you want to clean them, you can use the clean command
```bash
todoos clean
```
This will put all closed items in the archive list.
You can access the archive list by doing:
```bash
todoos --archive
```

You can also specify the list to use in the command
```bash
todoos --archive --list "My_new_list"
```

To have an overview of all commands, you can use the help command
```bash
todoos --help
```

# Configuration

```bash
# Choose where to store the lists
appDirStorage: 
# Choose the storage format: csv/json (recommended: csv)
fileDataStorageType: csv
# default list
defaultList: default
# theme
theme: default
# If you want to limit the size of the rendering (0 = use terminal width)
consoleRowMaxLength: 0
# Choose if you want to automatically archive the completed/cancelled items
archiveWhenCompleted: false
# Choose the type of ID: letters-lowercase, letters, random
idRandomGenerationType: letters-lowercase
```

## Sync

If you wish to sync the files accross multiple devices using Linux, you can use Unison. A basic configuration is available in the `Linux` directory.
