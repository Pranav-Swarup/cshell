
# C Shell

This project implements a simple C-based  custom shell. 


## Input Parsing `prompt.c parser.c`

- Reads a raw input string, tokenizes it into a hierarchical struct order.

`entire input >>(connector)>> blocks >>(pipe if present)>> atomics >> tokens`

- blocks are separated by connectors like ;, &&, &.
- Atomics are individual commands or piped segments.
- Each atomic is an individual commands with it's args.

The parser supports basic shell features like input/output redirection (<, >, >>), background execution (&), and piping (|).

## Input Validation `validate.c`

- Detects syntax errors such as missing filenames after redirection operators or empty commands after pipes.
- Throws an error for end of line ; and && characters. 

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


![Logo](https://www.iiit.ac.in/wp-content/uploads/2022/06/IIIT_Hyderabad_Logo-e1655116937986.jpg)

- This Repo was created for the CS3301_OSN course under Prof. [@Karthik Vaidhyanathan](https://github.com/karthikv1392)
