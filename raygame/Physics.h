#pragma once

#include"glm/vec2.hpp"
#include"Shapes.h"

class PhysicsObject
{
public:
	glm::vec2 pos;
	glm::vec2 vel;

	glm::vec2 forces;

	PhysicsObject();

	float mass;
	float drag;
	float gravity;

	Shape collider;

	void TickPhys(float delta);
	void Draw() const;

	//Add a continuous force with respect to mass
	void AddForce(glm::vec2 force);
	//Add an instantaneous force with respect to mass
	void AddImpluse(glm::vec2 impulse);

	//Accelerates the object, ignoring mass
	void AddAccel(glm::vec2 accel);
	//Add an instantaneous force, ignoring mass
	void AddVelocityChange(glm::vec2 delta);
};