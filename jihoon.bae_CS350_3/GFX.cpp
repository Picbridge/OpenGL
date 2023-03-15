///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <GFX.cpp>
//Purpose: <Overall management for data>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <28/03/2019>
//End Header --------------------------------------------------------*/

// Includes
#include "GFX.h"
#include "MeshFactory.h"
//-----------------------------------------------------------------------------
GFX::GFX()
{
  mGlobalAmbient = Vector4(.2f, .2f, .2f, 1);
  mFogColor      = Vector4(0,0,0,0);
  mEnableRotateLights = true;
  mEnableTexture = true;
  mEnableWireframe = false;
  mRenderOption = DEFAULT;
  mNormalMapAlpha = 1;
  mBindCubeMap = 0;
  mReflectivity = 1;
  mRefractiveIndex = 1.33f;
  mTimer = 0;
  mBVType = BV_NA;
  mBVHOption = BVH_NA;
  mBVHDraw = BVH_AABB;
  mBVHTermination = BVH_TOPDOWN_VERTICES;
  mHeight = -1;
  mNumOfObj = 1;
  mSaveNumObj = 1;
  mCamZoomOut = 0;
  for(unsigned i = 0; i < MAXTEXTURE; ++i)
    mTextureHandlePool.Push(i);

  InitializeAntTweakBar();
}

void GFX::Initialize()
{
  // Initialize camera
  mCamera = new Camera();
  mCamRadius = mCamera->GetRadius();

  // Initialize shaders
  bool result = false;
  mPhongShader = new Shader();
  while(!result)
    result = mPhongShader->InitializeShaders("Shaders/light.vs", 
                                             "Shaders/light.fs");

  result = false;;
  mShader = new Shader();
  while(!result)
    result = mShader->InitializeShaders("Shaders/color.vs", "Shaders/color.fs");

  result = false;;
  mSkyboxShader = new Shader();
  while(!result)
    result = mSkyboxShader->InitializeShaders("Shaders/skybox.vs", 
                                              "Shaders/skybox.fs");

  
  //MeshRenderer* mr = new MeshRenderer();
  //mr->InitBV();
  //mr->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_SPHERE));
  //mr->transform.SetPosition(Vector3(6,2,2));
  //mMeshRenderers.push_back(mr);
  //
  //mr  = new MeshRenderer();
  //mr->InitBV();
  //mr->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_HORSE));
  //mr->transform.SetPosition(Vector3(2,4,4));
  //mMeshRenderers.push_back(mr);
  //
  //mr  = new MeshRenderer();
  //mr->InitBV();
  //mr->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_TEAPOT));
  //mr->transform.SetPosition(Vector3(2,2,6));
  //mMeshRenderers.push_back(mr);

  //mr  = new MeshRenderer();
  //mr->InitBV();
  //mr->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_BUNNY));
  //mr->transform.SetPosition(Vector3(6,4,5));
  //mMeshRenderers.push_back(mr);

  // Initialize textures
  mDiffuseMap = new Texture();
  mDiffuseMap->mTextureUnit = 0;
  mDiffuseMap->LoadPPM("texture/metal_roof_diff_512x512.ppm", true);
  mNormalMap = new Texture();
  mNormalMap->mTextureUnit = 1;
  mNormalMap->LoadPPM("texture/metal_roof_spec_512x512.ppm", true);
  mDefaultTexture = new Texture();
  mDefaultTexture->mTextureUnit = 0;
  mDefaultTexture->LoadPPM("texture/metal_roof_diff_512x512.ppm", true);

  
 /* mBVHierarchy = new BVH();
  mBVHierarchy->ConstructBVH(mMeshRenderers);*/

  //mOctree = new Octree();
  //mOctree->BuildOctree(mMeshRenderers);
  //mBSPTree = new BSPTree();
  //mBSPTree->ConstructTree(mMeshRenderers);
  AddLight(this);
  AddObject(this);
  ToggleLightsRotation(this);
}

