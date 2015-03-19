#include "MyNetworkByteOrder.h"


//DDWORDÀàÐÍ´¦Àí
#define swap_ddword(x)  \
   ((((x) & 0xff00000000000000llu) >> 56) | \
    (((x) & 0x00ff000000000000llu) >> 40) | \
    (((x) & 0x0000ff0000000000llu) >> 24) | \
    (((x) & 0x0000ff0000000000llu) >> 24) | \
    (((x) & 0x000000ff00000000llu) >> 8) | \
    (((x) & 0x00000000ff000000llu) << 8) | \
    (((x) & 0x0000000000ff0000llu) << 24) | \
    (((x) & 0x000000000000ff00llu) << 40) | \
    (((x) & 0x00000000000000ffllu) << 56) )

uint64_t Switch(uint64_t a)
{   
#if BYTE_ORDER == BIG_ENDIAN
    return a;
#elif BYTE_ORDER == LITTLE_ENDIAN
    return swap_ddword(a);
#else
#error "What kind of system is this?"
#endif
}  



