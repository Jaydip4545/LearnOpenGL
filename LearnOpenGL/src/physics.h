#pragma once
#include"particle.h"
void resolve_collision(particle* p1, particle* p2);
float distance(particle* p1, particle* p2);
void collision(particle* p, int n, float& DeltaTime);
void collision_algo_2(particle* p, int n, float& DeltaTime);
void collision(particle* p, int n, float& DeltaTime, int* arr, int m);
void Earth_Gravity(particle* p, int n, float& DeltaTime);