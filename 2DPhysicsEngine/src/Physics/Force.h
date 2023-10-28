#pragma once
#include "Vec2.h"

struct Body;
struct Force
{
public:
    static Vec2 GenerateDragForce(const Body &body, float constant);
    static Vec2 GenerateFrictionForce(const Body &body, float constant);
    static Vec2 GenerateGravitationalForce(const Body &a, const Body &b, float constant);
    static Vec2 GenerateSpringForce(const Body &a, const Body &b, float restLength, float constant);
    static Vec2 GenerateSpringForce(const Body &body, Vec2 anchor, float restLength, float constant);
};
