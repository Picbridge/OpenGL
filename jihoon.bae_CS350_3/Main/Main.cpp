/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <main.cpp>
Purpose: <The main application body>
Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
Project: <jihoon.bae_CS350_3>
Author: <jihoon.bae>
Creation date: <26/09/2018>
End Header --------------------------------------------------------*/

// Includes
#include <time.h>
#include <windows.h>
#include "GFX.h"
#include "AntTweakBar/AntTweakBar.h"
#include "Support.h"
#include "OpenGL/glut.h"

//-----------------------------------------------------------------------------
// Glut assumes everthing is global (no userdata for callbacks! ;-;)
int winID;
GFX* mGraphics;
clock_t gLastTime = clock();

//-----------------------------------------------------------------------------
// Glut idle function where we update (glut will call this for us)
void Idle()
{
  // Determine the new clock ticks
  clock_t newTime = clock();

  // Get the difference between the new clock tick and the last clock tick
  clock_t ticks = newTime - gLastTime;

  // Divide out by tick frequency to get the frame time passed
  float frameTime = ticks / (float)CLOCKS_PER_SEC;

  // Update the last tick count
  gLastTime = newTime;

  // Now update the application with the frame time
  mGraphics->Update(frameTime);

  // Tell glut to redisplay the last frame buffer
  glutPostRedisplay();
}

//-----------------------------------------------------------------------------
// Glut display function where we draw (glut will call this for us)
void Display()
{
  // Let the application draw and present to the screen
  mGraphics->Draw();

  // Draw all the AntTweakBar windows / ui
  TwDraw();

  // Finally, present the back buffer to the screen
  glutSwapBuffers();
}

//-----------------------------------------------------------------------------
// Glut window resize/reshape function (glut will call this for us)
void Reshape(int width, int height)
{
  // Tell AntTweakBar that our window has been resized
  TwWindowSize(width, height);

  // Prevent a divide by zero, when window is too short
  // (you cant make a window of zero width).
  if (height <= 0)
  {
    height = 1;
  }
  
  // Compute the aspect ratio as w/h
  float aspectRatio = width / (float)height;

  // Set the viewport to be the entire window
  glViewport(0, 0, width, height);
  
  // Let the application handle it's own resizing
  mGraphics->Reshape(width, height, aspectRatio);
}

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  // Starting width / height of the window
  const int WindowWidth = 1024;
  const int WindowHeight = 768;

  // Initialize GLUT
  glutInit(&argc, argv);

  // Get how big our screen is that we're displaying the window on
  int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
  int screneHeight = glutGet(GLUT_SCREEN_HEIGHT);

  // Initialize the display mode to utilize double buffering, 4-channel framebuffer and depth buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
 
  // Setup the window
  glutInitWindowSize(WindowWidth, WindowHeight);
  glutInitWindowPosition((screenWidth - WindowWidth) / 2, (screneHeight - WindowHeight) / 2);
  winID = glutCreateWindow("jihoon.bae_CS350_3");
  
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  //fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  // Assign callback functions
  glutDisplayFunc(Display);
  glutIdleFunc(Idle); 
  glutReshapeFunc(Reshape);

  // Setup all the open-gl states we want to use (ones that don't change in the lifetime of the application)
  // Note: These can be changed anywhere, but generally we don't change the back buffer color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_LIGHTING);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(PRIMITIVEINDEX);
  
  // Show the window that we just initailized
  glutShowWindow();

  // Initialize AntTweakBar
  TwInit(TW_OPENGL, NULL);
  TwWindowSize(WindowWidth, WindowHeight);

  // We redirect all GLUT functions to point at AntTweakBar's input
  // If we want to do our own input, we need to make our own callbacks
  // Where we can intercept the input and remove these
  glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
  glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
  glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
  glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
  glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
 
  // Send the 'glutGetModifers' function pointer to AntTweakBar
  TwGLUTModifiersFunc(glutGetModifiers);

  // Check to see if we had any errors
  PrintOpenGLErrors();

  // Create the application
  mGraphics = GFX::getInstance();
  mGraphics->Initialize();

  // Now let glut takeover the main loop (we'll get callbacks in our display / idle functions)
  glutMainLoop();

  // Glut never returns here, so I'm not sure what we need to do to be proper...
  delete mGraphics;
  return 0;
}
