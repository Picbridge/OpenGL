///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Transform.cpp>
//Purpose: <exclusive tramsform calculation >
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <26/03/2019>
//End Header --------------------------------------------------------*/
#include "Transform.h"

CTransform::CTransform() : mRotation(0,0,0, EulerOrders::XYZs)
{
  mScaleMat.SetIdentity();
  mRotationMat.SetIdentity();
  mTranslationMat.SetIdentity();
  mWorld.SetIdentity();

  mPosition = Vector3(0,0,0);
  mScale = Vector3(1,1,1);
}

CTransform::CTransform( const Vector3& pos, const Vector3& scale, const EulerAngles& rot ) 
           : mRotation(rot),
             mPosition(pos),
             mScale(scale)
{
}

void CTransform::Update()
{
  mScaleMat.Scale(mScale);

  Math::ToMatrix4(mRotation, &mRotationMat);

  mTranslationMat.Translate(mPosition.x, mPosition.y, mPosition.z);

  mTranslationMat.Transpose();
  mRotationMat.Transpose();

  //mRotationMat = mTranslationMat * mRotationMat * mTranslationMat.Inverted();

  mWorld = mScaleMat * mRotationMat * mTranslationMat;
}

void CTransform::SetRotX( float rot )
{
  mRotation.Angles.x = Math::DegToRad(rot);
}

void CTransform::SetRotY( float rot )
{
  mRotation.Angles.y = Math::DegToRad(rot);
}

void CTransform::SetRot( float x, float y, float z )
{
  mRotation.Angles.x = Math::DegToRad(x);
  mRotation.Angles.y = Math::DegToRad(y);
  mRotation.Angles.z = Math::DegToRad(z);
}

void CTransform::PositionY(const float& y )
{
  mPosition.y = y;
}

void CTransform::SetScale( float x, float y, float z )
{
  mScale.x = x;
  mScale.y = y;
  mScale.z = z;
}

void CTransform::AddRot( Matrix3& rot )
{
  Matrix4 m = ToMatrix4(rot);
  m = mRotationMat.Transpose() * m;

  mRotation = ToEulerAngles(m, EulerOrders::XYZs);
}
