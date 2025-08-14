#ifndef VALIDATE_H
#define VALIDATE_H
#include "parser.h"

int validate_input(ParsedLine *pl);
char check_invalid_char(const char *str);
int rm_emptyblocks(ParsedLine *pl);

#endif
