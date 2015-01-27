#include "curve.h"

#ifdef __DEBUG
#include <stdio.h>
#endif //__DEBUG

Curve::Curve() {
   filled = true;
   numberOfPoints = segmentsPerCurve = lineWidth = 0;
   visible = endPoint = NULL;
   controlPoint = NULL;
   leftTangent = NULL;
   rightTangent = NULL;
}

Curve::~Curve() {
   if (visible != NULL)
      delete []visible;
   if (endPoint != NULL)
      delete []endPoint;
   
   if (controlPoint != NULL)
      delete []controlPoint;
   if (leftTangent != NULL)
      delete []leftTangent;
   if (rightTangent != NULL)
      delete []rightTangent;
}

BYTE *Curve::loadData(BYTE *data) {
   int i, j;
   BYTE val;
   
   //Pointer to curve data
   BYTE *ptr = data;

   //Load curve name
   ptr = readString(name, data);

#ifdef __DEBUG
   fprintf(stderr, "LOG: Curve::loadData() - Curve name: %s\n", name);
#endif //__DEBUG
   
   val = *ptr++;
   
   filled = (val&0x80) != 0;
   numberOfPoints = val&0x7F;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Curve::loadData() - Number of points: %d\n", numberOfPoints);
#endif //__DEBUG
   
   segmentsPerCurve = *ptr++;
   
#ifdef __DEBUG
   fprintf(stderr, "LOG: Curve::loadData() - Segments per curve: %d\n", segmentsPerCurve);
#endif //__DEBUG
   
   lineWidth = *ptr++;

#ifdef __DEBUG
   fprintf(stderr, "LOG: Curve::loadData() - Line width: %d\n", lineWidth);
#endif //__DEBUG
   
   if (visible != NULL)
      delete []visible;
   visible = new bool[8*(1+numberOfPoints/8)];
   
   if (endPoint != NULL)
      delete []endPoint;
   endPoint = new bool[8*(1+numberOfPoints/8)];
   
   if (controlPoint != NULL)
      delete []controlPoint;
   controlPoint = new int[numberOfPoints][2];
   
   if (leftTangent != NULL)
      delete []leftTangent;
   leftTangent = new int[numberOfPoints][2];
   
   if (rightTangent != NULL)
      delete []rightTangent;
   rightTangent = new int[numberOfPoints][2];
   
   for (i = 0; i < (numberOfPoints+7)/8; i++) {
      val = *ptr++;
      
#ifdef __DEBUG
   fprintf(stderr, "LOG: Curve::loadData() - Visible: %x\n", val);
#endif //__DEBUG
   
      for (j = 0; j < 8; j++)
         visible[i*8+j] = (val>>j)&0x01;
      
      val = *ptr++;
      
#ifdef __DEBUG
   fprintf(stderr, "LOG: Curve::loadData() - End point: %x\n", val);
#endif //__DEBUG
   
      for (j = 0; j < 8; j++)
         endPoint[i*8+j] = (val>>j)&0x01;
   }

   for (i = 0; i < numberOfPoints; i++) {
      controlPoint[i][0] = *ptr++;
      controlPoint[i][1] = *ptr++;
      
#ifdef __DEBUG
   fprintf(stderr, "LOG: Curve::loadData() - Control point: %d, %d\n", controlPoint[i][0], controlPoint[i][1]);
#endif //__DEBUG
   
      leftTangent[i][0] = *ptr++;
      leftTangent[i][1] = *ptr++;
      
#ifdef __DEBUG
   fprintf(stderr, "LOG: Curve::loadData() - Left tangent: %d, %d\n", leftTangent[i][0], leftTangent[i][1]);
#endif //__DEBUG
   
      rightTangent[i][0] = *ptr++;
      rightTangent[i][1] = *ptr++;

#ifdef __DEBUG
   fprintf(stderr, "LOG: Curve::loadData() - Right tangent: %d, %d\n", rightTangent[i][0], rightTangent[i][1]);
#endif //__DEBUG
   }
   
   return ptr;
}

bool Curve::getVisible(int i) {
   if (i < numberOfPoints)
      return visible[i];
   else
      return false;
}

bool Curve::getEndPoint(int i) {
   if (i < numberOfPoints)
      return endPoint[i];
   else
      return false;
}
   
int *Curve::getControlPoint(int i) {
   if (i < numberOfPoints)
      return controlPoint[i];
   else
      return false;
}

int *Curve::getLeftTangent(int i) {
   if (i < numberOfPoints)
      return leftTangent[i];
   else
      return false;
}

int *Curve::getRightTangent(int i) {
   if (i < numberOfPoints)
      return rightTangent[i];
   else
      return false;
}

void Curve::generate() {
}
