CC := gcc
CCFLAGS := -o 

all: client naming_server storage_server

client: client.c command/*.c
	$(CC) $(CCFLAGS) client client.c command/*.c

naming_server: naming_server.c nmutils/*.c
	$(CC) $(CCFLAGS) naming_server naming_server.c nmutils/*.c

storage_server: storage_server.c ssutils/*.c
	$(CC) $(CCFLAGS) storage_server storage_server.c ssutils/*.c

clean: 
	rm -f client naming_server storage_server
