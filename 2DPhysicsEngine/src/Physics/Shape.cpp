#include "Shape.h"
#include <iostream>

CircleShape::CircleShape(const float radius)
{
    this->radius = radius;
    std::cout << "CircleShape constructed" << std::endl;
}

CircleShape::~CircleShape()
{
    std::cout << "CircleShape destructed" << std::endl;
}

ShapeType CircleShape::GetType() const
{
    return CIRCLE;
}

Shape* CircleShape::Clone() const
{
    return new CircleShape(radius);
}

float CircleShape::GetMomentOfInertia() const
{
    return 0.5f * (radius * radius);
}

PolygonShape::PolygonShape(const std::vector<Vec2> vertices)
{
    //TODO
    std::cout << "PolygonShape constructed" << std::endl;
}

PolygonShape::~PolygonShape()
{
    //TODO
    std::cout << "PolygonShape destructed" << std::endl;
}

ShapeType PolygonShape::GetType() const
{
    return POLYGON;
}

Shape* PolygonShape::Clone() const
{
    return new PolygonShape(vertices);
}

float PolygonShape::GetMomentOfInertia() const
{
    //TODO
    return 0.0;
}

BoxShape::BoxShape(float width, float height)
{
    //TODO
    std::cout << "BoxShape constructed" << std::endl;
}

BoxShape::~BoxShape()
{
    //TODO
    std::cout << "BoxShape destructed" << std::endl;
}

ShapeType BoxShape::GetType() const
{
    return BOX;
}

Shape* BoxShape::Clone() const
{
    return new BoxShape(width, height);
}

float BoxShape::GetMomentOfInertia() const
{
    return 0.083333f * (width * width + height * height);
}
