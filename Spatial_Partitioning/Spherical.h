///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Spherical.h>
//Purpose: <Information for sphere rendering>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <Spatial_Partitioning>
//Author: <jihoon.bae>
//Creation date: <26/03/2019>
//End Header --------------------------------------------------------*/
#pragma once
#include "Main/Support.h"

class Spherical
{
public:

  Spherical();
  Spherical(const float& radius, const float& thetaX, const float& thetaY);
  Spherical(const Vector3& pos);

  Vector3 Cartesian();
  void Convert(const Vector3& pos);

  float Radius() const { return mRadius; }
  void Radius(float val) { mRadius = val; }
  float ThetaHoriz() const { return mthetaHoriz; }
  void ThetaHoriz(float val) { mthetaHoriz = val; }
  float ThetaVert() const { return mthetaVert; }
  void ThetaVert(float val) { mthetaVert = val; }

private:

  float mRadius;
  float mthetaHoriz;
  float mthetaVert;
  
};
