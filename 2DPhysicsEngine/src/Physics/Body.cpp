#include "Body.h"
#include <iostream>

Body::Body(const Shape& shape, float x, float y, float mass)
{
    this->shape = shape.Clone();
    position = Vec2(x, y);
    velocity = Vec2{0,0};
    acceleration = Vec2{0,0};
    rotation = 0.0f;
    angularVelocity = 0.0f;
    angularAcceleration = 0.0f;
    sumForces = Vec2{0,0};
    sumTorque = 0.0f;
    this->mass = mass;
    oneOverMass = (mass != 0.f ? 1.f / mass : 0.f);
    I = shape.GetMomentOfInertia() * mass;
    oneOverI = (mass != 0.f ? 1.f / I : 0.f);
    // std::cout << "Body constructor called" << std::endl;
}

Body::~Body()
{
    // std::cout << "Body destructor called" << std::endl;
    delete shape;
}

void Body::AddForce(const Vec2& force)
{
    sumForces += force;
}

void Body::AddTorque(float torque)
{
    sumTorque += torque;
}

void Body::IntegrateLinear(float dt)
{
    //F = ma
    acceleration = sumForces * oneOverMass;
    
    velocity += acceleration * dt;
    position += velocity/* * dt*/;

    ClearForces();
}

void Body::IntegrateAngular(float dt)
{
    //Torque = Ia
    angularAcceleration = sumTorque * oneOverI;
    
    angularVelocity += angularAcceleration * dt;
    rotation += angularVelocity/* * dt*/;

    ClearTorque();
}

void Body::ClearForces()
{
    sumForces = {0,0};
}

void Body::ClearTorque()
{
    sumTorque = 0.0f;
}

