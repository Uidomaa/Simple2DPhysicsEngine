#include "Particle.h"

#include <iostream>

// TODO:
Particle::Particle(float x, float y, float mass)
{
    position = Vec2(x, y);
    this->mass = mass;
    oneOverMass = (mass != 0.f ? 1.f / mass : 0.f);
    radius = 20;
    // std::cout << "Particle constructor called" << std::endl;
}

Particle::~Particle()
{
    // std::cout << "Particle destructor called" << std::endl;
}

void Particle::AddForce(const Vec2& force)
{
    forceSum += force;
}

void Particle::Integrate(float dt)
{
    //F = ma
    acceleration = forceSum * oneOverMass;
    
    velocity += acceleration * dt;
    position += velocity;

    ClearForces();
}

void Particle::ClearForces()
{
    forceSum = {0,0};
}

