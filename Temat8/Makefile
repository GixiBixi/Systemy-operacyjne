
CC      = gcc
CFLAGS  = -Wall -std=gnu99 -pedantic -g
LDFLAGS = -lrt -lpthread
LIBDIR  = .
LIB_STATIC = libutils.a
OBJ_LIB    = sem_utils.o shm_utils.o
LIB_SHARED     = libutils.so
LIB_SHARED_VER = libutils.so.1
MAIN     = main
PRODUCER = producer
CONSUMER = consumer

.PHONY: all static shared clean run_static run_shared run_shared_rpath \
        clean_ipc
 
all: static shared $(MAIN)_static $(MAIN)_shared $(MAIN)_rpath \
     $(PRODUCER)_static $(PRODUCER)_shared $(PRODUCER)_rpath \
     $(CONSUMER)_static $(CONSUMER)_shared $(CONSUMER)_rpath
 
sem_utils_pic.o: sem_utils.c sem_utils.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@
 
shm_utils_pic.o: shm_utils.c shm_utils.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@
 
sem_utils.o: sem_utils.c sem_utils.h
	$(CC) $(CFLAGS) -c $< -o $@
 
shm_utils.o: shm_utils.c shm_utils.h
	$(CC) $(CFLAGS) -c $< -o $@
 
static: $(LIB_STATIC)
 
$(LIB_STATIC): $(OBJ_LIB)
	ar rcs $@ $^
	@echo "[AR]  Zbudowano bibliotekę statyczną: $@"

shared: $(LIB_SHARED)
 
$(LIB_SHARED): sem_utils_pic.o shm_utils_pic.o
	$(CC) -shared -o $@ $^ $(LDFLAGS)
	@echo "[LD]  Zbudowano bibliotekę dzieloną: $@"

%_static.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
 
$(MAIN)_static: main_static.o $(LIB_STATIC)
	$(CC) $(CFLAGS) -o $@ $< -L. -Wl,-Bstatic -lutils -Wl,-Bdynamic $(LDFLAGS)
	@echo "[LD]  Zbudowano: $@"
 
$(PRODUCER)_static: producer_static.o $(LIB_STATIC)
	$(CC) $(CFLAGS) -o $@ $< -L. -Wl,-Bstatic -lutils -Wl,-Bdynamic $(LDFLAGS)
 
$(CONSUMER)_static: consumer_static.o $(LIB_STATIC)
	$(CC) $(CFLAGS) -o $@ $< -L. -Wl,-Bstatic -lutils -Wl,-Bdynamic $(LDFLAGS)
 
%_shared.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
 
$(MAIN)_shared: main_shared.o $(LIB_SHARED)
	$(CC) $(CFLAGS) -o $@ $< -L. -lutils $(LDFLAGS)
	@echo "[LD]  Zbudowano: $@"
 
$(PRODUCER)_shared: producer_shared.o $(LIB_SHARED)
	$(CC) $(CFLAGS) -o $@ $< -L. -lutils $(LDFLAGS)
 
$(CONSUMER)_shared: consumer_shared.o $(LIB_SHARED)
	$(CC) $(CFLAGS) -o $@ $< -L. -lutils $(LDFLAGS)

%_rpath.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
 
$(MAIN)_rpath: main_rpath.o $(LIB_SHARED)
	$(CC) $(CFLAGS) -o $@ $< -L. -lutils -Wl,-rpath,$(CURDIR) $(LDFLAGS)
	@echo "[LD]  Zbudowano: $@"
 
$(PRODUCER)_rpath: producer_rpath.o $(LIB_SHARED)
	$(CC) $(CFLAGS) -o $@ $< -L. -lutils -Wl,-rpath,$(CURDIR) $(LDFLAGS)
 
$(CONSUMER)_rpath: consumer_rpath.o $(LIB_SHARED)
	$(CC) $(CFLAGS) -o $@ $< -L. -lutils -Wl,-rpath,$(CURDIR) $(LDFLAGS)

producer: $(PRODUCER)_static
	ln -sf $(PRODUCER)_static producer
 
consumer: $(CONSUMER)_static
	ln -sf $(CONSUMER)_static consumer
 
producer_sh: $(PRODUCER)_shared
	ln -sf $(PRODUCER)_shared producer
 
consumer_sh: $(CONSUMER)_shared
	ln -sf $(CONSUMER)_shared consumer
 
producer_rp: $(PRODUCER)_rpath
	ln -sf $(PRODUCER)_rpath producer
 
consumer_rp: $(CONSUMER)_rpath
	ln -sf $(CONSUMER)_rpath consumer

clean_ipc:
	-rm -f /dev/shm/shm_zad9
	-rm -f /dev/shm/sem.sem_empty_zad9 /dev/shm/sem.sem_full_zad9 \
	        /dev/shm/sem.sem_mutex_zad9

run_static: all producer consumer clean_ipc
	@echo ""
	@echo "========================================================"
	@echo " Uruchomienie z biblioteką STATYCZNĄ"
	@echo "========================================================"
	./$(MAIN)_static
	@echo ""
	@echo "--- diff plików producenta i konsumenta ---"
	diff -s producer_data.txt consumer_data.txt

run_shared: all producer_sh consumer_sh clean_ipc
	@echo ""
	@echo "========================================================"
	@echo " Uruchomienie z biblioteką DZIELONĄ (LD_LIBRARY_PATH)"
	@echo "========================================================"
	LD_LIBRARY_PATH=$(LIBDIR):$$LD_LIBRARY_PATH ./$(MAIN)_shared
	@echo ""
	@echo "--- diff plików producenta i konsumenta ---"
	diff -s producer_data.txt consumer_data.txt

run_shared_rpath: all producer_rp consumer_rp clean_ipc
	@echo ""
	@echo "========================================================"
	@echo " Uruchomienie z biblioteką DZIELONĄ (rpath)"
	@echo "========================================================"
	./$(MAIN)_rpath
	@echo ""
	@echo "--- diff plików producenta i konsumenta ---"
	diff -s producer_data.txt consumer_data.txt

clean:
	rm -f *.o *.a *.so $(MAIN)_static $(MAIN)_shared $(MAIN)_rpath \
	      $(PRODUCER)_static $(PRODUCER)_shared $(PRODUCER)_rpath \
	      $(CONSUMER)_static $(CONSUMER)_shared $(CONSUMER)_rpath \
	      producer consumer \
	      producer_data.txt consumer_data.txt
	@echo "[CLEAN] Usunięto pliki wynikowe."


# all – kompiluje wszystko (biblioteki statyczna i dzielona + programy)
# run_static – uruchomienie z biblioteką statyczną
# run_shared – uruchomienie z biblioteką dzieloną (LD_LIBRARY_PATH)
# run_shared_rpath – uruchomienie z biblioteką dzieloną (rpath wbudowany)
# clean – usuwa pliki wynikowe
 