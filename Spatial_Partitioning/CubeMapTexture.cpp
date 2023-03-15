///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <CubeMapTexture.cpp>
//Purpose: <Functions used in cubemapping>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <Spatial_Partitioning>
//Author: <jihoon.bae>
//Creation date: <01/10/2018>
//End Header --------------------------------------------------------*/
#include "CubeMapTexture.h"
#include "GFX.h"

CubeMapTexture::CubeMapTexture()
{
  mTextureID = 0;
  mTextureUnit = 0;
  mFrameBufferID = 0;
  mDepthBufferID = 0;
  mWidth= 0;
  mHeight = 0;
  mIsloaded = false;

  mTextureIDArray = 0;
  mTextureUnitArray = 0;
}

CubeMapTexture::~CubeMapTexture()
{
  ReleaseTexture();
}

bool CubeMapTexture::LoadTexturesFromFile( const std::string& dir, 
                                           const std::string& right, 
                                           const std::string& left, 
                                           const std::string& top, 
                                           const std::string& bot, 
                                           const std::string& front, 
                                           const std::string& back)
{
  mTextureUnit = GFX::getInstance()->GetTextureHandlePool().Pop();
  
  std::string files[sizeof(CUBEMAP_TYPE) / sizeof(GLenum)];
  files[0] = dir + right;
  files[1] = dir + left;
  files[2] = dir + top;
  files[3] = dir + bot;
  files[4] = dir + front;
  files[5] = dir + back;

  glActiveTexture(GL_TEXTURE0 + mTextureUnit);

  glGenTextures(1, &mTextureID);

  glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);

  unsigned size = sizeof(CUBEMAP_TYPE) / sizeof(GLenum);
  for(unsigned i = 0; i < size; ++i)
  {
    Image* mImage = Image::Load(files[i].c_str());

    if(!mImage)
    {
      std::cout << "Error loading texture - " << files[i] << std::endl;
      return false;
    }

    glTexImage2D(CUBEMAP_TYPE[i], 0, 
                 (mImage->BPP() == 32) ? GL_RGBA : GL_RGB, 
                 mImage->SizeX(), mImage->SizeY(), 0, 
                 (mImage->BPP() == 32) ? GL_BGRA : GL_BGR,
                 GL_UNSIGNED_BYTE, mImage->Data());

    mWidth = mImage->SizeX();
    mHeight = mImage->SizeY();

    Image::Free(mImage);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  // Set that the texture is loaded.
  mIsloaded = true;

  return true;
}

bool CubeMapTexture::LoadTextures()
{
  mWidth = mHeight = 512;

  mTextureIDArray = new unsigned [CUBE_SIZE];
  mTextureUnitArray = new unsigned [CUBE_SIZE];


  for (int i = 0; i < CUBE_SIZE; ++i)
  {
    mTextureUnitArray[i] = GFX::getInstance()->GetTextureHandlePool().Pop();

    glActiveTexture(GL_TEXTURE0 + mTextureUnitArray[i]);

    glGenTextures(1, &mTextureIDArray[i]);
    glBindTexture(GL_TEXTURE_2D, mTextureIDArray[i]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // set textures
    glTexImage2D(GL_TEXTURE_2D, 0, 
                 GL_RGB,
                 mWidth, mHeight, 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE, 0);
  }

  // create the uniform depth buffer
  glGenRenderbuffers(1, &mDepthBufferID);
  glBindRenderbuffer(GL_RENDERBUFFER, mDepthBufferID);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  // create the fbo
  glGenFramebuffers(1, &mFrameBufferID);
  glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID);

  // attach it
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, 
                            GL_DEPTH_ATTACHMENT, 
                            GL_RENDERBUFFER, 
                            mDepthBufferID); 

  glFramebufferTexture2D(GL_FRAMEBUFFER, 
                         GL_COLOR_ATTACHMENT0, 
                         GL_TEXTURE_2D,
                         mTextureIDArray[0], 
                         0);  

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(status != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Error occurred creating FBO\n";
    return false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Set that the texture is loaded.
  mIsloaded = true;

  return true;
}

void CubeMapTexture::ReleaseTexture()
{
  if(mIsloaded)
  {
    if(mTextureIDArray)
    {
      for (int i = 0; i < CUBE_SIZE; ++i)
      {
        glDeleteTextures(1, &mTextureIDArray[i]);
        GFX::getInstance()->GetTextureHandlePool().Return(mTextureUnitArray[i]);
      }

      delete [] mTextureIDArray;
      delete [] mTextureUnitArray;
    }
    else
    {
      glDeleteTextures(1, &mTextureID);
      GFX::getInstance()->GetTextureHandlePool().Return(mTextureUnit);
    }

    glDeleteFramebuffers(1, &mFrameBufferID);
    glDeleteRenderbuffers(1, &mDepthBufferID);
    
    mIsloaded = false;
  }
}

unsigned CubeMapTexture::GetTextureUnit() const
{
  return mTextureUnit;
}

unsigned CubeMapTexture::GetWidth() const
{
  return mWidth;
}

unsigned CubeMapTexture::GetHeight() const
{
  return mHeight;
}

unsigned CubeMapTexture::GetFrameBufferID() const
{
  return mFrameBufferID;
}

unsigned CubeMapTexture::GetTextureID() const
{
  return mTextureID;
}

unsigned* CubeMapTexture::GetTextureIDArray() const
{
  return mTextureIDArray;
}

unsigned* CubeMapTexture::GetTextureUnitArray() const
{
  return mTextureUnitArray;
}
