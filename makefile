CC = g++

LIB = -lm `sdl2-config --libs`

SRC = 3drender.cpp

NAME = render

all:
	$(CC) $(SRC) $(LIB) -o $(NAME)