///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Shader.cpp>
//Purpose: <load shaderfiles from directory>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
//Project: <jihoon.bae_CS350_2>
//Author: <jihoon.bae>
//Creation date: <26/09/2018>
//End Header --------------------------------------------------------*/

#include "Shader.h"
#include <fstream>
#include <string>

Shader::Shader()
{
  mVertexShader = mFragmentShader = mShaderProgram = 0;
  mVSFileName = mFSFileName = 0;
  vsErr = fsErr = LinkErr = true;
}

Shader::~Shader()
{
  glDetachShader(mShaderProgram, mVertexShader);
  glDetachShader(mShaderProgram, mFragmentShader);

  glDeleteShader(mVertexShader);
  glDeleteShader(mFragmentShader);

  glDeleteProgram(mShaderProgram);

  mVertexShader = mFragmentShader = mShaderProgram = 0;
}

bool Shader::InitializeShaders(char* vsName, char* fsName)
{
  mVSFileName = vsName;
  mFSFileName = fsName;

  const char* vsBuffer;
  const char* fsBuffer;
  int error;

  // Load the vertex shader
  vsBuffer = LoadShaderFromFile(vsName);
  if(!vsBuffer)
  {
    delete [] vsBuffer;
    return false;
  }

  // Load the fragment shader
  fsBuffer = LoadShaderFromFile(fsName);
  if(!fsBuffer)
  {
    delete [] vsBuffer;
    delete [] fsBuffer;
    return false;
  }

  // Create a vertex and fragment shader object
  mVertexShader   = glCreateShader(GL_VERTEX_SHADER);
  mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  // Copy the shader source code
  glShaderSource(mVertexShader, 1, &vsBuffer, NULL);
  glShaderSource(mFragmentShader, 1, &fsBuffer, NULL);

  delete [] vsBuffer;
  delete [] fsBuffer;

  // Compile the shaders
  glCompileShader(mVertexShader);
  glCompileShader(mFragmentShader);

  // Check to see if the vertex shader compiled successfully
  glGetShaderiv(mVertexShader, GL_COMPILE_STATUS, &error);
  if(error != 1)
  {
    if(vsErr)
      OutputShaderError(mVertexShader, mVSFileName);

    vsErr = false;
    this->~Shader();

    return false;
  }

  // Check to see if the fragment shader compiled successfully
  glGetShaderiv(mFragmentShader, GL_COMPILE_STATUS, &error);
  if(error != 1)
  {
    if(fsErr)
      OutputShaderError(mFragmentShader, mFSFileName);

    fsErr = false;
    this->~Shader();

    return false;
  }

  // Create a shader program
  mShaderProgram = glCreateProgram();

  // Attach the vertex and fragment shader to the shader program
  glAttachShader(mShaderProgram, mVertexShader);
  glAttachShader(mShaderProgram, mFragmentShader);

  // Bind the shader input variables
  glBindAttribLocation(mShaderProgram, 0, "inputPosition");
  glBindAttribLocation(mShaderProgram, 1, "inputColor");
  glBindAttribLocation(mShaderProgram, 2, "inputNormal");
  glBindAttribLocation(mShaderProgram, 3, "inputTangent");
  glBindAttribLocation(mShaderProgram, 4, "inputBinormal");
  glBindAttribLocation(mShaderProgram, 5, "inputCoords");

  // Link the shader program
  glLinkProgram(mShaderProgram);

  glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &error);
  if(error != 1)
  {
    if(LinkErr)
      OutputLinkerError();

    LinkErr = false;
    this->~Shader();

    return false;
  }


  vsErr = fsErr = LinkErr = true;
  return true;
}

char* Shader::LoadShaderFromFile( char* filename )
{
  std::ifstream file;
  int fileSize = 0;
  char input;
  char* buffer;

  file.open(filename);

  if(!file.is_open())
  {
    printf("Error opening file\n");
    return 0;
  }

  file.get(input);

  while(!file.eof())
  {
    ++fileSize;
    file.get(input);
  }

  file.close();

  buffer = new char[fileSize+1];

  if(!buffer)
    return 0;

  file.open(filename);
  file.read(buffer, fileSize);
  file.close();

  buffer[fileSize] = '\0';

  return buffer;
}

void Shader::SetTexture( unsigned diffTexUnit, unsigned specTexUnit, Texture* texture )
{
  unsigned location;

  location = glGetUniformLocation(mShaderProgram, "diffuseMap");
  if(location == -1)
    PrintOpenGLErrors();

  glUniform1i(location, diffTexUnit);

  location = glGetUniformLocation(mShaderProgram, "normalMap");
  if(location == -1)
    PrintOpenGLErrors();

  glUniform1i(location, specTexUnit);

  if(texture)
  {
    // Set the width in the fragment shader
    location = glGetUniformLocation(mShaderProgram, "width");
    if(location == -1)
      PrintOpenGLErrors();

    glUniform1f(location, (float)texture->GetWidth());

    // Set the height in the fragment shader
    location = glGetUniformLocation(mShaderProgram, "height");
    if(location == -1)
      PrintOpenGLErrors();

    glUniform1f(location, (float)texture->GetHeight());
  }
}

bool Shader::ReloadShader()
{
  this->~Shader();
  return InitializeShaders(mVSFileName, mFSFileName);
}

void Shader::Use()
{
  glUseProgram(mShaderProgram);
}

void Shader::Unuse()
{
  glUseProgram(0);
}

void Shader::OutputShaderError(unsigned shaderID, char* filename)
{
  int logSize;
  char* errLog;

  glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);

  logSize++;

  errLog = new char[logSize];
  if(!errLog)
    return;

  glGetShaderInfoLog(shaderID, logSize, NULL, errLog);

  std::cout << "Error in shader: " << filename << std::endl;
  std::cout << errLog;

  delete [] errLog;
}

void Shader::OutputLinkerError()
{
  int logSize;
  char* errLog;

  glGetShaderiv(mShaderProgram, GL_INFO_LOG_LENGTH, &logSize);

  logSize++;

  errLog = new char[logSize];
  if(!errLog)
    return;

  glGetShaderInfoLog(mShaderProgram, logSize, NULL, errLog);

  std::cout << errLog;

  delete [] errLog;
}
