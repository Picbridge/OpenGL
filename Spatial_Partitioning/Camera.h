///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <camera.h>
//Purpose: <Use camera in application>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_2>
//Author: <jihoon.bae>
//Creation date: <01/10/2018>
//End Header --------------------------------------------------------*/
#pragma once

#include "Main/Support.h"
#include "Spherical.h"
#include "CubeMapTexture.h"

class Camera
{
public:

  Camera();
  ~Camera();

  void Resize(float fov, 
              float width, 
              float height, 
              float aspectratio, 
              float near, 
              float far);
  Matrix4 GetView();
  Matrix4 GetProj();
  void EnableCubeMapRendering();
  void DisableCubeMapRendering();
  void DrawFaces(unsigned face);
  void Update(float dt);

  float GetFar();
  float GetNear();
  float GetWidth();
  float GetHeight();
  void SetPosition(Vector3& pos);
  Vector3 GetPosition();
  void SetOrigin(Vector3& origin);
  void ZoomOut(float radius);
  float GetRadius() const;
  void SetRadius(const float& r);
  CubeMapTexture* GetCubeMap();
  
  float XAxisRot() const { return mXAxisRot; }
  void XAxisRot(float val) { mXAxisRot = val; }
  float YAxisRot() const { return mYAxisRot; }
  void YAxisRot(float val) { mYAxisRot = val; }
  float ZAxisRot() const { return mZAxisRot; }
  void ZAxisRot(float val) { mZAxisRot = val; }

private:

  float mXAxisRot;
  float mYAxisRot;
  float mZAxisRot;

  float mCamOffsetY;
  float mCamOffsetX;
  
  Vector3 mPos;
  Vector3 mOrigin;
  Vector3 mLook;
  Vector3 mUp;
  Vector3 mRight;
  Spherical mSphericalCoords;

  float mWidth;
  float mHeight;
  float mAspectRatio;
  float mNear;
  float mFar;
  float mFOV;

  Matrix4 mView;
  Matrix4 mProj;

  CubeMapTexture* mCubeMap;
};