GTKCFLAGS=$(shell gtk-config --cflags)
IMLIBCFLAGS=$(shell imlib-config --cflags)
CFLAGS=-g -I/usr/X11R6/include -I/usr/lib/glib/include -I$(PLOTUTILSINC) \
	-DBUILDING_XPBM $(GTKCFLAGS) $(IMLIBCFLAGS)

#GTKLIBS=-lgtk -lgdk -rdynamic -lgmodule -lglib -ldl
GTKLIBS=$(shell gtk-config --libs)
IMLIBLIBS=$(shell imlib-config --libs)
LDFLAGS=$(LIBS) $(GTKLIBS) $(PLOTLIB)

LIBS=-L/usr/lib -L/usr/X11R6/lib -L$(PLOTUTILSLIB) \
	-lm $(GTKLIBS) $(IMLIBLIBS) $(XLIBS) $(PLOTLIBS) -lgdk_imlib


XLIBS= -lXext -lX11 -lpng -ltiff -lz -lglib -lm

#PLOTLIBS=-lplot -lcommon
#PLOTUTILSDIR=../plotutils-2.3
#PLOTUTILSINC=$(PLOTUTILSDIR)/include
#PLOTUTILSLIB=$(PLOTUTILSDIR)/lib

OBJS=files.o lines.o map.o tiles.o vmap.o xpbm.o settings.o

xpbm: $(OBJS)
	$(CC) $^ -o $@ $(LIBS)
$(OBJS): xpbm.h

all: xpbm

install: all
	cp xpbm /home/jmccann/bin

CLEAN_FILES = $(wildcard *.o *~) xpbm cleankudge
clean:
	-touch cleankudge; rm -f $(CLEAN_FILES)
