#include <Renderer2D.h>
#include <glm\glm.hpp>
#include <vector>

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

	enum Corners { TopLeft = 0, TopRight, BottomLeft, BottomRight};
	std::vector<glm::vec2> corners;

	for (float x = -getWidth() / 2; x < getWidth(); x += getWidth())
	{
		for (float y = -getHeight() / 2; y < getHeight(); y += getHeight())
		{
			glm::vec2 p = getPosition() + x * getLocalX() + y * getLocalY();
			corners.push_back(p);
		}
	}

	renderer->setRenderColour(1, 0, 0, 1);
	renderer->drawCircle(corners[TopLeft].x, corners[TopLeft].y, 2);

	renderer->setRenderColour(0, 1, 0, 1);
	renderer->drawCircle(corners[TopRight].x, corners[TopRight].y, 2);

	renderer->setRenderColour(0, 0, 1, 1);
	renderer->drawCircle(corners[BottomLeft].x, corners[BottomLeft].y, 2);

	renderer->setRenderColour(1, 1, 0, 1);
	renderer->drawCircle(corners[BottomRight].x, corners[BottomRight].y, 2);
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