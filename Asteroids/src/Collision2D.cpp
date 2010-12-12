/*---------------------------------------------------------------------------
Project Title : CS 230: Project 2 part 1
File Name : Collision2D.cpp
Author : Steven Liss
Creation Date : 8 Feb 09
Purpose : a library of collision functions for the asteroids game
History
----------------------------------------------------------------------------*/

#include "Collision2D.h"
#include <cmath>

/*
This function test if the point pPos is colliding with the circle
whose center is pCtr and radius is "radius"
*/
bool TestPointToCircle(Vec2 *pPos, Vec2 *pCtr, float radius)
{
	if(Vec2SquareDistance(pCtr, pPos) < radius * radius )
		return true;
	else
		return false;
}

/*
This function checks if the point pPos is colliding with the rectangle
whose center is pRect, width is "sizeX" and height is sizeY
*/
bool TestPointToRect(Vec2 *pPos, Vec2 *pRect, float sizeX, float sizeY)
{
	if(std::abs(pRect->x - pPos->x) <= sizeX / 2 && 
		std::abs(pRect->y - pPos->y) <= sizeY / 2)
			return true;

	return false;
}

/*
This function checks for collision between 2 circles.
Circle0: Center is pCtr0, radius is "radius0"
Circle1: Center is pCtr1, radius is "radius1"
*/
bool TestCircleToCircle(Vec2 *pCtr0, float radius0, Vec2 *pCtr1, float radius1)
{
	if(Vec2SquareDistance(pCtr0, pCtr1 ) < (radius0 * radius0 ) + (radius1 * radius1 ) )
		return true;
	else
		return false;
}

/*bool TestCircleToRect()
{
	if()
		return true;
	else
		return false;
}*/

/*
This functions checks if 2 rectangles are colliding
Rectangle0: Center is pRect0, width is "sizeX0" and height is "sizeY0"
Rectangle1: Center is pRect1, width is "sizeX1" and height is "sizeY1"
*/
bool TestRectToRect(Vec2 *pRect0, float sizeX0, float sizeY0, Vec2 *pRect1, float sizeX1, float sizeY1)
{
	if(std::abs(pRect0->x - pRect1->x) <= sizeX0 + sizeX1 && std::abs(pRect0->y - pRect1->y) <= sizeY0 / 2 + sizeY1 / 2)
		return true;

	return false;
}
