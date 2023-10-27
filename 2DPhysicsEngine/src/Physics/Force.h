#pragma once
#include "Vec2.h"

struct Particle;
struct Force
{
public:
    static Vec2 GenerateDragForce(const Particle &particle, float constant);
    static Vec2 GenerateFrictionForce(const Particle &particle, float constant);
    static Vec2 GenerateGravitationalForce(const Particle &a, const Particle &b, float constant);
    static Vec2 GenerateSpringForce(const Particle &a, const Particle &b, float restLength, float constant);
    static Vec2 GenerateSpringForce(const Particle &particle, Vec2 anchor, float restLength, float constant);
};
