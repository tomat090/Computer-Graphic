#ifndef __VEC3DIMENTION_H__
#define __VEC3DIMENTION_H__

#include <cstdlib> 
#include <stdlib.h>
#include <cstdio> 
#include <cmath> 
#include <fstream> 
#include <vector> 
#include <iostream> 
#include <cassert> 
#include <random>
#include <time.h> 

#define M_PI 3.141592653589793 
#define INFINITY 1e8 

template<typename T>
class Vec3Dimention
{
public:
	T x, y, z;
	Vec3Dimention() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec3Dimention(T xx) : x(xx), y(xx), z(xx) {}
	Vec3Dimention(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vec3Dimention& normalize()
	{
		T nor2 = length2();
		if (nor2 > 0) {
			T invNor = 1 / sqrt(nor2);
			x *= invNor, y *= invNor, z *= invNor;
		}
		return *this;
	}
	Vec3Dimention<T> operator * (const T &f) const { return Vec3Dimention<T>(x * f, y * f, z * f); }
	Vec3Dimention<T> operator * (const Vec3Dimention<T> &v) const { return Vec3Dimention<T>(x * v.x, y * v.y, z * v.z); }
	T dot(const Vec3Dimention<T> &v) const { return x * v.x + y * v.y + z * v.z; }
	Vec3Dimention<T> vect(const Vec3Dimention<T> &v) const{
		float xx = y * v.z - z * v.y;
		float yy = z * v.x - z * v.z;
		float zz = x * v.y - y * v.x;
		return Vec3Dimention<T>(xx, yy, zz);
	}
	Vec3Dimention<T> operator - (const Vec3Dimention<T> &v) const { return Vec3Dimention<T>(x - v.x, y - v.y, z - v.z); }
	Vec3Dimention<T> operator + (const Vec3Dimention<T> &v) const { return Vec3Dimention<T>(x + v.x, y + v.y, z + v.z); }
	Vec3Dimention<T>& operator += (const Vec3Dimention<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vec3Dimention<T>& operator *= (const Vec3Dimention<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
	Vec3Dimention<T> operator - () const { return Vec3Dimention<T>(-x, -y, -z); }
	T length2() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(length2()); }
	friend std::ostream & operator << (std::ostream &os, const Vec3Dimention<T> &v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};

typedef Vec3Dimention<float> Vec3df;

#endif