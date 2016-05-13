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
#include "figures.h"

#define MAX_RAY_DEPTH 5 

float mix(const float &a, const float &b, const float &mix)
{
	return b * mix + a * (1 - mix);
}

Vec3df trace(const Vec3df &rayorig, const Vec3df &raydir, const std::vector<std::auto_ptr<Figure>> &figures, const int &depth)
{
	//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
	float tnear = INFINITY; // parameter of the nearest point of intersection
	const std::auto_ptr<Figure>* figure = NULL;
	// find intersection of this ray with the sphere in the scene
	for (unsigned i = 0; i < figures.size(); ++i) 
	{
		float t0 = INFINITY, t1 = INFINITY;
		if (figures[i]->intersect(rayorig, raydir, t0, t1)) 
		{
			if (t0 < 0) 
				t0 = t1;
			if (t0 < tnear) 
			{
				tnear = t0;
				figure = &figures[i];
			}
		}
	}
	// if there's no intersection return black or background color
	if (!figure) return Vec3df(2);
	Vec3df surfaceColor = 0; // color of the ray/surfaceof the object intersected by the ray 
	Vec3df phit = rayorig + raydir * tnear; // point of intersection 
	Vec3df nhit = (*figure)->findNormal(phit); // normal at the intersection point 
	nhit.normalize(); // normalize normal direction 
	// If the normal and the view direction are not opposite to each other
	// reverse the normal direction. That also means we are inside the sphere so set
	// the inside bool to true. Finally reverse the sign of IdotN which we want
	// positive.
	float bias = 1e-4; // add some bias to the point from which we will be tracing 
	bool inside = false;
	if (raydir.dot(nhit) > 0)
	{
		nhit = -nhit;
		inside = true;
	}
	if (((*figure)->transparency > 0 || (*figure)->reflection > 0) && depth < MAX_RAY_DEPTH)
	{
		float facingratio = -raydir.dot(nhit);
		// change the mix value to tweak the effect
		float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);
		// compute reflection direction (not need to normalize because all vectors
		// are already normalized)
		Vec3df refldir = raydir - nhit * 2 * raydir.dot(nhit);
		refldir.normalize();
		Vec3df reflection = trace(phit + nhit * bias, refldir, figures, depth + 1);
		Vec3df refraction = 0;
		// if the figure is also transparent compute refraction ray (transmission)
		if ((*figure)->transparency) 
		{
			float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface? 
			float cosi = -nhit.dot(raydir); // scalar = cos of angle between
			float k = 1 - eta * eta * (1 - cosi * cosi);
			Vec3df refrdir = raydir * eta + nhit * (eta *  cosi - sqrt(k));
			refrdir.normalize();
			refraction = trace(phit - nhit * bias, refrdir, figures, depth + 1);
		}
		// the result is a mix of reflection and refraction (if the sphere is transparent)
		surfaceColor = (reflection * fresneleffect + refraction * (1 - fresneleffect) * (*figure)->transparency) * (*figure)->surfaceColor;
	}
	else {
		// it's a diffuse object, no need to raytrace any further
		for (unsigned i = 0; i < figures.size(); ++i) {
			if (figures[i]->emissionColor.x > 0) {
				// this is a light
				Vec3df transmission = 1;
				Vec3df lightDirection = figures[i]->center - phit;
				lightDirection.normalize();
				for (unsigned j = 0; j < figures.size(); ++j) {
					if (i != j) {
						float t0, t1;
						if (figures[j]->intersect(phit + nhit * bias, lightDirection, t0, t1)) {
							transmission = 0;
							break;
						}
					}
				}
				surfaceColor += (*figure)->surfaceColor * transmission *
					std::fmax(float(0), nhit.dot(lightDirection)) * figures[i]->emissionColor;
			}
		}
	}

	return surfaceColor + (*figure)->emissionColor;
}

