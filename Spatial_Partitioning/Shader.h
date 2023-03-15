///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Shader.h>
//Purpose: <load shaderfiles from directory>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_2>
//Author: <jihoon.bae>
//Creation date: <26/09/2018>
//End Header --------------------------------------------------------*/

#pragma once

#include "Main/Support.h"
#include "Camera.h"
#include "Texture.h"

class Shader
{
public:

  Shader();
  ~Shader();

  bool InitializeShaders(char* vsName, char* fsName);
  char* LoadShaderFromFile(char* filename);
  void OutputShaderError(unsigned shaderID, char* filename);
  void OutputLinkerError();
  void SetTexture(unsigned diffTexUnit, unsigned specTexUnit, Texture* texture = NULL);
  bool ReloadShader();
  void Use();
  void Unuse();

  GLuint GetShaderProgram() {return mShaderProgram; }

private:

  GLuint mVertexShader;
  GLuint mFragmentShader;
  GLuint mShaderProgram;

  char* mVSFileName;
  char* mFSFileName;

  bool vsErr;
  bool fsErr;
  bool LinkErr;
};