all:
	gcc -Wall -Wextra -g -pthread main.c vm.c -o vm

clean:
	rm -f vm correct.txt