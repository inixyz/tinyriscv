CC := gcc
FLAGS := -Wall -O2

SRC := src/*.c
TARGET := tinyriscv.out

all: 
	$(CC) $(SRC) $(FLAGS) -o $(TARGET)

.PHONY: clean
clean: 
	rm $(TARGET)