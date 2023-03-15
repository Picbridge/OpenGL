///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Light.h>
//Purpose: <Give informations about light>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
//Project: <Spatial_Partitioning>
//Author: <jihoon.bae>
//Creation date: <28/09/2018>
//End Header --------------------------------------------------------*/
#pragma once

#include "Main/Support.h"
#include "MeshRenderer.h"

#define MAXLIGHTS 8
static unsigned LightID = 0;
static float YLightRot = 0;

struct LightCount
{
  unsigned mDir;
  unsigned mPoint;
  unsigned mSpot;
  unsigned mTotal;
};

enum LIGHTTYPE
{
  LIGHT_DIRECTIONAL,
  LIGHT_POINT,
  LIGHT_SPOT
};

class Light
{
public:

  Light();
  ~Light();
  void InitializePosition();
  void RotateAboutCentre();
  void BindLightMesh(unsigned shaderProgram);
  void Update(float dt);
  void DrawDebugLines();
  void Default();
  void Randomize();

  void Position(const Vector3& pos);
  void Position(const float& x, const float& y, const float& z);
  MeshRenderer* GetMeshRenderer() { return mMeshRenderer; }

  struct sortLights 
  {
    bool operator()( Light* lhs, Light* rhs ) const
    { return lhs->mType < rhs->mType; }
  };

  Vector4 mAmbient;
  Vector4 mDiffuse;
  Vector4 mSpecular;

  Vector3 mPosition;
  Vector3 mDirection;     // Direction for spotlight and directional light
  Vector3 mAttenuation;   // Dist att

  float mInnerAngle;      // Inner spotlight angle (DEGREES)
  float mOuterAngle;      // Outer spotlight angle (DEGREES)
  float mSpotFalloff;

  LIGHTTYPE mType; 
  unsigned mID;

  Vector3 mDefaultPos;
  MeshRenderer* mMeshRenderer;
};