#include "first.hpp"

bool lo2hi(void)
{ // returns 1 for littleendian and 0 for bigendian
    int one = 1;
    char * onePtr = reinterpret_cast<char *>(&one);
    char byte1 = *onePtr;
    assert ((byte1 == 0) || (byte1 == 1));
    return byte1 == 1; 
}