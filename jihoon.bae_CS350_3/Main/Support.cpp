/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <Support.cpp>
Purpose: <The cpp file that supports main>
Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
Project: <jihoon.bae_CS350_3>
Author: <jihoon.bae>
Creation date: <26/03/2019>
End Header --------------------------------------------------------*/

// Includes
#include "Support.h"
#include "OpenGL/glut.h"
#include "Math/Utilities.hpp"

//-----------------------------------------------------------------------------
bool ReportOpenGLErrors(const char* file, int line)
{
  // Get the error code from OpenGL
  GLenum errorCode = glGetError();

  // If there actually was an error...
  if (errorCode != GL_NO_ERROR)
  {
    // Store the last slash (if we don't find one, just start at the beginning!)
    const char* fileLastSlash = file;

    // We only want to get the last slash in the file to just show the file name
    while (*file != NULL)
    {
      // Get the current character
      char c = *file;

      // If the character is a slash...
      if (c == '\\' || c == '/')
      {
        // Update the last slash we've seen (move ahead of the slash by 1)
        fileLastSlash = file + 1;
      }

      // Move forward in the string
      ++file;
    }

    // Print the error out to the console and return that an error occurred
    PrintError("OpenGL Error %s(%d): %s\n", fileLastSlash, line, gluErrorString(errorCode));
    return true;
  }

  // No error occurred!
  return false;
}

std::string CreateKeyString( unsigned firstVert, unsigned secondVert )
{
  std::string key;
  char c[8];

  _itoa(firstVert, c, 10);
  key += c;
  key += "-";
  _itoa(secondVert, c, 10);
  key += c;

  return key;
}
