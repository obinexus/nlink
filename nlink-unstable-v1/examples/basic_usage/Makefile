# Makefile for NexusLink basic example
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -L../../lib -lnexus_core -ldl -lpthread
INCLUDES = -I../../include

TARGET = nexus_basic_example

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $(INCLUDES) main.c -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	LD_LIBRARY_PATH=../../lib:$(LD_LIBRARY_PATH) ./$(TARGET)

.PHONY: all clean run
