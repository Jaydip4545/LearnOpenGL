#include "particle.h"
#include "cmath"
#include "iostream"
particle::particle()
{
	radius = 0;
	center_x = 0;
	center_y = 0;
	valocity_x = 0;
	valocity_y = 0;
	mass = 0;
	acceleration = 0;
}
particle::particle(float r, float x, float y,float val_x,float val_y,float m,float a)
{
	radius = r;
	center_x = x;
	center_y = y;
	valocity_x = val_x;
	valocity_y = val_y;
	mass = m;
	acceleration = a;
}

particle::~particle()
{
	
	//delete[] this->points;
}

float* particle::draw()
{
	
	for (int i = 0; i < 72; i+=2)
	{
		points[i] = (center_x + radius * sin((5*i * 3.1415) / 180));
		points[i+1]= (center_y + radius * cos((5* i * 3.1415) / 180));
		//std::cout << points[i]<< " "<<points[i+1] << std::endl;
	}
	return points;
}

void particle::set_radius(float r)
{
	radius = r;
}

void particle::set_center(float x, float y)
{
	center_x = x;
	center_y = y;
}

void particle::set_valocity_x(float val_x)
{
	valocity_x = val_x;

}

void particle::set_valocity_y(float val_y)
{
	valocity_y = val_y;
}
void particle::set_acceleration(float val_a)
{
	acceleration = val_a;
}
void particle::set_mass(float val_m)
{
	mass = val_m;
}
float particle::get_acceleration()
{
	return acceleration;
}
float particle::get_mass()
{
	return mass;
}
float particle::get_radius()
{
	return radius;
}
float particle::get_center_x()
{
	return center_x;
}
float particle::get_center_y()
{
	return center_y;
}
float particle::get_valocity_x()
{
	return valocity_x;
}
float particle::get_valocity_y()
{
	return valocity_y;
}
bool particle::comp(particle& p1, particle& p2)
{
	if (p2.get_center_x() > p1.get_center_x())
	{
		return true;
	}
	return false;
}
