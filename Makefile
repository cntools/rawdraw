all : rawdraw

CFLAGS:=$(CFLAGS) -g -O0 -DRASTERIZER
CXXFLAGS:=$(CFLAGS)
LDFLAGS:=-g

MINGW32:=/usr/bin/i686-w64-mingw32-

rawdraw.exe : rawdraw.c RDFunctions.c RDWinDriver.c RD3D.c os_generic.c
	$(MINGW32)gcc -m32 $(CFLAGS) -o $@ $^  -lgdi32 

rawdraw : rawdraw.o RDFunctions.o RDXDriver.o os_generic.o RD3D.o
	gcc -o $@ $^ -lX11 -lm -lpthread -lXinerama -lXext $(LDFLAGS) 

ontop : ontop.o RDFunctions.o RDXDriver.o os_generic.o
	gcc -o $@ $^ -lpthread -lX11 -lm -lXinerama -lXext

clean : 
	rm -rf *.o *~ rawdraw ontop