void GFX::InitializeAntTweakBar()
{
  // AntTweak Bar
  mBar = TwNewBar("GFX");
  TwDefine(" GFX size='200 350' ");

  TwAddSeparator(mBar, NULL, NULL);
  TwAddSeparator(mBar, NULL, NULL);

  TwAddButton(mBar, "Add Light", AddLight, this, "");
  TwAddButton(mBar, "Remove Light", RemoveLight, this, "");

  TwAddSeparator(mBar, NULL, NULL);
  TwAddSeparator(mBar, NULL, NULL);

  twBVType = TwDefineEnum("BVType", NULL, 0);
  TwAddVarRW(mBar, "Bounding Volume", twBVType, &mBVType, 
             " enum='0 {None}, 1 {AABB}, 2 {OBB}, 3 {Centroid Sphere}, 4 {Ritters Sphere}, 5 {Larssons Sphere}, 6 {Eigen Sphere}, 7 {Eigen Ellipsoid}' ");
  TwAddVarRW(mBar, "Height", TW_TYPE_INT32, &mHeight, "");

  twBVH = TwDefineEnum("BVH", NULL, 0);
  TwAddVarRW(mBar, "BV Hierarchy,Octree,BSP", twBVH, &mBVHOption,
	  " enum='0 {None}, 1 {Top Down}, 2 {Bottom Up}' ");

  twBVHDraw = TwDefineEnum("BVHDraw", NULL, 0);
  TwAddVarRW(mBar, "BVH Draw", twBVHDraw, &mBVHDraw,
	  " enum='0 {AABB}, 1 {Sphere}' ");

  twTerm = TwDefineEnum("Term", NULL, 0);
  TwAddVarRW(mBar, "Terminate condition", twTerm, &mBVHTermination,
	  " enum='0 {Vertices <= 500}, 1 {Max Height = 7}' ");

  TwAddSeparator(mBar, NULL, NULL);
  TwAddSeparator(mBar, NULL, NULL);

  //TwAddVarRW(mBar, "Object population", TW_TYPE_INT32, &mNumOfObj, "");
  TwAddButton(mBar, "Add Object", AddObject, this, "");

  twTree = TwDefineEnum("Tree", NULL, 0);
  TwAddVarRW(mBar, "Octree,BSP", twTree, &mTreeDef,
	 " enum='0 {None}, 3 {Octree}, 4 {BSPTree}' ");
  TwAddVarRW(mBar, "OctreeHeight", TW_TYPE_INT32, &mOctreeHeight, "");

  TwAddSeparator(mBar, NULL, NULL);
  TwAddSeparator(mBar, NULL, NULL);

  //TwAddButton(mBar, "Toggle Lights Rotation", ToggleLightsRotation, this, "");

  //TwStructMember twStruct[] =
  //{
  //  {"Ambient", TW_TYPE_COLOR4F, offsetof(Light, mAmbient), ""},
  //  {"Diffuse", TW_TYPE_COLOR4F, offsetof(Light, mDiffuse), ""},
  //  {"Specular", TW_TYPE_COLOR4F, offsetof(Light, mSpecular), ""},
  //  {"PositionX", TW_TYPE_FLOAT, offsetof(Light, mPosition.x), ""},
  //  {"PositionY", TW_TYPE_FLOAT, offsetof(Light, mPosition.y), ""},
  //  {"PositionZ", TW_TYPE_FLOAT, offsetof(Light, mPosition.z), ""},
  //  {"Inner", TW_TYPE_FLOAT, offsetof(Light, mInnerAngle), ""},
  //  {"Outer", TW_TYPE_FLOAT, offsetof(Light, mOuterAngle), ""},
  //  {"Type", TW_TYPE_INT32, offsetof(Light, mType), "min=0 max=2"}
  //};

  //unsigned size = sizeof(twStruct) / sizeof(TwStructMember);

  //twStructType = TwDefineStruct("twStructType", twStruct, size, sizeof(Light), NULL, NULL);
  printf("%s\n", TwGetLastError());
}

