#include <Renderer2D.h>
#include <glm\glm.hpp>
#include "Box.h"

Box::Box(glm::vec2 position, glm::vec2 velocity, float mass, int width, int height, float rotation, float elasticity, glm::vec4 colour) :
	RigidBody(ShapeType::BOX, position, velocity, rotation, mass, elasticity), m_width(width), m_height(height)
{
	m_colour = colour;
	m_moment = 1.0f / 12.0f * m_mass * m_width * m_height;
}
Box::~Box()
{
}

void Box::draw(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(m_colour.r, m_colour.g, m_colour.b, m_colour.a);
	renderer->drawBox(m_position.x, m_position.y, m_width, m_height, m_rotation);
}

bool Box::checkCollision(PhysicsObject* pOther)
{
	return false;
}

void Box::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	RigidBody::fixedUpdate(gravity, timeStep);

	float cs = std::cosf(m_rotation);
	float sn = std::sinf(m_rotation);
	m_localX = glm::normalize(glm::vec2(cs, sn));
	m_localY = glm::normalize(glm::vec2(-sn, cs));
}