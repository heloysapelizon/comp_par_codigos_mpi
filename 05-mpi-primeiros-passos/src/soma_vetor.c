/*
 * soma_vetor.c — Comunicacao coletiva: MPI_Bcast, MPI_Scatter e MPI_Reduce.
 *
 * O rank 0 le o tamanho do problema e o anuncia a todos (Bcast). Depois cria
 * o vetor 1, 2, ..., n e o reparte em fatias iguais (Scatter). Cada processo
 * soma a sua fatia, e as somas parciais sao combinadas no rank 0 (Reduce).
 * O resultado esperado e' n(n+1)/2.
 *
 * n precisa ser multiplo do numero de processos.
 *
 * Compilar:  mpicc -O2 -Wall -o bin/soma_vetor src/soma_vetor.c
 * Executar:  mpirun -np 4 ./bin/soma_vetor 1000
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 0;
    if (rank == 0) n = (argc > 1) ? atoi(argv[1]) : 1000;
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (n % size != 0) {
        if (rank == 0) fprintf(stderr, "n=%d nao e' multiplo de %d\n", n, size);
        MPI_Finalize();
        return 1;
    }

    int *vetor = NULL;
    if (rank == 0) {
        vetor = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) vetor[i] = i + 1;
    }

    int fatia = n / size;
    int *parte = malloc(fatia * sizeof(int));
    MPI_Scatter(vetor, fatia, MPI_INT,
                parte, fatia, MPI_INT, 0, MPI_COMM_WORLD);

    long soma_local = 0;
    for (int i = 0; i < fatia; i++) soma_local += parte[i];
    printf("processo %d: soma da fatia = %ld\n", rank, soma_local);

    long soma_total = 0;
    MPI_Reduce(&soma_local, &soma_total, 1, MPI_LONG,
               MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("total = %ld (esperado %ld)\n",
               soma_total, (long)n * (n + 1) / 2);

    free(parte);
    free(vetor);
    MPI_Finalize();
    return 0;
}
