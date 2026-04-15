# name of the program (Minix service)
PROG=lab3
.PATH: ../lab2

# source code files to be compiled
SRCS = lab3.c timer.c

# additional compilation flags
CFLAGS += -pedantic
CFLAGS += -DLAB3
CFLAGS += -I../lab2
CFLAGS += -DLAB3

LIB_NAME = libkbc.a
LIB_SRCS = keyboard.c
LIB_OBJS = ${LIB_SRCS:.c=.o}

CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib

lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

DPADD += ${LIBLCF} ${LIB_NAME}
LDADD += -L. -lkbc -llcf

# include LCOM's makefile
.include <minix.lcom.mk>