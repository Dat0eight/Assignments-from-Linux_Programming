.PHONY: all , clean

CUR_DIR := .
SRC_DIR := $(CUR_DIR)/src
INC_DIR := $(CUR_DIR)/inc
BIN_DIR := $(CUR_DIR)/bin
OBJS_DIR := $(CUR_DIR)/objs
LIBS_DIR := $(CUR_DIR)/libs
SHARED_LIB := $(LIBS_DIR)/shared
STATIC_LIB := $(LIBS_DIR)/static

CC := gcc

INC_FLAGS := -I $(CUR_DIR)


mk_objs:
	$(CC) -c $(CUR_DIR)/main.c -o $(OBJS_DIR)/main.o $(INC_FLAGS)
	$(CC) -c $(SRC_DIR)/hello.c -o $(OBJS_DIR)/hello.o $(INC_FLAGS) 
	$(CC) -c $(SRC_DIR)/sum.c -o $(OBJS_DIR)/sum.o $(INC_FLAGS) 

mk_creat:
	ar -rcs $(STATIC_LIB)/libfirst-static.a $(OBJS_DIR)/hello.o $(OBJS_DIR)/sum.o

all: mk_objs mk_creat
	$(CC) $(OBJS_DIR)/main.o -L$(STATIC_LIB) -lfirst-static -o $(BIN_DIR)/statically-linked

clean: 
	rm -rf $(OBJS_DIR)/*
	rm -rf $(STATIC_LIB)/libfirst-static.a
	rm -rf $(BIN_DIR)/statically-linked