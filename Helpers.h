#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#ifndef _H_HELPERS
#define _H_HELPERS

#include <string>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <chrono>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

#ifndef FLT_EPSILON
#	define FLT_EPSILON 1.192092896e-07F 
#endif

inline double Sqr( double d ) {
	return d * d;
}

inline double Sign( double d ) {
	return (d > 0) ? 1 : -1;
}

inline bool IsEpsilon( double d ) {
	return abs( d ) <= FLT_EPSILON; 
}

inline double ToDegree( double rad ) {
	return rad / (2 * M_PI) * 360;
}

struct Degree {
	double degree;
	Degree( double rad ) : degree( ToDegree( rad ) ) {};
	operator double() const {
		return degree;
	}
};

template <typename T, typename T2>
bool HasElement( const T & in, const T2 & value ) {
	return std::find( in.begin(), in.end(), value ) != in.end();
}

template <typename T, typename Func>
void Filter( T & in, Func inFunc ) {
	in.erase( std::remove_if( in.begin(), in.end(), inFunc), in.end() );
}

template <typename T>
void Unique( T & in ) {
	in.erase( std::unique( in.begin(), in.end() ), in.end() );
}

typedef std::chrono::high_resolution_clock::time_point HrcTimePoint;
typedef std::chrono::high_resolution_clock Hrc;
typedef std::chrono::milliseconds Msec;
typedef std::chrono::microseconds Microsec;

class Timer {
	HrcTimePoint Start;
	Microsec TotalMicro = (Microsec)0;
	bool Active = false;
public:

	inline Timer() {
		//Total.count = Msec::zero;
	}
	inline void Begin() {
		Start = Hrc::now();
		Active = true;
	}
	inline void End() {
		TotalMicro += GetMicrosec();
		Active = false;
	}
	inline void Reset() {
		TotalMicro = (Microsec)0;
	}
	inline Msec GetMsec() const {
		return std::chrono::duration_cast<Msec>(Hrc::now() - Start);
	}
	inline Microsec GetMicrosec() const {
		return std::chrono::duration_cast<Microsec>(Hrc::now() - Start);
	}
	inline Msec GetTotalMsecs() const {
		return std::chrono::duration_cast<Msec>(TotalMicro);
	}
	inline Microsec GetTotalMicrosecs() const {
		return TotalMicro;
	}
};

class Perf {
public:

};


// static array
template <typename Type, int Size>
class Static {
	Type arr[Size];
	int num = 0;
public:
	Static() {

	}
	Static & operator+=( const Type & in ) {
		arr[num] = in;
		num++;
	}
	int size() const {
		return num;
	}

};

#undef min
#undef max

inline int minsgn( int a, int b ) {
	return abs( a ) < abs( b ) ? a : b;
}
inline int maxsgn( int a, int b ) {
	return abs( a ) > abs( b ) ? a : b;
}

inline double clamp( double in, double min, double max ) {
	if (in < min) return min;
	if (in > max) return max;
	return in;
}

inline double Lerp( double t, double d1, double d2 ) {
	return (1 - t)*d1 + t * d2;
}

inline double random( double min, double max ) {
	static const double RandMax = (double)RAND_MAX;
	return ((double)rand() / RandMax) * (min + (max - min));
}


class Vec3 {
public:
	double x, y, z;

	inline Vec3() {};
	inline Vec3( const Vec3 & in ) :x( in.x ), y( in.y ), z( in.z ) {};
	inline Vec3( double inX, double inY, double inZ ) :x( inX ), y( inY ), z( inZ ) {};

