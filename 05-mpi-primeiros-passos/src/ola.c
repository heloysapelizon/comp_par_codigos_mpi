/*
 * ola.c — O esqueleto de todo programa MPI.
 *
 * Cada processo descobre quem e' (rank) e quantos sao (size), imprime uma
 * linha e termina. O rank 0 imprime uma linha diferente das outras: e' o
 * mesmo fonte, mas o 'if' faz cada copia seguir um caminho.
 *
 * Compilar:  mpicc -O2 -Wall -o bin/ola src/ola.c
 * Executar:  mpirun -np 4 ./bin/ola
 */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
        printf("Sou o processo 0: somos %d no total\n", size);
    else
        printf("Ola do processo %d de %d\n", rank, size);

    MPI_Finalize();
    return 0;
}
