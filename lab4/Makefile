# name of the program (Minix service)
PROG=lab4 

# source code files to be compiled 
SRCS = lab4.c

# additional compilation flags
CFLAGS += -pedantic
CFLAGS += -DLAB4
CFLAGS += -I../lab2 -I../lab3       

# mouse library
LIB_MOUSE  = libmouse.a
MOUSE_SRCS = mouse.c
MOUSE_OBJS = ${MOUSE_SRCS:.c=.o}

CLEANFILES += ${LIB_MOUSE} ${MOUSE_OBJS}

all: libmouse

# build mouse lib
libmouse: ${MOUSE_OBJS}
	ar rcs ${LIB_MOUSE} ${MOUSE_OBJS}

# link: mouse + timer + utils + lcf
LIB_TIMER = ../lab2/libtimer.a
LIB_UTILS = ../lab2/libutils.a

# Make sure to link mouse, timer, and utils!
DPADD += ${LIBLCF} ${LIB_MOUSE} ${LIB_TIMER} ${LIB_UTILS}
LDADD += -llcf -L. -lmouse -L../lab2 -ltimer -lutils

# include LCOM's makefile
.include <minix.lcom.mk>
