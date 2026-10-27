# Roteiro de Laboratório 3: Mestre/escravo com saco de trabalho e speed-up

**Unidade 04** | Continuação do Laboratório 2.
Entrega pelo Moodle ao final da aula.

## Objetivos

- Implementar o modelo mestre/escravo com um saco de trabalho maior que o número
  de escravos, distribuído sob demanda.
- Usar tags para distinguir tipos de mensagem (pedido, tarefa, resultado, término).
- Usar alocação dinâmica para um saco de trabalho em forma de matriz.
- Medir speed-up e eficiência no cluster e relacioná-los com a granularidade das tarefas.

## Preparação

Leve os arquivos de `codigo/` para a Atlântica como no Laboratório 1: colando com
`cat > arquivo.c` ou pelo repositório do grupo no GitHub.

`mestre_escravo.c` é a versão mais simples do modelo: uma tarefa por escravo,
cada escravo devolve o quadrado do número recebido. `saco_sequencial.c` é a versão
sequencial da Parte 2: ordena, com Bubble Sort, cada vetor de uma matriz.

---

## Parte 1: Saco de trabalho distribuído sob demanda

1. Compile e rode `mestre_escravo.c` com 4 e com 8 processos:

   ```bash
   mpicc -O2 -Wall -o mestre_escravo mestre_escravo.c
   srun -N 1 -n 4 ./mestre_escravo
   ```

   Leia o código e observe as três limitações listadas no comentário do início do
   arquivo.

2. Reescreva o programa para que ele funcione com um saco de `T` tarefas (por exemplo,
   `T = 100`) e **qualquer** número de escravos:
   - A iniciativa é do escravo: ao começar, ele envia ao mestre um pedido de
     trabalho.
   - O mestre responde a cada pedido com a próxima tarefa do saco ou, se o saco
     estiver vazio, com uma mensagem de término.
   - O escravo processa a tarefa e devolve o resultado; para o mestre, cada
     resultado também vale como pedido da próxima tarefa.
   - O mestre recebe de quem ficar pronto primeiro (`MPI_ANY_SOURCE`) e guarda cada
     resultado **na posição original** do saco. Para isso, o índice da tarefa precisa ir e
     voltar junto com ela: envie um vetor de dois inteiros, com o índice e o valor.

   Use tags diferentes para cada tipo de mensagem e confira, no fim, que o saco contém
   os quadrados de 1 a `T` em ordem.

3. Meça com `MPI_Wtime` o tempo do mestre, do início da distribuição até o último
   resultado recebido. Rode no cluster com 2, 4, 8 e 16 processos e aumente `T` até 40000.
   Não passe disso: o quadrado de um número acima de 46340 não cabe em um `int`.

4. **Responda:** o tempo caiu na proporção do número de escravos? Compare o custo de
   calcular um quadrado com o custo de uma ida e volta de mensagem, e diga quem passa a
   ser o gargalo quando há muitos escravos.

## Parte 2: Saco de vetores

Agora cada tarefa é um vetor inteiro: o saco é uma matriz de `NV` vetores de `TAM`
inteiros, e cada escravo ordena o vetor que recebe. Ordenar com Bubble Sort custa
O(`TAM`²), muito mais que enviar o vetor, que custa O(`TAM`).

1. Compile e rode a versão sequencial, e anote o tempo: ele é a referência do speed-up.
   Rode-a no cluster, no mesmo tipo de nó em que vai rodar a versão paralela.

   ```bash
   gcc -O2 -Wall -o saco_sequencial saco_sequencial.c
   srun -N 1 ./saco_sequencial 200 5000
   ```

   Observe como a matriz é alocada. A declaração

   ```c
   int (*saco)[tam] = malloc(nv * sizeof *saco);
   ```

   reserva um bloco contíguo em que `saco[i][j]` funciona como numa matriz comum e
   `saco[i]` é o endereço da linha `i`, pronto para ser passado a `MPI_Send`.
   Um vetor declarado como `int saco[200][5000]` dentro de uma função ficaria na pilha e
   estouraria com tamanhos maiores.

2. Adapte a sua versão da Parte 1: o saco passa a ser essa matriz, cada tarefa é uma linha,
   o escravo aplica `bs()` à linha recebida e devolve a linha ordenada. Leia `NV` e
   `TAM` da linha de comando, como em `saco_sequencial.c`, e confira no fim que todas as
   linhas estão ordenadas.

   Mandar o índice numa posição extra obrigaria a copiar cada linha para um vetor maior
   antes do envio. Há um caminho mais simples: como cada escravo
   tem no máximo uma linha de cada vez, o mestre pode guardar numa tabela qual linha
   entregou a cada escravo. Ele só sabe de quem é um resultado depois de recebê-lo
   (`status.MPI_SOURCE`): receba num vetor auxiliar de `TAM` inteiros e copie para a
   linha certa do saco com `memcpy`.

3. Faça o mestre contar quantas tarefas cada escravo processou e imprimir a
   contagem no fim.

4. Rode no cluster com nós exclusivos (`--exclusive` impede que outro grupo divida o nó
   com você durante a medição) e complete a tabela:

   ```bash
   srun --exclusive -N 2 -n 8 ./mestre_escravo 200 5000
   ```

   | Processos | Tempo (s) | Speed-up | Eficiência por processo | Eficiência por escravo |
   |---|---|---|---|---|
   | 2 | | | | |
   | 4 | | | | |
   | 8 | | | | |
   | 16 | | | | |
   | 32 | | | | |

   Com `P` processos há `P − 1` escravos: a última coluna divide o speed-up por
   `P − 1`, e não por `P`.

5. **Responda:**
   - Com 2 processos, por que o speed-up fica perto de 1, e não de 2? Das duas colunas de
     eficiência, qual descreve melhor o uso da máquina?
   - Com 32 processos, as tarefas ficaram bem distribuídas entre os escravos? O que
     acontece no fim da execução quando `NV` não é muito maior que o número de
     escravos? Aumente `NV` e veja se a eficiência melhora.

---

## Erros comuns

| Sintoma | Causa provável |
|---|---|
| O programa não termina | algum escravo não recebeu a mensagem de término, ou o mestre espera mais resultados do que enviou tarefas |
| Saco final fora de ordem | o resultado foi guardado pela ordem de chegada, não pelo índice da tarefa |
| `Segmentation fault` com `NV` ou `TAM` grandes | matriz declarada como vetor local, na pilha, em vez de alocada com `malloc` |
| Speed-up muito instável entre execuções | medição sem `--exclusive`, dividindo o nó com outros grupos |

## Entrega

- O `mestre_escravo.c` final (saco de vetores com iniciativa do escravo).
- A tabela da Parte 2 e as respostas das duas partes.
