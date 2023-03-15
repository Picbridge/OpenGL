/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <MeshFactory.h>
Purpose: <Manages the multiple mesh>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <jihoon.bae_CS300_3>
Author: <jihoon.bae>
Creation date: <28/03/2018>
End Header --------------------------------------------------------*/
#pragma once

#include "Mesh.h"

enum MESHTYPE
{
  MESH_CUBE,
  MESH_HORSE,
  MESH_SPHERE,
  MESH_TEAPOT,
  MESH_BUNNY,
  MESH_PLANE,
};

class MeshFactory
{
public:

  MeshFactory();
  ~MeshFactory();
  Mesh* GetMesh(MESHTYPE mtype);

  static MeshFactory* getInstance();

private:

  Mesh* mCube;
  Mesh* mPlane;
  Mesh* mBunny;
  Mesh* mHorse;
  Mesh* mSphere;
  Mesh* mTeapot;

};