/*
 * ola_mpi.c — Cada processo diz quem e', quantos sao e em que no' esta.
 *
 * MPI_Get_processor_name devolve o nome da maquina onde o processo roda.
 * Na maquina local todos imprimem o mesmo nome; no cluster, o nome mostra
 * como o SLURM distribuiu os processos entre os nos.
 *
 * Compilar:  mpicc -O2 -Wall -o ola_mpi ola_mpi.c
 * Local:     mpirun -np 4 ./ola_mpi
 * No LAD:    srun -N 2 -n 8 ./ola_mpi
 */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size, tam_nome;
    char nome[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(nome, &tam_nome);

    printf("Processo %2d de %2d no no' %s\n", rank, size, nome);

    MPI_Finalize();
    return 0;
}
