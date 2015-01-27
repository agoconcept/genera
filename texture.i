%module texture
%{
#include "defines.h"
#include "texture.h"
#include "component.h"
#include "curve.h"
#include "layer.h"
#include "gradient.h"
#include "genera.h"
%}

%include "defines.h"
%include "component.h"
%include "curve.h"
%include "layer.h"
%include "gradient.h"
%include "genera.h"

class Layer;
class Genera;

class Texture : public Component {
   Layer layer[4];   //A texture has 4 layers

   Genera *genera;   //Pointer to genera, to be able to get gradients and curves of the project

   BYTE *data;       //Store texture data

public:
   Texture();
   ~Texture();

   void setGenerator(Genera *genera);

   BYTE *loadData(BYTE *data);

   void generate();

   Layer *getLayer(int i) {
      return &(layer[i]);
   }
   
   DWORD *getTexture() {
      return layer[0].getImageDataPtr();
   }

   //BASE TEXTURES
   BYTE *checker(BYTE *ptr, bool fxOn);
   BYTE *curve(BYTE *ptr, bool fxOn);
   BYTE *fractal(BYTE *ptr, bool fxOn);
   BYTE *glow(BYTE *ptr, bool fxOn);
   BYTE *noise(BYTE *ptr, bool fxOn);
   BYTE *picture(BYTE *ptr, bool fxOn);
   BYTE *sinus(BYTE *ptr, bool fxOn);
   
   //DISTORSIONS
   BYTE *flip(BYTE *ptr, bool fxOn);
   BYTE *glass(BYTE *ptr, bool fxOn);
   BYTE *move(BYTE *ptr, bool fxOn);
   BYTE *pixellate(BYTE *ptr, bool fxOn);
   BYTE *randomize(BYTE *ptr, bool fxOn);
   BYTE *rotate(BYTE *ptr, bool fxOn);
   BYTE *scale(BYTE *ptr, bool fxOn);
   BYTE *sphereMapping(BYTE *ptr, bool fxOn);
   BYTE *tile(BYTE *ptr, bool fxOn);
   BYTE *twirl(BYTE *ptr, bool fxOn);

   //COLORS
   BYTE *brightness(BYTE *ptr, bool fxOn);
   BYTE *bump(BYTE *ptr, bool fxOn);
   BYTE *bumpMore(BYTE *ptr, bool fxOn);
   BYTE *contrast(BYTE *ptr, bool fxOn);
   BYTE *edgeDetect(BYTE *ptr, bool fxOn);
   BYTE *emboss(BYTE *ptr, bool fxOn);
   BYTE *grayscale(BYTE *ptr, bool fxOn);
   BYTE *inverse(BYTE *ptr, bool fxOn);
   BYTE *posterize(BYTE *ptr, bool fxOn);
   BYTE *sharpen(BYTE *ptr, bool fxOn);
   BYTE *threshold(BYTE *ptr, bool fxOn);

   //BLURS
   BYTE *fastBlur(BYTE *ptr, bool fxOn);
   BYTE *fastHorizontalBlur(BYTE *ptr, bool fxOn);
   BYTE *fastVerticalBlur(BYTE *ptr, bool fxOn);
   BYTE *gaussianBlur(BYTE *ptr, bool fxOn);
   BYTE *gaussianHorizontalBlur(BYTE *ptr, bool fxOn);
   BYTE *gaussianVerticalBlur(BYTE *ptr, bool fxOn);
   BYTE *lineBlur(BYTE *ptr, bool fxOn);

   //LAYERS
   BYTE *add(BYTE *ptr, bool fxOn);
   BYTE *copy(BYTE *ptr, bool fxOn);
   BYTE *directionalBlur(BYTE *ptr, bool fxOn);
   BYTE *environment(BYTE *ptr, bool fxOn);
   BYTE *mix(BYTE *ptr, bool fxOn);
   BYTE *mul(BYTE *ptr, bool fxOn);
   BYTE *randomDump(BYTE *ptr, bool fxOn);
   BYTE *shade(BYTE *ptr, bool fxOn);
   BYTE *sub(BYTE *ptr, bool fxOn);
   BYTE *xclor(BYTE *ptr, bool fxOn);

   //COMBOS
   BYTE *textureCurve(BYTE *ptr, bool fxOn);
};
