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
    return new PolygonShape(localVertices);
}

float PolygonShape::GetMomentOfInertia() const
{
    //TODO
    return 0.0;
}

void PolygonShape::UpdateVertices(const Vec2& position, float angle)
{
    for (int i = 0; i < localVertices.size(); ++i)
    {
        worldVertices[i] = localVertices[i].Rotate(angle);
        worldVertices[i] += position;
    }
}

BoxShape::BoxShape(float width, float height)
{
    this->width = width;
    this->height = height;
    //Setup vertices
    auto a = Vec2{-width * 0.5f, -height * 0.5f};
    auto b = Vec2{width * 0.5f, -height * 0.5f};
    auto c = Vec2{width * 0.5f, height * 0.5f};
    auto d = Vec2{-width * 0.5f, height * 0.5f};
    
    localVertices.push_back(a);
    localVertices.push_back(b);
    localVertices.push_back(c);
    localVertices.push_back(d);
    
    worldVertices.push_back(a);
    worldVertices.push_back(b);
    worldVertices.push_back(c);
    worldVertices.push_back(d);
    
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
