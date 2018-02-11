#include <Renderer2D.h>
#include <glm\glm.hpp>

#include "PhysicsScene.h"
#include "RigidBody.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"

typedef bool(*fn)(PhysicsObject*, PhysicsObject*);

static fn collisionFunctionArray[] =
{
	PhysicsScene::plane2Plane, PhysicsScene::plane2Sphere, PhysicsScene::plane2Box,
	PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere, PhysicsScene::sphere2Box,
	PhysicsScene::box2Plane, PhysicsScene::box2Sphere, PhysicsScene::box2Box
};

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

		checkForCollision();
		/*for (auto pActor : m_actors)
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
		dirty.clear();*/
	}
}

void PhysicsScene::draw(aie::Renderer2D* renderer)
{
	for (auto pActor : m_actors)
		pActor->draw(renderer);
}

void PhysicsScene::checkForCollision()
{
	for (auto it = m_actors.begin(); it != std::prev(m_actors.end()); it++)
		for (auto it2 = std::next(it); it2 != m_actors.end(); it2++)
		{
			PhysicsObject* object1 = *it;
			PhysicsObject* object2 = *it2;

			int shapeId1 = object1->getShapeID();
			int shapeId2 = object2->getShapeID();

			int functionIdx = (shapeId1 * SHAPE_COUNT) + shapeId2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIdx];
			if (collisionFunctionPtr != nullptr)
				collisionFunctionPtr(object1, object2);
		}
}

bool PhysicsScene::plane2Plane(PhysicsObject*, PhysicsObject*)
{
	return false;
}

bool PhysicsScene::plane2Sphere(PhysicsObject*, PhysicsObject*)
{
	return false;
}

bool PhysicsScene::plane2Box(PhysicsObject*, PhysicsObject*)
{
	return false;
}

bool PhysicsScene::sphere2Plane(PhysicsObject* lhs, PhysicsObject* rhs)
{
	Sphere* sphere = dynamic_cast<Sphere*>(lhs);
	Plane* plane = dynamic_cast<Plane*>(rhs);

	if (sphere != nullptr && plane != nullptr)
	{
		glm::vec2 collisionNormal = plane->getNormal();
		float sphereToPlane = glm::dot(sphere->getPosition(), plane->getNormal()) - plane->getDistance();

		if (sphereToPlane < 0)
		{
			collisionNormal *= -1;
			sphereToPlane *= -1;
		}

		float intersection = sphere->getRadius() - sphereToPlane;
		if (intersection > 0)
		{
			sphere->setVelocity(glm::vec2(0, 0));
			return true;
		}
	}

	return false;
}

bool PhysicsScene::sphere2Sphere(PhysicsObject* lhs, PhysicsObject* rhs)
{
	Sphere* sphere1 = dynamic_cast<Sphere*>(lhs);
	Sphere* sphere2 = dynamic_cast<Sphere*>(rhs);

	if (sphere1 != nullptr && sphere2 != nullptr)
	{
		if (glm::distance(sphere1->getPosition(), sphere2->getPosition()) < sphere1->getRadius() + sphere2->getRadius())
		{
			sphere1->setVelocity(glm::vec2(0, 0));
			sphere2->setVelocity(glm::vec2(0, 0));
			return true;
		}
	}
	return false;
}

bool PhysicsScene::sphere2Box(PhysicsObject*, PhysicsObject*)
{
	return false;
}

bool PhysicsScene::box2Plane(PhysicsObject*, PhysicsObject*)
{
	return false;
}

bool PhysicsScene::box2Sphere(PhysicsObject*, PhysicsObject*)
{
	return false;
}

bool PhysicsScene::box2Box(PhysicsObject* lhs, PhysicsObject* rhs)
{
	Box* box1 = dynamic_cast<Box*>(lhs);
	Box* box2 = dynamic_cast<Box*>(rhs);

	if (box1 != nullptr && box2 != nullptr)
	{
		if (box1->getPosition().x + box1->getWidth() / 2 > box2->getPosition().x - box2->getWidth() / 2
			&& box1->getPosition().x - box1->getWidth() / 2 < box2->getPosition().x + box2->getWidth() / 2
			&& box1->getPosition().y + box1->getHeight() / 2 > box2->getPosition().y - box2->getHeight() / 2
			&& box1->getPosition().y - box1->getHeight() / 2 < box2->getPosition().y + box2->getHeight() / 2)
		{
			box1->setVelocity(glm::vec2(0, 0));
			box2->setVelocity(glm::vec2(0, 0));
			return true;
		}
	}

	return false;
}