/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <Support.cpp>
Purpose: <The h file that supports main>
Language: <c++,visual studio2017>
//Platform: <visual studio2017, win10>
Project: <jihoon.bae_CS350_3>
Author: <jihoon.bae>
Creation date: <26/03/2019>
End Header --------------------------------------------------------*/
#pragma once
#include "OpenGL/glew.h"
#include <stdlib.h>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include "Math/Math.hpp"
#include "Math/Utilities.hpp"
#include "AntTweakBar/AntTweakBar.h"
#include "Pool.h"

using namespace Math;

// Macro that we can use to report OpenGL errors
// This is slow, but it can be used for debugging
// Returns true if an error occurred, false otherwise
#define PrintOpenGLErrors() ReportOpenGLErrors(__FILE__, __LINE__)

// Reports OpenGL errors to the console
// Returns true if an error occurred, false otherwise
bool ReportOpenGLErrors(const char* file, int line);

std::string CreateKeyString(unsigned firstVert, unsigned secondVert);
