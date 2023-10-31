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
    restitution = 1.0f;
    this->mass = mass;
    invMass = (mass != 0.f ? 1.f / mass : 0.f);
    I = shape.GetMomentOfInertia() * mass;
    invI = (mass != 0.f ? 1.f / I : 0.f);
    // std::cout << "Body constructor called" << std::endl;
}

Body::~Body()
{
    // std::cout << "Body destructor called" << std::endl;
    delete shape;
}

bool Body::IsStatic() const
{
    return fabs(invMass - 0.0f) < FLT_EPSILON;
}

void Body::AddForce(const Vec2& force)
{
    sumForces += force;
}

void Body::AddTorque(float torque)
{
    sumTorque += torque;
}

void Body::ApplyImpulse(const Vec2& j)
{
    if (IsStatic()) { return; }

    velocity += j * invMass;
}

void Body::IntegrateLinear(float dt)
{
    if (IsStatic()) { return; }
    
    //F = ma
    acceleration = sumForces * invMass;
    
    velocity += acceleration * dt;
    position += velocity/* * dt*/;

    ClearForces();
}

void Body::IntegrateAngular(float dt)
{
    if (IsStatic()) { return; }
    
    //Torque = Ia
    angularAcceleration = sumTorque * invI;
    
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

void Body::Update(float deltaTime)
{
    const bool isPolygon = shape->GetType() == POLYGON || shape->GetType() == BOX;
    if (isPolygon)
    {
        PolygonShape* polygonShape = dynamic_cast<PolygonShape*>(shape);
        polygonShape->UpdateVertices(position, rotation);
    }
    //Integrate acceleration and velocity to get new position
    IntegrateLinear(deltaTime);
    IntegrateAngular(deltaTime);
}

