all:crawler
crawler:main.o
	gcc -g main.o analysis.o trie.o connserver.o threadpool.o hash.o bloom.o -o crawler -lnanomsg -lpthread -levent -levent_pthreads 
main.o:main.c analysis.o trie.o connserver.o analysis.o threadpool.o hash.o bloom.o
	gcc -g -c main.c -o main.o
analysis.o:analysis.c analysis.h trie.o threadpool.o
	gcc -g -c analysis.c -o analysis.o
connserver.o:connserver.c connserver.h common.h
	gcc -g -c connserver.c -o connserver.o
trie.o:trie.h trie.c
	gcc -g -c trie.c -o trie.o
threadpool.o:threadpool.c threadpool.h
	gcc -g -c threadpool.c -o threadpool.o
bloom.o:bloom.c bloom.h
	gcc -g -c bloom.c -o bloom.o
hash.o:hash.c hash.h
	gcc -g -c hash.c -o hash.o
clean:
	rm *.o && rm crawler
