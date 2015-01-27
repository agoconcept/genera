%module layer
%{
#include "defines.h"
#include "layer.h"
%}

%include "defines.h"

class Layer {
   DWORD *imageData;
   int width, height;

public:
   Layer();
   ~Layer();

   void setSize(int width, int height);

   DWORD *getImageDataPtr() {
      return imageData;
   }
   
   int getWidth() {return width;}
   int getHeight() {return height;}
   int getSize() {return width*height;}
};
