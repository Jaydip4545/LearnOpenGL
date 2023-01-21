#pragma once
class particle
{
	private:
		float radius;
		float center_x;
		float center_y;
		float valocity_x ;
		float valocity_y;
		float* points = new float[72];
		float mass;
		float acceleration;
	public:
		particle();
		particle(float r, float x, float y,float val_x,float val_y,float mass,float acceleration);
		~particle();
		float* draw();
		void set_radius(float r);
		void set_center(float x, float y);
		float get_radius();
		float get_center_x();
		float get_center_y();
		float get_valocity_x();
		float get_valocity_y();
		void set_valocity_x(float val_x);
		void set_valocity_y(float val_y);
		void set_acceleration(float val_a);
		void set_mass(float val_m);
		float get_acceleration();
		float get_mass();
		static bool comp(particle& p1, particle& p2);
		


};

