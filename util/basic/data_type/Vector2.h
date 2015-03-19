// ------------------------------------------------------------------------------------------------
//
//  File: Vector2.h
//
//      Description     - Implementation of a x,y vector class
//
// ------------------------------------------------------------------------------------------------

#ifndef __VECTOR2_H
#define __VECTOR2_H

////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//								INCLUDES.								  //
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////

#include "BaseTypes.h"
//#include "Point2D.h"
//#include "Math/EngineMath.h"
//#include "../Startup/Macros.h"
#include <limits>
#include <cmath>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//								DEFINES.								  //
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////

namespace Mortar
{
		// used to define conversion from one vector type to another
		// you must define conversion both ways
	template <typename ReturnType, typename InputType> inline ReturnType ConvertVector(const InputType&);
	
		// e.g.
//	template<> inline Vector2 ConvertVector<Vector2, PhysicsVector>(const PhysicsVector& input)
//	{
//		return Vector2(input.x, input.y);
//	}
//	template<> inline PhysicsVector ConvertVector<PhysicsVector, Vector2>(const Vector2& input)
//	{
//		return PhysicsVector(input.x, input.y);
//	}
	
};


////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//								STRUCTURES.								  //
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//  Vec2Fx32: 2D Vector of dec32
//----------------------------------------------------------------------------
struct Vec2Fx32
{
	dec32    x;
	dec32    y;
};

//----------------------------------------------------------------------------
//  Vec2Fx16: 2D Vector of fx16
//----------------------------------------------------------------------------
struct Vec2Fx16
{
	fx16    x;
	fx16    y;
};


