#pragma once

#include"glm/vec2.hpp"

#include"mapbox/variant.hpp"

struct Circle
{
	float radius;
};

struct AABB
{
	glm::vec2 halfExtents;
};

using Shape = mapbox::util::variant<Circle, AABB>;

bool CheckCircleCircle(glm::vec2 posA, Circle circleA, glm::vec2 posB, Circle circleB);
bool CheckAABBAABB(glm::vec2 posA, AABB aabbA, glm::vec2 posB, AABB aabbB);
bool CheckAABBPoint(glm::vec2 point, AABB aabb, glm::vec2 pos);
bool CheckCircleAABB(glm::vec2 posA, Circle circle, glm::vec2 posB, AABB aabb);

bool CheckCircleX(glm::vec2 posA, Circle lhs, glm::vec2 posB, Shape rhs);
bool CheckAABBX(glm::vec2 posA, AABB lhs, glm::vec2 posB, Shape rhs);

void DrawCircleCollider(glm::vec2 pos, float radius);
void DrawAABBCollider(glm::vec2 pos, glm::vec2 halfExtents);

void ResolvePhysicsBodies(class PhysicsObject& lhs, class PhysicsObject& rhs);

void ResolveCollision(glm::vec2 posA, glm::vec2 velA, float massA,
					  glm::vec2 posB, glm::vec2 velB, float massB,
					  float elasticity, glm::vec2 normal, glm::vec2* dst);

//glm::vec2 GetAABBCirclePen(PhysicsObject lhs, PhysicsObject rhs);