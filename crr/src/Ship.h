#pragma once
#define _USE_MATH_DEFINES
#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include "ObjModel.h"

constexpr auto TWO_PI = 2 * M_PI;
constexpr auto DEGREE = M_PI / 180.0;

GLuint loadTexture(const char* file_name);

void init(void);

void displayShip(void);

void Reshape(GLint newWidth, GLint newHeight);

void Mouse(int button, int state, int x, int y);

void MouseMotion(GLint x, GLint y);

void Keyboard(GLubyte key, GLint x, GLint y);