#include "gradient.h"

#ifdef __DEBUG
#include <stdio.h>
#endif //__DEBUG

Gradient::Gradient() {
   splineInterpolation = false;
   
   numberOfColors = 0;

   color = NULL;
   colorPosition = NULL;
}

Gradient::~Gradient() {
   if (color != NULL)
      delete []color;
   if (colorPosition != NULL)
      delete []colorPosition;
}

BYTE *Gradient::loadData(BYTE *data) {
   int i;
   BYTE val;
   
   Component::loadData(data);
   
   if (color != NULL)
      delete color;
   if (colorPosition != NULL)
      delete colorPosition;

   //Pointer to gradient data
   BYTE *ptr = data;

   //Load gradient name
   ptr = readString(name, data);

#ifdef __DEBUG
   fprintf(stderr, "LOG: Gradient::loadData() - Gradient name: %s\n", name);
#endif //__DEBUG
   
   //Get interpolation and number of colors in the gradient
   val = *ptr++;
   splineInterpolation = (val&0x80) != 0;
   numberOfColors = (int)(val&0x7F);

#ifdef __DEBUG
   fprintf(stderr, "LOG: Gradient::loadData() - Interpolation: %d\n", splineInterpolation);
   fprintf(stderr, "LOG: Gradient::loadData() - Number of colors: %d\n", numberOfColors);
#endif //__DEBUG
   
   //Allocate memory for those colors
   if (color != NULL)
      delete color;
   if (colorPosition != NULL)
      delete colorPosition;
   
   color = new Color[numberOfColors];
   colorPosition = new float[numberOfColors];

   for (i = 0; i < numberOfColors; i++) {
      BYTE r = *ptr++;
      BYTE g = *ptr++;
      BYTE b = *ptr++;
      color[i].setColor(r,g,b);
      colorPosition[i] = (*ptr++)/255.0f;
   
#ifdef __DEBUG
      fprintf(stderr, "LOG: Gradient::loadData() - Color %d read: %d\n", i);
#endif //__DEBUG
   }
   
   return ptr;
}

Color Gradient::getColorAt(float position) {
   int i;

   //Search position
   for (i = 0; colorPosition[i+1] < position; i++);

   //Get surrounding colors
   Color c1 = color[i];
   Color c2 = color[i+1];

   float delta = (position - colorPosition[i]) / (colorPosition[i+1] - colorPosition[i]);

   //Interpolation between two colors
   if (splineInterpolation) {
      float ar = 2*c1.getRed() - 2*c2.getRed();
      float br = -3*c1.getRed() + 3*c2.getRed();
      float dr = c1.getRed();
   
      float ag = 2*c1.getGreen() - 2*c2.getGreen();
      float bg = -3*c1.getGreen() + 3*c2.getGreen();
      float dg = c1.getGreen();
   
      float ab = 2*c1.getBlue() - 2*c2.getBlue();
      float bb = -3*c1.getBlue() + 3*c2.getBlue();
      float db = c1.getBlue();
      
      float delta2 = delta*delta;
      float delta3 = delta2*delta;
      
      Color c(
         (BYTE)(255.0f*(ar*delta3 + br*delta2 + dr)),
         (BYTE)(255.0f*(ag*delta3 + bg*delta2 + dg)),
         (BYTE)(255.0f*(ab*delta3 + bb*delta2 + db))
      );
      return c;
   }
   else {
      Color c(
         (BYTE)(255.0f*(c1.getRed()*(1.0f-delta)   + c2.getRed()*delta)),
         (BYTE)(255.0f*(c1.getGreen()*(1.0f-delta) + c2.getGreen()*delta)),
         (BYTE)(255.0f*(c1.getBlue()*(1.0f-delta)  + c2.getBlue()*delta))
      );
      return c;
   }
}
