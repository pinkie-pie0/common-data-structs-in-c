### Compiler Configurations
CC = gcc
C89 = -Wall -std=c89 -pedantic-errors
DEBUG = -g
OPTS = -Os

### Directory Configurations
SRCDIR = ./src
INCDIR = ./inc

### Compiler Flags
TARGET = driver
SRCS = $(wildcard $(SRCDIR)/*.c)
INCLUDE = $(addprefix -I,$(INCDIR))
CFLAGS = $(C89) $(DEBUG) $(OPTS) $(INCLUDE)

all: $(TARGET)

$(TARGET): %: %.c
	$(CC) $(CFLAGS) $(SRCS) -o $@ $<