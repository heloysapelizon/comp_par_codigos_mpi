/*
 * saco_sequencial.c — O saco de trabalho processado por um processo so'.
 *
 * O saco e' uma matriz de NV linhas por TAM colunas, alocada dinamicamente
 * em um bloco contiguo. Cada linha e' uma unidade de trabalho: um vetor
 * em ordem decrescente (pior caso) que precisa ser ordenado com o Bubble
 * Sort. E' a versao sequencial de referencia para calcular o speed-up da
 * versao mestre/escravo.
 *
 * A declaracao  int (*saco)[tam]  faz 'saco' apontar para linhas de 'tam'
 * inteiros: saco[i][j] funciona como numa matriz comum, e saco[i] e' o
 * endereco do inicio da linha i, pronto para MPI_Send.
 *
 * Compilar:  gcc -O2 -Wall -o saco_sequencial saco_sequencial.c
 * Executar:  ./saco_sequencial [num_vetores] [tamanho_vetor]
 *            ./saco_sequencial 200 5000
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Bubble Sort com parada antecipada: O(n^2), de proposito caro */
void bs(int n, int *vetor) {
    int c = 0, trocou = 1;
    while (c < n - 1 && trocou) {
        trocou = 0;
        for (int d = 0; d < n - c - 1; d++)
            if (vetor[d] > vetor[d + 1]) {
                int troca = vetor[d];
                vetor[d] = vetor[d + 1];
                vetor[d + 1] = troca;
                trocou = 1;
            }
        c++;
    }
}

static double agora(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

int main(int argc, char *argv[]) {
    int nv  = argc > 1 ? atoi(argv[1]) : 200;
    int tam = argc > 2 ? atoi(argv[2]) : 5000;

    int (*saco)[tam] = malloc(nv * sizeof *saco);
    if (saco == NULL) {
        fprintf(stderr, "Memoria insuficiente para %d x %d inteiros\n", nv, tam);
        return 1;
    }

    for (int i = 0; i < nv; i++)               /* pior caso: ordem decrescente */
        for (int j = 0; j < tam; j++)
            saco[i][j] = tam - j + i;

    double t1 = agora();
    for (int i = 0; i < nv; i++)
        bs(tam, saco[i]);
    double t2 = agora();

    for (int i = 0; i < nv; i++)               /* confere: linha i deve ser i+1, i+2, ... */
        if (saco[i][0] != i + 1 || saco[i][tam - 1] != tam + i) {
            printf("ERRO: vetor %d fora de ordem\n", i);
            return 1;
        }

    printf("%d vetores de %d inteiros ordenados em %.3f s\n", nv, tam, t2 - t1);
    free(saco);
    return 0;
}
