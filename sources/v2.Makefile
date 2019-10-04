default_target: all

.PHONY: clean test

CXX = gcc
CXXFLAGS = -Wpedantic -Wall -Wextra -std=gnu99
LDFLAGS = -pthread
SOURCE = main.c
OBJECT = $(SOURCE:.c=.o)
TARGET = main

all: $(TARGETS)

$(OBJECT): $(SOURCE)
	$(CXX) $(CXXFLAGS) -c -o $@ $^

$(TARGET): $(OBJECT)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(OBJECT) $(TARGET)

test: $(TARGET)
	./$^
