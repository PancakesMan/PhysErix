#include <glm\glm.hpp>
#include <Renderer2D.h>
#include <vector>

#include "Sphere.h"


Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, float elasticity, glm::vec4 colour) :
	RigidBody(SPHERE, position, velocity, 0, mass, elasticity)
{
	m_radius = radius;
	m_colour = colour;
	m_moment = 0.5f * m_mass * m_radius * m_radius;
}

Sphere::~Sphere()
{
}

void Sphere::draw(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(m_colour.r, m_colour.g, m_colour.b, m_colour.a);
	renderer->drawCircle(m_position.x, m_position.y, m_radius);
}

//bool Sphere::checkCollision(PhysicsObject* pOther)
//{
//	Sphere* other = dynamic_cast<Sphere*>(pOther);
//	if (other == nullptr) return false;
//
//	switch (other->getShapeID())
//	{
//		case ShapeType::SPHERE:
//			return glm::distance(other->getPosition(), m_position) < m_radius + other->getRadius();
//			break;
//		default:
//			return false;
//			break;
//	}
//}

bool Sphere::isInside(glm::vec2 point) {
	return glm::distance(point, m_position) < m_radius;
}
