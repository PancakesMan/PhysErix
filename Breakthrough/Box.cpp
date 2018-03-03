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
}

bool Box::checkCollision(PhysicsObject* pOther)
{
	return false;
}

void Box::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	RigidBody::fixedUpdate(gravity, timeStep);

	// Calculate positions of points factoring in objects rotation
	float cs = std::cosf(m_rotation);
	float sn = std::sinf(m_rotation);
	m_localX = glm::normalize(glm::vec2(cs, sn));
	m_localY = glm::normalize(glm::vec2(-sn, cs));
}

bool Box::isInside(glm::vec2 point) {
	// Check if point is inside the object
	// assuming no rotation
	return point.x > m_position.x - m_width / 2
		&& point.x < m_position.x + m_width / 2
		&& point.y > m_position.y - m_height / 2
		&& point.y < m_position.y + m_height / 2;
}

bool Box::checkBoxCorners(const Box& box, glm::vec2& contact, int& numContacts, float& pen, glm::vec2& edgeNormal)
{
	float minX, maxX, minY, maxY;
	float boxW = box.getWidth();
	float boxH = box.getHeight();
	int numLocalContacts = 0;
	glm::vec2 localContact(0, 0);

	bool first = true;
	for (float x = -box.getWidth() / 2; x < boxW; x += boxW)
	{
		for (float y = -box.getHeight() / 2; y < boxH; y += boxH)
		{
			glm::vec2 p = box.m_position + x * box.m_localX + y * box.m_localY;
			glm::vec2 p0(glm::dot(p - m_position, m_localX), glm::dot(p - m_position, m_localY));

			if (first || p0.x < minX) minX = p0.x;
			if (first || p0.x > maxX) maxX = p0.x;
			if (first || p0.y < minY) minY = p0.y;
			if (first || p0.y > maxY) maxY = p0.y;

			if (p0.x >= -m_width / 2 && p0.x <= m_width / 2 &&
				p0.y >= -m_height / 2 && p0.y <= m_height / 2)
			{
				numLocalContacts++;
				localContact += p0;
			}
			first = false;
		}
	}

	if (maxX <= -m_width / 2 || minX >= m_width / 2 ||
		maxY <= -m_height / 2 || minY >= m_height / 2)
		return false;

	if (!numLocalContacts)
		return false;

	bool res = false;

	contact += m_position + (localContact.x * m_localX + localContact.y * m_localY) / (float)numLocalContacts;
	numContacts++;

	float pen0 = (m_width / 2) - minX;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = m_localX;
		pen = pen0;
		res = true;
	}

	pen0 = maxX + (m_width / 2);
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = -m_localX;
		pen = pen0;
		res = true;
	}

	pen0 = (m_height / 2) - minY;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = m_localY;
		pen = pen0;
		res = true;
	}

	pen0 = maxY + m_height / 2;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = -m_localY;
		pen = pen0;
		res = true;
	}

	return res;
}