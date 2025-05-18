#include <stdbool.h>

void write_string_in_file(char string[], char filename[]);
bool search_string_in_file(char string[], char file[], int SUBSTRING);
void send_string(int sd, char *string);
void recv_string(int sd, char *string);
void send_int(int sd, int msg);
int recv_int(int sd);