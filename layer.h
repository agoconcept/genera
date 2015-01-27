#ifndef _LAYER_H
#define _LAYER_H

#include "defines.h"

class Layer {
   DWORD *imageData;
   int width, height;

public:
   Layer();
   ~Layer();

   void setSize(int width, int height);
   void updateSize(int width, int height) {
      this->width = width;
      this->height = height;
   }

   DWORD *getImageDataPtr() {
      return imageData;
   }
   
   int getWidth() {return width;}
   int getHeight() {return height;}
   int getSize() {return width*height;}
};

#endif //_LAYER_H
