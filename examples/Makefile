all : fontsize fontsize_ogl

#for X11 consider:             xorg-dev
#for X11, you will need:       libx-dev
#for full screen you'll need:  libxinerama-dev libxext-dev
#for OGL You'll need:          mesa-common-dev libglu1-mesa-dev

#-DCNFGRASTERIZER
#  and
#-CNFGOGL
#  are incompatible.


MINGW32:=/usr/bin/i686-w64-mingw32-

fontsize : fontsize.c ../CNFGFunctions.c ../CNFGXDriver.c ../CNFG3D.c
	gcc -o $@ $^ -lX11 -lm -lpthread -lXinerama -lXext -lGL -g -DCNFGRASTERIZER -Wall

fontsize_ogl : fontsize.c ../CNFGFunctions.c ../CNFGXDriver.c ../CNFG3D.c
	gcc -o $@ $^ -lX11 -lm -lpthread -lXinerama -lXext -lGL -g -lX11 -lXinerama -lGL -DCNFGOGL -Wall


clean : 
	rm -rf *.o *~ rawdraw.exe rawdraw ontop rawdraw_mac rawdraw_mac_soft rawdraw_mac_cg rawdraw_mac_ogl ogltest ogltest.exe rawdraw_egl fontsize fontsize_ogl

