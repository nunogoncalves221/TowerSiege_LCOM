# name of the program (Minix service)
PROG = proj

# source code files to be compiled
SRCS = serial.c

# additional compilation flags
CFLAGS += -pedantic
CFLAGS += -DSER_PORT
CFLAGS += -I../lab2

# uart driver library
LIB_UART  = libuart.a
UART_SRCS = uart.c
UART_OBJS = ${UART_SRCS:.c=.o}

CLEANFILES += ${LIB_UART} ${UART_OBJS}

all: libuart

# build uart library
libuart: ${UART_OBJS}
	ar rcs ${LIB_UART} ${UART_OBJS}

# link: uart + utils + lcf
LIB_UTILS = ../lab2/libutils.a

DPADD += ${LIBLCF} ${LIB_UART} ${LIB_UTILS}
LDADD += -llcf -L. -luart -L../lab2 -lutils

# include LCOM's makefile
.include <minix.lcom.mk>
