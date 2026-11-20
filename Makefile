all:
	gcc -Wall -Wextra main.c vm.c -o vm

clean:
	rm -f vm correct.txt