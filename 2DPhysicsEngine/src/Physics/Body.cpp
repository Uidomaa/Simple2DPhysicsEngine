#include "Body.h"

#include <iostream>

// TODO:
Body::Body(float x, float y, float mass)
{
    position = Vec2(x, y);
    this->mass = mass;
    oneOverMass = (mass != 0.f ? 1.f / mass : 0.f);
    radius = 20;
    // std::cout << "Body constructor called" << std::endl;
}

Body::~Body()
{
    // std::cout << "Body destructor called" << std::endl;
}

void Body::AddForce(const Vec2& force)
{
    forceSum += force;
}

void Body::Integrate(float dt)
{
    //F = ma
    acceleration = forceSum * oneOverMass;
    
    velocity += acceleration * dt;
    position += velocity;

    ClearForces();
}

void Body::ClearForces()
{
    forceSum = {0,0};
}

