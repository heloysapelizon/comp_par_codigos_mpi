/*
 * pipeline.c — Pipeline basico: ponto de partida do Laboratorio 2.
 *
 * E' a solucao do Laboratorio 1. Os processos formam uma fila
 * 0 -> 1 -> ... -> size-1; um inteiro passa de estagio em estagio, cada
 * estagio o incrementa em 1 e o ultimo mostra o valor final.
 *
 * O roteiro pede para trocar o inteiro por um vetor, fazer varias
 * mensagens atravessarem o pipeline e medir o tempo.
 *
 * Compilar:  mpicc -O2 -Wall -o pipeline pipeline.c
 * Local:     mpirun -np 4 ./pipeline
 * No LAD:    srun -N 2 -n 8 ./pipeline
 */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int mensagem;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* receber da esquerda */
    if (rank == 0)
        mensagem = 0;                       /* o primeiro cria a mensagem */
    else
        MPI_Recv(&mensagem, 1, MPI_INT, rank - 1, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &status);

    /* processar */
    mensagem += 1;

    /* enviar para a direita */
    if (rank == size - 1)
        printf("Estagio %d (ultimo): mensagem = %d\n", rank, mensagem);
    else
        MPI_Send(&mensagem, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}
