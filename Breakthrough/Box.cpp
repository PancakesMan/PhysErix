#include <Renderer2D.h>

#include "Box.h"

Box::Box(glm::vec2 position, glm::vec2 velocity, float mass, int width, int height, float rotation, float elasticity, glm::vec4 colour) :
	RigidBody(ShapeType::BOX, position, velocity, rotation, mass, elasticity), m_width(width), m_height(height)
{
	m_colour = colour;
}
Box::~Box()
{
}

void Box::draw(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(m_colour.r, m_colour.g, m_colour.b, m_colour.a);
	renderer->drawBox(m_position.x, m_position.y, m_width, m_height);
}

bool Box::checkCollision(PhysicsObject* pOther)
{
	return false;
}