Vec3df rotate(Vec3df pixel, Vec3df camera, Vec3df direction)
{
	Vec3df point = pixel - camera;
	Vec3df startVeiw(0, 0, 1), newVeiw(cos(direction.x), cos(direction.y), cos(direction.z));
	float cosTeta = startVeiw.dot(newVeiw), sinTeta = sqrtf(1 - cosTeta * cosTeta);
	Vec3df axisRotation = startVeiw.vect(newVeiw);
	float arX = axisRotation.x, arY = axisRotation.y, arZ = axisRotation.z;
	float x0 = (cosTeta + (1 - cosTeta) * arX * arX) * point.x +
		((1 - cosTeta) * arX * arY - sinTeta * arZ) * point.y +
		((1 - cosTeta) * arX * arZ + sinTeta * arY) * point.z;
	float y0 = ((1 - cosTeta) * arX * arY + sinTeta * arZ) * point.x +
		(cosTeta + (1 - cosTeta) * arY * arY)* point.y +
		((1 - cosTeta) * arY * arZ - sinTeta * arX) * point.z;
	float z0 = ((1 - cosTeta) * arX * arZ - sinTeta * arY) * point.x +
		((1 - cosTeta) * arY * arZ + sinTeta * arX) * point.y +
		(cosTeta + (1 - cosTeta) * arZ * arZ) * point.z;
	/*
	float cosAlpha = startVeiw.dot(Vec3df(0, cos(direction.y), cos(direction.z))), sinAlpha = sqrtf(1 - cosAlpha * cosAlpha);
	float cosBeta = startVeiw.dot(Vec3df(cos(direction.x), 0, cos(direction.z))), sinBeta = sqrtf(1 - cosBeta * cosBeta);
	float cosGama = startVeiw.dot(Vec3df(cos(direction.x), cos(direction.y), 0)), sinGama = sqrtf(1 - cosGama * cosGama);
	//rotate around Ox
	y = y0 * cosAlpha - z0 * sinAlpha;
	z = y0 * sinAlpha + z0 * cosAlpha;
	y0 = y;
	z0 = z;
	//rotate around Oy
	x = x0 * cosBeta + z0 * sinBeta;
	z = -x0 * sinBeta + z0 * cosBeta;
	x0 = x;
	z0 = z;
	//rotate around Oz
	x = x0 * cosGama - y0 * sinGama;
	y = x0 * sinGama + y0 * cosGama;*/
	Vec3df rotatedPixel = camera + Vec3df(x0, y0, z0);
	return rotatedPixel;
}

void render(const std::vector<std::auto_ptr<Figure>> &figures, Vec3df camera, Vec3df direction, 
	unsigned width, unsigned height, float fov_x, float fov_y)
{
	Vec3df *image = new Vec3df[width * height], *pixel = image;
	float invWidth = 1 / float(width), invHeight = 1 / float(height);
	float aspectratio = width / float(height);
	float angle_x = tan(M_PI * fov_x / 180.), angle_y = tan(M_PI * fov_y / 180.);
	// Trace rays
	for (unsigned y = 0; y < height; ++y) 
	{
		for (unsigned x = 0; x < width; ++x, ++pixel) 
		{
			/*float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle_x * aspectratio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle_y;*/
			float xx = (2 * x * invWidth - 1) * angle_x * aspectratio;
			float yy = (1 - 2 * y * invHeight) * angle_y;
			/*if ((x == 0) && (y == 0))
				std::cout<< "\n" << xx << " " << yy<<"\n";*/
			Vec3df raydir = rotate(Vec3df(xx, yy, 1), camera, direction);
			/*if ((x == 0) && (y == 0))
				std::cout << raydir.x << " " << raydir.y << " " << raydir.z << "\n";*/
			raydir.normalize();
			*pixel = trace(camera, raydir, figures, 0);
		}
	}
	// Save result to a JPG image
	std::ofstream ofs("./scene2.jpg", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::fmin(float(1), image[i].x) * 255) <<
			(unsigned char)(std::fmin(float(1), image[i].y) * 255) <<
			(unsigned char)(std::fmin(float(1), image[i].z) * 255);
	}
	ofs.close();
	delete[] image;
}


int main(int argc, char **argv)
{
	srand(13);
	//std::vector<Sphere> spheres;
	std::vector<std::auto_ptr<Figure>> figures;
	Vec3df camera(0, 0, 0);
	Vec3df direction(90, 90, 0);
	// position, radius, surface color, reflectivity, transparency, emission color
	//figures.emplace_back(new Sphere(Vec3df(0.0, -10004, -20), 10000, Vec3df(0.20, 0.20, 0.20), 0, 0.0));
	//figures.emplace_back(new Sphere(Vec3df(0.0, 0, 20), 4, Vec3df(1.00, 0.32, 0.36), 1, 0.5));
	figures.emplace_back(new Sphere(Vec3df(0.0, 0, 20), 4, Vec3df(1.00, 0.32, 0.36), 0, 0));
	figures.emplace_back(new Sphere(Vec3df(5.0, -1, 15), 2, Vec3df(0.90, 0.76, 0.46), 1, 0.0));
	//figures.emplace_back(new Sphere(Vec3df(5.0, 0, 25), 3, Vec3df(0.65, 0.77, 0.97), 1, 1));
	//figures.emplace_back(new Sphere(Vec3df(5.0, 0, -25), 3, Vec3df(0.65, 0.77, 0.97), 1, 0.0));
	//figures.emplace_back(new Sphere(Vec3df(-5.5, 0, -15), 3, Vec3df(0.90, 0.90, 0.90), 1, 0.0));
	//figures.emplace_back(new Plane(Vec3df(5, 5, 35), 0, 0, 1, Vec3df(0.90, 0.76, 0.46), 1, 0.0));
	// light
	figures.emplace_back(new Sphere(Vec3df(0.0, 20, 30), 3, Vec3df(0.00, 0.00, 0.00), 0, 0.0, Vec3df(3)));
	render(figures, camera, direction, 840, 640, 30, 30);
	int a;
	//std::cin >> a;
	return 0;
}