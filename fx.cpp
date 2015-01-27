#include <math.h>         //For mathematic operations
#include "fx.h"

#define DIV(n,d)   (((float)n)/((float)d))
#define REAL(n,d)  (DIV(n,d)-((int)DIV(n,d)))

inline int ABS(int x) {
   return (x>=0? x: -x);
}

//Performs cubic interpolation
//position indicates where to calculate the value
//values indicates the nodes on indexes -1, 0, 1 and 2
float cubicInterpolation(float position, int *values) {
   float a, b, c, d;
   float x3, x2, x;

   //Spline que pasa por todos los puntos
   /*a = -0.1667f*values[0] + 0.5000f*values[1] - 0.5000f*values[2] + 0.1667f*values[3];
   b = +0.5000f*values[0] - 1.0000f*values[1] + 0.5000f*values[2];
   c = -0.3333f*values[0] - 0.5000f*values[1] + 1.0000f*values[2] - 0.1667f*values[3];
   d =                              values[1];*/
   
   //Spline que pasa por todos los puntos y con tangente nula en los extremos del intervalo
   /*a = +2.0f*values[1] - 2.0f*values[2];
   b = -3.0f*values[1] + 3.0f*values[2];
   c = 0.0f;
   d =       values[1];*/

   //Spline que pasa por todos los puntos y con tangente calculada a partir de los vecinos de cada extremo
   a = 2.0f*values[1] + 0.5f*(values[2]-values[0]) - 2.0f*values[2] + 0.5f*(values[3]-values[1]);
   b = -3.0f*values[1] - 1.0f*(values[2]-values[0]) + 3.0f*values[2] - 0.5f*(values[3]-values[1]);
   c = 0.5f*(values[2]-values[0]);
   d = values[1];

   x = position - (int)position;
   x2 = x*x;
   x3 = x*x2;
   
   return a*x3 + b*x2 + c*x + d;
}

DWORD cubicColorInterpolation(float position, DWORD *colors) {
   int values[4], result;
   DWORD interpolatedColor;
   
   values[0] = (colors[0] & 0xFF);
   values[1] = (colors[1] & 0xFF);
   values[2] = (colors[2] & 0xFF);
   values[3] = (colors[3] & 0xFF);
   result = (int)cubicInterpolation(position, values);
   if (result > 255)
      result = 255;
   else if (result < 0)
      result = 0;
   interpolatedColor = result;
   
   values[0] = ((colors[0]>>8) & 0xFF);
   values[1] = ((colors[1]>>8) & 0xFF);
   values[2] = ((colors[2]>>8) & 0xFF);
   values[3] = ((colors[3]>>8) & 0xFF);
   result = (int)cubicInterpolation(position, values);
   if (result > 255)
      result = 255;
   else if (result < 0)
      result = 0;
   interpolatedColor += (result << 8);
   
   values[0] = ((colors[0]>>16) & 0xFF);
   values[1] = ((colors[1]>>16) & 0xFF);
   values[2] = ((colors[2]>>16) & 0xFF);
   values[3] = ((colors[3]>>16) & 0xFF);
   result = (int)cubicInterpolation(position, values);
   if (result > 255)
      result = 255;
   else if (result < 0)
      result = 0;
   interpolatedColor += (result << 16);
   
   return interpolatedColor;
}

DWORD bicubicInterpolation(Layer *layer, float x, float y) {
   int i, j, width, height, u, v;
   DWORD colors[4], *ptr;
   DWORD interpolatedColors[4];

   i = (int)x;
   j = (int)y;
   ptr = layer->getImageDataPtr();
   width = layer->getWidth();
   height = layer->getHeight();
   
   for (v = -1; v < 3; v++) {
      for (u = -1; u < 3; u++) {
         colors[u+1] = ptr[((j+v+height)%height)*width + ((i+u+width)%width)];
      }
      interpolatedColors[v+1] = cubicColorInterpolation(x, colors);
   }
   
   return cubicColorInterpolation(y, interpolatedColors);
}

void FX::checker(Layer *layer, DWORD color1, DWORD color2, int width, int height) {
   int x, y;
   DWORD *ptr;

   layer->setSize(width, height);
   ptr = layer->getImageDataPtr();

   for (y=0; y<height; y++) {
      for (x=0; x<width; x++) {
         if (((x < (width>>1)) && (y < (height>>1))) || ((x >= (width>>1)) && (y >= (height>>1))))
            *ptr++ = color1;
         else
            *ptr++ = color2;
      }
   }
}

void combine (GLdouble c[3], void *d[4], GLfloat w[4], void **out) {
   GLdouble *nv = new GLdouble[3];

   nv[0] = c[0];
   nv[1] = c[1];
   nv[2] = c[2];
   *out = nv; 
}

void FX::curve(Layer *layer, Curve *curve, int width, int height) {
   SDL_Surface *surface;
   int videoFlags;
   
   int firstPoint;
   int x, y;
   int j,t;
   float ax,ay,bx,by,cx,cy,dx,dy,aa,bb,b2,b3,a2,a3,px,py;
   static GLUtesselator *tess = gluNewTess();
   static double vertex[256*256][2];
   
   if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
      SDL_Quit( );

   /* the flags to pass to SDL_SetVideoMode */
   videoFlags  = SDL_OPENGLBLIT;          /* Enable OpenGL in SDL */
   videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */

   /* get a SDL surface */
   surface = SDL_SetVideoMode( width, height, 32, videoFlags );

   //////////////////////////////////////////////////////////////////////////////////////////////////
   
   layer->setSize(width, height);
   
   //Set destination buffer
   glDrawBuffer(GL_BACK);

   //Define tessellation callbacks
#ifdef WIN32
   gluTessCallback (tess, GLU_TESS_BEGIN, (GLvoid (__stdcall *) ())glBegin);
   gluTessCallback (tess, GLU_TESS_VERTEX, (GLvoid (__stdcall *) ())glVertex2dv);
   gluTessCallback (tess, GLU_TESS_END, (GLvoid (__stdcall *) ())glEnd);
   gluTessCallback (tess, GLU_TESS_COMBINE, (GLvoid (__stdcall *) ())combine);
#else
   gluTessCallback (tess, GLU_TESS_BEGIN, (GLvoid (*) ())glBegin);
   gluTessCallback (tess, GLU_TESS_VERTEX, (GLvoid (*) ())glVertex2dv);
   gluTessCallback (tess, GLU_TESS_END, (GLvoid (*) ())glEnd);
   gluTessCallback (tess, GLU_TESS_COMBINE, (GLvoid (*) ())combine);
#endif

   // Enable smooth shading
   glShadeModel( GL_SMOOTH );

   // Set background color
   glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

   // Disable depth testing
   glDisable( GL_DEPTH_TEST );

   // Setup viewport
   glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

   // Change to the projection matrix and set the viewing volume
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity( );

   // Set the viewport
   gluOrtho2D( 0, width, height, 0 );

   // Make sure we're changing the model view and not the projection
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity( );

   // Clear screen and depth buffer
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Set blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
   
   glColor4ub(255, 255, 255, 255);
   
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   gluTessBeginPolygon (tess, NULL);
   gluTessBeginContour (tess);
      
   firstPoint = 0;
   
   for (int i = 0; i < curve->getNumberOfPoints(); i++) {
      x = curve->getControlPoint(i)[0];
      y = curve->getControlPoint(i)[1];
   
      if (curve->getEndPoint(i)) {
         j = firstPoint;
         firstPoint = i+1;
      }
      else {
         if (i+1 < curve->getNumberOfPoints())
            j = i+1;
         else
            j = firstPoint;
      }
   
      if (curve->getVisible(i)) {
         ax = (float)curve->getControlPoint(i)[0];
         ay = (float)curve->getControlPoint(i)[1];
         bx = (float)curve->getRightTangent(i)[0];
         by = (float)curve->getRightTangent(i)[1];
         cx = (float)curve->getLeftTangent(j)[0];
         cy = (float)curve->getLeftTangent(j)[1];
         dx = (float)curve->getControlPoint(j)[0];
         dy = (float)curve->getControlPoint(j)[1];
   
         for (t = 0; t <= curve->getSegmentsPerCurve(); t++) {
            aa = t/(float)curve->getSegmentsPerCurve();
            bb = 1.0f - aa;
   
            b2 = bb*bb;
            b3 = b2*bb;
            a2 = aa*aa;
            a3 = a2*aa;
   
            px = ax*b3 + 3.0f*bx*b2*aa + 3.0f*cx*bb*a2 + dx*a3;
            py = ay*b3 + 3.0f*by*b2*aa + 3.0f*cy*bb*a2 + dy*a3;
   
            //Draw
            double data[3];
            data[0] = (px*width)/256;
            data[1] = height - (py*height)/256;
            data[2] = 0;
   
            vertex[i*(curve->getSegmentsPerCurve()+1) + t][0] = data[0];
            vertex[i*(curve->getSegmentsPerCurve()+1) + t][1] = data[1];
   
            gluTessVertex (tess, data, vertex[i*(curve->getSegmentsPerCurve()+1) + t]);
         }
      }
      if (curve->getEndPoint(i)) {
         gluTessEndContour (tess);
         gluTessEndPolygon (tess);
         
         gluTessBeginPolygon (tess, NULL);
         gluTessBeginContour (tess);
      }   
   }
      
   gluTessEndContour (tess);
   gluTessEndPolygon (tess);
   
   //Load texture data from openGL buffer
   glReadBuffer(GL_BACK);
   glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, layer->getImageDataPtr());
   
   // Disable blend
   glDisable(GL_BLEND);

   glFlush();

   SDL_Quit( );
}

