#ifndef __FIGURES_H__
#define __FIGURES_H__

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
#include "vec3dimention.h"

class Figure
{
public:
	Figure(const Vec3df &_c, const Vec3df &_sc, const float &_refl = 0, const float &_transp = 0, const Vec3df &_ec = 0) :
		center(_c), surfaceColor(_sc), emissionColor(_ec), transparency(_transp), reflection(_refl){}
	virtual bool intersect(const Vec3df &rayorig, const Vec3df &raydir, float &t0, float &t1) { return true; };
	virtual Vec3df findNormal(Vec3df point){ return Vec3df(0); }

public:
	Vec3df center;                           /// position of the figure
	Vec3df surfaceColor, emissionColor;      /// surface color and emission (light) 
	float transparency, reflection;         /// surface transparency and reflectivity 
};

class Sphere: public Figure
{
public:
	Sphere(const Vec3df &_c, const float &_r, const Vec3df &_sc, const float &_refl = 0, const float &_transp = 0, const Vec3df &_ec = 0) :
		Figure(_c, _sc, _refl, _transp, _ec), radius(_r), radius2(_r * _r){}

	bool intersect(const Vec3df &rayorig, const Vec3df &raydir, float &t0, float &t1);
	Vec3df findNormal(Vec3df intersectPoint);

public:
	float radius, radius2;                  /// sphere radius and radius^2 
};

class Plane : public Figure
{
public:
	Plane(const Vec3df &_c, const float &_A, const float &_B, const float &_C,
		const Vec3df &_sc, const float &_refl = 0, const float &_transp = 0, const Vec3df &_ec = 0) :
		Figure(_c, _sc, _refl, _transp, _ec), A(_A), B(_B), C(_C), D(_c.dot(Vec3df(_A, _B, _C))) {}

	bool intersect(const Vec3df &rayorig, const Vec3df &raydir, float &t0, float &t1);
	Vec3df findNormal(Vec3df intersectPoint);

public:
	float A, B, C, D;                  /// sphere radius and radius^2 
};

class Triangle : public Figure
{
public:
	Triangle(const Vec3df &_c, const Vec3df &_A, const Vec3df &_B, const Vec3df &_C,
		const Vec3df &_sc, const float &_refl = 0, const float &_transp = 0, const Vec3df &_ec = 0) :
		Figure(_c, _sc, _refl, _transp, _ec), A(_A), B(_B){}

	bool intersect(const Vec3df &rayorig, const Vec3df &raydir, float &t0, float &t1);
	Vec3df findNormal(Vec3df intersectPoint);

public:
	Vec3df A, B;                  /// sphere radius and radius^2 
};

#endif