#include "Physics.h"
#include"raylib.h"

PhysicsObject::PhysicsObject()
{
	pos = { 0, 0 };
	vel = { 0, 0 };
	forces = { 0, 0 };

	mass = 1.0f;
	drag = 1.0f;
}

void PhysicsObject::TickPhys(float delta)
{
	// integrating forces into velocity
	vel += forces * delta;
	forces = { 0, 0 };

	//integrate linear drag into velocity
	vel *= (1.0f - delta * drag);

	// integrating velocity into position
	pos += vel * delta;
}

void PhysicsObject::Draw() const
{
	collider.match([this](Circle s) { DrawCircleCollider(pos, s.radius); },
				   [this](AABB s)   { DrawAABBCollider(pos, s.halfExtents); });
	//DrawCircleLines(pos.x, pos.y, 15.0f, RED);
}

void PhysicsObject::AddForce(glm::vec2 force)
{
	forces += force * (1.0f / mass);
}

void PhysicsObject::AddImpluse(glm::vec2 impulse)
{
	vel += impulse * (1.0f / mass);
}

void PhysicsObject::AddAccel(glm::vec2 accel)
{
	assert(false);
}

void PhysicsObject::AddVelocityChange(glm::vec2 delta)
{
	vel += delta;
}