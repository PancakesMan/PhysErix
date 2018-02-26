#pragma once
#include "PhysicsObject.h"

const float MIN_LINEAR_THRESHOLD = 0.1f;
const float MIN_ROTATION_THRESHOLD = 0.1f;

class RigidBody : public PhysicsObject
{
public:
	RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, float elasticity, float linearDrag = 0.3f, float angularDrag = 0.3f, bool isKinematic = false);
	~RigidBody();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void debug();
	virtual void applyForce(glm::vec2 force, glm::vec2 pos);
	virtual void applyForceToActor(RigidBody* actor2, glm::vec2 force);

	virtual bool checkCollision(PhysicsObject* pOther) = 0;
	void resolveCollision(RigidBody* other, glm::vec2 contact, glm::vec2* collisionNormal = nullptr);

	void setKinematic(bool state) { m_isKinematic = state; }
	bool isKinematic() { return m_isKinematic; }

	virtual glm::vec2 getPosition() { return m_position; }
	void setPosition(glm::vec2 position) { m_position = position; }
	virtual glm::vec2 getVelocity() { return m_velocity; }
	void setVelocity(glm::vec2 velocity) { m_velocity = velocity; }
	float getRotation() { return m_rotation; }
	virtual float getMass() { return m_isKinematic ? INT_MAX : m_mass; }
	float getElasticity() { return m_elasticity; }
	float getMoment() { return m_moment; }
	float getAngularVelocity() { return m_angularVelocity; }
	float getTotalEnergy(glm::vec2 gravity);

protected:
	glm::vec2 m_position, m_velocity;
	float m_mass, m_rotation, m_elasticity, m_angularVelocity, m_moment, m_linearDrag, m_angularDrag;
	bool m_isKinematic;
};