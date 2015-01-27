#include "texture.h"

#include "fx.h"

#ifdef __DEBUG
#include <stdio.h>
#endif //__DEBUG

Texture::Texture() {
   int i;
   
   this->genera = NULL;
   this->data = NULL;
   
   for (i = 0; i < 4; i++)
      layer[i].setSize(256, 256);
}

Texture::~Texture() {
   if (this->data != NULL)
      delete []this->data;
}

void Texture::setGenerator(Genera *genera) {
   this->genera = genera;
}

BYTE *Texture::loadData(BYTE *data) {
   Component::loadData(data);
   
   if (this->data != NULL)
      delete []this->data;

   //Pointer to gradient data
   BYTE *ptr = data;

   //Load gradient name
   ptr = readString(name, data);

#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::loadData() - Texture name: %s\n", name);
#endif //__DEBUG
   
   //Load texture data size
   int size = (((int)ptr[0])<<16) + (((int)ptr[1])<<8) + ((int)ptr[2]);
   
   ptr += 3;

#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::loadData() - Texture data size: %d\n", size);
#endif //__DEBUG
   
   this->data = new BYTE[size];

   for (int i = 0; i < size; i++, ptr++)
      this->data[i] = *ptr;

   return ptr;
}

void Texture::generate() {
   BYTE value;
   bool fxOn;
   BYTE *ptr = data;
   
   int numberOfFx = *ptr++;
   
   for (int i = 0; i < numberOfFx; i++) {
      //Get active and FX number
      value = *ptr++;
      
#ifdef __DEBUG
      fprintf(stderr, "LOG: Texture::generate() - Component %d; value: %d\n", i, value & TEXTURE_FX_TYPE);
#endif //__DEBUG
   
      fxOn = (value & TEXTURE_FX_ON) != 0;

      switch ((TextureFXID)value & TEXTURE_FX_TYPE) {
         //Generate texture from loaded data
         case CHECKER:
            ptr = checker(ptr, fxOn);
            break;
         case CURVE:
            ptr = curve(ptr, fxOn);
            break;
         case FRACTAL:
            ptr = fractal(ptr, fxOn);
            break;
         case GLOW:
            ptr = glow(ptr, fxOn);
            break;
         case NOISE:
            ptr = noise(ptr, fxOn);
            break;
         case PICTURE:
            ptr = picture(ptr, fxOn);
            break;
         case SINUS:
            ptr = sinus(ptr, fxOn);
            break;

         case FLIP:
            ptr = flip(ptr, fxOn);
            break;
         case GLASS:
            ptr = glass(ptr, fxOn);
            break;
         case MOVE:
            ptr = move(ptr, fxOn);
            break;
         case PIXELLATE:
            ptr = pixellate(ptr, fxOn);
            break;
         case RANDOMIZE:
            ptr = randomize(ptr, fxOn);
            break;
         case ROTATE:
            ptr = rotate(ptr, fxOn);
            break;
         case SCALE:
            ptr = scale(ptr, fxOn);
            break;
         case SPHERE_MAPPING:
            ptr = sphereMapping(ptr, fxOn);
            break;
         case TILE:
            ptr = tile(ptr, fxOn);
            break;
         case TWIRL:
            ptr = twirl(ptr, fxOn);
            break;

         case BRIGHTNESS:
            ptr = brightness(ptr, fxOn);
            break;
         case BUMP:
            ptr = bump(ptr, fxOn);
            break;
         case BUMP_MORE:
            ptr = bumpMore(ptr, fxOn);
            break;
         case CONTRAST:
            ptr = contrast(ptr, fxOn);
            break;
         case EDGE_DETECT:
            ptr = edgeDetect(ptr, fxOn);
            break;
         case EMBOSS:
            ptr = emboss(ptr, fxOn);
            break;
         case GRAYSCALE:
            ptr = grayscale(ptr, fxOn);
            break;
         case INVERSE:
            ptr = inverse(ptr, fxOn);
            break;
         case POSTERIZE:
            ptr = posterize(ptr, fxOn);
            break;
         case SHARPEN:
            ptr = sharpen(ptr, fxOn);
            break;
         case THRESHOLD:
            ptr = threshold(ptr, fxOn);
            break;

         case FAST_BLUR:
            ptr = fastBlur(ptr, fxOn);
            break;
         case FAST_HORIZONTAL_BLUR:
            ptr = fastHorizontalBlur(ptr, fxOn);
            break;
         case FAST_VERTICAL_BLUR:
            ptr = fastVerticalBlur(ptr, fxOn);
            break;
         case GAUSSIAN_BLUR:
            ptr = gaussianBlur(ptr, fxOn);
            break;
         case GAUSSIAN_HORIZONTAL_BLUR:
            ptr = gaussianHorizontalBlur(ptr, fxOn);
            break;
         case GAUSSIAN_VERTICAL_BLUR:
            ptr = gaussianVerticalBlur(ptr, fxOn);
            break;
         case LINE_BLUR:
            ptr = lineBlur(ptr, fxOn);
            break;

         case ADD:
            ptr = add(ptr, fxOn);
            break;
         case COPY:
            ptr = copy(ptr, fxOn);
            break;
         case DIRECTIONAL_BLUR:
            ptr = directionalBlur(ptr, fxOn);
            break;
         case ENVIRONMENT:
            ptr = environment(ptr, fxOn);
            break;
         case MIX:
            ptr = mix(ptr, fxOn);
            break;
         case MUL:
            ptr = mul(ptr, fxOn);
            break;
         case RANDOM_DUMP:
            ptr = randomDump(ptr, fxOn);
            break;
         case SHADE:
            ptr = shade(ptr, fxOn);
            break;
         case SUB:
            ptr = sub(ptr, fxOn);
            break;
         case XOR:
            ptr = xclor(ptr, fxOn);
            break;
         
         case TEXTURE_CURVE:
            ptr = textureCurve(ptr, fxOn);
            break;
         
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////

BYTE *Texture::checker(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::checker() - CHECKER\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int width = 1 << ((((*ptr) & WIDTH)>>3) + 3);
   int height = 1 << (((*ptr) & HEIGHT) + 3);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::checker() - destinationLayer: %d; width: %d; height: %d\n", destinationLayer, width, height);
#endif //__DEBUG
   ptr++;

   char gradientName[256];
   ptr = readString(gradientName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::checker() - gradientName: %s\n", gradientName);
#endif //__DEBUG

   Gradient *gradient = (Gradient *)genera->getComponent(gradientName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::checker() - gradient: %p\n", gradient);
#endif //__DEBUG
   DWORD color1 = 0;
   DWORD color2 = 0;
   if (gradient) {
      color1 = gradient->getColorAt(0.0f).getColor();
      color2 = gradient->getColorAt(1.0f).getColor();
   }
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::checker() - color1: %x; color2: %x\n", color1, color2);
#endif //__DEBUG

   if (fxOn && gradient)
      FX::checker(&layer[destinationLayer], color1, color2, width, height);

   return ptr;
}

BYTE *Texture::curve(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::curve() - CURVE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int width = 1 << ((((*ptr) & WIDTH)>>3) + 3);
   int height = 1 << (((*ptr) & HEIGHT) + 3);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::curve() - destinationLayer: %d; width: %d; height: %d\n", destinationLayer, width, height);
#endif //__DEBUG
   ptr++;

   char curveName[256];
   ptr = readString(curveName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::curve() - curveName: %s\n", curveName);
#endif //__DEBUG
   
   Curve *curve = (Curve *)genera->getComponent(curveName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::curve() - curve: %p\n", curve);
#endif //__DEBUG
   
   if (fxOn && curve)
      FX::curve(&layer[destinationLayer], curve, width, height);

   return ptr;
}

BYTE *Texture::fractal(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fractal() - FRACTAL\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int width = 1 << ((((*ptr) & WIDTH)>>3) + 3);
   int height = 1 << (((*ptr) & HEIGHT) + 3);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fractal() - destinationLayer: %d; width: %d; height: %d\n", destinationLayer, width, height);
#endif //__DEBUG
   ptr++;

   char gradientName[256];
   ptr = readString(gradientName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fractal() - gradientName: %s\n", gradientName);
#endif //__DEBUG

   Gradient *gradient = (Gradient *)genera->getComponent(gradientName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fractal() - gradient: %p\n", gradient);
#endif //__DEBUG

   int noise = *ptr++;
   int seed = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::generate() - noise: %d; seed: %d\n", noise, seed);
#endif //__DEBUG

   if (fxOn && gradient)
      FX::fractal(&layer[destinationLayer], gradient, width, height, noise, seed);

   return ptr;
}

BYTE *Texture::glow(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::glow() - GLOW\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int width = 1 << ((((*ptr) & WIDTH)>>3) + 3);
   int height = 1 << (((*ptr) & HEIGHT) + 3);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::glow() - destinationLayer: %d; width: %d; height: %d\n", destinationLayer, width, height);
#endif //__DEBUG
   ptr++;

   char gradientName[256];
   ptr = readString(gradientName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::glow() - gradientName: %s\n", gradientName);
#endif //__DEBUG

   Gradient *gradient = (Gradient *)genera->getComponent(gradientName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::glow() - gradient: %p\n", gradient);
#endif //__DEBUG

   int radius = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::glow() - radius: %d\n", radius);
#endif //__DEBUG

   if (fxOn && gradient)
      FX::glow(&layer[destinationLayer], gradient, width, height, radius);

   return ptr;
}

BYTE *Texture::noise(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::noise() - NOISE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int width = 1 << ((((*ptr) & WIDTH)>>3) + 3);
   int height = 1 << (((*ptr) & HEIGHT) + 3);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::noise() - destinationLayer: %d; width: %d; height: %d\n", destinationLayer, width, height);
#endif //__DEBUG
   ptr++;

   char gradientName[256];
   ptr = readString(gradientName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::noise() - gradientName: %s\n", gradientName);
#endif //__DEBUG

   Gradient *gradient = (Gradient *)genera->getComponent(gradientName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::noise() - gradient: %p\n", gradient);
#endif //__DEBUG

   int seed = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::noise() - seed: %d\n", seed);
#endif //__DEBUG

   if (fxOn && gradient)
      FX::noise(&layer[destinationLayer], gradient, width, height, seed);

   return ptr;
}

BYTE *Texture::picture(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::picture() - PICTURE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int width = 1 << ((((*ptr) & WIDTH)>>3) + 3);
   int height = 1 << (((*ptr) & HEIGHT) + 3);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::picture() - destinationLayer: %d; width: %d; height: %d\n", destinationLayer, width, height);
#endif //__DEBUG
   ptr++;

   BYTE *data = ptr;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::picture() - data: %p\n", data);
#endif //__DEBUG
   ptr += width*height*3;

   if (fxOn)
      FX::picture(&layer[destinationLayer], width, height, data);

   return ptr;
}

BYTE *Texture::sinus(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sinus() - SINUS\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int width = 1 << ((((*ptr) & WIDTH)>>3) + 3);
   int height = 1 << (((*ptr) & HEIGHT) + 3);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sinus() - destinationLayer: %d; width: %d; height: %d\n", destinationLayer, width, height);
#endif //__DEBUG
   ptr++;

   char gradientName[256];
   ptr = readString(gradientName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sinus() - gradientName: %s\n", gradientName);
#endif //__DEBUG

   Gradient *gradient = (Gradient *)genera->getComponent(gradientName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sinus() - gradient: %p\n", gradient);
#endif //__DEBUG

   int turbX1 = *ptr++;
   int turbX2 = *ptr++;
   int turbY1 = *ptr++;
   int turbY2 = *ptr++;
   int phase1 = *ptr++;
   int phase2 = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sinus() - turbX1: %d; turbX2: %d; turbY1: %d; turbY2: %d; phase1: %d; phase2: %d; \n", turbX1, turbX2, turbY1, turbY2, phase1, phase2);
#endif //__DEBUG

   if (fxOn && gradient)
      FX::sinus(&layer[destinationLayer], gradient, width, height, turbX1, turbX2, turbY1, turbY2, phase1, phase2);

   return ptr;
}

BYTE *Texture::flip(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::flip() - FLIP\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   bool horizontal = ((*ptr) & HORIZONTAL) != 0;
   bool vertical = ((*ptr) & VERTICAL) != 0;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::flip() - destinationLayer: %d; horizontal: %d; vertical: %d\n", destinationLayer, horizontal, vertical);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::flip(&layer[destinationLayer], horizontal, vertical);

   return ptr;
}

BYTE *Texture::glass(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::glass() - GLASS\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::glass() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int turbX1 = *ptr++;
   int turbX2 = *ptr++;
   int turbY1 = *ptr++;
   int turbY2 = *ptr++;
   int phase1 = *ptr++;
   int phase2 = *ptr++;
   int intensity = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::glass() - turbX1: %d; turbX2: %d; turbY1: %d; turbY2: %d; phase1: %d; phase2: %d; intensity: %d\n", turbX1, turbX2, turbY1, turbY2, phase1, phase2, intensity);
#endif //__DEBUG

   if (fxOn)
      FX::glass(&layer[destinationLayer], turbX1, turbX2, turbY1, turbY2, phase1, phase2, intensity);

   return ptr;
}

BYTE *Texture::move(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::move() - MOVE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int direction = ((*ptr) & DIRECTION)>>3;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::move() - destinationLayer: %d; direction: %d\n", destinationLayer, direction);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::move() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::move(&layer[destinationLayer], direction, value);

   return ptr;
}

BYTE *Texture::pixellate(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::pixellate() - PIXELLATE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::pixellate() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::pixellate() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::pixellate(&layer[destinationLayer], value);

   return ptr;
}

BYTE *Texture::randomize(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::randomize() - RANDOMIZE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::randomize() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
   int seed = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::randomize() - value: %d; seed: %d\n", value, seed);
#endif //__DEBUG

   if (fxOn)
      FX::randomize(&layer[destinationLayer], value, seed);

   return ptr;
}

BYTE *Texture::rotate(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::rotate() - ROTATE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::rotate() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::rotate() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::rotate(&layer[destinationLayer], value);

   return ptr;
}

BYTE *Texture::scale(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::scale() - SCALE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int width = 1 << ((((*ptr) & WIDTH)>>3) + 3);
   int height = 1 << (((*ptr) & HEIGHT) + 3);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::scale() - destinationLayer: %d; width: %d; height: %d\n", destinationLayer, width, height);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::scale(&layer[destinationLayer], width, height);

   return ptr;
}

BYTE *Texture::sphereMapping(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sphereMapping() - SPHERE MAPPING\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sphereMapping() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::sphereMapping(&layer[destinationLayer]);

   return ptr;
}

BYTE *Texture::tile(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::tile() - TILE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::tile() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int horizontalTile = *ptr++;
   int verticalTile = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::tile() - horizontalTile: %d; verticalTile: %d\n", horizontalTile, verticalTile);
#endif //__DEBUG

   if (fxOn)
      FX::tile(&layer[destinationLayer], horizontalTile, verticalTile);

   return ptr;
}

BYTE *Texture::twirl(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::twirl() - TWIRL\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::twirl() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::twirl() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::twirl(&layer[destinationLayer], value);

   return ptr;
}

BYTE *Texture::brightness(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::brightness() - BRIGHTNESS\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::brightness() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::brightness() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::brightness(&layer[destinationLayer], value);

   return ptr;
}

BYTE *Texture::bump(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::bump() - BUMP\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::bump() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::bump(&layer[destinationLayer]);

   return ptr;
}

BYTE *Texture::bumpMore(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::bumpMore() - BUMP MORE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::bumpMore() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::bumpMore(&layer[destinationLayer]);

   return ptr;
}

BYTE *Texture::contrast(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::contrast() - CONTRAST\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::contrast() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::contrast() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::contrast(&layer[destinationLayer], value);

   return ptr;
}

BYTE *Texture::edgeDetect(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::edgeDetect() - EDGE DETECT\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::edgeDetect() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::edgeDetect(&layer[destinationLayer]);

   return ptr;
}

BYTE *Texture::emboss(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::emboss() - EMBOSS\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int direction = ((*ptr) & EMBOSS_VAL)>>3;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::emboss() - destinationLayer: %d; direction: %d\n", destinationLayer, direction);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::emboss(&layer[destinationLayer], direction);

   return ptr;
}

BYTE *Texture::grayscale(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::grayscale() - GRAYSCALE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::grayscale() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::grayscale(&layer[destinationLayer]);

   return ptr;
}

BYTE *Texture::inverse(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::inverse() - INVERSE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::inverse() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::inverse(&layer[destinationLayer]);

   return ptr;
}

BYTE *Texture::posterize(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::posterize() - POSTERIZE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::posterize() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int levels = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::posterize() - levels: %d\n", levels);
#endif //__DEBUG

   if (fxOn)
      FX::posterize(&layer[destinationLayer], levels);

   return ptr;
}

BYTE *Texture::sharpen(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sharpen() - SHARPEN\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sharpen() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sharpen() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::sharpen(&layer[destinationLayer], value);

   return ptr;
}

BYTE *Texture::threshold(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::threshold() - THRESHOLD\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::threshold() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::threshold() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::threshold(&layer[destinationLayer], value);

   return ptr;
}

BYTE *Texture::fastBlur(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fastBlur() - FAST BLUR\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fastBlur() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int radius = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fastBlur() - radius: %d\n", radius);
#endif //__DEBUG

   if (fxOn)
      FX::fastBlur(&layer[destinationLayer], radius);

   return ptr;
}

BYTE *Texture::fastHorizontalBlur(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fastHorizontalBlur() - FAST HORIZONTAL BLUR\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fastHorizontalBlur() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int radius = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fastHorizontalBlur() - radius: %d\n", radius);
#endif //__DEBUG

   if (fxOn)
      FX::fastHorizontalBlur(&layer[destinationLayer], radius);

   return ptr;
}

BYTE *Texture::fastVerticalBlur(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fastVerticalBlur() - FAST VERTICAL BLUR\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fastVerticalBlur() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int radius = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::fastVerticalBlur() - radius: %d\n", radius);
#endif //__DEBUG

   if (fxOn)
      FX::fastVerticalBlur(&layer[destinationLayer], radius);

   return ptr;
}

BYTE *Texture::gaussianBlur(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::gaussianBlur() - GAUSSIAN BLUR\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::gaussianBlur() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int radius = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::gaussianBlur() - radius: %d\n", radius);
#endif //__DEBUG

   if (fxOn)
      FX::gaussianBlur(&layer[destinationLayer], radius);

   return ptr;
}

BYTE *Texture::gaussianHorizontalBlur(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::gaussianHorizontalBlur() - GAUSSIAN HORIZONTAL BLUR\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::gaussianHorizontalBlur() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int radius = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::gaussianHorizontalBlur() - radius: %d\n", radius);
#endif //__DEBUG

   if (fxOn)
      FX::gaussianHorizontalBlur(&layer[destinationLayer], radius);

   return ptr;
}

BYTE *Texture::gaussianVerticalBlur(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::gaussianVerticalBlur() - GAUSSIAN VERTICAL BLUR\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::gaussianVerticalBlur() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int radius = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::gaussianVerticalBlur() - radius: %d\n", radius);
#endif //__DEBUG

   if (fxOn)
      FX::gaussianVerticalBlur(&layer[destinationLayer], radius);

   return ptr;
}

BYTE *Texture::lineBlur(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::lineBlur() - LINE BLUR\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::lineBlur() - destinationLayer: %d\n", destinationLayer);
#endif //__DEBUG
   ptr++;

   int orientation = *ptr++;
   int radius = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::lineBlur() - orientation: %d; radius: %d\n", orientation, radius);
#endif //__DEBUG

   if (fxOn)
      FX::lineBlur(&layer[destinationLayer], orientation, radius);

   return ptr;
}

BYTE *Texture::add(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::add() - ADD\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::add() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::add(&layer[destinationLayer], &layer[sourceLayer]);

   return ptr;
}

BYTE *Texture::copy(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::copy() - COPY\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::copy() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::copy(&layer[destinationLayer], &layer[sourceLayer]);

   return ptr;
}

BYTE *Texture::directionalBlur(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::directionalBlur() - DIRECTIONAL BLUR\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::directionalBlur() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::directionalBlur() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::directionalBlur(&layer[destinationLayer], &layer[sourceLayer], value);

   return ptr;
}

BYTE *Texture::environment(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::environment() - ENVIRONMENT\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::environment() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::environment() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::environment(&layer[destinationLayer], &layer[sourceLayer], value);

   return ptr;
}

BYTE *Texture::mix(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::mix() - MIX\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::mix() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   int value = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::mix() - value: %d\n", value);
#endif //__DEBUG

   if (fxOn)
      FX::mix(&layer[destinationLayer], &layer[sourceLayer], value);

   return ptr;
}

BYTE *Texture::mul(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::mul() - MUL\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::mul() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::mul(&layer[destinationLayer], &layer[sourceLayer]);

   return ptr;
}

BYTE *Texture::randomDump(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::randomDump() - RANDOM DUMP\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::randomDump() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   int number = *ptr++;
   int seed = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::randomDump() - number: %d; seed: %d\n", number, seed);
#endif //__DEBUG

   if (fxOn)
      FX::randomDump(&layer[destinationLayer], &layer[sourceLayer], number, seed);

   return ptr;
}

BYTE *Texture::shade(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::shade() - SHADE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::shade() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::shade(&layer[destinationLayer], &layer[sourceLayer]);

   return ptr;
}

BYTE *Texture::sub(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sub() - SUB\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::sub() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::sub(&layer[destinationLayer], &layer[sourceLayer]);

   return ptr;
}

BYTE *Texture::xclor(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::xclor() - XOR\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int sourceLayer = ((*ptr) & SOURCE_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::xclor() - destinationLayer: %d; sourceLayer: %d\n", destinationLayer, sourceLayer);
#endif //__DEBUG
   ptr++;

   if (fxOn)
      FX::xclor(&layer[destinationLayer], &layer[sourceLayer]);

   return ptr;
}

BYTE *Texture::textureCurve(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::textureCurve() - TEXTURE CURVE\n");
#endif //__DEBUG
            
   //Read parameters
   int destinationLayer = ((*ptr) & DESTINATION_LAYER)>>6;
   int width = 1 << ((((*ptr) & WIDTH)>>3) + 3);
   int height = 1 << (((*ptr) & HEIGHT) + 3);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::textureCurve() - destinationLayer: %d; width: %d; height: %d\n", destinationLayer, width, height);
#endif //__DEBUG
   ptr++;

   char curveName[256];
   ptr = readString(curveName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::textureCurve() - curveName: %s\n", curveName);
#endif //__DEBUG
   
   Curve *curve = (Curve *)genera->getComponent(curveName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::textureCurve() - curve: %p\n", curve);
#endif //__DEBUG
   
   int textureLayer = ((*ptr) & TEXTURE_LAYER)>>6;
   int maskLayer = ((*ptr) & MASK_LAYER)>>4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::textureCurve() - textureLayer: %d; maskLayer: %d\n", textureLayer, maskLayer);
#endif //__DEBUG
   ptr++;

   int edgeWidth = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Texture::textureCurve() - edgeWidth: %d\n", edgeWidth);
#endif //__DEBUG

   if (fxOn && curve)
      FX::textureCurve(&layer[destinationLayer], width, height, curve, &layer[textureLayer], &layer[maskLayer], edgeWidth);

   return ptr;
}
