/*---------------------------------------------------------------------------
Project Title : CS 230: Project 2 part 1
File Name : Matrix3x3.h
Author : Steven Liss
Creation Date : 8 Feb 09
Purpose : the interface for the matrix library
History
----------------------------------------------------------------------------*/

#ifndef MATRIX3X3
#define MATRIX3X3

#include "Vector2.h"

struct Mtx33
{
	float m[3][3];
};

float DegreesToRadians(float degrees);

/*
This function sets the matrix pResult to the identity matrix
*/
void Mtx33Identity(Mtx33 *pResult);

/*
This functions calculated the transpose matrix of pMtx and saves it in pResult
*/
void Mtx33Transpose	(Mtx33 *pResult, Mtx33 *pMtx);

/*
This function multiplies pMtx0 with pMtx1 and saves the result in pResult
pResult = pMtx0*pMtx1
*/
void Mtx33Concat(Mtx33 *pResult, Mtx33 *pMtx0, Mtx33 *pMtx1);

/*
This function creates a translation matrix from x & y and saves it in pResult
*/
void Mtx33Trans(Mtx33 *pResult, float x, float y);

/*
This function creates a scaling matrix from x & y and saves it in pResult
*/
void Mtx33Scale(Mtx33 *pResult, float x, float y);

/*
This matrix creates a rotation matrix from "angle" whose value is in degree.
Save the resultant matrix in pResult
*/
void Mtx33RotDeg(Mtx33 *pResult, float angle);

/*
This matrix creates a rotation matrix from "angle" whose value is in radian.
Save the resultant matrix in pResult
*/
void Mtx33RotRad(Mtx33 *pResult, float angle);

/*
This function multiplies the matrix pMtx with the vector pVec and saves the result in pResult
*/
void Mtx33MultVec(Vec2 *pResult, Mtx33 *pMtx, Vec2 *pVec);


#endif