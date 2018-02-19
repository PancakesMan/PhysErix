#pragma once
#include "PhysicsObject.h"

namespace aie {
	class Renderer2D;
}

class RigidBody;

class Plane : public PhysicsObject
{
public:
	Plane();
	Plane(glm::vec2 normal, float distance);
	~Plane();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void debug();
	virtual void draw(aie::Renderer2D* renderer);
	virtual void resetPosition();
	void resolveCollision(RigidBody* actor, glm::vec2 contact);

	glm::vec2 getNormal() { return m_normal; }
	float getDistance() { return m_distanceToOrigin; }
	void setDistance(float distance) { m_distanceToOrigin = distance; }

protected:
	glm::vec2 m_normal;
	float m_distanceToOrigin;
};