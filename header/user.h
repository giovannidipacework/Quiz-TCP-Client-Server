#include <stdbool.h>

#include "config.h"

struct user{
    char nickname[MAX_NICKNAME_SIZE];
    bool connected;
};

struct user *create_user(char nickname[], int connected);