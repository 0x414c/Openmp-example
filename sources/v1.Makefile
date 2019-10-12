default_target: test

.PHONY: clean test

main: main.c
	gcc -pipe -Wpedantic -Wall -Wextra -std=gnu99 -O3 -static -static-libgcc -fopenmp -DINPUT=INPUT_STDIN -DMETHOD=METHOD_RK4 -DWITH_OMP -o main main.c

clean:
	rm -f main

test: main parameters.txt
	cat parameters.txt | ./main