// Class: _Vector2
//
template <class T>class _Vector2
{
public:

	typedef T ElemType;

	static const _Vector2 Zero;
	static const _Vector2 One;
	static const _Vector2 UnitX;
	static const _Vector2 UnitY;
	static const _Vector2 MaxVal;
	static const _Vector2 MinVal;

public:

	// Don't initialize, otherwise we end up with usually unnecessary overhead which can cripple the system
	// (I've seen this actually end up taking over 1/5th of the frame)
	_Vector2() {}


	inline _Vector2( T nx, T ny)
		: x ( nx )
		, y ( ny )
	{}

	inline _Vector2( T nx, T ny, T nz)
		: x ( nx )
		, y ( ny )
	{ASSERT(nz == 0);}


	inline _Vector2( Vec2Fx32 vec )
		: x ( vec.x )
		, y ( vec.y )
	{}


	inline _Vector2( const _Vector2& rhs )
		: x ( rhs.x )
		, y ( rhs.y )
	{}

/*	inline _Vector2(const _Point2D<T>& point, T depth)
		: x(point.x)
		, y(point.y)
	{ASSERT(depth == 0)}

	inline _Vector2(const _Point2D<T>& point)
		: x(point.x)
		, y(point.y)
	{}
*/
	template <class oT> _Vector2( const _Vector2<oT>& rhs )
		: x ( rhs.x )
		, y ( rhs.y )
	{}
	
	inline _Vector2& operator=(const _Vector2& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	
	template<typename InputType> inline _Vector2& operator=(const InputType& rhs)
	{
		return *this = ConvertFrom(rhs);
	}
	
	template<typename ReturnType> inline ReturnType ConvertTo() const
	{
		return ConvertTo<ReturnType>(*this);
	}
	
	template<typename InputType> static inline _Vector2 ConvertFrom(const InputType& rhs)
	{
		return Mortar::ConvertVector<_Vector2>(rhs);
	}
	
	template<typename ReturnType> static inline ReturnType ConvertTo(const _Vector2& input)
	{
		return Mortar::ConvertVector<ReturnType>(input);
	}

	~_Vector2() {}


	Vec2Fx32 Vec2Fx()
	{
		return *(Vec2Fx32*)this;
	}

/*
	platVector2 PlatformVec()
	{
		return *(platVector2*)this;
	}
*/

	T Magnitude() const;


	// Method: MagnitudeSqr
	//
	//		Returns the square of the magnitude of this vector.
	//
	T MagnitudeSqr() const;


	// Method: Normalise
	//
	//		Normalises the vector.
	//
	// Returns:
	//
	//		The original length (magnitude) of the vector.
	//
	float Normalise();

	inline _Vector2 Normalised() const
	{
		_Vector2 ans = *this;
		ans.Normalise();
		return ans;
	}


	// Method: Cross
	//
	//		Returns the cross product of this and another vector.
	//
	T Cross(const _Vector2& rhs) const;


	// Method: Dot
	//
	//		Returns the dot product of this and another vector.
	//
	T Dot(const _Vector2& rhs) const;


	// Method: Lerp
	//
	//		linearly interpolates to the vector based on time
	//		time should be between 0 - 1.0f
	//
	_Vector2 Lerp(const _Vector2 &rhs, const T time) const;

	// Method: Perpendicular
	//
	//		Returns a vector perpendicular to the current vector
	//
	_Vector2 Perpendicular() const;


	// Method: Magnitude
	//
	//		Returns the magnitude (length) of this vector.
	//
	static T Magnitude(const _Vector2& v);


	// Method: MagnitudeSqr
	//
	//		Returns the square of the magnitude of this vector.
	//
	static T MagnitudeSqr(const _Vector2& v);


	// Method: Normalise
	//
	//		Normalises the vector.
	//
	// Returns:
	//
	//		The normalised vector.
	//
	static _Vector2 Normalise(const _Vector2& v);


	// Method: Cross
	//
	//		Returns the cross product of lhs and rhs.
	//
	static T Cross(const _Vector2& lhs, const _Vector2& rhs);


	// Method: Dot
	//
	//		Returns the dot product of lhs and rhs.
	//
	static T Dot(const _Vector2& lhs, const _Vector2& rhs);


	// Method: Lerp
	//
	//		linearly interpolates between the two vectors based on time
	//		time should be between 0 - 1.0f
	//
	static _Vector2 Lerp(const _Vector2& lhs, const _Vector2& rhs, T time);


	// Method: ToString
	//
	//      Returns the object as a string.
	//
	const char *ToString() const;

	inline _Vector2 operator + ( const _Vector2& rhs ) const;
	inline _Vector2 operator + ( T rhs ) const;

	inline _Vector2  operator - ( const _Vector2& rhs ) const;
	inline _Vector2  operator - ( T rhs ) const;

	inline _Vector2  operator * ( const T& rhs ) const;
	inline _Vector2  operator * (const _Vector2& val) const;

	inline _Vector2  operator / ( const T& rhs ) const;

	inline _Vector2  operator << (const u32 val) const;

	inline _Vector2  operator >> (const u32 val) const;

	inline _Vector2& operator += ( const _Vector2& rhs);

	inline _Vector2& operator -= ( const _Vector2& rhs );

	inline _Vector2& operator *= ( const T val );

	inline _Vector2  operator *= ( const _Vector2& rhs );

	inline _Vector2& operator /= ( const T val );

	inline _Vector2& operator <<= (const u32 val);

	inline _Vector2& operator >>= (const u32 val);

	inline _Vector2  operator - () const;

	inline bool operator == (const _Vector2 &v) const;

	inline bool operator != (const _Vector2 &v) const;

	inline bool Equals(const _Vector2 &v, float tolerance) const;
	static inline bool Equals(const _Vector2 &lhs, const _Vector2 &rhs, float tolerance) { return lhs.Equals(rhs, tolerance); }

	// This is only useful when sorting a list of vectors
	// orders by x, then y
	inline bool operator <(const _Vector2 &rhs) const { return CompareLess(*this, rhs); }
	static inline bool CompareLess(const _Vector2 &lhs, const _Vector2 &rhs, float tolerance = 1e-5f)
	{
		if(ABS(lhs.x - rhs.x) > tolerance)
			return lhs.x < rhs.x;
		if(ABS(lhs.y - rhs.y) > tolerance)
			return lhs.y < rhs.y;
		return false;
	}

	/// <summary>
	/// Returns the minimum of two Vector2's.
	/// </summary>
	static inline _Vector2 Min(const _Vector2& a, const _Vector2& b)
	{
		return _Vector2(std::min(a.x, b.x), std::min(a.y, b.y));
	}

	/// <summary>
	/// Returns the maximum of two Vector2's.
	/// </summary>
	static inline _Vector2 Max(const _Vector2& a, const _Vector2& b)
	{
		return _Vector2(std::max(a.x, b.x), std::max(a.y, b.y));
	}

	// Data is public for quicker access.
public:

	union
	{
		struct
		{
			T	element[2];
		};

		struct
		{
			T	x;
			T	y;
		};
	};

};


template <class T> inline _Vector2<T> operator + ( const T& lhs, const _Vector2<T>& rhs);
template <class T> inline _Vector2<T> operator - ( const T& lhs, const _Vector2<T>& rhs);
template <class T> inline _Vector2<T> operator * ( const T& lhs, const _Vector2<T>& rhs);
template <class T> inline _Vector2<T> operator / ( const T& lhs, const _Vector2<T>& rhs);
/*
#include "../File/DataStreamReader.h"
#include "../File/DataStreamWriter.h"

namespace Mortar
{
	template <class T>
	void Read(DataStreamReader &reader, _Vector2<T> &value) { reader >> value.x >> value.y; }

	template <class T>
	void Write(DataStreamWriter &writer, const _Vector2<T> &value) { writer << value.x << value.y; }
}
*/
namespace Math
{
		/// <summary>
	/// Returns the minimum of two Vector2's.
	/// </summary>
	template <class T>
	static inline _Vector2<T> Min(const _Vector2<T>& a, const _Vector2<T>& b)
	{
		return _Vector2<T>::Min(a, b);
	}

	/// <summary>
	/// Returns the maximum of two Vector2's.
	/// </summary>
	template <class T>
	static inline _Vector2<T> Max(const _Vector2<T>& a, const _Vector2<T>& b)
	{
		return _Vector2<T>::Max(a, b);
	}
}

#include "Vector2.inl"

typedef _Vector2<dec32> Vector2;
typedef _Vector2<kf32>  Vector2f;
typedef _Vector2<fx32>  Vector2fx;

namespace Mortar
{
	template<> inline Vector2 ConvertVector<Vector2,Vector2>(const Vector2& input)
	{
		return input;
	}
	
	template<> inline Vector2fx ConvertVector<Vector2fx,Vector2fx>(const Vector2fx& input)
	{
		return input;
	}
	
	template<> inline Vector2 ConvertVector<Vector2,Vector2fx>(const Vector2fx& input)
	{
		return Vector2((dec32)input.x, (dec32)input.y);
	}
	
	template<> inline Vector2fx ConvertVector<Vector2fx,Vector2>(const Vector2& input)
	{
		return Vector2fx((fx32)input.x, (fx32)input.y);
	}
}

#endif // _VECTOR2_H