void FX::calculateFractal(float *colorArray, int width, int height, int xSide, int ySide, int noise, Random *random) {
   float val,temp;
   int x,y,delta;

   if (xSide>1) {
      //Lineas pares
      for (y=0; y<height; y+=ySide) {
         delta = y*width;
         for (x=(xSide>>1); x<width; x+=xSide) {
            temp = (float)((colorArray[delta + x-(xSide>>1)] + colorArray[delta + ((x+(xSide>>1))%width)])/2.0f);
            
            val = temp + ((noise/4.0f)*((RAND_MAX>>1)-random->rand())*(((float)xSide)/width))/(RAND_MAX>>1);
            val = (val>=1.0f)? 1.0f : val;
            val = (val<0.0f)? 0.0f : val;
            
            colorArray[delta + x] = val;
         }
      }
   }

   if (ySide>1) {
      //Lineas impares
      for (y=(ySide>>1); y<height; y+=ySide) {
         delta = y*width;
         for (x=0; x<width; x+=xSide) {
            val = (float)((colorArray[(y-(ySide>>1))*width+x] + colorArray[((y+(ySide>>1))%height)*width+x])/2.0f + ((noise/4.0f)*((RAND_MAX>>1)-random->rand())*(((float)ySide)/height))/(RAND_MAX>>1));

            val = (val>=1.0f)? 1.0f : val;
            val = (val<0.0f)? 0.0f : val;
            
            colorArray[delta + x] = val;
         }
         for (x=(xSide>>1); x<width; x+=xSide) {
            val = (float)((colorArray[(y-(ySide>>1))*width+(x-(xSide>>1))] + colorArray[(y-(ySide>>1))*width+((x+(xSide>>1))%width)] + colorArray[((y+(ySide>>1))%height)*width+(x-(xSide>>1))] + colorArray[((y+(ySide>>1))%height)*width+((x+(xSide>>1))%width)])/4.0f + ((noise/4.0f)*((RAND_MAX>>1)-random->rand())*(((float)xSide+ySide)/(width+height)))/(RAND_MAX>>1));

            val = (val>=1.0f)? 1.0f : val;
            val = (val<0.0f)? 0.0f : val;

            colorArray[delta + x] = val;
         }
      }
   }

   //Recurse
   if (xSide == 2 && ySide == 2)
      return;
   if (xSide > 2 && ySide > 2)
      calculateFractal(colorArray,width,height,xSide>>1,ySide>>1,noise,random);
   else if (xSide > 2)
      calculateFractal(colorArray,width,height,xSide>>1,2,noise,random);
   else if (ySide > 2)
      calculateFractal(colorArray,width,height,2,ySide>>1,noise,random);
}

void FX::fractal(Layer *layer, Gradient *gradient, int width, int height, int noise, int seed) {
   int n;
   float *colorArray,*p;
   DWORD *ptr;

   p = colorArray = new float[width*height];
   Random random(seed);

   //Init fractal
   colorArray[0] = ((float)random.rand())/RAND_MAX;

   calculateFractal(colorArray, width, height, width, height, noise, &random);

   layer->setSize(width, height);
   ptr = layer->getImageDataPtr();

   for (n=0; n<width*height; n++)
      *ptr++ = gradient->getColorAt(*p++).getColor();
   
   delete []colorArray;
}

void FX::glow(Layer *layer, Gradient *gradient, int width, int height, int radius) {
   int x,y;
   float val;
   DWORD *ptr;

   layer->setSize(width, height);
   ptr = layer->getImageDataPtr();

   for (y=0; y<height; y++) {
      for (x=0; x<width; x++) {
         val = ((x-(width/2.0f))*(x-(width/2.0f))+(y-(height/2.0f))*(y-(height/2.0f)))/((width/2.0f)*(height/2.0f));
         val *= 128.0f / radius;
         if (val>=1.0f) val = 1.0f;

         *ptr++ = gradient->getColorAt(val).getColor();
      }
   }
}

void FX::noise(Layer *layer, Gradient *gradient, int width, int height, int seed) {
   int i;
   float r;
   DWORD *ptr;

   Random random(seed);

   layer->setSize(width, height);
   ptr = layer->getImageDataPtr();

   for (i=0; i<width*height; i++) {
      r = ((float)random.rand())/RAND_MAX;
      *ptr++ = gradient->getColorAt(r).getColor();
   }
}

void FX::picture(Layer *layer, int width, int height, BYTE *data) {
   int i,r,g,b;
   DWORD *ptr;

   layer->setSize(width, height);
   ptr = layer->getImageDataPtr();

   for (i=0; i<width*height; i++) {
      r = *data++;
      g = *data++;
      b = *data++;
      *ptr++ = r + (g<<8) + (b<<16);
   }
}

void FX::sinus(Layer *layer, Gradient *gradient, int width, int height, int turbX1, int turbX2, int turbY1, int turbY2, int phase1, int phase2) {
   float turb,val;
   int x,y;
   DWORD *ptr;

   layer->setSize(width, height);
   ptr = layer->getImageDataPtr();

   for (y=0; y<height; y++) {
      for (x=0; x<width; x++) {
         turb = (float)(sin((float)(x*2.0f*PI/((float)width))) * (turbX1/64.0f));
         val = (float)(sin((float)(y*2.0f*PI/((float)height)+turb+(phase1*2.0f*PI/256.0f))) * (turbX2/64.0f));

         turb = (float)(sin((float)(y*2.0f*PI/((float)height))) * (turbY1/64.0f));
         val += (float)(sin((float)(x*2.0f*PI/((float)width)+turb+(phase2*2.0f*PI/256.0f))) * (turbY2/64.0f));

         if (val < 0.0f) val=-val;

         *ptr++ = gradient->getColorAt(REAL(val,1.0f)).getColor();
      }
   }
}

void FX::flip(Layer *layer, bool horizontal, bool vertical) {
   int x,y,n,width,height;
   DWORD *ptr,*temp,*p,*p2,delta;

   p2 = ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD[width*height];

   if (horizontal) {
      for (y = 0; y < height; y++) {
         delta = y*width + width-1;
         for (x = 0; x < width; x++) {
            *p++ = ptr[delta - x];
         }
      }
      
      p = temp;
      for (n = 0; n < width*height; n++)
         *p2++ = *p++;
   }

   p2 = ptr;
   p = temp;
   if (vertical) {
      for (y = 0; y < height; y++) {
         delta = (height-1-y)*width;
         for (x = 0; x < width; x++) {
            *p++ = ptr[delta + x];
         }
      }
      
      p = temp;
      for (n = 0; n < width*height; n++)
         *p2++ = *p++;
   }

   delete []temp;
}

void FX::glass(Layer *layer, int turbX1, int turbX2, int turbY1, int turbY2, int phase1, int phase2, int intensity) {
   float turb,val,dx,dy;
   int x,y,n,width,height;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];

   for (y=0; y<height; y++) {
      for (x=0; x<width; x++) {
         turb = (float)(sin((float)(x*2.0f*PI/((float)width))) * (turbX1/64.0f));
         val = (float)(sin((float)(y*2.0f*PI/((float)height)+turb+(phase1*2.0f*PI/256.0))) * (turbX2/64.0f));

         turb = (float)(sin((float)(y*2.0f*PI/((float)height))) * (turbY1/64.0f));
         val += (float)(sin((float)(x*2.0f*PI/((float)width)+turb+(phase2*2.0f*PI/256.0))) * (turbY2/64.0f));

         dx = intensity*((float)cos((float)(val*2.0f*PI)));
         dy = intensity*((float)sin((float)(val*2.0f*PI)));

         *p++ = bicubicInterpolation(layer, x+dx, y+dy);
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete temp;
}

void FX::move(Layer *layer, int direction, int value) {
   DWORD *ptr,*temp,*p;
   int x,y,n,width,height;
   int deltaX = 0;
   int deltaY = 0;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD[width*height];

   switch (direction) {
   case UPLEFT :
      deltaX = value;
      deltaY = value;
      break;
   case UP :
      deltaX = 0;
      deltaY = value;
      break;
   case UPRIGHT :
      deltaX = -value;
      deltaY = value;
      break;
   case LEFT :
      deltaX = value;
      deltaY = 0;
      break;
   case RIGHT :
      deltaX = -value;
      deltaY = 0;
      break;
   case DOWNLEFT :
      deltaX = value;
      deltaY = -value;
      break;
   case DOWN :
      deltaX = 0;
      deltaY = -value;
      break;
   case DOWNRIGHT :
      deltaX = -value;
      deltaY = -value;
      break;
   }

   //Add width and height to avoid instability problems with % operator
   deltaX += width;
   deltaY += height;
   
   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         *p++ = ptr[((x+deltaX) % width) + ((y+deltaY) % height)*width];
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete []temp;
}

void FX::pixellate(Layer *layer, int value) {
   float deltaX,deltaY;
   int n,x,y,width,height;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];

   deltaX = width/(float)value;
   deltaY = height/(float)value;
   
   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         *p++ = ptr[((int)(((int)(x/deltaX))*deltaX)) + ((int)(((int)(y/deltaY))*deltaY))*width];
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete []temp;
}

void FX::randomize(Layer *layer, int value, int seed) {
   int n,x,y,width,height,r1,r2;
   DWORD *ptr,*temp,*p;

   Random random(seed);

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];

   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         r1 = (int)(value*((random.rand()/(RAND_MAX+1.0f))-0.5f));
         r2 = (int)(value*((random.rand()/(RAND_MAX+1.0f))-0.5f));
         *p++ = ptr[((x+width+r1)%width) + ((y+height+r2)%height)*width];
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete []temp;
}

void FX::rotate(Layer *layer, int value) {
   int n,x,y,width,height,deltaX,deltaY;
   DWORD *ptr,*temp,*p;
   float dx, dy;
   
   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];

   for (y = 0; y < height; y++) {
      deltaY = (y-(height>>1));
      for (x = 0; x < width; x++) {
         deltaX = (x-(width>>1));
         
         dx = deltaX*cos(value*PI/128.0f) + deltaY*sin(value*PI/128.0f);
         dy = deltaX*sin(value*PI/128.0f) + deltaY*cos(value*PI/128.0f);
         
         *p++ = bicubicInterpolation(layer, (width>>1) + dx, (height>>1) - dy);
      }
   }
   
   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;
   
   delete []temp;
}

void FX::scale(Layer *layer, int width, int height) {
   int x,y;
   float i,j,deltaX,deltaY;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();

   p = temp = new DWORD [width*height];

   deltaX = ((float)layer->getWidth()) / ((float)width);
   deltaY = ((float)layer->getHeight()) / ((float)height);

   j = deltaY/2.0f;
   for (y = 0; y < height; y++) {
      i = deltaX/2.0f;
      for (x = 0; x < width; x++) {
         *p++ = bicubicInterpolation(layer, i, j);
         i += deltaX;
      }
      j += deltaY;
   }

   layer->setSize(width, height);
   ptr = layer->getImageDataPtr();

   p = temp;
   for (x = 0; x < width*height; x++)
      *ptr++ = *p++;
   
   delete []temp;
}

void FX::sphereMapping(Layer *layer) {
   int x,y,width,height,middleY,mask,i,r,g,b,intervals,intervalWidth,delta;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];

   middleY = height / 2;
   mask = width-1;

   for (y=0; y < height; y++) {

      //Calculates number of intervals in current texture line:
      //
      //                  y - height/2                 width - 1
      // length = [ cos( -------------- * PI) + 1 ] * ----------- + 1
      //                    height/2                       2
      //
      //This way: 1 <= intervals <= width
      intervals = (int)((cos((y-middleY)*2.0f*PI/height)+1) * (width-1)/2.0f + 1);

      intervalWidth = width/intervals;      //Width of intervals in current line

      //Calculates average color of each texel
      for (x = 0; x < width; x++) {
         r = g = b = 0;
         delta = y*width;
         for (i = 0; i < intervalWidth; i++) {
            r += (ptr[delta + ((x + i - intervalWidth/2)&mask)] >> 16) & 0xFF;
            g += (ptr[delta + ((x + i - intervalWidth/2)&mask)] >> 8) & 0xFF;
            b += (ptr[delta + ((x + i - intervalWidth/2)&mask)]) & 0xFF;
         }
         r /= intervalWidth;
         g /= intervalWidth;
         b /= intervalWidth;
         *p++ = (r << 16) | (g << 8) | b;
      }
   }

   p = temp;
   for (i = 0; i < width*height; i++)
      *ptr++ = *p++;
   
   delete []temp;
}

