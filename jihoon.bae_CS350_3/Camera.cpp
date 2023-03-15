///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <camera.cpp>
//Purpose: <Use camera in application>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_2>
//Author: <jihoon.bae>
//Creation date: <01/10/2018>
//End Header --------------------------------------------------------*/
#include "Camera.h"
#include "OpenGL/glut.h"
#include "GFX.h"
#include <windows.h>

static const Vector3 DEFAULT_LOOKAT(0,0,-1);
static const Vector3 DEFAULT_UP(0,2,0);
static const Vector3 DEFAULT_RIGHT(0,0,-1);

Camera::Camera() : mPos(0,0,0), mLook(0,0,0), mUp(0,0,0), mSphericalCoords(12,0,0),
                   mOrigin(4,2,2)
{
  mWidth = 0;
  mHeight = 0;
  mAspectRatio = 0;
  mNear = 0;
  mFar = 0;
  mXAxisRot = 0;
  mYAxisRot = 0;
  mZAxisRot = 0;
  mCamOffsetY = 0.3;
  mCamOffsetX = 0;
  mView.SetIdentity();
  mProj.SetIdentity();

  mCubeMap = new CubeMapTexture();
  mCubeMap->LoadTextures();
}

Camera::~Camera()
{
  delete mCubeMap;
}

void Camera::Resize( float fov, float width, float height, float aspectratio, float n, float f )
{
  mWidth = width;
  mHeight = height;
  mAspectRatio = aspectratio;
  mNear = n;
  mFar = f;
  mFOV = fov;

  // Use the Projection Matrix
  glMatrixMode(GL_PROJECTION);

  // Load the identity matrix over the top of the current projection
  glLoadIdentity();

  // Set the correct perspective
  gluPerspective(fov, aspectratio, n, f);

  glGetFloatv(GL_PROJECTION_MATRIX, mProj.array);

  // Get back to the Modelview
  glMatrixMode(GL_MODELVIEW);
}

Matrix4 Camera::GetView()
{
  return mView;
}

Matrix4 Camera::GetProj()
{
  return mProj;
}

float Camera::GetFar()
{
  return mFar;
}

float Camera::GetNear()
{
  return mNear;
}

void Camera::SetPosition( Vector3& pos )
{
  mPos = pos;
}

Vector3 Camera::GetPosition()
{
  return mPos;
}

void Camera::SetOrigin( Vector3& origin )
{
  mOrigin = origin;
}

void Camera::ZoomOut(float radius)
{
	float r = mSphericalCoords.Radius();
	if (r < 30)
	{
		mSphericalCoords.Radius(r + radius);
	}
}

void Camera::Update(float dt)
{
  if( GetAsyncKeyState('D') )
    mCamOffsetX += .01f;

  if( GetAsyncKeyState('A') )
    mCamOffsetX -= .01f;

  if( GetAsyncKeyState('W') && mCamOffsetY < .77f)
    mCamOffsetY += .01f;

  if( GetAsyncKeyState('S') && mCamOffsetY > -.77f)
    mCamOffsetY -= .01f;

  // Zoom in
  if( GetAsyncKeyState('Q') )
  {
    float r = mSphericalCoords.Radius();
    if(r > 2)
      mSphericalCoords.Radius(r - .5f);
  }
   // Zoom out
  if( GetAsyncKeyState('E') )
  {
    float r = mSphericalCoords.Radius();
    if(r < 30)
      mSphericalCoords.Radius(r + .5f);
  }

  //mSphericalCoords.ThetaVert(sinf(mCamOffsetY) * .7f);
  mSphericalCoords.ThetaVert(mCamOffsetY);
  mSphericalCoords.ThetaHoriz(mCamOffsetX);

  Vector3 offset = mSphericalCoords.Cartesian();

  mPos = mOrigin + offset;

  mXAxisRot = mSphericalCoords.ThetaVert();
  mYAxisRot = mSphericalCoords.ThetaHoriz();

  glLoadIdentity();

  // Yaw Pitch Roll, rotate Z first,X second,Y third
  glRotatef(RadToDeg(mYAxisRot),0,1,0);
  glRotatef(RadToDeg(mXAxisRot),1,0,0);
  glRotatef(RadToDeg(mZAxisRot),0,0,1);

  Matrix4 rot;
  glGetFloatv(GL_MODELVIEW_MATRIX, rot.array);
  rot.Transpose();

  mLook = TransformPointProjected(rot, DEFAULT_LOOKAT);
  mUp = TransformPointProjected(rot, DEFAULT_UP);
  mRight = mLook.Cross(mUp);

  glLoadIdentity();
  gluLookAt(mPos.x, mPos.y, mPos.z,
            mOrigin.x, mOrigin.y, mOrigin.z,
            mUp.x, mUp.y, mUp.z);

  glGetFloatv(GL_MODELVIEW_MATRIX, mView.array);
}

