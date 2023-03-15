#pragma once

#include "CubeMapTexture.h"
#include "MeshRenderer.h"
#include "Camera.h"

class Skybox
{
public:

  ~Skybox();
  bool Initialize(const std::string& dir,
                  const std::string& PosXFile,
                  const std::string& NegXFile,
                  const std::string& PosYFile,
                  const std::string& NegYFile,
                  const std::string& PosZFile,
                  const std::string& NegZFile);

  void SetWorldMatrix(const Vector3& camPos);
  void Bind(unsigned ShaderProgram);
  void Render();

private:

  CubeMapTexture* mCubeMapTexture;
  MeshRenderer* mSkyboxMesh;
};