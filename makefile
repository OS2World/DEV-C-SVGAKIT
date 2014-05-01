#
# /emx/jm/svgakit/makefile
#
GCC=gcc -Wall -O2 -mprobe
#GCC=gcc -g -Wall

.SUFFIXES: .c .o .a .exe

.c.o:
	$(GCC) -c $*.c

.o.exe:
	$(GCC) -o $*.exe $*.o jmgraph.a -lvesa
	emxbind -eq -f $*.exe
	emxbind -aq $*.exe -acim

default: jmgraph.a jmgraph.lib graph.exe tgaview.exe

install: default
	copy jmgraph.h \emx\include
	copy jmgraph.a \emx\lib
	copy jmgraph.lib \emx\lib

oldinstall: install
	copy jmgraph.h \emx\include\graph.h
	copy jmgraph.a \emx\lib\graph.a
	copy jmgraph.lib \emx\lib\graph.lib

install2: default
	cp jmgraph.h /emx/include
	cp jmgraph.a /emx/lib
	cp jmgraph.lib /emx/lib

oldinstall2: install2
	cp jmgraph.h /emx/include/graph.h
	cp jmgraph.a /emx/lib/graph.a
	cp jmgraph.lib /emx/lib/graph.lib

clean:
	-del *.o *.a *.lib *.exe *.rsp core

graph1.o:   graph1.c   jmgraph.h graph2.h graph3.h vgaports.h
gbox.o:     gbox.c     jmgraph.h graph2.h
gclear.o:   gclear.c   jmgraph.h graph2.h graph3.h vgaports.h
gclip.o:    gclip.c    jmgraph.h graph2.h
gellipse.o: gellipse.c jmgraph.h graph2.h
gget.o:     gget.c     jmgraph.h graph2.h graph3.h vgaports.h
ghline.o:   ghline.c   jmgraph.h graph2.h graph3.h vgaports.h
gimage.o:   gimage.c   jmgraph.h graph2.h graph3.h vgaports.h
gline.o:    gline.c    jmgraph.h graph2.h graph3.h
glock.o:    glock.c    jmgraph.h graph2.h
gpolygon.o: gpolygon.c jmgraph.h graph2.h
gsave.o:    gsave.c    jmgraph.h graph2.h graph3.h vgaports.h
gset.o:     gset.c     jmgraph.h graph2.h graph3.h vgaports.h
gtriangl.o: gtriangl.c jmgraph.h graph2.h
gvgapal.o:  gvgapal.c  jmgraph.h graph2.h graph3.h vgaports.h
gvline.o:   gvline.c   jmgraph.h graph2.h graph3.h vgaports.h
gwaitv.o:   gwaitv.c   jmgraph.h graph2.h          vgaports.h
gfont.o:    gfont.c    jmgraph.h

graph.rsp: makefile
	echo graph1.o    > graph.rsp
	echo gbox.o      >>graph.rsp
	echo gclear.o    >>graph.rsp
	echo gclip.o     >>graph.rsp
	echo gellipse.o  >>graph.rsp
	echo gget.o      >>graph.rsp
	echo ghline.o    >>graph.rsp
	echo gimage.o    >>graph.rsp
	echo gline.o     >>graph.rsp
	echo glock.o     >>graph.rsp
	echo gpolygon.o  >>graph.rsp
	echo gsave.o     >>graph.rsp
	echo gset.o      >>graph.rsp
	echo gtriangl.o  >>graph.rsp
	echo gvgapal.o   >>graph.rsp
	echo gvline.o    >>graph.rsp
	echo gwaitv.o    >>graph.rsp
	echo gfont.o     >>graph.rsp

jmgraph.a: graph1.o gbox.o gclear.o gclip.o gellipse.o gget.o ghline.o   \
            gimage.o gline.o glock.o gpolygon.o gsave.o gset.o gtriangl.o \
            gvgapal.o gvline.o gwaitv.o gfont.o graph.rsp
	-if exist jmgraph.a del jmgraph.a
	ar r jmgraph.a @graph.rsp
	ar s jmgraph.a

jmgraph.lib: jmgraph.a
	emxomf -o jmgraph.lib jmgraph.a

graph.o:   graph.c jmgraph.h
graph.exe: graph.o jmgraph.a

tgaview.o:   tgaview.c jmgraph.h
tgaview.exe: tgaview.o jmgraph.a
