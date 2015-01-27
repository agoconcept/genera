#ifndef _GRADIENT_H
#define _GRADIENT_H

#include "defines.h"
#include "component.h"
#include "color.h"

class Gradient : public Component {
   bool splineInterpolation;

   int numberOfColors;

   Color *color;
   float *colorPosition;
   
public:
   Gradient();
   ~Gradient();

   BYTE *loadData(BYTE *data);

   Color getColorAt(float position);
};

#endif //_GRADIENT_H
