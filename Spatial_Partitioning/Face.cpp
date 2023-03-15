///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Face.cpp>
//Purpose: <Faces used in trees>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <Spatial_Partitioning>
//Author: <jihoon.bae>
//Creation date: <28/03/2019>
//End Header --------------------------------------------------------*/
#include "Face.h"

Face::Face() : mHEdge(NULL), mVisited(false)
{
}

unsigned Face::GetNumofFreeNeighbors()
{
  unsigned count = 0;
  if(mNeighbors.size())
  {   
    std::vector<HalfEdge*>::iterator iter;

    for(iter = mNeighbors.begin(); iter != mNeighbors.end(); ++iter)
    {
      if( !(*iter)->mFace->mVisited )
        ++count;
    }
  }

  return count;
}

HalfEdge* Face::FindSharedHalfEdge(Face* f)
{
  std::vector<HalfEdge*>::iterator iter;

  for(iter = mNeighbors.begin(); iter != mNeighbors.end(); ++iter)
  {
    if( (*iter)->mFace == f )
      return ((*iter)->mTwin);
  }

  return NULL;
}
