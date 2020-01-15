#TO MAKEFILE MOU
#sta flags isws thelei -Wall

OBJS1	= jobExecutor.o worker_client.o server.o doc_map.o worker_trie.o worker_utils.o
OUT1	= jobExecutor
CC	= gcc
FLAGS	= -c
LIB 	= -lm

# test sort
jobExecutor.o: jobExecutor.c
	$(CC) $(FLAGS) jobExecutor.c
server.o: server.c
	$(CC) $(FLAGS) server.c
worker_client.o: worker_client.c
	$(CC) $(FLAGS) worker_client.c
worker_trie.o: worker_trie.c
	$(CC) $(FLAGS) worker_trie.c
doc_map.o: doc_map.c
	$(CC) $(FLAGS) doc_map.c
worker_utils.o: worker_utils.c
	$(CC) $(FLAGS) worker_utils.c
t1: $(OBJS1)
	$(CC) $(OBJS1) $(LIB) -o $(OUT1)


# clean up
clean:
	rm -f $(OBJS1) $(OUT1)
