#!/bin/bash
SOURCES="random vector fx color component gradient curve layer texture model genera"

for s in ${SOURCES}
do
echo "Running SWIG on module ${s}"
echo "---"
swig -c++ -python -o ${s}_wrap.cpp ${s}.i
g++ -fpic -c ${s}.cpp
g++ -fpic -c ${s}_wrap.cpp -o ${s}_wrap.o -I/usr/include/python2.7/
g++ -shared ${s}_wrap.o ${s}.o -o _${s}.so -lGL -lGLU `sdl-config --cflags --libs`
echo
done

echo "Creating libraries"
echo "---"
swig -c++ -python -o genera_wrap.cpp genera.i
g++ -fpic -c genera_wrap.cpp -o genera_wrap.o -I/usr/include/python2.7/
g++ -shared random_wrap.o random.o vector_wrap.o vector.o fx_wrap.o fx.o color_wrap.o color.o component_wrap.o component.o gradient_wrap.o gradient.o curve_wrap.o curve.o layer_wrap.o layer.o texture_wrap.o texture.o model_wrap.o model.o genera_wrap.o genera.o -o _genera.so -lGL -lGLU `sdl-config --cflags --libs`
echo

echo "Copying files to GUI directory"
echo "---"
cp _genera.so ../GUI
cp genera.py ../GUI
echo
