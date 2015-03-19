#ifndef _STDALL_H
#define _STDALL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <err.h>
#include <assert.h>
#include <netdb.h>



#include "atomic.h"

#define REG_HOSTNAME "[a-z]{2}(01)?-[a-z0-9.-]*\\.[a-z]{2}01"
#define NEW(s)          (s*)calloc(1,sizeof(s))
#define bool int
#define true 1
#define false 0

#define MB 1024*1024
#define KB 1024

#ifdef __linux__
#define LIKELY(x)           (__builtin_expect((x),1))
#define UNLIKELY(x)         (__builtin_expect((x),0))
#define UNLIKELY_IF(x)      if (__builtin_expect((x),0))
#else
#define LIKELY(x)           (x)
#define UNLIKELY(x)         (x)
#define UNLIKELY_IF(x)      if (x)
#endif


#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif

#ifndef container_of
#define container_of(address, type, field) ((type *)( 						\
                                                 (char*)(address) - 			\
                                                 offsetof(type, field)))
#endif

typedef float  float32_t;  
typedef double float64_t;  
typedef int    bool_t, lresult_t;  
#define        bool_true     1  
#define        bool_false    0  
#define        res_success   0  
#define        res_error    -1  
#define        res_false     1  
#ifndef IN  
#define IN  
#endif  
#ifndef OUT  
    #define OUT  
#endif  
#ifndef INOUT  
    #define INOUT  
#endif  
#ifndef OPTIONAL  
    #define OPTIONAL  
#endif  


/* date of building */
#if defined(__DATE__) && defined(__TIME__)
    static const char g_build_time[] = __DATE__ " " __TIME__;
#else
    static const char g_build_time[] = "unknown";
#endif


#endif
