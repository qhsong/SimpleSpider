all:crawler
crawler:main.o
	gcc -g main.o analysis.o trie.o connserver.o -o crawler -lnanomsg -lpthread -levent -levent_pthreads 
main.o:main.c analysis.o trie.o connserver.o
	gcc -g -c main.c -o main.o
analysis.o:analysis.c analysis.h trie.o
	gcc -g -c analysis.c -o analysis.o
connserver.o:connserver.c connserver.h common.h
	gcc -g -c connserver.c -o connserver.o
trie.o:trie.h trie.c
	gcc -g -c trie.c -o trie.o
clean:
	rm *.o && rm crawler
