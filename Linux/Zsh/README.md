To install autocompletion with ZSH and `oh-my-zsh`, you can use the following file: `_todoos`
and put it in ~/.oh-my-zsh/completions

(folder might need to be created manually)

Then, reload `oh-my-zsh` with:
 `source ~/.zshrc`
 `omz reload`

## Todoos aliases

### Setup 

- If you want to use the most common shortcuts, you can use the zsh file:

 ```
 cp zsh_todoos_aliases ~/.zsh_todoos_aliases
 ```

 - If you use the `unison` configuration, you can also copy `zsh_todoos_sync` to your home directory and load it in your `.zshrc`
 This will sync your files when todoos is used.

 ```
 cp zsh_todoos_sync ~/.zsh_todoos_sync
 ```

### Install 

 vi ~/.zshrc
 ```
# Todoos
source ~/.zsh_todoos_aliases
```
OR:
```
source ~/.zsh_todoos_sync
```

And in your terminal:
```
source ~/.zshrc && omz reload
```
