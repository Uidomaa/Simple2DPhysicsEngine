#ifndef BODY_H
#define BODY_H
#include <SDL.h>
#include <SDL_image.h>

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
    float invMass; //Inverse mass for performance
    float I;
    float invI;

    float restitution; //aka bounciness
    float friction; //coefficient of friction
    
    Shape* shape = nullptr;

    SDL_Texture* texture = nullptr;

    Body(const Shape& shape, float x, float y, float mass);
    ~Body();

    void SetTexture(const char* textureFileName);
    bool IsStatic() const;
    void AddForce(const Vec2& force);
    void AddTorque(float torque);
    void ApplyImpulse(const Vec2& j);
    void ApplyImpulse(const Vec2& j, const Vec2& r);
    void IntegrateLinear(float dt);
    void IntegrateAngular(float dt);
    void ClearForces();
    void ClearTorque();
    void Update(float dt);
};

#endif