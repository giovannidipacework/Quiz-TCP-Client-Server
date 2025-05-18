#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../header/question.h"
#include "../header/theme.h"

struct theme *create_theme(char *title, int theme_id)
{
    struct theme *tm = (struct theme *)malloc(sizeof(struct theme));
    strcpy(tm->title, title);
    if (tm->title[strlen(tm->title) - 1] == '\n')
        tm->title[strlen(tm->title) - 1] = '\0';
    tm->theme_id = theme_id;
    return tm;
}

// carico le domande relative al tema
struct node *load_question(struct theme *tm)
{

    FILE *fp;
    char found[1024] = {0};
    char stringa[6 + 12] = "THEME ";
    char integer[12] = {0};

    struct node *node_qs = NULL;

    fp = fopen(QUIZ_FILE, "r");
    if (fp == NULL)
    {
        perror("Error fopen");
        exit(EXIT_FAILURE);
    }

    sprintf(integer, "%d", tm->theme_id);
    // concateno "THEME " e id per trovarlo nel file
    strcat(stringa, integer);

    // scorro il file finche non trovo il la stringa "THEME - integer"
    while (fgets(found, sizeof(found), fp) != NULL)
    {
        if (strstr(found, stringa) != NULL)
        {
            break;
        }
    }

    for (int i = 0; i < QUESTION_NUM; i++)
    {
        // finche non a fine file o trovo una riga vuota leggi domanda e risposta
        if (fgets(found, sizeof(found), fp) != NULL || strcmp(found, "\n") == 0)
        {
            struct question *qs = create_question(found);
            node_qs = insert_tail(node_qs, qs, sizeof(struct question));
        }
    }

    if (fclose(fp) == 1)
    {
        perror("Error fclose");
        exit(EXIT_FAILURE);
    }

    return node_qs;
};

// cerco la domanda di indice question_id nel tema tm
struct question *get_question(struct theme *tm, int question_id)
{
    struct question *qs = (struct question *)malloc(sizeof(struct question));
    struct node *node_qs = (struct node *)malloc(sizeof(struct node));
    node_qs = tm->questions;

    int id = 1;
    while (node_qs != NULL && id <= question_id)
    {
        if (id == question_id)
        {
            qs = (struct question *)(node_qs->data);
            return qs;
        }

        id++;
        node_qs = node_qs->next;
    }

    return NULL;
}