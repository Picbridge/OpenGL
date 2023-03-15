
#include "Skybox.h"
#include "MeshFactory.h"

bool Skybox::Initialize( const std::string& dir, 
                         const std::string& PosXFile, 
                         const std::string& NegXFile, 
                         const std::string& PosYFile, 
                         const std::string& NegYFile, 
                         const std::string& PosZFile, 
                         const std::string& NegZFile )
{
  mSkyboxMesh = new MeshRenderer();
  mSkyboxMesh->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_CUBE));
  mSkyboxMesh->transform.SetScale(21.f);

  mCubeMapTexture = new CubeMapTexture();
  bool result = mCubeMapTexture->LoadTexturesFromFile(dir,
                                                      PosXFile,
                                                      NegXFile,
                                                      PosYFile,
                                                      NegYFile,
                                                      PosZFile,
                                                      NegZFile);

  return result;
}

Skybox::~Skybox()
{
  delete mCubeMapTexture;
  delete mSkyboxMesh;
}

void Skybox::SetWorldMatrix( const Vector3& camPos )
{
  mSkyboxMesh->transform.SetPosition(camPos);
  mSkyboxMesh->transform.Update();
}

void Skybox::Render()
{
  mSkyboxMesh->GetMesh()->DrawMesh();
}

void Skybox::Bind( unsigned ShaderProgram )
{
  mSkyboxMesh->BindMesh(ShaderProgram);

  unsigned location;

  location = glGetUniformLocation(ShaderProgram, "cubemapTexture");
  if(location == -1)
    PrintOpenGLErrors();

  glUniform1i(location, mCubeMapTexture->GetTextureUnit());
}
