﻿#pragma once
#include <vector>
#include "Vec2.h"

struct Body;

enum ShapeType
{
    CIRCLE,
    POLYGON,
    BOX
};

struct Shape
{
    bool isColliding{false};
    
    virtual ~Shape() = default;
    virtual ShapeType GetType() const = 0;
    virtual Shape* Clone() const = 0;
    virtual float GetMomentOfInertia() const = 0;
};

struct CircleShape: public Shape
{
    float radius;

    CircleShape(const float radius);
    virtual ~CircleShape() override;
    virtual ShapeType GetType() const override;
    // Clone() is necessary because passing a ShapeType into the constructor will lose reference to the pointer once
    // the calling method goes out of scope and pops the ShapeType off the stack
    virtual Shape* Clone() const override;
    virtual float GetMomentOfInertia() const override;
};

struct PolygonShape : public Shape
{
    std::vector<Vec2> localVertices;
    std::vector<Vec2> worldVertices;

    PolygonShape() = default;
    PolygonShape(const std::vector<Vec2> vertices);
    virtual ~PolygonShape() override;
    virtual ShapeType GetType() const override;
    virtual Shape* Clone() const override;
    virtual float GetMomentOfInertia() const override;
    //Translate vertices from local space to world space
    void UpdateVertices(const Vec2& position, float angle);
    //Return edge along v[index] to v[index+1]
    Vec2 EdgeAt(int index) const;
    float FindMinSeparation(const PolygonShape& other, Vec2& axis, Vec2& point) const;
    float FindMinSeparation(const Body& circle, Vec2& axis, Vec2& point) const;
};

struct BoxShape : public PolygonShape
{
    float width;
    float height;

    BoxShape(float width, float height);
    virtual ~BoxShape() override;
    virtual ShapeType GetType() const override;
    virtual Shape* Clone() const override;
    virtual float GetMomentOfInertia() const override;
};
