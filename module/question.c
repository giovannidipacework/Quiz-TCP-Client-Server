#include <stdlib.h>
#include <string.h>

#include "../header/question.h"

struct question *create_question(char *string)
{
    char *trunk;
    char request[MAX_REQUEST_SIZE];
    char answer[MAX_ANSWER_SIZE];

    struct question *qs = (struct question *)malloc(sizeof(struct question));

    trunk = strtok(string, "?");
    strcpy(request,trunk);
    strcat(request, "?");
    trunk = strtok(NULL, "");
    strcpy(answer,trunk+1);
    answer[strlen(answer)-1] = '\0';

    strcpy(qs->request, request);
    strcpy(qs->answer, answer);

    return qs;
}