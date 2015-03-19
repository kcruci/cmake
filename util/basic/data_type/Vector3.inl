// ------------------------------------------------------------------------------------------------
//
//  File: Vector3.inl
//
//      Description     - Implementation of the inline Vector3 functions
//      Author          - Aidan Millott
//
//  Modifications:
//  --------------
//      July 29, 2009   - Initial creation.
//		July 30, 2009	- Woody Updated to templates
//
// ------------------------------------------------------------------------------------------------

// Operator +
//
//#include "../File/StdCStringFunctions.h"

template <class T> const _Vector3<T> _Vector3<T>::One = _Vector3<T>(1.0f,1.0f,1.0f);
template <class T> const _Vector3<T> _Vector3<T>::Zero = _Vector3<T>(0.0f,0.0f,0.0f); 
template <class T> const _Vector3<T> _Vector3<T>::UnitX = _Vector3<T>(1.0f,0.0f,0.0f); 
template <class T> const _Vector3<T> _Vector3<T>::UnitY = _Vector3<T>(0.0f,1.0f,0.0f); 
template <class T> const _Vector3<T> _Vector3<T>::UnitZ = _Vector3<T>(0.0f,0.0f,1.0f);
template <class T> const _Vector3<T> _Vector3<T>::MaxVal = _Vector3<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
template <class T> const _Vector3<T> _Vector3<T>::MinVal = _Vector3<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::min(), std::numeric_limits<T>::min());


template <class T> _Vector3<T> _Vector3<T>::operator + ( const _Vector3<T>& rhs ) const
{
	return _Vector3<T>( x + rhs.x, y + rhs.y, z + rhs.z );
}

template <class T> _Vector3<T> _Vector3<T>::operator + ( T rhs ) const
{
	return _Vector3<T>( x + rhs, y + rhs, z + rhs );
}



// Operator -
//
template <class T> _Vector3<T> _Vector3<T>::operator - ( const _Vector3<T>& rhs ) const
{
	return _Vector3<T>( x - rhs.x, y - rhs.y, z - rhs.z );
}

template <class T> _Vector3<T> _Vector3<T>::operator - ( T rhs ) const
{
	return _Vector3<T>( x - rhs, y - rhs, z - rhs );
}



// Operator *
//
template <class T> _Vector3<T> _Vector3<T>::operator * ( const T& rhs ) const
{
	return	_Vector3<T>
		(
		x * rhs,
		y * rhs,
		z * rhs
		);
}

template <class T> _Vector3<T> _Vector3<T>::operator * (const _Vector3<T>& val) const
{
	return _Vector3<T>
		(
		x * val.x,
		y * val.y,
		z * val.z
		);
}

// Operator >>/<<
template <class T> _Vector3<T> _Vector3<T>::operator >> ( const u32 val ) const
{
	return	_Vector3<T>
		(
		x>>val,
		y>>val,
		z>>val
		);
}


template <class T> _Vector3<T> _Vector3<T>::operator << ( const u32 val ) const
{
	return	_Vector3<T>
		(
		x<<val,
		y<<val,
		z<<val
		);
}

// Operator /
//

template <class T> _Vector3<T> _Vector3<T>::operator / ( const T& rhs ) const
{
	return	_Vector3<T>
		(
		x / rhs,
		y / rhs,
		z / rhs
		);
}



// Operator +=
//
template <class T> _Vector3<T>& _Vector3<T>::operator += (const _Vector3<T>& rhs )
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

// Operator >>=/<<=

template <class T> _Vector3<T>& _Vector3<T>::operator >>= (const u32 val )
{
	x >>= val;
	y >>= val;
	z >>= val;

	return *this;
}

template <class T> _Vector3<T>& _Vector3<T>::operator <<= (const u32 val )
{
	x <<= val;
	y <<= val;
	z <<= val;

	return *this;
}



// Operator -=
//
template <class T> _Vector3<T>& _Vector3<T>::operator -= ( const _Vector3<T>& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}



// Operator *=
//
template <class T> _Vector3<T>& _Vector3<T>::operator *= ( const T val )
{
	x *= val;
	y *= val;
	z *= val;

	return *this;
}

template <class T> _Vector3<T> _Vector3<T>::operator *= (const _Vector3<T>& val)
{
	x *= val.x;
	y *= val.y;
	z *= val.z;

	return *this;
}


// Operator /=
template <class T> _Vector3<T>& _Vector3<T>::operator /= ( const T val )
{
	x /= val;
	y /= val;
	z /= val;

	return *this;
}



// Operator -
//
template <class T> _Vector3<T> _Vector3<T>::operator - () const
{
	return _Vector3<T>(-x, -y, -z);
}



// Operator ==
template <class T> bool _Vector3<T>::operator == (const _Vector3<T> &v) const
{
	return (x == v.x && y == v.y && z == v.z);
}


// Operator !=
template <class T> bool _Vector3<T>::operator != (const _Vector3<T> &v) const
{
	return (x != v.x || y != v.y || z != v.z);
}

template <class T> bool _Vector3<T>::Equals(const _Vector3 &v, float tolerance) const
{
	return
		BETWEEN(x-v.x, -tolerance, tolerance) &&
		BETWEEN(y-v.y, -tolerance, tolerance) &&
		BETWEEN(z-v.z, -tolerance, tolerance) ;
}

