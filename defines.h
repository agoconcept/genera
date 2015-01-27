#ifndef _DEFINES_H
#define _DEFINES_H

#ifdef WIN32
   #include <windows.h>
#else
   typedef unsigned char BYTE;
   typedef unsigned short WORD;
   typedef unsigned int DWORD;
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef PI
#define PI 3.1415926f
#endif

#ifndef SQRT2
#define SQRT2 1.4142136f
#endif

#ifndef DELTA
#define DELTA 0.000001
#endif

//#define __DEBUG

#endif //_DEFINES_H
