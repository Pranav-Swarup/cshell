#ifndef VALIDATE_H
#define VALIDATE_H
#include "parser.h"

int validate_input(ParsedLine *pl);
char check_invalid_char(const char *str);

// Useless
int rm_emptyblocks(ParsedLine *pl);	// Also marks final connection as NONE if &

#endif
