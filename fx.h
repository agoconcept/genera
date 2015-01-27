#ifndef _FX_H
#define _FX_H

#include "defines.h"

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __WIN32__
#include "SDL.h"
#else
#include <SDL/SDL.h>
#endif

#include "random.h"
#include "gradient.h"
#include "curve.h"
#include "layer.h"
#include "model.h"
#include "vector.h"

#define UPLEFT      0
#define UP          1
#define UPRIGHT     2
#define LEFT        3
#define RIGHT       4
#define DOWNLEFT    5
#define DOWN        6
#define DOWNRIGHT   7

#define X_DIRECTION 0
#define Y_DIRECTION 1
#define Z_DIRECTION 2

#define MAP_XY      0
#define MAP_XZ      1
#define MAP_YZ      2
#define CYLINDRIC   3
#define SPHERIC     4

class FX {
   static void calculateFractal(float *colorArray, int width, int height, int xSide, int ySide, int noise, Random *random);

public:
   
   //////////////
   // TEXTURES //
   //////////////
   
   //BASE TEXTURES
   static void checker(Layer *layer, DWORD color1, DWORD color2, int width, int height);
   static void curve(Layer *layer, Curve *curve, int width, int height);
   static void fractal(Layer *layer, Gradient *gradient, int width, int height, int noise, int seed);
   static void glow(Layer *layer, Gradient *gradient, int width, int height, int radius);
   static void noise(Layer *layer, Gradient *gradient, int width, int height, int seed);
   static void picture(Layer *layer, int width, int height, BYTE *data);
   static void sinus(Layer *layer, Gradient *gradient, int width, int height, int turbX1, int turbX2, int turbY1, int turbY2, int phase1, int phase2);

   //DISTORTIONS
   static void flip(Layer *layer, bool horizontal, bool vertical);
   static void glass(Layer *layer, int turbX1, int turbX2, int turbY1, int turbY2, int phase1, int phase2, int intensity);
   static void move(Layer *layer, int direction, int value);
   static void pixellate(Layer *layer, int value);
   static void randomize(Layer *layer, int value, int seed);
   static void rotate(Layer *layer, int value);
   static void scale(Layer *layer, int width, int height);
   static void sphereMapping(Layer *layer);
   static void tile(Layer *layer, int horizontalTile, int verticalTile);
   static void twirl(Layer *layer, int value);

   //COLORS
   static void brightness(Layer *layer, int value);
   static void bump(Layer *layer);
   static void bumpMore(Layer *layer);
   static void contrast(Layer *layer, int value);
   static void edgeDetect(Layer *layer);
   static void emboss(Layer *layer, int direction);
   static void grayscale(Layer *layer);
   static void inverse(Layer *layer);
   static void posterize(Layer *layer, int levels);
   static void sharpen(Layer *layer, int value);
   static void threshold(Layer *layer, int value);

   //BLURS
   static void fastBlur(Layer *layer, int radius);
   static void fastHorizontalBlur(Layer *layer, int radius);
   static void fastVerticalBlur(Layer *layer, int radius);
   static void gaussianBlur(Layer *layer, int radius);
   static void gaussianHorizontalBlur(Layer *layer, int radius);
   static void gaussianVerticalBlur(Layer *layer, int radius);
   static void lineBlur(Layer *layer, int orientation, int radius);

   //LAYERS
   static void add(Layer *dst, Layer *src);
   static void copy(Layer *dst, Layer *src);
   static void directionalBlur(Layer *dst, Layer *src, int value);
   static void environment(Layer *dst, Layer *src, int value);
   static void mix(Layer *dst, Layer *src, int value);
   static void mul(Layer *dst, Layer *src);
   static void randomDump(Layer *dst, Layer *src, int number, int seed);
   static void shade(Layer *dst, Layer *src);
   static void sub(Layer *dst, Layer *src);
   static void xclor(Layer *dst, Layer *src);

   //COMBOS
   static void textureCurve(Layer *dest, int width, int height, Curve *curve, Layer *texture, Layer *mask, int edgeWidth);

   ////////////
   // MODELS //
   ////////////
   
   static void cube(Model *model);
   static void cylinder(Model *model, int segments);
   static void heightField(Model *model, Texture *texture, int facets);

   static void octahedron(Model *model);
   static void sphere(Model *model, int segments, int rings);
   static void star(Model *model, int horizontalTips, int verticalTips, float minRadius, float maxRadius, int segments, int rings);
   static void torus(Model *model, float radius1, float radius2, int segments, int rings);

   static void heightMap(Model *model, int type, float value, Texture *texture);
   static void moveObject(Model *model, Vector delta);
   static void noise(Model *model, float value, int seed);
   static void refine(Model *model);
   static void rotateObject(Model *model, int direction, float value);
   static void scaleObject(Model *model, Vector delta);
   static void spherize(Model *model, float value);
   static void taper(Model *model, int direction, float value);
   static void tessellate(Model *model);
   static void twistObject(Model *model, int direction, float value);

   static void textureMap(Model *model, int type, Texture *texture);
   static void environmentMap(Model *model, Texture *texture);
};

#endif //_FX_H
