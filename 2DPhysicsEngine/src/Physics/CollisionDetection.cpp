#include "CollisionDetection.h"
#include "Body.h"
#include "Contact.h"

bool CollisionDetection::IsColliding(Body* a, Body* b, Contact& contact)
{
    const bool aIsCircle = a->shape->GetType() == CIRCLE;
    const bool bIsCircle = b->shape->GetType() == CIRCLE;

    if (aIsCircle && bIsCircle)
    {
        return IsCollidingCircleCircle(a, b, contact);
    }
    else//TODO
    {
        
    }
    return false;
}

bool CollisionDetection::IsCollidingCircleCircle(Body* a, Body* b, Contact& contact)
{
    const CircleShape* circleA = dynamic_cast<CircleShape*>(a->shape);
    const CircleShape* circleB = dynamic_cast<CircleShape*>(b->shape);

    const Vec2 ab = b->position - a->position;
    const float sumRadii = circleA->radius + circleB->radius;

    const bool isColliding = ab.MagnitudeSquared() <= (sumRadii * sumRadii);
    if (!isColliding) { return false; }

    //Compute contact information
    contact.a = a;
    contact.b = b;
    contact.normal = ab;
    contact.normal.Normalize();

    contact.start = b->position - contact.normal * circleB->radius;
    contact.end = a->position + contact.normal * circleA->radius;

    contact.depth = (contact.end - contact.start).Magnitude();

    return true;
}
