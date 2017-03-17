all : rawdraw

CFLAGS:=$(CFLAGS) -g -O0 -DRASTERIZER
CXXFLAGS:=$(CFLAGS)
LDFLAGS:=-g

MINGW32:=/usr/bin/i686-w64-mingw32-

rawdraw.exe : rawdraw.c CNFGFunctions.c CNFGWinDriver.c CNFG3D.c os_generic.c
	$(MINGW32)gcc -m32 $(CFLAGS) -o $@ $^  -lgdi32 

rawdraw : rawdraw.o CNFGFunctions.o CNFGXDriver.o os_generic.o CNFG3D.o
	gcc -o $@ $^ -lX11 -lm -lpthread -lXinerama -lXext $(LDFLAGS) 

ontop : ontop.o CNFGFunctions.o CNFGXDriver.o os_generic.o
	gcc -o $@ $^ -lpthread -lX11 -lm -lXinerama -lXext

rawdraw_mac : rawdraw.c CNFGFunctions.c CNFGCocoaDriver.m os_generic.c CNFG3D.c
	gcc -o $@ $^ -x objective-c -framework Cocoa -framework OpenGL -lm -lpthread $(CFLAGS) $(LDFLAGS) 

clean : 
	rm -rf *.o *~ rawdraw ontop rawdraw.exe

