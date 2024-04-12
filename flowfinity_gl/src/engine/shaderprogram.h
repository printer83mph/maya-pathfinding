#pragma once

#include "drawable.h"

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <string>

class ShaderProgram
{
  struct Handles {
    Handles();

    // in vec4 => vertex position
    int attr_pos;
    // in vec4 => vertex color
    int attr_col;
    // in vec4 => vertex normal
    int attr_nor;
    // uniform mat4 => model matrix
    int unif_model;
    // uniform mat4 => inverse transpose model matrix
    int unif_modelInvTr;
    // uniform mat4 => combined projection and view matrices
    int unif_viewProj;
    // uniform vec3 => camera position
    int unif_camPos;
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
  void create(const char* vertFile, const char* fragFile);
  void useMe();

  // Draw the given object to our screen using this ShaderProgram's shaders
  void draw(Drawable& drawable);

  // Pass model matrix to this shader on the GPU
  void setModelMatrix(const glm::mat4& model);
  // Pass Projection * View matrix to this shader on the GPU
  void setViewProjMatrix(const glm::mat4& vp);
  // Pass camera position to this shader on the GPU
  void setCamPos(const glm::vec3& cp);

private:
  // Utility functions used by draw()
  void bindDrawable(Drawable& drawable);
  void unbindDrawable();

  // Utility function used in create()
  std::string textFileRead(const char*);
};
