#ifndef REDIR_H
#define REDIR_H

#include "parser.h"

int apply_redirection(Atomic *atomic);
void restore_redirection();

#endif
