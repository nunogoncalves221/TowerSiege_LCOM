# name of the program (Minix service)
PROG=lab2

# source code files to be compiled
SRCS = lab2.c

# flags
CFLAGS += -pedantic

# -------------------------------------------------------
# utils library
# -------------------------------------------------------
LIB_UTILS = libutils.a
UTILS_SRCS = utils.c
UTILS_OBJS = ${UTILS_SRCS:.c=.o}

# -------------------------------------------------------
# timer library
# -------------------------------------------------------
LIB_TIMER = libtimer.a
TIMER_SRCS = timer.c
TIMER_OBJS = ${TIMER_SRCS:.c=.o}

# -------------------------------------------------------
# link against both libraries
# -------------------------------------------------------
DPADD += ${LIBLCF} ${LIB_TIMER} ${LIB_UTILS}
LDADD += -llcf -L. -ltimer -lutils

# -------------------------------------------------------
# build everything
# -------------------------------------------------------
all: libutils libtimer

# build utils lib
libutils: ${UTILS_OBJS}
	rm -f ${LIB_UTILS}
	ar rcs ${LIB_UTILS} ${UTILS_OBJS}

# build timer lib
libtimer: ${TIMER_OBJS}
	rm -f ${LIB_TIMER}
	ar rcs ${LIB_TIMER} ${TIMER_OBJS}

# include LCOM's makefile
.include <minix.lcom.mk>