#include "Contact.h"
#include "Body.h"

void Contact::ResolvePenetration()
{
    //Projection Method
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
    //Friction
    float f = std::min(a->friction, b->friction);
    
    //Relative velocity
    const Vec2 ra = end - a->position;
    const Vec2 rb = start - b->position;
    const Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
    const Vec2 vb = b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
    const Vec2 vRel = va - vb;    
    const float vRelDotNormal = vRel.Dot(normal);
    //Impulse vector
    Vec2 impulseDirection = normal;
    const float raCrossNormal = ra.Cross(normal);
    const float rbCrossNormal = rb.Cross(normal);
    float impulseMagnitude = -(1 + e) * vRelDotNormal / ((a->invMass + b->invMass + (raCrossNormal * raCrossNormal) * a->invI) + (rbCrossNormal * rbCrossNormal) * b->invI);
    //Inpulse along the Normal
    Vec2 jN = impulseDirection * impulseMagnitude;
    
    //Relative velocity
    Vec2 tangent = normal.Normal();
    const float vRelDotTangent = vRel.Dot(tangent);
    //Impulse vector
    impulseDirection = tangent;
    const float raCrossTangent = ra.Cross(tangent);
    const float rbCrossTangent = rb.Cross(tangent);
    impulseMagnitude = f * -(1 + e) * vRelDotTangent / ((a->invMass + b->invMass + (raCrossTangent * raCrossTangent) * a->invI) + (rbCrossTangent * rbCrossTangent) * b->invI);
    //Impulse along the tangent
    Vec2 jT = impulseDirection * impulseMagnitude;
    
    //Apply impulse vector
    Vec2 j = jN + jT;
    a->ApplyImpulse(j, ra);
    b->ApplyImpulse(-j, rb);
}