void FX::tile(Layer *layer, int horizontalTile, int verticalTile) {
   int n,x,y,width,height;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];

   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         *p++ = ptr[((y*verticalTile)%height)*width + ((x*horizontalTile)%width)];
      }
   }   

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete []temp;
}

void FX::twirl(Layer *layer, int value) {
   int n,x,y,midX,midY,oldX,oldY,width,height;
   float radio,angle, dx, dy;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];

   midX = width >> 1;
   midY = height >> 1;

   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         oldX = x - midX;
         oldY = y - midY;
         radio = (float)sqrt(oldX*oldX+oldY*oldY);

         if ((radio < midX) && (radio < midY)) {
            angle = (value-128)*((128.0f-radio)/128.0f)/(2.0f*PI);

            dx = oldX*cos(angle) + oldY*sin(angle);
            dy = oldY*cos(angle) - oldX*sin(angle);
            
            *p++ = bicubicInterpolation(layer, midX+dx, midY+dy);
         }
         else
            *p++ = ptr[x + y*width];
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete []temp;
}

void FX::brightness(Layer *layer, int value) {
   DWORD color;
   int n,add,r,g,b,width,height;
   DWORD *ptr;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   add = value-128;
   for (n = 0; n < width*height; n++) {
      color = *ptr;

      r = (color & 0xFF) + add;
      if (r>255) r = 255;
      else if (r<0) r = 0;

      g = ((color>>8) & 0xFF) + add;
      if (g>255) g = 255;
      else if (g<0) g = 0;

      b = ((color>>16) & 0xFF) + add;
      if (b>255) b = 255;
      else if (b<0) b = 0;

      *ptr++ = r + (g<<8) + (b<<16);
   }
}

void FX::bump(Layer *layer) {
   DWORD color1,color2,color3,color4;
   int n,x,y,r,g,b,width,height;
   DWORD *ptr;
   BYTE *temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new BYTE [width*height];
   
   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         color1 = ptr[((y+height-1)%height)*width + x];
         color2 = ptr[y*width + ((x+width-1)%width)];
         color3 = ptr[y*width + ((x+width+1)%width)];
         color4 = ptr[((y+height+1)%height)*width + x];

         r = 255 - ((ABS((color1&0xFF) - (color4&0xFF)) +
                     ABS((color2&0xFF) - (color3&0xFF))) >> 1);
         g = 255 - ((ABS(((color1>>8)&0xFF) - ((color4>>8)&0xFF)) +
                     ABS(((color2>>8)&0xFF) - ((color3>>8)&0xFF))) >> 1);
         b = 255 - ((ABS(((color1>>16)&0xFF) - ((color4>>16)&0xFF)) +
                     ABS(((color2>>16)&0xFF) - ((color3>>16)&0xFF))) >> 1);

         *p++ = (r + g + b) / 3;
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++) {
      r = *p * (*ptr & 0xFF);
      g = *p * ((*ptr>>8) & 0xFF);
      b = *p++ * ((*ptr>>16) & 0xFF);

      r >>= 8;
      g >>= 8;
      b >>= 8;

      *ptr++ = r + (g<<8) + (b<<16);
   }

   delete []temp;
}

void FX::bumpMore(Layer *layer) {
   DWORD color1,color2,color3,color4;
   int n,x,y,r,g,b,width,height;
   DWORD *ptr;
   BYTE *temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new BYTE [width*height];
   
   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         color1 = ptr[((y+height-1)%height)*width + x];
         color2 = ptr[y*width + ((x+width-1)%width)];
         color3 = ptr[y*width + ((x+width+1)%width)];
         color4 = ptr[((y+height+1)%height)*width + x];

         r = 255 - ((ABS((color1&0xFF) - (color4&0xFF)) +
                     ABS((color2&0xFF) - (color3&0xFF))) << 1);
         g = 255 - ((ABS(((color1>>8)&0xFF) - ((color4>>8)&0xFF)) +
                     ABS(((color2>>8)&0xFF) - ((color3>>8)&0xFF))) << 1);
         b = 255 - ((ABS(((color1>>16)&0xFF) - ((color4>>16)&0xFF)) +
                     ABS(((color2>>16)&0xFF) - ((color3>>16)&0xFF))) << 1);

         *p++ = (r + g + b) / 3;
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++) {
      r = (*p * (*ptr & 0xFF)) / 192;
      g = (*p * ((*ptr>>8) & 0xFF)) / 192;
      b = (*p++ * ((*ptr>>16) & 0xFF)) / 192;

      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;

      *ptr++ = r + (g<<8) + (b<<16);
   }

   delete []temp;
}

void FX::contrast(Layer *layer, int value) {
   int n,r,g,b,width,height;
   float scale;
   DWORD *ptr;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   if (value > 128)
      scale = (128 + 8*(value-128)) / 128.0f;
   else
      scale = 128.0f / (128+8*(128-value));

   for (n = 0; n < width*height; n++) {
      r = ((int)(*ptr & 0xFF)) - 128;
      r = (int)(r*scale)+128;
      if (r>255) r = 255;
      else if (r<0) r = 0;

      g = ((int)((*ptr>>8) & 0xFF)) - 128;
      g = (int)(g*scale)+128;
      if (g>255) g = 255;
      else if (g<0) g = 0;

      b = ((int)((*ptr>>16) & 0xFF)) - 128;
      b = (int)(b*scale)+128;
      if (b>255) b = 255;
      else if (b<0) b = 0;

      *ptr++ = r + (g<<8) + (b<<16);
   }
}

void FX::edgeDetect(Layer *layer) {
   DWORD color1,color2,color3,color4;
   int n,x,y,r,g,b,width,height;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];
   
   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         color1 = ptr[((y+height-1)%height)*width + x];
         color2 = ptr[y*width + ((x+width-1)%width)];
         color3 = ptr[y*width + ((x+width+1)%width)];
         color4 = ptr[((y+height+1)%height)*width + x];

         r = (ABS((color1&0xFF) - (color4&0xFF)) +
              ABS((color2&0xFF) - (color3&0xFF))) >> 1;
         g = (ABS(((color1>>8)&0xFF) - ((color4>>8)&0xFF)) +
              ABS(((color2>>8)&0xFF) - ((color3>>8)&0xFF))) >> 1;
         b = (ABS(((color1>>16)&0xFF) - ((color4>>16)&0xFF)) +
              ABS(((color2>>16)&0xFF) - ((color3>>16)&0xFF))) >> 1;

         *p++ = r + (g<<8) + (b<<16);
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete []temp;
}

void FX::emboss(Layer *layer, int direction) {
   DWORD color1,color2;
   int n,x,y,r,g,b,val,width,height;
   DWORD *ptr,*temp,*p;
   int xOffset = 0;
   int yOffset = 0;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD[width*height];

   switch (direction) {
   case UPLEFT :
      xOffset = +1;
      yOffset = +1;
      break;
   case UP :
      xOffset = +0;
      yOffset = +1;
      break;
   case UPRIGHT :
      xOffset = -1;
      yOffset = +1;
      break;
   case LEFT :
      xOffset = +1;
      yOffset = +0;
      break;
   case RIGHT :
      xOffset = -1;
      yOffset = +0;
      break;
   case DOWNLEFT :
      xOffset = +1;
      yOffset = -1;
      break;
   case DOWN :
      xOffset = +0;
      yOffset = -1;
      break;
   case DOWNRIGHT :
      xOffset = -1;
      yOffset = -1;
      break;
   }

   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         color1 = ptr[((y+height+yOffset)%height)*width + ((x+width+xOffset)%width)];
         color2 = ptr[((y+height-yOffset)%height)*width + ((x+width-xOffset)%width)];

         r = 128 + (((int)color1&0xFF) - ((int)color2&0xFF));
         g = 128 + ((((int)color1>>8)&0xFF) - (((int)color2>>8)&0xFF));
         b = 128 + ((((int)color1>>16)&0xFF) - (((int)color2>>16)&0xFF));
         
         val = (r + g + b)/3;
         if (val > 255) val = 255;
         if (val < 0) val = 0;
         
         *p++ = val + (val<<8) + (val<<16);
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete []temp;
}

void FX::grayscale(Layer *layer) {
   DWORD color;
   int n,t,width,height;
   DWORD *ptr;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   for (n = 0; n < width*height; n++) {
      color = *ptr;
      t = (((color>>16) & 0xFF) + ((color>>8) & 0xFF) + (color & 0xFF)) / 3;
      *ptr++ = (t<<16) + (t<<8) + t;
   }
}

void FX::inverse(Layer *layer) {
   int n,width,height;
   DWORD *ptr;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   for (n = 0; n < width*height; n++)
      *ptr++ = ~(*ptr);
}

void FX::posterize(Layer *layer, int levels) {
   int n,width,height;
   int r,g,b;
   int delta;
   DWORD *ptr;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   if (levels == 0)
      levels = 1;
   
   delta = 256/levels;

   for (n = 0; n < width*height; n++) {
      r = *ptr & 0xFF;
      g = (*ptr >> 8) & 0xFF;
      b = (*ptr >> 16) & 0xFF;
      
      //Quantization
      r = (r/delta) * delta + (delta>>1);
      if (r > 255)
         r = 255;
      g = (g/delta) * delta + (delta>>1);
      if (g > 255)
         g = 255;
      b = (b/delta) * delta + (delta>>1);
      if (b > 255)
         b = 255;

      *ptr++ = r + (g<<8) + (b<<16);
   }
}

