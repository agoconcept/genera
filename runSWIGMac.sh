#!/bin/bash
SOURCES="random vector fx color component gradient curve layer texture model genera"

for s in ${SOURCES}
do
echo "Running SWIG on module ${s}"
echo "---"
swig -c++ -python -o ${s}_wrap.cpp ${s}.i
echo
done

echo "Copying files to GUI directory"
echo "---"
#cp _genera.so ../GUI
cp genera.py ../GUI
echo

echo "Running setup"
echo "---"
python setup.py install
echo
