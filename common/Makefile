VERSION := 0.2a
CC	:= gcc
PREFIX  := /usr
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS    := $(OBJECTS:.o=.deps)
LIB     := libvlc-server-common.a
CFLAGS  := -g -fpie -fpic -Wall -DVERSION=\"$(VERSION)\" -DPREFIX=\"$(PREFIX)\" -I include ${EXTRA_CFLAGS}

all: $(LIB) 

$(LIB): $(OBJECTS) 
	rm -f $(LIB)
	ar q $(LIB) $(OBJECTS) 

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -I include src -c -o $@ $<

clean:
	rm -f build/* $(LIB)

-include $(DEPS)

.PHONY: clean


