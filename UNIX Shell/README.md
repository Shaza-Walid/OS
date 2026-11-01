# UNIX Shell (`osh`)

A simple UNIX command-line shell written in C. It reads a command, runs it in a separate child process, and supports background execution, command history, input/output redirection, and a pipe between two commands.

**Course:** CSE 323 – Operating Systems, Faculty of Engineering, Zagazig University
**Project:** Programming Project #2 – UNIX Shell
**Author:** Shada Walid Farid Mohamed

Based on *Project 1 – UNIX Shell* from Chapter 3 of *Operating System Concepts* (10th Edition).

---

## Features

| Feature | Example | Description |
|---|---|---|
| Command execution | `ls -l` | The shell forks a child, which runs the command with `execvp()`; the parent waits with `waitpid()`. |
| Background execution | `sleep 10 &` | A trailing `&` makes the parent skip waiting, so the prompt returns immediately. |
| History | `!!` | Re-runs the most recent command. Prints `No commands in history.` if there is none. |
| Output redirection | `ls > output.txt` | Redirects standard output to a file using `dup2()`. |
| Input redirection | `sort < output.txt` | Reads standard input from a file using `dup2()`. |
| Pipe | `ls \| wc -l` | Connects the output of one command to the input of another using `pipe()` and `dup2()`. |
| Exit | `exit` | Terminates the shell. |

## System calls used

`fork()`, `execvp()`, `waitpid()`, `dup2()`, `pipe()`, `open()`, `close()`

## Build and run

Requires a Linux/UNIX system with GCC (tested on Kali Linux 2026.2).

```bash
gcc shell.c -o shell
./shell
```

## Example session

```text
osh> pwd
/home/kali/Documents/OS/CSE323_Project2
osh> !!
pwd
/home/kali/Documents/OS/CSE323_Project2
osh> ls > output.txt
osh> cat output.txt
output.txt
shell
shell.c
osh> sort < output.txt
output.txt
shell
shell.c
osh> ls | wc -l
3
osh> sleep 10 &
osh> exit
```

## How it works

1. The main loop runs while `should_run` is 1 and prints the `osh>` prompt.
2. The line is read with `fgets()` and split into tokens with `strtok()`.
3. The tokens are scanned for the special symbols `&`, `|`, `<` and `>`; the remaining tokens form the argument arrays passed to `execvp()`.
4. Depending on the command:
   - **Pipe:** the shell creates a pipe and two children. The first writes to the pipe, the second reads from it, and the parent closes both ends and waits for both.
   - **Redirection / plain command:** the shell forks one child, which opens the file (if any), redirects standard input or output with `dup2()`, and calls `execvp()`.
5. The parent waits for the child unless the command ended with `&`.

## Assumptions and limitations

As specified in the project description:

- A command contains **either** one redirection **or** one pipe, not both (and not several of them).
- Maximum command length is 80 characters.
- Tokens, including `&`, `|`, `<` and `>`, must be separated by spaces (e.g. `ls | wc -l`, `sleep 10 &`).
