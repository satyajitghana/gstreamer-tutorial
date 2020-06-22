# Chapter 1

Install on Linux using
```bash
sudo apt install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools
```

sample Makefile
```Makefile
CC=gcc
LIBS=`pkg-config --cflags --libs gstreamer-1.0`
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
```

build
```bash
make
```

run
```bash
./out test.mp4
```