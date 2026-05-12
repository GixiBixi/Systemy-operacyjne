CC      = gcc
CFLAGS  = -Wall -std=c99 -pedantic
LDFLAGS = -pthread

N_PROC    = 3
N_SEKCJI  = 2

POWIELACZ    = powielacz
WYKLUCZANIE  = wykluczanie
 
SRC_SEM  = semafor.c
OBJ_SEM  = semafor.o
 
OBJ_POW  = powielacz.o $(OBJ_SEM)
OBJ_WYK  = wykluczanie.o $(OBJ_SEM)
 
.PHONY: all clean run run_nosem
 
all: $(POWIELACZ) $(WYKLUCZANIE)

$(POWIELACZ): $(OBJ_POW)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
 
$(WYKLUCZANIE): $(OBJ_WYK)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

powielacz.o: powielacz.c semafor.h checkerror.h
	$(CC) $(CFLAGS) -c -o $@ $<
 
wykluczanie.o: wykluczanie.c semafor.h checkerror.h
	$(CC) $(CFLAGS) -c -o $@ $<
 
$(OBJ_SEM): semafor.c semafor.h
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	@echo "=== Uruchamianie Z semaforem ($(N_PROC) procesów, $(N_SEKCJI) sekcji) ==="
	./$(POWIELACZ) $(N_PROC) $(N_SEKCJI)

	@echo "=== Uruchamianie BEZ semafora ($(N_PROC) procesów, $(N_SEKCJI) sekcji) ==="
	./$(POWIELACZ) $(N_PROC) $(N_SEKCJI)

clean:
	rm -f $(POWIELACZ) $(WYKLUCZANIE) *.o numer.txt
	# Usuń semafor jeśli pozostał (ignoruj błędy)
	-rm -f /dev/shm/sem.wykluczanie_sem 2>/dev/null || true
 