CC=gcc
LIBS=`pkg-config --cflags --libs gstreamer-1.0 gstreamer-pbutils-1.0`
EXTRA_WARNINGS=-Wall -W -Wformat-nonliteral -Wcast-align

SRCS=main.c
BIN=out

out: $(SRCS)
	$(CC) $(EXTRA_WARNINGS) $(SRCS) -o $@ $(LIBS)

all:
	out

clean:
	rm -rf $(BIN)

run:
	./out