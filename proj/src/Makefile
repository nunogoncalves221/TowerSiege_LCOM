# Nome do executável
PROG = proj

# APENAS os ficheiros que estão fisicamente na pasta proj/src
SRCS = main.c

# Dizer ao compilador onde procurar os ficheiros .h
CFLAGS += -I../../lab2 -I../../lab3 -I../../lab4 -I../../lab5 -I../../lab1

CFLAGS += -pedantic -D__LCOM_OPTIMIZED__ -O2

# Importar as bibliotecas das labs (tal como no exemplo da lab5)
# O "-L" diz onde está a pasta, o "-l" diz o nome da biblioteca (sem "lib" e sem ".a")
DPADD += ${LIBLCF}
LDADD += -L../../lab5 -lvideo -L../../lab4 -lmouse -L../../lab3 -lkbd -L../../lab2 -ltimer -lutils -L../../lab1 -lrtc -llcf

.include <minix.lcom.mk>