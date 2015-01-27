#include "random.h"

///////////////////////////////////////////////////////////////////////////////////
// Adapted from :
// http://www.duckware.com/bugfreec/appendix.html
///////////////////////////////////////////////////////////////////////////////////

Random::Random(int seed) {
   prev = seed<<16;
}

int Random::rand() {
   prev = prev*214013L + 2531011L;
   return (prev>>16)&0x7FFF;
}
