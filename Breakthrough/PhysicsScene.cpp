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

float PhysicsScene::getTotalEnergy()
{
	float total = 0.0f;
	for (auto actor : m_actors)
		total += actor->getTotalEnergy(m_gravity);
	return total;
}

void PhysicsScene::checkForCollision()
{
	if (m_actors.size() == 0) return;

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

bool PhysicsScene::plane2Sphere(PhysicsObject* lhs, PhysicsObject* rhs)
{
	return sphere2Plane(rhs, lhs);
}

bool PhysicsScene::plane2Box(PhysicsObject* lhs, PhysicsObject* rhs)
{
	return box2Plane(rhs, lhs);
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
		if (intersection > 0 && glm::dot(collisionNormal, sphere->getVelocity()) < 0)
		{
			glm::vec2 contact = sphere->getPosition() + (collisionNormal * -sphere->getRadius());
			plane->resolveCollision(sphere, contact);

			sphere->setPosition(sphere->getPosition() + collisionNormal * intersection);

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
		glm::vec2 delta = sphere2->getPosition() - sphere1->getPosition();
		float distance = glm::length(delta);
		float intersection = sphere1->getRadius() + sphere2->getRadius() - distance;

		if (intersection > 0)
		{
			glm::vec2 contactForce = 0.5f * (distance - (sphere1->getRadius() + sphere2->getRadius())) * delta / distance;

			if (!sphere1->isKinematic() && !sphere2->isKinematic()) {
				sphere1->setPosition(sphere1->getPosition() + contactForce);
				sphere2->setPosition(sphere2->getPosition() - contactForce);
			}
			else if (!sphere1->isKinematic())
				sphere1->setPosition(sphere1->getPosition() - contactForce * 2.f);
			else
				sphere2->setPosition(sphere2->getPosition() + contactForce * 2.f);

			sphere1->resolveCollision(sphere2, 0.5f * (sphere1->getPosition() + sphere2->getPosition()));
			return true;
		}
	}
	return false;
}

bool PhysicsScene::sphere2Box(PhysicsObject* lhs, PhysicsObject* rhs)
{
	Sphere* sphere = dynamic_cast<Sphere*>(lhs);
	Box* box = dynamic_cast<Box*>(rhs);

	if (sphere != nullptr && box != nullptr)
	{
		glm::vec2 circlePos = sphere->getPosition() - box->getPosition();
		float hw = box->getWidth() / 2, hh = box->getHeight() / 2;
		int numContacts = 0;
		glm::vec2 contact(0, 0);

		for (float x = -hw; x <= hw; x += box->getWidth()) {
			for (float y = -hh; y <= hh; y += box->getHeight()) {
				glm::vec2 p = x * box->getLocalX() + y * box->getLocalY();
				glm::vec2 dp = p - circlePos;
				if (dp.x * dp.x + dp.y * dp.y < sphere->getRadius()*sphere->getRadius()) {
					numContacts++;
					contact += glm::vec2(x, y);
				}
			}
		}

		glm::vec2* direction = nullptr;
		glm::vec2 localPos(glm::dot(box->getLocalX(), circlePos), glm::dot(box->getLocalY(), circlePos));

		if (localPos.y < hh && localPos.y > -hh) {
			if (localPos.x > 0 && localPos.x < hw + sphere->getRadius()) {
				numContacts++;
				contact += glm::vec2(hw, localPos.y);
				direction = new glm::vec2(box->getLocalX());
			}
			if (localPos.x < 0 && localPos.x > -(hw + sphere->getRadius())) {
				numContacts++;
				contact += glm::vec2(-hw, localPos.y);
				direction = new glm::vec2(-box->getLocalX());
			}
		}
		if (localPos.x < hw && localPos.x > -hw) {
			if (localPos.y > 0 && localPos.y < hh + sphere->getRadius()) {
				numContacts++;
				contact += glm::vec2(localPos.x, hh);
				direction = new glm::vec2(box->getLocalY());
			}
			if (localPos.y < 0 && localPos.y > -(hh + sphere->getRadius())) {
				numContacts++;
				contact += glm::vec2(localPos.x, -hh);
				direction = new glm::vec2(-box->getLocalY());
			}
		}

		if (numContacts > 0) {
			contact = box->getPosition() + (1.0f / numContacts) * (box->getLocalX() * contact.x + box->getLocalY() * contact.y);
			box->resolveCollision(sphere, contact, direction);
		}

		delete direction;
	}

	return false;
}

bool PhysicsScene::box2Plane(PhysicsObject* lhs, PhysicsObject* rhs)
{
	Box* box = dynamic_cast<Box*>(lhs);
	Plane* plane = dynamic_cast<Plane*>(rhs);

	if (box != nullptr && plane != nullptr)
	{
		int numContacts = 0;
		glm::vec2 contact(0, 0);
		float contactV = 0;
		float radius = 0.5f * std::fminf(box->getWidth(), box->getHeight());
		float penetration = 0;

		glm::vec2 planeOrigin = plane->getNormal() * plane->getDistance();
		float comFromPlane = glm::dot(box->getPosition() - planeOrigin, plane->getNormal());

		for (float x = -box->getWidth() / 2; x < box->getWidth(); x += box->getWidth())
		{
			for (float y = -box->getHeight() / 2; y < box->getHeight(); y += box->getHeight())
			{
				glm::vec2 p = box->getPosition() + x * box->getLocalX() + y * box->getLocalY();
				float distFromPlane = glm::dot(p - planeOrigin, plane->getNormal());

				float velocityIntoPlane = glm::dot(box->getVelocity() + box->getAngularVelocity() * (-y * box->getLocalX() + x * box->getLocalY()), plane->getNormal());

				if ((distFromPlane > 0 && comFromPlane < 0 && velocityIntoPlane >= 0)
					|| (distFromPlane < 0 && comFromPlane > 0 && velocityIntoPlane <= 0))
				{
					numContacts++;
					contact += p;
					contactV += velocityIntoPlane;

					if (comFromPlane >= 0) {
						if (penetration > distFromPlane)
							penetration = distFromPlane;
					}
					else {
						if (penetration < distFromPlane)
							penetration = distFromPlane;
					}
				}
			}
		}

		if (numContacts > 0)
		{
			float collisionV = contactV / (float)numContacts;
			glm::vec2 acceleration = -plane->getNormal() * ((1.0f + box->getElasticity()) * collisionV);
			glm::vec2 localContact = (contact / (float)numContacts) - box->getPosition();
			float r = glm::dot(localContact, glm::vec2(plane->getNormal().y, -plane->getNormal().x));
			float m0 = 1.0f / (1.0f / box->getMass() + (r*r) / box->getMoment());
			box->applyForce(acceleration * m0, localContact);
			box->setPosition(box->getPosition() - plane->getNormal() * penetration);
		}
	}

	return false;
}

bool PhysicsScene::box2Sphere(PhysicsObject* lhs, PhysicsObject* rhs)
{
	return sphere2Box(rhs, lhs);
}

bool PhysicsScene::box2Box(PhysicsObject* lhs, PhysicsObject* rhs)
{
	Box* box1 = dynamic_cast<Box*>(lhs);
	Box* box2 = dynamic_cast<Box*>(rhs);

	if (box1 != nullptr && box2 != nullptr)
	{
		glm::vec2 boxPos = box2->getPosition() - box1->getPosition();

		glm::vec2 norm(0, 0), contact(0, 0);
		float pen = 0;
		int numContacts = 0;

		box1->checkBoxCorners(*box2, contact, numContacts, pen, norm);
		if (box2->checkBoxCorners(*box1, contact, numContacts, pen, norm))
			norm = -norm;

		if (pen > 0) {
			box1->resolveCollision(box2, contact / (float)numContacts, &norm);

			glm::vec2 displacement = pen * norm;
			box1->setPosition(box1->getPosition() - displacement * 0.5f);
			box2->setPosition(box2->getPosition() + displacement * 0.5f);

			return true;
		}
	}

	return false;
}