///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Texture.h>
//Purpose: <Load texture from file and bind it>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <26/03/2019>
//End Header --------------------------------------------------------*/
#pragma once

#include "TGA\tgaimage.h"

const unsigned MAXTEXTURE = 32;

class Texture
{
public:

  Texture();
  ~Texture();
  bool LoadTGA(char* filename, bool wrap);
  bool LoadPPM(char* filename, bool wrap);
  void ReleaseTexture();
  unsigned GetTextureUnit() const { return mTextureUnit; }
  unsigned GetWidth();
  unsigned GetHeight();
  unsigned mTextureUnit;
private:

  unsigned mTextureID;
  bool mIsloaded;
  unsigned mWidth;
  unsigned mHeight;
};