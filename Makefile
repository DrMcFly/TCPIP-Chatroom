CC = gcc
CFLAGS = -Wall
TARGET = server client
PORT = 2005

all: $(TARGET)

kill-port:
	@echo "Killing process using port $(PORT)..."
	@PID=$$(lsof -t -i :$(PORT)) && [ ! -z "$$PID" ] && kill -9 $$PID || echo "No process found on port $(PORT)"

server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f $(TARGET)