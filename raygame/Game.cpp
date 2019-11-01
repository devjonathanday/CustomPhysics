#include "Game.h"
#include "raylib.h"
#include<iostream>

Game::Game()
{
	targetFixedStep = 1.0f / 60.0f;
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
		{
			newObject.collider = Circle{ 15.0f };
			newObject.mass = 1.0f;
			//newObject.gravity = 0;
		}
		else
		{
			newObject.collider = AABB{ {15, 15} };
			newObject.mass = 1.0f;
			//newObject.gravity = 0;
		}
	}
}

void Game::TickPhys()
{
	accumulatedFixedTime -= targetFixedStep;

	//for loop with specified range of objects
	for (auto& i : physicsObjects)
	{
		i.TickPhys(targetFixedStep);
		i.AddAccel({ 0, i.gravity });
		//if (IsKeyDown(KEY_W)) i.AddForce({ 0, -150 });
	}

	for (auto& i : physicsObjects)
	{
		for (auto& j : physicsObjects)
		{
			if (&i == &j) { continue; }

			bool collision = false;

			i.collider.match([i, j, &collision](Circle c) { if (CheckCircleX(i.pos, c, j.pos, j.collider)) { collision = true; }},
							 [i, j, &collision](AABB a)   { if (CheckAABBX(i.pos, a, j.pos, j.collider))   { collision = true; }});

			if (collision) { ResolvePhysicsBodies(i, j); }

			if (i.pos.y > GetScreenHeight())
				i.pos.y = 0;
			if (i.pos.x < 0)
				i.pos.x = GetScreenWidth();
			if (i.pos.x > GetScreenWidth())
				i.pos.x = 0;
		}
	}
}

void Game::Draw() const
{
	BeginDrawing();

	ClearBackground(BLACK);

	for (const auto& i : physicsObjects)
	{
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