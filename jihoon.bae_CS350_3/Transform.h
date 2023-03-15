///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Transform.h>
//Purpose: <exclusive tramsform calculation >
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <26/03/2019>
//End Header --------------------------------------------------------*/
#pragma once

#include "Main/Support.h"

class CTransform
{
public:

  CTransform();
  CTransform(const Vector3& pos, const Vector3& scale, const EulerAngles& rot);

  Matrix4 GetScale() const { return mScaleMat; }
  Matrix4 GetRotation() const { return mRotationMat; }
  Matrix4 GetTranslation() const { return mTranslationMat; }
  Matrix4 GetWorld() const { return mWorld; }

  Vector3 GetPosition() const { return mPosition; }
  float GetScale() { return mScale.x; }
  void SetScale(float scale) { mScale = Vector3(scale, scale, scale); }
  void SetScale(float x, float y, float z);
  void SetPosition(Vector3 pos) { mPosition = pos; }
  void PositionY(const float& y);
  void SetRotX(float rot);
  void SetRotY(float rot);
  void SetRotZ(float rot);
  void SetRot(float x, float y, float z);
  void AddRot(Matrix3& rot);

  void Update();

private:

  Vector3 mPosition;
  EulerAngles mRotation;
  Vector3 mScale;
  
  Matrix4 mScaleMat;
  Matrix4 mRotationMat;
  Matrix4 mTranslationMat;
  Matrix4 mWorld;
};
