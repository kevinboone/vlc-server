TARGET  := $(NAME) 
PREFIX  := /usr
BINDIR  := $(DESTDIR)/$(PREFIX)/bin

VERSION := 0.2a

all: $(TARGET)

install:
	mkdir -p $(BINDIR) 
	install -m 755 vlc-server-get-stations.pl vlc-server-add-station-list.sh ${BINDIR}
	
clean:

