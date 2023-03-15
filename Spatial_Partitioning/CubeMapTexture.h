///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <CubeMapTexture.h>
//Purpose: <Functions used in cubemapping>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <Spatial_Partitioning>
//Author: <jihoon.bae>
//Creation date: <01/10/2018>
//End Header --------------------------------------------------------*/
#pragma once

#include "Main/Support.h"
#include "TGA\tgaimage.h"
#include "Texture.h"

const int CUBE_SIZE = 6;

static const GLenum CUBEMAP_TYPE[CUBE_SIZE] = {  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                                 GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                                 GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                                                 GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                                 GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                                                 GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

class CubeMapTexture
{
public:

  CubeMapTexture();
  ~CubeMapTexture();

  bool LoadTexturesFromFile(const std::string& dir, 
                            const std::string& right, 
                            const std::string& left, 
                            const std::string& top, 
                            const std::string& bot, 
                            const std::string& front, 
                            const std::string& back);
  bool LoadTextures();
  void ReleaseTexture();

  unsigned GetTextureID() const;
  unsigned GetTextureUnit() const;
  unsigned* GetTextureIDArray() const;
  unsigned* GetTextureUnitArray() const;
  unsigned GetFrameBufferID() const;
  unsigned GetWidth() const;
  unsigned GetHeight() const;

private:

  unsigned mTextureID;
  unsigned mTextureUnit;
  unsigned mFrameBufferID;
  unsigned mDepthBufferID;
  unsigned mWidth;
  unsigned mHeight;
  bool mIsloaded;

  unsigned* mTextureIDArray;
  unsigned* mTextureUnitArray;
};