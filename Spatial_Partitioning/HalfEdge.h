///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <HalfEdge.cpp>
//Purpose: <Halfwidth used in octree, bsptree>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <Spatial_Partitioning>
//Author: <jihoon.bae>
//Creation date: <28/03/2019>
//End Header --------------------------------------------------------*/
#pragma once

#include "Vertex.h"
#include "Face.h"

class Vertex;
class Face;

class HalfEdge
{
public:

  HalfEdge();
  HalfEdge* Prev();

  Vertex* mVertex; // Vertex half edge is pointing to
  Face* mFace;     // Face half edge is encapsulating CCW
  HalfEdge* mNext; // The next half edge
  HalfEdge* mTwin; // The twin of this half edge (different direction)
};