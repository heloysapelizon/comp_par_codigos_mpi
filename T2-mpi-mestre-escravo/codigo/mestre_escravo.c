/*
 * mestre_escravo.c — Mestre/escravo na versao mais simples.
 *
 * O processo 0 (mestre) tem um saco de trabalho com exatamente uma
 * tarefa por escravo. Ele envia uma tarefa a cada um, e cada
 * escravo devolve o quadrado do numero recebido. O mestre recebe
 * as respostas por ordem de chegada (MPI_ANY_SOURCE) e guarda cada uma na
 * posicao do saco de quem a enviou (status.MPI_SOURCE).
 *
 * Limitacoes que o roteiro pede para remover:
 *   - o saco tem tantas tarefas quanto escravos, nem mais nem menos;
 *   - a iniciativa e' do mestre: o escravo nao pede trabalho;
 *   - cada escravo faz uma tarefa e termina, sem mensagem de termino.
 *
 * Compilar:  mpicc -O2 -Wall -o mestre_escravo mestre_escravo.c
 * Local:     mpirun -np 4 ./mestre_escravo
 * No LAD:    srun -N 2 -n 8 ./mestre_escravo
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_TAREFA    1
#define TAG_RESULTADO 2

int main(int argc, char *argv[]) {
    int rank, size;
    int mensagem;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) fprintf(stderr, "Use pelo menos 2 processos.\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        /* papel do mestre */
        int tarefas = size - 1;
        int *saco = malloc(tarefas * sizeof *saco);
        for (int i = 0; i < tarefas; i++)
            saco[i] = i + 1;

        for (int i = 0; i < tarefas; i++)          /* tarefa saco[i] vai para o processo i+1 */
            MPI_Send(&saco[i], 1, MPI_INT, i + 1, TAG_TAREFA, MPI_COMM_WORLD);

        for (int i = 0; i < tarefas; i++) {        /* recebe por ordem de chegada */
            MPI_Recv(&mensagem, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESULTADO,
                     MPI_COMM_WORLD, &status);
            saco[status.MPI_SOURCE - 1] = mensagem;
        }

        printf("Saco processado:");
        for (int i = 0; i < tarefas; i++)
            printf(" %d", saco[i]);
        printf("\n");
        free(saco);
    } else {
        /* papel do escravo */
        MPI_Recv(&mensagem, 1, MPI_INT, 0, TAG_TAREFA, MPI_COMM_WORLD, &status);
        mensagem = mensagem * mensagem;
        MPI_Send(&mensagem, 1, MPI_INT, 0, TAG_RESULTADO, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
