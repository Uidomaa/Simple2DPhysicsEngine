#include "Force.h"
#include "Body.h"

Vec2 Force::GenerateDragForce(const Body& body, float constant)
{
    Vec2 dragForce = Vec2{0,0};
    const float velMagSquared = body.velocity.MagnitudeSquared();
    if (velMagSquared > 0)
    {
        //Calculate drag direction
        const Vec2 dragDirection = body.velocity.UnitVector() * -1.f;
        //Calculate drag magnitude
        const float dragMag = constant * velMagSquared;
        //Generate final drag force with direction and magnitude
        dragForce = dragDirection * dragMag;
    }
    return dragForce;
}

Vec2 Force::GenerateFrictionForce(const Body& body, float constant)
{
    //Calculate friction direction
    const Vec2 frictionDirection = body.velocity.UnitVector() * -1.f;
    //Calculate friction magnitude
    const float frictionMagnitude = constant;
    //Generate final friction force with direction and magnitude
    Vec2 frictionForce{frictionDirection * frictionMagnitude};
    
    return frictionForce;
}

Vec2 Force::GenerateGravitationalForce(const Body& a, const Body& b, float constant)
{
    const Vec2 d = b.position - a.position;

    const float distanceSquared = d.MagnitudeSquared();

    const Vec2 attractionDirection = d.UnitVector();
    const float attractionMagnitude = constant * (a.mass * b.mass) / distanceSquared;

    const Vec2 attractionForce = attractionDirection * attractionMagnitude;
    return attractionForce;
}

Vec2 Force::GenerateSpringForce(const Body& a, const Body& b, float restLength, float constant)
{
    const Vec2 d = a.position - b.position;

    const float displacement = d.Magnitude() - restLength;
    const Vec2 springDirection = d.UnitVector();
    const float springMagnitude = -constant * displacement;

    const Vec2 springForce{springDirection * springMagnitude};
    return springForce;
}

Vec2 Force::GenerateSpringForce(const Body& body, Vec2 anchor, float restLength, float constant)
{
    const Vec2 d = body.position - anchor;

    const float displacement = d.Magnitude() - restLength;
    const Vec2 springDirection = d.UnitVector();
    const float springMagnitude = -constant * displacement;

    const Vec2 springForce{springDirection * springMagnitude};
    return springForce;
}
