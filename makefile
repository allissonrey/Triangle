CC = gcc
LIBS = sdl2 gl glew
CFLAGS = -Wall -Wextra
SRCFILES = $(wildcard *.c)
TARGET = output

$(TARGET):
	$(CC) $(SRCFILES) `pkg-config --libs $(LIBS)` $(CFLAGS) -o build/$(TARGET)

run:
	./build/$(TARGET)