//-----------------------------------------------------------------------------
GFX::~GFX()
{
  delete mCamera;

  delete mPhongShader;
  delete mShader;
  delete mSkyboxShader;

  std::vector<MeshRenderer*>::iterator iterMR;
  for(iterMR = mMeshRenderers.begin(); iterMR != mMeshRenderers.end(); ++iterMR)
    delete *iterMR;
  
  delete mDiffuseMap;
  delete mNormalMap;
  delete mDefaultTexture;

  delete mBVHierarchy;
  delete MeshFactory::getInstance();

  DestroyLights();
}

//-----------------------------------------------------------------------------
void GFX::Update(float frameTime)
{
  mFrameTime = frameTime;


  if (mNumOfObj != mSaveNumObj)
  {
	  mCamZoomOut += 0.01f;
	  mCamera->ZoomOut(mCamZoomOut);

	  if (mCamZoomOut >= mNumOfObj / (mNumOfObj*4.5f))
	  {
		  mSaveNumObj = mNumOfObj;
		  mCamZoomOut = 0;
	  }
  }

  std::vector<MeshRenderer*>::iterator iterMR;
  for(iterMR = mMeshRenderers.begin(); iterMR != mMeshRenderers.end(); ++iterMR)
  {
    (*iterMR)->Update(frameTime);
  }

  mCamera->Update(frameTime);

  UpdateLights(frameTime);
}

void GFX::UpdateLights(float dt)
{
  std::vector<Light*>::iterator iter;
  for(iter = mLights.begin(); iter != mLights.end(); ++iter)
    (*iter)->Update(dt);

  mLightCount.mDir   = 0;
  mLightCount.mPoint = 0;
  mLightCount.mSpot  = 0;
  mLightCount.mTotal = mLights.size();

  // Sort lights
  std::sort(mLights.begin(), mLights.end(), Light::sortLights());

  for(unsigned int i = 0; i < mLightCount.mTotal; ++i)
  {
    switch(mLights[i]->mType)
    {
    case 0:
      ++mLightCount.mDir;
      break;

    case 1:
      ++mLightCount.mDir;
      break;

    case 2:
      ++mLightCount.mDir;
      break;
    }
  }
}

//-----------------------------------------------------------------------------
void GFX::Draw()
{
	glClearColor(0.4f, 0.4f, 0.5f, 1.f);
  //glClearColor(.5,.5,.5,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
  SetBindCubeMap(0);
  RenderScene();

  // Default shader to draw objects not affected by light
  mShader->Use();

  BindViewProj(mShader->GetShaderProgram());
  glPolygonMode(GL_FRONT, GL_LINE);
  std::vector<Light*>::iterator iter;
  for(iter = mLights.begin(); iter != mLights.end(); ++iter)
  {
    (*iter)->BindLightMesh(mShader->GetShaderProgram());
    (*iter)->GetMeshRenderer()->GetMesh()->DrawMesh();
  }
  
  // Draw Bounding Volumes  
  glPolygonMode( GL_FRONT, GL_LINE );

  std::vector<MeshRenderer*>::iterator iterMR;
  for(iterMR = mMeshRenderers.begin(); iterMR != mMeshRenderers.end(); ++iterMR)
  {    
    (*iterMR)->BindBV(mShader->GetShaderProgram());
    (*iterMR)->DrawBV();
  }

  if(mBVHDraw == BVH_SPHERE)
    mBVHierarchy->DrawBVH(mShader->GetShaderProgram());

  if(!mEnableWireframe)
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  mShader->Unuse();
  if (mTreeDef == BVH_Octree)
      mOctree->DrawOctree();
  if (mTreeDef == BVH_BSPTree)
      mBSPTree->DrawTree();
  if (mBVHDraw == BVH_AABB)
      mBVHierarchy->DrawBVH();
  

  //for(iter = mLights.begin(); iter != mLights.end(); ++iter)
  //  (*iter)->DrawDebugLines();


  // Destroy light buffers
  glDeleteBuffers(1, &mlightCountUBO);
  if(mLights.size())
    glDeleteBuffers(1, &mlightUBO);

  PrintOpenGLErrors();
}

