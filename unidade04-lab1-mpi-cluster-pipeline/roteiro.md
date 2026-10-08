# Roteiro de Laboratório 1: Primeiros passos com MPI no cluster

**Unidade 04** | Continuação da Aula 06 e da aula sobre o cluster do LAD.
Entrega pelo Moodle ao final da aula.

## Objetivos

- Compilar e executar um programa MPI no cluster Atlântica do LAD.
- Observar como os processos são distribuídos entre os nós do cluster.
- Usar `MPI_Send` e `MPI_Recv`, e entender o efeito de `MPI_ANY_SOURCE` na ordem de
  recebimento.
- Implementar um pipeline básico a partir de um pseudocódigo.

## Preparação

Os exercícios rodam no cluster Atlântica. A Atlântica não é acessível diretamente da sua
máquina: entre antes na `sparta` e, de lá, na Atlântica, com a conta do grupo, como visto
na aula sobre o cluster. Troque a senha padrão no primeiro acesso.

```bash
ssh usuario_pucrs@sparta.pucrs.br
ssh usuario_lad@atlantica.lad.pucrs.br
yppasswd
```

Como a Atlântica fica atrás da `sparta`, um `scp` da sua máquina não chega até ela. Para
levar os arquivos de `codigo/` ao cluster, use uma destas alternativas:

- **Colar o conteúdo no terminal.** Na Atlântica, rode `cat > ola_mpi.c`, cole o conteúdo
  do arquivo, tecle Enter e depois Ctrl+D para encerrar. Repita para cada arquivo.
- **Usar um repositório no GitHub.** Coloque os arquivos num repositório do grupo e clone
  na Atlântica com `git clone https://github.com/usuario/repositorio.git`. Para
  atualizar depois de editar na sua máquina, `git pull`. Se o repositório for privado, o
  `git clone` pede um token de acesso do GitHub no lugar da senha.

Instalar o Open MPI na sua máquina é opcional, mas ajuda a testar rápido antes de ir ao
cluster: `sudo apt install openmpi-bin libopenmpi-dev`. Os comandos mudam pouco entre os
dois ambientes:

| | Máquina local | Cluster (Atlântica) |
|------|----------|----------|
| Compilar | `mpicc -O2 -Wall -o prog prog.c` | igual |
| Executar | `mpirun -np 4 ./prog` | `srun -N 2 -n 4 ./prog` |
| Excedendo os núcleos | `mpirun --oversubscribe -np 16 ./prog` | `srun --oversubscribe -N 2 -n 200 ./prog` |

No `srun`, `-N` é o número de nós e `-n` o número total de processos.

---

## Parte 1: Olá, mundo no cluster

`ola_mpi.c` imprime, para cada processo, o seu `rank`, o total de processos e o nome do nó
em que ele está rodando.

1. Compile na Atlântica e rode variando nós e processos:

   ```bash
   mpicc -O2 -Wall -o ola_mpi ola_mpi.c
   srun -N 1 -n 4 ./ola_mpi
   srun -N 2 -n 4 ./ola_mpi
   srun -N 2 -n 32 ./ola_mpi
   ```

2. Descubra quantos núcleos um nó da Atlântica oferece com `srun -N 1 nproc` e
   `srun -N 1 lscpu`.

3. **Responda:** com `-N 2 -n 4`, quais ranks ficaram em cada nó: em blocos consecutivos
   (0 e 1 num nó, 2 e 3 no outro) ou alternados? Rode de novo acrescentando
   `--distribution=cyclic` e compare. Por que essa distribuição importa para um programa
   em que cada processo conversa principalmente com o vizinho de rank?

## Parte 2: Mensagens e ordem de chegada

Em `ola_mensagens.c`, todo processo diferente de 0 espera um tempo aleatório e envia uma
saudação ao processo 0, que recebe e imprime.

1. Compile e rode várias vezes no cluster, com 8 ou mais processos:

   ```bash
   mpicc -O2 -Wall -o ola_mensagens ola_mensagens.c
   srun -N 2 -n 8 ./ola_mensagens
   ```

2. **Responda:** a ordem das mensagens muda entre as execuções? Por que não, se cada
   processo fica pronto num momento diferente?

3. Troque, no `MPI_Recv` do processo 0, a origem `origem` por `MPI_ANY_SOURCE`, e faça o
   processo 0 imprimir também `status.MPI_SOURCE`. Recompile e rode de novo.

4. **Responda:** o que mudou na ordem? Num programa em que o processo 0 distribui trabalho
   e espera os resultados, qual das duas formas de receber deixa o processo 0 menos tempo
   parado?

## Parte 3: Pipeline básico

Num pipeline, os processos formam uma fila: o processo 0 é o primeiro estágio e o processo
`size-1` é o último. Cada estágio recebe uma mensagem do vizinho da esquerda, processa e
envia ao vizinho da direita. O pseudocódigo:

```c
MPI_Init();
my_rank = MPI_Comm_rank();
proc_n  = MPI_Comm_size();

// receber da esquerda
if (my_rank == 0)                     // sou o primeiro?
    message = 0;                      // sim: crio a mensagem sem receber
else
    MPI_Recv(message, my_rank - 1);   // não: recebo da esquerda

// processar
message += 1;

// enviar para a direita
if (my_rank == proc_n - 1)            // sou o último?
    printf("Mensagem: %d", message);  // sim: mostro na tela
else
    MPI_Send(message, my_rank + 1);   // não: envio para a direita

MPI_Finalize();
```

1. Complete os três `TODO` de `pipeline.c` seguindo o pseudocódigo. Use a assinatura
   completa de `MPI_Send` e `MPI_Recv`, como em `ola_mensagens.c`.

2. Teste no cluster com diferentes números de processos:

   ```bash
   mpicc -O2 -Wall -o pipeline pipeline.c
   srun -N 2 -n 16 ./pipeline
   ```

3. **Responda:** que valor o último estágio imprime com `N` processos? O programa
   funciona com um único processo (`-n 1`)? Se não funcionar, corrija.

Guarde o `pipeline.c`: ele é o ponto de partida do Laboratório 2.

---

## Erros comuns

| Sintoma | Causa provável |
|---|---|
| `There are not enough slots available` no `mpirun` | mais processos que núcleos; acrescente `--oversubscribe` |
| O programa trava sem imprimir nada | um `MPI_Recv` espera uma mensagem que ninguém envia: confira origem, destino e tag dos dois lados |
| `srun` fica parado em `job ... queued and waiting for resources` | outros grupos estão usando os nós; aguarde ou peça menos nós |
| `Permission denied` no `ssh` | conta ou senha erradas, ou acesso de fora da rede sem passar pela `sparta` |

## Entrega

- O `ola_mensagens.c` com `MPI_ANY_SOURCE` e o `pipeline.c` completo.
- As respostas das três partes.
