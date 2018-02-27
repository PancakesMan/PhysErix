#pragma once
#include <glm\glm.hpp>

#include "RigidBody.h"

class Spring : public PhysicsObject
{
public:
	Spring(RigidBody* body1, RigidBody* body2, float restLength, float springCoefficient, float damping = 0.1f);
	~Spring();

	void fixedUpdate(glm::vec2 gravity, float timeStep);
	void draw(aie::Renderer2D* renderer);

	RigidBody* getFirstBody() { return m_body1; }
	RigidBody* getSecondBody() { return m_body2; }

	void debug() {}

private:
	RigidBody* m_body1;
	RigidBody* m_body2;

	glm::vec2 m_contact1, m_contact2;

	float m_damping, m_restLength, m_springCoefficient;
};