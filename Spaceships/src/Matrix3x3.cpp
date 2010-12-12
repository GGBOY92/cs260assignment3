/*---------------------------------------------------------------------------
Project Title : CS 230: Project 2 part 1
File Name : Matrix.cpp
Author : Steven Liss
Creation Date : 8 Feb 09
Purpose : a library of Matrix functions for the asteroids game
History
----------------------------------------------------------------------------*/

#include "Matrix3x3.h"
#include <memory>
#include <cmath>

const double PI = 3.14159265;

float DegreesToRadians(float degrees)
{
  return (degrees * PI / 180.0);
}

/*
This function sets the matrix pResult to the identity matrix
*/
void Mtx33Identity(Mtx33 *pResult)
{
	for(int row = 0; row <= 2; row++)
		for(int col = 0; col <= 2; col++)
			if(col == row)
				pResult->m[row][col] = 1.0f;
			else
				pResult->m[row][col] = 0.0f;
}


//This functions calculated the transpose matrix of pMtx and saves it in pResult
void Mtx33Transpose	(Mtx33 *pResult, Mtx33 *pMtx)
{
	for(int row = 0; row <= 2; row++)
		for(int col = 0; col <= 2; col++)
			pResult->m[col][row] = pMtx->m[row][col];
}

/*
This function multiplies pMtx0 with pMtx1 and saves the result in pResult
pResult = pMtx0*pMtx1
*/
void Mtx33Concat(Mtx33 *pResult, Mtx33 *pMtx0, Mtx33 *pMtx1)
{
	if(pResult == pMtx0 || pResult == pMtx1)
	{
		Mtx33 temp;

		for(int row = 0; row <= 2; row++)
		{
			for(int col = 0; col <= 2; col++)
			{
				float sum = 0;
			
				int i = 0;
				for(int i = 0; i <= 2; i++)
				{
					sum += pMtx0->m[row][i] * pMtx1->m[i][col];
				}

				temp.m[row][col] = sum;
			}
		}

		*pResult = temp;
	} else
	{
		for(int row = 0; row <= 2; row++)
		{
			for(int col = 0; col <= 2; col++)
			{
				float sum = 0;
			
				for(int i = 0; i <= 2; i++)
				{
					sum += pMtx0->m[row][i] * pMtx1->m[i][col];
				}

				pResult->m[row][col] = sum;
			}
		}
	}
}


//This function creates a translation matrix from x & y and saves it in pResult
void Mtx33Trans(Mtx33 *pResult, float x, float y)
{
	Mtx33Identity(pResult);

	pResult->m[0][2] = x;
	pResult->m[1][2] = y;
}


//This function creates a scaling matrix from x & y and saves it in pResult
void Mtx33Scale(Mtx33 *pResult, float x, float y)
{
	Mtx33Identity(pResult);

	pResult->m[0][0] = x;
	pResult->m[1][1] = y;
}

/*
This matrix creates a rotation matrix from "angle" whose value is in degree.
Save the resultant matrix in pResult
*/
void Mtx33RotDeg(Mtx33 *pResult, float angle)
{
	float angleInRads = DegreesToRadians(angle);

	for(int row = 0; row <=2; row++)
		for(int col = 0; col <=2; col++)
			pResult->m[row][col] = 0;

	pResult->m[0][0] = cos(angleInRads);
	pResult->m[0][1] = -sin(angleInRads);
	pResult->m[1][0] = sin(angleInRads);
	pResult->m[1][1] = cos(angleInRads);
	pResult->m[2][2] = 1.0f;
}

/*
This matrix creates a rotation matrix from "angle" whose value is in radian.
Save the resultant matrix in pResult
*/
void Mtx33RotRad(Mtx33 *pResult, float angle)
{
	for(int row = 0; row <=2; row++)
		for(int col = 0; col <=2; col++)
			pResult->m[row][col] = 0;

	pResult->m[0][0] = cos(angle);
	pResult->m[0][1] = -sin(angle);
	pResult->m[1][0] = sin(angle);
	pResult->m[1][1] = cos(angle);
	pResult->m[2][2] = 1.0f;
}


//This function multiplies the matrix pMtx with the vector pVec and saves the result in pResult
void Mtx33MultVec(Vec2 *pResult, Mtx33 *pMtx, Vec2 *pVec)
{
	if(pResult == pVec)
	{
		Vec2 temp;
		
		temp.x = pMtx->m[0][0] * pVec->x + pMtx->m[0][1] * pVec->y + pMtx->m[0][2];
		temp.y = pMtx->m[1][0] * pVec->x + pMtx->m[1][1] * pVec->y + pMtx->m[1][2];
		
		*pResult = temp;
	} else
	{
		pResult->x = pMtx->m[0][0] * pVec->x + pMtx->m[0][1] * pVec->y + pMtx->m[0][2];
		pResult->y = pMtx->m[1][0] * pVec->y + pMtx->m[1][1] * pVec->y + pMtx->m[1][2];
	}
}
