CC = g++

LIB = -lm -lraylib

SRC = 3drender.cpp

NAME = render

all:
	$(CC) $(SRC) $(LIB) -o $(NAME)