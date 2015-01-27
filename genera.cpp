#include "genera.h"

#ifdef __DEBUG
#include <stdio.h>
#endif //__DEBUG

Genera::Genera() {
   int i;
   
   numberOfComponents = 0;
   for (i = 0; i < 65536; i++) {
      component[i] = NULL;
   }
}

Genera::~Genera() {
   int i;
   
   for (i = 0; i < 65536; i++) {
      if (component[i] != NULL)
         delete component[i];
   }
}

void Genera::load(BYTE *data) {
   int i;
   
   BYTE *ptr = data;

   //Get number of components in the project (2 bytes)
   numberOfComponents = ((int)ptr[0])*256 + ((int)ptr[1]);
   ptr += 2;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Genera::load() - Number of components read: %d\n", numberOfComponents);
#endif //__DEBUG
   
   //Load every component
   for (i = 0; i < numberOfComponents; i++) {
      int componentType = (int)(*(ptr++));
      
#ifdef __DEBUG
      fprintf(stderr, "LOG: Genera::load() - Component type: %d\n", componentType);
#endif //__DEBUG
   
      if (component[i] != NULL)
         delete component[i];
      
      switch (componentType) {
      case GRADIENT_ID :
         component[i] = new Gradient();
         break;
      case CURVE_ID :
         component[i] = new Curve();
         break;
      case TEXTURE_ID :
         component[i] = new Texture();
         ((Texture *)component[i])->setGenerator(this);
         break;
      case MODEL_ID :
         component[i] = new Model();
         ((Model *)component[i])->setGenerator(this);
         break;
      case PARAMETER_ID :
         //component[i] = new Parameter();
         break;
      case SCENE_ID :
         //component[i] = new Scene();
         break;
      default :
#ifdef __DEBUG
         fprintf(stderr, "LOG: Genera::load() - ERROR: component unknown\n");
#endif //__DEBUG
         break;
      }
      
      ptr = component[i]->loadData(ptr);
      component[i]->setType(componentType);
   
#ifdef __DEBUG
      fprintf(stderr, "LOG: Genera::load() - Component %d loaded\n", i);
#endif //__DEBUG
  
   }
}

Component *Genera::getComponent(const char *componentName) {
   int i;
   
   for (i = 0; i < numberOfComponents; i++) {
      if (component[i]->nameIs(componentName))
         return component[i];
   }
   return (Component *)NULL;
}

void Genera::generateTexture(const char *textureName) {
   int i;
   
   for (i = 0; i < numberOfComponents; i++) {
      if (component[i]->nameIs(textureName) && component[i]->getType() == TEXTURE_ID) {
         component[i]->generate();
      }
#ifdef __DEBUG
      fprintf(stderr, "LOG: Genera::generateTexture() - Component %d generated\n", i);
#endif //__DEBUG
   }
}

void Genera::generateAll() {
   int i;

   for (i = 0; i < numberOfComponents; i++) {
      component[i]->generate();
#ifdef __DEBUG
      fprintf(stderr, "LOG: Genera::generateAll() - Component %d generated\n", i);
#endif //__DEBUG
   }
}

//////////////////////////////////////////
// Python compatibility layer functions //
//////////////////////////////////////////
void Genera::loadPy(char *data, int len) {
   int i;
   BYTE *temp = new BYTE[len];
   
   for (i = 0; i < len; i++) {
      temp[i] = (BYTE)data[i];
   
#ifdef __DEBUG
      fprintf(stderr, "LOG: Genera::loadPy() - %c,%x\n", temp[i], temp[i]);
#endif //__DEBUG
   }
   
   load(temp);
   
   delete []temp;
}

