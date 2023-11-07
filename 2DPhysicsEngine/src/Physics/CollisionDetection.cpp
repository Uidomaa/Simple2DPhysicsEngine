#include "CollisionDetection.h"
#include "Body.h"
#include "Contact.h"

bool CollisionDetection::IsColliding(Body* a, Body* b, Contact& contact)
{
    if (a->IsStatic() && b->IsStatic()) { return false; }
    
    const bool aIsCircle = a->shape->GetType() == CIRCLE;
    const bool bIsCircle = b->shape->GetType() == CIRCLE;
    const bool aIsPolygon = a->shape->GetType() == POLYGON || a->shape->GetType() == BOX;
    const bool bIsPolygon = b->shape->GetType() == POLYGON || b->shape->GetType() == BOX;

    if (aIsCircle)
    {
        if (bIsCircle){ return IsCollidingCircleCircle(a, b, contact); }
        if (bIsPolygon) { return  IsCollidingPolygonCircle(b, a, contact); }
    }
    else if (aIsPolygon)
    {
        if (bIsPolygon) { return IsCollidingPolygonPolygon(a, b, contact); }
        if (bIsCircle) { return IsCollidingPolygonCircle(a, b, contact); }
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

bool CollisionDetection::IsCollidingPolygonPolygon(Body* a, Body* b, Contact& contact)
{
    const PolygonShape* polygonA = dynamic_cast<PolygonShape*>(a->shape);
    const PolygonShape* polygonB = dynamic_cast<PolygonShape*>(b->shape);
    Vec2 axisA, axisB, pointA, pointB;
    const float abSeparation = polygonA->FindMinSeparation(*polygonB, axisA, pointA);
    if (abSeparation > 0) { return false; }
    const float baSeparation = polygonB->FindMinSeparation(*polygonA, axisB, pointB);
    if (baSeparation > 0) { return false; }
    
    //Compute contact information
    contact.a = a;
    contact.b = b;
    if (abSeparation > baSeparation)
    {
        contact.depth = -abSeparation;
        contact.normal = axisA.Normal();
        contact.start = pointA;
        contact.end = contact.start + contact.normal * contact.depth;
    }
    else
    {
        contact.depth = -baSeparation;
        contact.normal = -axisB.Normal();
        contact.end = pointB;
        contact.start = contact.end - contact.normal * contact.depth;
    }
    return true;
}

bool CollisionDetection::IsCollidingPolygonCircle(Body* polygon, Body* circle, Contact& contact)
{
    const PolygonShape* polygonShape = dynamic_cast<PolygonShape*>(polygon->shape);
    const CircleShape* circleShape{dynamic_cast<CircleShape*>(circle->shape)};
    const std::vector<Vec2>& polygonVertices = polygonShape->worldVertices;
    
    bool isOutside{false};
    Vec2 minCurrVertex{};
    Vec2 minNextVertex{};
    float distanceCircleToEdge{std::numeric_limits<float>::lowest()};
    
    for (int currVertex = 0; currVertex < polygonVertices.size(); currVertex++)
    {
        int nextVertex = (currVertex + 1) % polygonVertices.size();
        Vec2 normal = polygonShape->EdgeAt(currVertex).Normal();
        // Compare circle centre with polygon vertex
        Vec2 vertexToCircleCentre = circle->position - polygonVertices[currVertex];
        float projection = vertexToCircleCentre.Dot(normal);
        //If circle centre is outside of polygon
        if (projection > 0)
        {
            distanceCircleToEdge = projection;
            minCurrVertex = polygonVertices[currVertex];
            minNextVertex = polygonVertices[nextVertex];
            isOutside = true;
            break;
        }
        //Else circle centre is inside polygon, find nearest edge
        else
        {
            if (projection > distanceCircleToEdge)
            {
                distanceCircleToEdge = projection;
                minCurrVertex = polygonVertices[currVertex];
                minNextVertex = polygonVertices[nextVertex];
            }
        }
    }
    //If outside polygon
    if (isOutside)
    {
        //If inside region A
        Vec2 v1 = circle->position - minCurrVertex;
        Vec2 v2 = minNextVertex - minCurrVertex;
        if (v1.Dot(v2) < 0)
        {
            const float v1Magnitude = v1.Magnitude();
            //Check if circle edge overlaps vertex
            if (v1Magnitude > circleShape->radius){ return false; }
            //Collision info
            contact.a = polygon;
            contact.b = circle;
            contact.depth = circleShape->radius - v1Magnitude;
            contact.normal = v1.Normalize();
            contact.start = circle->position + (contact.normal * -circleShape->radius);
            contact.end = contact.start + (contact.normal * contact.depth);
        }
        else
        {
            //If inside region B
            v1 = circle->position - minNextVertex;
            v2 = minCurrVertex - minNextVertex;
            if (v1.Dot(v2) < 0)
            {
                const float v1Magnitude = v1.Magnitude();
                //Check if circle edge overlaps vertex
                if (v1Magnitude > circleShape->radius){ return false; }
                //Collision info
                contact.a = polygon;
                contact.b = circle;
                contact.depth = circleShape->radius - v1Magnitude;
                contact.normal = v1.Normalize();
                contact.start = circle->position + (contact.normal * -circleShape->radius);
                contact.end = contact.start + (contact.normal * contact.depth);
            }
            else
            {
                //If inside region C
                //Check if circle edge overlaps edge
                if (distanceCircleToEdge > circleShape->radius) { return false; }
                //Collision info
                contact.a = polygon;
                contact.b = circle;
                contact.depth = circleShape->radius - distanceCircleToEdge;
                contact.normal = (minNextVertex - minCurrVertex).Normal();
                contact.start = circle->position - (contact.normal * circleShape->radius);
                contact.end = contact.start + (contact.normal * contact.depth);
            }
        }
    }
    else
    {
        //If inside polygon
        contact.a = polygon;
        contact.b = circle;
        contact.depth = circleShape->radius - distanceCircleToEdge;
        contact.normal = (minNextVertex - minCurrVertex).Normal();
        contact.start = circle->position - (contact.normal * circleShape->radius);
        contact.end = contact.start + (contact.normal * contact.depth);
    }
    return true;
}
