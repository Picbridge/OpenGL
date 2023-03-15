///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <MeshRenderer.cpp>
//Purpose: <Setting rendered buffers>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
//Project: <jjihoon.bae_CS300_3>
//Author: <jihoon.bae>
//Creation date: <26/09/2018>
//End Header --------------------------------------------------------*/
#include "MeshRenderer.h"
#include "MeshFactory.h"
#include "GFX.h"

MeshRenderer::MeshRenderer() : mMesh(0), mBV(0)
{
}

void MeshRenderer::InitBV()
{
  mBV = new MeshRenderer();
}

MeshRenderer::~MeshRenderer()
{
  delete mBV;
}

void MeshRenderer::Update(float dt)
{
  transform.Update();
  
  if(mBV)
    UpdateBV();
}

void MeshRenderer::BindMesh( const unsigned& shaderProgram )
{
  Matrix4 WorldInvTranspose = transform.GetWorld();
  (WorldInvTranspose.Invert()).Transpose();

  unsigned location;

  // Set the world matrix in the vertex shader
  location = glGetUniformLocation(shaderProgram, "worldMatrix");
  if(location == -1)
    PrintOpenGLErrors();
  glUniformMatrix4fv(location, 1, false, transform.GetWorld().array);

  // Set the worldInvTranspose matrix in the vertex shader
  location = glGetUniformLocation(shaderProgram, "worldInvTranspose");
  if(location == -1)
    PrintOpenGLErrors();
  glUniformMatrix4fv(location, 1, false, WorldInvTranspose.array);

  location = glGetUniformLocation(shaderProgram, "color");
  if(location == -1)
    PrintOpenGLErrors();
  glUniform4fv(location, 1, mBVColor.array);
}

void MeshRenderer::DrawBV()
{
  if(mBV->GetMesh())
    mBV->GetMesh()->DrawMesh();
}

void MeshRenderer::BindBV( const unsigned& shaderProgram )
{
  if(mBV->GetMesh())
  {
    mBV->BindMesh(shaderProgram);

    unsigned location;

    // Set the view matrix in the vertex shader
    location = glGetUniformLocation(shaderProgram, "color");
    if(location == -1)
      PrintOpenGLErrors();
    glUniform4fv(location, 1, mBVColor.array);
  }
}

void MeshRenderer::UpdateBV()
{
  // Update Bounding volume
  mBV->transform = transform;
  BVInfo* info = mMesh->GetBVInfo();

  switch (GFX::getInstance()->GetBVType())
  {
  case BV_NA:

    mBV->SetMesh(NULL);
    break;

  case BV_AABB:

    mBVColor = Vector4(1,0,0,.5);
    mBV->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_CUBE));
    mBV->transform.SetPosition(mBV->transform.GetPosition() + info->mAABB.Centre);
    mBV->transform.SetScale(info->mAABB.HalfExtent.x,
                            info->mAABB.HalfExtent.y,
                            info->mAABB.HalfExtent.z);
    break;

  case BV_CentroidSphere:

    mBVColor = Vector4(1,1,0,.1);
    mBV->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_SPHERE));
    mBV->transform.SetPosition(mBV->transform.GetPosition() + info->mCentroid.Centre);
    mBV->transform.SetScale(info->mCentroid.Radius);
    break;

  case BV_RitterSphere:

    mBVColor = Vector4(0,1,0,.1);
    mBV->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_SPHERE));
    mBV->transform.SetPosition(mBV->transform.GetPosition() + info->mRitters.Centre);
    mBV->transform.SetScale(info->mRitters.Radius);
    break;

  case BV_LarssonSphere:

    mBVColor = Vector4(0.3,.8,.5,.1);
    mBV->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_SPHERE));
    mBV->transform.SetPosition(mBV->transform.GetPosition() + info->mLarssons.Centre);
    mBV->transform.SetScale(info->mLarssons.Radius);
    break;

  case BV_PCASphere:

    mBVColor = Vector4(0,1,1,.1);
    mBV->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_SPHERE));
    mBV->transform.SetPosition(mBV->transform.GetPosition() + info->mPCASphere.Centre);
    mBV->transform.SetScale(info->mPCASphere.Radius);
    break;

  case BV_PCAEllipsoid:

    mBVColor = Vector4(1,0,1,.1);
    mBV->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_SPHERE));
    mBV->transform.SetPosition(mBV->transform.GetPosition() + info->mPCAEllipsoid.Centre);
    mBV->transform.SetScale(info->mPCAEllipsoid.HalfExtent.x,
                            info->mPCAEllipsoid.HalfExtent.y,
                            info->mPCAEllipsoid.HalfExtent.z);
    mBV->transform.AddRot(info->mPCAEllipsoid.JacobiRotation);
    break;

  case BV_OBB:

    mBVColor = Vector4(1,0.5f,0.5f,.7);
    mBV->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_CUBE));
    mBV->transform.SetPosition(mBV->transform.GetPosition() + info->mOBB.Centre);
    mBV->transform.SetScale(info->mOBB.HalfExtent.x,
                            info->mOBB.HalfExtent.y,
                            info->mOBB.HalfExtent.z);
    mBV->transform.AddRot(info->mOBB.JacobiRotation);
    break;
  }

  mBV->transform.Update();
}
