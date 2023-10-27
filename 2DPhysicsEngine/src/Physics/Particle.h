#ifndef PARTICLE_H
#define PARTICLE_H
#include "Vec2.h"

struct Particle
{
    int radius;
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    Vec2 forceSum{};

    float mass;
    float oneOverMass; //Inverse mass for performance

    Particle(float x, float y, float mass);
    ~Particle();

    void AddForce(const Vec2& force);
    void Integrate(float dt);
    void ClearForces();
};

#endif