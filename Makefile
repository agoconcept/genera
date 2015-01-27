NAME = genera

SOURCES = random.cpp vector.cpp fx.cpp color.cpp component.cpp gradient.cpp curve.cpp layer.cpp texture.cpp model.cpp genera.cpp
OBJECTS = random.o vector.cpp fx.o color.o component.o gradient.o curve.o layer.o texture.o model.o genera.o

INCLUDE = #-I/usr/include/ -I./
LIBDIR  = #-L/usr/X11R6/lib -L./lib/mfmod/linux

COMPILERFLAGS = -Wall
CC = g++
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -lGL -lGLU `sdl-config --cflags --libs` #-lX11 -lXi -lXmu -lglut -lGL -lGLU -lm -lminifmod -lpthread

all : $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) main.cpp -o $(NAME) $(LIBDIR) $(LIBRARIES)
	# Size can be reduced with linker options: -s -Os -nostdlib -lgcc

compile : $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -c -Os

debug : $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) main.cpp -ggdb -o $(NAME) -s -Os $(LIBDIR) $(LIBRARIES)
	# Size can be reduced with linker options: -nostdlib -lgcc

clean :
	rm *.o

