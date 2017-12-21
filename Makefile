CC = gcc
CFLAGS += -lncurses $(shell libmikmod-config --cflags) $(shell libmikmod-config --libs)
TARGETS = mikmodtest mikmodvis

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@
