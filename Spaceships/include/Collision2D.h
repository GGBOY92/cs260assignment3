/*---------------------------------------------------------------------------
Project Title : CS 230: Project 2 part 1
File Name : Collision2D.h
Author : Steven Liss
Creation Date : 8 Feb 09
Purpose : the collision library interface
History
----------------------------------------------------------------------------*/

#ifndef COLLISION2D_H
#define COLLISION2D_H

#include "Vector2.h"

/*
This function test if the point pPos is colliding with the circle
whose center is pCtr and radius is "radius"
*/
bool TestPointToCircle(Vec2 *pPos, Vec2 *pCtr, float radius);

/*
This function checks if the point pPos is colliding with the rectangle
whose center is pRect, width is "sizeX" and height is sizeY
*/
bool TestPointToRect(Vec2 *pPos, Vec2 *pRect, float sizeX, float sizeY);

/*
This function checks for collision between 2 circles.
Circle0: Center is pCtr0, radius is "radius0"
Circle1: Center is pCtr1, radius is "radius1"
*/
bool TestCircleToCircle(Vec2 *pCtr0, float radius0, Vec2 *pCtr1, float radius1);

/*
This functions checks if 2 rectangles are colliding
Rectangle0: Center is pRect0, width is "sizeX0" and height is "sizeY0"
Rectangle1: Center is pRect1, width is "sizeX1" and height is "sizeY1"
*/
bool TestRectToRect(Vec2 *pRect0, float sizeX0, float sizeY0, Vec2 *pRect1, float sizeX1, float sizeY1);

#endif