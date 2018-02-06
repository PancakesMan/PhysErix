#pragma once
#include <list>

#include "RigidBody.h"

class CompositeObject : public RigidBody
{
public:
	CompositeObject();
	~CompositeObject();

	virtual float getMass();
	virtual void draw(aie::Renderer2D* renderer);
	virtual void applyForce(glm::vec2 force);
	virtual void applyForceToActor(RigidBody* actor2, glm::vec2 force);
	virtual bool checkCollision(PhysicsObject* other);
	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual glm::vec2 getPosition();
	virtual glm::vec2 getVelocity();

	void addComponent(RigidBody* component);
	void removeComponent(RigidBody* component);

private:
	std::list<RigidBody*> m_components;
};