#include "Force.h"
#include "Particle.h"

Vec2 Force::GenerateDragForce(const Particle& particle, float constant)
{
    Vec2 dragForce = Vec2{0,0};
    const float velMagSquared = particle.velocity.MagnitudeSquared();
    if (velMagSquared > 0)
    {
        //Calculate drag direction
        const Vec2 dragDirection = particle.velocity.UnitVector() * -1.f;
        //Calculate drag magnitude
        const float dragMag = constant * velMagSquared;
        //Generate final drag force with direction and magnitude
        dragForce = dragDirection * dragMag;
    }
    return dragForce;
}

Vec2 Force::GenerateFrictionForce(const Particle& particle, float constant)
{
    //Calculate friction direction
    const Vec2 frictionDirection = particle.velocity.UnitVector() * -1.f;
    //Calculate friction magnitude
    const float frictionMagnitude = constant;
    //Generate final friction force with direction and magnitude
    Vec2 frictionForce{frictionDirection * frictionMagnitude};
    
    return frictionForce;
}

Vec2 Force::GenerateGravitationalForce(const Particle& a, const Particle& b, float constant)
{
    const Vec2 d = b.position - a.position;

    const float distanceSquared = d.MagnitudeSquared();

    const Vec2 attractionDirection = d.UnitVector();
    const float attractionMagnitude = constant * (a.mass * b.mass) / distanceSquared;

    Vec2 attractionForce = attractionDirection * attractionMagnitude;
    return attractionForce;
}
