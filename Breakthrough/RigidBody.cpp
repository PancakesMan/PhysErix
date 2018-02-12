#include <glm\glm.hpp>

#include "RigidBody.h"

RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, float elasticity) :
	PhysicsObject(shapeID), m_position(position), m_velocity(velocity), m_rotation(rotation), m_mass(mass), m_elasticity(elasticity)
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	m_position += m_velocity * timeStep;
	applyForce(gravity * m_mass * timeStep);
}

void RigidBody::debug()
{
}

void RigidBody::applyForce(glm::vec2 force)
{
	m_velocity += force / m_mass;
}

void RigidBody::applyForceToActor(RigidBody* actor2, glm::vec2 force)
{
	actor2->applyForce(force);
	applyForce(-force);
}

void RigidBody::resolveCollision(RigidBody* other)
{
	glm::vec2 normal = glm::normalize(other->getPosition() - m_position);
	glm::vec2 relativeVelocity = other->getVelocity() - m_velocity;

	if (glm::dot(normal, relativeVelocity) > 0)
		return;

	float j = glm::dot(-(1 + m_elasticity) * relativeVelocity, normal) / glm::dot(normal, normal * ((1 / m_mass) + (1 / other->getMass())));

	glm::vec2 force = normal * j;
	applyForceToActor(other, force);
}