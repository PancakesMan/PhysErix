#include <glm\ext.hpp>
#include <time.h>

#include "BreakthroughApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include "CompositeObject.h"

BreakthroughApp::BreakthroughApp() {
	srand((unsigned)time(NULL));
	for (int i = 0; i < 50; i++)
		rand();
}

BreakthroughApp::~BreakthroughApp() {

}

bool BreakthroughApp::startup() {

	m_2dRenderer = new aie::Renderer2D();

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);

	m_physicsScene = new PhysicsScene();
	m_physicsScene->setGravity(glm::vec2(0, -98));
	m_physicsScene->setTimeStep(0.01f);

	Sphere* ball1 = new Sphere(glm::vec2(getWindowWidth() * (rand() / (double)RAND_MAX), getWindowHeight() * (rand() / (double)RAND_MAX)), glm::vec2(0, 0), 3.0f, 25, 0.f, glm::vec4(1, 0, 0, 1));
	Sphere* ball2 = new Sphere(glm::vec2(getWindowWidth() * (rand() / (double)RAND_MAX), getWindowHeight() * (rand() / (double)RAND_MAX)), glm::vec2(0, 0), 3.0f, 25, 0.f, glm::vec4(1, 0, 0, 1));
	Sphere* ball3 = new Sphere(glm::vec2(getWindowWidth() * (rand() / (double)RAND_MAX), getWindowHeight() * (rand() / (double)RAND_MAX)), glm::vec2(0, 0), 3.0f, 25, 0.f, glm::vec4(1, 0, 0, 1));
	Sphere* ball4 = new Sphere(glm::vec2(getWindowWidth() * (rand() / (double)RAND_MAX), getWindowHeight() * (rand() / (double)RAND_MAX)), glm::vec2(0, 0), 3.0f, 25, 0.f, glm::vec4(1, 0, 0, 1));
	m_physicsScene->addActor(ball1);
	m_physicsScene->addActor(ball2);
	m_physicsScene->addActor(ball3);
	m_physicsScene->addActor(ball4);

	Box* centerBox = new Box(glm::vec2(getWindowWidth() / 2, getWindowHeight() / 2), glm::vec2(0, 0), 1.0f, 20, 20, 0.0f, 0.2f, glm::vec4(1, 0, 0, 1));
	Box* leftBox = new Box(glm::vec2(getWindowWidth() / 2 - 50, getWindowHeight() / 2), glm::vec2(0, 0), 1.0f, 20, 20, 0.0f, 0.2f, glm::vec4(0, 1, 0, 1));
	Box* rightBox = new Box(glm::vec2(getWindowWidth() / 2 + 50, getWindowHeight() / 2), glm::vec2(0, 0), 1.0f, 20, 20, 0.0f, 0.2f, glm::vec4(0, 0, 1, 1));
	Box* aboveBox = new Box(glm::vec2(getWindowWidth() / 2, getWindowHeight() / 2 + 50), glm::vec2(0, 0), 1.0f, 20, 20, 0.0f, 0.2f, glm::vec4(1, 0, 1, 1));
	Box* belowBox = new Box(glm::vec2(getWindowWidth() / 2, getWindowHeight() / 2 - 50), glm::vec2(0, 0), 1.0f, 20, 20, 0.0f, 0.2f, glm::vec4(1, 1, 0, 1));
	Box* belowBox2 = new Box(glm::vec2(getWindowWidth() / 2, getWindowHeight() / 2 - 300), glm::vec2(0, 0), 1.0f, 20, 20, 0.0f, 0.2f, glm::vec4(1, 1, 0, 1));

	//m_physicsScene->addActor(centerBox);
	//m_physicsScene->addActor(leftBox);
	//m_physicsScene->addActor(rightBox);
	//m_physicsScene->addActor(aboveBox);
	//m_physicsScene->addActor(belowBox);
	//m_physicsScene->addActor(belowBox2);

	//CompositeObject* snowman = new CompositeObject();
	//snowman->addComponent(new Sphere(glm::vec2(700, 250), glm::vec2(0, 0), 5.0f, 20, glm::vec4(1, 1, 1, 1)));
	//snowman->addComponent(new Sphere(glm::vec2(700, 275), glm::vec2(0, 0), 5.0f, 15, glm::vec4(1, 1, 1, 1)));
	//m_physicsScene->addActor(snowman);
	//snowman->applyForce(glm::vec2(50, 0));

	Plane* plane = new Plane(glm::vec2(1, 0), 50);
	Plane* plane2 = new Plane(glm::vec2(1, 0), getWindowWidth() - 50);
	Plane* plane3 = new Plane(glm::vec2(0, 1), 50);
	Plane* plane4 = new Plane(glm::vec2(0, 1), getWindowHeight() - 50);

	m_physicsScene->addActor(plane);
	m_physicsScene->addActor(plane2);
	m_physicsScene->addActor(plane3);
	m_physicsScene->addActor(plane4);

	ball1->applyForce(glm::vec2(0, -20));
	ball2->applyForce(glm::vec2(-270, 0));

	leftBox->applyForce(glm::vec2(20, 0));
	rightBox->applyForce(glm::vec2(-20, 0));
	aboveBox->applyForce(glm::vec2(0, -20));
	belowBox->applyForce(glm::vec2(0, -20));
	belowBox2->applyForce(glm::vec2(0, 20));

	return true;
}

void BreakthroughApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void BreakthroughApp::update(float deltaTime) {

	// input example
	aie::Input* input = aie::Input::getInstance();

	m_physicsScene->update(deltaTime);

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	if (input->wasMouseButtonPressed(0))
	{
		creating = true;
		pos.x = input->getMouseX();
		pos.y = input->getMouseY();
	}

	if (input->isMouseButtonDown(0))
	{
		mPos.x = input->getMouseX();
		mPos.y = input->getMouseY();
	}

	if (input->wasMouseButtonReleased(0))
	{
		creating = false;
		m_physicsScene->addActor(new Sphere(pos, mPos - pos, 1.0f, 15, 0.f, glm::vec4(1, 0, 0, 1)));
		count++;
	}
}

void BreakthroughApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw your stuff here!
	m_physicsScene->draw(m_2dRenderer);
	
	// output some text, uses the last used colour
	m_2dRenderer->drawText(m_font, std::to_string(count).c_str(), 0, 5);

	if (creating)
	{
		m_2dRenderer->setRenderColour(1, 1, 1, 1);
		m_2dRenderer->drawCircle(pos.x, pos.y, 15);
		m_2dRenderer->drawLine(pos.x, pos.y, mPos.x, mPos.y);
	}

	// done drawing sprites
	m_2dRenderer->end();
}