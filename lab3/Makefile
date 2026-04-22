# name of the program (Minix service)
PROG=lab3

# source code files to be compiled (only lab3-specific code;
# timer and utils come from the pre-built libraries in lab2)
SRCS = lab3.c keyboard.c

# additional compilation flags
CFLAGS += -pedantic
CFLAGS += -DLAB3
CFLAGS += -I../lab2

# -------------------------------------------------------
# kbd library  (created by this lab)
# -------------------------------------------------------
LIB_KBD  = libkbd.a
KBD_SRCS = keyboard.c
KBD_OBJS = ${KBD_SRCS:.c=.o}

all: libkbd

# build kbd lib
libkbd: ${KBD_OBJS}
	ar rcs ${LIB_KBD} ${KBD_OBJS}

# -------------------------------------------------------
# link: kbd (this lab) + timer + utils (from lab2) + lcf
# -------------------------------------------------------
LIB_TIMER = ../lab2/libtimer.a
LIB_UTILS = ../lab2/libutils.a

DPADD += ${LIBLCF} ${LIB_KBD} ${LIB_TIMER} ${LIB_UTILS}
LDADD += -llcf -L. -lkbd -L../lab2 -ltimer -lutils

# include LCOM's makefile
.include <minix.lcom.mk>