///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <MeshRenderer.h>
//Purpose: <Setting rendered buffers>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
//Project: <jjihoon.bae_CS300_3>
//Author: <jihoon.bae>
//Creation date: <26/09/2018>
//End Header --------------------------------------------------------*/
#pragma once

#include "Mesh.h"
#include "Transform.h"
#include "CubeMapTexture.h"

class MeshRenderer
{
public:

  MeshRenderer();
  ~MeshRenderer();

  Mesh* GetMesh() { return mMesh; }
  BVInfo* GetBVInfo() { return mMesh->GetBVInfo(); }
  MeshRenderer* GetBV() { return mBV; }
  void SetMesh(Mesh* mesh) { mMesh = mesh; }
  void SetColor(Vector4 color) { mBVColor = color; }
  void BindMesh(const unsigned& shaderProgram);
  
  void BindBV(const unsigned& shaderProgram);
  void DrawBV();

  void InitBV();
  void Update(float dt);
  void UpdateBV();

  CTransform transform;

private:

  Mesh* mMesh;
  MeshRenderer* mBV;
  Vector4 mBVColor;
};