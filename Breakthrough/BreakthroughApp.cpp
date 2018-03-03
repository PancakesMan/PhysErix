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

#define PI 3.141592654f

BreakthroughApp::BreakthroughApp() {

	// Setup RNG
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

	// Create and setup Physics Scene
	m_physicsScene = new PhysicsScene();
	m_physicsScene->setGravity(glm::vec2(0, -98));
	m_physicsScene->setTimeStep(0.01f);

	// Create scene border walls
	execute(std::string("create borders 50"));

	int centerX = getWindowWidth() / 2, centerY = getWindowHeight() / 2;

	// Create 2 boxes for spring demonstration
	Box* leftBox = new Box(glm::vec2(centerX - 50, centerY), glm::vec2(0, 0), 1.0f, 30, 30, 0.0f, 0.8f, glm::vec4(1, 1, 1, 1));
	Box* rightBox = new Box(glm::vec2(centerX + 50, centerY), glm::vec2(0, 0), 1.0f, 30, 30, 0.0f, 0.8f, glm::vec4(1, 1, 1, 1));

	leftBox->setKinematic(true);
	m_physicsScene->addActor(leftBox);
	m_physicsScene->addActor(rightBox);

	// Join LeftBox and RightBox with Spring
	m_physicsScene->addActor(new Spring(leftBox, rightBox, 120.0f, 1, 0.1));

	rightBox->applyForce(glm::vec2(-20, 0), glm::vec2(20, 0));

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

	// Remove object if you right click it
	if (input->wasMouseButtonPressed(1))
	{
		PhysicsObject* removing = nullptr;
		for (PhysicsObject* po : m_physicsScene->getActors())
			if (po->isInside({ input->getMouseX() + m_cameraX, input->getMouseY() + m_cameraY }))
			{
				removing = po;
				break;
			}
		m_physicsScene->removeActor(removing);
	}

	// start creating ball at mouse position
	if (input->wasMouseButtonPressed(0))
	{
		creating = true;
		float x, y;
		m_2dRenderer->getCameraPos(x, y);

		pos.x = input->getMouseX() + x;
		pos.y = input->getMouseY() + y;
	}

	// Allow moving mouse to 
	// determine balls velocity
	if (input->isMouseButtonDown(0))
	{
		float x, y;
		m_2dRenderer->getCameraPos(x, y);

		mPos.x = input->getMouseX() + x;
		mPos.y = input->getMouseY() + y;
	}

	// Create ball with initial position
	// and velocity
	if (input->wasMouseButtonReleased(0))
	{
		creating = false;
		m_physicsScene->addActor(new Sphere(pos, mPos - pos, 225.f*PI, 15, 0.8f, glm::vec4(1, 0, 0, 1))); //Box(pos, mPos - pos, 1.0f, 25, 25, 0, 0.8f, glm::vec4(1, 0, 0, 1))
		count++;
	}

	// Pause and open command console
	if (input->wasKeyPressed(aie::INPUT_KEY_GRAVE_ACCENT))
	{
		paused = !paused;
		command = "";
	}

	// Code for entering command
	if (paused) {
		// timeout to allow holding backspace
		timeout += deltaTime;

		if (input->wasKeyPressed(aie::INPUT_KEY_ENTER))
			execute(command);

		if (timeout > cd && input->isKeyDown(aie::INPUT_KEY_BACKSPACE))
		{
			timeout = 0.0f;
			command = command.substr(0, command.length() - 1);
		}

		// Shortcut to re-use last used command
		if (input->wasKeyPressed(aie::INPUT_KEY_UP))
			command = lastUsedCommand;

		// Insert mouse position into command
		if (input->wasKeyPressed(aie::INPUT_KEY_TAB))
		{
			if (command.length() > 0 && command[command.length() - 1] != ' ')
				command += " ";
			command += std::to_string((int)input->getMouseX() + (int)m_cameraX) + " ";
			command += std::to_string((int)input->getMouseY() + (int)m_cameraY) + " ";
		}

		// Place pressed characters (as lowercase) into command
		for (auto c : input->getPressedCharacters())
			command += (char)c | 32;
	}

	//update physics code
	if (!paused) {
		m_physicsScene->update(deltaTime);

		// Move the camera with left/right/up/down arrows
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
	
	// Draw command at bottom left 
	// of screen, if game is paused
	if (paused) {
		float length = m_font->getStringWidth(arrow.c_str()), x, y;
		m_2dRenderer->getCameraPos(x, y);

		m_2dRenderer->setRenderColour(1, 1, 1, 1);
		m_2dRenderer->drawText(m_font, arrow.c_str(), x + 50, y + 10);
		m_2dRenderer->drawText(m_font, command.c_str(), x + 55 + length, y + 10);
		m_2dRenderer->drawText(m_font, std::string("_").c_str(), x + 75 + m_font->getStringWidth(command.c_str()), y + 8);
	}

	// Draw line to represent
	// direction and magnitude of
	// sphere being created with mouse
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
	
	// Branch for create commands
	if (commandParams.size() > 0 && commandParams[0] == "create")
	{
		// Create command for spheres
		if (commandParams.size() > 3 && commandParams[1] == "sphere")
		{
			glm::vec2 pos(0, 0);
			float radius = std::stof(commandParams[2]);

			// Get x,y of mouse if mouse is used as parameter
			if (commandParams[3] == "mouse")
			{
				pos.x = input->getMouseX() + x;
				pos.y = input->getMouseY() + y;
			}
			else if (commandParams.size() > 4)
			{
				pos.x = std::stof(commandParams[3]) + x;
				pos.y = std::stof(commandParams[4]) + y;
			}

			Sphere* sphere = new Sphere(pos, glm::vec2(0, 0), radius*radius*PI, radius, 0.8f, glm::vec4(1, 0, 0, 1));

			// Set sphere as kinematic if command end with -k
			if (commandParams[commandParams.size() - 1] == "-k")
				sphere->setKinematic(true);

			m_physicsScene->addActor(sphere);
		}

		// Create command for boxes
		else if (commandParams.size() > 4 && commandParams[1] == "box")
		{
			glm::vec2 pos(0, 0);
			float width = 10, height = 10;

			// Get x,y of mouse if mouse is used as parameter
			if (commandParams[2] == "mouse")
			{
				pos.x = input->getMouseX() + x;
				pos.y = input->getMouseY() + y;

				width = std::stof(commandParams[3]);
				height = std::stof(commandParams[4]);
			}
			else if (commandParams.size() > 5)
			{
				pos.x = std::stof(commandParams[2]) + x;
				pos.y = std::stof(commandParams[3]) + y;

				width = std::stof(commandParams[4]);
				height = std::stof(commandParams[5]);
			}

			Box* box = new Box(pos, glm::vec2(0, 0), width*height, width, height, 0, 0.8f, glm::vec4(0, 0, 1, 1));

			// Set sphere as kinematic if command end with -k
			if (commandParams[commandParams.size() - 1] == "-k")
				box->setKinematic(true);

			m_physicsScene->addActor(box);
		}

		// Create command for planes
		else if (commandParams.size() > 3 && commandParams[1] == "plane")
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

		// Create command for border planes
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

		// Create command for springs
		else if (commandParams.size() > 5 && commandParams[1] == "spring")
		{
			// Positions of objects 1 and 2
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

			float restLength = 120, springCoefficient = 1.0f;
			if (commandParams.size() > 6) restLength = std::stof(commandParams[6]);
			if (commandParams.size() > 7) springCoefficient = std::stof(commandParams[7]);

			if (obj1 != nullptr && obj2 != nullptr && obj1 != obj2)
				m_physicsScene->addActor(new Spring(obj1, obj2, restLength, springCoefficient));
		}

		// Create command for softbody
		else if (commandParams.size() > 1 && commandParams[1] == "softbody")
		{
			if (commandParams.size() > 6)
			{
				float radius = std::stof(commandParams[2]);
				float width = std::stof(commandParams[3]), height = std::stof(commandParams[4]);
				float initialX = std::stof(commandParams[5]), initialY = std::stof(commandParams[6]);

				float rigidity = 1000.0f, damping = 0.1f;
				if (commandParams.size() > 7) rigidity = std::stof(commandParams[7]);
				if (commandParams.size() > 8) damping = std::stof(commandParams[8]);

				std::vector<PhysicsObject*> m_softBodyParts;
				std::vector<PhysicsObject*> m_springs;

				// Returns a RigidBody* from the m_softBodyParts
				// list, treating the list as a 2D array
				auto getRB = [&](int x, int y) {
					return (RigidBody*)m_softBodyParts[(x * height) + y];
				};

				// Create spheres
				for (int i = 0; i < width; i++)
					for (int j = 0; j < height; j++)
						m_softBodyParts.push_back(
							new Sphere(
								glm::vec2(initialX + i * radius*4, initialY - j * radius*4), // Position
								glm::vec2(0, 0),                                             // Velocity
								radius*radius*PI,                                            // Mass
								radius,                                                      // Radius
								0.8f,                                                        // Elasticity
								glm::vec4(0.5, 1, 0.5, 1)));                                 // Colour

				// Create vertical springs between spheres
				for (int x = 0; x < width; x++)
					for (int y = 0; y < height - 1; y++)
						m_springs.push_back(new Spring(getRB(x, y), getRB(x, y + 1), radius * 4.f, rigidity, damping));

				// Create horizontal springs between sphere
				for (int x = 0; x < width - 1; x++)
					for (int y = 0; y < height; y++)
						m_springs.push_back(new Spring(getRB(x, y), getRB(x + 1, y), radius * 4.f, rigidity, damping));

				// Create diagonal-right-down springs between sphere
				for (int x = 0; x < width - 1; x++)
					for (int y = 0; y < height - 1; y++)
						m_springs.push_back(new Spring(getRB(x, y), getRB(x + 1, y + 1), radius * 4.f * sqrtf(2), rigidity, damping));

				// Create diagonal-right-up springs between sphere
				for (int x = 0; x < width - 1; x++)
					for (int y = 1; y < height; y++)
						m_springs.push_back(new Spring(getRB(x, y), getRB(x + 1, y - 1), radius * 4.f * sqrtf(2), rigidity, damping));

				// Create vertical jump springs between spheres
				for (int x = 0; x < width; x++)
					for (int y = 0; y < height - 2; y++)
						m_springs.push_back(new Spring(getRB(x, y), getRB(x, y + 2), radius * 8.f, rigidity, damping));

				// Create horizontal jump springs between sphere
				for (int x = 0; x < width - 2; x++)
					for (int y = 0; y < height; y++)
						m_springs.push_back(new Spring(getRB(x, y), getRB(x + 2, y), radius * 8.f, rigidity, damping));

				// Add obbjects and springs to the scene
				for (auto o : m_softBodyParts)
					m_physicsScene->addActor(o);

				for (auto o : m_springs)
					m_physicsScene->addActor(o);
			}
		}
	}

	// Branch for moveto command
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

	// Branch for clear command
	else if (commandParams.size() > 0 && commandParams[0] == "clear")
	{
		m_physicsScene->clearActors();
	}

	// Branch for move last created to (mlct) command
	else if (commandParams.size() > 0 && commandParams[0] == "mlct")
	{
		PhysicsObject* lastCreated = m_physicsScene->getLastActor();
		RigidBody* rigid; Plane* plane;

		rigid = dynamic_cast<RigidBody*>(lastCreated);
		if (rigid == nullptr)
			plane = dynamic_cast<Plane*>(lastCreated);

		// If mouse is passed in for object position
		// get object at mouse's position
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

		// If mouse isn't passed in, and we have
		// parameters for x and y position
		// get object at those positions
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

	// Branch for setgrav command
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

	// Branch for setbackcolour command
	else if (commandParams.size() > 3 && commandParams[0] == "setbackcolour")
	{
		float r, g, b, a = 1.0f;
		r = std::stof(commandParams[1]);
		g = std::stof(commandParams[2]);
		b = std::stof(commandParams[3]);

		if (commandParams.size() > 4) a = std::stof(commandParams[4]);
		setBackgroundColour(r, g, b, a);
	}

	// set lastUsedCommand to the
	// command that was entered, 
	// and clear the current command
	lastUsedCommand = command;
	command = "";
}

// Split string into array
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