	inline Vec3 operator+( const Vec3 & in ) const {
		return Vec3( x + in.x, y + in.y, z + in.z );
	}
	inline Vec3 operator-( const Vec3 & in ) const {
		return Vec3( x - in.x, y - in.y, z - in.z );
	}
	inline Vec3 operator*( const Vec3 & in ) const {
		return Vec3( x * in.x, y * in.y, z * in.z );
	}
	inline Vec3 operator/( const Vec3 & in ) const {
		return Vec3( x / in.x, y / in.y, z / in.z );
	}
	inline Vec3 operator+( double in ) const {
		return Vec3( x + in, y + in, z + in );
	}
	inline Vec3 operator-( double in ) const {
		return Vec3( x - in, y - in, z - in );
	}
	inline Vec3 operator*( double in ) const {
		return Vec3( x * in, y * in, z * in );
	}
	inline Vec3 operator/( double in ) const {
		return Vec3( x / in, y / in, z / in );
	}
	inline Vec3 & operator+=( const Vec3 & in ) {
		x += in.x; y += in.y; z += in.z;
		return *this;
	}
	inline Vec3 & operator-=( const Vec3 & in ) {
		x -= in.x; y -= in.y; z -= in.z;
		return *this;
	}
	inline Vec3 & operator*=( const Vec3 & in ) {
		x *= in.x; y *= in.y; z *= in.z;
		return *this;
	}
	inline Vec3 & operator/=( const Vec3 & in ) {
		x /= in.x; y /= in.y; z /= in.z;
		return *this;
	}
	inline Vec3 & operator+=( double in ) {
		x += in; y += in; z += in;
		return *this;
	}
	inline Vec3 & operator-=( double in ) {
		x -= in; y -= in; z -= in;
		return *this;
	}
	inline Vec3 & operator*=( double in ) {
		x *= in; y *= in; z *= in;
		return *this;
	}
	inline Vec3 & operator/=( double in ) {
		x /= in; y /= in; z /= in;
		return *this;
	}
	inline Vec3 & operator=( const Vec3 & in ) {
		x = in.x; y = in.y; z = in.z;
		return *this;
	}
	inline Vec3 operator-() const {
		return Vec3( -x, -y, -z );
	}
	inline bool operator==( const Vec3 & in ) const {
		return x == in.x && y == in.y && z == in.z;
	}
	inline bool operator!=( const Vec3 & in ) const {
		return x != in.x || y != in.y || z != in.z;
	}
	inline double Length() const {
		return sqrt( x * x + y * y + z * z );
	}
	inline double LengthSq() const {
		return x * x + y * y + z * z;
	}
	inline double Dot( const Vec3 & in ) const {
		return x * in.x + y * in.y + z * in.z;
	}
	inline double DotNorm( const Vec3 & in ) const {
		return Normalized().Dot( in.Normalized() );
	}
	inline Vec3 Cross( const Vec3 & in ) const {
		//(a2b3  -   a3b2,     a3b1   -   a1b3,     a1b2   -   a2b1)
		return Vec3( y*in.z - z * in.y, z*in.x - x * in.z, x*in.y - y * in.x );
	}
	inline double Dist( const Vec3 & in ) const {
		return (in - *this).Length();
	}
	inline double DistSq( const Vec3 & in ) const {
		return (in - *this).LengthSq();
	}
	inline double Normalize() {
		double l = Length();
		*this /= l;
		return l;
	}
	inline Vec3 Normalized() const {
		return *this / Length();
	}
	inline Vec3 Rescaled( double in ) const {
		return *this * (in / Length());
	}
	inline Vec3 Clamped( double maxLen ) const {
		double f = Length() / maxLen;
		return (f > 1.0) ? *this / f : *this;
	}
	inline Vec3 Lerped( double t, const Vec3 & to ) const {
		return (*this)*(1 - t) + to * t;
	}
	inline Vec3 Slerped( double t, const Vec3 & to ) const {
		double theta = acos( Dot( to ) );
		//double sinTheta = sin( theta );
		//return (*this)*sin( (1 - t)*theta ) / sinTheta + to*sin( t*theta ) / sinTheta;
		return ((*this)*sin( (1 - t)*theta ) + to * sin( t*theta )) / sin( theta );
	}
	inline Vec3 Nlerped( double t, const Vec3 & to ) const {
		return Lerped( t, to ).Normalized();
	}
	inline void MakeZero() {
		x = 0; y = 0; z = 0;
	}
	inline bool IsZero() const {
		return x == 0.0 && y == 0.0 && z == 0.0;
	}
	inline bool IsEpsilon() const {
		return abs( x ) + abs( y ) + abs( z ) <= FLT_EPSILON;
	}
	inline bool IsAxis( double eps = FLT_EPSILON ) const {
		if (abs( x ) >  eps && abs( y ) <= eps && abs( z ) <= eps) return true;
		if (abs( x ) <= eps && abs( y ) >  eps && abs( z ) <= eps) return true;
		if (abs( x ) <= eps && abs( y ) <= eps && abs( z ) >  eps) return true;
		return false;
	}
	inline Vec3 SetX( double in ) const {
		return Vec3( in, y, z );
	}
	inline Vec3 SetY( double in ) const {
		return Vec3( x, in, z );
	}
	inline Vec3 SetZ( double in ) const {
		return Vec3( x, y, in );
	}
};

