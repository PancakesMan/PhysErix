#pragma once
#include <Renderer2D.h>

#include "Spring.h"

Spring::Spring(RigidBody* body1, RigidBody* body2, float restLength, float springCoefficient, float damping)
	: PhysicsObject(ShapeType::SPRING), m_body1(body1), m_body2(body2), m_restLength(restLength), m_springCoefficient(springCoefficient), m_damping(damping)
{
	m_contact1 = glm::vec2(0, 0);
	m_contact2 = glm::vec2(0, 0);
}

Spring::~Spring()
{

}

void Spring::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	// Get positions of 2 objects
	glm::vec2 p2 = m_body2->getPosition();
	glm::vec2 p1 = m_body1->getPosition();

	// Calculate the distance between them
	glm::vec2 dist = p2 - p1;
	float length = sqrtf(dist.x*dist.x + dist.y*dist.y);

	// Calculate how fast the bodies are moving relative to each other
	glm::vec2 relativeVelocity = m_body2->getVelocity() - m_body1->getVelocity();

	// Calcualte the force necessary to bring them back together
	glm::vec2 force = dist * m_springCoefficient * (m_restLength - length) - m_damping * relativeVelocity;

	// Only apply the force to objects that aren't kinematic
	if (!m_body1->isKinematic())
		m_body1->applyForce(-force * timeStep, p1 - m_body1->getPosition());

	if (!m_body2->isKinematic())
		m_body2->applyForce(force * timeStep, p2 - m_body2->getPosition());
}

void Spring::draw(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(1, 1, 1, 1);
	renderer->drawLine(m_body1->getPosition().x, m_body1->getPosition().y, m_body2->getPosition().x, m_body2->getPosition().y);
}