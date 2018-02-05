#pragma once

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
};