///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Vertex.h>
//Purpose: <Data for managing vertices>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <26/03/2019>
//End Header --------------------------------------------------------*/

#pragma once

#include "Main/Support.h"
#include "HalfEdge.h"

class HalfEdge;

class Vertex
{
public:

  Vertex();
  Vertex(Vector3 pos);
  Vertex& operator=(const Vertex& rhs);

  Vector3 mPos;
  Vector3 mColor;
  Vector3 mNormal;
  Vector3 mTangent;
  Vector3 mBinormal;
  Vector2 mUVCoords;

  HalfEdge* mHEdge; // First HEdge that starts from this vertex
};