#include "Contact.h"
#include "Body.h"

void Contact::ResolvePenetration()
{
    if (a->IsStatic() && b->IsStatic()) { return; }
    const float da = depth / (a->invMass + b->invMass) * a->invMass;
    const float db = depth / (a->invMass + b->invMass) * b->invMass;

    a->position -= normal * da;
    b->position += normal * db;    
}

void Contact::ResolveCollision()
{
    ResolvePenetration();
    
    //Elasticity
    float e = std::min(a->restitution, b->restitution);
    //Relative velocity
    Vec2 vRel = a->velocity - b->velocity;
    float vRelDotNormal = vRel.Dot(normal);
    //Impulse vector
    float impulseMagnitude = -(1 + e) * vRelDotNormal / (a->invMass + b->invMass);
    Vec2 impulseDirection = normal;
    //JN
    Vec2 jn = impulseDirection * impulseMagnitude;

    //Apply impulse vector
    a->ApplyImpulse(jn);
    b->ApplyImpulse(-jn);
}
