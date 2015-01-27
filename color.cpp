#include "color.h"

Color::Color() {
   r = g = b = 0.0f;
}

Color::Color(BYTE r, BYTE g, BYTE b) {
   setColor(r, g, b);
}

void Color::setColor(BYTE r, BYTE g, BYTE b) {
   this->r = r/255.0f;
   this->g = g/255.0f;
   this->b = b/255.0f;
}

DWORD Color::getColor() {
   return (((DWORD)(r*255.0f))) + (((DWORD)(g*255.0f))<<8) + (((DWORD)(b*255.0f))<<16);
}

float Color::getRed() {
   return r;
}

float Color::getGreen() {
   return g;
}

float Color::getBlue() {
   return b;
}

/*Color Color::operator=(Color &color) {
   this->r = color.r;
   this->g = color.g;
   this->b = color.b;
   return *this;
}*/
