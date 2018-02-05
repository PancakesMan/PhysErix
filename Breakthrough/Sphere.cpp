#include <gizmos.h>
#include <glm\glm.hpp>
#include "Sphere.h"


Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour) :
	RigidBody(SPHERE, position, velocity, 0, mass)
{
	m_radius = radius;
	m_colour = colour;
}

Sphere::~Sphere()
{
}

void Sphere::makeGizmo()
{
	aie::Gizmos::add2DCircle(m_position, m_radius, 32, m_colour);
}

bool Sphere::checkCollision(PhysicsObject* pOther)
{
	Sphere* other = dynamic_cast<Sphere*>(pOther);
	return glm::distance(other->getPosition(), m_position) < m_radius + other->getRadius();
}
