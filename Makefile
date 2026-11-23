all:
	gcc -Wall -Wextra -pthread main.c vm.c -o vm

clean:
	rm -f vm correct.txt