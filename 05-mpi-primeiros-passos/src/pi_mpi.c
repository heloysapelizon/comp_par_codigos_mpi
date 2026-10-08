/*
 * pi_mpi.c — O pi da Aula 05, agora com processos.
 *
 * Mesma integral de 4/(1+x^2) em [0,1] pela regra do ponto medio. Cada
 * processo soma um bloco contiguo de retangulos e MPI_Reduce combina as
 * somas parciais no rank 0. E' o 'reduction(+:soma)' do OpenMP com a
 * comunicacao escrita por extenso.
 *
 * O tempo e' medido com MPI_Wtime entre duas barreiras: a primeira alinha a
 * largada, e o tempo reportado e' o do processo mais lento (MPI_MAX).
 *
 * Compilar:  mpicc -O2 -Wall -o bin/pi_mpi src/pi_mpi.c
 * Executar:  mpirun -np 4 ./bin/pi_mpi 100000000
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long n = (argc > 1) ? atol(argv[1]) : 100000000L;
    double passo = 1.0 / (double)n;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    long inicio = rank * n / size;
    long fim    = (rank + 1) * n / size;
    double soma_local = 0.0;
    for (long i = inicio; i < fim; i++) {
        double x = (i + 0.5) * passo;
        soma_local += 4.0 / (1.0 + x * x);
    }

    double soma = 0.0;
    MPI_Reduce(&soma_local, &soma, 1, MPI_DOUBLE,
               MPI_SUM, 0, MPI_COMM_WORLD);

    double t_local = MPI_Wtime() - t0, t;
    MPI_Reduce(&t_local, &t, 1, MPI_DOUBLE,
               MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("pi = %.10f  erro = %.1e  processos = %d  tempo = %.3f s\n",
               soma * passo, fabs(soma * passo - M_PI), size, t);

    MPI_Finalize();
    return 0;
}
