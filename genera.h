#ifndef _GENERA_H
#define _GENERA_H

#include "defines.h"

#include "component.h"
#include "gradient.h"
#include "curve.h"
#include "texture.h"
#include "model.h"

class Component;
class Gradient;
class Curve;
class Texture;

class Genera {

   int numberOfComponents;
   
   Component *component[65536];

public:
   Genera();
   ~Genera();

   void load(BYTE *data);

   Component *getComponent(const char *componentName);

   void generateTexture(const char *textureName);
   //TODO: Add other components

   void generateAll();

   //Python compatibility layer functions
   void loadPy(char *data, int len);
   void getTextureLayer(const char *textureName, int layer, char *data, int *maxData);
   int getTextureLayerWidth(const char *textureName, int layer);
   int getTextureLayerHeight(const char *textureName, int layer);
   void getRenderedModel(const char *modelName, float xPosition, float yPosition, float zPosition, float xTarget, float yTarget, float zTarget, float xUp, float yUp, float zUp, float FOV, int width, int height, int renderMode, char *data, int *maxData);
};

#endif //_GENERA_H
