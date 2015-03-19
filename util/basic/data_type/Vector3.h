// ------------------------------------------------------------------------------------------------
//
//  File: Vector3.h
//
//      Description     - Implementation of a x,y,z vector class
//
// ------------------------------------------------------------------------------------------------

#ifndef __VECTOR3_H
#define __VECTOR3_H

#include "BaseTypes.h"
//#include "Point2D.h"
#include "Vector2.h"

//----------------------------------------------------------------------------
//  VecFx32: 3D Vector of dec32
//----------------------------------------------------------------------------
typedef struct
{
	dec32    x;
	dec32    y;
	dec32    z;
}
VecFx32;

//----------------------------------------------------------------------------
//  VecFx16: 3D Vector of fx16
//----------------------------------------------------------------------------
typedef struct
{
	fx16    x;
	fx16    y;
	fx16    z;
}
VecFx16;

// Class: _Vector3
//
template <class T>
class _Vector3
{
public:
	typedef T ElemType;

	static const _Vector3 Zero;
	static const _Vector3 One;
	static const _Vector3 UnitX;
	static const _Vector3 UnitY;
	static const _Vector3 UnitZ;
	static const _Vector3 MaxVal;
	static const _Vector3 MinVal;

public:
	// Don't initialize, otherwise we end up with usually unnecessary overhead which can cripple the system
	// (I've seen this actually end up taking over 1/5th of the frame)
	_Vector3():\
		x(0),
		y(0),
		z(0)
	{}

	_Vector3(T nx, T ny, T nz) : \
		x(nx),
		y(ny),
		z(nz)
	{}

	_Vector3( VecFx32 vec ) : \
		x(vec.x),
		y(vec.y),
		z(vec.z)
	{}

	_Vector3(const _Vector3& rhs ) : \
		x( rhs.x ),
		y( rhs.y ),
		z( rhs.z )
	{}
/*
	_Vector3(const _Point2D<T>& point, T depth) : \
		x(point.x),
		y(point.y),
		z(depth)
	{}
*/
	template <class oT>
	_Vector3(const _Vector3<oT>& rhs ) : \
		x(rhs.x),
		y(rhs.y),
		z(rhs.z)
	{}

	template <class oT>
	_Vector3(const _Vector2<oT>& xy, float z) : \
		x(xy.x),
		y(xy.y),
		z(z)
	{}

	~_Vector3()
	{}

public:
	_Vector2<T> ToVector2() const
	{
		return _Vector2<T>(x,y);
	}

	static _Vector3 FromVector2(const _Vector2<T>& vec2)
	{
		return _Vector3(vec2.x, vec2.y, 0.0f);
	}

	static _Vector3 FromVector2(const _Vector2<T>& vec2, T z)
	{
		return _Vector3(vec2.x, vec2.y, z);
	}

