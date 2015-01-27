#include "model.h"

#include <math.h>

#include "fx.h"

#ifdef __DEBUG
#include <stdio.h>
#endif //__DEBUG

Model::Model() {
   genera = NULL;
   data = NULL;
   
   numberOfVertices = 0;
   vertex = NULL;
   vertexNormal = NULL;
   vertexUV = NULL;

   numberOfFaces = 0;
   face = NULL;
   faceNormal = NULL;
   
   textureMap = NULL;
   environmentMap = NULL;
   
   textureMapped = false;
   environmentMapped = false;
   
   render = NULL;
}

Model::~Model() {
   if (data != NULL)
      delete []data;
   
   if (vertex != NULL)
      delete []vertex;
   if (vertexNormal != NULL)
      delete []vertexNormal;
   if (vertexUV != NULL)
      delete []vertexUV;

   if (face != NULL)
      delete []face;
   if (faceNormal != NULL)
      delete []faceNormal;
}

void Model::setGenerator(Genera *genera) {
   this->genera = genera;
}

BYTE *Model::loadData(BYTE *data) {
   Component::loadData(data);
   
   if (this->data != NULL)
      delete []this->data;

   //Pointer to model data
   BYTE *ptr = data;

   //Load model name
   ptr = readString(name, data);

#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::loadData() - Model name: %s\n", name);
#endif //__DEBUG
   
   //Load model data size
   int size = (((int)ptr[0])<<16) + (((int)ptr[1])<<8) + ((int)ptr[2]);
   
   ptr += 3;

#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::loadData() - Model data size: %d\n", size);
#endif //__DEBUG
   
   this->data = new BYTE[size];

   for (int i = 0; i < size; i++, ptr++)
      this->data[i] = *ptr;

   return ptr;
}

void Model::generate() {
   BYTE value;
   bool fxOn;
   BYTE *ptr = data;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::generate()\n");
#endif //__DEBUG
   
   int numberOfFx = *ptr++;
   
   for (int i = 0; i < numberOfFx; i++) {
      //Get active and FX number
      value = *ptr++;
      
#ifdef __DEBUG
      fprintf(stderr, "LOG: Model::generate() - Component %d; value: %d\n", i, value & MODEL_FX_TYPE);
#endif //__DEBUG
   
      fxOn = (value & MODEL_FX_ON) != 0;

      //TODO: Se puede hacer una lista de funciones y llamarla directamente sin necesidad de un switch
      //ocupa menos y es mas intuitivo
      switch ((ModelFXID)value & MODEL_FX_TYPE) {
         //Generate model from loaded data
         case CUBE:
            ptr = cube(ptr, fxOn);
            break;
         case CYLINDER:
            ptr = cylinder(ptr, fxOn);
            break;
         /*case EXTRUDE_CURVE:
            ptr = extrudeCurve(ptr, fxOn);
            break;*/
         case HEIGHT_FIELD:
            ptr = heightField(ptr, fxOn);
            break;
         /*case METABALL:
            ptr = metaball(ptr, fxOn);
            break;*/
         case OCTAHEDRON:
            ptr = octahedron(ptr, fxOn);
            break;
         case SPHERE:
            ptr = sphere(ptr, fxOn);
            break;
         case STAR:
            ptr = star(ptr, fxOn);
            break;
         /*case SWEEP_CURVE:
            ptr = sweepCurve(ptr, fxOn);
            break;*/
         case TORUS:
            ptr = torus(ptr, fxOn);
            break;

         case HEIGHT_MAP:
            ptr = heightMap(ptr, fxOn);
            break;
         case MOVE_OBJECT:
            ptr = moveObject(ptr, fxOn);
            break;
         case NOISE_OBJECT:
            ptr = noise(ptr, fxOn);
            break;
         case REFINE:
            ptr = refine(ptr, fxOn);
            break;
         case ROTATE_OBJECT:
            ptr = rotateObject(ptr, fxOn);
            break;
         case SCALE_OBJECT:
            ptr = scaleObject(ptr, fxOn);
            break;
         case SPHERIZE:
            ptr = spherize(ptr, fxOn);
            break;
         case TAPER:
            ptr = taper(ptr, fxOn);
            break;
         case TESSELLATE:
            ptr = tessellate(ptr, fxOn);
            break;
         case TWIST_OBJECT:
            ptr = twistObject(ptr, fxOn);
            break;

         case ENVIRONMENT_MAP:
            ptr = environment(ptr, fxOn);
            break;
         case TEXTURE_MAP:
            ptr = texture(ptr, fxOn);
            break;
         
         default:
            break;
      }
      precalculate();
   }
}

