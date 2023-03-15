///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Texture.cpp>
//Purpose: <Load texture from file and bind it>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <26/03/2019>
//End Header --------------------------------------------------------*/
#include "Texture.h"
#include "GFX.h"

Texture::Texture()
{
  mTextureID = 0;
  mIsloaded = false;
}

bool Texture::LoadPPM(char* filename, bool wrap)
{
	FILE* fid = fopen(filename, "r");
	const int readBufferSize = 50;
	unsigned int bitDepth;
	unsigned int bytesPerPix;
	Vector3* image;
	Vector2* mTexcoord;
	int mSize;

	char buffer[readBufferSize];
	if (!fid)
	{
		return false;
	}

	fgets(buffer, readBufferSize, fid);
	if (strncmp(buffer, "P3", 3) == 0)
	{
		return false;
	}
	fgets(buffer, readBufferSize, fid);
	while (1)
	{
		if (strncmp(buffer, "#", 1) == 0)
		{
			fgets(buffer, readBufferSize, fid);
		}
		else
			break;
	}
	int save;
	mWidth = atoi(buffer);
	for (int i = 0; i < readBufferSize; ++i)
	{
		if (strncmp(buffer + i, " ", 1) == 0)
		{
			save = i + 1;
			break;
		}
	}
	mHeight = atoi(buffer + save);
	fgets(buffer, readBufferSize, fid);
	if (atoi(buffer) != 255)
	{
		return false;
	}
	bytesPerPix = 3;
	bitDepth = atoi(buffer);

	mSize = mWidth * mHeight;
	image = new Vector3[mSize];
	mTexcoord = new Vector2[mSize];
	unsigned int bytesPerLine = mWidth * bytesPerPix;

	int i = 0;
	while (i < mSize)
	{
		Vector3 vertex;
		fscanf(fid, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
		image[i] = vertex / (float)bitDepth;
		i++;
	}
	fclose(fid);
	glActiveTexture(GL_TEXTURE0 + mTextureUnit);

	// Generate an ID for the texture.
	glGenTextures(1, &mTextureID);

	// Bind the texture as a 2D texture.
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	// Set the texture color to either wrap around or clamp to the edge.
	if (wrap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load the image data into the texture unit.
	glTexImage2D(GL_TEXTURE_2D, 0,
		GL_RGB,
		mWidth, mHeight, 0,
		GL_RGB,
		GL_FLOAT, image);
	// Generate mipmaps for the texture.
	glGenerateMipmap(GL_TEXTURE_2D);


	// Set that the texture is loaded.
	mIsloaded = true;

	return true;
}

bool Texture::LoadTGA(char* filename, bool wrap)
{
  mTextureUnit = GFX::getInstance()->GetTextureHandlePool().Pop();

  Image* mImage = Image::Load(filename);

  if(!mImage)
    return false;

  glActiveTexture(GL_TEXTURE0 + mTextureUnit);

  // Generate an ID for the texture.
  glGenTextures(1, &mTextureID);

  // Bind the texture as a 2D texture.
  glBindTexture(GL_TEXTURE_2D, mTextureID);

  // Load the image data into the texture unit.
  glTexImage2D(GL_TEXTURE_2D, 0, 
               (mImage->BPP() == 32) ? GL_RGBA : GL_RGB, 
               mImage->SizeX(), mImage->SizeY(), 0, 
               (mImage->BPP() == 32) ? GL_BGRA : GL_BGR,
               GL_UNSIGNED_BYTE, mImage->Data());

  // Set the texture color to either wrap around or clamp to the edge.
  if(wrap)
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  else
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  }

  // Set the texture filtering.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Generate mipmaps for the texture.
  glGenerateMipmap(GL_TEXTURE_2D);

  mWidth = mImage->SizeX();
  mHeight = mImage->SizeY();

  // Release TGA data
  Image::Free(mImage);
  mImage = 0;

  // Set that the texture is loaded.
  mIsloaded = true;

  return true;
}

void Texture::ReleaseTexture()
{
  if(mIsloaded)
  {
    glDeleteTextures(1, &mTextureID);
    GFX::getInstance()->GetTextureHandlePool().Return(mTextureUnit);
    mIsloaded = false;
  }
}

Texture::~Texture()
{
  ReleaseTexture();
}

unsigned Texture::GetWidth()
{
  return mWidth;
}

unsigned Texture::GetHeight()
{
  return mHeight;
}
