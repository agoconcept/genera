#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "defines.h"
#include "component.h"
#include "curve.h"
#include "gradient.h"
#include "layer.h"

#include "genera.h"

#define TEXTURE_FX_ON      0x80
#define TEXTURE_FX_TYPE    0x7F

#define DESTINATION_LAYER  0xC0
#define SOURCE_LAYER       0x30
#define WIDTH              0x38
#define HEIGHT             0x07
#define SCALE_X            0xF0
#define SCALE_Y            0x0F
#define HORIZONTAL         0x20
#define VERTICAL           0x10
#define DIRECTION          0x38
#define EMBOSS_VAL         0x38
#define POSTERIZE_VAL      0x38
#define TEXTURE_LAYER      0xC0
#define MASK_LAYER         0x30

enum TextureFXID {
   CHECKER,CURVE,FRACTAL,GLOW,NOISE,PICTURE,SINUS,
   FLIP,GLASS,MOVE,PIXELLATE,RANDOMIZE,ROTATE,SCALE,SPHERE_MAPPING,TILE,TWIRL,
   BRIGHTNESS,BUMP,BUMP_MORE,CONTRAST,EDGE_DETECT,EMBOSS,GRAYSCALE,INVERSE,POSTERIZE,SHARPEN,THRESHOLD,
   FAST_BLUR,FAST_HORIZONTAL_BLUR,FAST_VERTICAL_BLUR,GAUSSIAN_BLUR,GAUSSIAN_HORIZONTAL_BLUR,GAUSSIAN_VERTICAL_BLUR,LINE_BLUR,
   ADD,COPY,DIRECTIONAL_BLUR,ENVIRONMENT,MIX,MUL,RANDOM_DUMP,SHADE,SUB,XOR,
   TEXTURE_CURVE
};

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

#endif //_TEXTURE_H