void Model::precalculate() {
   int faceNumber, vertexNumber;
   int vertex1, vertex2, vertex3;
   Vector vector1, vector2;

   if (vertexNormal != NULL)
      delete []vertexNormal;
   if (faceNormal != NULL)
      delete []faceNormal;
   
   vertexNormal = new Vector[numberOfVertices];
   faceNormal = new Vector[numberOfFaces];

   //Calculate minimum, maximum and center
   min = max = vertex[0];
   for (vertexNumber = 1; vertexNumber < numberOfVertices; vertexNumber++) {
      if (vertex[vertexNumber].x < min.x)
         min.x = vertex[vertexNumber].x;
      if (vertex[vertexNumber].x > max.x)
         max.x = vertex[vertexNumber].x;
      if (vertex[vertexNumber].y < min.y)
         min.y = vertex[vertexNumber].y;
      if (vertex[vertexNumber].y > max.y)
         max.y = vertex[vertexNumber].y;
      if (vertex[vertexNumber].z < min.z)
         min.z = vertex[vertexNumber].z;
      if (vertex[vertexNumber].z > max.z)
         max.z = vertex[vertexNumber].z;
   }
   center.x = (min.x + max.x)/2;
   center.y = (min.y + max.y)/2;
   center.z = (min.z + max.z)/2;
   
   //Precalculate face normals
   for (faceNumber = 0; faceNumber < numberOfFaces; faceNumber++) {
      vertex1 = face[faceNumber].vertex1;
      vertex2 = face[faceNumber].vertex2;
      vertex3 = face[faceNumber].vertex3;
      
      vector1 = vertex[vertex2] - vertex[vertex1];
      vector2 = vertex[vertex3] - vertex[vertex1];

      faceNormal[faceNumber] = vector1*vector2;
      faceNormal[faceNumber].normalize();
   }

   //Precalculate vertex normals
   for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++)
      vertexNormal[vertexNumber].x = vertexNormal[vertexNumber].y = vertexNormal[vertexNumber].z = 0;

   for (faceNumber = 0; faceNumber < numberOfFaces; faceNumber++) {
      vertex1 = face[faceNumber].vertex1;
      vertex2 = face[faceNumber].vertex2;
      vertex3 = face[faceNumber].vertex3;

      vertexNormal[vertex1] = faceNormal[faceNumber] + vertexNormal[vertex1];
      vertexNormal[vertex2] = faceNormal[faceNumber] + vertexNormal[vertex2];
      vertexNormal[vertex3] = faceNormal[faceNumber] + vertexNormal[vertex3];
   }

   for (vertexNumber = 0; vertexNumber < numberOfVertices; vertexNumber++)
      vertexNormal[vertexNumber].normalize();
}

//////////////////////////////////////////////////////////////////////////

float readFloat(void *ptr) {
   return *(float *)ptr;
}

BYTE *Model::cube(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::cube() - CUBE\n");
#endif //__DEBUG
            
   if (fxOn)
      FX::cube(this);

   return ptr;
}

BYTE *Model::cylinder(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::cylinder() - CYLINDER\n");
#endif //__DEBUG
            
   //Read parameters
   int segments = *ptr++;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::cylinder() - segments: %d\n", segments);
#endif //__DEBUG
   
   if (fxOn)
      FX::cylinder(this, segments);

   return ptr;
}

