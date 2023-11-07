#pragma once
#include "Contact.h"

struct Body;
class CollisionDetection
{
public:
    static bool IsColliding(Body* a, Body* b, Contact& contact);
    static bool IsCollidingCircleCircle(Body* a, Body* b, Contact& contact);
    static bool IsCollidingPolygonPolygon(Body* a, Body* b, Contact& contact);
    static bool IsCollidingPolygonCircle(Body* polygon, Body* circle, Contact& contact);
};
