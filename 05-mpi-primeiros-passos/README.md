# 05 — MPI: primeiros passos

Exemplo de apoio da **Aula 06 (Unidade 04: Programação Paralela com Troca de
Mensagens)**. Todos os trechos de código e todas as saídas que aparecem nos
slides saem daqui.

```bash
make            # lista os alvos
make all        # compila tudo
make test       # verificação rápida
make ola NP=8   # qualquer demonstração, com o número de processos que quiser
```

Precisa de `mpicc` e `mpirun` (Open MPI): `sudo apt install openmpi-bin libopenmpi-dev`.

## Os programas

| Fonte | Do que trata | Alvo |
|---|---|---|
| [`src/ola.c`](src/ola.c) | `MPI_Init`, `MPI_Comm_rank`, `MPI_Comm_size`, `MPI_Finalize`; o `rank` decide | `make ola` |
| [`src/comunicadores.c`](src/comunicadores.c) | `MPI_Comm_split` em pares e ímpares; um `MPI_Bcast` que só alcança o próprio grupo | `make comunicadores` |
| [`src/mestre_trabalhador.c`](src/mestre_trabalhador.c) | `MPI_Send`, `MPI_Recv`, `MPI_ANY_SOURCE` e `status.MPI_SOURCE` | `make mestre-trabalhador` |
| [`src/deadlock.c`](src/deadlock.c) | Dois `Send` antes de dois `Recv`; a correção com `MPI_Sendrecv` | `make deadlock` |
| [`src/soma_vetor.c`](src/soma_vetor.c) | `MPI_Bcast`, `MPI_Scatter` e `MPI_Reduce` | `make soma-vetor` |
| [`src/pi_mpi.c`](src/pi_mpi.c) | O `pi.c` da Aula 05 com `MPI_Reduce`, medido com `MPI_Wtime` | `make pi`, `make escalabilidade` |

## O deadlock depende do tamanho da mensagem

`deadlock.c` termina com 1010 inteiros e trava com 1011. O limiar é o
`btl_vader_eager_limit` do Open MPI (4096 bytes, cabeçalho incluído): abaixo
dele, `MPI_Send` copia a mensagem para um buffer interno e retorna; acima,
espera o `MPI_Recv` do outro lado. O padrão MPI não garante nenhum dos dois
comportamentos, então o programa está errado nos dois casos. Ele só não
aparenta estar quando a mensagem é pequena.

```bash
ompi_info --param btl vader --level 9 | grep eager_limit
```
