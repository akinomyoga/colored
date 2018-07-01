# colored

Highlight the output of commands in my preferences

```bash
colored ls ...
colored diff ...
```

## Setup

### Requirements

- GNU Make
- C++11 Compiler
- POSIX 2003 API
- ncursesw (optional)

### Download & Build

```console
$ git clone git@github.com:akinomyoga/colored.git
$ cd colored
$ make
```

If you do not have `ncursesw` library headers, please use the following command instead of `make`.

```console
$ make use_ncurses=no
```

### Install

Using the following command The `colored` can be installed to the specified directory (e.g., `/path/to/install` in the following example). If `PREFIX` is not specified, the default install position is `$HOME/.mwg`. To use the `colored` command, please ensure to have `/path/to/install/bin` in your `PATH` environment variable.

```console
$ make PREFIX=/path/to/install install
```

With the following aliases you will have highlighted outputs of `ls` and `diff`.

```bash
alias ls='colored ls'
alias diff='colored diff'
```
