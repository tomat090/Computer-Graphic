#include "figures.h"

bool Sphere::intersect(const Vec3df &rayorig, const Vec3df &raydir, float &t0, float &t1)
{
	Vec3df l = center - rayorig;
	float scalarBetRays = l.dot(raydir);
	if (scalarBetRays < 0) return false; // if scalar product less then 0, then sphere is behind camera
	float d2 = l.dot(l) - scalarBetRays * scalarBetRays; // square distance
	if (d2 > radius2) return false;
	float thc = sqrt(radius2 - d2);
	t0 = scalarBetRays - thc;
	t1 = scalarBetRays + thc;
	return true;
}

Vec3df Sphere::findNormal(Vec3df intersectPoint)
{
	return (intersectPoint - center);
}

bool Plane::intersect(const Vec3df &rayorig, const Vec3df &raydir, float &t0, float &t1)
{
	Vec3df norm(-A, -B, -C);
	Vec3df direction = raydir - rayorig;
	float scalarBetRays = norm.dot(direction);
	if (scalarBetRays >= 0) return false; // if scalar product less then 0, then sphere is behind camera
	t0 = -(norm.dot(rayorig) + D) / scalarBetRays;
	if ((direction.x == 0) && (direction.y == 0) && (direction.z == -1))
		std::cout << t0;
	return true;
}

Vec3df Plane::findNormal(Vec3df intersectPoint)
{
	return Vec3df(A, B, C);
}

bool Triangle::intersect(const Vec3df &rayorig, const Vec3df &raydir, float &t0, float &t1)
{
	Vec3df CA = A - center, CB = B - center;
	Vec3df norm = CA.vect(CB);
	return true;
}