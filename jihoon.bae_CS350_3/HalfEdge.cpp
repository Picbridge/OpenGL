///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <HalfEdge.cpp>
//Purpose: <Halfwidth used in octree, bsptree>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <28/03/2019>
//End Header --------------------------------------------------------*/
#include "HalfEdge.h"

HalfEdge::HalfEdge() : mVertex(NULL),
                       mFace(NULL),
                       mNext(NULL),
                       mTwin(NULL)
{
}

HalfEdge* HalfEdge::Prev()
{
  return mNext->mNext;
}
