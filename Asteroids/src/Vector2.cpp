/*---------------------------------------------------------------------------
Project Title : CS 230: Project 2 part 1
File Name : Vector2.cpp
Author : Steven Liss
Creation Date : 8 Feb 09
Purpose : a library of vector functions for the asteroids game
History
----------------------------------------------------------------------------*/

#pragma once
#include "Matrix3x3.h"

#include "Vector2.h"
#include <cmath>

Vec2::Vec2()
{

}

Vec2::Vec2(float x, float y)
{
	this->x = x;
	this->y = y;
}

void Vec2Zero(Vec2 *pResult)
{
	pResult->x = 0;
	pResult->y = 0;
}

//in place rotation of the vector
void Vec2RotVec(Vec2 *result, float angle)
{
	float degrees = DegreesToRadians(angle);

	float tempx = cosf(degrees) * result->x - sinf(degrees) * result->y;
	float tempy = sinf(degrees) * result->x + cosf(degrees) * result->y;

	result->x = tempx;
	result->y = tempy;
}

void Vec2Set(Vec2 *pResult, float x, float y)
{
	pResult->x = x;
	pResult->y = y;
}

void Vec2Neg(Vec2 *pResult, Vec2 *pVec0)
{
	pResult->x = -pVec0->x;
	pResult->y = -pVec0->y;
}

//In this function, pResult will be the sum of pVec0 and pVec1
void Vec2Add(Vec2 *pResult, Vec2 *pVec0, Vec2 *pVec1)
{
	pResult->x = pVec0->x + pVec1->x;
	pResult->y = pVec0->y + pVec1->y;
}

//In this function, pResult will be the difference between pVec0 & pVec1: pVec0 - pVec1
void Vec2Sub(Vec2 *pResult, Vec2 *pVec0, Vec2 *pVec1)
{
	pResult->x = pVec0->x - pVec1->x;
	pResult->y = pVec0->y - pVec1->y;
}

//In this function, pResult will be the unit vector of pVec0
void Vec2Normalize(Vec2 *pResult, Vec2 *pVec0)
{
	float mag = Vec2Length(pVec0);

	pResult->x = pVec0->x / mag;
	pResult->y = pVec0->y / mag;

}

//In this function, pResult will be the the vector pVec0 scaled by the value c
void Vec2Scale(Vec2 *pResult, Vec2 *pVec0, float c)
{
	pResult->x = pVec0->x * c;
	pResult->y = pVec0->y * c;
}

//This function returns the length of the vector pVec0
float Vec2Length(Vec2 *pVec0)
{
	return sqrt((pVec0->x * pVec0->x ) + (pVec0->y * pVec0->y ) );
}

//This function return the square of pVec0's length
float Vec2SquareLength(Vec2 *pVec0)
{
	return (pVec0->x * pVec0->x ) + (pVec0->y * pVec0->y );
}

/*
In this function, pVec0 and pVec1 are considered as 2D points.
The distance between these 2 2D points is returned
*/
float Vec2Distance(Vec2 *pVec0, Vec2 *pVec1)
{
	return sqrt((pVec0->x - pVec1->x ) * (pVec0->x - pVec1->x ) + (pVec0->y - pVec1->y ) * (pVec0->y - pVec1->y ) );
}

/*
In this function, pVec0 and pVec1 are considered as 2D points.
The distance between these 2 2D points is returned
*/
float Vec2SquareDistance(Vec2 *pVec0, Vec2 *pVec1)
{
	return (pVec0->x - pVec1->x ) * (pVec0->x - pVec1->x ) + (pVec0->y - pVec1->y ) * (pVec0->y - pVec1->y ); 
}

//This function returns the dot product between pVec0 and pVec1
float Vec2DotProduct(Vec2 *pVec0, Vec2 *pVec1)
{
	return pVec0->x * pVec1->x + pVec0->y * pVec1->y;
}

//This functions return the cross product magnitude between pVec0 and pVec1
float Vec2CrossProductMag(Vec2 *pVec0, Vec2 *pVec1)
{
	return pVec0->x * pVec1->y - pVec1->x * pVec0->y;
}
