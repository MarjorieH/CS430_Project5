#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>

#include "../deps/linmath.h"
#include "pixmap.c"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define ONE 0.99999
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

typedef struct {
  float Position[2];
  float TexCoord[2];
} Vertex;

// (-1, 1)  (1, 1)
// (-1, -1) (1, -1)
Vertex vertices[] = {
  {{-1, -1}, {  0,   0}}, // bottom left
  {{-1,  1}, {  0, ONE}}, // top left
  {{ 1,  1}, {ONE, ONE}}, // top right
  {{ 1, -1}, {ONE,   0}}, // bottom right
  {{-1, -1}, {  0,   0}} // bottom left
};

int imageState = 0;
float interval = 0.01;
float current = 0.0;
