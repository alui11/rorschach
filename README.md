CSE.30341.FA17: Project 01
==========================

[Project 01]:       https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/project01.html
[CSE.30341.FA17]:   https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/


Overview
-------

Rorschach is a file watching service which scans a directory for any file creations, deletions, or modifications to files underneath this root folder and executes user-specified actions on the files based on pattern matching rules. 
By default rorschach will load in pattern rules from the file `rules` unless otherwise specified by the -f flag.

Rules File
-------
The rules file contails a series of rules (one per line) specified in the following format:
```
EVENT   PATTERN     ACTION
```
The comment delimiter is `#`. `PATTERN` indicates a shell pattern and `ACTION` indicates a shell command.

Environment Variables
-------
Rorscach will set the following environment variables for each event:

1. *BASEPATH*: This is the base path of the file (i.e. without any proceeding extensions).
2. *FULLPATH*: This is the full path of the file.
3. *EVENT*: This is the type of event detected.
4. *TIMESTAMP*: This is the current timestamp.

Usage
-------
The full set of rorschach command-line options are show below.
```
$ ./rorschach -h
Usage: rorschach [options] ROOT

Options:
    -h          Print this help message
    -f RULES    Load rules from this file (default is rules)
    -t SECONDS  Time between scans (default is 5 seconds)
```

Building Rorschach
-------
```shell
$ make          # Builds rorschach
$ make clean    # Remove rorschach and any intermediate files
```

Contributors
-------

1. Alison Lui (alui@nd.edu)
2. Ivy Wang (jwang28@nd.edu)