void FX::sharpen(Layer *layer, int value) {
   DWORD color,color1,color2,color3,color4;
   int n,x,y,maskX,maskY,r,g,b,width,height;
   DWORD *ptr,*p2,*temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   maskX = width - 1;
   maskY = height - 1;

   p = temp = new DWORD [width*height];

   for (; value; value--) {
      p2 = ptr;
      n = 0;
      for (y = 0; y < height; y++) {
         for (x = 0; x < width; x++) {
            color = p2[n];
            color1 = p2[((y+height-1)%height)*width + x];
            color2 = p2[y*width + ((x+width-1)%width)];
            color3 = p2[y*width + ((x+width+1)%width)];
            color4 = p2[((y+height+1)%height)*width + x];

            r = 2*(color&0xFF) - ((color1>>2)&0x3F) - ((color2>>2)&0x3F) - ((color3>>2)&0x3F) - ((color4>>2)&0x3F);
            if (r < 0) r = 0;
            else if (r > 255) r = 255;

            g = 2*((color>>8)&0xFF) - ((color1>>10)&0x3F) - ((color2>>10)&0x3F) - ((color3>>10)&0x3F) - ((color4>>10)&0x3F);
            if (g < 0) g = 0;
            else if (g > 255) g = 255;

            b = 2*((color>>16)&0xFF) - ((color1>>18)&0x3F) - ((color2>>18)&0x3F) - ((color3>>18)&0x3F) - ((color4>>18)&0x3F);
            if (b < 0) b = 0;
            else if (b > 255) b = 255;

            *p++ = r + (g<<8) + (b<<16);

            n++;
         }
      }
      p = temp;
      p2 = ptr;
      for (n = 0; n < width*height; n++)
         *p2++ = *p++;
   }

   delete []temp;
}

void FX::threshold(Layer *layer, int value) {
   int n,width,height,r,g,b;
   DWORD *ptr;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   for (n = 0; n < width*height; n++) {
      r = (int)(*ptr & 0xFF);
      if (r <= value)
         r = 0;
      else
         r = 255;

      g = (int)((*ptr>>8) & 0xFF);
      if (g <= value)
         g = 0;
      else
         g = 255;

      b = (int)((*ptr>>16) & 0xFF);
      if (b <= value)
         b = 0;
      else
         b = 255;

      *ptr++ = r + (g<<8) + (b<<16);
   }
}

void FX::fastBlur(Layer *layer, int radius) {
   FX::fastHorizontalBlur(layer, radius);
   FX::fastVerticalBlur(layer, radius);
}

void FX::fastHorizontalBlur(Layer *layer, int radius) {
   DWORD *temp, *ptr, *p, val;
   int x, y, i, r, g, b, delta, sum;
   int width, height;
   
   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD[width*height];
   
   sum = 2*radius+1;
   for (y = 0; y < height; y++) {
      //Init row
      r = g = b = 0;
      delta = y*width;
      
      //Calculate first pixel
      for (i = -radius; i <= radius; i++) {
         val = ptr[delta + ((i+width)%width)];
         r += val & 0xFF;
         g += (val>>8) & 0xFF;
         b += (val>>16) & 0xFF;
      }
      
      *p++ = (r/sum) + ((g/sum)<<8) + ((b/sum)<<16);
      
      //Calculate remaining pixels
      for (x = 1; x < width; x++) {
         val = ptr[delta + ((x+width-radius-1)%width)];
         r -= val & 0xFF;
         g -= (val>>8) & 0xFF;
         b -= (val>>16) & 0xFF;
         
         val = ptr[delta + ((x+width+radius)%width)];
         r += val & 0xFF;
         g += (val>>8) & 0xFF;
         b += (val>>16) & 0xFF;
         
         *p++ = (r/sum) + ((g/sum)<<8) + ((b/sum)<<16);
      }
   }
   p = temp;
   for (i = 0; i < width*height; i++)
      *ptr++ = *p++;
      
   delete []temp;
}

void FX::fastVerticalBlur(Layer *layer, int radius) {
   DWORD *temp, *ptr, *p, val;
   int x, y, i, r, g, b, sum;
   int width, height;
   
   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD[width*height];
   
   sum = 2*radius+1;
   for (x = 0; x < width; x++) {
      //Init column
      r = g = b = 0;
      
      //Calculate first pixel
      for (i = -radius; i <= radius; i++) {
         val = ptr[((i+height)%height)*width + x];
         r += val & 0xFF;
         g += (val>>8) & 0xFF;
         b += (val>>16) & 0xFF;
      }
      
      *p++ = (r/sum) + ((g/sum)<<8) + ((b/sum)<<16);
      
      //Calculate remaining pixels
      for (y = 1; y < height; y++) {
         val = ptr[((y+height-radius-1)%height)*width + x];
         r -= val & 0xFF;
         g -= (val>>8) & 0xFF;
         b -= (val>>16) & 0xFF;
         
         val = ptr[((y+height+radius)%height)*width + x];
         r += val & 0xFF;
         g += (val>>8) & 0xFF;
         b += (val>>16) & 0xFF;
         
         *p++ = (r/sum) + ((g/sum)<<8) + ((b/sum)<<16);
      }
   }
   
   p = temp;
   for (y = 0; y < height; y++)
      for (x = 0; x < width; x++)
         ptr[y*width + x] = p[x*width + y];

   delete []temp;
}

void FX::gaussianBlur(Layer *layer, int radius) {
   if (radius > 0) {
      FX::gaussianHorizontalBlur(layer, radius);
      FX::gaussianVerticalBlur(layer, radius);
   }
}

void FX::gaussianHorizontalBlur(Layer *layer, int radius) {
   int n,x,y,width,height,delta;
   DWORD color;
   float *weights,sum,rSum,gSum,bSum;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];
   
   weights = new float[2*radius+1];
   
   //Precalculate gaussian weights
   sum = 0;
   for (n = 0; n < 2*radius+1; n++) {
      weights[n] = (float)(exp(-4.0f*(n-radius)*(n-radius)/(radius*radius)));
      sum += weights[n];
   }
   for (n = 0; n < 2*radius+1; n++)
      weights[n] /= sum;
   
   //Do actual blur
   for (y = 0; y < height; y++) {
      delta = y*width;
      for (x = 0; x < width; x++) {
         rSum = gSum = bSum = 0;
         for (n = 0; n < 2*radius+1; n++) {
            color = ptr[delta + ((x+width-radius+n)%width)];

            rSum += (color & 0xFF) * weights[n];
            gSum += ((color >> 8) & 0xFF) * weights[n];
            bSum += ((color >> 16) & 0xFF) * weights[n];
         }

         *p++ = ((int)rSum) | (((int)gSum) << 8) | (((int)bSum) << 16);
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete []temp;
   
   delete []weights;
}

void FX::gaussianVerticalBlur(Layer *layer, int radius) {
   int n,x,y,width,height;
   DWORD color;
   float *weights,sum,rSum,gSum,bSum;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();

   width = layer->getWidth();
   height = layer->getHeight();

   p = temp = new DWORD [width*height];
   
   weights = new float[2*radius+1];
   
   //Precalculate gaussian weights
   sum = 0;
   for (n = 0; n < 2*radius+1; n++) {
      weights[n] = (float)(exp(-5.0f*(n-radius)*(n-radius)/(radius*radius)));
      sum += weights[n];
   }
   for (n = 0; n < 2*radius+1; n++)
      weights[n] /= sum;
   
   //Do actual blur
   for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
         rSum = gSum = bSum = 0;
         for (n = 0; n < 2*radius+1; n++) {
            color = ptr[((y+height-radius+n)%height)*width + x];

            rSum += (color & 0xFF) * weights[n];
            gSum += ((color >> 8) & 0xFF) * weights[n];
            bSum += ((color >> 16) & 0xFF) * weights[n];
         }

         *p++ = ((int)rSum) | (((int)gSum) << 8) | (((int)bSum) << 16);
      }
   }

   p = temp;
   for (y = 0; y < height; y++)
      for (x = 0; x < width; x++)
         ptr[y*width + x] = p[x*width + y];
   
   delete []temp;
   
   delete []weights;
}

void FX::lineBlur(Layer *layer, int orientation, int radius) {
   DWORD color;
   int i,width,height,rSum,gSum,bSum,x,y,n;
   float arg,xPos,yPos,dx,dy;
   DWORD *ptr,*temp,*p;

   ptr = layer->getImageDataPtr();
   
   width = layer->getWidth();
   height = layer->getHeight();
   
   p = temp = new DWORD[width*height];

   for (n = 0; n < width*height; n++)
      *p++ = 0;

   arg = orientation*2.0f*PI/256.0f;
   dx = -(float)cosf(arg);
   dy = -(float)sinf(arg);
   
   p = temp;
   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         rSum = gSum = bSum = 0;
         xPos = (float)x - radius*dx/2;
         yPos = (float)y - radius*dy/2;
         for (i = 0; i < radius; i++) {
            color = ptr[(((int)yPos+height)%height)*width + (((int)xPos+width)%width)];

            rSum += color & 0xFF;
            gSum += (color >> 8) & 0xFF;
            bSum += (color >> 16) & 0xFF;

            xPos += dx;
            yPos += dy;
         }

         *p++ = (rSum/radius) | ((gSum/radius) << 8) | ((bSum/radius) << 16);
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *ptr++ = *p++;

   delete []temp;
}

void FX::add(Layer *dst, Layer *src) {
   int n,r,g,b;
   int width, height;
   DWORD *dest,*sour;
   Layer scaled;

   dest = dst->getImageDataPtr();
   sour = src->getImageDataPtr();

   width = dst->getWidth();
   height = dst->getHeight();
   
   if (src->getWidth() != width || src->getHeight() != height) {
      FX::copy(&scaled, src);
      FX::scale(&scaled, width, height);
      sour = scaled.getImageDataPtr();
   }

   for (n = 0; n < width*height; n++) {
      r = (*dest&0xFF) + (*sour&0xFF);
      if (r > 255) r = 255;

      g = ((*dest>>8)&0xFF) + ((*sour>>8)&0xFF);
      if (g > 255) g = 255;

      b = ((*dest>>16)&0xFF) + ((*sour++>>16)&0xFF);
      if (b > 255) b = 255;

      *dest++ = r | (g<<8) | (b<<16);
   }
}

void FX::copy(Layer *dst, Layer *src) {
   int n;
   int width, height;
   DWORD *dest,*sour;

   if (dst == src)
      return;

   dst->setSize(src->getWidth(), src->getHeight());

   dest = dst->getImageDataPtr();
   sour = src->getImageDataPtr();
   
   width = dst->getWidth();
   height = dst->getHeight();

   for (n = 0; n < width*height; n++)
      *dest++ = *sour++;
}

