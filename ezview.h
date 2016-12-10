#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>

#include "../deps/linmath.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct {
  float Position[2];
  float TexCoord[2];
} Vertex;

// (-1, 1)  (1, 1)
// (-1, -1) (1, -1)

Vertex vertices[] = {
  {{1, -1}, {0.99999, 0}},
  {{1, 1},  {0.99999, 0.99999}},
  {{-1, 1}, {0, 0.99999}}
};


// Function prototype declarations
void rotate_image(void);
void translate_image(void);
void scale_image(void);
void shear_image(void);
