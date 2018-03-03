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
	Spring* spring = dynamic_cast<Spring*>(actor);
	if (spring != nullptr)
		m_springs.push_back(spring);
	else
		m_actors.push_back(actor);
}

void PhysicsScene::removeActor(PhysicsObject* actor)
{
	std::list<Spring*> springsToRemove;
	for (Spring* spring : m_springs)
		if (spring->getFirstBody() == actor || spring->getSecondBody() == actor)
			springsToRemove.push_back(spring);

	m_actors.remove(actor);
	delete actor;

	for (Spring* spring : springsToRemove) {
		m_springs.remove(spring);
		delete spring;
	}
}

void PhysicsScene::update(float dt)
{
	static std::list<PhysicsObject*> dirty;

	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;

	while (accumulatedTime >= m_timeStep)
	{
		for (auto pSpring : m_springs)
			pSpring->fixedUpdate(m_gravity, m_timeStep);

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

	for (auto pSpring : m_springs)
		pSpring->draw(renderer);
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

	// Need to check for collisions against all objects except this one.
	for (auto it = m_actors.begin(); it != std::prev(m_actors.end()); it++)
		for (auto it2 = std::next(it); it2 != m_actors.end(); it2++)
		{
			PhysicsObject* object1 = *it;
			PhysicsObject* object2 = *it2;

			int shapeId1 = object1->getShapeID();
			int shapeId2 = object2->getShapeID();

			if (shapeId1 < 0 || shapeId2 < 0)
				continue;

			// Using function pointers
			int functionIdx = (shapeId1 * SHAPE_COUNT) + shapeId2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIdx];
			if (collisionFunctionPtr != nullptr)
				// Did a collision occur?
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
	// Try to cast objects to sphere and plane
	Sphere* sphere = dynamic_cast<Sphere*>(lhs);
	Plane* plane = dynamic_cast<Plane*>(rhs);

	// If we are successful then test for collision
	if (sphere != nullptr && plane != nullptr)
	{
		glm::vec2 collisionNormal = plane->getNormal();
		float sphereToPlane = glm::dot(sphere->getPosition(), plane->getNormal()) - plane->getDistance();

		// Double sided plane
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
	// Try to cast objects to Sphere and Sphere
	Sphere* sphere1 = dynamic_cast<Sphere*>(lhs);
	Sphere* sphere2 = dynamic_cast<Sphere*>(rhs);

	// If we're successful do collission check
	if (sphere1 != nullptr && sphere2 != nullptr)
	{
		glm::vec2 delta = sphere2->getPosition() - sphere1->getPosition();
		float distance = glm::length(delta);
		float intersection = sphere1->getRadius() + sphere2->getRadius() - distance;

		// If spheres are overlapping
		if (intersection > 0)
		{
			glm::vec2 contactForce = 0.5f * (distance - (sphere1->getRadius() + sphere2->getRadius())) * delta / distance;

			// Do checks to make sure we apply
			// entire force to 1 object if the
			// other is kinematic
			if (!sphere1->isKinematic() && !sphere2->isKinematic()) {
				sphere1->setPosition(sphere1->getPosition() + contactForce);
				sphere2->setPosition(sphere2->getPosition() - contactForce);
			}
			else if (!sphere1->isKinematic())
				sphere1->setPosition(sphere1->getPosition() + contactForce * 2.f);
			else
				sphere2->setPosition(sphere2->getPosition() - contactForce * 2.f);

			sphere1->resolveCollision(sphere2, 0.5f * (sphere1->getPosition() + sphere2->getPosition()));
			return true;
		}
	}
	return false;
}

bool PhysicsScene::sphere2Box(PhysicsObject* lhs, PhysicsObject* rhs)
{
	// Try to cast objects to sphere and box
	Sphere* sphere = dynamic_cast<Sphere*>(lhs);
	Box* box = dynamic_cast<Box*>(rhs);

	// If we're successful do collision check
	if (sphere != nullptr && box != nullptr)
	{
		// Get distance between box center and sphere center
		glm::vec2 circlePos = sphere->getPosition() - box->getPosition();
		float hw = box->getWidth() / 2, hh = box->getHeight() / 2;
		int numContacts = 0;
		glm::vec2 contact(0, 0);

		// Check for sphere collision with the box corners
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

		// Check for sphere collision with box edges
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

		// If the sphere made contact with the box
		if (numContacts > 0) {
			contact = box->getPosition() + (1.0f / numContacts) * (box->getLocalX() * contact.x + box->getLocalY() * contact.y);

			float pen = sphere->getRadius() - glm::length(contact - sphere->getPosition());
			glm::vec2 penVec = glm::normalize(contact - sphere->getPosition()) * pen;

			// Do checks to make sure we apply
			// entire force to 1 object if the
			// other is kinematic
			if (!box->isKinematic() && !sphere->isKinematic()) {
				box->setPosition(box->getPosition() + penVec * 0.5f);
				sphere->setPosition(sphere->getPosition() - penVec * 0.5f);
			}
			else if (!box->isKinematic())
				box->setPosition(box->getPosition() + penVec);
			else
				sphere->setPosition(sphere->getPosition() - penVec);

			box->resolveCollision(sphere, contact, direction);
		}

		delete direction;
	}

	return false;
}

bool PhysicsScene::box2Plane(PhysicsObject* lhs, PhysicsObject* rhs)
{
	// Try to cast objects to box and plane
	Box* box = dynamic_cast<Box*>(lhs);
	Plane* plane = dynamic_cast<Plane*>(rhs);

	// If successful, check for collision
	if (box != nullptr && plane != nullptr)
	{
		int numContacts = 0;
		glm::vec2 contact(0, 0);
		float contactV = 0;
		float radius = 0.5f * std::fminf(box->getWidth(), box->getHeight());
		float penetration = 0;

		// Which side is the centre of mass on?
		glm::vec2 planeOrigin = plane->getNormal() * plane->getDistance();
		float comFromPlane = glm::dot(box->getPosition() - planeOrigin, plane->getNormal());

		// Check all four corners to see if we've hit the plane
		for (float x = -box->getWidth() / 2; x < box->getWidth(); x += box->getWidth())
		{
			for (float y = -box->getHeight() / 2; y < box->getHeight(); y += box->getHeight())
			{
				// Get the position of the corner in world space
				glm::vec2 p = box->getPosition() + x * box->getLocalX() + y * box->getLocalY();
				float distFromPlane = glm::dot(p - planeOrigin, plane->getNormal());

				// This is the total velocity of the point
				float velocityIntoPlane = glm::dot(box->getVelocity() + box->getAngularVelocity() * (-y * box->getLocalX() + x * box->getLocalY()), plane->getNormal());

				// If this corner is on the opposite side from the COM, 
				// and moving further in, we need to resolve the collision
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

		// We've had a hit - typically only two corners can contact
		if (numContacts > 0)
		{
			// Get the average collision velocity into the plane 
			// (covers linear and rotational velocity of all corners involved)
			float collisionV = contactV / (float)numContacts;

			// Get the acceleration required to stop (restitution = 0) or reverse 
			// (restitution = 1) the average velocity into the plane
			glm::vec2 acceleration = -plane->getNormal() * ((1.0f + box->getElasticity()) * collisionV);

			// and the average position at which we'll apply the force 
			// (corner or edge centre)
			glm::vec2 localContact = (contact / (float)numContacts) - box->getPosition();

			// This is the perpendicular distance we apply the force at relative to 
			// the COM, so Torque = F*r
			float r = glm::dot(localContact, glm::vec2(plane->getNormal().y, -plane->getNormal().x));

			// work out the "effective mass" - this is a combination of moment of 
			// inertia and mass, and tells us how much the contact point velocity 
			// will change with the force we're applying
			float m0 = 1.0f / (1.0f / box->getMass() + (r*r) / box->getMoment());

			// and apply the force
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
	// Try to cast objects to box and box
	Box* box1 = dynamic_cast<Box*>(lhs);
	Box* box2 = dynamic_cast<Box*>(rhs);

	// If successful, check for collisions
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

			if (!box1->isKinematic() && !box2->isKinematic()) {
				box1->setPosition(box1->getPosition() - displacement * 0.5f);
				box2->setPosition(box2->getPosition() + displacement * 0.5f);
			}
			else if (!box1->isKinematic())
				box1->setPosition(box1->getPosition() - displacement);
			else
				box2->setPosition(box2->getPosition() + displacement);

			return true;
		}
	}

	return false;
}