void FX::directionalBlur(Layer *dst, Layer *src, int value) {
   DWORD color;
   int i,width,height,rSum,gSum,bSum,x,y,n;
   float arg,xPos,yPos,dx,dy;
   DWORD *dest,*sour,*temp,*p;
   Layer scaled;

   dest = dst->getImageDataPtr();
   sour = src->getImageDataPtr();
   
   width = dst->getWidth();
   height = dst->getHeight();
   
   if (src->getWidth() != width || src->getHeight() != height) {
      FX::copy(&scaled, src);
      FX::scale(&scaled, width, height);
      sour = scaled.getImageDataPtr();
   }

   p = temp = new DWORD[width*height];

   for (n = 0; n < width*height; n++)
      *p++ = 0;

   p = temp;
   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         color = *sour++;
         arg = (float)(((color >> 16) & 0xFF) + ((color >> 8) & 0xFF) + (color & 0xFF));
         arg *= 2.0f*PI/(3*256.0f);
         dx = (float)cosf(arg);
         dy = (float)sinf(arg);
         rSum = gSum = bSum = 0;
         xPos = (float)x;
         yPos = (float)y;
         for (i = 0; i < value; i++) {
            color = dest[(((int)yPos+height)%height)*width + (((int)xPos+width)%width)];

            rSum += color & 0xFF;
            gSum += (color >> 8) & 0xFF;
            bSum += (color >> 16) & 0xFF;

            xPos += dx;
            yPos += dy;
         }

         *p++ = (rSum/value) | ((gSum/value) << 8) | ((bSum/value) << 16);
      }
   }

   p = temp;
   for (n = 0; n < width*height; n++)
      *dest++ = *p++;

   delete []temp;
}

void FX::environment(Layer *dst, Layer *src, int value) {
   DWORD color,colorEnv;
   int width,height,x,y,dx,dy,r,g,b;
   DWORD *dest,*sour,*hmap;
   Layer heightmap;
   Layer scaled;

   dest = dst->getImageDataPtr();
   sour = src->getImageDataPtr();
   
   width = dst->getWidth();
   height = dst->getHeight();
   
   if (src->getWidth() != width || src->getHeight() != height) {
      FX::copy(&scaled, src);
      FX::scale(&scaled, width, height);
      sour = scaled.getImageDataPtr();
   }

   FX::copy(&heightmap, dst);
   FX::grayscale(&heightmap);
   hmap = heightmap.getImageDataPtr();
   
   for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
         dx = (hmap[y*width + ((x+width-1)%width)] & 0xFF) -
              (hmap[y*width + ((x+width+1)%width)] & 0xFF);
         dy = (hmap[((y+height-1)%height)*width + x] & 0xFF) -
              (hmap[((y+height+1)%height)*width + x] & 0xFF);

         dx = (dx*value) >> 2;
         dy = (dy*value) >> 2;

         color = *dest;
         colorEnv = sour[((y+height+dy)%height)*width + ((x+width+dx)%width)];

         r = (color & 0xFF) + (colorEnv & 0xFF);
         if (r > 255) r = 255;

         g = ((color >> 8) & 0xFF) + ((colorEnv >> 8) & 0xFF);
         if (g > 255) g = 255;

         b = ((color >> 16) & 0xFF) + ((colorEnv >> 16) & 0xFF);
         if (b > 255) b = 255;

         *dest++ = r | (g << 8) | (b << 16);
      }
   }
}

void FX::mix(Layer *dst, Layer *src, int value) {
   int n,r,g,b,mul1,mul2;
   int width, height;
   DWORD *dest,*sour;
   Layer scaled;

   dest = dst->getImageDataPtr();
   sour = src->getImageDataPtr();
   
   width = dst->getWidth();
   height = dst->getHeight();
   
   if (src->getWidth() != width || src->getHeight() != height) {
      FX::copy(&scaled, src);
      FX::scale(&scaled, width, height);
      sour = scaled.getImageDataPtr();
   }

   mul1 = 255 - value;
   mul2 = value;
   for (n = 0; n < width*height; n++) {
      r = ((*dest&0xFF) * mul1 +
           (*sour&0xFF) * mul2) / 255;
      g = (((*dest>>8)&0xFF) * mul1 +
           ((*sour>>8)&0xFF) * mul2) / 255;
      b = (((*dest>>16)&0xFF) * mul1 +
           ((*sour++>>16)&0xFF) * mul2) / 255;
      *dest++ = r | (g<<8) | (b<<16);
   }
}

void FX::mul(Layer *dst, Layer *src) {
   int n,r,g,b;
   int width, height;
   DWORD *dest,*sour;
   Layer scaled;

   dest = dst->getImageDataPtr();
   sour = src->getImageDataPtr();
   
   width = dst->getWidth();
   height = dst->getHeight();
   
   if (src->getWidth() != width || src->getHeight() != height) {
      FX::copy(&scaled, src);
      FX::scale(&scaled, width, height);
      sour = scaled.getImageDataPtr();
   }

   for (n = 0; n < width*height; n++) {
      r = ((*dest&0xFF) * (*sour&0xFF))/255;
      g = (((*dest>>8)&0xFF) * ((*sour>>8)&0xFF))/255;
      b = (((*dest>>16)&0xFF) * ((*sour++>>16)&0xFF))/255;
      
      *dest++ = r | (g<<8) | (b<<16);
   }
}

void FX::randomDump(Layer *dst, Layer *src, int number, int seed) {
   DWORD color1,color2;
   int i,xDest,yDest,xSrc,ySrc,r,g,b,offset;
   int width, height;
   DWORD *dest,*sour,*temp, *p, *p2;
   Layer source;

   //Copy original layer to allow that source and destination layers be the same
   FX::copy(&source, src);
   
   dst->setSize(src->getWidth(), src->getHeight());
   
   dest = dst->getImageDataPtr();
   sour = source.getImageDataPtr();
   
   width = dst->getWidth();
   height = dst->getHeight();
   
   p = temp = new DWORD[width*height];

   for (i = 0; i < width*height; i++)
      *p++ = 0;

   Random random(seed);
   for (i = 0; i < number; i++) {
      xDest = (int)(width*(random.rand()/(RAND_MAX+1.0f)));
      yDest = (int)(height*(random.rand()/(RAND_MAX+1.0f)));

      p2 = sour;
      for (ySrc = 0; ySrc < height; ySrc++) {
         for (xSrc = 0; xSrc < width; xSrc++) {
            offset = ((yDest+ySrc+height)%height)*width + ((xDest+xSrc+width)%width);

            color1 = *p2++;
            color2 = temp[offset];

            r = (color1&0xFF) + (color2&0xFF);
            if (r > 255) r = 255;

            g = ((color1>>8)&0xFF) + ((color2>>8)&0xFF);
            if (g > 255) g = 255;

            b = ((color1>>16)&0xFF) + ((color2>>16)&0xFF);
            if (b > 255) b = 255;

            temp[offset] = r | (g<<8) | (b<<16);
         }
      }
   }

   p = temp;
   for (i = 0; i < width*height; i++)
      *dest++ = *p++;

   delete []temp;
}

void FX::shade(Layer *dst, Layer *src) {
   int n,r,g,b,rMap,gMap,bMap;
   int width, height;
   DWORD *dest,*sour;
   Layer scaled;

   dest = dst->getImageDataPtr();
   sour = src->getImageDataPtr();
   
   width = dst->getWidth();
   height = dst->getHeight();
   
   if (src->getWidth() != width || src->getHeight() != height) {
      FX::copy(&scaled, src);
      FX::scale(&scaled, width, height);
      sour = scaled.getImageDataPtr();
   }

   for (n = 0; n < width*height; n++) {
      r = *dest&0xFF;
      rMap = *sour&0xFF;
      if (rMap < 128)
         r = (rMap*r)>>7;
      else
         r = r + (2*(rMap-128)*(255-r))/254;

      g = (*dest>>8)&0xFF;
      gMap = (*sour>>8)&0xFF;
      if (gMap < 128)
         g = (gMap*g)>>7;
      else
         g = g + (2*(gMap-128)*(255-g))/254;

      b = (*dest>>16)&0xFF;
      bMap = (*sour++>>16)&0xFF;
      if (bMap < 128)
         b = (bMap*b)>>7;
      else
         b = b + (2*(bMap-128)*(255-b))/254;

      *dest++ = r | (g<<8) | (b<<16);
   }
}

void FX::sub(Layer *dst, Layer *src) {
   int n,r,g,b;
   int width, height;
   DWORD *dest,*sour;
   Layer scaled;

   dest = dst->getImageDataPtr();
   sour = src->getImageDataPtr();
   
   width = dst->getWidth();
   height = dst->getHeight();
   
   if (src->getWidth() != width || src->getHeight() != height) {
      FX::copy(&scaled, src);
      FX::scale(&scaled, width, height);
      sour = scaled.getImageDataPtr();
   }

   for (n = 0; n < width*height; n++) {
      r = (*dest&0xFF) - (*sour&0xFF);
      if (r < 0) r = 0;
      
      g = ((*dest>>8)&0xFF) - ((*sour>>8)&0xFF);
      if (g < 0) g = 0;

      b = ((*dest>>16)&0xFF) - ((*sour++>>16)&0xFF);
      if (b < 0) b = 0;

      *dest++ = r | (g<<8) | (b<<16);
   }
}

void FX::xclor(Layer *dst, Layer *src) {
   int n;
   int width, height;
   DWORD *dest,*sour;
   Layer scaled;

   dest = dst->getImageDataPtr();
   sour = src->getImageDataPtr();
   
   width = dst->getWidth();
   height = dst->getHeight();

   if (src->getWidth() != width || src->getHeight() != height) {
      FX::copy(&scaled, src);
      FX::scale(&scaled, width, height);
      sour = scaled.getImageDataPtr();
   }

   for (n = 0; n < width*height; n++)
      *dest++ ^= *sour++;
}

void FX::textureCurve(Layer *dest, int width, int height, Curve *curve, Layer *texture, Layer *mask, int edgeWidth) {
   Layer temp;
   
   //Create curve
   FX::curve(dest, curve, width, height);
   
   //Save copies of curve
   FX::copy(mask, dest);
   FX::copy(&temp, mask);
   
   //Texture curve
   FX::mul(dest, texture);
   
   if (edgeWidth > 0) {
      //Create contour
      FX::edgeDetect(mask);
      FX::gaussianBlur(mask, edgeWidth);
      FX::threshold(mask, 16);
      FX::gaussianBlur(mask, 2);
      
      //Add contour to textured curve
      FX::add(dest, mask);
   }
   
   //Create mask
   FX::copy(mask, &temp);
   FX::gaussianBlur(mask, edgeWidth);
   FX::threshold(mask, 16);
   FX::gaussianBlur(mask, 2);
}

