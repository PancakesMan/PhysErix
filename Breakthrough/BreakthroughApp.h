#pragma once
#include <vector>

#include "Application.h"
#include "Renderer2D.h"
#include "PhysicsScene.h"

class BreakthroughApp : public aie::Application {
public:

	BreakthroughApp();
	virtual ~BreakthroughApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	PhysicsScene*       m_physicsScene;

	glm::vec2 pos, vel, mPos;
	bool creating = false, paused = false;
	std::string command, lastUsedCommand;
	const std::string arrow = "> ";

	int count = 0;

private:
	void execute(std::string& command);
	std::vector<std::string> split(const std::string string, char delim);
};