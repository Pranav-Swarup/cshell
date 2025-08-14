#ifndef CMDRUNNER_H
#define CMDRUNNER_H
#include "parser.h"

const char* conn_type(ConnectorType type, const ParsedLine* parsedline);
int print_cmd(ParsedLine* p);

#endif
