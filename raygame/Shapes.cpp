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

boo CheckAABBPoint(glm::vec2 point, AABB aabb, glm::vec2 pos)
{
	return point.x > pos.x - aabb.halfExtents.x &&
		   point.x < pos.x + aabb.halfExtents.x &&
		   point.y > pos.y - aabb.halfExtents.y &&
		   point.y < pos.y + aabb.halfExtents.y;
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

//glm::vec2 GetAABBCirclePen(PhysicsObject lhs, PhysicsObject rhs)
//{
//	//lhs = circle
//	//rhs = aabb
//
//	//Get closest point on edge of circle
//	glm::vec2 closestPoint = glm::normalize(rhs.pos - lhs.pos) * lhs.collider.get<Circle>().radius;
//	glm::vec2 penDir = lhs.pos - closestPoint;
//	glm::vec2 penVec = { 0,0 };
//
//	if (abs(penDir.y) > abs(penDir.x)) //Pointing at a top or bottom edge
//	{
//		if (penDir.y >= 0) //Pointing at the bottom
//		{
//			float worldPosEdgeY = rhs.pos.y + rhs.collider.get<AABB>().halfExtents.y;
//			float distToEdgeY = worldPosEdgeY - closestPoint.y;
//			float edgePosX = closestPoint.x + ((penDir.y / penDir.x) * distToEdgeY);
//			return (edgePosX, worldPosEdgeY) - closestPoint;
//		}
//		else //Pointing at the top
//		{
//			float worldPosEdgeY = rhs.pos.y - rhs.collider.get<AABB>().halfExtents.y;
//			float distToEdgeY = closestPoint.y - worldPosEdgeY;
//			float edgePosX = closestPoint.x + ((penDir.y / penDir.x) * distToEdgeY);
//			return closestPoint - (edgePosX, worldPosEdgeY);
//		}
//	}
//	else //Pointing at a left or right edge
//	{
//		if (penDir.x >= 0) //Pointing at the right
//		{
//			float worldPosEdgeX = rhs.pos.x + rhs.collider.get<AABB>().halfExtents.x;
//			float distToEdgeX = worldPosEdgeX - closestPoint.x;
//			float edgePosY = closestPoint.y + ((penDir.y / penDir.x) * distToEdgeX);
//			return (worldPosEdgeX, edgePosY) - closestPoint;
//		}
//		else //Pointing at the left
//		{
//			float worldPosEdgeX = rhs.pos.x - rhs.collider.get<AABB>().halfExtents.x;
//			float distToEdgeX = closestPoint.x - worldPosEdgeX;
//			float edgePosY = closestPoint.y + ((penDir.y / penDir.x) * distToEdgeX);
//			return closestPoint - (worldPosEdgeX, edgePosY);
//		}
//	}
//}

void ResolvePhysicsBodies(PhysicsObject & lhs, PhysicsObject & rhs)
{
	glm::vec2 resImpulses[2];

	glm::vec2 normal = { 0,0 };
	float pen = 0.0f;

	normal = lhs.collider.match(
	[lhs, rhs, &pen](Circle c)
	{
		return rhs.collider.match(
		[lhs, rhs, &pen](Circle c)
		{
			float dist = glm::length(lhs.pos - rhs.pos);
			float sum = lhs.collider.get<Circle>().radius + rhs.collider.get<Circle>().radius;
			pen = sum - dist;
			return glm::normalize(lhs.pos - rhs.pos);
		},
		[lhs, rhs, &pen](AABB a)
		{
			//lhs = circle
			//rhs = aabb

			//Point on edge of circle, closest to center of AABB (world space)
			glm::vec2 closestPoint = lhs.pos + glm::normalize(rhs.pos - lhs.pos) * lhs.collider.get<Circle>().radius;
			//Vector from center of circle to point on edge
			glm::vec2 penDir = lhs.pos - closestPoint;
			//Clamp the point to the extents of the AABB (world space)
			float distToEdgeX = (rhs.pos.x + rhs.collider.get<AABB>().halfExtents.x - closestPoint.x);
			float distToEdgeY = (rhs.pos.y + rhs.collider.get<AABB>().halfExtents.y - closestPoint.y);
			penDir.x = glm::clamp(penDir.x, -distToEdgeX, distToEdgeX);
			penDir.x = glm::clamp(penDir.x, -distToEdgeY, distToEdgeY);
			//Set the pen amount and direction
			pen = glm::length(penDir);
			return glm::normalize(penDir);
		});
	},
	[lhs, rhs, &pen](AABB a)
	{
		return rhs.collider.match(
		[lhs, rhs, &pen](Circle c)
		{
			//rhs = circle
			//lhs = aabb

			//Point on edge of circle, closest to center of AABB (world space)
			glm::vec2 closestPoint = rhs.pos + glm::normalize(lhs.pos - rhs.pos) * rhs.collider.get<Circle>().radius;
			//Vector from center of circle to point on edge
			glm::vec2 penDir = rhs.pos - closestPoint;
			//Clamp the point to the extents of the AABB (world space)
			float distToEdgeX = (lhs.pos.x + lhs.collider.get<AABB>().halfExtents.x - closestPoint.x);
			float distToEdgeY = (lhs.pos.y + lhs.collider.get<AABB>().halfExtents.y - closestPoint.y);
			penDir.x = glm::clamp(penDir.x, -distToEdgeX, distToEdgeX);
			penDir.x = glm::clamp(penDir.x, -distToEdgeY, distToEdgeY);
			//Set the pen amount and direction
			pen = glm::length(penDir);
			return glm::normalize(penDir);
		},
		[lhs, rhs, &pen](AABB a)
		{
			//lhs = left collider
			//rhs = right collider
			glm::vec2 start = lhs.pos;
			glm::vec2 end = rhs.pos;

			glm::vec2 lhsTopLeft = { lhs.pos.x - lhs.collider.get<AABB>().halfExtents.x, lhs.pos.y - lhs.collider.get<AABB>().halfExtents.y };
			glm::vec2 lhsBottomRight = { lhs.pos.x + lhs.collider.get<AABB>().halfExtents.x, lhs.pos.y + lhs.collider.get<AABB>().halfExtents.y };

			glm::vec2 rhsTopLeft = { rhs.pos.x - rhs.collider.get<AABB>().halfExtents.x, rhs.pos.y - rhs.collider.get<AABB>().halfExtents.y };
			glm::vec2 rhsBottomRight = { rhs.pos.x + rhs.collider.get<AABB>().halfExtents.x, rhs.pos.y + rhs.collider.get<AABB>().halfExtents.y };

			start = glm::clamp(start, lhsTopLeft, lhsBottomRight);
			end = glm::clamp(end, lhsTopLeft, lhsBottomRight);

			start = glm::clamp(start, rhsTopLeft, rhsBottomRight);
			end = glm::clamp(end, rhsTopLeft, rhsBottomRight);

			glm::vec2 penVec = start - end;
			pen = glm::length(penVec);
			return glm::normalize(penVec);
		});
	});

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

