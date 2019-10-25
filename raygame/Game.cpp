#include "Game.h"
#include "raylib.h"
#include<iostream>

Game::Game()
{
	targetFixedStep = 1.0f / 30.0f;
	accumulatedFixedTime = 0.0f;
}

void Game::Init()
{
	int screenWidth = 800;
	int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "Jonathan's World Of Physics");

	SetTargetFPS(60);
}

void Game::Tick()
{
	accumulatedFixedTime += GetFrameTime();

	bool mb0 = IsMouseButtonPressed(0);
	bool mb1 = IsMouseButtonPressed(1);

	if (mb0 || mb1)
	{
		physicsObjects.emplace_back();
		std::cout << "Added a physics object!" << std::endl;

		PhysicsObject& newObject = physicsObjects[physicsObjects.size() - 1];
		Vector2 mousePos = GetMousePosition();
		newObject.pos = { mousePos.x, mousePos.y };

		if (mb0)
			newObject.collider = Circle{ 15.0f };
		else
			newObject.collider = AABB{ {15, 15} };
	}
}

void Game::TickPhys()
{
	accumulatedFixedTime -= targetFixedStep;

	//for loop with specified range of objects
	for (auto& i : physicsObjects)
	{
		i.TickPhys(targetFixedStep);
	}

	for (auto& i : physicsObjects)
	{
		for (auto& j : physicsObjects)
		{
			if (&i == &j) { continue; }

			i.collider.match([i, j](Circle c) { if (CheckCircleX(i.pos, c, j.pos, j.collider)) { std::cout << "Circle collision!" << std::endl; }},
							 [i, j](AABB a)   { if (CheckAABBX(i.pos, a, j.pos, j.collider))   { std::cout << "AABB collision!" << std::endl; }});
		}
	}
}

void Game::Draw() const
{
	BeginDrawing();

	ClearBackground(RAYWHITE);

	for (const auto& i : physicsObjects)
	{
		i.collider.draw
		i.Draw();
	}

	EndDrawing();
}

void Game::Exit()
{
	CloseWindow();
}

bool Game::ShouldClose()
{
	return WindowShouldClose();
}

bool Game::ShouldPhysics()
{
	return accumulatedFixedTime >= targetFixedStep;
}