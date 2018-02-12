#include "CompositeObject.h"

CompositeObject::CompositeObject() : RigidBody(COMPOSITE, glm::vec2(0,0), glm::vec2(0,0), 0, 0, 0)
{
}

CompositeObject::~CompositeObject()
{
	for (RigidBody* rb : m_components)
		delete rb;
}

void CompositeObject::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	for (RigidBody* rb : m_components)
		rb->fixedUpdate(gravity, timeStep);
}

glm::vec2 CompositeObject::getPosition()
{
	return m_components.front()->getPosition();
}

glm::vec2 CompositeObject::getVelocity()
{
	return m_components.front()->getVelocity();
}

float CompositeObject::getMass()
{
	float mass = 0.0f;
	for (RigidBody* rb : m_components)
		mass += rb->getMass();
	return mass;
}

void CompositeObject::draw(aie::Renderer2D* renderer)
{
	for (RigidBody* rb : m_components)
		rb->draw(renderer);
}

void CompositeObject::applyForce(glm::vec2 force)
{
	for (RigidBody* rb : m_components)
		rb->applyForce(force);
}

void CompositeObject::applyForceToActor(RigidBody* actor2, glm::vec2 force)
{
	actor2->applyForce(force);
	for(RigidBody* rb : m_components)
		rb->applyForce(-force);
}

bool CompositeObject::checkCollision(PhysicsObject* other)
{
	for (RigidBody* rb : m_components)
		if (rb->checkCollision(other) == true)
			return true;
	return false;
}

void CompositeObject::addComponent(RigidBody* component)
{
	m_components.push_back(component);
}

void CompositeObject::removeComponent(RigidBody* component)
{
	m_components.remove(component);
}
