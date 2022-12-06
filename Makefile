# ------------------- # --- VARIÁVEIS DE AMBIENTE --- # -------------------- #

CC = gcc
MPIC = mpicc
MPIR = mpirun
NUM_PROCS = 4
RM = rm -f
CFLAGS = -Wall -Wextra -lm
SEQ_BIN = seq
PAR_BIN = par
ZIP = parallel.zip

# ------------------- # --- DIRETIVAS PRINCIPAIS --- # -------------------- #

# Global
all: $(SEQ_BIN) $(PAR_BIN)

# Produção do executável
$(SEQ_BIN):
	$(CC) pvc-seq.c -o $(SEQ_BIN) -lm
$(PAR_BIN):
	$(MPIC) pvc-par.c -o $(PAR_BIN) -lm

# Execução convencional do programa
runseq: $(SEQ_BIN)
	./$(SEQ_BIN)
runpar: $(PAR_BIN)
	$(MPIR) -np $(NUM_PROCS) ./$(PAR_BIN)

# Compressão dos arquivos
zip: clean
	zip -r $(ZIP) Makefile lib/* src/* obj/

# Execução do programa paralelo com Valgrind
valgrind: $(PAR_BIN)
	valgrind -s --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(PAR_BIN)

# Limpeza de objetos e de executável
clean:
	$(RM) $(SEQ_BIN) $(PAR_BIN) $(ZIP) obj/*.o
