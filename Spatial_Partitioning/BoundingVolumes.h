/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <BoundingVolume.h>
Purpose: <Load objects and set basic position,scale info>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <Spatial_Partitioning>
Author: <jihoon.bae>
Creation date: <23/02/2019>
End Header --------------------------------------------------------*/
#pragma once

#include "Main/Support.h"
#include "Mesh.h"
#include "Transform.h"

typedef enum
{
  BV_NA,
  BV_AABB,
  BV_OBB,
  BV_CentroidSphere,
  BV_RitterSphere,
  BV_LarssonSphere,
  BV_PCASphere,
  BV_PCAEllipsoid,  

  TOTAL

} BVType;

struct AABB 
{
  Vector3 Centre;
  Vector3 HalfExtent;
};
struct RawFace
{
    Vector3 mVertices[3];
};
struct Sphere
{
  Vector3 Centre;
  float Radius;

  Sphere() : Radius(0) {}
};

struct Ellipsoid
{
  Vector3 Centre;
  Vector3 HalfExtent;
  Matrix3 JacobiRotation;
};

struct OBB
{
  Vector3 Centre;
  Vector3 HalfExtent;
  Matrix3 JacobiRotation;
};

struct BVInfo
{
  AABB mAABB;
  Sphere mCentroid;
  Sphere mRitters;
  Sphere mLarssons;
  Sphere mPCASphere;
  Ellipsoid mPCAEllipsoid;
  OBB mOBB;
};

static const Vector3 LarssonNormals[] =
{
  Vector3(1, 0, 0),  Vector3(0, 1, 0),  Vector3(0, 0, 1),  Vector3(1, 1, 1), 
  Vector3(1, 1, -1), Vector3(1, -1, 1), Vector3(1, -1, -1),Vector3(1, 1, 0), 
  Vector3(1, -1, 0), Vector3(1, 0, 1),  Vector3(1, 0, -1), Vector3(0, 1, 1), 
  Vector3(0, 1, -1), Vector3(0, 1, 2),  Vector3(0, 2, 1),  Vector3(1, 0, 2), 
  Vector3(2, 0, 1),  Vector3(1, 2, 0),  Vector3(2, 1, 0),  Vector3(0, 1, -2), 
  Vector3(0, 2, -1), Vector3(1, 0, -2), Vector3(2, 0, -1), Vector3(1, -2, 0), 
  Vector3(2, -1, 0), Vector3(1, 1, 2),  Vector3(2, 1, 1),  Vector3(1, 2, 1), 
  Vector3(1, -1, 2), Vector3(1, 1, -2), Vector3(1, -1, -2),Vector3(2, -1, 1), 
  Vector3(2, 1, -1), Vector3(2, -1, -1),Vector3(1, -2, 1), Vector3(1, 2, -1), 
  Vector3(1, -2, -1),Vector3(2, 2, 1),  Vector3(1, 2, 2),  Vector3(2, 1, 2), 
  Vector3(2, -2, 1), Vector3(2, 2, -1), Vector3(2, -2, -1),Vector3(1, -2, 2), 
  Vector3(1, 2, -2), Vector3(1, -2, -2),Vector3(2, -1, 2), Vector3(2, 1, -2), 
  Vector3(2, -1, -2)
};
static const Vector4 Colors[] =
{
    Vector4(1, 0, 0, .1f), Vector4(1, .5, 0, .1f), Vector4(1, 1, 0, .1f),
    Vector4(0, 1, 0, .1f), Vector4(0, 0, 1, .1f), Vector4(.29f, 0, .51f, .1f),
    Vector4(.56f, 0, 1, .1f), Vector4(.29f, .51f, .51f, .1f), Vector4(.29f, .81f, .51f, .1f)
};

static Vector4 GetColor(unsigned index)
{
    unsigned totalCol = sizeof(Colors) / sizeof(Vector4);
    return Colors[index % totalCol];
}
static void DrawAABB(AABB& box, Vector4 color)
{
    Vector3 topLeftF, topRightF, botLeftF, botRightF,
        topLeftB, topRightB, botLeftB, botRightB;

    topLeftF = topRightF = botLeftF = botRightF =
        topLeftB = topRightB = botLeftB = botRightB = box.Centre;

    topRightF += box.HalfExtent;
    botLeftB -= box.HalfExtent;

    topLeftF.x -= box.HalfExtent.x;
    topLeftF.y += box.HalfExtent.y;
    topLeftF.z += box.HalfExtent.z;

    botLeftF.x -= box.HalfExtent.x;
    botLeftF.y -= box.HalfExtent.y;
    botLeftF.z += box.HalfExtent.z;

    botRightF.x += box.HalfExtent.x;
    botRightF.y -= box.HalfExtent.y;
    botRightF.z += box.HalfExtent.z;

    topLeftB.x -= box.HalfExtent.x;
    topLeftB.y += box.HalfExtent.y;
    topLeftB.z -= box.HalfExtent.z;

    topRightB.x += box.HalfExtent.x;
    topRightB.y += box.HalfExtent.y;
    topRightB.z -= box.HalfExtent.z;

    botRightB.x += box.HalfExtent.x;
    botRightB.y -= box.HalfExtent.y;
    botRightB.z -= box.HalfExtent.z;

    glColor4f(color.x, color.y, color.z, 1.0f);

    glBegin(GL_LINES);

    glVertex3f(topRightF.x, topRightF.y, topRightF.z);
    glVertex3f(topLeftF.x, topLeftF.y, topLeftF.z);
    glVertex3f(topRightF.x, topRightF.y, topRightF.z);
    glVertex3f(topRightB.x, topRightB.y, topRightB.z);
    glVertex3f(topRightF.x, topRightF.y, topRightF.z);
    glVertex3f(botRightF.x, botRightF.y, botRightF.z);

    glVertex3f(topLeftB.x, topLeftB.y, topLeftB.z);
    glVertex3f(botLeftB.x, botLeftB.y, botLeftB.z);
    glVertex3f(topLeftB.x, topLeftB.y, topLeftB.z);
    glVertex3f(topLeftF.x, topLeftF.y, topLeftF.z);
    glVertex3f(topLeftB.x, topLeftB.y, topLeftB.z);
    glVertex3f(topRightB.x, topRightB.y, topRightB.z);

    glVertex3f(botLeftF.x, botLeftF.y, botLeftF.z);
    glVertex3f(botLeftB.x, botLeftB.y, botLeftB.z);
    glVertex3f(botLeftF.x, botLeftF.y, botLeftF.z);
    glVertex3f(topLeftF.x, topLeftF.y, topLeftF.z);
    glVertex3f(botLeftF.x, botLeftF.y, botLeftF.z);
    glVertex3f(botRightF.x, botRightF.y, botRightF.z);

    glVertex3f(botRightB.x, botRightB.y, botRightB.z);
    glVertex3f(botLeftB.x, botLeftB.y, botLeftB.z);
    glVertex3f(botRightB.x, botRightB.y, botRightB.z);
    glVertex3f(topRightB.x, topRightB.y, topRightB.z);
    glVertex3f(botRightB.x, botRightB.y, botRightB.z);
    glVertex3f(botRightF.x, botRightF.y, botRightF.z);

    glEnd();
}