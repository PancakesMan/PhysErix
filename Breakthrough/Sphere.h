#pragma once
#include "RigidBody.h"

class Sphere : public RigidBody
{
public:
	Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, float elasticity, glm::vec4 colour);
	~Sphere();

	virtual void draw(aie::Renderer2D* renderer);
	virtual bool checkCollision(PhysicsObject* pOther);

	float getRadius() { return m_radius; }

protected:
	float m_radius;
};