# Nome do executável
PROG = proj

# APENAS os ficheiros que estão fisicamente na pasta proj/src

SRCS = main.c menu.c game.c sprite.c map.c hero.c enemy.c wave.c font.c tower.c bullet.c instructions.c pause.c gameovermenu.c menu_nav.c animated_sprite.c player.c

# Procurar os .c nas subpastas
.PATH: menu game sprite map hero enemy font tower player

# Dizer ao compilador onde procurar os ficheiros .h
CFLAGS += -I../../lab1 -I../../lab2 -I../../lab3 -I../../lab4 -I../../lab5
CFLAGS += -Imenu -Igame -Isprite -Imap -Ihero -Ienemy -Ifont -Itower -Iinstructions -Iplayer

CFLAGS += -pedantic -D__LCOM_OPTIMIZED__ -O2

# Importar as bibliotecas das labs (tal como no exemplo da lab5)
# O "-L" diz onde está a pasta, o "-l" diz o nome da biblioteca (sem "lib" e sem ".a")
LIB_RTC   = ../../lab1/librtc.a
LIB_TIMER = ../../lab2/libtimer.a
LIB_UTILS = ../../lab2/libutils.a
LIB_KBD   = ../../lab3/libkbd.a
LIB_MOUSE = ../../lab4/libmouse.a
LIB_VIDEO = ../../lab5/libvideo.a

# Adicionar as dependências
DPADD += ${LIBLCF}  ${LIB_RTC} ${LIB_UTILS} ${LIB_TIMER} ${LIB_KBD} ${LIB_MOUSE} ${LIB_VIDEO}

# Indicar ao linker onde carregar as bibliotecas (-L) e quais (-l)
LDADD +=  -L../../lab1 -lrtc \
          -L../../lab5 -lvideo \
          -L../../lab4 -lmouse \
          -L../../lab3 -lkbd \
          -L../../lab2 -ltimer -lutils \
          -llcf

.include <minix.lcom.mk>
