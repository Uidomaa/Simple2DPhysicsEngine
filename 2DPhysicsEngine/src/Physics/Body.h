#ifndef BODY_H
#define BODY_H
#include "Shape.h"
#include "Vec2.h"

struct Body
{
    //Linear motion
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    //Angular motion
    float rotation;
    float angularVelocity;
    float angularAcceleration;
    // Forces and torque
    Vec2 sumForces;
    float sumTorque;
    //Mass and Moment of Inertia    
    float mass;
    float oneOverMass; //Inverse mass for performance
    float I;
    float oneOverI;
    
    Shape* shape = nullptr;

    Body(const Shape& shape, float x, float y, float mass);
    ~Body();

    void AddForce(const Vec2& force);
    void AddTorque(float torque);
    void IntegrateLinear(float dt);
    void IntegrateAngular(float dt);
    void ClearForces();
    void ClearTorque();
};

#endif