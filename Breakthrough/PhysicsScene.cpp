#include <Renderer2D.h>

#include "PhysicsScene.h"
#include "RigidBody.h"

PhysicsScene::PhysicsScene() : m_timeStep(0.01f), m_gravity(glm::vec2(0,0))
{
}

PhysicsScene::~PhysicsScene()
{
	for (auto pActor : m_actors)
		delete pActor;
}

void PhysicsScene::addActor(PhysicsObject* actor)
{
	m_actors.push_back(actor);
}

void PhysicsScene::removeActor(PhysicsObject* actor)
{
	m_actors.remove(actor);
}

void PhysicsScene::update(float dt)
{
	static std::list<PhysicsObject*> dirty;

	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;

	while (accumulatedTime >= m_timeStep)
	{
		for (auto pActor : m_actors)
			pActor->fixedUpdate(m_gravity, m_timeStep);
		accumulatedTime -= m_timeStep;

		for (auto pActor : m_actors)
			for (auto pOther : m_actors)
			{
				if (pActor == pOther) continue;

				if (std::find(dirty.begin(), dirty.end(), pActor) != dirty.end() &&
					std::find(dirty.begin(), dirty.end(), pOther) != dirty.end())
					continue;

				RigidBody* pRigid = dynamic_cast<RigidBody*>(pActor);
				if (pRigid->checkCollision(pOther))
				{
					pRigid->applyForceToActor(dynamic_cast<RigidBody*>(pOther), pRigid->getVelocity() * pRigid->getMass());
					dirty.push_back(pRigid);
					dirty.push_back(pOther);
				}
			}
		dirty.clear();
	}
}

void PhysicsScene::draw(aie::Renderer2D* renderer)
{
	for (auto pActor : m_actors)
		pActor->draw(renderer);
}
