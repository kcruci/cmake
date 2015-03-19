// ------------------------------------------------------------------------------------------------
//
//	File: Vector3.h
//
//		Description		- Implementation of a x,y,z,w vector class
//							(not quaternion, used for matrix transforms)
//		Author			- Grant Peters
//
//	Modifications:
//	--------------
//		Sun 15, 2009	- Initial creation.
//
// ------------------------------------------------------------------------------------------------

#ifndef __VECTOR4_H
#define __VECTOR4_H

//////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//								INCLUDES.								//
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////

#include "BaseTypes.h"
#include "Vector3.h"

//////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//								DEFINES.								//
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//								STRUCTURES.								//
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////

// Class: _Vector4
//
template <class T>class _Vector4
{
public:

	typedef T ElemType;

	static const _Vector4 Zero;
	static const _Vector4 One;

public:

	// Don't initialize, otherwise we end up with usually unnessecary overhead which can cripple the system
	// (I've seen this actually end up taking over 1/5th of the frame)
	_Vector4(){}


	_Vector4( T nx, T ny, T nz, T nw = 1)
		: x ( nx )
		, y ( ny )
		, z ( nz )
		, w ( nw )
	{}


	_Vector4( const _Vector4& rhs )
		: x ( rhs.x )
		, y ( rhs.y )
		, z ( rhs.z )
		, w ( rhs.w )
	{}
	_Vector4( const _Vector3<T>& rhs, T nw = 1 )
		: x ( rhs.x )
		, y ( rhs.y )
		, z ( rhs.z )
		, w ( nw )
	{}
	
	template <class oT> _Vector4(const oT rhs[4])
		: x ( (T)rhs[0] )
		, y ( (T)rhs[1] )
		, z ( (T)rhs[2] )
		, w ( (T)rhs[3] )
	{
	}

	template <class oT> _Vector4( const _Vector4<oT>& rhs )
		: x ( rhs.x )
		, y ( rhs.y )
		, z ( rhs.z )
		, w ( rhs.w )
	{}

	inline _Vector4& operator=(const _Vector4& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
		return *this;
	}
	
	template<typename InputType> inline _Vector4& operator=(const InputType& rhs)
	{
		return *this = ConvertFrom(rhs);
	}
	
	template<typename ReturnType> inline ReturnType ConvertTo() const
	{
		return ConvertTo<ReturnType>(*this);
	}
	
	template<typename InputType> static inline _Vector4 ConvertFrom(const InputType& rhs)
	{
		return Mortar::ConvertVector<_Vector4>(rhs);
	}
	
	template<typename ReturnType> static inline ReturnType ConvertTo(const _Vector4& input)
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


	// Method: Dot
	//
	//		Returns the dot product of this and another vector.
	//
	T Dot(const _Vector4& rhs) const;


	// Method: Lerp
	//
	//		linearly interpolates to the vector based on time
	//		time should be between 0 - 1.0f
	//
	_Vector4 Lerp(const _Vector4 &rhs, const T time) const;

	//
	// Static Functions
	//


	// Method: Magnitude
	//
	//		Returns the magnitude (length) of this vector.
	//
	static T Magnitude(_Vector4 v);


	// Method: MagnitudeSqr
	//
	//		Returns the square of the magnitude of this vector.
	//
	static T MagnitudeSqr(_Vector4 v);


	// Method: Normalise
	//
	//		Normalises the vector.
	//
	// Returns:
	//
	//		The normalised vector.
	//
	static _Vector4 Normalise(_Vector4 v);


	// Method: Dot
	//
	//		Returns the dot product of lhs and rhs.
	//
	static T Dot(_Vector4 lhs, _Vector4 rhs);


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
	static _Vector4 Lerp(_Vector4 lhs, _Vector4 rhs, T time);


	// Method: ToString
	//
	//      Returns the object as a string.
	//
	const char *ToString() const;


	inline _Vector4 operator + ( const _Vector4& rhs ) const;
	inline _Vector4 operator + ( T rhs ) const;


	inline _Vector4 operator - ( const _Vector4& rhs ) const;
	inline _Vector4 operator - ( T rhs ) const;


	inline _Vector4 operator * ( const T& rhs ) const;

	inline _Vector4 operator * (const _Vector4& val) const;


	inline _Vector4 operator / ( const T& rhs ) const;

	inline _Vector4 operator << (const u32 val) const;

	inline _Vector4 operator >> (const u32 val) const;


	inline _Vector4& operator += ( const _Vector4& rhs);


	inline _Vector4& operator -= ( const _Vector4& rhs );


	inline _Vector4& operator *= ( const T val );

	inline _Vector4 operator *= ( const _Vector4& rhs );

	inline _Vector4& operator /= ( const T val );

	inline _Vector4& operator <<= (const u32 val);

	inline _Vector4& operator >>= (const u32 val);


	inline _Vector4 operator - () const;


	inline bool operator == (const _Vector4 &v) const;

	inline bool operator != (const _Vector4 &v) const;

	inline bool Equals(const _Vector4 &v, float tolerance) const;

	static inline _Vector4 Min(const _Vector4 &a, const _Vector4 &b) { return _Vector4(MIN(a.x,b.x),MIN(a.y,b.y),MIN(a.z,b.z),MIN(a.w,b.w)); }
	static inline _Vector4 Max(const _Vector4 &a, const _Vector4 &b) { return _Vector4(MAX(a.x,b.x),MAX(a.y,b.y),MAX(a.z,b.z),MAX(a.w,b.w)); }

	// Data is public for quicker access.
public:

	union
	{
		struct
		{
			T	element[4];
		};

		struct
		{
			T	x, y, z, w;
		};
	};

};
/*
#include "../File/DataStreamReader.h"
#include "../File/DataStreamWriter.h"

namespace Mortar
{
	template <class T>
	void Read(DataStreamReader &reader, _Vector4<T> &value) { reader >> value.x >> value.y >> value.z >> value.w; }
	
	template <class T>
	void Write(DataStreamWriter &writer, const _Vector4<T> &value) { writer << value.x << value.y << value.z << value.w; }
}
*/
#include "Vector4.inl"


typedef _Vector4<dec32> Vector4;
typedef _Vector4<kf32> Vector4f;
typedef _Vector4<fx32> Vector4fx;


namespace Mortar {
	template<> inline Vector4 ConvertVector<Vector4,Vector4>(const Vector4& input)
	{
		return input;
	}
	
	template<> inline Vector4fx ConvertVector<Vector4fx,Vector4fx>(const Vector4fx& input)
	{
		return input;
	}
	
	template<> inline Vector4 ConvertVector<Vector4,Vector4fx>(const Vector4fx& input)
	{
		return Vector4((dec32)input.x, (dec32)input.y, (dec32)input.z, (dec32)input.w);
	}
	
	template<> inline Vector4fx ConvertVector<Vector4fx,Vector4>(const Vector4& input)
	{
		return Vector4fx((fx32)input.x, (fx32)input.y, (fx32)input.z, (fx32)input.w);
	}
}

#endif // _VECTOR4_H
