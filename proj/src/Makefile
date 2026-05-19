# Nome do executável
PROG = proj

# APENAS os ficheiros que estão fisicamente na pasta proj/src
SRCS = main.c menu.c game.c

# Procurar os .c nas subpastas
.PATH: menu game

# Dizer ao compilador onde procurar os ficheiros .h
CFLAGS += -I../../lab2 -I../../lab3 -I../../lab4 -I../../lab5
CFLAGS += -Imenu -Igame
# -I../../lab1
CFLAGS += -pedantic -D__LCOM_OPTIMIZED__ -O2

# Importar as bibliotecas das labs (tal como no exemplo da lab5)
# O "-L" diz onde está a pasta, o "-l" diz o nome da biblioteca (sem "lib" e sem ".a")
#LIB_RTC   = ../../lab1/librtc.a
LIB_TIMER = ../../lab2/libtimer.a
LIB_UTILS = ../../lab2/libutils.a
LIB_KBD   = ../../lab3/libkbd.a
LIB_MOUSE = ../../lab4/libmouse.a
LIB_VIDEO = ../../lab5/libvideo.a

# Adicionar as dependências
DPADD += ${LIBLCF} ${LIB_UTILS} ${LIB_TIMER} ${LIB_KBD} ${LIB_MOUSE} ${LIB_VIDEO}
#  ${LIB_RTC}

# Indicar ao linker onde carregar as bibliotecas (-L) e quais (-l)
LDADD += -L../../lab5 -lvideo \
         -L../../lab4 -lmouse \
         -L../../lab3 -lkbd \
         -L../../lab2 -ltimer -lutils \
         -llcf

#-L../../lab1 -lrtc \

.include <minix.lcom.mk>
