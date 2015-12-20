#include <Windows.h>
#define CASTOFFSET(type, a, o) (type*)((uchar*)a + o)
typedef unsigned char uchar;

#ifdef ENABLE_LOG
#define LOG(f, m) g_logger.Log(f, m)
#else
#define LOG(f, m)
#endif

