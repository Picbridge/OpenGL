///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <GFX.h>
//Purpose: <Overall management for data>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <28/03/2019>
//End Header --------------------------------------------------------*/
#pragma once
#define MAXOBJECTS 8
// Includes
#include "Main/Support.h"
#include "OpenGL/glut.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "BVH.h"
#include "Octree.h"
#include "BSPTree.h"
// The main flow behind your application
class GFX
{
public:

  // Constructor
  GFX();

  // Destructor
  ~GFX();

  // This function gets called every frame 
  void Update(float frameTime);

  // Draw the application in its current state
  void Draw();

  // Reshape our projection matrix since the window size changed
  void Reshape(int width, int height, float aspectRatio);

  // Occurs when the user presses the 'reload shaders' button
  static void TW_CALL OnReloadShaders(void* selfPointer);

  static void TW_CALL ToggleTextureMapping(void* selfPointer);
  static void TW_CALL ToggleVertexNormals(void* selfPointer);
  static void TW_CALL ToggleTriangleStrip(void* selfPointer);
  static void TW_CALL ToggleWireFrame(void* selfPointer);
  static void TW_CALL ToggleLightsRotation(void* selfPointer);
  static void TW_CALL AddLight(void* selfPointer);
  static void TW_CALL AddObject(void* selfPointer);
  static void TW_CALL RemoveLight(void* selfPointer);
  static void TW_CALL RenderNormalMap(void* selfPointer);
  static void TW_CALL RenderTangent(void* selfPointer);
  static void TW_CALL RenderBinormal(void* selfPointer);
  static void TW_CALL ToggleNormalMap(void* selfPointer);

  void Initialize();
  void InitializeAntTweakBar();
  void UpdateLights(float dt);
  void BindUniforms(unsigned shaderProgram);
  void BindLights();
  void BindLightCount();
  void BindViewProj(unsigned shaderProgram);
  void BindRenderOption(unsigned shaderProgram);
  void BindCubeMap(unsigned shaderProgram, unsigned* cubemap);
  void DestroyLights();
  void RenderScene();

  Pool<unsigned>& GetTextureHandlePool();
  Camera*         GetCamera() { return mCamera; }
  MeshRenderer*   GetMeshRenderer() { 
	  if (mMeshRenderers.size() > 0)
		  return mMeshRenderers[0];
	  else
		  return NULL;
  }
  TwBar*          GetAntTweakBar() { return mBar; }
  unsigned        GetActiveLights() const;
  BVType          GetBVType() { return mBVType; }
  BVHOPTION       GetBVHOption() { return mBVHOption; }
  BVHTERMINATION  GetBVHTermination() { return mBVHTermination; }
  BVHDRAW         GetBVHDraw() { return mBVHDraw; }
  int             GetTreeHeight() { return mHeight; }
  int             GetOctreeHeight() { return mOctreeHeight; }
  bool IsEnabledRotateLights() { return mEnableRotateLights; }  
  void SetBindCubeMap(const int& option);
  

  static GFX* getInstance();

  enum RENDEROPTION
  {
    DEFAULT,
    NORMALS,      // Vert normals
    BUMPNORMALS,  // Normal map normals
    TANGENTS,
    BINORMALS,
    OFF_NORMALMAP,
    TRIANGLESTRIPS,

    TOTALOPTIONS
  };

private:
  
  // The ui that represents our application
  TwBar* mBar;
  TwType twStructType;
  TwType twBVType;
  TwType twBVH;
  TwType twTree;
  TwType twTerm;
  TwType twBVHDraw;
  TwType twOctreeHeight;
  BVType mBVType;

  std::vector<MeshRenderer*> mMeshRenderers;
  MeshRenderer* mr;//////

  Camera* mCamera;
  Shader* mPhongShader;
  Shader* mShader;
  Shader* mSkyboxShader;

  Pool<unsigned> mTextureHandlePool;
  Texture* mDefaultTexture;
  Texture* mDiffuseMap;
  Texture* mNormalMap;

  Vector4 mGlobalAmbient;
  Vector4 mFogColor;

  LightCount mLightCount;
  std::vector<Light*> mLights;
  float mFrameTime;

  GLuint mlightCountUBO;
  GLuint mlightUBO;
  RENDEROPTION mRenderOption;
  float mNormalMapAlpha;
  float mReflectivity;
  float mRefractiveIndex;
  float mCamRadius;
  float mTimer;
  float mCamZoomOut;

  bool mEnableWireframe;
  bool mEnableTexture;
  bool mEnableRotateLights;
  int mBindCubeMap;

  int mHeight;
  int mOctreeHeight;
  int mNumOfObj;
  int mSaveNumObj;
  BVH* mBVHierarchy;
  Octree* mOctree;
  BSPTree* mBSPTree;
  BVHOPTION mBVHOption;
  BVHOPTION mTreeDef;
  BVHDRAW mBVHDraw;
  BVHTERMINATION mBVHTermination;
};
