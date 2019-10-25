#include "Shapes.h"
#include"raylib.h"

#include"glm/glm.hpp"
#include"Physics.h"

typedef bool boo; //Like a bool, but spooky

boo CheckCircleCircle(glm::vec2 posA, Circle circleA, glm::vec2 posB, Circle circleB)
{
	//returns true if the distance is less than the sum of the circles' radii
	return glm::length(posB - posA) < (circleA.radius + circleB.radius);
}

boo CheckAABBAABB(glm::vec2 posA, AABB aabbA, glm::vec2 posB, AABB aabbB)
{
	return posA.x - aabbA.halfExtents.x < posB.x + aabbB.halfExtents.x && // L/R
		   posA.x + aabbA.halfExtents.x > posB.x - aabbB.halfExtents.x && // R/L
		   posA.y - aabbA.halfExtents.y < posB.y + aabbB.halfExtents.y && // T/B
		   posA.y + aabbA.halfExtents.y > posB.y - aabbB.halfExtents.y;   // B/T
}

boo CheckCircleAABB(glm::vec2 posA, Circle circle, glm::vec2 posB, AABB aabb)
{
	float distX = posA.x - glm::clamp(posA.x, posB.x - aabb.halfExtents.x, posB.x + aabb.halfExtents.x);
	float distY = posA.y - glm::clamp(posA.y, posB.y - aabb.halfExtents.y, posB.y + aabb.halfExtents.y);
	return (distX * distX + distY * distY) < (circle.radius * circle.radius);
}

boo CheckCircleX(glm::vec2 posA, Circle lhs, glm::vec2 posB, Shape rhs)
{
	return rhs.match([posA, lhs, posB](Circle s) { return CheckCircleCircle(posA, lhs, posB, s); },
					 [posA, lhs, posB](AABB s)   { return CheckCircleAABB(posA, lhs, posB, s); });
}

boo CheckAABBX(glm::vec2 posA, AABB lhs, glm::vec2 posB, Shape rhs)
{
	return rhs.match([posA, lhs, posB](Circle s) { return CheckCircleAABB(posA, s, posB, lhs); },
					 [posA, lhs, posB](AABB s)   { return CheckAABBAABB(posA, lhs, posB, s); });
}

void DrawCircleCollider(glm::vec2 pos, float radius)
{
	DrawCircleLines(pos.x, pos.y, radius, RED);
}

void DrawAABBCollider(glm::vec2 pos, glm::vec2 halfExtents)
{
	DrawRectangleLines(pos.x - halfExtents.x, pos.y - halfExtents.y,
		               halfExtents.x * 2, halfExtents.y * 2, RED);
}

void ResolvePhysicsBodies(PhysicsObject & lhs, PhysicsObject & rhs)
{
	glm::vec2 resImpulses[2];

	glm::vec2 normal = { 0,0 };
	float pen = 0.0f;

	normal = lhs.collider.match(
	[lhs, rhs, &pen](Circle c)
	{
		float dist = glm::length(lhs.pos - rhs.pos);
		float sum = lhs.collider.get<Circle>().radius + rhs.collider.get<Circle>().radius;
		pen = sum - dist;
		return glm::normalize(lhs.pos - rhs.pos);
	},
	[lhs, rhs, &pen](AABB a)
	{
		assert(false && "not yet implemented");
		return glm::vec2();
	}
	);

	ResolveCollision(lhs.pos, lhs.vel, lhs.mass, rhs.pos, rhs.vel, rhs.mass, 1.0f, normal, resImpulses);

	lhs.pos += normal * pen;
	rhs.pos -= normal * pen;

	lhs.vel = resImpulses[0];
	rhs.vel = resImpulses[1];
}

void ResolveCollision(glm::vec2 posA, glm::vec2 velA, float massA,
					  glm::vec2 posB, glm::vec2 velB, float massB,
					  float elasticity, glm::vec2 normal, glm::vec2 * dst)
{
	glm::vec2 relVel = velA - velB;
	float impulseMag = glm::dot(-(1.0f + elasticity) * relVel, normal) /
					   glm::dot(normal, normal * (1 / massA + 1 / massB));

	dst[0] = velA + (impulseMag / massA) * normal;
	dst[1] = velB - (impulseMag / massB) * normal;
}