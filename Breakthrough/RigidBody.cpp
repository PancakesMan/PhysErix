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
	if (m_isKinematic) {
		m_angularVelocity = 0;
		m_velocity = glm::vec2(0, 0);
		return;
	}

	// Remember when applying the force of gravity, mass cancels out
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
	// Find the vector between their centres, or use the provided direction of force
	glm::vec2 normal = collisionNormal ? *collisionNormal : glm::normalize(other->getPosition() - m_position);

	// Get the vector perpendicular to the collision normal
	glm::vec2 perp(normal.y, -normal.x);


	// Detemine the total velocity of the contact points for the two objects, 
	// for both linear and rotational
	// 'r' is the radius from axis to application of force
	float r1 = glm::dot(contact - m_position, -perp);
	float r2 = glm::dot(contact - other->getPosition(), perp);

	// Velocity of the contact point on this object
	float v1 = glm::dot(m_velocity, normal) - r1 * m_angularVelocity;

	// Velocity of contact point on other actor
	float v2 = glm::dot(other->m_velocity, normal) + r2 * other->getAngularVelocity();

	// If they're moving closer
	if (v1 > v2)
	{
		// Calculate the effective mass at contact point for each object 
		// ie how much the contact point will move due to the force applied.
		float m1 = 1.0f / (1.0f / getMass() + (r1*r1) / getMoment());
		float m2 = 1.0f / (1.0f / other->getMass() + (r2*r2) / other->getMoment());

		float elasticity = (m_elasticity + other->getElasticity()) / 2;

		glm::vec2 force = (1.0f + elasticity) * m1 * m2 / (m1 + m2) * (v1 - v2) * normal;

		// Apply equal and opposite forces
		applyForce(-force, contact - m_position);
		other->applyForce(force, contact - other->getPosition());
	}
}

float RigidBody::getTotalEnergy(glm::vec2 gravity)
{
	float eLinear = 0.5f * m_mass * glm::dot(m_velocity, m_velocity);
	float eRotational = 0.5f * m_moment * m_angularVelocity * m_angularVelocity;
	float eGravity = -m_mass * glm::dot(m_position, gravity);

	return eLinear + eRotational + eGravity;
}