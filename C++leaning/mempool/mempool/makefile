INCLUDE =./include
SRC = ./src
OBJ = ./obj
BIN = ./bin
SOURCES = $(wildcard $(SRC)/*.cc)
OBJS = $(patsubst %.cc,$(OBJ)/%.o, $(notdir $(SOURCES)))
TARGET = $(BIN)/mempool
XX = g++
CC = gcc
CFLAGS = -Wall -g -O -I$(INCLUDE) -std=c++11 -lpthread
LIBS = -lm
all: $(TARGET)
$(TARGET): $(OBJS)
	$(XX) $^ $(CFLAGS) -o $@ 
$(OBJ)/%.o: $(SRC)/%.cc
	$(XX) -c $(CFLAGS) -o $@ $<
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
