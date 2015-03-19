//#include "../File/StdCStringFunctions.h"

template <class T> const _Vector2<T> _Vector2<T>::One  = _Vector2<T>(1, 1);
template <class T> const _Vector2<T> _Vector2<T>::Zero = _Vector2<T>(0, 0); 
template <class T> const _Vector2<T> _Vector2<T>::UnitY = _Vector2<T>(0, 1); 
template <class T> const _Vector2<T> _Vector2<T>::UnitX = _Vector2<T>(1, 0);
template <class T> const _Vector2<T> _Vector2<T>::MaxVal = _Vector2<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
template <class T> const _Vector2<T> _Vector2<T>::MinVal = _Vector2<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::min());


template <class T> _Vector2<T> _Vector2<T>::operator + ( const _Vector2<T>& rhs ) const
{
	return _Vector2<T>( x + rhs.x, y + rhs.y );
}

template <class T> _Vector2<T> _Vector2<T>::operator + ( T rhs ) const
{
	return _Vector2<T>( x + rhs, y + rhs );
}


// Operator -
//
template <class T> _Vector2<T> _Vector2<T>::operator - ( const _Vector2<T>& rhs ) const
{
	return _Vector2<T>( x - rhs.x, y - rhs.y );
}

template <class T> _Vector2<T> _Vector2<T>::operator - ( T rhs ) const
{
	return _Vector2<T>( x - rhs, y - rhs );
}


// Operator *
//
template <class T> _Vector2<T> _Vector2<T>::operator * ( const T& rhs ) const
{
	return	_Vector2<T>
		(
		x * rhs,
		y * rhs
		);
}

template <class T> _Vector2<T> _Vector2<T>::operator * (const _Vector2<T>& val) const
{
	return _Vector2<T>
		(
		x * val.x,
		y * val.y
		);
}


// Operator >>/<<
//
template <class T> _Vector2<T> _Vector2<T>::operator >> ( const u32 val ) const
{
	return	_Vector2<T>
		(
		x>>val,
		y>>val
		);
}


template <class T> _Vector2<T> _Vector2<T>::operator << ( const u32 val ) const
{
	return	_Vector2<T>
		(
		x<<val,
		y<<val
		);
}


// Operator /
//
template <class T> _Vector2<T> _Vector2<T>::operator / ( const T& rhs ) const
{
	return	_Vector2<T>
		(
		x / rhs,
		y / rhs
		);
}



// Operator +=
//
template <class T> _Vector2<T>& _Vector2<T>::operator += (const _Vector2<T>& rhs )
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}


// Operator >>=/<<=
//
template <class T> _Vector2<T>& _Vector2<T>::operator >>= (const u32 val )
{
	x >>= val;
	y >>= val;

	return *this;
}

template <class T> _Vector2<T>& _Vector2<T>::operator <<= (const u32 val )
{
	x <<= val;
	y <<= val;

	return *this;
}


// Operator -=
//
template <class T> _Vector2<T>& _Vector2<T>::operator -= ( const _Vector2<T>& rhs )
{
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}


// Operator *=
//
template <class T> _Vector2<T>& _Vector2<T>::operator *= ( const T val )
{
	x *= val;
	y *= val;

	return *this;
}

template <class T> _Vector2<T> _Vector2<T>::operator *= (const _Vector2<T>& val)
{
	x *= val.x;
	y *= val.y;

	return *this;
}


// Operator /=
//
template <class T> _Vector2<T>& _Vector2<T>::operator /= ( const T val )
{
	x /= val;
	y /= val;

	return *this;
}


// Operator -
//
template <class T> _Vector2<T> _Vector2<T>::operator - () const
{
	return _Vector2<T>(-x, -y);
}


// Operator ==
//
template <class T> bool _Vector2<T>::operator == (const _Vector2<T> &v) const
{
	return (x == v.x && y == v.y);
}


// Operator !=
//
template <class T> bool _Vector2<T>::operator != (const _Vector2<T> &v) const
{
	return (x != v.x || y != v.y );
}

