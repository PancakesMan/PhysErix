#include <glm\ext.hpp>

#include "BreakthroughApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Sphere.h"
#include "CompositeObject.h"

BreakthroughApp::BreakthroughApp() {

}

BreakthroughApp::~BreakthroughApp() {

}

bool BreakthroughApp::startup() {

	m_2dRenderer = new aie::Renderer2D();

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);

	m_physicsScene = new PhysicsScene();
	m_physicsScene->setGravity(glm::vec2(0, 0));
	m_physicsScene->setTimeStep(0.01f);

	Sphere* ball1 = new Sphere(glm::vec2(500, 250), glm::vec2(0, 0), 3.0f, 10, glm::vec4(1, 0, 0, 1));
	Sphere* ball2 = new Sphere(glm::vec2(600, 250), glm::vec2(0, 0), 3.0f, 10, glm::vec4(1, 0, 0, 1));
	m_physicsScene->addActor(ball1);
	m_physicsScene->addActor(ball2);

	CompositeObject* snowman = new CompositeObject();
	snowman->addComponent(new Sphere(glm::vec2(700, 250), glm::vec2(0, 0), 5.0f, 20, glm::vec4(1, 1, 1, 1)));
	snowman->addComponent(new Sphere(glm::vec2(700, 275), glm::vec2(0, 0), 5.0f, 15, glm::vec4(1, 1, 1, 1)));
	m_physicsScene->addActor(snowman);
	snowman->applyForce(glm::vec2(50, 0));

	ball1->applyForce(glm::vec2(180, 0));
	ball2->applyForce(glm::vec2(-90, 0));

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
}

void BreakthroughApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw your stuff here!
	m_physicsScene->draw(m_2dRenderer);
	
	// output some text, uses the last used colour
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}