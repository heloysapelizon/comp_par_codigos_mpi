# CC tem valor padrao no make (cc), entao ?= nao o substituiria.
CC     = mpicc
CFLAGS ?= -O2 -Wall -Wextra -std=gnu11
LDLIBS ?= -lm

SRC_DIR = src
BIN_DIR = bin

# Com --oversubscribe o mpirun aceita mais processos que nucleos: a aula roda
# em notebooks de 4 nucleos com -np 8 sem recusa.
MPIRUN ?= mpirun --oversubscribe
NP     ?= 4

TARGETS = $(BIN_DIR)/ola \
          $(BIN_DIR)/comunicadores \
          $(BIN_DIR)/mestre_trabalhador \
          $(BIN_DIR)/deadlock \
          $(BIN_DIR)/soma_vetor \
          $(BIN_DIR)/pi_mpi

.DEFAULT_GOAL := help

help:
	@echo "=========================================================="
	@echo " Exemplo da Aula 06: Introducao ao MPI"
	@echo "=========================================================="
	@echo "Uso: make <alvo> [NP=4]"
	@echo ""
	@echo "Compilacao:"
	@echo "  all                  Compila todos os programas"
	@echo ""
	@echo "Demonstracoes (na ordem da aula):"
	@echo "  ola                  Init, rank, size e Finalize"
	@echo "  comunicadores        MPI_Comm_split: pares e impares (8 processos)"
	@echo "  mestre-trabalhador   Send e Recv, MPI_ANY_SOURCE e status"
	@echo "  deadlock             Send antes de Recv: funciona pequeno, trava grande"
	@echo "  soma-vetor           Bcast, Scatter e Reduce"
	@echo "  pi                   O pi da Aula 05 com MPI_Reduce"
	@echo "  escalabilidade       pi com 1, 2, 4 e 8 processos"
	@echo ""
	@echo "Conjuntos:"
	@echo "  test                 Verificacao rapida de todos os programas"
	@echo "  clean                Remove bin/"

all: $(BIN_DIR) $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

ola: $(BIN_DIR)/ola
	@$(MPIRUN) -np $(NP) ./$<

# Sempre 8 processos: e' o caso desenhado no slide.
comunicadores: $(BIN_DIR)/comunicadores
	@$(MPIRUN) -np 8 ./$< | sort -n -k2

mestre-trabalhador: $(BIN_DIR)/mestre_trabalhador
	@$(MPIRUN) -np $(NP) ./$<

# O limiar de 1010 inteiros e' o eager_limit de 4096 bytes do Open MPI em
# memoria compartilhada (btl vader), descontado o cabecalho. Em rede ele muda.
deadlock: $(BIN_DIR)/deadlock
	@echo ">>> 1010 inteiros (cabe no buffer interno):"
	@$(MPIRUN) -np 2 ./$< 1010
	@echo ">>> 1011 inteiros (nao cabe), com limite de 5 s:"
	@timeout 5 $(MPIRUN) -np 2 ./$< 1011 || echo "    TRAVOU: nenhum dos dois chegou ao Recv"
	@echo ">>> 1000000 inteiros com MPI_Sendrecv:"
	@$(MPIRUN) -np 2 ./$< 1000000 corrigido

soma-vetor: $(BIN_DIR)/soma_vetor
	@$(MPIRUN) -np $(NP) ./$< 1000

pi: $(BIN_DIR)/pi_mpi
	@$(MPIRUN) -np $(NP) ./$<

escalabilidade: $(BIN_DIR)/pi_mpi
	@for p in 1 2 4 8; do $(MPIRUN) -np $$p ./$<; done

test: all
	@$(MPIRUN) -np 2 ./$(BIN_DIR)/ola > /dev/null && echo "ola ok"
	@$(MPIRUN) -np 4 ./$(BIN_DIR)/comunicadores | grep -q "valor 101" && echo "comunicadores ok"
	@$(MPIRUN) -np 3 ./$(BIN_DIR)/mestre_trabalhador > /dev/null && echo "mestre_trabalhador ok"
	@$(MPIRUN) -np 2 ./$(BIN_DIR)/deadlock 100 > /dev/null && echo "deadlock (pequeno) ok"
	@$(MPIRUN) -np 4 ./$(BIN_DIR)/soma_vetor 1000 | grep -q "total = 500500" && echo "soma_vetor ok"
	@$(MPIRUN) -np 2 ./$(BIN_DIR)/pi_mpi 1000000 > /dev/null && echo "pi_mpi ok"

clean:
	rm -rf $(BIN_DIR)

.PHONY: help all clean test ola comunicadores mestre-trabalhador deadlock soma-vetor pi escalabilidade
