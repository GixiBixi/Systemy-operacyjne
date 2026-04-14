CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -pedantic

UTILS   = pipe_utils.o

INPUT   = input.txt
OUTPUT4 = output_ex4.txt
OUTPUT5 = output_ex5.txt
OUTPUT5A= output_ex5a.txt
FIFO    = /tmp/ex5_fifo

all: ex4 ex5_producer ex5_consumer ex5a

pipe_utils.o: pipe_utils.c pipe_utils.h
	$(CC) $(CFLAGS) -c pipe_utils.c
 
ex4: ex4_main.o $(UTILS)
	$(CC) $(CFLAGS) -o ex4 ex4_main.o $(UTILS)
 
ex5_producer: ex5_producer.o $(UTILS)
	$(CC) $(CFLAGS) -o ex5_producer ex5_producer.o $(UTILS)
 
ex5_consumer: ex5_consumer.o $(UTILS)
	$(CC) $(CFLAGS) -o ex5_consumer ex5_consumer.o $(UTILS)
 
ex5a: ex5a_main.o $(UTILS)
	$(CC) $(CFLAGS) -o ex5a ex5a_main.o $(UTILS)
 
ex4_main.o: ex4_main.c pipe_utils.h
	$(CC) $(CFLAGS) -c ex4_main.c
 
ex5_producer.o: ex5_producer.c pipe_utils.h
	$(CC) $(CFLAGS) -c ex5_producer.c
 
ex5_consumer.o: ex5_consumer.c pipe_utils.h
	$(CC) $(CFLAGS) -c ex5_consumer.c
 
ex5a_main.o: ex5a_main.c pipe_utils.h
	$(CC) $(CFLAGS) -c ex5a_main.c
 

$(INPUT):
	echo "To jest przykladowy tekst do przetestowania potoków." > $(INPUT)
	echo "Producent czyta ten plik porcjami i wysyla przez potok." >> $(INPUT)
	echo "Konsument odbiera dane i zapisuje do pliku wyjsciowego." >> $(INPUT)
	echo "Na koncu oba pliki powinny byc identyczne." >> $(INPUT)
 

run4: ex4 $(INPUT)
	@echo ""
	@echo ">>> Uruchamiam Cwiczenie 4: potok nienazwany <<<"
	@echo ""
	./ex4 $(INPUT) $(OUTPUT4)
	@echo ""
	@echo ">>> Porownanie plikow <<<"
	diff -s $(INPUT) $(OUTPUT4)
 

run5a: ex5a $(INPUT)
	@echo ""
	@echo ">>> Uruchamiam Cwiczenie 5(a): FIFO, jeden program <<<"
	@echo ""
	./ex5a $(INPUT) $(OUTPUT5A)
	@echo ""
	@echo ">>> Porownanie plikow <<<"
	diff -s $(INPUT) $(OUTPUT5A)
 
# ------------------------------------------------------------------ #
# Uruchomienie cwiczenia 5(b) - niezalezne procesy w osobnych oknach #
# Wymaga srodowiska graficznego z xterm.                             #
# ------------------------------------------------------------------ #
run5b: ex5_producer ex5_consumer $(INPUT)
	@echo ""
	@echo ">>> Uruchamiam Cwiczenie 5(b): FIFO, niezalezne procesy <<<"
	@echo ""
	mkfifo $(FIFO) 2>/dev/null || true
	xterm -hold -title "Konsument" -e ./ex5_consumer $(FIFO) $(OUTPUT5) &
	xterm -hold -title "Producent" -e ./ex5_producer $(FIFO) $(INPUT) &
	@echo "Uruchomiono dwa okna xterm. Poczekaj na zakonczenie obu procesow."
	@echo "Nastepnie sprawdz: diff -s $(INPUT) $(OUTPUT5)"
	@echo "Potok FIFO mozna usunac recznie: rm -f $(FIFO)"
 
# ------------------------------------------------------------------ #
# Sprzatanie                                                          #
# ------------------------------------------------------------------ #
clean:
	rm -f *.o ex4 ex5_producer ex5_consumer ex5a
	rm -f $(OUTPUT4) $(OUTPUT5) $(OUTPUT5A)
	rm -f $(FIFO) /tmp/ex5a_fifo
 
.PHONY: all run4 run5a run5b clean