# name of the program (Minix service)
PROG=lab4 

# directories to look for source files
.PATH: ../lab2 ../lab3

# source code files to be compiled 
SRCS = lab4.c mouse.c utils.c timer.c

# additional compilation flags
CFLAGS += -pedantic
CFLAGS += -DLAB4
CFLAGS += -I../lab2 -I../lab3       

# link the LCF library
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile
.include <minix.lcom.mk>