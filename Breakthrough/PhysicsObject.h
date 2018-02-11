#pragma once
#include <glm\vec2.hpp>
#include <glm\vec4.hpp>

enum ShapeType { 
	PLANE = 0,
	SPHERE,
	BOX,
	SHAPE_COUNT,
	COMPOSITE
};

namespace aie {
	class Renderer2D;
}

class PhysicsObject
{
protected:
	PhysicsObject(ShapeType a_shapeID) : m_shapeID(a_shapeID) {};

public:
	virtual void fixedUpdate(glm::vec2 gravity, float timeStep) = 0;
	virtual void debug() = 0;
	virtual void draw(aie::Renderer2D* renderer) = 0;
	virtual void resetPosition() {};
	
	ShapeType getShapeID() { return m_shapeID; }
	glm::vec4 getColour() { return m_colour; }

protected:
	ShapeType m_shapeID;
	glm::vec4 m_colour;
};