BYTE *Model::heightField(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::heightField() - HEIGHT FIELD\n");
#endif //__DEBUG
            
   //Read parameters
   char textureName[256];
   ptr = readString(textureName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::heightField() - textureName: %s\n", textureName);
#endif //__DEBUG

   Texture *texture = (Texture *)genera->getComponent(textureName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::heightField() - texture: %p\n", texture);
#endif //__DEBUG
   
   int facets = *ptr++;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::heightField() - facets: %d\n", facets);
#endif //__DEBUG
   
   if (fxOn && texture)
      FX::heightField(this, texture, facets);

   return ptr;
}

BYTE *Model::octahedron(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::octahedron() - OCTAHEDRON\n");
#endif //__DEBUG
            
   if (fxOn)
      FX::octahedron(this);

   return ptr;
}

BYTE *Model::sphere(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::sphere() - SPHERE\n");
#endif //__DEBUG
            
   //Read parameters
   int segments = *ptr++;
   int rings = *ptr++;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::sphere() - segments: %d; rings: %d\n", segments, rings);
#endif //__DEBUG
   
   if (fxOn)
      FX::sphere(this, segments, rings);

   return ptr;
}

BYTE *Model::star(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::star() - STAR\n");
#endif //__DEBUG
            
   //Read parameters
   int horizontalTips = *ptr++;
   int verticalTips = *ptr++;
   float minRadius = readFloat(ptr);
   ptr += 4;
   float maxRadius = readFloat(ptr);
   ptr += 4;
   int segments = *ptr++;
   int rings = *ptr++;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::star() - horizontalTips: %d; verticalTips: %d; minRadius: %f; maxRadius: %f; segments: %d; rings: %d\n", horizontalTips, verticalTips, minRadius, maxRadius, segments, rings);
#endif //__DEBUG
   
   if (fxOn)
      FX::star(this, horizontalTips, verticalTips, minRadius, maxRadius, segments, rings);

   return ptr;
}

BYTE *Model::torus(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::torus() - TORUS\n");
#endif //__DEBUG
            
   //Read parameters
   float radius1 = readFloat(ptr);
   ptr += 4;
   float radius2 = readFloat(ptr);
   ptr += 4;
   int segments = *ptr++;
   int rings = *ptr++;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::torus() - radius1: %f; radius2: %f; segments: %d; rings: %d\n", radius1, radius2, segments, rings);
#endif //__DEBUG
   
   if (fxOn)
      FX::torus(this, radius1, radius2, segments, rings);

   return ptr;
}

BYTE *Model::heightMap(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::heightMap() - HEIGHT MAP\n");
#endif //__DEBUG
            
   //Read parameters
   int type = *ptr++;
   float value = readFloat(ptr);
   ptr += 4;
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::heightMap() - type: %d; value: %f\n", type, value);
#endif //__DEBUG
   
   char textureName[256];
   ptr = readString(textureName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::heightMap() - textureName: %s\n", textureName);
#endif //__DEBUG

   Texture *texture = (Texture *)genera->getComponent(textureName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::heightMap() - texture: %p\n", texture);
#endif //__DEBUG
   
   if (fxOn && texture)
      FX::heightMap(this, type, value, texture);

   return ptr;
}

BYTE *Model::moveObject(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::moveObject() - MOVE OBJECT\n");
#endif //__DEBUG
            
   //Read parameters
   float x = readFloat(ptr);
   ptr += 4;
   float y = readFloat(ptr);
   ptr += 4;
   float z = readFloat(ptr);
   ptr += 4;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::moveObject() - X: %f; Y: %f; Z: %f\n", x, y, z);
#endif //__DEBUG
   
   if (fxOn)
      FX::moveObject(this, Vector(x, y, z));

   return ptr;
}

BYTE *Model::noise(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::noise() - NOISE\n");
#endif //__DEBUG
            
   //Read parameters
   float value = readFloat(ptr);
   ptr += 4;
   int seed = *ptr++;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::noise() - value: %f; seed: %d\n", value, seed);
#endif //__DEBUG
   
   if (fxOn)
      FX::noise(this, value, seed);

   return ptr;
}

BYTE *Model::refine(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::refine() - REFINE\n");
#endif //__DEBUG
            
   if (fxOn)
      FX::refine(this);

   return ptr;
}

BYTE *Model::rotateObject(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::rotateObject() - ROTATE OBJECT\n");
#endif //__DEBUG
            
   //Read parameters
   int direction = *ptr++;
   float value = readFloat(ptr);
   ptr += 4;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::rotateObject() - direction: %d; value: %f\n", direction, value);
#endif //__DEBUG
   
   if (fxOn)
      FX::rotateObject(this, direction, value);

   return ptr;
}

BYTE *Model::scaleObject(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::scaleObject() - SCALE OBJECT\n");
#endif //__DEBUG
            
   //Read parameters
   float x = readFloat(ptr);
   ptr += 4;
   float y = readFloat(ptr);
   ptr += 4;
   float z = readFloat(ptr);
   ptr += 4;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::scaleObject() - X: %f; Y: %f; Z: %f\n", x, y, z);
#endif //__DEBUG
   
   if (fxOn)
      FX::scaleObject(this, Vector(x, y, z));

   return ptr;
}

BYTE *Model::spherize(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::spherize() - SPHERIZE\n");
#endif //__DEBUG
            
   //Read parameters
   float value = readFloat(ptr);
   ptr += 4;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::spherize() - value: %f\n", value);
#endif //__DEBUG
   
   if (fxOn)
      FX::spherize(this, value);

   return ptr;
}

BYTE *Model::taper(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::taper() - TAPER\n");
#endif //__DEBUG
            
   //Read parameters
   int direction = *ptr++;
   float value = readFloat(ptr);
   ptr += 4;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::taper() - direction: %d; value: %f\n", direction, value);
#endif //__DEBUG
   
   if (fxOn)
      FX::taper(this, direction, value);

   return ptr;
}

BYTE *Model::tessellate(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::tessellate() - TESSELLATE\n");
#endif //__DEBUG
            
   if (fxOn)
      FX::tessellate(this);

   return ptr;
}

BYTE *Model::twistObject(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::twistObject() - TWIST OBJECT\n");
#endif //__DEBUG
            
   //Read parameters
   int direction = *ptr++;
   float value = readFloat(ptr);
   ptr += 4;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::twistObject() - direction: %d; value: %f\n", direction, value);
#endif //__DEBUG
   
   if (fxOn)
      FX::twistObject(this, direction, value);

   return ptr;
}

BYTE *Model::texture(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::texture() - TEXTURE\n");
#endif //__DEBUG
            
   //Read parameters
   int type = *ptr++;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::texture() - type: %d\n", type);
#endif //__DEBUG
   
   char textureName[256];
   ptr = readString(textureName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::texture() - textureName: %s\n", textureName);
#endif //__DEBUG

   Texture *texture = (Texture *)genera->getComponent(textureName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::texture() - texture: %p\n", texture);
#endif //__DEBUG
   
   if (fxOn && texture)
      FX::textureMap(this, type, texture);
   
   return ptr;
}

BYTE *Model::environment(BYTE *ptr, bool fxOn) {
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::environment() - ENVIRONMENT\n");
#endif //__DEBUG
            
   //Read parameters
   char textureName[256];
   ptr = readString(textureName, ptr);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::environment() - textureName: %s\n", textureName);
#endif //__DEBUG

   Texture *texture = (Texture *)genera->getComponent(textureName);
#ifdef __DEBUG
   fprintf(stderr, "LOG: Model::environment() - texture: %p\n", texture);
#endif //__DEBUG
   
   if (fxOn && texture)
      FX::environmentMap(this, texture);
   
   return ptr;
}

DWORD *Model::draw(float xPosition, float yPosition, float zPosition, float xTarget, float yTarget, float zTarget, float xUp, float yUp, float zUp, float FOV, int width, int height, int renderMode) {
   SDL_Surface *surface;
   int videoFlags;
   
   if (render != NULL)
      delete render;
      
   render = new Layer;

   //////////////////////////////////////////////////////////////////////////////////////////////////
   
   if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
      SDL_Quit( );

   /* the flags to pass to SDL_SetVideoMode */
   videoFlags  = SDL_OPENGLBLIT;          /* Enable OpenGL in SDL */
   videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */

   /* get a SDL surface */
   surface = SDL_SetVideoMode( width, height, 32, videoFlags );

   //////////////////////////////////////////////////////////////////////////////////////////////////
   
   render->setSize(width, height);
   
   //Set destination buffer
   glDrawBuffer(GL_BACK);

   // Enable smooth shading
   glShadeModel( GL_SMOOTH );

   // Set background color
   glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
   
   // Set clear depth
   glClearDepth( 1.0f );

   // Enable depth testing
   glEnable( GL_DEPTH_TEST );
   glDepthFunc(GL_LEQUAL);
   
   // Perspective correction
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   
   // Setup viewport
   glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

   // Change to the projection matrix and set the viewing volume
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity( );

   // Set the viewport
   gluPerspective( FOV, (float)width/(float)height, 0.1f, 1000.0f );

   // Make sure we're changing the model view and not the projection
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity( );
   
   // Clear screen and depth buffer
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   //Create a light at the camera position
   GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
   GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};
   glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
   glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHTING);
   
   gluLookAt(xPosition, yPosition, zPosition, xTarget, yTarget, zTarget, xUp, yUp, zUp);
   
   glColor4ub(255, 255, 255, 255);
   
   //TODO: Renderizar cada poly de distinta forma o cambiar la propiedad para que sea del objeto completo
   if (face[0].twoSided) {
      // Disable culling back faces
      glDisable(GL_CULL_FACE);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
   }
   else {
      // Enable culling back faces
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
   }
         
   if (textureMapped) {
      // Enable texturing
      GLuint textureID;
      
      glActiveTexture(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      
      glGenTextures(1, &textureID);
      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureMap->getLayer(0)->getWidth(), textureMap->getLayer(0)->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureMap->getTexture());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
      glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
   }
   else {
      glActiveTexture(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);      
   }
   
   if (environmentMapped) {
      // Enable texturing
      GLuint environmentID;
      
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
      
      glGenTextures(1, &environmentID);
      glBindTexture(GL_TEXTURE_2D, environmentID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, environmentMap->getLayer(0)->getWidth(), environmentMap->getLayer(0)->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, environmentMap->getTexture());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
      glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);
      
      //Environment mapping
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
      
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
   }
   else {
      glActiveTexture(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);      
   }
   
   if (renderMode == 0) {
      glDisable(GL_LIGHTING);
      glBegin(GL_LINES);
      for (int faceNumber = 0; faceNumber < numberOfFaces; faceNumber++) {
         int vertex1 = face[faceNumber].vertex1;
         int vertex2 = face[faceNumber].vertex2;
         int vertex3 = face[faceNumber].vertex3;
      
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex1]);
         glVertex3fv((GLfloat *)&vertex[vertex1]);
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex2]);
         glVertex3fv((GLfloat *)&vertex[vertex2]);
         
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex2]);
         glVertex3fv((GLfloat *)&vertex[vertex2]);
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex3]);
         glVertex3fv((GLfloat *)&vertex[vertex3]);
         
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex3]);
         glVertex3fv((GLfloat *)&vertex[vertex3]);
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex1]);
         glVertex3fv((GLfloat *)&vertex[vertex1]);
      }
      glEnd();
   }
   else if (renderMode == 1) {
      glBegin(GL_TRIANGLES);
      for (int faceNumber = 0; faceNumber < numberOfFaces; faceNumber++) {
         glNormal3fv((GLfloat *)&faceNormal[faceNumber]);
      
         int vertex1 = face[faceNumber].vertex1;
         int vertex2 = face[faceNumber].vertex2;
         int vertex3 = face[faceNumber].vertex3;
      
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex1]);
         glVertex3fv((GLfloat *)&vertex[vertex1]);
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex2]);
         glVertex3fv((GLfloat *)&vertex[vertex2]);
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex3]);
         glVertex3fv((GLfloat *)&vertex[vertex3]);
      }
      glEnd();
   }
   else if (renderMode == 2) {
      glBegin(GL_TRIANGLES);
      for (int faceNumber = 0; faceNumber < numberOfFaces; faceNumber++) {
         int vertex1 = face[faceNumber].vertex1;
         int vertex2 = face[faceNumber].vertex2;
         int vertex3 = face[faceNumber].vertex3;
      
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex1]);
         glNormal3fv((GLfloat *)&vertexNormal[vertex1]);
         glVertex3fv((GLfloat *)&vertex[vertex1]);
         
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex2]);
         glNormal3fv((GLfloat *)&vertexNormal[vertex2]);
         glVertex3fv((GLfloat *)&vertex[vertex2]);
         
         glMultiTexCoord2fv(GL_TEXTURE0_ARB, (GLfloat *)&vertexUV[vertex3]);
         glNormal3fv((GLfloat *)&vertexNormal[vertex3]);
         glVertex3fv((GLfloat *)&vertex[vertex3]);
      }
      glEnd();
   }
   
   //Load texture data from openGL buffer
   glReadBuffer(GL_BACK);
   glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, render->getImageDataPtr());
   
   glFlush();

   SDL_Quit( );
   
   return render->getImageDataPtr();
}
