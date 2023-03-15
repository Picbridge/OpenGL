///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Spherical.cpp>
//Purpose: <Information for sphere rendering>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <26/03/2019>
//End Header --------------------------------------------------------*/
#include "Spherical.h"

Spherical::Spherical() : mRadius(0),
                         mthetaHoriz(0),
                         mthetaVert(0)
{
}

Spherical::Spherical( const float& radius, const float& thetaX, const float& thetaY )
          : mRadius(radius),
            mthetaHoriz(thetaX),
            mthetaVert(thetaY)
{
}

Spherical::Spherical( const Vector3& pos )
{
  Convert(pos);
}

Vector3 Spherical::Cartesian()
{
  Vector3 coords(mRadius * sinf(mthetaHoriz) * cosf(mthetaVert),
                 mRadius * sinf(mthetaVert),
                 mRadius * cosf(mthetaHoriz) * cosf(mthetaVert));
  return coords;
}

void Spherical::Convert(const Vector3& pos)
{
  mRadius = pos.Length();
  mthetaVert = ArcTan(pos.y / pos.x);
  mthetaHoriz = ArcCos(pos.z / mRadius);
}
