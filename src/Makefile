CC=g++

#includes
IDIR=../include
GFH_DIR=../gif-h
STB_DIR=../stb
CFLAGS=-I$(IDIR) -I$(GFH_DIR) -I$(STB_DIR) -O3

#dependencies
_DEPS=helper.h ising.h latticeimage.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

#objects
ODIR=obj
_OBJ=gui.o helper.o ising.o latticeimage.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

#bin
BDIR=../bin

#imgui
IMG_DIR=../imgui
IMP_DIR=../implot
IMG_BACK=$(IMG_DIR)/backends
IMG_CFLAGS=$(SDL_INC) -I$(IMG_DIR) -I$(IMG_BACK) -I$(IMP_DIR) -O3

#graphics
SDL_INC=$(shell sdl2-config --cflags)
SDL_LIB=$(shell sdl2-config --libs)
GFX_LIBS=$(SDL_LIB) -lGL -ldl

#targets
.PHONY: all clean

all : ising

ising: $(OBJ) | imgui
	$(CC) -o $@ $^ $(GFX_LIBS) ./libimgui.so

$(ODIR)/%.o: %.cpp $(DEPS) | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(IMG_CFLAGS)

$(ODIR):
	mkdir -p $@

imgui: libimgui.so

#compile imgui as a shared object
libimgui.so: $(IMG_BACK)/imgui_impl_sdl.cpp $(IMG_BACK)/imgui_impl_opengl3.cpp $(IMG_DIR)/imgui*.cpp $(IMP_DIR)/implot*.cpp
	$(CC) -shared -fPIC -o libimgui.so $(IMG_CFLAGS) $(IMG_BACK)/imgui_impl_sdl.cpp $(IMG_BACK)/imgui_impl_opengl3.cpp $(IMG_DIR)/imgui*.cpp $(IMP_DIR)/implot*.cpp $(GFX_LIBS)

clean:
	rm -f $(ODIR)/*.o


