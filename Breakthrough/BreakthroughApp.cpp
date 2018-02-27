#include <glm\ext.hpp>
#include <time.h>
#include <iostream>

#include "BreakthroughApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include "Spring.h"
#include "CompositeObject.h"

#define PI 3.141592654f

BreakthroughApp::BreakthroughApp() {
	srand((unsigned)time(NULL));
	for (int i = 0; i < 50; i++)
		rand();
}

BreakthroughApp::~BreakthroughApp() {

}

bool BreakthroughApp::startup() {

	m_2dRenderer = new aie::Renderer2D();
	m_cameraX = m_cameraY = 0;

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);

	m_physicsScene = new PhysicsScene();
	m_physicsScene->setGravity(glm::vec2(0, -98));
	m_physicsScene->setTimeStep(0.01f);

	Box* centerBox = new Box(glm::vec2(getWindowWidth() / 2, getWindowHeight() / 2), glm::vec2(0, 0), 1.0f, 30, 30, 0.0f, 0.8f, glm::vec4(1, 1, 1, 1));
	Box* leftBox = new Box(glm::vec2(getWindowWidth() / 2 - 50, getWindowHeight() / 2), glm::vec2(0, 0), 1.0f, 30, 30, 0.0f, 0.8f, glm::vec4(1, 1, 1, 1));
	Box* rightBox = new Box(glm::vec2(getWindowWidth() / 2 + 50, getWindowHeight() / 2), glm::vec2(0, 0), 1.0f, 30, 30, 0.0f, 0.8f, glm::vec4(1, 1, 1, 1));
	Box* aboveBox = new Box(glm::vec2(getWindowWidth() / 2, getWindowHeight() / 2 + 50), glm::vec2(0, 0), 1.0f, 30, 30, 0.0f, 0.8f, glm::vec4(1, 1, 1, 1));
	Box* belowBox = new Box(glm::vec2(getWindowWidth() / 2, getWindowHeight() / 2 - 50), glm::vec2(0, 0), 1.0f, 30, 30, 0.0f, 0.8f, glm::vec4(1, 1, 1, 1));
	Box* belowBox2 = new Box(glm::vec2(getWindowWidth() / 2, getWindowHeight() / 2 - 300), glm::vec2(0, 0), 1.0f, 30, 30, 0.0f, 0.8f, glm::vec4(1, 1, 1, 1));

	leftBox->setKinematic(true);

	//m_physicsScene->addActor(centerBox);
	m_physicsScene->addActor(leftBox);
	m_physicsScene->addActor(rightBox);
	//m_physicsScene->addActor(aboveBox);
	//m_physicsScene->addActor(belowBox);
	//m_physicsScene->addActor(belowBox2);

	m_physicsScene->addActor(new Spring(leftBox, rightBox, 120.0f, 1, 0.1));

	Plane* plane = new Plane(glm::vec2(1, 0), 50);
	Plane* plane2 = new Plane(glm::vec2(1, 0), getWindowWidth() - 50);
	Plane* plane3 = new Plane(glm::vec2(0, 1), 50);
	Plane* plane4 = new Plane(glm::vec2(0, 1), getWindowHeight() - 50);

	m_physicsScene->addActor(plane);
	m_physicsScene->addActor(plane2);
	m_physicsScene->addActor(plane3);
	m_physicsScene->addActor(plane4);

	leftBox->applyForce(glm::vec2(20, 0), glm::vec2(20, 0));
	rightBox->applyForce(glm::vec2(-20, 0), glm::vec2(20, 0));
	aboveBox->applyForce(glm::vec2(0, -20), glm::vec2(20, 0));
	belowBox->applyForce(glm::vec2(0, -20), glm::vec2(20, 0));
	belowBox2->applyForce(glm::vec2(0, 20), glm::vec2(20, 0));

	return true;
}

void BreakthroughApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void BreakthroughApp::update(float deltaTime) {

	// input example
	aie::Input* input = aie::Input::getInstance();
	m_2dRenderer->setCameraPos(m_cameraX, m_cameraY);

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	if (input->wasMouseButtonPressed(1))
	{
		PhysicsObject* removing = nullptr;
		for (PhysicsObject* po : m_physicsScene->getActors())
			if (po->isInside({ input->getMouseX(), input->getMouseY() }))
			{
				removing = po;
				break;
			}
		m_physicsScene->removeActor(removing);
	}

	if (input->wasMouseButtonPressed(0))
	{
		creating = true;
		float x, y;
		m_2dRenderer->getCameraPos(x, y);

		pos.x = input->getMouseX() + x;
		pos.y = input->getMouseY() + y;
	}

	if (input->isMouseButtonDown(0))
	{
		float x, y;
		m_2dRenderer->getCameraPos(x, y);

		mPos.x = input->getMouseX() + x;
		mPos.y = input->getMouseY() + y;
	}

	if (input->wasMouseButtonReleased(0))
	{
		creating = false;
		m_physicsScene->addActor(new Sphere(pos, mPos - pos, 225.f*PI, 15, 0.8f, glm::vec4(1, 0, 0, 1))); //Box(pos, mPos - pos, 1.0f, 25, 25, 0, 0.8f, glm::vec4(1, 0, 0, 1))
		count++;
	}

	if (input->wasKeyPressed(aie::INPUT_KEY_GRAVE_ACCENT))
	{
		paused = !paused;
		command = "";
	}

	if (paused) {
		timeout += deltaTime;

		if (input->wasKeyPressed(aie::INPUT_KEY_ENTER))
			execute(command);

		if (timeout > cd && input->isKeyDown(aie::INPUT_KEY_BACKSPACE))
		{
			timeout = 0.0f;
			command = command.substr(0, command.length() - 1);
		}

		if (input->wasKeyPressed(aie::INPUT_KEY_UP))
			command = lastUsedCommand;

		if (input->wasKeyPressed(aie::INPUT_KEY_TAB))
		{
			if (command.length() > 0 && command[command.length() - 1] != ' ')
				command += " ";
			command += std::to_string((int)input->getMouseX() + (int)m_cameraX) + " ";
			command += std::to_string((int)input->getMouseY() + (int)m_cameraY) + " ";
		}

		for (auto c : input->getPressedCharacters())
			command += (char)c;
	}

	//update physics code
	if (!paused) {
		m_physicsScene->update(deltaTime);

		if (input->isKeyDown(aie::INPUT_KEY_LEFT))
			m_cameraX -= 2;

		if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
			m_cameraX += 2;

		if (input->isKeyDown(aie::INPUT_KEY_UP))
			m_cameraY += 2;

		if (input->isKeyDown(aie::INPUT_KEY_DOWN))
			m_cameraY -= 2;
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
	//m_2dRenderer->drawText(m_font, std::to_string(m_physicsScene->getTotalEnergy()).c_str(), 0, 5); //std::to_string(count).c_str()
	if (paused) {
		float length = m_font->getStringWidth(arrow.c_str()), x, y;
		m_2dRenderer->getCameraPos(x, y);

		m_2dRenderer->setRenderColour(1, 1, 1, 1);
		m_2dRenderer->drawText(m_font, arrow.c_str(), x + 50, y + 10);
		m_2dRenderer->drawText(m_font, command.c_str(), x + 55 + length, y + 10);
		m_2dRenderer->drawText(m_font, std::string("_").c_str(), x + 75 + m_font->getStringWidth(command.c_str()), y + 8);
	}

	if (creating)
	{
		m_2dRenderer->setRenderColour(1, 1, 1, 1);
		m_2dRenderer->drawCircle(pos.x, pos.y, 15);
		m_2dRenderer->drawLine(pos.x, pos.y, mPos.x, mPos.y);
	}

	// done drawing sprites
	m_2dRenderer->end();
}

void BreakthroughApp::execute(std::string& command)
{
	float x, y;
	m_2dRenderer->getCameraPos(x, y);

	aie::Input* input = aie::Input::getInstance();
	std::vector<std::string> commandParams = split(command, ' ');
	
	if (commandParams.size() > 0 && commandParams[0] == "create")
	{
		if (commandParams.size() > 1 && commandParams[1] == "sphere")
		{
			glm::vec2 pos(0, 0);
			float radius = std::stof(commandParams[2]);

			if (commandParams[3] == "mouse")
			{
				pos.x = input->getMouseX() + x;
				pos.y = input->getMouseY() + y;
			}
			else
			{
				pos.x = std::stof(commandParams[3]) + x;
				pos.y = std::stof(commandParams[4]) + y;
			}

			Sphere* sphere = new Sphere(pos, glm::vec2(0, 0), radius*radius*PI, radius, 0.8f, glm::vec4(1, 0, 0, 1));

			if (commandParams[commandParams.size() - 1] == "-k")
				sphere->setKinematic(true);

			m_physicsScene->addActor(sphere);
		}
		else if (commandParams.size() > 1 && commandParams[1] == "box")
		{
			glm::vec2 pos(0, 0);
			float width, height;

			if (commandParams[2] == "mouse")
			{
				pos.x = input->getMouseX() + x;
				pos.y = input->getMouseY() + y;

				width = std::stof(commandParams[3]);
				height = std::stof(commandParams[4]);
			}
			else
			{
				pos.x = std::stof(commandParams[2]) + x;
				pos.y = std::stof(commandParams[3]) + y;

				width = std::stof(commandParams[4]);
				height = std::stof(commandParams[5]);
			}

			Box* box = new Box(pos, glm::vec2(0, 0), width*height, width, height, 0, 0.8f, glm::vec4(0, 0, 1, 1));

			if (commandParams[commandParams.size() - 1] == "-k")
				box->setKinematic(true);

			m_physicsScene->addActor(box);
		}
		else if (commandParams.size() > 1 && commandParams[1] == "plane")
		{
			if (commandParams[2] == "vertical")
			{
				glm::vec2 normal(1, 0);
				if (commandParams[3] == "mouse")
					m_physicsScene->addActor(new Plane(normal, input->getMouseX() + m_cameraX));
			}
			else if (commandParams[2] == "horizontal")
			{
				glm::vec2 normal(0, 1);
				if (commandParams[3] == "mouse")
					m_physicsScene->addActor(new Plane(normal, input->getMouseY() + m_cameraY));
			}
		}
		else if (commandParams.size() > 1 && commandParams[1] == "borders")
		{
			glm::vec2 vertical(1, 0);
			glm::vec2 horizontal(0, 1);

			float xOffset = 0, yOffset = 0;
			if (commandParams.size() > 2)
				xOffset = yOffset = std::stof(commandParams[2]);

			m_physicsScene->addActor(new Plane(vertical, 0 + xOffset + m_cameraX));
			m_physicsScene->addActor(new Plane(vertical, getWindowWidth() - xOffset + m_cameraX));
			m_physicsScene->addActor(new Plane(horizontal, 0 + yOffset + m_cameraY));
			m_physicsScene->addActor(new Plane(horizontal, getWindowHeight() - yOffset + m_cameraY));
		}
		else if (commandParams.size() > 1 && commandParams[1] == "spring")
		{
			glm::vec2 p1 = glm::vec2(std::stof(commandParams[2]), std::stof(commandParams[3]));
			glm::vec2 p2 = glm::vec2(std::stof(commandParams[4]), std::stof(commandParams[5]));

			RigidBody *obj1 = nullptr, *obj2 = nullptr;

			for (PhysicsObject* obj : m_physicsScene->getActors())
			{
				RigidBody* rb = dynamic_cast<RigidBody*>(obj);
				if (rb != nullptr)
				{
					if (rb->isInside(p1))
						obj1 = rb;
					else if (rb->isInside(p2))
						obj2 = rb;
				}
			}

			float rl = 0, sc = 0;
			if (commandParams.size() > 6) rl = std::stof(commandParams[6]);
			if (commandParams.size() > 7) sc = std::stof(commandParams[7]);


			if (obj1 != nullptr && obj2 != nullptr && obj1 != obj2)
				m_physicsScene->addActor(new Spring(obj1, obj2, (int)rl ? rl : 120, (int)sc ? sc : 1));
		}
	}
	else if (commandParams.size() > 0 && commandParams[0] == "moveto")
	{
		if (commandParams.size() > 2)
		{
			m_cameraX = std::stof(commandParams[1]);
			m_cameraY = std::stof(commandParams[2]);
		}
		else if (commandParams[commandParams.size() - 1] == "origin")
			m_cameraX = m_cameraY = 0;
	}
	else if (commandParams.size() > 0 && commandParams[0] == "clear")
	{
		m_physicsScene->clearActors();
	}
	else if (commandParams.size() > 0 && commandParams[0] == "mlct")
	{
		PhysicsObject* lastCreated = m_physicsScene->getLastActor();
		RigidBody* rigid; Plane* plane;

		rigid = dynamic_cast<RigidBody*>(lastCreated);
		if (rigid == nullptr)
			plane = dynamic_cast<Plane*>(lastCreated);

		if (commandParams.size() > 1 && commandParams[1] == "mouse")
		{
			float x = input->getMouseX(), y = input->getMouseY();

			if (rigid != nullptr)
				rigid->setPosition(glm::vec2(x + m_cameraX, y + m_cameraY));
			else
			{
				if (plane != nullptr) {
					glm::vec2 verticalNormal(1, 0);
					glm::vec2 horizontalNormal(0, 1);

					if (plane->getNormal() == verticalNormal)
						plane->setDistance(x + m_cameraX);
					else if (plane->getNormal() == horizontalNormal)
						plane->setDistance(y + m_cameraY);
				}
			}
		}
		else if (commandParams.size() > 2)
		{
			float x = std::stof(commandParams[1]), y = std::stof(commandParams[2]);
			if (rigid != nullptr)
				rigid->setPosition(glm::vec2(x + m_cameraX, y + m_cameraY));
			else if (plane != nullptr)
			{
				glm::vec2 verticalNormal(1, 0);
				glm::vec2 horizontalNormal(0, 1);

				if (plane->getNormal() == verticalNormal)
					plane->setDistance(x + m_cameraX);
				else if (plane->getNormal() == horizontalNormal)
					plane->setDistance(y + m_cameraY);
			}
		}
	}
	else if (commandParams.size() > 0 && commandParams[0] == "setgrav")
	{
		if (commandParams.size() > 2)
		{
			m_physicsScene->setGravity(glm::vec2(std::stof(commandParams[1]), std::stof(commandParams[2])));
			for (auto actor : m_physicsScene->getActors())
			{
				RigidBody* rb = dynamic_cast<RigidBody*>(actor);
				if (rb != nullptr)
					rb->setVelocity(glm::vec2(0, 0));
			}
		}
	}
	else if (commandParams.size() >= 4 && commandParams[0] == "setbackcolour")
	{
		float r, g, b;
		r = std::stof(commandParams[1]);
		g = std::stof(commandParams[2]);
		b = std::stof(commandParams[3]);

		setBackgroundColour(r, g, b);
	}

	lastUsedCommand = command;
	command = "";
}

std::vector<std::string> BreakthroughApp::split(const std::string string, char delim)
{
	std::string tmp = "";
	std::vector<std::string> ret;

	for (char c : string)
	{
		if (c != delim)
			tmp += c;
		else {
			if (tmp.length() > 0)
				ret.push_back(tmp);
			tmp.clear();
		}
	}

	if (tmp.length() > 0)
		ret.push_back(tmp);

	return ret;
}