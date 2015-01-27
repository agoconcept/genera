%module random
%{
#include "random.h"
%}

class Random {
   int prev;

public:
   Random(int seed);
   int rand();
};