struct Vec2 {
	double x, y;

	inline Vec2() :x( 0 ), y( 0 ) {};
	inline Vec2( double InX, double InY ) :x( InX ), y( InY ) {};
	inline Vec2 operator-() const {
		return Vec2( -x, -y );
	}
	inline Vec2 operator-( Vec2 InV ) const {
		return Vec2( x - InV.x, y - InV.y );
	}
	inline Vec2 operator+( Vec2 InV ) const {
		return Vec2( x + InV.x, y + InV.y );
	}
	inline Vec2 operator*( double in ) const {
		return Vec2( x*in, y*in );
	}
	inline Vec2 operator/( double in ) const {
		return Vec2( x / in, y / in );
	}
	inline bool operator==( Vec2 InV ) const {
		return (x == InV.x && y == InV.y);
	}
	inline Vec2 operator+=( Vec2 InV ) {
		x += InV.x;
		y += InV.y;
		return *this;
	}
	inline Vec2 operator-=( Vec2 InV ) {
		x -= InV.x;
		y -= InV.y;
		return *this;
	}
	inline double Dist( Vec2 InV ) const {
		return (InV - *this).Len();
	}
	inline double Dist2( Vec2 InV ) const {
		return (InV - *this).Len2();
	}
	// returns angle betwwen -PI and PI
	inline double Angle( Vec2 InV ) const {
		double dot = x * InV.x + y * InV.y;
		double det = x * InV.y - y * InV.x;
		return atan2( det, dot );
	}
	// return perpendicular vector
	inline Vec2 Perp() const {
		return Vec2( -y, x );
	}
	inline Vec2 DirTo( const Vec2 & in ) const {
		return (in - *this).Normalized();
	}
	inline double Dot( Vec2 InV ) const {
		return x * InV.x + y * InV.y;
	}
	inline double DotNorm( Vec2 InV ) const {
		return Normalized().Dot( InV.Normalized() );
	}
	inline double Len2() const {
		return x * x + y * y;
	}
	inline double Len() const {
		return sqrt( x*x + y * y );
	}
	inline double Normalize() {
		double l = Len();
		x /= l; y /= l;
		return l;
	}
	inline Vec2 Normalized() const {
		return *this / Len();
	}
	inline Vec2 Rescale( double in ) const {
		double lvLen = Len();
		return Vec2( x, y ) * (in / lvLen);
	}
	inline Vec2 Lerped( double t, Vec2 to ) const {
		return (*this)*(1 - t) + to * t;
	}
	inline Vec2 Slerped( double t, Vec2 to ) const {
		//double d = Dot( to );
		double theta = acos( Dot( to ) );
		//double sinTheta = sin( theta );
		//return (*this)*sin( (1 - t)*theta ) / sinTheta + to*sin( t*theta ) / sinTheta;
		return ((*this)*sin( (1 - t)*theta ) + to * sin( t*theta )) / sin( theta );
	}
	inline Vec2 Nlerped( double t, Vec2 to ) const {
		return Lerped( t, to ).Normalized();
	}
	inline Vec2 Left( double t ) const {
		return Slerped( t, Perp() );
	}
	inline Vec2 Right( double t ) const {
		return Slerped( t, -Perp() );
	}
	inline bool IsZero() const {
		return x == 0 && y == 0;
	}
	inline bool IsEpsilon() const {
		return abs( x ) <= FLT_EPSILON && abs( y ) <= FLT_EPSILON;
	}
	inline void GetCellAt( int & OutX, int & OutY, double CellWidth ) const {
		OutX = (int)ceil( x / CellWidth ) - 1;
		OutY = (int)ceil( y / CellWidth ) - 1;
	}
	inline static Vec2 Lerp( double Factor, Vec2 From, Vec2 To ) {
		return From * (1.0f - Factor) + To * Factor;
	}
	static Vec2 CellCenter( int InX, int InY, double CellWidth ) {
		return Vec2( InX * CellWidth - CellWidth * 0.5f, InY * CellWidth - CellWidth * 0.5f );
	}

	//static const Vec2 Zero;
	//static const Vec2 UnitX;
	//static const Vec2 UnitY;
};


class Plane {
public:
	Vec3 Normal;
	double Dist;
	//int Sign;

	inline Plane() {	};
	inline Plane( Vec3 InNormal, double InDist ) :Normal( InNormal ), Dist( InDist ) {};
	inline Plane( Vec3 InPoint, Vec3 InNormal ) {

	}

