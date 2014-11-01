all:spider
spider:main.o
	gcc -g main.o analysis.o trie.o -o spider -lnanomsg -lpthread 
main.o:main.c analysis.o trie.o
	gcc -g -c main.c -o main.o
analysis.o:analysis.c analysis.h trie.o
	gcc -g -c analysis.c -o analysis.o
trie.o:trie.h trie.c
	gcc -g -c trie.c -o trie.o
clean:
	rm *.o && rm spider
