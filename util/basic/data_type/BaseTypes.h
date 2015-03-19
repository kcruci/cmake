// ------------------------------------------------------------------------------------------------
//
//  File: BaseTypes.h
//
//      Description     - Defines the Basic Types used by the engine
//      Author          - Aidan Millott
//
//  Modifications:
//  --------------
//      July 29, 2009    - Initial creation.
//
// ------------------------------------------------------------------------------------------------

#ifndef __BASETYPES_H
#define __BASETYPES_H

typedef unsigned char		u8;
typedef unsigned short int	u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef signed char			s8;
typedef signed short int	s16;
typedef signed int			s32;
typedef signed long long	s64;

//typedef u32					uPtr_t;
//typedef s32					sPtr_t;

typedef volatile u8			vu8;
typedef volatile u16		vu16;
typedef volatile u32		vu32;
typedef volatile u64		vu64;

typedef volatile s8			vs8;
typedef volatile s16		vs16;
typedef volatile s32		vs32;
typedef volatile s64		vs64;

typedef float				kf32;
typedef volatile kf32		vkf32;

typedef signed int			fx32;
typedef signed short int	fx16;

typedef kf32				dec32;


#if 0
////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//								INCLUDES.								  //
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////

#include "../Startup/OSDefs.h"
#include "../RenderingApiSettings.h"

////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//								DEFINES.								  //
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////

// Windows Development

#if IS_RENDERING_API(D3D9)
	#include <d3dx9.h>
#endif

#if defined(PLATFORM_PC)  || defined(PLATFORM_360)

typedef unsigned char		u8;
typedef unsigned short int	u16;
typedef unsigned long		u32;
typedef unsigned __int64	u64;

typedef signed char			s8;
typedef signed short int	s16;
typedef signed long			s32;
typedef signed __int64		s64;

#ifdef _MSC_EXTENSIONS
// This should remove warnings about pointer sizes under MS compilers
typedef __w64 u32			uPtr_t;
typedef __w64 s32			sPtr_t;
#else
typedef u32					uPtr_t;
typedef s32					sPtr_t;
#endif

typedef volatile u8			vu8;
typedef volatile u16		vu16;
typedef volatile u32		vu32;
typedef volatile u64		vu64;

typedef volatile s8			vs8;
typedef volatile s16		vs16;
typedef volatile s32		vs32;
typedef volatile s64		vs64;

typedef float				kf32;
typedef volatile kf32		vkf32;

typedef signed long			fx32;
typedef signed short int	fx16;

typedef kf32				dec32;

#elif defined(PLATFORM_PSP)		// PSP Development

#include <psptypes.h>

typedef unsigned char		u8;
typedef unsigned short int	u16;
typedef unsigned long		u32;
typedef u_int64				u64;

typedef signed char			s8;
typedef signed short int	s16;
typedef signed long			s32;
typedef int64				s64;

typedef u32					uPtr_t;
typedef s32					sPtr_t;

typedef volatile u8			vu8;
typedef volatile u16		vu16;
typedef volatile u32		vu32;
typedef volatile u64		vu64;

typedef volatile s8			vs8;
typedef volatile s16		vs16;
typedef volatile s32		vs32;
typedef volatile s64		vs64;

typedef float				kf32;
typedef volatile kf32		vkf32;

typedef signed long			fx32;
typedef signed short int	fx16;

typedef kf32				dec32;

#error These need to be moved to a Rendering API
typedef ScePspFVector3 platVector3;
typedef ScePspFMatrix4 platMatrix44;
typedef u32 platColour;
typedef ScePspFVector2 platVector2;
#elif defined(PLATFORM_IPHONE) 
//#include <MacTypes.h>

typedef unsigned char		u8;
typedef unsigned short int	u16;
typedef unsigned long		u32;
typedef unsigned long long	u64;

typedef signed char			s8;
typedef signed short int	s16;
typedef signed long			s32;
typedef signed long long	s64;

typedef u32					uPtr_t;
typedef s32					sPtr_t;

typedef volatile u8			vu8;
typedef volatile u16		vu16;
typedef volatile u32		vu32;
typedef volatile u64		vu64;

typedef volatile s8			vs8;
typedef volatile s16		vs16;
typedef volatile s32		vs32;
typedef volatile s64		vs64;

typedef float				kf32;
typedef volatile kf32		vkf32;

typedef signed long			fx32;
typedef signed short int	fx16;

typedef kf32				dec32;

