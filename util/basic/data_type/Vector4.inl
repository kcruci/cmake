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

template <class T> const _Vector4<T> _Vector4<T>::One = _Vector4<T>(1.0f,1.0f,1.0f,1.0f);
template <class T> const _Vector4<T> _Vector4<T>::Zero = _Vector4<T>(0.0f,0.0f,0.0f,0.0f); 


template <class T> _Vector4<T> _Vector4<T>::operator + ( const _Vector4<T>& rhs ) const
{
	return _Vector4<T>( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w );
}

template <class T> _Vector4<T> _Vector4<T>::operator + ( T rhs ) const
{
	return _Vector4<T>( x + rhs, y + rhs, z + rhs, w + rhs );
}



// Operator -
//
template <class T> _Vector4<T> _Vector4<T>::operator - ( const _Vector4<T>& rhs ) const
{
	return _Vector4<T>( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w );
}

template <class T> _Vector4<T> _Vector4<T>::operator - ( T rhs ) const
{
	return _Vector4<T>( x - rhs, y - rhs, z - rhs, w - rhs );
}



// Operator *
//
template <class T> _Vector4<T> _Vector4<T>::operator * ( const T& rhs ) const
{
	return	_Vector4<T>
		(
		x * rhs,
		y * rhs,
		z * rhs,
		w * rhs
		);
}

template <class T> _Vector4<T> _Vector4<T>::operator * (const _Vector4<T>& val) const
{
	return _Vector4<T>
		(
		x * val.x,
		y * val.y,
		z * val.z,
		w * val.w
		);
}

// Operator >>/<<
template <class T> _Vector4<T> _Vector4<T>::operator >> ( const u32 val ) const
{
	return	_Vector4<T>
		(
		x>>val,
		y>>val,
		z>>val,
		w>>val
		);
}


template <class T> _Vector4<T> _Vector4<T>::operator << ( const u32 val ) const
{
	return	_Vector4<T>
		(
		x<<val,
		y<<val,
		z<<val,
		w<<val
		);
}

// Operator /
//

template <class T> _Vector4<T> _Vector4<T>::operator / ( const T& rhs ) const
{
	T inverse = 1 / rhs;
	return	_Vector4<T>
		(
		x * inverse,
		y * inverse,
		z * inverse,
		w * inverse
		);
}



// Operator +=
//
template <class T> _Vector4<T>& _Vector4<T>::operator += (const _Vector4<T>& rhs )
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;

	return *this;
}

// Operator >>=/<<=

template <class T> _Vector4<T>& _Vector4<T>::operator >>= (const u32 val )
{
	x >>= val;
	y >>= val;
	z >>= val;
	w >>= val;

	return *this;
}

template <class T> _Vector4<T>& _Vector4<T>::operator <<= (const u32 val )
{
	x <<= val;
	y <<= val;
	z <<= val;
	w <<= val;

	return *this;
}



// Operator -=
//
template <class T> _Vector4<T>& _Vector4<T>::operator -= ( const _Vector4<T>& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;

	return *this;
}



// Operator *=
//
template <class T> _Vector4<T>& _Vector4<T>::operator *= ( const T val )
{
	x *= val;
	y *= val;
	z *= val;
	w *= val;

	return *this;
}

template <class T> _Vector4<T> _Vector4<T>::operator *= (const _Vector4<T>& val)
{
	x *= val.x;
	y *= val.y;
	z *= val.z;
	w *= val.w;

	return *this;
}


// Operator /=
template <class T> _Vector4<T>& _Vector4<T>::operator /= ( const T val )
{
	*this *= 1 / val;

	return *this;
}



// Operator -
//
template <class T> _Vector4<T> _Vector4<T>::operator - () const
{
	return _Vector4<T>(-x, -y, -z, -w);
}



// Operator ==
template <class T> bool _Vector4<T>::operator == (const _Vector4<T> &v) const
{
	return (x == v.x && y == v.y && z == v.z && w == v.w);
}


// Operator !=
template <class T> bool _Vector4<T>::operator != (const _Vector4<T> &v) const
{
	return (x != v.x || y != v.y || z != v.z || w != v.w);
}

template <class T> bool _Vector4<T>::Equals(const _Vector4 &v, float tolerance) const
{
	return
		BETWEEN(x-v.x, -tolerance, tolerance) &&
		BETWEEN(y-v.y, -tolerance, tolerance) &&
		BETWEEN(z-v.z, -tolerance, tolerance) &&
		BETWEEN(w-v.w, -tolerance, tolerance) ;
}

// Magnitude
//
template <class T> T _Vector4<T>::Magnitude() const
{
	return std::sqrt(MagnitudeSqr());
}



// MagnitudeSqr
//
template <class T> T _Vector4<T>::MagnitudeSqr() const
{
	return Dot(*this);
}



// Normalise
//
template <class T> float _Vector4<T>::Normalise()
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




// Dot
//
template <class T> T _Vector4<T>::Dot(const _Vector4<T>& rhs) const
{
	return (x * rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w);
}



// Lerp
//
template <class T> _Vector4<T> _Vector4<T>::Lerp(const _Vector4<T>& rhs, const T time) const
{
	_Vector4<T> tmp = (rhs - *this) * time;
	return *this + tmp;
}



//
// Static Functions
//


// Magnitude
//
template <class T> T _Vector4<T>::Magnitude(_Vector4<T> v)
{
	return v.Magnitude();
}



// MagnitudeSqr
//
template <class T> T _Vector4<T>::MagnitudeSqr(_Vector4<T> v)
{
	return v.MagnitudeSqr();
}



// Normalise
//
template <class T> _Vector4<T> _Vector4<T>::Normalise(_Vector4<T> v)
{
	v.Normalise();
	return v;
}



// Dot
//
template <class T> T _Vector4<T>::Dot(_Vector4<T> lhs, _Vector4<T> rhs)
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
template <class T> _Vector4<T> _Vector4<T>::Lerp(_Vector4<T> lhs, _Vector4<T> rhs, T time)
{
	return lhs.Lerp(rhs, time);
}



// Returns the points data as a string.
//
template <class T> const char *_Vector4<T>::ToString() const
{
	static char buffer[256];
/*	
#if defined(PLATFORM_PC) || defined(PLATFORM_360)
	sprintf_s(buffer, "%f, %f, %f, %f", (float)x, (float)y, (float)z, (float)w);
#elif defined(PLATFORM_BADA)
    OS_snprintf(buffer, sizeof(buffer), "%f, %f, %f, %f", (float)x, (float)y, (float)z, (float)w);
#else
	OS_sprintf(buffer, "%f, %f, %f, %f", (float)x, (float)y, (float)z, (float)w);
#endif
*/
	snprintf(buffer, sizeof(buffer), "%f, %f, %f, %f", (float)x, (float)y, (float)z, (float)w);
	return buffer;
}

