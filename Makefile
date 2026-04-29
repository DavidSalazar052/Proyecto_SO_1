CC=gcc
CFLAGS=-Wall -Wextra -pthread -Iinclude
SRC=src/main.c src/utils.c src/truck.c src/sync.c src/queue.c src/scheduler.c src/metrics.c
OUT=bin/terminal_carga

all: $(OUT)

$(OUT): $(SRC)
	mkdir -p bin logs
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)
	rm -f logs/*.log

.PHONY: all run clean