void FX::cube(Model *model) {
   Vector *vertex;
   Face *face;
   Texel *vertexUV;
   
   vertex = new Vector[8];
   face = new Face[12];
   vertexUV = new Texel[8];
   
   model->setNumberOfVertices(8);
   model->setNumberOfFaces(12);
   
   if (model->getVertex() != NULL)
      delete []model->getVertex();
   if (model->getFace() != NULL)
      delete []model->getFace();
   if (model->getVertexUV() != NULL)
      delete []model->getVertexUV();
   
   model->setVertex(vertex);
   model->setFace(face);
   model->setVertexUV(vertexUV);

   //Generate vertices
   vertex[0].x = -1; vertex[0].y = -1; vertex[0].z = -1;
   vertex[1].x =  1; vertex[1].y = -1; vertex[1].z = -1;
   vertex[2].x = -1; vertex[2].y =  1; vertex[2].z = -1;
   vertex[3].x =  1; vertex[3].y =  1; vertex[3].z = -1;
   vertex[4].x = -1; vertex[4].y = -1; vertex[4].z =  1;
   vertex[5].x =  1; vertex[5].y = -1; vertex[5].z =  1;
   vertex[6].x = -1; vertex[6].y =  1; vertex[6].z =  1;
   vertex[7].x =  1; vertex[7].y =  1; vertex[7].z =  1;
   
   //Generate faces
   face[0].vertex1 = 0; face[0].vertex2 = 3; face[0].vertex3 = 1; face[0].twoSided = false;
   face[1].vertex1 = 0; face[1].vertex2 = 2; face[1].vertex3 = 3; face[1].twoSided = false;

   face[2].vertex1 = 0; face[2].vertex2 = 1; face[2].vertex3 = 4; face[2].twoSided = false;
   face[3].vertex1 = 1; face[3].vertex2 = 5; face[3].vertex3 = 4; face[3].twoSided = false;

   face[4].vertex1 = 4; face[4].vertex2 = 7; face[4].vertex3 = 6; face[4].twoSided = false;
   face[5].vertex1 = 4; face[5].vertex2 = 5; face[5].vertex3 = 7; face[5].twoSided = false;

   face[6].vertex1 = 2; face[6].vertex2 = 6; face[6].vertex3 = 3; face[6].twoSided = false;
   face[7].vertex1 = 3; face[7].vertex2 = 6; face[7].vertex3 = 7; face[7].twoSided = false;

   face[8].vertex1 = 1; face[8].vertex2 = 3; face[8].vertex3 = 7; face[8].twoSided = false;
   face[9].vertex1 = 1; face[9].vertex2 = 7; face[9].vertex3 = 5; face[9].twoSided = false;

   face[10].vertex1 = 0; face[10].vertex2 = 6; face[10].vertex3 = 2; face[10].twoSided = false;
   face[11].vertex1 = 0; face[11].vertex2 = 4; face[11].vertex3 = 6; face[11].twoSided = false;
}

void FX::cylinder(Model *model, int segments) {
   Vector *vertex;
   Face *face;
   Texel *vertexUV;
   int segmentNumber;
   
   vertex = new Vector[2*segments];
   face = new Face[2*segments];
   vertexUV = new Texel[2*segments];
   
   model->setNumberOfVertices(2*segments);
   model->setNumberOfFaces(2*segments);
   
   if (model->getVertex() != NULL)
      delete []model->getVertex();
   if (model->getFace() != NULL)
      delete []model->getFace();
   if (model->getVertexUV() != NULL)
      delete []model->getVertexUV();
   
   model->setVertex(vertex);
   model->setFace(face);
   model->setVertexUV(vertexUV);

   //Generate vertices
   for (segmentNumber = 0; segmentNumber < segments; segmentNumber++) {
      vertex[segmentNumber].x = cos(2*PI*segmentNumber/segments);
      vertex[segmentNumber].y = -1;
      vertex[segmentNumber].z = sin(2*PI*segmentNumber/segments);
   
      vertex[segmentNumber + segments].x = cos(2*PI*segmentNumber/segments);
      vertex[segmentNumber + segments].y = 1;
      vertex[segmentNumber + segments].z = sin(2*PI*segmentNumber/segments);
   }

   //Generate faces
   for (segmentNumber = 0; segmentNumber < segments; segmentNumber++) {
      face[2*segmentNumber + 0].vertex1 = segmentNumber;
      face[2*segmentNumber + 0].vertex2 = segments + segmentNumber;
      face[2*segmentNumber + 0].vertex3 = segments + ((segmentNumber + 1) % segments);
      face[2*segmentNumber + 0].twoSided = true;
   
      face[2*segmentNumber + 1].vertex1 = segmentNumber;
      face[2*segmentNumber + 1].vertex2 = segments + ((segmentNumber + 1) % segments);
      face[2*segmentNumber + 1].vertex3 = (segmentNumber + 1) % segments;
      face[2*segmentNumber + 1].twoSided = true;
   }
}

void FX::heightField(Model *model, Texture *texture, int facets) {
   Vector *vertex;
   Face *face;
   Texel *vertexUV;
   int numberOfVertices, numberOfFaces, vertexNumber, faceNumber, xIndex, zIndex;
   int textureWidth, textureHeight;
   Layer gray;
   
   numberOfVertices = facets*facets;
   numberOfFaces = 2*(facets-1)*(facets-1);
   vertexUV = new Texel[facets*facets];
   
   
   vertex = new Vector[numberOfVertices];
   face = new Face[numberOfFaces];
   
   model->setNumberOfVertices(numberOfVertices);
   model->setNumberOfFaces(numberOfFaces);
   
   if (model->getVertex() != NULL)
      delete []model->getVertex();
   if (model->getFace() != NULL)
      delete []model->getFace();
   if (model->getVertexUV() != NULL)
      delete []model->getVertexUV();
   
   model->setVertex(vertex);
   model->setFace(face);
   model->setVertexUV(vertexUV);

   FX::copy(&gray, texture->getLayer(0));
   FX::grayscale(&gray);
   textureWidth = gray.getWidth();
   textureHeight = gray.getHeight();
   
   //Generate vertices
   vertexNumber = 0;
   for (zIndex = 0; zIndex < facets; zIndex++) {
      for (xIndex = 0; xIndex < facets; xIndex++) {
         vertex[vertexNumber].x = -2 + xIndex*4.0/(facets-1);
         vertex[vertexNumber].y = (bicubicInterpolation(&gray, ((float)textureWidth*xIndex)/facets, ((float)textureHeight*zIndex)/facets)&0xFF)/255.0f;
         vertex[vertexNumber++].z = -2 + zIndex*4.0/(facets-1);
      }
   }

   //Generate faces
   faceNumber = 0;
   for (zIndex = 0; zIndex < facets-1; zIndex++) {
      for (xIndex = 0; xIndex < facets-1; xIndex++) {
         face[faceNumber].vertex1 = zIndex*facets + xIndex;
         face[faceNumber].vertex2 = (zIndex+1)*facets + xIndex+1;
         face[faceNumber].vertex3 = (zIndex+1)*facets + xIndex;
         face[faceNumber++].twoSided = true;
      
         face[faceNumber].vertex1 = zIndex*facets + xIndex+1;
         face[faceNumber].vertex2 = (zIndex+1)*facets + xIndex+1;
         face[faceNumber].vertex3 = zIndex*facets + xIndex;
         face[faceNumber++].twoSided = true;
      }
   }
}

void FX::octahedron(Model *model) {
   Vector *vertex;
   Face *face;
   Texel *vertexUV;
   
   vertex = new Vector[6];
   face = new Face[8];
   vertexUV = new Texel[6];
   
   model->setNumberOfVertices(6);
   model->setNumberOfFaces(8);
   
   if (model->getVertex() != NULL)
      delete []model->getVertex();
   if (model->getFace() != NULL)
      delete []model->getFace();
   if (model->getVertexUV() != NULL)
      delete []model->getVertexUV();
   
   model->setVertex(vertex);
   model->setFace(face);
   model->setVertexUV(vertexUV);

   //Generate vertices
   vertex[0].x = 0; vertex[0].y = 1; vertex[0].z = 0;
   vertex[1].x = 1/SQRT2; vertex[1].y = 0; vertex[1].z = 1/SQRT2;
   vertex[2].x = 1/SQRT2; vertex[2].y = 0; vertex[2].z = -1/SQRT2;
   vertex[3].x = -1/SQRT2; vertex[3].y = 0; vertex[3].z = -1/SQRT2;
   vertex[4].x = -1/SQRT2; vertex[4].y = 0; vertex[4].z = 1/SQRT2;
   vertex[5].x = 0; vertex[5].y = -1; vertex[5].z = 0;
   
   //Generate faces
   face[0].vertex1 = 0; face[0].vertex2 = 1; face[0].vertex3 = 2; face[0].twoSided = false;
   face[1].vertex1 = 0; face[1].vertex2 = 2; face[1].vertex3 = 3; face[1].twoSided = false;

   face[2].vertex1 = 0; face[2].vertex2 = 3; face[2].vertex3 = 4; face[2].twoSided = false;
   face[3].vertex1 = 0; face[3].vertex2 = 4; face[3].vertex3 = 1; face[3].twoSided = false;

   face[4].vertex1 = 5; face[4].vertex2 = 2; face[4].vertex3 = 1; face[4].twoSided = false;
   face[5].vertex1 = 5; face[5].vertex2 = 3; face[5].vertex3 = 2; face[5].twoSided = false;

   face[6].vertex1 = 5; face[6].vertex2 = 4; face[6].vertex3 = 3; face[6].twoSided = false;
   face[7].vertex1 = 5; face[7].vertex2 = 1; face[7].vertex3 = 4; face[7].twoSided = false;
}

