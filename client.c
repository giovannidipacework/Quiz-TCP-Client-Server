#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#include "header/config.h"
#include "header/utility.h"

#define BUF_SIZE 1024

// funzione che ridefinisce il comportamentamento del segnale SIGPIPE
void sigpipe(int signal)
{
    printf("Server disconnected\n");
}

// funzione che chiede all'utente di immettere da tastiera un interno compreso tra min e max
int choose_between(int min, int max)
{
    int choice = 0;
    char buf[12];

    scanf("%s", buf);
    sscanf(buf, "%d", &choice);
    while (choice < min || choice > max)
    {
        printf("Scelta non valida: '%d'\nScegli un numero tra %d e %d\n", choice, min, max);
        scanf("%s", buf);
        sscanf(buf, "%d", &choice);
        continue;
    }

    return choice;
}

int main(int argc, char *argv[])
{

    int choice, ret, number_of_themes;

    int endquiz = 0;
    int port = 8080;
    char buffer[BUF_SIZE] = {0};
    char names_themes[2048] = {0};
    char integer[12] = {0};
    char quiz_title[1024] = {0};

    struct sockaddr_in server_addr;
    int client_sd;

    signal(SIGPIPE, sigpipe);

    if (argc == 2)
    {
        // se il comando ho 2 argomenti il secondo `e la porta
        port = atoi(argv[1]);
    }

    while (endquiz == 0)
    {
        client_sd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_sd == -1)
        {
            perror("Errore crezione socket");
            exit(EXIT_FAILURE);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        ret = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
        if (ret == -1)
        {
            perror("Errore pton socket");
            close(client_sd);
            _exit(EXIT_FAILURE);
        }

        printf("Trivia Quiz \n++++++++++++++++++++++++++++++++++++++++ \nMenu: \n1 - Comincia una sessione di Trivia \n2 - Esci \n++++++++++++++++++++++++++++++++++++++++ \nLa tua scelta: ");

        // scelgo tra 2 scelte
        choice = choose_between(1, 2);

        if (choice == 2)
        {
            // se scelgo 2 chiudo il client
            close(client_sd);
            printf("Fine \n");
            return 0;
        }

        ret = connect(client_sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret == -1)
        {
            // se scelgo 1 mi connetto al server
            perror("Errore connect socket");
            close(client_sd);
            exit(EXIT_FAILURE);
        }

        printf("\nTrivia Quiz \n++++++++++++++++++++++++++++++++++++++++ \nScegli un nickname (deve essere univoco): ");

        ret = false;
        while (ret == false)
        {
            // nickname non univoco
            scanf("%s", buffer);
            send_string(client_sd, buffer);
            ret = recv_int(client_sd);
            if (ret == false)
            {
                printf("Scegli un altro nickname: ");
            }
        }

        //++++++++++++++++++++++++++++++++++++++++
        while (1)
        {   
            endquiz = 0;
            // ricevo il numero di temi
            number_of_themes = recv_int(client_sd);
            // ricevo i nomi dei temi
            strcpy(names_themes, "");
            recv_string(client_sd, names_themes);
            printf("\nQuiz disponibili\n++++++++++++++++++++++++++++++++++++++++ \n");
            printf("%s", names_themes);
            printf("++++++++++++++++++++++++++++++++++++++++ \nLa tua scelta: ");

            ret = false;
            while (ret == false)
            {
                // l'utente deve scegliere tra 1 e il numero dei temi
                choice = choose_between(1, number_of_themes);
                // invio il tema scelto
                send_int(client_sd, choice);
                ret = recv_int(client_sd);
                if (ret == false)
                {
                    printf("Scegli un altro tema: ");
                }
            }

            // prendo il nome del quiz dalla lista
            strcpy(quiz_title, "");
            sprintf(integer, "%d", choice);

            strtok(names_themes, integer);
            strcpy(quiz_title, strtok(NULL,"\n")+2);
            
            printf("\nQuiz - %s\n++++++++++++++++++++++++++++++++++++++++\n", quiz_title);

            while (endquiz == 0)
            {
                // ricevo ret == 1, se  esiste un'altra domanda
                ret = recv_int(client_sd);
                if (ret == 0)
                {
                    printf("Fine domande del quiz\n");
                    break;
                }

                // ricevo la domanda
                strcpy(buffer, "");
                recv_string(client_sd, buffer);
                printf("%s\n\nRisposta: ", buffer);

                // leggo la risposta da tastiera tenendo conto che possa contenere eventuali spazi
                scanf(" %[^\n]s", buffer);
                printf("\n");

                // invio la risposta
                send_string(client_sd, buffer);
                // se l'utente da il comando endquiz devo uscire dai 2 while e ricominciare dalla scelta del nickname
                if (strcmp(buffer, "endquiz") == 0)
                {
                    endquiz = 1;
                    break;
                }
                // ricevo "Rispota corretta" o "Risposta errata"
                recv_string(client_sd, buffer);
                printf("%s\n", buffer);
            }
        }
        //++++++++++++++++++++++++++++++++++++++++
        close(client_sd);
        printf("Disconnessione\n\n");
    }

    return 0;
}
