#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include "../header/quiz.h"
#include "../header/question.h"
#include "../header/result.h"
#include "../header/utility.h"

// funzione che si occupa di creare la struttara quiz e inizzializzarla
struct quiz *create_quiz()
{
    struct quiz *qz = (struct quiz *)malloc(sizeof(struct quiz));
    pthread_mutex_t mutex_user;
    pthread_mutex_t mutex_score;

    struct node *node_tm;
    struct node *node_qs;
    struct node *node_u;
    struct node *node_sc;

    if (pthread_mutex_init(&mutex_user, NULL) != 0)
    {
        perror("Error pthread_mutex_init");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&mutex_score, NULL) != 0)
    {
        perror("Error pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    qz->themes = load_themes(qz);
    qz->users = load_users(qz);
    qz->results = load_scores(qz);

    // DEBUG
    if (0)
    {
        node_tm = qz->themes;
        while (node_tm != NULL)
        {
            printf("Theme %s %d\n", ((struct theme *)(node_tm->data))->title, ((struct theme *)(node_tm->data))->theme_id);
            node_qs = ((struct theme *)(node_tm->data))->questions;
            while (node_qs != NULL)
            {
                printf("Theme %d r:%s a:%s\n", ((struct theme *)(node_tm->data))->theme_id, ((struct question *)(node_qs->data))->request, ((struct question *)(node_qs->data))->answer);
                node_qs = node_qs->next;
            }

            node_tm = node_tm->next;
        }

        node_u = qz->users;
        while (node_u != NULL)
        {
            printf("User %s\n", ((struct user *)(node_u->data))->nickname);
            node_u = node_u->next;
        }

        node_sc = qz->results;
        while (node_sc != NULL)
        {
            printf("Result u:%s t:%s p:%d\n", ((struct result *)(node_sc->data))->u->nickname, ((struct result *)(node_sc->data))->tm->title, ((struct result *)(node_sc->data))->points);
            node_sc = node_sc->next;
        }
    }
    // DEBUG

    return qz;
}

// carico i temi dal file
struct node *load_themes(struct quiz *qz)
{
    FILE *fp;
    char found[1024];
    int items = 0;

    fp = fopen(QUIZ_FILE, "r");
    if (fp == NULL)
    {
        perror("Error fopen");
        exit(EXIT_FAILURE);
    }

    struct node *node_tm = NULL;

    while (fgets(found, sizeof(found), fp) != NULL)
    {
        // i temi hanno struttura "THEME - %nome_tema"
        if (strstr(found, "THEME") != NULL)
        {
            items++;
            struct theme *tm = create_theme(found + 6, items);
            tm->questions = load_question(tm);
            node_tm = insert_tail(node_tm, tm, sizeof(struct theme));
        }
    }

    if (fclose(fp) == 1)
    {
        perror("Error fclose");
        exit(EXIT_FAILURE);
    }

    return node_tm;
}

// carico i nickname dal file
struct node *load_users(struct quiz *qz)
{
    FILE *fp;
    char found[1024];

    fp = fopen(NICKNAME_FILE, "r");
    if (fp == NULL)
    {
        perror("Error fopen");
        exit(EXIT_FAILURE);
    }

    struct node *node_u = NULL;

    while (fgets(found, sizeof(found), fp) != NULL)
    {
        // i nickname degli utenti nel file saranno di utenti sconnessi
        struct user *u = create_user(found, false);
        node_u = insert_head(node_u, u, sizeof(struct user));
    }

    if (fclose(fp) == 1)
    {
        perror("Error fclose");
        exit(EXIT_FAILURE);
    }

    return node_u;
}

// carico i puntggi dal file
struct node *load_scores(struct quiz *qz)
{
    FILE *fp;
    char found[1024];

    fp = fopen(SCORE_FILE, "r");
    if (fp == NULL)
    {
        perror("Error fopen");
        exit(EXIT_FAILURE);
    }

    struct node *node_r = NULL;

    while (fgets(found, sizeof(found), fp) != NULL)
    {
        char R_nickname[MAX_NICKNAME_SIZE];
        int R_theme_id, R_points;

        // i punteggi hanno struttura "%nickname %id_del_tema %punteggio"
        char *trunk;
        trunk = strtok(found, " ");
        strcpy(R_nickname, trunk);

        trunk = strtok(NULL, " ");
        sscanf(trunk, "%d", &R_theme_id);

        trunk = strtok(NULL, " ");
        sscanf(trunk, "%d", &R_points);

        // cerco il tema relativo a quel punteggio
        struct node *node_tm = (struct node *)malloc(sizeof(struct node));
        struct theme *tm = (struct theme *)malloc(sizeof(struct theme));
        node_tm = qz->themes;
        while (node_tm != NULL)
        {
            struct theme *fnd_theme = (struct theme *)(node_tm->data);
            if (fnd_theme->theme_id == R_theme_id)
            {
                tm = fnd_theme;
            }

            node_tm = node_tm->next;
        }

        // cerco l'utente relativo a quel punteggio
        struct node *node_u = qz->users;
        struct user *u = (struct user *)malloc(sizeof(struct user));
        while (node_u != NULL)
        {
            struct user *fnd_user = (struct user *)(node_u->data);
            if (strcmp(fnd_user->nickname, R_nickname) == 0)
            {
                u = fnd_user;
            }
            node_u = node_u->next;
        }

        struct result *r = create_result(u, tm, R_points, true);

        // inserimento ordinato
        if (node_r == NULL)
            // se la lista `e vuota inserisco in testa
            node_r = insert_head(node_r, r, sizeof(struct result));
        else
        {
            // inserisco i punteggi in ordine
            insert_ordered_result(node_r, r);
        }
    }

    if (fclose(fp) == 1)
    {
        perror("Error fclose");
        exit(EXIT_FAILURE);
    }

    return node_r;
}

// return false if user exist, true otherwise
int record_user(struct quiz *qz, char nickname[])
{
    struct node *node_u = qz->users;
    struct user *u = create_user(nickname, true);

    if (strcmp(nickname, "") == 0)
    {
        // nickname vuoto
        return 0;
    }

    if (strlen(nickname) >= MAX_NICKNAME_SIZE)
    {
        // nickname troppo lungo
        return 0;
    }

    while (node_u != NULL)
    {
        struct user *tmp = (struct user *)(node_u->data);
        char *R_nickname = tmp->nickname;

        // controllo se il nickname esiste
        if (strcmp(R_nickname, nickname) == 0)
        {
            return 0;
        }

        node_u = node_u->next;
    }

    qz->users = insert_tail(qz->users, u, sizeof(struct user));

    return 1;
}

// return false if score of user U for theme TM exist, true otherwise
int score_user(struct quiz *qz, struct user *u, struct theme *tm)
{
    char *nickname = u->nickname;
    int theme = tm->theme_id;

    struct node *node_r = qz->results;

    while (node_r != NULL)
    {
        struct result *tmp_r = (struct result *)(node_r->data);
        char *R_nickname = tmp_r->u->nickname;
        int R_theme_id = tmp_r->tm->theme_id;

        if (strcmp(R_nickname, nickname) == 0 && R_theme_id == theme)
        {
            return 0;
        }

        node_r = node_r->next;
    }

    return 1;
}

// rimuovo il vecchio punteggio e aggiungo quello nuovo per mantenere ordinata la lista
void record_score(struct quiz *qz, struct user *u, struct theme *tm, int points, int completed)
{
    struct result *r;

    remove_score(qz, u, tm);

    // aggiungo il nuovo score
    r = create_result(u, tm, points, completed);

    struct node *punt = qz->results;
    insert_ordered_result(punt, r);
}

// rimuovo uno score
void remove_score(struct quiz *qz, struct user *u, struct theme *tm)
{
    int index = 0;
    struct node *node_r = qz->results;
    struct node *ret;

    while (node_r != NULL)
    {
        struct result *r = (struct result *)(node_r->data);
        if (r->tm->theme_id == tm->theme_id && strcmp(r->u->nickname, u->nickname) == 0)
        {
            struct node *head = qz->results;
            ret = (struct node *)(remove_node(&head, index));
            qz->results = head;

            if (ret == NULL)
            {
                perror("Error remove_node");
                exit(EXIT_FAILURE);
            }
            break;
        }
        index++;
        node_r = node_r->next;
    }
}

// stampa i punteggi dei giocatori
void show_score(struct quiz *qz, char *buffer)
{
    int num_tm = number_of_themes(qz) + 1;
    char tmp[1024] = {0};
    char tmp2[1024] = {0};
    int connected_user = 0;
    struct node *node_u = qz->users;
    struct node *node_r = qz->results;

    strcpy(buffer, "");
    strcpy(tmp, "");

    while (node_u != NULL)
    {
        struct user *u = (struct user *)(node_u->data);
        if (u->connected)
        {
            sprintf(tmp2, "- %s\n", u->nickname);
            strcat(tmp, tmp2);
            connected_user++;
        }
        node_u = node_u->next;
    }

    sprintf(buffer, "++++++++++++++++++++++++++++++++++++++++\nPartecipanti (%d)\n", connected_user);
    strcat(buffer, tmp);

    for (int i = 1; i < num_tm; i++)
    {
        int j = 0;
        strcpy(tmp, "");
        node_r = qz->results;

        while (node_r != NULL)
        {
            struct result *r = (struct result *)(node_r->data);

            if (r->tm->theme_id == i)
            {
                if (j == 0)
                {
                    sprintf(tmp2, "\nPunteggio tema %d\n", r->tm->theme_id);
                    strcat(tmp, tmp2);
                    j++;
                }

                sprintf(tmp2, "- %s %d\n", r->u->nickname, r->points);
                strcat(tmp, tmp2);
            }

            node_r = node_r->next;
        }
        strcat(buffer, tmp);
    }

    for (int i = 1; i < num_tm; i++)
    {
        int j = 0;
        strcpy(tmp, "");
        struct node *node_r = qz->results;
        while (node_r != NULL)
        {
            struct result *r = (struct result *)(node_r->data);
            if (r->tm->theme_id == i && r->completed == 1)
            {
                if (j == 0)
                {
                    sprintf(tmp2, "\nQuiz Tema %d comepletato\n", r->tm->theme_id);
                    strcat(tmp, tmp2);
                    j++;
                }
                sprintf(tmp2, "- %s\n", r->u->nickname);
                strcat(tmp, tmp2);
            }
            node_r = node_r->next;
        }
        strcat(buffer, tmp);
    }

    strcat(buffer, "++++++++++++++++++++++++++++++++++++++++");
}

// ritorna il numero di temi
int number_of_themes(struct quiz *qz)
{
    struct node *tm = qz->themes;
    int num = 0;

    while (tm != NULL)
    {
        tm = tm->next;
        num++;
    }

    return num;
}

// ritorna un puntatore a user con quel nickname
struct user *get_user(struct quiz *qz, char nickname[])
{
    struct user *u;
    struct node *node_u = qz->users;

    while (node_u != NULL)
    {
        u = (struct user *)(node_u->data);

        if (strcmp(u->nickname, nickname) == 0)
        {
            return u;
        }

        node_u = node_u->next;
    }

    return NULL;
}

// ritorna un puntatore a tema con quel id
struct theme *get_theme(struct quiz *qz, int theme_id)
{
    struct theme *tm;
    struct node *node_tm = qz->themes;

    while (node_tm != NULL)
    {
        tm = (struct theme *)(node_tm->data);

        if (tm->theme_id == theme_id)
        {
            return tm;
        }

        node_tm = node_tm->next;
    }

    return NULL;
}

// ritorna la struttura question numero id del tema tm
struct question *get_question_from_theme(struct theme *tm, int qs_id)
{
    struct question *qs = get_question(tm, qs_id);
    return qs;
}

// salva nickname e punteggi su file
void save_file(struct quiz *qz)
{
    struct user *u;
    struct node *node_u = qz->users;

    while (node_u != NULL)
    {
        u = (struct user *)(node_u->data);

        if (search_string_in_file(u->nickname, NICKNAME_FILE, 0) == true)
        {
            write_string_in_file(u->nickname, NICKNAME_FILE);
        }

        node_u = node_u->next;
    }

    struct result *sc;
    struct node *node_sc = qz->results;
    char buf[MAX_NICKNAME_SIZE + 10];

    while (node_sc != NULL)
    {
        sc = (struct result *)(node_sc->data);
        sprintf(buf, "%s %d %d", sc->u->nickname, sc->tm->theme_id, sc->points);

        if (search_string_in_file(buf, SCORE_FILE, 0) == true)
        {
            write_string_in_file(buf, SCORE_FILE);
        }

        node_sc = node_sc->next;
    }
}