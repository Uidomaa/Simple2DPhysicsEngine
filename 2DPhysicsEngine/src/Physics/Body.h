#ifndef BODY_H
#define BODY_H
#include "Vec2.h"

struct Body
{
    int radius;
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    Vec2 forceSum{};

    float mass;
    float oneOverMass; //Inverse mass for performance

    Body(float x, float y, float mass);
    ~Body();

    void AddForce(const Vec2& force);
    void Integrate(float dt);
    void ClearForces();
};

#endif