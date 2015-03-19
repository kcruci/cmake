#ifndef _MY_NETWORK_BYTE_ORDER_H
#define  _MY_NETWORK_BYTE_ORDER_H
#include <stdint.h>

#if defined(_WIN32)
#include <windows.h>
#else //if defined(_LINUX)
#include <arpa/inet.h>
#endif

uint64_t Switch(uint64_t a);
#ifndef htonll
#define htonll(x) Switch((x))
#endif
#ifndef ntohll
#define ntohll(x) Switch((x))
#endif

#endif // _MY_NETWORK_BYTE_ORDER_H
