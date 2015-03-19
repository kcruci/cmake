#ifndef _IDS_BASIC_MACRO_H
#define _IDS_BASIC_MACRO_H

#include <linux/stddef.h>

#define DIM(x) (sizeof(x) / sizeof((x)[0]))
#define DIME(arr) (sizeof(arr) / sizeof(((typeof(arr)){})[0]))

#if 0
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) \
        + sizeof(typeof(int[1 - 2*!!__builtin_types_compatible_p(typeof(arr),typeof(&arr[0]))]))*0)
#endif


#define MemberToStruct(MemberPtr, StructType, MemberName) \
	((StructType *) ((char *) MemberPtr - offsetof(StructType, MemberName)))


#define CAT_TOKEN_1(t1,t2) t1##t2
#define CAT_TOKEN(t1,t2) CAT_TOKEN_1(t1,t2)

#define COMPILE_ASSERT(x)  enum {CAT_TOKEN (comp_assert_at_line_, __LINE__) = 1 / !!(x) };


/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#define typecheck(type,x) \
({  type __dummy; \
    typeof(x) __dummy2; \
    (void)(&__dummy == &__dummy2); \
	 1; \
})

/*
 * Check at compile time that 'function' is a certain type, or is a pointer
 * to that type (needs to use typedef for the function type.)
 */
#define typecheck_fn(type,function) \
({  typeof(type) __tmp = function; \
    (void)__tmp; \
})





#endif
