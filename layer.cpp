#include "layer.h"

#ifdef __DEBUG
#include <stdio.h>
#endif //__DEBUG

Layer::Layer() {
   imageData = NULL;
   width = height = 0;
}

Layer::~Layer() {
   if (imageData != NULL)
      delete []imageData;
}

void Layer::setSize(int width, int height) {
   if (imageData != NULL)
      delete []imageData;

   imageData = new DWORD[width*height];

   for (int i = 0; i < width*height; i++)
      imageData[i] = 0x00000000;

   this->width = width;
   this->height = height;
}
