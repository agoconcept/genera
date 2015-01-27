%module curve
%{
#include "defines.h"
#include "curve.h"
#include "component.h"
%}

%include "defines.h"
%include "component.h"

class Curve : public Component {
   bool filled;
   
   int numberOfPoints;
   int segmentsPerCurve;
   int lineWidth;

   bool *visible;
   bool *endPoint;

   int (*controlPoint)[2];
   int (*leftTangent)[2];
   int (*rightTangent)[2];

public:
   Curve();
   ~Curve();
   
   BYTE *loadData(BYTE *data);
   
   bool getFilled() {return filled;}
   
   int getNumberOfPoints() {return numberOfPoints;}
   int getSegmentsPerCurve() {return segmentsPerCurve;}
   int getLineWidth() {return lineWidth;}
   
   bool getVisible(int i);
   bool getEndPoint(int i);
   
   int *getControlPoint(int i);
   int *getLeftTangent(int i);
   int *getRightTangent(int i);
   
   void generate();
};
