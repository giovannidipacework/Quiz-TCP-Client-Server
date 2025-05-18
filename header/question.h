#include "config.h"

struct question{
    char request[MAX_REQUEST_SIZE];
    char answer[MAX_ANSWER_SIZE];
};

struct question *create_question(char *string);