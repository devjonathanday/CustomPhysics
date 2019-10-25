#include "Shapes.h"
#include"raylib.h"

#include"glm/glm.hpp"

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

bool CheckCircleX(glm::vec2 posA, Circle lhs, glm::vec2 posB, Shape rhs)
{
	return rhs.match([posA, lhs, posB](Circle s) { return CheckCircleCircle(posA, lhs, posB, s); },
					 [posA, lhs, posB](AABB s)   { return CheckCircleAABB(posA, lhs, posB, s); });
}

bool CheckAABBX(glm::vec2 posA, AABB lhs, glm::vec2 posB, Shape rhs)
{
	return rhs.match([posA, lhs, posB](Circle s) { return CheckCircleAABB(posA, s, posB, lhs); },
					 [posA, lhs, posB](AABB s)   { return CheckAABBAABB(posA, lhs, posB, s); });
}

void Draw(glm::vec2 pos)
{

}

//void Circle::Draw(glm::vec2 pos)
//{
//	DrawCircleLines(pos.x, pos.y, 15.0f, RED);
//}
//void AABB::Draw(glm::vec2 pos)
//{
//	DrawRectangleLines(pos.x - halfExtents.x, pos.y - halfExtents.y,
//					   pos.x + halfExtents.x, pos.y + halfExtents.y, RED);
//}