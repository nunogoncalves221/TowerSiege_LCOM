# name of the program (Minix service)
PROG=lab5

# source code files to be compiled 
SRCS = lab5.c video.c

# additional compilation flags
CFLAGS += -pedantic
CFLAGS += -DLAB5

# link only with lcf
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile
.include <minix.lcom.mk>