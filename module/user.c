#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../header/user.h"

struct user *create_user(char nickname[], int connected)
{
    struct user *u = (struct user *)malloc(sizeof(struct user));
    strcpy(u->nickname, nickname);
    if( u->nickname[strlen(u->nickname)-1] == '\n')
        u->nickname[strlen(u->nickname)-1] = '\0';
    u->connected = connected;

    return u;
};