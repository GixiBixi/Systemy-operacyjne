CC      = gcc
CFLAGS  = -Wall -std=gnu99 -pedantic -g
LDFLAGS = -lrt

LIB_OBJ = mqueue_lib.o
LIB_A   = libmqueue.a
LIB_SO  = libmqueue.so

TARGETS = serwer.x klient.x

.PHONY: all clean run_static run_so_1 run_so_2 help

all: $(LIB_A) $(LIB_SO) $(TARGETS)

$(LIB_A): $(LIB_OBJ)
	ar rcs $@ $^
	@echo "Utworzono biblioteke statyczna: $@"

$(LIB_SO): mqueue_lib.c mqueue_lib.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ mqueue_lib.c $(LDFLAGS)
	@echo "Utworzono biblioteke dzielona: $@"

mqueue_lib.o: mqueue_lib.c mqueue_lib.h
	$(CC) $(CFLAGS) -c -o $@ $<

serwer.x: serwer.c common.h mqueue_lib.h $(LIB_A)
	$(CC) $(CFLAGS) -o $@ serwer.c $(LIB_A) $(LDFLAGS)

klient.x: klient.c common.h mqueue_lib.h $(LIB_A)
	$(CC) $(CFLAGS) -o $@ klient.c $(LIB_A) $(LDFLAGS)

run_static: all
	xterm -hold -title "SERWER (static)" -bg red    -fg black -e ./serwer.x &
	sleep 1
	xterm -hold -title "KLIENT1 (static)" -bg green -fg black -e ./klient.x &
	xterm -hold -title "KLIENT2 (static)" -bg green -fg black -e ./klient.x &


serwer_so.x: serwer.c common.h mqueue_lib.h $(LIB_SO)
	$(CC) $(CFLAGS) -o $@ serwer.c -L. -lmqueue $(LDFLAGS)

klient_so.x: klient.c common.h mqueue_lib.h $(LIB_SO)
	$(CC) $(CFLAGS) -o $@ klient.c -L. -lmqueue $(LDFLAGS)

run_so_1: $(LIB_SO) serwer_so.x klient_so.x
	xterm -hold -title "SERWER (so1)" -bg red    -fg black \
	    -e env LD_LIBRARY_PATH=. ./serwer_so.x &
	sleep 1
	xterm -hold -title "KLIENT1 (so1)" -bg green -fg black \
	    -e env LD_LIBRARY_PATH=. ./klient_so.x &
	xterm -hold -title "KLIENT2 (so1)" -bg green -fg black \
	    -e env LD_LIBRARY_PATH=. ./klient_so.x &

serwer_rpath.x: serwer.c common.h mqueue_lib.h $(LIB_SO)
	$(CC) $(CFLAGS) -o $@ serwer.c -L. -lmqueue \
	    -Wl,-rpath,'$$ORIGIN' $(LDFLAGS)

klient_rpath.x: klient.c common.h mqueue_lib.h $(LIB_SO)
	$(CC) $(CFLAGS) -o $@ klient.c -L. -lmqueue \
	    -Wl,-rpath,'$$ORIGIN' $(LDFLAGS)

run_so_2: $(LIB_SO) serwer_rpath.x klient_rpath.x
	xterm -hold -title "SERWER (so2)" -bg red    -fg black -e ./serwer_rpath.x &
	sleep 1
	xterm -hold -title "KLIENT1 (so2)" -bg green -fg black -e ./klient_rpath.x &
	xterm -hold -title "KLIENT2 (so2)" -bg green -fg black -e ./klient_rpath.x &

clean:
	rm -f *.o *.a *.so *.x serwer_so.x klient_so.x serwer_rpath.x klient_rpath.x

help:
	@echo "Dostepne cele:"
	@echo "  all        - kompilacja wszystkiego"
	@echo "  run_static - uruchomienie z biblioteka statyczna"
	@echo "  run_so_1   - uruchomienie z biblioteka dzielona (LD_LIBRARY_PATH)"
	@echo "  run_so_2   - uruchomienie z biblioteka dzielona (rpath)"
	@echo "  clean      - usuniecie plikow posrednich i wynikowych"