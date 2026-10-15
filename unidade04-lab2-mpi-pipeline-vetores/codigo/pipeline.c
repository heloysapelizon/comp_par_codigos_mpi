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

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* receber da esquerda */
    int TAM = (argc > 1) ? atoi(argv[1]) : 1000;
    int *vetor = malloc(TAM * sizeof(int));
    
    if (rank == 0)
        for (int i = 0; i < TAM; i++)
            vetor[i] = 0;                       /* o primeiro cria a mensagem */
    else
        MPI_Recv(vetor, TAM, MPI_INT, rank - 1, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &status);

    /* processar */
    for (int i = 0; i < TAM; i++)
        vetor[i] += 1;

    /* enviar para a direita */
        if (rank == size - 1) {
        /* o último confere: todos devem valer 'size' (nº de estágios) */
        int ok = 1;
        for (int i = 0; i < TAM; i++) {
            if (vetor[i] != size) {
                printf("Estagio %d (ultimo): ERRO em vetor[%d] = %d (esperado %d)\n",
                       rank, i, vetor[i], size);
                ok = 0;
                break;
            }
        }
        if (ok)
            printf("Estagio %d (ultimo): OK, todos os %d elementos = %d\n",
                   rank, TAM, size);
    } else {
        MPI_Send(vetor, TAM, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }

    free(vetor);

    MPI_Finalize();
    return 0;
}

