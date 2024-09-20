all: shaman


WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2
FILES = main.c sensors-view.c sensors-info.c
CFLAGS=`pkg-config --cflags gtk4`
LIBS=`pkg-config --libs gtk4` -lsensors


shaman: Makefile
	$(CC) $(CFLAGS) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(FILES) $(LIBS)

clean:
	rm -f shaman

install:
	echo "Installing is not supported"


run:
	./shaman

