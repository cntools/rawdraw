all : rawdraw

CFLAGS:=$(CFLAGS) -g -Os
# -DRASTERIZER
CXXFLAGS:=$(CFLAGS)
LDFLAGS:=-g

MINGW32:=/usr/bin/i686-w64-mingw32-

rawdraw.exe : rawdraw.c DrawFunctions.c WinDriver.c 3d.c os_generic.c
	$(MINGW32)gcc -m32 $(CFLAGS) -o $@ $^  -lgdi32

# Rasterizer.o
rawdraw : rawdraw.o DrawFunctions.o XDriver.o os_generic.o 3d.o
	gcc -o $@ $^ -lX11 -lm -lpthread -lXinerama -lXext $(LDFLAGS) 

ontop : ontop.o DrawFunctions.o XDriver.o os_generic.o
	gcc -o $@ $^ -lpthread -lX11 -lm -lXinerama -lXext

clean : 
	rm -rf *.o *~ rawdraw ontop

