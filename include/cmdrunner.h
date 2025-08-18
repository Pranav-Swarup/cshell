#ifndef CMDRUNNER_H
#define CMDRUNNER_H
#include "parser.h"

int print_cmd(ParsedLine* p);
int dispatch_cmd(ParsedLine* p);

// Useless
const char* conn_type(ConnectorType type, const ParsedLine* parsedline);
int run_block(CommandBlock *block);

#endif
