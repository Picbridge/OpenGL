///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Light.cpp>
//Purpose: <Give informations about light>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
//Project: <Spatial_Partitioning>
//Author: <jihoon.bae>
//Creation date: <28/09/2018>
//End Header --------------------------------------------------------*/
#include "Light.h"
#include "GFX.h"
#include "MeshFactory.h"

Light::Light()
{
  mID = LightID++;

  mAmbient = Vector4(0,0,0,0);
  mDiffuse = Vector4(0.8f,0.8f,0.8f,1);
  mSpecular = Vector4(1,1,1,1);

  MeshRenderer* gameObject = NULL;// GFX::getInstance()->GetMeshRenderer();
  if(GFX::getInstance()->GetMeshRenderer())
	  gameObject = GFX::getInstance()->GetMeshRenderer();

  if (gameObject)
  {
	  float maxY = gameObject->GetMesh()->GetMax().y * gameObject->transform.GetScale();

	  InitializePosition();
	  mDirection = gameObject->transform.GetPosition() - mPosition;
	  mDirection.Normalize();	 
  }
  mAttenuation = Vector3(1, .1f, 0);
  mInnerAngle = 15;  
  mOuterAngle = 30;
  mSpotFalloff = 1;

  mType = LIGHT_POINT;

  mMeshRenderer = new MeshRenderer();
  mMeshRenderer->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_SPHERE));
  mMeshRenderer->transform.SetScale(.05f);
}

void Light::Update(float dt)
{
  if(GFX::getInstance()->IsEnabledRotateLights())
  {
    if(mID == GFX::getInstance()->GetActiveLights() - 1)
      YLightRot += .5f;

    RotateAboutCentre();
  }

  MeshRenderer* mr = GFX::getInstance()->GetMeshRenderer();
  if (mr)
  {
	  Vector3 mrPos = mr->transform.GetPosition();
	  mDirection = mrPos - mPosition;
	  mDirection.Normalize();

	  mMeshRenderer->transform.SetPosition(mPosition);
	  mMeshRenderer->Update(dt);
  }
}

void Light::InitializePosition()
{
  MeshRenderer* mr = GFX::getInstance()->GetMeshRenderer();
  if (mr)
  {
	  Vector3 max = mr->GetMesh()->GetMax();

	  //float ymax = max.y;
	  //max.y = 0;
	  float length = max.Length() * 10.f;
	  max.Normalize();
	  max *= length;

	  mDefaultPos = Vector3(8, 8, 8);
	  Randomize();
	  RotateAboutCentre();
  }
}

void Light::RotateAboutCentre()
{
  float ang = YLightRot + mID * 45;
  EulerAngles angle(0, DegToRad(ang), 0, EulerOrders::XYZs);
  Matrix4 rot;
  Math::ToMatrix4(angle, &rot);

  mPosition = TransformPoint(rot, mDefaultPos);
}

void Light::Position( const Vector3& pos )
{
  mPosition = pos;
}

void Light::Position( const float& x, const float& y, const float& z )
{
  mPosition.x = x;
  mPosition.y = y;
  mPosition.z = z;
}

void Light::DrawDebugLines()
{
  glLineWidth(2.5);
  glColor3f(0.f, 1.0f, 0.0f);

  glBegin(GL_LINES);
  
  glVertex3f(mPosition.x, mPosition.y, mPosition.z);
  glVertex3f(mPosition.x + mDirection.x, mPosition.y + mDirection.y, mPosition.z + mDirection.z);

  glEnd();
}

Light::~Light()
{
  --LightID;
  delete mMeshRenderer;
}

void Light::BindLightMesh(unsigned shaderProgram)
{
  mMeshRenderer->BindMesh(shaderProgram);

  unsigned location;   

  // Set the view matrix in the vertex shader
  location = glGetUniformLocation(shaderProgram, "color");
  if(location == -1)
    PrintOpenGLErrors();
  glUniform4fv(location, 1, mDiffuse.array);
}

void Light::Randomize()
{
  mAmbient = Vector4(0,0,0,0);
  mDiffuse = Vector4(RandomFloat(0,1),RandomFloat(0,1),RandomFloat(0,1),1);
  mSpecular = Vector4(RandomFloat(0,1),RandomFloat(0,1),RandomFloat(0,1),1);

  int LType = 0;// rand() % 3;

  switch(LType)
  {
  case 0:
    mType = LIGHT_DIRECTIONAL;
    break;

  case 1:
    mType = LIGHT_POINT;
    break;

  case 2:
    mType = LIGHT_SPOT;
    break;
  }
}

void Light::Default()
{
  mAmbient = Vector4(0,0,0,0);
  mDiffuse = Vector4(0.8f,0.8f,0.8f,1);
  mSpecular = Vector4(1,1,1,1);
  mType = LIGHT_POINT;
}