	VecFx32 VecFx()
	{
		return *(VecFx32*)this;
	}
/*
	platVector3 PlatformVec()
	{
		return *(platVector3*)this;
	}
*/
	inline _Vector3& operator=(const _Vector3& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	template<typename InputType>
	inline _Vector3& operator=(const InputType& rhs)
	{
		return *this = ConvertFrom(rhs);
	}

	template<typename ReturnType>
	inline ReturnType ConvertTo() const
	{
		return ConvertTo<ReturnType>(*this);
	}

	template<typename InputType>
	static inline _Vector3 ConvertFrom(const InputType& rhs)
	{
		return Mortar::ConvertVector<_Vector3>(rhs);
	}

	template<typename ReturnType>
	static inline ReturnType ConvertTo(const _Vector3& input)
	{
		return Mortar::ConvertVector<ReturnType>(input);
	}

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

	// Method: Cross
	//
	//		Returns the cross product of this and another vector.
	//
	_Vector3 Cross(const _Vector3& rhs) const;

	// Method: Dot
	//
	//		Returns the dot product of this and another vector.
	//
	T Dot(const _Vector3& rhs) const;

	// Method: Lerp
	//
	//		linearly interpolates to the vector based on time
	//		time should be between 0 - 1.0f
	//
	_Vector3 Lerp(const _Vector3 &rhs, const T time) const;

	// Method: Get[A][B]()
	//
	//		returns the '[A]' & '[B]' components as a Vector2 (in the order they were given)
	//      e.g. GetXY() return - Vector2(x, y)
	//      e.g. GetYX() return - Vector2(y, x)
	//      e.g. GetYZ() return - Vector2(y, z)
	//
	_Vector2<T> GetXY() const { return _Vector2<T>(x,y); }
	_Vector2<T> GetYX() const { return _Vector2<T>(y,x); }
	_Vector2<T> GetXZ() const { return _Vector2<T>(x,z); }
	_Vector2<T> GetZX() const { return _Vector2<T>(z,x); }
	_Vector2<T> GetYZ() const { return _Vector2<T>(y,z); }
	_Vector2<T> GetZY() const { return _Vector2<T>(z,y); }

	//
	// Static Functions
	//

	// Method: Magnitude
	//
	//		Returns the magnitude (length) of this vector.
	//
	static T Magnitude(const _Vector3& v);


	// Method: MagnitudeSqr
	//
	//		Returns the square of the magnitude of this vector.
	//
	static T MagnitudeSqr(const _Vector3& v);


	// Method: Normalise
	//
	//		Normalises the vector.
	//
	// Returns:
	//
	//		The normalised vector.
	//
	static _Vector3 Normalise(_Vector3 v);


	// Method: Cross
	//
	//		Returns the cross product of lhs and rhs.
	//
	static _Vector3 Cross(const _Vector3& lhs, const _Vector3& rhs);


	// Method: Dot
	//
	//		Returns the dot product of lhs and rhs.
	//
	static T Dot(const _Vector3& lhs, const _Vector3& rhs);


	// Method: MultMtx
	//
	//		Multiplies a vector by a matrix and returns the result as a new vector.
	//
	//static Vector3 MultMtx(Vector3 v, Mtx33 m);

	// Method: Lerp
	//
	//		linearly interpolates between the two vectors based on time
	//		time should be between 0 - 1.0f
	//
	static _Vector3 Lerp(const _Vector3& lhs, const _Vector3& rhs, T time);


	// Method: ToString
	//
	//      Returns the object as a string.
	//
	const char *ToString() const;

	// Method: CatmullRom
	//
	//		
	//		
	//
	static _Vector3 CatmullRom(const _Vector3 &p0, const _Vector3 &p1, const _Vector3 &p2, const _Vector3 &p3, T time);	

	inline _Vector3 operator + ( const _Vector3& rhs ) const;
	inline _Vector3 operator + ( T rhs ) const;

	inline _Vector3 operator - ( const _Vector3& rhs ) const;
	inline _Vector3 operator - ( T rhs ) const;

	inline _Vector3 operator * ( const T& rhs ) const;

	inline _Vector3 operator * (const _Vector3& val) const;

	inline _Vector3 operator / ( const T& rhs ) const;

	inline _Vector3 operator << (const u32 val) const;

	inline _Vector3 operator >> (const u32 val) const;

	inline _Vector3& operator += ( const _Vector3& rhs);

	inline _Vector3& operator -= ( const _Vector3& rhs );

	inline _Vector3& operator *= ( const T val );

	inline _Vector3 operator *= ( const _Vector3& rhs );

	inline _Vector3& operator /= ( const T val );

	inline _Vector3& operator <<= (const u32 val);

	inline _Vector3& operator >>= (const u32 val);

	inline _Vector3 operator - () const;

	inline bool operator == (const _Vector3 &v) const;

	inline bool operator != (const _Vector3 &v) const;

	inline bool Equals(const _Vector3 &v, float tolerance) const;

	static inline _Vector3 Min(const _Vector3 &a, const _Vector3 &b) { return _Vector3(MIN(a.x,b.x),MIN(a.y,b.y),MIN(a.z,b.z)); }
	static inline _Vector3 Max(const _Vector3 &a, const _Vector3 &b) { return _Vector3(MAX(a.x,b.x),MAX(a.y,b.y),MAX(a.z,b.z)); }

	inline T& operator [] (const int index);
	// Data is public for quicker access.
public:

	union
	{
		struct
		{
			ElemType	element[3];
		};

		struct
		{
			ElemType	x;
			ElemType	y;
			ElemType	z;
		};
	};

};
/*
#include "../File/DataStreamReader.h"
#include "../File/DataStreamWriter.h"

namespace Mortar
{
	template <class T>
	void Read(DataStreamReader &reader, _Vector3<T> &value) { reader >> value.x >> value.y >> value.z; }

	template <class T>
	void Write(DataStreamWriter &writer, const _Vector3<T> &value) { writer << value.x << value.y << value.z; }
}
*/
#include "Vector3.inl"

typedef _Vector3<dec32> Vector3;
typedef _Vector3<kf32> Vector3f;
typedef _Vector3<fx32> Vector3fx;

namespace Mortar
{
	template<>
	inline Vector3 ConvertVector<Vector3, Vector3>(const Vector3& input)
	{
		return input;
	}

	template<>
	inline Vector3fx ConvertVector<Vector3fx, Vector3fx>(const Vector3fx& input)
	{
		return input;
	}

	template<>
	inline Vector3 ConvertVector<Vector3, Vector3fx>(const Vector3fx& input)
	{
		return Vector3((dec32)input.x, (dec32)input.y, (dec32)input.z);
	}

	template<>
	inline Vector3fx ConvertVector<Vector3fx, Vector3>(const Vector3& input)
	{
		return Vector3fx((fx32)input.x, (fx32)input.y, (fx32)input.z);
	}
}

#endif // _VECTOR3_H
