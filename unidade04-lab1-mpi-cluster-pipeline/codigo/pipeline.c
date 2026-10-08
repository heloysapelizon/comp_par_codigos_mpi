/*
 * pipeline.c — Esqueleto do pipeline basico.
 *
 * Os processos formam uma fila: 0 -> 1 -> 2 -> ... -> size-1. Um inteiro
 * passa de estagio em estagio; cada estagio o incrementa em 1, e o ultimo
 * mostra o valor final na tela.
 *
 * So' a estrutura do programa esta pronta. Complete as tres partes
 * marcadas com TODO seguindo o pseudocodigo do roteiro.
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

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* TODO 1: receber da esquerda.
     * O primeiro estagio cria a mensagem (valor 0); os demais recebem
     * do vizinho da esquerda. */

    /* TODO 2: processar.
     * Incrementar a mensagem em 1. */

    /* TODO 3: enviar para a direita.
     * O ultimo estagio mostra a mensagem na tela; os demais enviam
     * para o vizinho da direita. */

    (void) mensagem;   /* apague esta linha ao usar 'mensagem' */

    MPI_Finalize();
    return 0;
}
