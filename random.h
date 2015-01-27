#ifndef _RANDOM_H
#define _RANDOM_H

///////////////////////////////////////////////////////////////////////////////////
// Adapted from :
// http://www.duckware.com/bugfreec/appendix.html
///////////////////////////////////////////////////////////////////////////////////

//RAND_MAX = 2^15 - 1
#ifndef RAND_MAX
#define RAND_MAX 0x7FFF
#endif

class Random {
   int prev;

public:
   Random(int seed);
   int rand();
};

#endif //_RANDOM_H