void FX::sphere(Model *model, int segments, int rings) {
   Vector *vertex;
   Face *face;
   Texel *vertexUV;
   int numberOfVertices, numberOfFaces, segmentNumber, ringNumber, vertexNumber, faceNumber;
   float theta, phi;
   
   numberOfVertices = (rings-2)*segments + 2;
   numberOfFaces = 2*segments*(rings-3) + 2*segments;
   
   vertex = new Vector[numberOfVertices];
   face = new Face[numberOfFaces];
   vertexUV = new Texel[numberOfVertices];
   
   model->setNumberOfVertices(numberOfVertices);
   model->setNumberOfFaces(numberOfFaces);
   
   if (model->getVertex() != NULL)
      delete []model->getVertex();
   if (model->getFace() != NULL)
      delete []model->getFace();
   if (model->getVertexUV() != NULL)
      delete []model->getVertexUV();
   
   model->setVertex(vertex);
   model->setFace(face);
   model->setVertexUV(vertexUV);

   //Generate vertices
   vertexNumber = 0;
   vertex[vertexNumber].x = 0;
   vertex[vertexNumber].y = 1;
   vertex[vertexNumber++].z = 0;
   
   for (ringNumber = 1; ringNumber < rings-1; ringNumber++) {
      theta = (ringNumber*PI)/(rings-1);
      for (segmentNumber = 0; segmentNumber < segments; segmentNumber++) {
         phi = (segmentNumber*2*PI)/segments;
         vertex[vertexNumber].x = sin(theta)*cos(phi);
         vertex[vertexNumber].y = cos(theta);
         vertex[vertexNumber++].z = sin(theta)*sin(phi);
      }
   }
   
   vertex[vertexNumber].x = 0;
   vertex[vertexNumber].y = -1;
   vertex[vertexNumber++].z = 0;

   //Generate faces
   faceNumber = 0;
   
   for (segmentNumber = 0; segmentNumber < segments-1; segmentNumber++) {
      face[faceNumber].vertex1 = 0;
      face[faceNumber].vertex2 = segmentNumber+2;
      face[faceNumber].vertex3 = segmentNumber+1;
      face[faceNumber++].twoSided = false;
   }
   face[faceNumber].vertex1 = 0;
   face[faceNumber].vertex2 = 1;
   face[faceNumber].vertex3 = segments;
   face[faceNumber++].twoSided = false;
   
   for (ringNumber = 1; ringNumber < rings-2; ringNumber++) {
      for (segmentNumber = 1; segmentNumber < segments; segmentNumber++) {
         face[faceNumber].vertex1 = (ringNumber-1)*segments + segmentNumber;
         face[faceNumber].vertex2 = (ringNumber-1)*segments + segmentNumber+1;
         face[faceNumber].vertex3 = ringNumber*segments + segmentNumber;
         face[faceNumber++].twoSided = false;
      
         face[faceNumber].vertex1 = (ringNumber-1)*segments + segmentNumber+1;
         face[faceNumber].vertex2 = ringNumber*segments + segmentNumber+1;
         face[faceNumber].vertex3 = ringNumber*segments + segmentNumber;
         face[faceNumber++].twoSided = false;
      }
      face[faceNumber].vertex1 = (ringNumber-1)*segments + segments;
      face[faceNumber].vertex2 = (ringNumber-1)*segments + 1;
      face[faceNumber].vertex3 = ringNumber*segments + segments;
      face[faceNumber++].twoSided = false;
   
      face[faceNumber].vertex1 = (ringNumber-1)*segments + 1;
      face[faceNumber].vertex2 = ringNumber*segments + 1;
      face[faceNumber].vertex3 = ringNumber*segments + segments;
      face[faceNumber++].twoSided = false;
   }

   for (segmentNumber = 1; segmentNumber < segments; segmentNumber++) {
      face[faceNumber].vertex1 = (rings-2)*segments + 1;
      face[faceNumber].vertex2 = (rings-3)*segments + segmentNumber;
      face[faceNumber].vertex3 = (rings-3)*segments + segmentNumber+1;
      face[faceNumber++].twoSided = false;
   }
   face[faceNumber].vertex1 = (rings-2)*segments + 1;
   face[faceNumber].vertex2 = (rings-2)*segments;
   face[faceNumber].vertex3 = (rings-3)*segments + 1;
   face[faceNumber++].twoSided = false;
}

void FX::star(Model *model, int horizontalTips, int verticalTips, float minRadius, float maxRadius, int segments, int rings) {
   Vector *vertex;
   Face *face;
   Texel *vertexUV;
   int numberOfVertices, numberOfFaces, segmentNumber, ringNumber, vertexNumber, faceNumber;
   float theta, phi;
   
   numberOfVertices = (rings-2)*segments + 2;
   numberOfFaces = 2*segments*(rings-3) + 2*segments;
   
   vertex = new Vector[numberOfVertices];
   face = new Face[numberOfFaces];
   vertexUV = new Texel[numberOfVertices];
   
   model->setNumberOfVertices(numberOfVertices);
   model->setNumberOfFaces(numberOfFaces);
   
   if (model->getVertex() != NULL)
      delete []model->getVertex();
   if (model->getFace() != NULL)
      delete []model->getFace();
   if (model->getVertexUV() != NULL)
      delete []model->getVertexUV();
   
   model->setVertex(vertex);
   model->setFace(face);
   model->setVertexUV(vertexUV);

   //Generate vertices
   vertexNumber = 0;
   vertex[vertexNumber].x = 0;
   vertex[vertexNumber].y = minRadius;
   vertex[vertexNumber++].z = 0;
   
   for (ringNumber = 1; ringNumber < rings-1; ringNumber++) {
      theta = (ringNumber*PI)/(rings-1);
      for (segmentNumber = 0; segmentNumber < segments; segmentNumber++) {
         phi = (segmentNumber*2*PI)/segments;
         vertex[vertexNumber].x = (minRadius + maxRadius*sin(theta*verticalTips)*cos(phi*horizontalTips)) * sin(theta)*cos(phi);
         vertex[vertexNumber].y = (minRadius + maxRadius*sin(theta*verticalTips)*cos(phi*horizontalTips)) * cos(theta);
         vertex[vertexNumber++].z = (minRadius + maxRadius*sin(theta*verticalTips)*cos(phi*horizontalTips)) * sin(theta)*sin(phi);
      }
   }
   
   vertex[vertexNumber].x = 0;
   vertex[vertexNumber].y = -minRadius;
   vertex[vertexNumber++].z = 0;

   //Generate faces
   faceNumber = 0;
   
   for (segmentNumber = 0; segmentNumber < segments-1; segmentNumber++) {
      face[faceNumber].vertex1 = 0;
      face[faceNumber].vertex2 = segmentNumber+2;
      face[faceNumber].vertex3 = segmentNumber+1;
      face[faceNumber++].twoSided = false;
   }
   face[faceNumber].vertex1 = 0;
   face[faceNumber].vertex2 = 1;
   face[faceNumber].vertex3 = segments;
   face[faceNumber++].twoSided = false;
   
   for (ringNumber = 1; ringNumber < rings-2; ringNumber++) {
      for (segmentNumber = 1; segmentNumber < segments; segmentNumber++) {
         face[faceNumber].vertex1 = (ringNumber-1)*segments + segmentNumber;
         face[faceNumber].vertex2 = (ringNumber-1)*segments + segmentNumber+1;
         face[faceNumber].vertex3 = ringNumber*segments + segmentNumber;
         face[faceNumber++].twoSided = false;
      
         face[faceNumber].vertex1 = (ringNumber-1)*segments + segmentNumber+1;
         face[faceNumber].vertex2 = ringNumber*segments + segmentNumber+1;
         face[faceNumber].vertex3 = ringNumber*segments + segmentNumber;
         face[faceNumber++].twoSided = false;
      }
      face[faceNumber].vertex1 = (ringNumber-1)*segments + segments;
      face[faceNumber].vertex2 = (ringNumber-1)*segments + 1;
      face[faceNumber].vertex3 = ringNumber*segments + segments;
      face[faceNumber++].twoSided = false;
   
      face[faceNumber].vertex1 = (ringNumber-1)*segments + 1;
      face[faceNumber].vertex2 = ringNumber*segments + 1;
      face[faceNumber].vertex3 = ringNumber*segments + segments;
      face[faceNumber++].twoSided = false;
   }

   for (segmentNumber = 1; segmentNumber < segments; segmentNumber++) {
      face[faceNumber].vertex1 = (rings-2)*segments + 1;
      face[faceNumber].vertex2 = (rings-3)*segments + segmentNumber;
      face[faceNumber].vertex3 = (rings-3)*segments + segmentNumber+1;
      face[faceNumber++].twoSided = false;
   }
   face[faceNumber].vertex1 = (rings-2)*segments + 1;
   face[faceNumber].vertex2 = (rings-2)*segments;
   face[faceNumber].vertex3 = (rings-3)*segments + 1;
   face[faceNumber++].twoSided = false;
}

void FX::torus(Model *model, float radius1, float radius2, int segments, int rings) {
   Vector *vertex;
   Face *face;
   Texel *vertexUV;
   int numberOfVertices, numberOfFaces, segmentNumber, ringNumber, vertexNumber, faceNumber;
   float theta, phi;
   Vector ringCenter;
   
   numberOfVertices = segments*rings;
   numberOfFaces = 2*segments*rings;
   
   vertex = new Vector[numberOfVertices];
   face = new Face[numberOfFaces];
   vertexUV = new Texel[numberOfVertices];
   
   model->setNumberOfVertices(numberOfVertices);
   model->setNumberOfFaces(numberOfFaces);
   
   if (model->getVertex() != NULL)
      delete []model->getVertex();
   if (model->getFace() != NULL)
      delete []model->getFace();
   if (model->getVertexUV() != NULL)
      delete []model->getVertexUV();
   
   model->setVertex(vertex);
   model->setFace(face);
   model->setVertexUV(vertexUV);

   //Generate vertices
   vertexNumber = 0;
   for (ringNumber = 0; ringNumber < rings; ringNumber++) {
      phi = (ringNumber*2*PI)/rings;
      ringCenter.x = radius1*cos(phi);
      ringCenter.y = 0;
      ringCenter.z = radius1*sin(phi);
      for (segmentNumber = 0; segmentNumber < segments; segmentNumber++) {
         theta = (segmentNumber*2*PI)/segments;
         vertex[vertexNumber].x = ringCenter.x + radius2*sin(theta)*cos(phi);
         vertex[vertexNumber].y = ringCenter.y + radius2*cos(theta);
         vertex[vertexNumber++].z = ringCenter.z + radius2*sin(theta)*sin(phi);
      }
   }
   
   //Generate faces
   faceNumber = 0;
   for (ringNumber = 0; ringNumber < rings; ringNumber++) {
      for (segmentNumber = 0; segmentNumber < segments; segmentNumber++) {
         face[faceNumber].vertex1 = segments*ringNumber + segmentNumber;
         face[faceNumber].vertex2 = segments*ringNumber + ((segmentNumber + segments-1)%segments);
         face[faceNumber].vertex3 = segments*((ringNumber+1)%rings) + ((segmentNumber + segments-1)%segments);
         face[faceNumber++].twoSided = false;
         
         face[faceNumber].vertex1 = segments*ringNumber + segmentNumber;
         face[faceNumber].vertex2 = segments*((ringNumber+1)%rings) + ((segmentNumber + segments-1)%segments);
         face[faceNumber].vertex3 = segments*((ringNumber+1)%rings) + segmentNumber;
         face[faceNumber++].twoSided = false;
      }
   }
}

