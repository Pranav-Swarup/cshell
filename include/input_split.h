#ifndef SPLIT_H
#define SPLIT_H

#include <stdlib.h>

char** split_into_words(const char* input, int* word_count);
void free_words(char** words);

#endif /* SPLIT_H */
