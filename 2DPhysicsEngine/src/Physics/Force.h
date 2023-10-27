#pragma once
#include "Vec2.h"

struct Particle;
struct Force
{
public:
    static Vec2 GenerateDragForce(const Particle &particle, float constant);
    static Vec2 GenerateFrictionForce(const Particle &particle, float constant);
};