void GFX::RenderScene()
{

  unsigned diffTexUnit, normTexUnit;
  if(mEnableTexture)
  {
    diffTexUnit = mDiffuseMap->GetTextureUnit();
    normTexUnit = mNormalMap->GetTextureUnit();
  }
  else
    diffTexUnit = normTexUnit = mDefaultTexture->GetTextureUnit();

  // Enables the PHONG shader
  mPhongShader->Use();

  BindRenderOption(mPhongShader->GetShaderProgram());

  BindLights();
  // Passes uniforms into shader
  BindUniforms(mPhongShader->GetShaderProgram());
  BindCubeMap(mPhongShader->GetShaderProgram(), 
              mCamera->GetCubeMap()->GetTextureUnitArray());
  mPhongShader->SetTexture(diffTexUnit, normTexUnit, mNormalMap);

  if (mMeshRenderers.size() > 0)
  {
	  // Draws centre object with reflection mapping
	  MeshRenderer* mr = *mMeshRenderers.begin();

	  mr->BindMesh(mPhongShader->GetShaderProgram());
	  mr->GetMesh()->DrawMesh();
	  mr->GetMesh()->DrawBoundary();
  }
  // Draws the rest of the meshes 
  if(mMeshRenderers.size() > 1)
  {
    mBindCubeMap = 0;
    BindCubeMap(mPhongShader->GetShaderProgram(), 
      mCamera->GetCubeMap()->GetTextureUnitArray());

    std::vector<MeshRenderer*>::iterator iterMR;
    for(iterMR = mMeshRenderers.begin() + 1; iterMR != mMeshRenderers.end(); ++iterMR)
    {
      mr = *iterMR;
      
      mr->BindMesh(mPhongShader->GetShaderProgram());
      mr->GetMesh()->DrawMesh();
    }
  }

  // Disables shader
  mPhongShader->Unuse();
}

//-----------------------------------------------------------------------------
void GFX::Reshape(int width, int height, float aspectRatio)
{
  mCamera->Resize(45.f, width, height, aspectRatio, .1f, 1000.f);
}

//-----------------------------------------------------------------------------
void GFX::OnReloadShaders(void* selfPointer)
{
  // Get our application (it's basically our this pointer!)
  GFX* self = (GFX*)selfPointer;

  bool result = false;
  while(!result)
    result = self->mPhongShader->ReloadShader();
  
  result = false;
  while(!result)
    result = self->mShader->ReloadShader();

  result = false;
  while(!result)
    result = self->mSkyboxShader->ReloadShader();
}

void GFX::ToggleVertexNormals(void* selfPointer)
{
  GFX* self = (GFX*)selfPointer;

  if(self->mRenderOption == NORMALS)
    self->mRenderOption = DEFAULT;
  else
    self->mRenderOption = NORMALS;
}

void GFX::ToggleTriangleStrip(void* selfPointer)
{
  GFX* self = (GFX*)selfPointer;

  if(self->mRenderOption == TRIANGLESTRIPS)
    self->mRenderOption = DEFAULT;
  else
    self->mRenderOption = TRIANGLESTRIPS;
}

void GFX::RenderNormalMap(void* selfPointer)
{
  GFX* self = (GFX*)selfPointer;

  if(self->mRenderOption == BUMPNORMALS)
    self->mRenderOption = DEFAULT;
  else
    self->mRenderOption = BUMPNORMALS;
}

void GFX::RenderTangent(void* selfPointer)
{
  GFX* self = (GFX*)selfPointer;

  if(self->mRenderOption == TANGENTS)
    self->mRenderOption = DEFAULT;
  else
    self->mRenderOption = TANGENTS;
}

