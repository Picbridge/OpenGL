///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Vertex.cpp>
//Purpose: <Data for managing vertices>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <Spatial_Partitioning>
//Author: <jihoon.bae>
//Creation date: <26/03/2019>
//End Header --------------------------------------------------------*/

#include "Vertex.h"

Vertex::Vertex()
{
  mPos = Vector3(0,0,0);
  mColor = Vector3(1,1,1);
  mNormal = Vector3(0.f);
  mTangent = Vector3(0.f);
  mBinormal = Vector3(0.f);
  mUVCoords = Vector2(0,0);
  mHEdge = NULL;
}

Vertex::Vertex( Vector3 pos )
{
  mPos = pos;
  mColor = Vector3(1,1,1);
  mNormal = Vector3(0.f);
  mTangent = Vector3(0.f);
  mBinormal = Vector3(0.f);
  mUVCoords = Vector2(0,0);
}

Vertex& Vertex::operator=( const Vertex& rhs )
{
  mPos = rhs.mPos;
  mColor = rhs.mColor;
  mNormal = rhs.mNormal;
  mTangent = rhs.mTangent;
  mBinormal = rhs.mBinormal;
  mUVCoords = rhs.mUVCoords;

  return *this;
}