template <class T> bool _Vector2<T>::Equals(const _Vector2 &v, float tolerance) const
{
	return
		BETWEEN(x-v.x, -tolerance, tolerance) &&
		BETWEEN(y-v.y, -tolerance, tolerance);
}


// Magnitude
//
template <class T> T _Vector2<T>::Magnitude() const
{
	return std::sqrt(MagnitudeSqr());
}


// MagnitudeSqr
//
template <class T> T _Vector2<T>::MagnitudeSqr() const
{
	return Dot(*this);
}


// Normalise
//
template <class T> float _Vector2<T>::Normalise()
{
	if(x == 0 && y == 0)
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
template <class T> T _Vector2<T>::Cross(const _Vector2<T>& rhs) const
{
	return (x * rhs.y) - (y * rhs.x);
}


// Dot
//
template <class T> T _Vector2<T>::Dot(const _Vector2<T>& rhs) const
{
	return (x * rhs.x) + (y * rhs.y);
}


// Lerp
//
template <class T> _Vector2<T> _Vector2<T>::Lerp(const _Vector2<T>& rhs, const T time) const
{
	_Vector2<T> tmp = (rhs - *this) * time;
	return *this + tmp;
}

// Perpendicular
//
template <class T> _Vector2<T> _Vector2<T>::Perpendicular() const
{
	return _Vector2<T>(-y, x);
}

// Magnitude
//
template <class T> T _Vector2<T>::Magnitude(const _Vector2<T>& v)
{
	return v.Magnitude();
}


// MagnitudeSqr
//
template <class T> T _Vector2<T>::MagnitudeSqr(const _Vector2<T>& v)
{
	return v.MagnitudeSqr();
}


// Normalise
//
template <class T> _Vector2<T> _Vector2<T>::Normalise(const _Vector2<T>& v)
{
	//modify by frank.cheng at 2015.02.03 这里编译不过修改一下
	//Vector2 normalise = v; 
	_Vector2<T> normalise = v; 
	normalise.Normalise();
	return normalise;
}


// Cross
//
template <class T> T _Vector2<T>::Cross(const _Vector2<T>& lhs, const _Vector2<T>& rhs)
{
	return lhs.Cross(rhs);
}


// Dot
//
template <class T> T _Vector2<T>::Dot(const _Vector2<T>& lhs, const _Vector2<T>& rhs)
{
	return lhs.Dot(rhs);
}


// Lerp
//
template <class T> _Vector2<T> _Vector2<T>::Lerp(const _Vector2<T>& lhs, const _Vector2<T>& rhs, T time)
{
	return lhs.Lerp(rhs, time);
}


template <class T> inline _Vector2<T> operator + ( const T& lhs, const _Vector2<T>& rhs) { return _Vector2<T>(lhs + rhs.x, lhs + rhs.y); }
template <class T> inline _Vector2<T> operator - ( const T& lhs, const _Vector2<T>& rhs) { return _Vector2<T>(lhs - rhs.x, lhs - rhs.y); }
template <class T> inline _Vector2<T> operator * ( const T& lhs, const _Vector2<T>& rhs) { return _Vector2<T>(lhs * rhs.x, lhs * rhs.y); }
template <class T> inline _Vector2<T> operator / ( const T& lhs, const _Vector2<T>& rhs) { return _Vector2<T>(lhs / rhs.x, lhs / rhs.y); }

// Returns the points data as a string.
//
template <class T> const char *_Vector2<T>::ToString() const
{
	static char buffer[256];
/*	
#if defined(PLATFORM_PC) || defined(PLATFORM_360)
	sprintf_s(buffer, "%i, %i", (int)x, (int)y);
#elif defined(PLATFORM_BADA)
    OS_snprintf(buffer, sizeof(buffer), "%i, %i", (int)x, (int)y);
#else
    OS_sprintf(buffer, "%i, %i", (int)x, (int)y);
#endif
*/
	snprintf(buffer, sizeof(buffer), "%i, %i", (int)x, (int)y);
	return buffer;
}
