/*---------------------------------------------------------------------------
Project Title : CS 230: Project 2 part 1
File Name : Vector2.h
Author : Steven Liss
Creation Date : 8 Feb 09
Purpose : the interface for the vector library
History
----------------------------------------------------------------------------*/

#ifndef VECTOR2_H
#define VECTOR2_H

struct Vec2
{
	float x, y;
	Vec2();
	Vec2(float x, float y);
};



/*
This function sets the coordinates of the 2D vector (pResult) to 0
*/
void Vec2Zero(Vec2 *pResult);

//rotate vec in place
void Vec2RotVec(Vec2 *result, float degrees);

/*
This function sets the coordinates of the 2D vector (pResult) to x & y
*/
void Vec2Set(Vec2 *pResult, float x, float y);

/*
In this function, pResult will be set to the opposite of pVec0
*/
void Vec2Neg(Vec2 *pResult, Vec2 *pVec0);

/*
In this function, pResult will be the sum of pVec0 and pVec1
*/
void Vec2Add(Vec2 *pResult, Vec2 *pVec0, Vec2 *pVec1);

/*
In this function, pResult will be the difference between pVec0 & pVec1: pVec0 - pVec1
*/
void Vec2Sub(Vec2 *pResult, Vec2 *pVec0, Vec2 *pVec1);

/*
In this function, pResult will be the unit vector of pVec0
*/
void Vec2Normalize(Vec2 *pResult, Vec2 *pVec0);

/*
In this function, pResult will be the the vector pVec0 scaled by the value c
*/
void Vec2Scale(Vec2 *pResult, Vec2 *pVec0, float c);

/*
This function returns the length of the vector pVec0
*/
float Vec2Length(Vec2 *pVec0);

/*
This function return the square of pVec0's length
*/
float Vec2SquareLength(Vec2 *pVec0);

/*
In this function, pVec0 and pVec1 are considered as 2D points.
The distance between these 2 2D points is returned
*/
float Vec2Distance(Vec2 *pVec0, Vec2 *pVec1);

/*
In this function, pVec0 and pVec1 are considered as 2D points.
The distance between these 2 2D points is returned
*/
float Vec2SquareDistance(Vec2 *pVec0, Vec2 *pVec1);

/*
This function returns the dot product between pVec0 and pVec1
*/
float Vec2DotProduct(Vec2 *pVec0, Vec2 *pVec1);

/*
This functions return the cross product magnitude between pVec0 and pVec1
*/
float Vec2CrossProductMag(Vec2 *pVec0, Vec2 *pVec1);

#endif