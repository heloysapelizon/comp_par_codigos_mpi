/*
 * mestre_trabalhador.c — Comunicacao ponto a ponto com MPI_Send e MPI_Recv.
 *
 * O rank 0 (mestre) envia um numero a cada trabalhador. Cada trabalhador
 * calcula o quadrado e devolve. O mestre recebe as respostas com
 * MPI_ANY_SOURCE, na ordem em que chegam, e descobre quem respondeu pelo
 * campo status.MPI_SOURCE.
 *
 * Compilar:  mpicc -O2 -Wall -o bin/mestre_trabalhador src/mestre_trabalhador.c
 * Executar:  mpirun -np 4 ./bin/mestre_trabalhador
 */

#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

#define TAG_TAREFA   1
#define TAG_RESPOSTA 2

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        for (int dest = 1; dest < size; dest++) {
            int tarefa = 10 * dest;
            MPI_Send(&tarefa, 1, MPI_INT, dest, TAG_TAREFA, MPI_COMM_WORLD);
        }
        for (int i = 1; i < size; i++) {
            int resposta;
            MPI_Status status;
            MPI_Recv(&resposta, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESPOSTA,
                     MPI_COMM_WORLD, &status);
            printf("mestre: recebi %5d do processo %d\n", resposta, status.MPI_SOURCE);
        }
    } else {
        int tarefa;
        MPI_Recv(&tarefa, 1, MPI_INT, 0, TAG_TAREFA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        usleep(1000 * (size - rank));   /* trabalho de duracao desigual */
        int resposta = tarefa * tarefa;
        MPI_Send(&resposta, 1, MPI_INT, 0, TAG_RESPOSTA, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
