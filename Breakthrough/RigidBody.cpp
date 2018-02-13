#include <glm\glm.hpp>

#include "RigidBody.h"

RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, float elasticity, float linearDrag, float angularDrag, bool isKinematic) :
	PhysicsObject(shapeID), m_position(position), m_velocity(velocity), m_rotation(rotation),
	m_mass(mass), m_elasticity(elasticity), m_linearDrag(linearDrag), m_angularDrag(angularDrag), m_isKinematic(isKinematic), m_angularVelocity(0)
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	if (m_isKinematic) return;

	m_position += m_velocity * timeStep;
	m_velocity += gravity * timeStep;

	m_velocity -= m_velocity * m_linearDrag * timeStep;
	m_rotation += m_angularVelocity * timeStep;
	m_angularVelocity -= m_angularVelocity * m_angularDrag * timeStep;

	if (glm::length(m_velocity) < MIN_LINEAR_THRESHOLD)
		m_velocity = glm::vec2();

	if (abs(m_angularVelocity) < MIN_ROTATION_THRESHOLD)
		m_angularVelocity = 0;

	//applyForce(gravity * m_mass * timeStep);
}

void RigidBody::debug()
{
}

void RigidBody::applyForce(glm::vec2 force, glm::vec2 pos)
{
	m_velocity += force / m_mass;
	m_angularVelocity += (force.y * pos.x - force.x * pos.y) / m_moment;
}

void RigidBody::applyForceToActor(RigidBody* actor2, glm::vec2 force)
{
	//actor2->applyForce(force);
	//applyForce(-force);
}

void RigidBody::resolveCollision(RigidBody* other, glm::vec2 contact, glm::vec2* collisionNormal)
{
	//glm::vec2 normal = glm::normalize(other->getPosition() - m_position);
	glm::vec2 normal = collisionNormal ? *collisionNormal : glm::normalize(other->getPosition() - m_position);
	glm::vec2 perp(normal.y, -normal.x);


	float r1 = glm::dot(contact - m_position, -perp);
	float r2 = glm::dot(contact - other->getPosition(), perp);

	float v1 = glm::dot(m_velocity, normal) - r1 * m_rotation;
	float v2 = glm::dot(other->m_velocity, normal) + r2 * other->getRotation();

	if (v1 > v2)
	{
		float m1 = 1.0f / (1.0f / m_mass + (r1*r1) / m_moment);
		float m2 = 1.0f / (1.0f / other->getMass() + (r2*r2) / other->m_moment);

		float elasticity = (m_elasticity + other->getElasticity()) / 2;

		glm::vec2 force = (1.0f + elasticity) * m1 * m2 / (m1 + m2) * (v1 - v2) * normal;

		applyForce(-force, contact - m_position);
		other->applyForce(force, contact - other->getPosition());
	}
	/*glm::vec2 relativeVelocity = other->getVelocity() - m_velocity;

	if (glm::dot(normal, relativeVelocity) > 0)
		return;

	float j = glm::dot(-(1 + m_elasticity) * relativeVelocity, normal) / glm::dot(normal, normal * ((1 / m_mass) + (1 / other->getMass())));

	glm::vec2 force = normal * j;
	applyForceToActor(other, force);*/
}