	inline double DistTo( const Vec3 & InPoint ) const {
		return (Normal * Normal.Dot( InPoint )).Length();
	}

	inline double DistSq( const Vec3 & InPoint ) const {
		return (Normal * Normal.Dot( InPoint )).LengthSq();
	}

	inline Vec3 Project( const Vec3 & InPoint ) const {
		return InPoint - Normal * Normal.Dot( InPoint );
	}
};



struct Line2 {
	Vec2	Normal;
	double	Dist;

	inline Line2() {

	}

	inline Line2( double A, double B, double C ) {
		Normal.x = A;
		Normal.y = B;
		Dist = C;
	}

	inline Line2( const Vec2 & InNormal, double InDist ) {
		Normal = InNormal;
		Dist = InDist;
	}

	inline Line2( const Vec2 & InA, const Vec2 & InB ) {
		FromSegment( InA, InB );
	}

	inline void FromSegment( const Vec2 & InA, const Vec2 & InB ) {
		Normal.x = InA.y - InB.y;
		Normal.y = InB.x - InA.x;
		if (!Normal.IsEpsilon()) Normal = Normal.Normalized();
		Dist = InA.x*InB.y - InB.x*InA.y;
	}

	//inline void FromRay( Vec2 point, Vec2 dir ) {
	//	Normal.x = -dir.y;
	//	Normal.y = dir.x;
	//	Dist = point.x*dir.y - dir.x*point.y;
	//}

	inline void FromRay( const Vec2 & point, const Vec2 & dir ) {
		FromSegment( point, point + dir );
	}

	inline void FromOrigin( const Vec2 & dir ) {
		//Normal.x = -dir.y;
		//Normal.y = dir.x;
		//Dist = 0;
		FromRay( Vec2( 0, 0 ), dir );
	}

	inline Vec2 GetDir() const {
		return -Normal.Perp();
	}

	inline Line2 FlipSide() const {
		return Line2( -Normal, -Dist );
	}

	inline double DistToPoint( Vec2 InPoint ) const {
		//return Normal.x*InPoint.x + Normal.y*InPoint.y + Dist / sqrt( Sqr( Normal.x ) + Sqr( Normal.y ) );
		return Normal.Dot( InPoint ) + Dist;
	}

	inline int GetSign( Vec2 InPoint, double eps = -FLT_EPSILON ) const {
		return (DistToPoint( InPoint ) >= eps )? 1 : -1;
	}

	inline bool IsSameSide( Vec2 inA, Vec2 inB ) const {
		return GetSign( inA ) == GetSign( inB );
	}

	inline Vec2 Project( Vec2 point ) const {
		//return InPoint + Normal * DistToPoint( InPoint );
		//return InPoint + Normal * abs( DistToPoint( InPoint ) );
		double x = Normal.y * (Normal.y * point.x - Normal.x * point.y) - Normal.x*Dist;
		double y = Normal.x * (-Normal.y * point.x + Normal.x * point.y) - Normal.y*Dist;
		return Vec2( x, y );
	}

	inline bool IsEpsilon() const {
		return Normal.IsEpsilon();
	}
};

class Bounds {
public:
	Vec3 Min, Max;

	inline Bounds() {

	}
	inline Bounds( Vec3 InMin, Vec3 InMax ) :Min( InMin ), Max( InMax ) {

	}
};

inline void GenSphereVectors2( int numH, int numV, std::vector<Vec3> & vecs ) {
	for (int i = 0; i < numH; i++) {
		for (int j = 0; j < numV; j++) {
			Vec3 v( sin( M_PI * i / (double)numH ) * cos( 2 * M_PI * j / (double)numV ), sin( M_PI * i / (double)numH ) * sin( 2 * M_PI * j / (double)numV ), cos( M_PI * i / (double)numH ) );
			vecs.emplace_back( v );
		}
	}

	std::unique( vecs.begin(), vecs.end() );
}

inline void GenCircleVectors( int num, std::vector<Vec3> & vecs ) {
	for (int i = 0; i < num; i++) {
		vecs.emplace_back( sin( M_PI * i / (double)num ), 0, cos( M_PI * i / (double)num ) );
	}
}

inline bool IsInside( Vec3 p, Vec3 min, Vec3 max ) {
	return (p.x > min.x && p.x < max.x) && (p.y > min.y && p.y < max.y) && (p.z > min.z && p.z < max.z);
}

#endif // ! _H_HELPERS