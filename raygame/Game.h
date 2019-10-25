#pragma once

#include<vector>

#include"Physics.h"

class Game
{
	float accumulatedFixedTime;

	std::vector<PhysicsObject> physicsObjects;

public:
	Game();

	void Init();
	void Tick();
	void TickPhys();
	void Draw() const;
	void Exit();

	bool ShouldClose();
	bool ShouldPhysics();

	float targetFixedStep;
};