void Genera::getTextureLayer(const char *textureName, int layer, char *data, int *maxData) {
   int i, j;

   for (i = 0; i < numberOfComponents; i++) {
#ifdef __DEBUG
      fprintf(stderr, "LOG: Genera::getTextureLayer() - i = %d\n", i);
#endif //__DEBUG
      if (component[i]->nameIs(textureName) && component[i]->getType() == TEXTURE_ID) {
#ifdef __DEBUG
         fprintf(stderr, "LOG: Genera::getTextureLayer() - textureName = %s\n", textureName);
#endif //__DEBUG
         
         DWORD *ptr = ((Texture *)component[i])->getLayer(layer)->getImageDataPtr();
#ifdef __DEBUG
         fprintf(stderr, "LOG: Genera::getTextureLayer() - ptr = %p\n", ptr);
#endif //__DEBUG
         
         if (ptr) {
            *maxData = ((Texture *)component[i])->getLayer(layer)->getSize()*sizeof(ptr[0]);
            for (j = 0; j < *maxData; j++)
               data[j] = ((char *)ptr)[j];
         }
         else {
            *maxData = 0;
         }

#ifdef __DEBUG
         fprintf(stderr, "LOG: Genera::getTextureLayer() - maxData = %d\n", *maxData);
#endif //__DEBUG
         
         return;
      }
   }
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Genera::getTextureLayer() - No texture found\n");
#endif //__DEBUG
   *maxData = 0;
}

int Genera::getTextureLayerWidth(const char *textureName, int layer) {
   int i;

   for (i = 0; i < numberOfComponents; i++) {
#ifdef __DEBUG
      fprintf(stderr, "LOG: Genera::getTextureLayerWidth() - i = %d\n", i);
#endif //__DEBUG
      if (component[i]->nameIs(textureName) && component[i]->getType() == TEXTURE_ID) {
#ifdef __DEBUG
         fprintf(stderr, "LOG: Genera::getTextureLayerWidth() - textureName = %s\n", textureName);
#endif //__DEBUG
         
         return ((Texture *)component[i])->getLayer(layer)->getWidth();
      }
   }
#ifdef __DEBUG
   fprintf(stderr, "LOG: Genera::getTextureLayerWidth() - No texture found\n");
#endif //__DEBUG
   return -1;
}

int Genera::getTextureLayerHeight(const char *textureName, int layer) {
   int i;

   for (i = 0; i < numberOfComponents; i++) {
#ifdef __DEBUG
      fprintf(stderr, "LOG: Genera::getTextureLayerHeight() - i = %d\n", i);
#endif //__DEBUG
      if (component[i]->nameIs(textureName) && component[i]->getType() == TEXTURE_ID) {
#ifdef __DEBUG
         fprintf(stderr, "LOG: Genera::getTextureLayerHeight() - textureName = %s\n", textureName);
#endif //__DEBUG
         
         return ((Texture *)component[i])->getLayer(layer)->getHeight();
      }
   }
#ifdef __DEBUG
   fprintf(stderr, "LOG: Genera::getTextureLayerHeight() - No texture found\n");
#endif //__DEBUG
   return -1;
}

void Genera::getRenderedModel(const char *modelName, float xPosition, float yPosition, float zPosition, float xTarget, float yTarget, float zTarget, float xUp, float yUp, float zUp, float FOV, int width, int height, int renderMode, char *data, int *maxData) {
   int i, j;

   for (i = 0; i < numberOfComponents; i++) {
#ifdef __DEBUG
      fprintf(stderr, "LOG: Genera::getRenderedModel() - i = %d\n", i);
#endif //__DEBUG
      if (component[i]->nameIs(modelName) && component[i]->getType() == MODEL_ID) {
#ifdef __DEBUG
         fprintf(stderr, "LOG: Genera::getRenderedModel() - modelName = %s\n", modelName);
#endif //__DEBUG
         
         DWORD *ptr = ((Model *)component[i])->draw(xPosition, yPosition, zPosition, xTarget, yTarget, zTarget, xUp, yUp, zUp, FOV, width, height, renderMode);
#ifdef __DEBUG
         fprintf(stderr, "LOG: Genera::getRenderedModel() - ptr = %p\n", ptr);
#endif //__DEBUG
         
         if (ptr) {
            *maxData = width*height*sizeof(ptr[0]);
            for (j = 0; j < *maxData; j++)
               data[j] = ((char *)ptr)[j];
         }
         else {
            *maxData = 0;
         }

#ifdef __DEBUG
         fprintf(stderr, "LOG: Genera::getRenderedModel() - maxData = %d\n", *maxData);
#endif //__DEBUG
         
         return;
      }
   }
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Genera::getRenderedModel() - No texture found\n");
#endif //__DEBUG
   *maxData = 0;
}