void FX::heightMap(Model *model, int type, float value, Texture *texture) {
   Vector *vertex;
   int vertexNumber, numberOfVertices, textureWidth, textureHeight;
   float uIndex, vIndex;
   Layer gray;
   Vector min, max, delta, center;
   
   vertex = model->getVertex();
   numberOfVertices = model->getNumberOfVertices();
   
   FX::copy(&gray, texture->getLayer(0));
   FX::grayscale(&gray);
   textureWidth = gray.getWidth();
   textureHeight = gray.getHeight();
   
   min = model->getMin();
   max = model->getMax();
   delta = max-min;
   center = model->getCenter();
   
   switch (type) {
   case MAP_XY :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         uIndex = textureWidth * (vertex[vertexNumber].x - min.x) / delta.x;
         vIndex = textureHeight * (vertex[vertexNumber].y - min.y) / delta.y;
         vertex[vertexNumber].z += value * (((int)bicubicInterpolation(&gray, uIndex, vIndex)&0xFF)-128)/128.0f;
      }
      break;
   case MAP_XZ :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         uIndex = textureWidth * (vertex[vertexNumber].x - min.x) / delta.x;
         vIndex = textureHeight * (vertex[vertexNumber].z - min.z) / delta.z;
         vertex[vertexNumber].y += value * (((int)bicubicInterpolation(&gray, uIndex, vIndex)&0xFF)-128)/128.0f;
      }
      break;
   case MAP_YZ :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         uIndex = textureWidth * (vertex[vertexNumber].y - min.y) / delta.y;
         vIndex = textureHeight * (vertex[vertexNumber].z - min.z) / delta.z;
         vertex[vertexNumber].x += value * (((int)bicubicInterpolation(&gray, uIndex, vIndex)&0xFF)-128)/128.0f;
      }
      break;
   case CYLINDRIC :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         delta = vertex[vertexNumber] - center;
         delta.y = 0;
         delta.normalize();
         uIndex = textureWidth * (PI+atan2(delta.x, delta.z))/(2*PI);
         vIndex = textureHeight * (vertex[vertexNumber].y - min.y) / (max.y - min.y);
         vertex[vertexNumber] = vertex[vertexNumber] + delta*(value*(((int)bicubicInterpolation(&gray, uIndex, vIndex)&0xFF)-128)/128.0f);
      }      
      break;
   case SPHERIC :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         delta = vertex[vertexNumber] - center;
         delta.normalize();
         uIndex = textureWidth * (PI+atan2(delta.x, delta.z))/(2*PI);
         vIndex = textureHeight * acos(-delta.y)/PI;
         vertex[vertexNumber] = vertex[vertexNumber] + (delta*(value*(((int)bicubicInterpolation(&gray, uIndex, vIndex)&0xFF)-128)/255.0f));
      }      
      break;
   }
}

void FX::moveObject(Model *model, Vector delta) {
   int vertexNumber;
   Vector *vertex;
   
   vertex = model->getVertex();
   for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
      vertex[vertexNumber] = vertex[vertexNumber] + delta;
   }
}

void FX::noise(Model *model, float value, int seed) {
   int vertexNumber;
   Vector *vertex, center, delta;
   
   vertex = model->getVertex();
   center = model->getCenter();
   
   Random random(seed);

   for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
      vertex[vertexNumber].x += 2*value*((((float)random.rand())/RAND_MAX) - 0.5f);
      vertex[vertexNumber].y += 2*value*((((float)random.rand())/RAND_MAX) - 0.5f);
      vertex[vertexNumber].z += 2*value*((((float)random.rand())/RAND_MAX) - 0.5f);
   }
}

void FX::refine(Model *model) {
   //TODO
}

void FX::rotateObject(Model *model, int direction, float value) {
   int vertexNumber;
   Vector *vertex, center, delta;
   float angle;
   
   vertex = model->getVertex();
   center = model->getCenter();
   
   angle = value*PI/180;
   
   if (direction == X_DIRECTION) {
      for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
         delta = vertex[vertexNumber] - center;
         vertex[vertexNumber].y = center.y + delta.y*cos(angle) + delta.z*sin(angle);
         vertex[vertexNumber].z = center.z - delta.y*sin(angle) + delta.z*cos(angle);
      }
   }
   else if (direction == Y_DIRECTION) {
      for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
         delta = vertex[vertexNumber] - center;
         vertex[vertexNumber].x = center.x + delta.x*cos(angle) + delta.z*sin(angle);
         vertex[vertexNumber].z = center.z - delta.x*sin(angle) + delta.z*cos(angle);
      }
   }
   else if (direction == Z_DIRECTION) {
      for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
         delta = vertex[vertexNumber] - center;
         vertex[vertexNumber].x = center.x + delta.x*cos(angle) + delta.y*sin(angle);
         vertex[vertexNumber].y = center.y - delta.x*sin(angle) + delta.y*cos(angle);
      }
   }
}

void FX::scaleObject(Model *model, Vector delta) {
   int vertexNumber;
   Vector *vertex, center;
   
   vertex = model->getVertex();
   center = model->getCenter();
   
   for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
      vertex[vertexNumber].x = center.x + (vertex[vertexNumber].x - center.x) * delta.x;
      vertex[vertexNumber].y = center.y + (vertex[vertexNumber].y - center.y) * delta.y;
      vertex[vertexNumber].z = center.z + (vertex[vertexNumber].z - center.z) * delta.z;
   }
}

void FX::spherize(Model *model, float value) {
   int vertexNumber;
   Vector *vertex, center, delta;
   
   vertex = model->getVertex();
   center = model->getCenter();
   
   for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
      delta = vertex[vertexNumber] - center;
      delta.normalize();
      vertex[vertexNumber] = vertex[vertexNumber] + delta*value;
   }
}

void FX::taper(Model *model, int direction, float value) {
   int vertexNumber;
   Vector *vertex, center;
   float delta;
   
   vertex = model->getVertex();
   center = model->getCenter();
   
   if (direction == X_DIRECTION) {
      for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
         delta = vertex[vertexNumber].x - center.x;
         vertex[vertexNumber].y += (vertex[vertexNumber].y - center.y)*delta*value;
         vertex[vertexNumber].z += (vertex[vertexNumber].z - center.z)*delta*value;
      }
   }
   else if (direction == Y_DIRECTION) {
      for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
         delta = vertex[vertexNumber].y - center.y;
         vertex[vertexNumber].x += (vertex[vertexNumber].x - center.x)*delta*value;
         vertex[vertexNumber].z += (vertex[vertexNumber].z - center.z)*delta*value;
      }
   }
   else if (direction == Z_DIRECTION) {
      for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
         delta = vertex[vertexNumber].z - center.z;
         vertex[vertexNumber].x += (vertex[vertexNumber].x - center.x)*delta*value;
         vertex[vertexNumber].y += (vertex[vertexNumber].y - center.y)*delta*value;
      }
   }
}

void FX::tessellate(Model *model) {
   //TODO
/*   int faceNumber, numberOfFaces;
   
   numberOfFaces = model->getNumberOfFaces();
   
   for (faceNumber = 0; faceNumber < numberOfFaces; faceNumber++) {
      face[faceNumber].vertex1;
      face[faceNumber].vertex2;
      face[faceNumber].vertex3;
   }*/
}

void FX::twistObject(Model *model, int direction, float value) {
   int vertexNumber;
   Vector *vertex, center, max;
   float angle, delta, x, y, z;
   
   vertex = model->getVertex();
   center = model->getCenter();
   max = model->getMax();
   
   angle = value*PI/180;
   
   if (direction == X_DIRECTION) {
      for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
         x = vertex[vertexNumber].x;
         y = vertex[vertexNumber].y;
         z = vertex[vertexNumber].z;
         delta = (x - center.x) / (max.x - center.x);
         vertex[vertexNumber].y = y*cos(angle*delta) + z*sin(angle*delta);
         vertex[vertexNumber].z = -y*sin(angle*delta) + z*cos(angle*delta);
      }
   }
   else if (direction == Y_DIRECTION) {
      for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
         x = vertex[vertexNumber].x;
         y = vertex[vertexNumber].y;
         z = vertex[vertexNumber].z;
         delta = (y - center.y) / (max.y - center.y);
         vertex[vertexNumber].x = x*cos(angle*delta) + z*sin(angle*delta);
         vertex[vertexNumber].z = -x*sin(angle*delta) + z*cos(angle*delta);
      }
   }
   else if (direction == Z_DIRECTION) {
      for (vertexNumber = 0; vertexNumber < model->getNumberOfVertices(); vertexNumber++) {
         x = vertex[vertexNumber].x;
         y = vertex[vertexNumber].y;
         z = vertex[vertexNumber].z;
         delta = (z - center.z) / (max.z - center.z);
         vertex[vertexNumber].x = x*cos(angle*delta) + y*sin(angle*delta);
         vertex[vertexNumber].y = -x*sin(angle*delta) + y*cos(angle*delta);
      }
   }
}

void FX::textureMap(Model *model, int type, Texture *texture) {
   Vector *vertex;
   Texel *vertexUV;
   int numberOfVertices, vertexNumber;
   Vector min, max, delta, center;
   
   vertex = model->getVertex();
   numberOfVertices = model->getNumberOfVertices();
   
   vertexUV = new Texel[numberOfVertices];
   
   if (model->getVertexUV() != NULL)
      delete []model->getVertexUV();
   
   model->setVertexUV(vertexUV);
   model->setTextureMap(texture);
   model->setTextureMapped(true);

   min = model->getMin();
   max = model->getMax();
   delta = max-min;
   delta.x += 0.1;
   delta.y += 0.1;
   delta.z += 0.1;
   center = model->getCenter();
   
   //Generate texture coordinates
   switch (type) {
   case MAP_XY :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         vertexUV[vertexNumber].u = (vertex[vertexNumber].x - min.x + 0.05) / delta.x;
         vertexUV[vertexNumber].v = (vertex[vertexNumber].y - min.y + 0.05) / delta.y;
      }
      break;
   case MAP_XZ :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         vertexUV[vertexNumber].u = (vertex[vertexNumber].x - min.x + 0.05) / delta.x;
         vertexUV[vertexNumber].v = (vertex[vertexNumber].z - min.z + 0.05) / delta.z;
      }
      break;
   case MAP_YZ :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         vertexUV[vertexNumber].u = (vertex[vertexNumber].y - min.y + 0.05) / delta.y;
         vertexUV[vertexNumber].v = (vertex[vertexNumber].z - min.z + 0.05) / delta.z;
      }
      break;
   case CYLINDRIC :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         delta = vertex[vertexNumber] - center;
         delta.y = 0;
         delta.normalize();
         vertexUV[vertexNumber].u = (PI+atan2(delta.x, delta.z))/(2*PI);
         vertexUV[vertexNumber].v = (vertex[vertexNumber].y - min.y + 0.05) / (max.y - min.y + 0.1);
         
      }      
      break;
   case SPHERIC :
      for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++) {
         delta = vertex[vertexNumber] - center;
         delta.normalize();
         vertexUV[vertexNumber].u = (PI+atan2(delta.x, delta.z))/(2*PI);
         vertexUV[vertexNumber].v = acos(-delta.y)/PI;
      }      
      break;
   }
}

void FX::environmentMap(Model *model, Texture *texture) {
   model->setEnvironmentMap(texture);
   model->setEnvironmentMapped(true);
}
