#include"physics.h"
#include "algorithm"
#include <iostream>
#include <cmath>
void resolve_collision(particle* p1, particle* p2)
{
    float m1;
    float m2;
    float v1_x = 0;
    float v1_y = 0;
    float v2_x = 0;
    float v2_y = 0;
    m1 = p1->get_mass();
    v1_x = p1->get_valocity_x();
    v1_y = p1->get_valocity_y();
    m2 = p2->get_mass();
    v2_x = p2->get_valocity_x();
    v2_y = p2->get_valocity_y();
    p2->set_valocity_x(((m2 - m1) / (m1 + m2)) * v2_x + ((2.0 * m1) / (m1 + m2)) * v1_x);
    p1->set_valocity_x(((m1 - m2) / (m1 + m2)) * v1_x + ((2.0 * m2) / (m1 + m2)) * v2_x);
    p2->set_valocity_y(((m2 - m1) / (m1 + m2)) * v2_y + ((2.0 * m1) / (m1 + m2)) * v1_y);
    p1->set_valocity_y(((m1 - m2) / (m1 + m2)) * v1_y + ((2.0 * m2) / (m1 + m2)) * v2_y);
}
float distance(particle* p1, particle* p2)
{
    float dis = 0;
    dis = pow((p1->get_center_x() - p2->get_center_x()), 2) + pow((p1->get_center_y() - p2->get_center_y()), 2);
    return sqrt(dis);
}
void collision(particle* p, int n, float& DeltaTime)
{
    std::sort(p, p + n, particle::comp);
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (p[j].get_center_x() - p[j].get_radius() > p[i].get_center_x() + p[i].get_radius() || p[j].get_center_x() + p[j].get_radius() < p[i].get_center_x() - p[i].get_radius())
            {
                break;
            }
            if (distance(&p[i], &p[j]) <= p[i].get_radius() + p[j].get_radius())
            {
                resolve_collision(&p[i], &p[j]);
            }
        }
        if (p[i].get_center_x() + p[i].get_valocity_x() >= 1 - p[i].get_radius())
        {

            p[i].set_valocity_x(-1 * p[i].get_valocity_x());
        }
        if (p[i].get_center_y() + p[i].get_valocity_y() >= 1 - p[i].get_radius())
        {
            p[i].set_valocity_y(-1 * p[i].get_valocity_y());
        }
        if (p[i].get_center_x() + p[i].get_valocity_x() <= -1 + p[i].get_radius())
        {
            p[i].set_valocity_x(-1 * p[i].get_valocity_x());
        }
        if (p[i].get_center_y() + p[i].get_valocity_y() <= -1 + p[i].get_radius())
        {
            p[i].set_valocity_y(-1 * p[i].get_valocity_y());
        }
        
    }
    for (int i = 0; i < n; i++)
    {
        p[i].set_center(p[i].get_center_x() + (p[i].get_valocity_x()) * DeltaTime, p[i].get_center_y() + p[i].get_valocity_y() * DeltaTime);
    }

}
void collision(particle* p, int n, float& DeltaTime, int* arr, int m)
{
   
    for (int i = 0; i < m; i++)
    {
        for (int j = i + 1; j < m; j++)
        {
            /*if (p[j].get_center_x() - p[j].get_radius() > p[i].get_center_x() + p[i].get_radius() || p[j].get_center_x() + p[j].get_radius() < p[i].get_center_x() - p[i].get_radius())
            {
                break;
            }*/
            if (distance(&p[arr[i]], &p[arr[j]]) <= p[arr[i]].get_radius() + p[arr[j]].get_radius())
            {
                resolve_collision(&p[arr[i]], &p[arr[j]]);
            }
        }
        if (p[arr[i]].get_center_x() + p[arr[i]].get_valocity_x() >= 1 - p[arr[i]].get_radius())
        {

            p[arr[i]].set_valocity_x(-1 * p[arr[i]].get_valocity_x());
        }
        if (p[arr[i]].get_center_y() + p[arr[i]].get_valocity_y() >= 1 - p[arr[i]].get_radius())
        {
            p[arr[i]].set_valocity_y(-1 * p[arr[i]].get_valocity_y());
        }
        if (p[arr[i]].get_center_x() + p[arr[i]].get_valocity_x() <= -1 + p[arr[i]].get_radius())
        {
            p[arr[i]].set_valocity_x(-1 * p[arr[i]].get_valocity_x());
        }
        if (p[arr[i]].get_center_y() + p[arr[i]].get_valocity_y() <= -1 + p[arr[i]].get_radius())
        {
            p[arr[i]].set_valocity_y(-1 * p[arr[i]].get_valocity_y());
        }
        
    }

}
void Earth_Gravity(particle* p, int n, float& DeltaTime)
{
    for (int i = 0; i < n; i++)
    {
        p[i].set_valocity_y(p[i].get_valocity_y()-0.0001*DeltaTime);
    }
}
void collision_algo_2(particle* p, int n, float& DeltaTime)
{
    int c[4][100];
    int a = 0;
    int b = 0;
    int d = 0;
    int e = 0;
    for (int i = 0; i < n; i++)
    {
        if (p[i].get_center_x()+p[i].get_radius() >= 0 && p[i].get_center_y()+p[i].get_radius() >= 0)
        {
            c[0][a]=i;
            a++;
        }
        if (p[i].get_center_x()- p[i].get_radius() <= 0 && p[i].get_center_y() + p[i].get_radius() >= 0)
        {
            c[1][b]=i;
            b++;
        }
        if (p[i].get_center_x() - p[i].get_radius() <= 0 && p[i].get_center_y() - p[i].get_radius() <= 0)
        {
            c[2][d]=i;
            d++;
        }
        if (p[i].get_center_x() + p[i].get_radius() >= 0 && p[i].get_center_y() - p[i].get_radius() <= 0)
        {
            c[3][e]=i;
            e++;
        }
    }
    collision(p, n, DeltaTime,c[0],a);
    collision(p, n, DeltaTime, c[1], b);
    collision(p, n, DeltaTime, c[2], d);
    collision(p, n, DeltaTime, c[3], e);
    //std::cout << a<<" "<<b<<" "<<d<<" "<<e << std::endl;
    for (int i = 0; i < n; i++)
    {
        p[i].set_center(p[i].get_center_x() + (p[i].get_valocity_x()) * DeltaTime, p[i].get_center_y() + p[i].get_valocity_y() * DeltaTime);
    }

}