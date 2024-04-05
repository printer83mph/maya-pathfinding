#include "cube.h"
#include "glm/ext/matrix_float4x4.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#undef GLM_ENABLE_EXPERIMENTAL

#include <glm/vec4.hpp>
#include <vector>

Cube::Cube() : Drawable(), x(0), y(0), z(0) {}

Cube::Cube(float x, float y, float z) : Drawable(), x(x), y(y), z(z) {}

Cube::~Cube() {}

void Cube::create() {
  std::vector<glm::vec4> pos_data{
      glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1), glm::vec4(1, 1, -1, 1),
      glm::vec4(-1, 1, -1, 1)};

  std::vector<glm::vec4> pos{
      glm::vec4(-1 + x, -1 + y, -1 + z, 1), glm::vec4(1 + x, -1 + y, -1 + z, 1),
      glm::vec4(1 + x, 1 + y, -1 + z, 1), glm::vec4(-1 + x, 1 + y, -1 + z, 1)};

  std::vector<glm::vec4> nor{glm::vec4(0, 0, -1, 0), glm::vec4(0, 0, -1, 0),
                             glm::vec4(0, 0, -1, 0), glm::vec4(0, 0, -1, 0)};

  std::vector<glm::vec4> col{
      glm::vec4(0.5, 0.5, 0.5, 1), glm::vec4(0.5, 0.5, 0.5, 1),
      glm::vec4(0.5, 0.5, 0.5, 1), glm::vec4(0.5, 0.5, 0.5, 1)};

  std::vector<GLuint> idx{0, 1, 2, 0, 2, 3};

  for (int face = 1; face < 6; face++) {
    glm::mat4 rot;
    if (face < 4) {
      rot = glm::rotate(glm::radians(90.0f * face), glm::vec3(0, 1, 0));
    } else if (face == 4) {
      rot = glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));
    } else {
      rot = glm::rotate(glm::radians(270.0f), glm::vec3(1, 0, 0));
    }

    for (int i = 0; i < 4; i++) {
      pos.push_back((rot * pos_data[i]) + glm::vec4(x, y, z, 0));
      nor.push_back(rot * nor[i]);
      col.push_back(glm::vec4(0.5, 0.5, 0.5, 1));
    }
    for (int j = 0; j < 6; j++) {
      idx.push_back(face * 4 + idx[j]);
    }
  }
  m_count = idx.size();

  m_attributes.idx.generate();
  m_attributes.idx.bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(),
               GL_STATIC_DRAW);

  m_attributes.pos.generate();
  m_attributes.pos.bind();
  glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(),
               GL_STATIC_DRAW);

  m_attributes.col.generate();
  m_attributes.col.bind();
  glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(),
               GL_STATIC_DRAW);

  m_attributes.nor.generate();
  m_attributes.nor.bind();
  glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(),
               GL_STATIC_DRAW);
}