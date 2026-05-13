# Name of the program
PROG = lab5

# source code files to be compiled
SRCS = lab5.c

# additional compilation flags
CFLAGS += -pedantic -DLAB5
CFLAGS += -I../lab2 -I../lab3

# video lib setup
LIB_VIDEO  = libvideo.a
VIDEO_SRCS = video.c
VIDEO_OBJS = ${VIDEO_SRCS:.c=.o}

CLEANFILES += ${LIB_VIDEO} ${VIDEO_OBJS}

all: libvideo

# build video lib
libvideo: ${VIDEO_OBJS}
	ar rcs ${LIB_VIDEO} ${VIDEO_OBJS}

# paths to libraries from other labs
LIB_TIMER    = ../lab2/libtimer.a
LIB_UTILS    = ../lab2/libutils.a
LIB_KBD = ../lab3/libkbd.a

# link: video + keyboard + timer + utils + lcf
DPADD += ${LIB_VIDEO} ${LIB_KBD} ${LIB_TIMER} ${LIB_UTILS} ${LIBLCF}
LDADD += -Wl,--whole-archive -lvideo -Wl,--no-whole-archive -L../lab3 -lkbd -L../lab2 -ltimer -lutils -llcf

# include LCOM's general makefile
.include <minix.lcom.mk>