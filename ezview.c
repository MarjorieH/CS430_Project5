#include "ezview.h"

static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec2 TexCoordIn;\n"
"attribute vec2 vPos;\n"
"varying vec2 TexCoordOut;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    TexCoordOut = TexCoordIn;\n"
"}\n";

static const char* fragment_shader_text =
"varying vec2 TexCoordOut;\n"
"uniform sampler2D Texture;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(Texture, TexCoordOut);\n"
"}\n";

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  else if (key == GLFW_KEY_1 && action == GLFW_PRESS) { // translate
    current = 0.0;
    imageState = 1;
  }
  else if (key == GLFW_KEY_2 && action == GLFW_PRESS) { // rotate
    current = 0.0;
    imageState = 2;
  }
  else if (key == GLFW_KEY_3 && action == GLFW_PRESS) { // scale
    current = 0.0;
    imageState = 3;
  }
  else if (key == GLFW_KEY_4 && action == GLFW_PRESS) { // shear
    current = 0.0;
    imageState = 4;
  }
  else if (action == GLFW_PRESS) { // reset
    imageState = 0;
  }
}

void glCompileShaderOrDie(GLuint shader) {
  GLint compiled;
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    char* info = malloc(infoLen + 1);
    GLint done;
    glGetShaderInfoLog(shader, infoLen, &done, info);
    printf("Unable to compile shader: %s\n", info);
    exit(1);
  }
}

void reset_transformations(mat4x4 m) {
  mat4x4_rotate_Z(m, m, 3.14159);
  mat4x4_scale(m, m, 1.0);
  current = 0.0;
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Error: usage: ezview input.ppm\n");
    return(1);
  }
  FILE* fh = fopen(argv[1], "r");
  if (!fh) {
    fprintf(stderr, "Error: Unable to open file.\n");
    return(1);
  }
  store_image(fh);

  GLFWwindow* window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program;
  GLint mvp_location, vpos_location, vcol_location;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
  exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ezview", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // NOTE: OpenGL error checks have been omitted for brevity

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShaderOrDie(vertex_shader);

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShaderOrDie(fragment_shader);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  // more error checking! glLinkProgramOrDie!

  mvp_location = glGetUniformLocation(program, "MVP");
  assert(mvp_location != -1);

  vpos_location = glGetAttribLocation(program, "vPos");
  assert(vpos_location != -1);

  GLint texcoord_location = glGetAttribLocation(program, "TexCoordIn");
  assert(texcoord_location != -1);

  GLint tex_location = glGetUniformLocation(program, "Texture");
  assert(tex_location != -1);

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

  glEnableVertexAttribArray(texcoord_location);
  glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 2));

  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixmap);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);
  glUniform1i(tex_location, 0);

  while (!glfwWindowShouldClose(window)) {
    float ratio;
    int width, height;
    mat4x4 m, p, mvp;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    if (imageState == 0) { // still image
      mat4x4_identity(m);
      reset_transformations(m);
      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4x4_mul(mvp, p, m);
    }
    else if (imageState == 1) { // translate
      mat4x4_identity(m);
      mat4x4_translate(m, current, current, 1.0);
      mat4x4_rotate_Z(m, m, 3.14159);
      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4x4_mul(mvp, p, m);
      current += interval;
    }
    else if (imageState == 2) { // rotate
      mat4x4_identity(m);
      mat4x4_rotate_Z(m, m, 3.14159 + current);
      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4x4_mul(mvp, p, m);
      current += interval;
    }
    else if (imageState == 3) { // scale
      mat4x4_identity(m);
      mat4x4_rotate_Z(m, m, 3.14159);
      mat4x4_scale_aniso(m, m, current, current, 1.0);
      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4x4_mul(mvp, p, m);
      current += interval;
    }
    else { // shear
      mat4x4_identity(m);
      mat4x4_rotate_Z(m, m, 3.14159);
      m[0][1] = current;
      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4x4_mul(mvp, p, m);
      current += interval;
    }

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}
