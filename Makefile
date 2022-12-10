# ------------------- # --- VARIÁVEIS DE AMBIENTE --- # -------------------- #

CC = gcc
MPIC = mpicc
MPIR = mpirun
RM = rm -f
CFLAGS = -Wall -Wextra -lm
SEQ_BIN = seq
PAR_BIN = par
ZIP = parallel.zip
NUM_PROCS = 8
N = 8

# ------------------- # --- DIRETIVAS PRINCIPAIS --- # -------------------- #

# Execução convencional do programa
runseq:
	$(CC) pvc-seq.c -o $(SEQ_BIN) -lm -fopenmp
	./$(SEQ_BIN) $(N)
runpar:
	$(MPIC) pvc-par.c -o $(PAR_BIN) -lm -fopenmp
	$(MPIR) --use-hwthread-cpus -np $(NUM_PROCS) --oversubscribe ./$(PAR_BIN) $(N)

# Compressão dos arquivos
zip: clean
	zip -r $(ZIP) Makefile lib/* src/* obj/

# Execução do programa paralelo com Valgrind
valgrind: $(PAR_BIN)
	valgrind -s --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(PAR_BIN)

# Limpeza de objetos e de executável
clean:
	$(RM) $(SEQ_BIN) $(PAR_BIN) $(ZIP) obj/*.o
