#ifndef _COMPONENT_H
#define _COMPONENT_H

#include "defines.h"

enum ComponentID {
   GRADIENT_ID = 0, CURVE_ID = 1, TEXTURE_ID = 2,
   MODEL_ID = 3, PARAMETER_ID = 4, SCENE_ID = 5,
   UNKNOWN_ID = 255
};

class Component {
protected:
   char name[256];
   bool generated;
   
   BYTE type;      //Type of component

public:
   Component();

   virtual ~Component() {}

   virtual void generate() {
      generated = true;
   }

   virtual BYTE *loadData(BYTE *data) {
      generated = false;
      return data;
   }
   
   char *getName();
   void setName(const char *name);
   
   bool getGenerated() {
      return generated;
   }
   
   void setType(int type) {
      this->type = (BYTE)type;
   }
   int getType() {
      return (int)type;
   }
   
   //Return 1 if they are equal, or 0 otherwise
   int nameIs(const char *str);
   
   //Store in 'output' the ASCIIZ string starting from 'src'
   //Return the position after the string
   BYTE *readString(char *output, const BYTE *src);
};

#endif //_COMPONENT_H
