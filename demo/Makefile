# NexusLink Demo Application Makefile
# Aegis Project - Waterfall Methodology

CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -I../include -DETPS_ENABLED=1
LDFLAGS = -L../lib -lnlink -lm -lpthread

DEMO_SOURCES = nlink_demo.c
DEMO_BINARY = nlink_demo

.PHONY: all clean run

all: $(DEMO_BINARY)

$(DEMO_BINARY): $(DEMO_SOURCES)
	@echo "Building demo application..."
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "✓ Demo built successfully"

run: $(DEMO_BINARY)
	@echo "Running NexusLink demo..."
	@LD_LIBRARY_PATH=../lib ./$(DEMO_BINARY)

run-export: $(DEMO_BINARY)
	@echo "Running demo with event export..."
	@LD_LIBRARY_PATH=../lib ./$(DEMO_BINARY) --export demo_events.json

clean:
	rm -f $(DEMO_BINARY) *.o demo_events.json

install: $(DEMO_BINARY)
	@mkdir -p ../bin
	@cp $(DEMO_BINARY) ../bin/
	@echo "✓ Demo installed to bin/"
