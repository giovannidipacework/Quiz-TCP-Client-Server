#!/bin/bash

# Compilare il file server.c
gcc -o gcc -o server server.c module/quiz.c module/question.c module/result.c module/utility.c module/user.c module/theme.c module/list.c -Wall
if [ $? -ne 0 ]; then
    echo "Errore durante la compilazione di server.c"
    exit 1
fi

# Compilare il file client.c
gcc -o client client.c module/utility.c  -Wall
if [ $? -ne 0 ]; then
    echo "Errore durante la compilazione di client.c"
    exit 1
fi

# Eseguire il file server
./server &
server_pid=$!

# Aprire un nuovo terminale ed eseguire il file client
gnome-terminal -- bash -c "./client; exec bash"

# Attendere che il server termini
wait $server_pid
