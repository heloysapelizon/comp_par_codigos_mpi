/*
 * ola_mensagens.c — Todos mandam uma saudacao para o processo 0.
 *
 * Cada processo diferente de 0 monta uma string e a envia ao processo 0
 * com MPI_Send. O processo 0 recebe uma mensagem de cada um com MPI_Recv
 * e imprime. A espera aleatoria antes do envio faz os processos ficarem
 * prontos em momentos diferentes, como aconteceria com trabalho real.
 *
 * Compilar:  mpicc -O2 -Wall -o ola_mensagens ola_mensagens.c
 * Local:     mpirun -np 4 ./ola_mensagens
 * No LAD:    srun -N 2 -n 8 ./ola_mensagens
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>

#define TAG 50

int main(int argc, char *argv[]) {
    int rank, size;
    char mensagem[100];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank != 0) {
        srand(rank * 7919 + (unsigned) MPI_Wtime());
        usleep(rand() % 500000);          /* ate 0,5 s de "trabalho" */
        sprintf(mensagem, "Saudacoes do processo %d!", rank);
        MPI_Send(mensagem, strlen(mensagem) + 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
    } else {
        for (int origem = 1; origem < size; origem++) {
            MPI_Recv(mensagem, 100, MPI_CHAR, origem, TAG, MPI_COMM_WORLD, &status);
            printf("%s\n", mensagem);
        }
    }

    MPI_Finalize();
    return 0;
}