#elif defined(PLATFORM_ANDROID)
//#include <MacTypes.h>

typedef unsigned char		u8;
typedef unsigned short int	u16;
typedef unsigned long		u32;
typedef unsigned long long	u64;

typedef signed char			s8;
typedef signed short int	s16;
typedef signed long			s32;
typedef signed long long	s64;

typedef u32					uPtr_t;
typedef s32					sPtr_t;

typedef volatile u8			vu8;
typedef volatile u16		vu16;
typedef volatile u32		vu32;
typedef volatile u64		vu64;

typedef volatile s8			vs8;
typedef volatile s16		vs16;
typedef volatile s32		vs32;
typedef volatile s64		vs64;

typedef float				kf32;
typedef volatile kf32		vkf32;

typedef signed long			fx32;
typedef signed short int	fx16;

typedef kf32				dec32;

#elif defined(PLATFORM_MANDREEL)

typedef unsigned char           u8;
typedef unsigned short int      u16;
typedef unsigned long           u32;
typedef unsigned long long      u64;

typedef signed char             s8;
typedef signed short int		s16;
typedef signed long			    s32;
typedef signed long long		s64;

typedef u32                     uPtr_t;
typedef s32					    sPtr_t;

typedef volatile u8			    vu8;
typedef volatile u16			vu16;
typedef volatile u32			vu32;
typedef volatile u64			vu64;

typedef volatile s8			    vs8;
typedef volatile s16			vs16;
typedef volatile s32			vs32;
typedef volatile s64			vs64;

typedef float					kf32;
typedef volatile kf32			vkf32;

typedef signed long			    fx32;
typedef signed short int		fx16;

typedef kf32					dec32;

#elif defined(PLATFORM_BADA) || defined(PLATFORM_NOKIA) || defined(PLATFORM_WEBOS) || defined(PLATFORM_QT)  ||defined(PLATFORM_LGLINUX)

typedef unsigned char           u8;
typedef unsigned short int      u16;
typedef unsigned long           u32;
typedef unsigned long long      u64;

typedef signed char             s8;
typedef signed short int		s16;
typedef signed long			    s32;
typedef signed long long		s64;

typedef u32                     uPtr_t;
typedef s32					    sPtr_t;

typedef volatile u8			    vu8;
typedef volatile u16			vu16;
typedef volatile u32			vu32;
typedef volatile u64			vu64;

typedef volatile s8			    vs8;
typedef volatile s16			vs16;
typedef volatile s32			vs32;
typedef volatile s64			vs64;

typedef float					kf32;
typedef volatile kf32			vkf32;

typedef signed long			    fx32;
typedef signed short int		fx16;

typedef kf32					dec32;

#elif defined(PLATFORM_OSX)
//#include <MacTypes.h>

typedef unsigned char		u8;
typedef unsigned short int	u16;
typedef unsigned long		u32;
typedef unsigned long long	u64;

typedef signed char			s8;
typedef signed short int	s16;
typedef signed long			s32;
typedef signed long long	s64;

typedef u32					uPtr_t;
typedef s32					sPtr_t;

typedef volatile u8			vu8;
typedef volatile u16		vu16;
typedef volatile u32		vu32;
typedef volatile u64		vu64;

typedef volatile s8			vs8;
typedef volatile s16		vs16;
typedef volatile s32		vs32;
typedef volatile s64		vs64;

typedef float				kf32;
typedef volatile kf32		vkf32;

typedef signed long			fx32;
typedef signed short int	fx16;

typedef kf32				dec32;


#else
#error no option defined for this platform
#endif

#if IS_RENDERING_API(D3D11)
	
struct platVector3
{
	float x,y,z;
};

struct platVector2
{
	float x,y;
};

struct platMatrix44
{
	float m[16];
};
typedef u32 platColour;

#elif IS_RENDERING_API(D3D9)

typedef D3DXVECTOR3 platVector3;
#if defined(PLATFORM_360)
typedef D3DXMATRIXA16 platMatrix44;
#else
typedef D3DXMATRIX platMatrix44;
#endif
typedef u32 platColour;

typedef D3DXVECTOR2 platVector2;

#elif IS_RENDERING_API(GLES1) || IS_RENDERING_API(GLES2)

struct platVector3
{
	float x,y,z;
};

struct platVector2
{
	float x,y;
};

struct platMatrix44
{
	float m[16];
};
typedef u32 platColour;

#else

#error Please define the platform specific types for this rendering API

#endif



