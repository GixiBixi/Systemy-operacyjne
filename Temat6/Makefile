CC      = gcc
CFLAGS  = -Wall -std=gnu99 -pedantic -I.
LDFLAGS = -lrt -lpthread

LIBDIR  = lib

OBJ_SEM = obj/semafory.o
OBJ_MEM = obj/pamiec.o
OBJ_MOD = $(OBJ_SEM) $(OBJ_MEM)

LIBSTATIC  = $(LIBDIR)/libpd.a
LIBSHARED  = $(LIBDIR)/libpd.so

MAIN      = main
PROD      = producent
KONS      = konsument

PLIK_PROD = producent.txt
PLIK_KONS = konsument.txt

all: dirs $(LIBSTATIC) $(LIBSHARED) $(MAIN) $(PROD) $(KONS)

dirs:
	mkdir -p $(LIBDIR) obj

obj/semafory.o: semafory.c semafory.h common.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

obj/pamiec.o: pamiec.c pamiec.h common.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(LIBSTATIC): $(OBJ_MOD)
	ar rcs $@ $^
	@echo ">>> Biblioteka statyczna: $@"

$(LIBSHARED): $(OBJ_MOD)
	$(CC) -shared -o $@ $^ $(LDFLAGS)
	@echo ">>> Biblioteka dynamiczna: $@"

$(MAIN): main.c $(LIBSTATIC)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -Wl,-Bstatic -lpd -Wl,-Bdynamic $(LDFLAGS)

$(PROD): producent.c $(LIBSTATIC)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -Wl,-Bstatic -lpd -Wl,-Bdynamic $(LDFLAGS)

$(KONS): konsument.c $(LIBSTATIC)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -Wl,-Bstatic -lpd -Wl,-Bdynamic $(LDFLAGS)

main_dyn: main.c $(LIBSHARED)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -lpd \
	      -Wl,-rpath,$(LIBDIR) $(LDFLAGS)

producent_dyn: producent.c $(LIBSHARED)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -lpd \
	      -Wl,-rpath,$(LIBDIR) $(LDFLAGS)

konsument_dyn: konsument.c $(LIBSHARED)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -lpd \
	      -Wl,-rpath,$(LIBDIR) $(LDFLAGS)

run: all $(PLIK_PROD)
	@echo ""
	@echo "========================================================"
	@echo " Uruchomienie z biblioteka STATYCZNA"
	@echo "========================================================"
	-shm_unlink_cleanup() { true; }; \
	 rm -f $(PLIK_KONS)
	./$(MAIN)
	@echo ""
	@echo "--- Porownanie plikow producenta i konsumenta ---"
	diff -s $(PLIK_PROD) $(PLIK_KONS) || true

run_dyn: dirs $(LIBSHARED) main_dyn producent_dyn konsument_dyn $(PLIK_PROD)
	@echo ""
	@echo "========================================================"
	@echo " Uruchomienie z biblioteka DYNAMICZNA (LD_LIBRARY_PATH)"
	@echo "========================================================"
	rm -f $(PLIK_KONS)
	LD_LIBRARY_PATH=$(LIBDIR):$$LD_LIBRARY_PATH ./main_dyn
	@echo ""
	@echo "--- Porownanie plikow producenta i konsumenta ---"
	diff -s $(PLIK_PROD) $(PLIK_KONS) || true

run_dyn2: dirs $(LIBSHARED) main_dyn producent_dyn konsument_dyn $(PLIK_PROD)
	@echo ""
	@echo "========================================================"
	@echo " Uruchomienie z biblioteka DYNAMICZNA (rpath)"
	@echo "========================================================"
	rm -f $(PLIK_KONS)
	./main_dyn
	@echo ""
	@echo "--- Porownanie plikow producenta i konsumenta ---"
	diff -s $(PLIK_PROD) $(PLIK_KONS) || true

clean:
	rm -f $(MAIN) $(PROD) $(KONS) \
	      main_dyn producent_dyn konsument_dyn \
	      obj/*.o $(LIBDIR)/*.a $(LIBDIR)/*.so \
	      $(PLIK_KONS)
	@echo "Wyczyszczono."

.PHONY: all dirs run run_dyn run_dyn2 clean