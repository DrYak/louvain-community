#!/bin/bash

CC=g++
CFLAGS= -std=c++11 -O5 -Wall
LDFLAGS= -std=c++11 -lm -Wall
EXEC=linux-community linux-convert linux-hierarchy
OBJ1= graph_binary.o community.o
OBJ2= graph.o

all: $(EXEC)

linux-community : $(OBJ1) main_community.o
	$(CC) -o $@ $^ $(LDFLAGS)

linux-convert : $(OBJ2) main_convert.o
	$(CC) -o $@ $^ $(LDFLAGS)

linux-hierarchy : main_hierarchy.o
	$(CC) -o $@ $^ $(LDFLAGS)

##########################################
# Generic rules
##########################################

%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.o *~ $(EXEC)
