#include "square.h"

#include <glm/vec4.hpp>
#include <vector>

Square::Square() : Drawable() {}

Square::~Square() {}

void Square::create()
{
  std::vector<glm::vec4> pos{glm::vec4(-1, -1, 0, 1), glm::vec4(1, -1, 0, 1), glm::vec4(1, 1, 0, 1),
                             glm::vec4(-1, 1, 0, 1)};

  std::vector<glm::vec4> col{glm::vec4(0, 0.6, 0, 1), glm::vec4(0, 0.5, 0, 1),
                             glm::vec4(0, 0.6, 0, 1), glm::vec4(0, 0.4, 0, 1)};

  std::vector<GLuint> idx{0, 1, 2, 0, 2, 3};

  m_count = 6;

  m_attributes.idx.generate();
  m_attributes.idx.bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

  m_attributes.pos.generate();
  m_attributes.pos.bind();
  glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

  m_attributes.col.generate();
  m_attributes.col.bind();
  glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}