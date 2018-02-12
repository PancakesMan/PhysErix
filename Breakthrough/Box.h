#pragma once
#include "RigidBody.h"

class Box : public RigidBody
{
public:
	Box(glm::vec2 position, glm::vec2 velocity, float mass, int width, int height, float rotation, float elasticity, glm::vec4 colour);
	~Box();

	virtual void draw(aie::Renderer2D* renderer);
	virtual bool checkCollision(PhysicsObject* pOther);

	int getWidth() { return m_width; }
	int getHeight() { return m_height; }

private:
	int m_width, m_height;
};