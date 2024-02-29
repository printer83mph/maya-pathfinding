#pragma once

#include "GL/glew.h"
#include "drawable.h"
#include <glm/mat4x4.hpp>

class ShaderProgram {
  struct Handles {
    Handles();

    // in vec4 => vertex position
    int attr_pos;
    // in vec4 => vertex color
    int attr_col;

    // uniform mat4 => model matrix
    int unif_model;
    // uniform mat4 => inverse transpose model matrix
    int unif_modelInvTr;
    // uniform mat4 => combined projection and view matrices
    int unif_viewProj;
  };

public:
  // Handles to attributes and uniforms on GPU
  Handles m_handles;
  // The vertex shader stored in this shader program
  GLuint m_vertShader;
  // The fragment shader stored in this shader program
  GLuint m_fragShader;
  // The linked shader program stored in this class
  GLuint m_prog;

  ShaderProgram();
  void create(const char *vertFile, const char *fragFile);
  void useMe();

  // Draw the given object to our screen using this ShaderProgram's shaders
  void draw(Drawable &drawable);

  // Pass model matrix to this shader on the GPU
  void setModelMatrix(const glm::mat4 &model);
  // Pass Projection * View matrix to this shader on the GPU
  void setViewProjMatrix(const glm::mat4 &vp);

private:
  // Utility functions used by draw()
  void bind(Drawable &drawable);
  void unbind();

  // Utility function used in create()
  char *textFileRead(const char *);
};
