#include "raylib.h"
#include "Game.h"

int main()
{
	Game app;

	app.Init();

	while (!app.ShouldClose())
	{
		app.Tick();

		while (app.ShouldPhysics())
		{
			app.TickPhys();
		}

		app.Draw();
	}

	app.Exit();

	return 0;
}