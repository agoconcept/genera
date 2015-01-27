#include "genera.h"
#include "project.h"

#include <stdio.h>

#include "random.h"
//#include <stdlib.h>

//Performs cubic interpolation
//position indicates where to calculate the value
//values indicates the nodes on indexes -1, 0, 1 and 2
float cubicInterpolation2(float position, int *values) {
   float a, b, c, d;
   float x3, x2, x;

   a = -0.1667f*values[0] + 0.5000f*values[1] - 0.5000f*values[2] + 0.1667f*values[3];
   b = +0.5000f*values[0] - 1.0000f*values[1] + 0.5000f*values[2];
   c = -0.3333f*values[0] - 0.5000f*values[1] + 1.0000f*values[2] - 0.1667f*values[3];
   d =                              values[1];

   x = position - (int)position;
   x2 = x*x;
   x3 = x*x2;
   
   return a*x3 + b*x2 + c*x + d;
}

DWORD cubicColorInterpolation2(float position, DWORD *colors) {
   int values[4];
   DWORD interpolatedColor;
   
   values[0] = (colors[0] & 0xFF);
   values[1] = (colors[1] & 0xFF);
   values[2] = (colors[2] & 0xFF);
   values[3] = (colors[3] & 0xFF);
   interpolatedColor = (int)cubicInterpolation2(position, values);
   
   values[0] = ((colors[0]>>8) & 0xFF);
   values[1] = ((colors[1]>>8) & 0xFF);
   values[2] = ((colors[2]>>8) & 0xFF);
   values[3] = ((colors[3]>>8) & 0xFF);
   interpolatedColor += (((int)cubicInterpolation2(position, values)) << 8);
   
   values[0] = ((colors[0]>>16) & 0xFF);
   values[1] = ((colors[1]>>16) & 0xFF);
   values[2] = ((colors[2]>>16) & 0xFF);
   values[3] = ((colors[3]>>16) & 0xFF);
   interpolatedColor += (((int)cubicInterpolation2(position, values)) << 16);
   
   return interpolatedColor;
}

int main(int argc, char *argv[]) {
   Genera *g = new Genera();

   g->load(project);
   
   g->generateTexture("Texture0");
   
   printf("%d\n\n", RAND_MAX);
   for (int i = 0; i < 255; i++) {
      Random random(i);
      printf("%d\n", random.rand());
   }
   //for (int i = 0; i < 20; i++)
   //   printf("%d\n", rand());
   
   delete g;
   
   DWORD values[4];
    
   values[0] = 0x345678;
   values[1] = 0x68ACE0;
   values[2] = 0x699630;
   values[3] = 0x484848;
   
   DWORD result = cubicColorInterpolation2(5.3f, values);
   
   printf("Interpolation result: %x\n", result);
}
