# Name of the program
PROG = lab5

# Source code files for the main program
SRCS = lab5.c

# Additional compilation flags
CFLAGS += -pedantic -DLAB5
CFLAGS += -I../lab2 -I../lab3

# Video library setup
LIB_VIDEO  = libvideo.a
VIDEO_SRCS = video.c
VIDEO_OBJS = ${VIDEO_SRCS:.c=.o}

CLEANFILES += ${LIB_VIDEO} ${VIDEO_OBJS}

all: libvideo

# Build video lib
libvideo: ${VIDEO_OBJS}
	ar rcs ${LIB_VIDEO} ${VIDEO_OBJS}

# Paths to libraries from other labs
LIB_TIMER    = ../lab2/libtimer.a
LIB_UTILS    = ../lab2/libutils.a
LIB_KEYBOARD = ../lab3/libkeyboard.a

# Link: video (local) + keyboard + timer + utils + lcf
DPADD += ${LIBLCF} ${LIB_VIDEO} ${LIB_KEYBOARD} ${LIB_TIMER} ${LIB_UTILS}
LDADD += -llcf -L. -lvideo -L../lab3 -lkeyboard -L../lab2 -ltimer -lutils

# Include LCOM's general makefile
.include <minix.lcom.mk>