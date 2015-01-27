#ifndef _COLOR_H
#define _COLOR_H

#include "defines.h"

class Color {
   float r;
   float g;
   float b;

public:
   Color();
   Color(BYTE r, BYTE g, BYTE b);

   void setColor(BYTE r, BYTE g, BYTE b);

   DWORD getColor();

   float getRed();
   float getGreen();
   float getBlue();
   
   //Color operator=(Color &color);
};

#endif //_COLOR_H
