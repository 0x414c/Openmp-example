default_target: main

main:
	g++ -std=gnu99 -pthread -o main main.c

clean:
	rm -f main

test: main
	./main