//Operator []
template <class T> T& _Vector3<T>::operator [] (const int index)
{
	return element[index];
}

// Magnitude
//
template <class T> T _Vector3<T>::Magnitude() const
{
	//return Math::Sqrt(MagnitudeSqr());
	return std::sqrt(MagnitudeSqr());
}



// MagnitudeSqr
//
template <class T> T _Vector3<T>::MagnitudeSqr() const
{
	return Dot(*this);
}



// Normalise
//
template <class T> float _Vector3<T>::Normalise()
{
	if(x == 0 && y == 0 && z == 0)
		return 0;

	T denom = Magnitude();
	if(denom != 0)
	{
		*this /= denom;
	}
	else
	{
		//wasn't 0, but was very small, multiply by a large number
		*this *= 1000000;
		Normalise();
	}

	return denom;
}



// Cross
//
template <class T> _Vector3<T> _Vector3<T>::Cross(const _Vector3<T>& rhs) const
{
	_Vector3<T> res;

	res.x = (y * rhs.z) - (rhs.y * z);
	res.y = (z * rhs.x) - (rhs.z * x);
	res.z = (x * rhs.y) - (rhs.x * y);

	return res;
}



// Dot
//
template <class T> T _Vector3<T>::Dot(const _Vector3<T>& rhs) const
{
	return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
}



// MultMtx
//
/*_Vector3<T> Vector3::MultMtx(const Mtx33& rhs) const
{
VecFx32 tmp;

VEC_Set(&tmp, x, y, z);

MTX_MultVec33(&tmp, &rhs, &tmp);

return Vector3(tmp.x, tmp.y, tmp.z);
}*/


// Lerp
//
template <class T> _Vector3<T> _Vector3<T>::Lerp(const _Vector3<T>& rhs, const T time) const
{
	_Vector3<T> tmp = (rhs - *this) * time;
	return *this + tmp;
}



//
// Static Functions
//


// Magnitude
//
template <class T> T _Vector3<T>::Magnitude(const _Vector3<T>& v)
{
	return v.Magnitude();
}



// MagnitudeSqr
//
template <class T> T _Vector3<T>::MagnitudeSqr(const _Vector3<T>& v)
{
	return v.MagnitudeSqr();
}



// Normalise
//
template <class T> _Vector3<T> _Vector3<T>::Normalise(_Vector3<T> v)
{
	v.Normalise();
	return v;
}



// Cross
//
template <class T> _Vector3<T> _Vector3<T>::Cross(const _Vector3<T>& lhs, const _Vector3<T>& rhs)
{
	return lhs.Cross(rhs);
}



// Dot
//
template <class T> T _Vector3<T>::Dot(const _Vector3<T>& lhs, const _Vector3<T>& rhs)
{
	return lhs.Dot(rhs);
}



// MultMtx
//
//Vector3 Vector3::MultMtx(Vector3 v, Mtx33 m)
//{
//	VecFx32 tmp = *(VecFx32 *)(&v);
//
//	MTX_MultVec33(&tmp, &m, &tmp);
//
//	return Vector3(tmp);
//}



// Lerp
//
template <class T> _Vector3<T> _Vector3<T>::Lerp(const _Vector3<T>& lhs, const _Vector3<T>& rhs, T time)
{
	return lhs.Lerp(rhs, time);
}



// Returns the points data as a string.
//
template <class T> const char *_Vector3<T>::ToString() const
{
	static char buffer[256];
	/*
#if defined(PLATFORM_PC) || defined(PLATFORM_360)
	sprintf_s(buffer, "%i, %i, %i", (int)x, (int)y, (int)z);
#elif(PLATFORM_BADA)
	OS_snprintf(buffer, sizeof(buffer), "%i, %i, %i", (int)x, (int)y, (int)z);
#else
	OS_sprintf(buffer, "%i, %i, %i", (int)x, (int)y, (int)z);
	//sprintf_s(buffer, "%i, %i, %i", (int)x, (int)y, (int)z);
#endif
*/
	snprintf(buffer, sizeof(buffer), "%i, %i, %i", (int)x, (int)y, (int)z);
	return buffer;
}

// CatmullRom
//
template <class T> _Vector3<T> _Vector3<T>::CatmullRom(const _Vector3<T> &p0, const _Vector3<T> &p1, const _Vector3<T> &p2, const _Vector3<T> &p3, T time)
{
	T time2 = time * time;
	T time3 = time2 * time;

	return ((p1 * 2) + (-p0 + p2) * time +
		(p0*2 - p1*5 + p2*4 - p3) * time2 +
		(-p0 + p1*3- p2*3 + p3) * time3) / 2;
}

//// Friends.
////
//ErrorLogManager& operator << (ErrorLogManager& elm, Vector3& vector)
//{
//    elm << vector.ToString();
//    return elm;
//}
//
//
//
//// Friends.
////
//ErrorLogManager& operator << (ErrorLogManager& elm, const Vector3& vector)
//{
//    elm << vector.ToString();
//    return elm;
//}
