#pragma once
#include "PhysicsObject.h"

class RigidBody : public PhysicsObject
{
public:
	RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, float elasticity);
	~RigidBody();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void debug();
	virtual void applyForce(glm::vec2 force);
	virtual void applyForceToActor(RigidBody* actor2, glm::vec2 force);

	virtual bool checkCollision(PhysicsObject* pOther) = 0;
	void resolveCollision(RigidBody* other);

	virtual glm::vec2 getPosition() { return m_position; }
	virtual glm::vec2 getVelocity() { return m_velocity; }
	void setVelocity(glm::vec2 velocity) { m_velocity = velocity; }
	float getRotation() { return m_rotation; }
	virtual float getMass() { return m_mass; }
	float getElasticity() { return m_elasticity; }

protected:
	glm::vec2 m_position, m_velocity;
	float m_mass, m_rotation, m_elasticity;
};