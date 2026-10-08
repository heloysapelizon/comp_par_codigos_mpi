/*
 * comunicadores.c — MPI_COMM_WORLD e dois subgrupos criados por MPI_Comm_split.
 *
 * O grupo (rank % 2, parametro color) decide o comunicador: pares ou
 * impares. A key (rank) decide a ordem dentro dele, entao os ranks novos
 * seguem a ordem dos antigos. Depois da divisao, um MPI_Bcast no comunicador
 * 'metade' so' alcanca o proprio grupo: cada raiz manda um valor diferente,
 * e os grupos nao se misturam.
 *
 * Compilar:  mpicc -O2 -Wall -o bin/comunicadores src/comunicadores.c
 * Executar:  mpirun -np 8 ./bin/comunicadores
 */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int grupo = rank % 2;                   /* 0: pares, 1: impares */
    MPI_Comm metade;
    MPI_Comm_split(MPI_COMM_WORLD, grupo, rank, &metade);

    int rank_m, size_m;
    MPI_Comm_rank(metade, &rank_m);
    MPI_Comm_size(metade, &size_m);

    int valor = (rank_m == 0) ? 100 + grupo : -1;
    MPI_Bcast(&valor, 1, MPI_INT, 0, metade);

    printf("WORLD %d de %d -> %-7s %d de %d: valor %d\n",
           rank, size, grupo ? "impares" : "pares",
           rank_m, size_m, valor);

    MPI_Comm_free(&metade);
    MPI_Finalize();
    return 0;
}
