#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include <stdint.h>

#if !defined(__i386__) && !defined(__x86_64__)
#error	"Arch not supprot!"
#endif

#ifdef	__cplusplus
extern "C" {
#endif

static inline int 		// return old value
atomic_add(volatile int *count, int add)
{
#ifdef __linux__
	__asm__ __volatile__(
		"lock xadd %0, (%1);"
		: "=a"(add)
		: "r"(count), "a"(add)
		: "memory"
	);
#else
#error "not done yet "
#endif
	return add;
}

static inline int 		// return old value
atomic_swap(volatile void *lockword, int value)
{
#ifdef __linux__
    __asm__ __volatile__(
        "lock xchg %0, (%1);"
        : "=a"(value)
        : "r"(lockword), "a"(value)
        : "memory"
        );
#else
#error "not done yet "
#endif
    return value;
}

static inline int 		// return old value
atomic_comp_swap(volatile void *lockword, 
                  int exchange,  
                  int comperand)
{
#ifdef __linux__
    __asm__ __volatile__(
        "lock cmpxchg %1, (%2)"
        :"=a"(comperand)
        :"d"(exchange), "r"(lockword), "a"(comperand) 
        );
#else
    __asm {
        mov eax, comperand
        mov edx, exchange
        mov ecx, lockword
        lock cmpxchg [ecx], edx
        mov comperand, eax
    }
#endif /* #ifdef __linux__ */
    return comperand;
}

static inline int 		// return old value
atomic_comp_swap64(volatile void *lockword, 
                 int64_t exchange,  
                 int64_t comperand)
{
#ifdef __linux__
    __asm__ __volatile__(
        "lock cmpxchg %1, (%2)"
        :"=a"(comperand)
        :"d"(exchange), "r"(lockword), "a"(comperand) 
        );
#else
#error "not done yet!"
#endif /* #ifdef __linux__ */
    return comperand;
}

#define nop()						    __asm__ ("pause" )

#ifdef __linux__
#define mfence_c()					    __asm__ __volatile__ ("": : :"memory")
#else
#define mfence_c()                      _ReadWriteBarrier()
#endif

#define mfence_x86()                    __asm__ __volatile__ ("mfence": : :"memory")

#define AtomicGetValue(x)			    (atomic_comp_swap(&(x), 0, 0))
#define AtomicSetValue(x, v)		    (atomic_swap(&(x), (v)))
#define AtomicSetValueIf(x, v, ifn)		(atomic_comp_swap(&(x), (v), ifn))

// not tested
//#define AtomicGetValue64(c)			(atomic_comp_swap64(&(c), 0, 0))
//#define AtomicSetValue64(c, n)		(atomic_swap64(&(c), (n), (c)))

// return new value
#define AtomicDec(c)				    (atomic_add(&(c), -1) - 1)
#define AtomicInc(c)				    (atomic_add(&(c), 1) + 1)

#ifdef	__cplusplus
}
#endif

#endif
