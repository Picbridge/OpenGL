#pragma once

#include "HalfEdge.h"
///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Face.h>
//Purpose: <Faces used in trees>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <28/03/2019>
//End Header --------------------------------------------------------*/
class HalfEdge;



class Face
{
public:

  Face();
  unsigned GetNumofFreeNeighbors();
  HalfEdge* FindSharedHalfEdge(Face* f);

  struct sortFaces
  {
    bool operator()( Face* lhs, Face* rhs ) const
    {
      return lhs->mNeighbors.size() > rhs->mNeighbors.size();
    }
  };

  HalfEdge* mHEdge;
  Vector3 mFaceNormal;
  Vector3 mTangent;
  Vector3 mBinormal;

  std::vector<HalfEdge*> mNeighbors;
  bool mVisited;    // For triangle strip
};

