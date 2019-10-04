default_target: main

main:
	gcc -std=gnu99 -pthread -o main main.c

clean:
	rm -f main

test: main
	./main
