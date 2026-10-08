/*
 * deadlock.c — Dois processos que enviam antes de receber.
 *
 * Os ranks 0 e 1 trocam um vetor de n inteiros. Os dois chamam MPI_Send
 * primeiro. Se a mensagem for pequena, a biblioteca a copia para um buffer
 * interno, o Send retorna e o programa funciona. Se for grande, o Send so'
 * retorna quando o outro lado chamar Recv, e nenhum dos dois chega la.
 *
 * Compilar:  mpicc -O2 -Wall -o bin/deadlock src/deadlock.c
 * Executar:  mpirun -np 2 ./bin/deadlock 100        (termina)
 *            mpirun -np 2 ./bin/deadlock 1000000    (trava)
 *            mpirun -np 2 ./bin/deadlock 1000000 corrigido
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = (argc > 1) ? atoi(argv[1]) : 100;
    int corrigido = (argc > 2) && strcmp(argv[2], "corrigido") == 0;
    int *meu   = calloc(n, sizeof(int));
    int *outro = calloc(n, sizeof(int));
    int par = 1 - rank;

    if (!corrigido) {
        MPI_Send(meu,   n, MPI_INT, par, 0, MPI_COMM_WORLD);
        MPI_Recv(outro, n, MPI_INT, par, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Sendrecv(meu,   n, MPI_INT, par, 0,
                     outro, n, MPI_INT, par, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    printf("processo %d: troca de %d inteiros concluida\n", rank, n);
    free(meu);
    free(outro);
    MPI_Finalize();
    return 0;
}
