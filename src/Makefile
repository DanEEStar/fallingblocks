# the makefile for the project

CC = gcc
CFLAGS = `sdl-config --cflags`
LDFLAGS = `sdl-config --libs` -lSDL_image -lSDL_mixer
OBJS = game.o data.o SFont.o fadingblocks.o menu.o vidinfo.o config.o utils.o helper.o

fallingblocks: main.c $(OBJS)
	$(CC) -o fallingblocks main.c $(LDFLAGS) $(CFLAGS) $(OBJS)

.c.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm $(OBJS) fallingblocks
