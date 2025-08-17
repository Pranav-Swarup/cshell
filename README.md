
# C Shell

This project implements a simple C-based  custom shell. 


## Input Parsing `prompt.c | parser.c`

- Reads a raw input string, tokenizes it into a hierarchical struct order.

`entire input >>(connector)>> blocks >>(pipe if present)>> atomics >> tokens`

- blocks are separated by connectors like ;, &&, &.
- Atomics are individual commands or piped segments.
- Each atomic is an individual commands with it's args.

The parser supports basic shell features like input/output redirection (<, >, >>), background execution (&), and piping (|).

## Input Validation `validate.c`

- Detects syntax errors such as missing filenames after redirection operators or empty commands after pipes.
- Throws an error for end of line ; and && characters.
- Checks for unclosed quotations and invalid characters inside the quotes.

## Command Execution Manager `cmdrunner.c`

- Detects the command and arguments chain in a particular atomic. Checks if it is piped to a next command.
- Calls necessary functions to run the command.
- Based on function return values, it handles piping and `&&` logic. 

### Authors

- [@Pranav-Swarup](https://www.github.com/Pranav-Swarup)
## Run Locally

Clone the project

```bash
  git clone https://github.com/Pranav-Swarup/cshell
```

Go to the project directory

```bash
  cd cshell
```

Run the Makefile

```bash
  make clean
  make all
```

Run the custom C Shell

```bash
  ./shell.out
```


## Changelog and Updates

### 17/08/25

- Fixed background process marking.
- Combined command sequence print handling into one function in `cmdrunner.c`
- Fixed `validate.c` -> Final & block will be marked as CONN_NONE while being background.
- Updated .h files for parser and cmdrunner
- Only the LAST input and output redirected files are considered when multiple are chained.


### 14/08/25

- Added quotation handling logic.
- Invalidate unclosed quotes and invalid chars inside quotes.
- Added cmdrunner.c and .h 
- Migrated print function to `cmdrunner.c`.


### 13/08/25

- Redefined storage structure to a more useful hierarchical format. (Check `parser.c`)
- Added validate.c to check for invalid atomics and tokens.
- Defined & connector as a marker for background process. 
- Created README

