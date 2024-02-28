#include "drawable.h"

Drawable::Drawable() : m_count(-1), buf_idx(), buf_col(), buf_pos() {}

Drawable::~Drawable() {}

void Drawable::destroy() {
  glDeleteBuffers(1, &buf_idx);
  glDeleteBuffers(1, &buf_pos);
  glDeleteBuffers(1, &buf_col);
}

// default draw mode is `GL_TRIANGLES`
GLenum Drawable::draw_mode() { return GL_TRIANGLES; }

int Drawable::elem_count() { return m_count; }