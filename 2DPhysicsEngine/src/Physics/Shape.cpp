#include "Shape.h"
#include <iostream>
#include "Body.h"

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

Vec2 PolygonShape::EdgeAt(int index) const
{
    const int currVertex = index;
    const int nextVertex = (index + 1) % worldVertices.size();
    return worldVertices[nextVertex] - worldVertices[currVertex];
}

float PolygonShape::FindMinSeparation(const PolygonShape& other, Vec2& axis, Vec2& point) const
{
    float separation = std::numeric_limits<float>::lowest();
    //Loop through all the vertices of a
    for (int i = 0; i < worldVertices.size(); i++)
    {
        //  Find the normal axis (perpendicular)
        Vec2 va = worldVertices[i];
        Vec2 normal = EdgeAt(i).Normal();
        float minSep = std::numeric_limits<float>::max();
        Vec2 minVertex;
        //  Loop through all the vertices of b
        for (int j = 0; j < other.worldVertices.size(); j++)
        {
            Vec2 vb = other.worldVertices[j];
            //project vertex b onto the normal axis
            float projection = (vb - va).Dot(normal);
            //Keep track of the min separation
            if (projection < minSep)
            {
                minSep = projection;
                minVertex = vb;
            }
        }
        if (minSep > separation)
        {
            separation = minSep;
            axis = EdgeAt(i);
            point = minVertex;
        }        
    }
    return separation;
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
