/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <MeshFactory.cpp>
Purpose: <Manages the multiple mesh>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <jihoon.bae_CS300_3>
Author: <jihoon.bae>
Creation date: <28/03/2018>
End Header --------------------------------------------------------*/
#include "MeshFactory.h"

MeshFactory::MeshFactory() : mCube(0), mBunny(0), mHorse(0), mPlane(0),
                             mSphere(0), mTeapot(0)
{
}

Mesh* MeshFactory::GetMesh( MESHTYPE mtype )
{
  Mesh* mesh = 0;

  switch (mtype)
  {
  case MESH_CUBE:

    if(!mCube)
    {
      mCube = new Mesh();
      mCube->LoadFromObj("cube_low_poly", PLANAR);
      mCube->InitializeBuffers();
    }

    mesh = mCube;
    break;

  case MESH_BUNNY:

    if(!mBunny)
    {
      mBunny = new Mesh();
      mBunny->LoadFromObj("bunny_high_poly", SPHERICAL);
      mBunny->InitializeBuffers();
    }

    mesh = mBunny;
    break;

  case MESH_HORSE:

    if(!mHorse)
    {  
      mHorse = new Mesh();
      mHorse->LoadFromObj("horse_high_poly", SPHERICAL);
      mHorse->InitializeBuffers();
    }

    mesh = mHorse;
    break;

  case MESH_PLANE:

    if(!mPlane)
    {
      mPlane = new Mesh();
      mPlane->LoadFromObj("plane_low_poly");
      mPlane->InitializeBuffers();
    }    

    mesh = mPlane;
    break;

  case MESH_SPHERE:

    if(!mSphere)
    {
      mSphere = new Mesh();
      mSphere->LoadFromObj("sphere_high_poly", SPHERICAL);
      mSphere->InitializeBuffers();
    }       

    mesh = mSphere;
    break;

  case MESH_TEAPOT:

    if(!mTeapot)
    {
		mTeapot = new Mesh();
		mTeapot->LoadFromObj("teapot_mid_poly", SPHERICAL);
		mTeapot->InitializeBuffers();
    }

    mesh = mTeapot;
    break;
  }

  return mesh;
}

MeshFactory* MeshFactory::getInstance()
{
  static MeshFactory* instance = 0;

  if(!instance)
    instance = new MeshFactory();

  return instance;
}

MeshFactory::~MeshFactory()
{
  if(mCube)
    delete mCube;

  if(mBunny)
    delete mBunny;

  if(mHorse)
    delete mHorse;

  if(mPlane)
    delete mPlane;

  if(mSphere)
    delete mSphere;

  if(mTeapot)
    delete mTeapot;
}
