#ifndef VECTOR_H
#define VECTOR_H

#include "defines.h"

class Vector {
public:
   float x, y, z;      //3D coordinates

   Vector() {
      x = y = z = 0;
   }
   Vector(float vx, float vy, float vz) {
      x = vx; y = vy; z = vz;
   }

   Vector operator +(Vector v);
   Vector operator -(Vector v);

   float operator ^(Vector v);     //Dot product
   Vector operator *(Vector v);    //Vectorial product
   Vector operator *(float f);

   Vector set(Vector v);

   void normalize();
};

#endif   //VECTOR_H
