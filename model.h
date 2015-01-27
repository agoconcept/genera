#ifndef _MODEL_H
#define _MODEL_H

#include "defines.h"

//Use openGL extensions
#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#ifdef __WIN32__
#include "SDL.h"
#else
#include <SDL/SDL.h>
#endif

#include "vector.h"
#include "component.h"
#include "curve.h"
#include "texture.h"

#include "genera.h"

#define MODEL_FX_ON        0x80
#define MODEL_FX_TYPE      0x7F

enum ModelFXID {
   CUBE, CYLINDER, EXTRUDE_CURVE, HEIGHT_FIELD, METABALL, OCTAHEDRON, SPHERE, STAR, SWEEP_CURVE, TORUS,
   HEIGHT_MAP, MOVE_OBJECT, NOISE_OBJECT, REFINE, ROTATE_OBJECT, SCALE_OBJECT, SPHERIZE, TAPER, TESSELLATE, TWIST_OBJECT,
   ENVIRONMENT_MAP, TEXTURE_MAP
};

class Texture;
class Genera;

struct Texel {
   float u, v;
};

struct Face {
   int vertex1, vertex2, vertex3;
   bool twoSided;
};

class Model : public Component {
   Genera *genera;   //Pointer to genera, to be able to get gradients and curves of the project

   BYTE *data;       //Store model data
   
   int      numberOfVertices;
   Vector   *vertex;             //Vertex list
   Texel    *vertexUV;           //Texture UV
   Vector   *vertexNormal;       //Vertex normal list
   
   Vector   min, max, center;

   int      numberOfFaces;
   Face     *face;               //Faces list
   Vector   *faceNormal;         //Faces normal list
   
   Texture  *textureMap;         //Texture map
   Texture  *environmentMap;     //Environment map
   
   bool     textureMapped;
   bool     environmentMapped;
   
   Layer    *render;             //Render of the object
   
public:
   Model();
   ~Model();

   void setGenerator(Genera *genera);

   BYTE *loadData(BYTE *data);

   void generate();
   
   void precalculate();          //Precalculate

   int getNumberOfVertices() {return numberOfVertices;}
   void setNumberOfVertices(int numberOfVertices) {this->numberOfVertices = numberOfVertices;}
   Vector *getVertex() {return vertex;}
   void setVertex(Vector *vertex) {this->vertex = vertex;}
   Vector *getVertexNormal() {return vertexNormal;}
   void setVertexNormal(Vector *vertexNormal) {this->vertexNormal = vertexNormal;}
   Texel *getVertexUV() {return vertexUV;}
   void setVertexUV(Texel *vertexUV) {this->vertexUV = vertexUV;}
   
   Vector getMin() {return min;}
   Vector getMax() {return max;}
   Vector getCenter() {return center;}

   int getNumberOfFaces() {return numberOfFaces;}
   void setNumberOfFaces(int numberOfFaces) {this->numberOfFaces = numberOfFaces;}
   Face *getFace() {return face;}
   void setFace(Face *face) {this->face = face;}
   Vector *getFaceNormal() {return faceNormal;}
   void setFaceNormal(Vector *faceNormal) {this->faceNormal = faceNormal;}
   
   Texture *getTextureMap() {return textureMap;}
   void setTextureMap(Texture *textureMap) {this->textureMap = textureMap;}
   Texture *getEnvironmentMap() {return environmentMap;}
   void setEnvironmentMap(Texture *environmentMap) {this->environmentMap = environmentMap;}
   
   bool getTextureMapped() {return textureMapped;}
   void setTextureMapped(bool textureMapped) {this->textureMapped = textureMapped;}
   bool getEnvironmentMapped() {return environmentMapped;}
   void setEnvironmentMapped(bool environmentMapped) {this->environmentMapped = environmentMapped;}
   
   //BASE MODELS
   BYTE *cube(BYTE *ptr, bool fxOn);
   BYTE *cylinder(BYTE *ptr, bool fxOn);
   //BYTE *extrudeCurve(BYTE *ptr, bool fxOn);
   BYTE *heightField(BYTE *ptr, bool fxOn);
   //BYTE *metaball(BYTE *ptr, bool fxOn);
   BYTE *octahedron(BYTE *ptr, bool fxOn);
   BYTE *sphere(BYTE *ptr, bool fxOn);
   BYTE *star(BYTE *ptr, bool fxOn);
   //BYTE *sweepCurve(BYTE *ptr, bool fxOn);
   BYTE *torus(BYTE *ptr, bool fxOn);
   //TODO: import3DS
   
   //DISTORSIONS
   BYTE *heightMap(BYTE *ptr, bool fxOn);
   BYTE *moveObject(BYTE *ptr, bool fxOn);
   BYTE *noise(BYTE *ptr, bool fxOn);
   BYTE *refine(BYTE *ptr, bool fxOn);
   BYTE *rotateObject(BYTE *ptr, bool fxOn);
   BYTE *scaleObject(BYTE *ptr, bool fxOn);
   BYTE *spherize(BYTE *ptr, bool fxOn);
   BYTE *taper(BYTE *ptr, bool fxOn);
   BYTE *tessellate(BYTE *ptr, bool fxOn);
   BYTE *twistObject(BYTE *ptr, bool fxOn);
   //TODO: bend?, disconnect (separar cada cara), refine, smooth, tessellate

   //TEXTURE MAPS
   BYTE *environment(BYTE *ptr, bool fxOn);
   BYTE *texture(BYTE *ptr, bool fxOn);

   //Draw object
   DWORD *draw(float xPosition, float yPosition, float zPosition, float xTarget, float yTarget, float zTarget, float xUp, float yUp, float zUp, float FOV, int width, int height, int renderMode);
};

#endif //_MODEL_H
