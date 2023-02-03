NAME    := vlc-server-client
VERSION := 0.1a
CC      := gcc
LIBPATH1 := ../api
LIBPATH2 := ../common
LIBS    := ${EXTRA_LIBS} -lvlc -lcurl -lvlc-server-client -lvlc-server-common -lpthread
TARGET	:= $(NAME)
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS	:= $(OBJECTS:.o=.deps)
DESTDIR := /
PREFIX  := /usr
MANDIR  := $(DESTDIR)/$(PREFIX)/share/man
BINDIR  := $(DESTDIR)/$(PREFIX)/bin
SHARE   := $(DESTDIR)/$(PREFIX)/share/$(TARGET)
CFLAGS  := -g -fpie -fpic -Wall -Werror -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\" -DSHARE=\"$(SHARE)\" -DPREFIX=\"$(PREFIX)\" ${EXTRA_CFLAGS}
LDFLAGS := -pie ${EXTRA_LDFLAGS}
INCLUDE1 := ../api/include
INCLUDE2 := ../common/include

$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) -L$(LIBPATH1) -L$(LIBPATH2) $(LIBS) 

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -I$(INCLUDE1) -I$(INCLUDE2) -iquote src -c -o $@ $<

install: $(TARGET)
	mkdir -p $(BINDIR) 
	mkdir -p $(MANDIR)/man1
	strip $(TARGET)
	install -m 755 $(TARGET) ${BINDIR}
	install -m 644 man1/* ${MANDIR}/man1

clean:
	$(RM) -r build/ $(TARGET) 

-include $(DEPS)

.PHONY: clean

