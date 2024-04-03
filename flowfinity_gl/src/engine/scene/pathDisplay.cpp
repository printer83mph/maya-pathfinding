#define GLM_ENABLE_EXPERIMENTAL
#include "pathDisplay.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtx/transform.hpp"

#include <glm/vec4.hpp>
#include <vector>

PathDisplay::PathDisplay() : Drawable(), m_path() {}

PathDisplay::PathDisplay(const std::vector<glm::vec3> &path)
    : Drawable(), m_path(path) {}

PathDisplay::~PathDisplay() {}

void PathDisplay::create() {
  std::vector<glm::vec4> pos{}, col{};

  for (auto &point : m_path) {
    pos.push_back(glm::vec4(point, 1));
    col.push_back(glm::vec4(1, 1, 1, 1));
  }

  std::vector<GLuint> idx{};
  for (int i = 0; i < pos.size() - 1; i++) {
    idx.push_back(i);
    idx.push_back(i + 1);
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
}

GLenum PathDisplay::drawMode() { return GL_LINES; }