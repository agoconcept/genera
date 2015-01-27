#include "component.h"

#ifdef __DEBUG
#include <stdio.h>
#endif //__DEBUG

Component::Component() {
   name[0] = 0;
   generated = false;
   type = UNKNOWN_ID;
}

char *Component::getName() {
   return name;
}

void Component::setName(const char *name) {
   int i;

   for (i = 0; name[i] != 0; i++)
      this->name[i] = name[i];
   this->name[i] = 0;
}

int Component::nameIs(const char *str) {
   int i;
   
   for (i = 0; name[i] && str[i]; i++) {
      if (name[i] != str[i])
         return 0;
   }
   if (name[i] != str[i])
      return 0;
   else
      return 1;
}

BYTE *Component::readString(char *output, const BYTE *src) {
   int i;
   char *ptr = (char *)src;
   
   for (i = 0; *ptr; ptr++, i++)
      output[i] = *ptr;
   output[i] = 0;
   return (BYTE *)(ptr+1);
}
