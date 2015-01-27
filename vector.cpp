#include "vector.h"

#include <math.h>

Vector Vector::operator +(Vector v) {
   return Vector(x+v.x, y+v.y, z+v.z);
}

Vector Vector::operator -(Vector v) {
   return Vector(x-v.x, y-v.y, z-v.z);
}

float Vector::operator ^(Vector v) {
   return (x*v.x + y*v.y + z*v.z);
}
   
Vector Vector::operator *(Vector v) {
   return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

Vector Vector::operator *(float f) {
   return Vector(x*f, y*f, z*f);
}

Vector Vector::set(Vector v) {
   x = v.x; y = v.y; z = v.z;
   return *this;
}
   
void Vector::normalize() {
float value;
   value = x*x + y*y + z*z;
   if (value > DELTA) {
      value = sqrt(value);
      x /= value;
      y /= value;
      z /= value;
   }
}
