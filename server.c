#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>

#include "header/config.h"
#include "header/quiz.h"
#include "header/question.h"
#include "header/result.h"
#include "header/utility.h"

void *start_quiz(void *i);
struct quiz *qz;

void sigpipe(int sig){}

int main(int argc, char *argv[])
{
    int ret;
    int server_sd, client_sd;

    int option = 1;
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_len = sizeof(server_addr);

    int port = 8080;

    signal(SIGPIPE, sigpipe);

    if (argc == 2)
    {
        // se il comando ho 2 argomenti il secondo `e la porta
        port = atoi(argv[1]);
    }

    // socket TCP
    server_sd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (server_sd == -1)
    {
        perror("Error creation socket");
        _exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1)
    {
        perror("Error binding socket");
        close(server_sd);
        exit(EXIT_FAILURE);
    }

    ret = listen(server_sd, MAX_CLIENTS);
    if (ret == -1)
    {
        perror("Error listen socket");
        close(server_sd);
        _exit(EXIT_FAILURE);
    }
    printf("Server PID: %d\nListening on port: %d\n", getpid(), port);

    qz = create_quiz();

    // scrivo i nomi dei temi su console
    char names_themes[1048] = {0};
    char buffer[1024] = {0};
    struct node *tm_node = (struct node *)malloc(sizeof(struct node));
    tm_node = qz->themes;

    while (tm_node != NULL)
    {
        struct theme *fnd_theme = (struct theme *)(tm_node->data);
        strcpy(buffer, fnd_theme->title);
        strcat(names_themes, strcat(buffer, "\n"));

        tm_node = tm_node->next;
    }
    printf("Trivia Quiz \n++++++++++++++++++++++++++++++++++++++++\nTemi:\n%s++++++++++++++++++++++++++++++++++++++++\n", names_themes);

    while (1)
    {
        client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &server_len);

        // creo un nuovo thread e gli passo il socket
        pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));

        int a = client_sd;
        int *v = (int *)malloc(sizeof(*v));
        *v = a;

        ret = pthread_create(thread, NULL, start_quiz, v);
        if (ret)
        {
            perror("Errore creazione del thread");
            exit(EXIT_FAILURE);
        }
    }

    close(server_sd);
    printf("Closing server \n");
    return 0;
}

void *start_quiz(void *i)
{
    int num_themes = 0;
    int thread = *(int *)i;

    int chosen_theme = 1;
    int endquiz = 0;
    int points, ret, completed;
    char nickname[MAX_NICKNAME_SIZE] = {0};
    char names_themes[1024] = {0};
    char buffer[1024] = {0};
    char integer[12] = {0};

    struct node *tm_node = (struct node *)malloc(sizeof(struct node));
    struct theme *tm = (struct theme *)malloc(sizeof(struct theme));
    struct node *qs_node = (struct node *)malloc(sizeof(struct node));
    struct node *qs_node_prec = (struct node *)malloc(sizeof(struct node));
    struct question *qs = (struct question *)malloc(sizeof(struct question));
    struct user *u = (struct user *)malloc(sizeof(struct user));

    // gestione del thread
    ret = 0;
    while (ret == 0)
    {
        recv_string(thread, nickname);

        // to lower case
        for (char *p = nickname; *p; ++p)
            *p = *p > 0x40 && *p < 0x5b ? *p | 0x60 : *p;

        pthread_mutex_lock(&qz->mutex_score);
        // registro un nuovo utente
        ret = record_user(qz, nickname);
        pthread_mutex_unlock(&qz->mutex_score);

        // invio il risultato della registrazione, 0 fallita, 1 effettuata
        send_int(thread, ret);
    }
    u = get_user(qz, nickname);

    show_score(qz, buffer);
    printf("%s\n", buffer);

    //++++++++++++++++++++++++++++++++++++++++
    while (1)
    {
        // invio il numero di temi
        printf("Invio num temi\n");
        num_themes = number_of_themes(qz);
        send_int(thread, num_themes);

        tm_node = qz->themes;

        // invio i nomi dei temi
        strcpy(names_themes, "");
        while (tm_node != NULL)
        {
            struct theme *fnd_theme = (struct theme *)(tm_node->data);
            strcpy(buffer, fnd_theme->title);
            strcat(names_themes, strcat(buffer, "\n"));

            tm_node = tm_node->next;
        }
        endquiz = 0;

        send_string(thread, names_themes);

        ret = false;
        while (ret == false)
        {
            // ricevo il tema scelto
            chosen_theme = recv_int(thread);
            sprintf(integer, " %d", chosen_theme);

            // controllo se l'utente abbia gia fatto il quiz su quel tema
            tm = get_theme(qz, chosen_theme);
            ret = score_user(qz, u, tm);

            // invio 0 se l'utente ha gia fatto quel quiz
            send_int(thread, ret);
        }

        points = 0;
        pthread_mutex_lock(&qz->mutex_score);
        // registro un nuovo punteggio per quel tema
        record_score(qz, u, tm, points, false);
        pthread_mutex_unlock(&qz->mutex_score);


        qs_node_prec = NULL;
        qs_node = tm->questions;
        while (endquiz == 0)
        {
            int exist;
            // cerco la domanda numero 'id' del tema 'theme' e la invio se esiste
            if (qs_node == NULL)
            {
                // se non ci sono piu domande invio 0 al client, e registro il quiz come completato
                exist = 0;
                completed = 1;
                send_int(thread, exist);
                break;
            }
            else
            {
                qs = (struct question *)qs_node->data;
                exist = 1;
                send_int(thread, exist);
                qs_node_prec = qs_node;
                qs_node = qs_node->next;
            }

            //invia domanda
            send_string(thread, qs->request);

            // ricevi risposta/comando
            recv_string(thread, buffer);

            // se ricevo endquiz rimuovo il punteggio
            if (strcmp(buffer, "endquiz") == 0)
            {
                completed = 0;
                endquiz = 1;

                pthread_mutex_lock(&qz->mutex_score);
                // se l'utente fa endquiz rimuovo il punteggio per quel quiz
                remove_score(qz, u, tm);
                pthread_mutex_unlock(&qz->mutex_score);

                show_score(qz, buffer);
                printf("%s\n", buffer);

                break;
            }
            // se ricevo showscore mostro il punteggio
            else if (strcmp(buffer, "showscore") == 0)
            {
                show_score(qz, buffer);
                send_string(thread, buffer);

                printf("%s\n", buffer);
                // il client si aspetta una stringa, normalmente una domanda, in questo caso non lo `e quindi devo tornare alla domanda precedente
                qs_node = qs_node_prec;
            }
            else
            {
                // ricevo la risposta
                if (strcmp(buffer, qs->answer) == 0)
                {
                    send_string(thread, "Risposta corretta\n");
                    points++;
                }
                else
                {
                    send_string(thread, "Rispotta errata\n");
                }
                record_score(qz, u, tm, points, false);
            }
            show_score(qz, buffer);
            printf("%s\n", buffer);
        }

        if (completed == 1)
        {
            pthread_mutex_lock(&qz->mutex_score);
            // registro il quiz come completato
            record_score(qz, u, tm, points, 1);
            pthread_mutex_unlock(&qz->mutex_score);

            show_score(qz, buffer);
            printf("%s\n", buffer);
        }
    }
    //++++++++++++++++++++++++++++++++++++++++
    // "disconnetto" il client nella struttura dati
    u->connected = false;
    printf("THREAD %d disconnect\nServer: %d\n", thread, getpid());
    close(thread);

    // salvo su file i nuovi nickname e i nuovi punteggi
    save_file(qz);

    pthread_exit(NULL);
}