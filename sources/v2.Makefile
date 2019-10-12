default_target: test

.PHONY: clean test

CC = gcc
CFLAGS = -pipe -Wpedantic -Wall -Wextra -std=gnu99 -O3 -DINPUT=INPUT_STDIN -DMETHOD=METHOD_RK4 -DWITH_OMP
LDFLAGS = -static -static-libgcc -fopenmp
SOURCE = main.c
OBJECT = $(SOURCE:.c=.o)
TARGET = main
TEST = parameters.txt

$(OBJECT): $(SOURCE)
	$(CC) $(CFLAGS) -c -o $@ $^

$(TARGET): $(OBJECT)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(OBJECT) $(TARGET)

test: $(TARGET) $(TEST)
	cat $(TEST) | ./$^
