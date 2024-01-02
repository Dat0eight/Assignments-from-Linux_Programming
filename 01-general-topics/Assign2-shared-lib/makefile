.PHONY: all , clean

CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
INC_DIR := $(CUR_DIR)/inc
OBJS_DIR := $(CUR_DIR)/objs
SRC_DIR := $(CUR_DIR)/src
LIBS_DIR := $(CUR_DIR)/libs
SHARED_LIB := $(LIBS_DIR)/shared
STATIC_LIB := $(LIBS_DIR)/static

CC := gcc

INC_FLAGS := -I $(INC_DIR)

mk_objs:
	$(CC) -c $(CUR_DIR)/main.c -o $(OBJS_DIR)/main.o
	$(CC) -c -fPIC $(SRC_DIR)/multi-math.c -o $(OBJS_DIR)/multi-math.o $(INC_FLAGS)
	$(CC) -c -fPIC $(SRC_DIR)/quote-msg.c -o $(OBJS_DIR)/quote-msg.o $(INC_FLAGS)

mk_creat:
	$(CC) -shared $(OBJS_DIR)/multi-math.o $(OBJS_DIR)/quote-msg.o -o $(SHARED_LIB)/libsecond-lib.so

all: mk_objs mk_creat
	$(CC) $(OBJS_DIR)/main.o -L$(SHARED_LIB) -lsecond-lib -o $(BIN_DIR)/use-shared-library

clean:
	rm -rf objs/*
	rm -rf $(SHARED_LIB)/libsecond-lib.so
	rm -rf $(BIN_DIR)/use-shared-library