float Camera::GetWidth()
{
  return mWidth;
}

float Camera::GetHeight()
{
  return mHeight;
}

void Camera::DrawFaces( unsigned face )
{
	
  Vector3 pos = Vector3(0,0,0);

  if (GFX::getInstance()->GetMeshRenderer() != NULL)
	  pos = GFX::getInstance()->GetMeshRenderer()->transform.GetPosition();
  // clear
  glFramebufferTexture2D(GL_FRAMEBUFFER, 
                         GL_COLOR_ATTACHMENT0, 
                         GL_TEXTURE_2D, 
                         mCubeMap->GetTextureIDArray()[face],
                         0);

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  switch (CUBEMAP_TYPE[face])
  {
  case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    
    gluLookAt(0, 0, 0,  // Pos
              1, 0, 0,  // Look at
              0, 1, 0); // Up
    break;

  case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:

    gluLookAt(0, 0, 0,  // Pos
             -1, 0, 0,  // Look at
              0, 1, 0); // Up
    break;

  case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:

    gluLookAt(0, 0, 0,  // Pos
              0, 1, 0,  // Look at
              0, 0, 1); // Up
    break;

  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:

    gluLookAt(0, 0, 0,  // Pos
              0, -1, 0, // Look at
              0, 0, 1); // Up
    break;

  case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:

    gluLookAt(0, 0, 0,  // Pos
              0, 0, 1,  // Look at
              0, 1, 0); // Up
    break;

  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:

    gluLookAt(0, 0, 0,  // Pos
              0, 0, -1, // Look at
              0, 1, 0); // Up
    break;
  }

  glTranslatef(-pos.x, -pos.y, -pos.z);

  glGetFloatv(GL_MODELVIEW_MATRIX, mView.array);
}

void Camera::DisableCubeMapRendering()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glViewport(0, 0, mWidth, mHeight);

  GFX::getInstance()->SetBindCubeMap(1);

  // Reset Projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(mFOV, mAspectRatio, mNear, mFar);
  glGetFloatv(GL_PROJECTION_MATRIX, mProj.array);

  // Reset View matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(mPos.x, mPos.y, mPos.z,
            mOrigin.x, mOrigin.y, mOrigin.z,
            mUp.x, mUp.y, mUp.z);

  glGetFloatv(GL_MODELVIEW_MATRIX, mView.array);
}

void Camera::EnableCubeMapRendering()
{
  glBindFramebuffer(GL_FRAMEBUFFER, mCubeMap->GetFrameBufferID());
  
  glViewport(0, 0, mCubeMap->GetWidth(), mCubeMap->GetHeight());

  GFX::getInstance()->SetBindCubeMap(0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.f, 1.f, mNear, mFar);
  glGetFloatv(GL_PROJECTION_MATRIX, mProj.array);
  
}

CubeMapTexture* Camera::GetCubeMap()
{
  return mCubeMap;
}

void Camera::SetRadius( const float& r )
{
  mSphericalCoords.Radius(r);
}

float Camera::GetRadius() const
{
  return mSphericalCoords.Radius();
}