void GFX::RenderBinormal(void* selfPointer)
{
  GFX* self = (GFX*)selfPointer;

  if(self->mRenderOption == BINORMALS)
    self->mRenderOption = DEFAULT;
  else
    self->mRenderOption = BINORMALS;
}

void GFX::ToggleNormalMap(void* selfPointer)
{
  GFX* self = (GFX*)selfPointer;

  if(self->mRenderOption == OFF_NORMALMAP)
    self->mRenderOption = DEFAULT;
  else
    self->mRenderOption = OFF_NORMALMAP;
}

void TW_CALL GFX::ToggleWireFrame( void* selfPointer )
{
  GFX* self = (GFX*)selfPointer;
  self->mEnableWireframe = !self->mEnableWireframe;

  if(self->mEnableWireframe)
    glPolygonMode( GL_FRONT, GL_LINE );
  else
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void TW_CALL GFX::ToggleTextureMapping( void* selfPointer )
{
  GFX* self = (GFX*)selfPointer;
  self->mEnableTexture = !self->mEnableTexture;
}

void TW_CALL GFX::ToggleLightsRotation( void* selfPointer )
{
  GFX* self = (GFX*)selfPointer;
  self->mEnableRotateLights = !self->mEnableRotateLights;
}

void GFX::BindLights()
{
  BindLightCount();

  if(mLights.size())
  {
    // Bind Lights
    glGenBuffers (1, &mlightUBO);
    glBindBuffer (GL_UNIFORM_BUFFER, mlightUBO);

    const unsigned lightStructSize = 9;
    GLuint shaderProgram = mPhongShader->GetShaderProgram();

    GLuint uniformBlockIndex = glGetUniformBlockIndex (shaderProgram, "Lights");

    GLint uniformBlockSize = 0;
    glGetActiveUniformBlockiv (shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

    std::vector<std::string> varNames(lightStructSize * mLightCount.mTotal);
    std::vector<const char*> lightVariables(lightStructSize * mLightCount.mTotal);

    unsigned indexCount = 0;

    for(unsigned i = 0; i < mLightCount.mTotal; ++i)
    {
      char c[4];
      std::string start, end, result;
      start = "Lights.light[";
      _itoa(i, c, 10);
      start += c;

      varNames[indexCount] = start + "].ambient";
      lightVariables[indexCount] = varNames[indexCount].c_str();
      ++indexCount;

      varNames[indexCount] = start + "].diffuse";
      lightVariables[indexCount] = varNames[indexCount].c_str();
      ++indexCount;

      varNames[indexCount] = start + "].specular";
      lightVariables[indexCount] = varNames[indexCount].c_str();
      ++indexCount;

      varNames[indexCount] = start + "].position";
      lightVariables[indexCount] = varNames[indexCount].c_str();
      ++indexCount;

      varNames[indexCount] = start + "].direction";
      lightVariables[indexCount] = varNames[indexCount].c_str();
      ++indexCount;

      varNames[indexCount] = start + "].attenuation";
      lightVariables[indexCount] = varNames[indexCount].c_str();
      ++indexCount;

      varNames[indexCount] = start + "].innerangle";
      lightVariables[indexCount] = varNames[indexCount].c_str();
      ++indexCount;

      varNames[indexCount] = start + "].outerangle";
      lightVariables[indexCount] = varNames[indexCount].c_str();
      ++indexCount;

      varNames[indexCount] = start + "].spotfalloff";
      lightVariables[indexCount] = varNames[indexCount].c_str();
      ++indexCount;
    }

    std::vector<GLuint> indices(indexCount);
    glGetUniformIndices (shaderProgram, indexCount, &lightVariables[0], &indices[0]);

    std::vector<GLint>lightUniformOffsets(indexCount);
    glGetActiveUniformsiv (shaderProgram, lightUniformOffsets.size(), &indices[0], GL_UNIFORM_OFFSET, &lightUniformOffsets[0]);
    GLint *offsets = &lightUniformOffsets[0];

    std::vector<unsigned char>buffer(uniformBlockSize);

    for (unsigned n = 0; n < mLightCount.mTotal; ++n)
    {
      int index = 0;

      // Light ambient (vec4)
      int offset = offsets[index++ + n * lightStructSize];
      for (int i = 0; i < 4; ++i)
      {
        *(reinterpret_cast<GLfloat*> (&buffer[0] + offset)) = mLights[n]->mAmbient[i];
        offset += sizeof (GLfloat);
      }

      // Light diffuse color (vec4)
      offset = offsets[index++ + n * lightStructSize];
      for (int i = 0; i < 4; ++i)
      {
        *(reinterpret_cast<GLfloat*> (&buffer[0] + offset)) = mLights[n]->mDiffuse[i];
        offset += sizeof (GLfloat);
      }

      // Light specular color (vec4)
      offset = offsets[index++ + n * lightStructSize];
      for (int i = 0; i < 4; ++i)
      {
        *(reinterpret_cast<GLfloat*> (&buffer[0] + offset)) = mLights[n]->mSpecular[i];
        offset += sizeof (GLfloat);
      }

      // Light position (vec3)
      offset = offsets[index++ + n * lightStructSize];
      for (int i = 0; i < 3; ++i)
      {
        *(reinterpret_cast<GLfloat*> (&buffer[0] + offset)) = mLights[n]->mPosition[i];
        offset += sizeof (GLfloat);
      }

      // Light direction (vec3)
      offset = offsets[index++ + n * lightStructSize];
      for (int i = 0; i < 3; ++i)
      {
        *(reinterpret_cast<GLfloat*> (&buffer[0] + offset)) = mLights[n]->mDirection[i];
        offset += sizeof (GLfloat);
      }

      // Light attenuation (vec3)
      offset = offsets[index++ + n * lightStructSize];
      for (int i = 0; i < 3; ++i)
      {
        *(reinterpret_cast<GLfloat*> (&buffer[0] + offset)) = mLights[n]->mAttenuation[i];
        offset += sizeof (GLfloat);
      }

      // Light innerangle (float)
      offset = offsets[index++ + n * lightStructSize];
      *(reinterpret_cast<GLfloat*> (&buffer[0] + offset)) = mLights[n]->mInnerAngle;

      // Light outerangle (float)
      offset = offsets[index++ + n * lightStructSize];
      *(reinterpret_cast<GLfloat*> (&buffer[0] + offset)) = mLights[n]->mOuterAngle;

      // Light spotfalloff (float)
      offset = offsets[index++ + n * lightStructSize];
      *(reinterpret_cast<GLfloat*> (&buffer[0] + offset)) = mLights[n]->mSpotFalloff;
    }

    glBufferData (GL_UNIFORM_BUFFER, uniformBlockSize, &buffer[0], GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, 1, mlightUBO);
    glUniformBlockBinding (shaderProgram, uniformBlockIndex, 1);
  }
}

void GFX::BindLightCount()
{
  // Bind LightCount
  glGenBuffers (1, &mlightCountUBO);
  glBindBuffer (GL_UNIFORM_BUFFER, mlightCountUBO);

  GLuint shaderProgram = mPhongShader->GetShaderProgram();
  const unsigned lightCountSize = 4;

  const GLchar *uniformNames[4] =
  {
    "LightCount.Dir",
    "LightCount.Point",
    "LightCount.Spot",
    "LightCount.Total"
  };

  GLuint uniformIndices[4]; 
  glGetUniformIndices (shaderProgram, lightCountSize, uniformNames, uniformIndices);

  GLint uniformOffsets[4];
  glGetActiveUniformsiv (shaderProgram, lightCountSize, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);

  GLuint uniformBlockIndex = glGetUniformBlockIndex (shaderProgram, "LightCount");

  GLint uniformBlockSize = 0;
  glGetActiveUniformBlockiv (shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

  std::vector<unsigned char> buffer(uniformBlockSize);
  
  // number of Dir lights (int)
  int offset = uniformOffsets[0];
  *(reinterpret_cast<GLint*> (&buffer[0] + offset)) = mLightCount.mDir;

  // number of Point lights (int)
  offset = uniformOffsets[1];
  *(reinterpret_cast<GLint*> (&buffer[0] + offset)) = mLightCount.mPoint;

  // number of Spot lights (int)
  offset = uniformOffsets[2];
  *(reinterpret_cast<GLint*> (&buffer[0] + offset)) = mLightCount.mSpot;

  // number of Total lights (int)
  offset = uniformOffsets[3];
  *(reinterpret_cast<GLint*> (&buffer[0] + offset)) = mLightCount.mTotal;

  glBufferData (GL_UNIFORM_BUFFER, uniformBlockSize, &buffer[0], GL_DYNAMIC_DRAW);
  glBindBufferBase (GL_UNIFORM_BUFFER, 0, mlightCountUBO);
  glUniformBlockBinding (shaderProgram, uniformBlockIndex, 0);
}

void GFX::BindUniforms(unsigned shaderProgram)
{
  unsigned location;

  BindViewProj(shaderProgram); 

  // Set the global ambient in the fragment shader
  location = glGetUniformLocation(shaderProgram, "globalambient");
  if(location == -1)
    PrintOpenGLErrors();
  glUniform4fv(location, 1, mGlobalAmbient.array);

  // Set the fog color in the fragment shader
  location = glGetUniformLocation(shaderProgram, "fogcolor");
  if(location == -1)
    PrintOpenGLErrors();
  glUniform4fv(location, 1, mFogColor.array);

  // Set the camera pos in the fragment shader
  location = glGetUniformLocation(shaderProgram, "camPos");
  if(location == -1)
    PrintOpenGLErrors();
  glUniform3fv(location, 1, mCamera->GetPosition().array);

  // Set the near in the fragment shader
  location = glGetUniformLocation(shaderProgram, "camNear");
  if(location == -1)
    PrintOpenGLErrors();

  glUniform1f(location, mCamera->GetFar() * .5f);

  // Set the far in the fragment shader
  location = glGetUniformLocation(shaderProgram, "camFar");
  if(location == -1)
    PrintOpenGLErrors();

  glUniform1f(location, mCamera->GetFar());

  // Set the normal map scalar in the fragment shader
  location = glGetUniformLocation(shaderProgram, "normalMapScalar");
  if(location == -1)
    PrintOpenGLErrors();
  glUniform1f(location, mNormalMapAlpha);

  location = glGetUniformLocation(shaderProgram, "reflectivity");
  if(location == -1)
    PrintOpenGLErrors();

  glUniform1f(location, mReflectivity);

  location = glGetUniformLocation(shaderProgram, "refractindex");
  if(location == -1)
    PrintOpenGLErrors();

  glUniform1f(location, mRefractiveIndex);
}

void GFX::BindViewProj(unsigned shaderProgram)
{
  unsigned location;   

  // Set the view matrix in the vertex shader
  location = glGetUniformLocation(shaderProgram, "viewMatrix");
  if(location == -1)
    PrintOpenGLErrors();
  glUniformMatrix4fv(location, 1, false, mCamera->GetView().array);

  // Set the projection matrix in the vertex shader
  location = glGetUniformLocation(shaderProgram, "projectionMatrix");
  if(location == -1)
    PrintOpenGLErrors();
  glUniformMatrix4fv(location, 1, false, mCamera->GetProj().array); 
}

GFX* GFX::getInstance()
{
  static GFX* instance = 0;

  if(!instance)
    instance = new GFX();

  return instance;
}

void TW_CALL GFX::AddObject(void* selfPointer)
{
	GFX* self = (GFX*)selfPointer;

	if (self->mMeshRenderers.size() < MAXOBJECTS)
	{
		MESHTYPE type = (MESHTYPE)(self->mNumOfObj%4+1);
		self->mr = new MeshRenderer();
		self->mr->InitBV();
		self->mr->SetMesh(MeshFactory::getInstance()->GetMesh(type));
		self->mr->transform.SetPosition(Vector3(self->mNumOfObj*1.5f, (int)RandomFloat(0, MAXOBJECTS), self->mNumOfObj*1.5f));
		self->mMeshRenderers.push_back(self->mr);
		self->mNumOfObj++;

		self->mBVHierarchy = new BVH();
		self->mBVHierarchy->ConstructBVH(self->mMeshRenderers);
		self->mOctree = new Octree();
		self->mOctree->BuildOctree(self->mMeshRenderers);
		self->mBSPTree = new BSPTree();
		self->mBSPTree->ConstructTree(self->mMeshRenderers);
	}
}

void TW_CALL GFX::AddLight(void* selfPointer)
{
  GFX* self = (GFX*)selfPointer;

  if(self->mLights.size() < MAXLIGHTS)
  {
    Light* L = new Light();
    self->mLights.push_back(L);

    std::string str = "Light ";
    char c[4];
    _itoa(L->mID,c,10);
    str += c;
  }
}

void TW_CALL GFX::RemoveLight( void* selfPointer )
{
  GFX* self = (GFX*)selfPointer;

  if(self->mLights.size())
  {
    std::vector<Light*>::iterator iter;
    for(iter = self->mLights.begin(); iter != self->mLights.end(); ++iter)
    {
      if((*iter)->mID == self->mLightCount.mTotal - 1)
        break;
    }

    Light* L = (*iter);
    self->mLights.erase(iter);

    std::string str = "Light ";
    char c[4];
    _itoa(L->mID,c,10);
    str += c;

    delete L;

    self->UpdateLights(self->mFrameTime);
  }
}

unsigned GFX::GetActiveLights() const
{
  return mLightCount.mTotal;
}

void GFX::DestroyLights()
{
  while(!mLights.empty())
    RemoveLight(this);
}

void GFX::BindRenderOption( unsigned shaderProgram )
{
  unsigned location = glGetUniformLocation(shaderProgram, "option");
  if(location == -1)
    PrintOpenGLErrors();
  glUniform1i(location, (int)mRenderOption);
}

Pool<unsigned>& GFX::GetTextureHandlePool()
{
  return mTextureHandlePool;
}

void GFX::SetBindCubeMap( const int& option )
{
  mBindCubeMap = option;
}

void GFX::BindCubeMap( unsigned shaderProgram, unsigned* cubemap )
{
  unsigned location = glGetUniformLocation(shaderProgram, "enableCubeMap");
  if(location == -1)
    PrintOpenGLErrors();
  glUniform1i(location, mBindCubeMap);

  if(mBindCubeMap)
  {
    location = glGetUniformLocation(shaderProgram, "cubemap_right");
    if(location == -1)
      PrintOpenGLErrors();

    glUniform1i(location, cubemap[0]);

    location = glGetUniformLocation(shaderProgram, "cubemap_left");
    if(location == -1)
      PrintOpenGLErrors();

    glUniform1i(location, cubemap[1]);

    location = glGetUniformLocation(shaderProgram, "cubemap_top");
    if(location == -1)
      PrintOpenGLErrors();

    glUniform1i(location, cubemap[2]);

    location = glGetUniformLocation(shaderProgram, "cubemap_bot");
    if(location == -1)
      PrintOpenGLErrors();

    glUniform1i(location, cubemap[3]);

    location = glGetUniformLocation(shaderProgram, "cubemap_back");
    if(location == -1)
      PrintOpenGLErrors();

    glUniform1i(location, cubemap[4]);

    location = glGetUniformLocation(shaderProgram, "cubemap_front");
    if(location == -1)
      PrintOpenGLErrors();

    glUniform1i(location, cubemap[5]);
  }
}