#if defined(_MSC_VER)
#pragma warning(disable : 4201)	// "nonstandard extension used: nameless struct/union"
#endif
union float4
{
	struct
	{
		kf32 x,y,z,w;
	};
	struct
	{
		kf32 a[4];
	};
};
struct float3
{
	kf32 x,y,z;
};
struct float2
{
	kf32 x,y;
};

union float4x4
{
	struct
	{
		kf32 _00, _01, _02, _03, _10, _11, _12, _13, _20, _21, _22, _23, _30, _31, _32, _33; 
        };
	kf32 arrayForm[16];
};


// This include is placed here so that the base types will always be defined for 'MortarAssert'
#include "../Debug/CompileTimeHelpers.h"

////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//								STRUCTURES.								  //
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////

template<typename T>
class PointerConverter
{
	union
	{
		void*	m_ptr;
		T		m_val;
	};

public:
	inline PointerConverter();
	inline PointerConverter(T val);

	template<typename PtrType>
	inline PointerConverter(PtrType *ptr);
	
	inline operator void*() const { return m_ptr; }
	inline operator T() const { return m_val; }

	template<typename PtrType>
	inline PtrType *GetPtr() { return (PtrType*)m_ptr; }

	inline bool operator <(T rhs) const { return m_val < rhs; }
	inline bool operator >(T rhs) const { return m_val > rhs; }
	inline bool operator ==(T rhs) const { return m_val == rhs; }
	inline bool operator <=(T rhs) const { return m_val <= rhs; }
	inline bool operator >=(T rhs) const { return m_val >= rhs; }
	inline bool operator !=(T rhs) const { return m_val != rhs; }

	inline void operator -=(T rhs) { m_val -= rhs; }
	inline void operator +=(T rhs) { m_val += rhs; }
};

typedef PointerConverter<uPtr_t> uPtr;
typedef PointerConverter<sPtr_t> sPtr;



template<typename T> PointerConverter<T>::PointerConverter() { COMPILER_ASSERT(sizeof(T)	== sizeof(void*)); }
template<typename T> PointerConverter<T>::PointerConverter(T val) { COMPILER_ASSERT(sizeof(T)	== sizeof(void*)); m_val = val; }

template<typename T>
template<typename PtrType>
PointerConverter<T>::PointerConverter(PtrType *ptr) { COMPILER_ASSERT(sizeof(T)	== sizeof(void*)); m_ptr = (void*)ptr; }


////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//								FUNCTIONS.								  //
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//								EXTERNALS.								  //
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//								Asserts.								  //
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////
#include "../Debug/MortarAssert.h"
#include "../Startup/macros.h"
//COMPILER_ASSERT(sizeof(u8)		== 1);
//COMPILER_ASSERT(sizeof(u16)		== 2);
//COMPILER_ASSERT(sizeof(u32)		== 4);
//COMPILER_ASSERT(sizeof(u64)		== 8);
//
//COMPILER_ASSERT(sizeof(s8)		== 1);
//COMPILER_ASSERT(sizeof(s16)		== 2);
//COMPILER_ASSERT(sizeof(s32)		== 4);
//COMPILER_ASSERT(sizeof(s64)		== 8);
//
//COMPILER_ASSERT(sizeof(vu8)		== 1);
//COMPILER_ASSERT(sizeof(vu16)	== 2);
//COMPILER_ASSERT(sizeof(vu32)	== 4);
//COMPILER_ASSERT(sizeof(vu64)	== 8);
//
//COMPILER_ASSERT(sizeof(vs8)		== 1);
//COMPILER_ASSERT(sizeof(vs16)	== 2);
//COMPILER_ASSERT(sizeof(vs32)	== 4);
//COMPILER_ASSERT(sizeof(vs64)	== 8);
//
//COMPILER_ASSERT(sizeof(kf32)	== 4);
//COMPILER_ASSERT(sizeof(vkf32)	== 4);
//
//COMPILER_ASSERT(sizeof(fx32)	== 4);
//COMPILER_ASSERT(sizeof(fx16)	== 2);
//
//COMPILER_ASSERT(sizeof(dec32)	== 4);
//
//COMPILER_ASSERT_MSG(sizeof(uPtr_t) == sizeof(void*), "Size mismatch between 'uPtr_t' and an actual pointer");
//COMPILER_ASSERT_MSG(sizeof(sPtr_t) == sizeof(void*), "Size mismatch between 'sPtr_t' and an actual pointer");
#endif
#endif //__BASETYPES_H
