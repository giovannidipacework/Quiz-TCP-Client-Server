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

// scrivo la stringa "string" nel file "filename"
void write_string_in_file(char string[], char filename[])
{

    FILE *fp;
    fp = fopen(filename, "a");

    if (fp == NULL)
    {
        perror("Error fopen");
        _exit(EXIT_FAILURE);
    }

    fprintf(fp, "\n%s", string);

    if (fclose(fp) == 1)
    {
        perror("Error fclose");
        _exit(EXIT_FAILURE);
    }
}

// ritorna TRUE se string non `e presente nel file, FALSE altrimenti
bool search_string_in_file(char string[], char file[], int SUBSTRING)
{
    FILE *fp;
    char found[1024];

    fp = fopen(file, "r+");
    if (fp == NULL)
    {
        perror("Error fopen");
        _exit(EXIT_FAILURE);
    }

    while (fgets(found, sizeof(found), fp) != NULL)
    {
        if (strcmp(found, string) == 0 && SUBSTRING == false)
        {
            return false;
        }

        if (strstr(found, string) != NULL && SUBSTRING == false)
        {
            return false;
        }
    }

    if (fclose(fp) == 1)
    {
        perror("Error fclose");
        _exit(EXIT_FAILURE);
    }

    return true;
}

//invio la stringa su socket sd
void send_string(int sd, char *string)
{
    int ret;
    int len = strlen(string);
    uint32_t u_len = htonl(len);

    ret = send(sd, &u_len, sizeof(u_len), 0);
    if (ret != sizeof(u_len))
    {
        printf("Error send_string\n");
        close(sd);
        pthread_exit(NULL);
    }

    ret = send(sd, string, len, 0);
    if (ret != len)
    {
        printf("Error send_string\n");
        close(sd);
        pthread_exit(NULL);
    }

}

//ricevo la stringa su socket sd
void recv_string(int sd, char *string)
{
    int ret;
    int len;
    uint32_t u_len;

    ret = recv(sd, &u_len, sizeof(u_len), 0);
    if (!ret)
    {
        printf("Disconnected\n");
        close(sd);
        pthread_exit(NULL);
    }
    if (ret != sizeof(u_len))
    {
        printf("Error recv_string\n");
        close(sd);
        pthread_exit(NULL);
    }

    len = ntohl(u_len);
    ret = recv(sd, string, len, 0);
    if (!ret)
    {
        printf("Disconnected\n");
        close(sd);
        pthread_exit(NULL);
    }
    if (ret != len)
    {
        printf("%d\n", ret);
        printf("%d\n", len);
        printf("qui\n");
        printf("Error recv_string\n");
        close(sd);
        pthread_exit(NULL);
    }

    string[len] = '\0';
}

//invio l' intero su socket sd
void send_int(int sd, int msg)
{
    int ret;
    uint32_t u_msg = htonl(msg);

    ret = send(sd, &u_msg, sizeof(uint32_t), 0);
    if (ret != sizeof(uint32_t))
    {
        printf("Error send_int\n");
        close(sd);
        pthread_exit(NULL);
    }
}

//ricevo l' intero su socket sd
int recv_int(int sd)
{
    int ret;
    uint32_t u_msg;

    ret = recv(sd, &u_msg, sizeof(uint32_t), 0);
    if (ret != sizeof(uint32_t))
    {
        printf("Error recv_int\n");
        close(sd);
        pthread_exit(NULL);
    }
    return ntohl(u_msg);
}