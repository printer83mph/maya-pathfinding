#pragma once

#include <GL/glew.h>

#define GL_ATTRIBUTE_DEF(name, type)                                           \
protected:                                                                     \
  GLuint buf_##name;                                                           \
  bool name##_bound;                                                           \
                                                                               \
public:                                                                        \
  inline bool bind_##name() {                                                  \
    if (name##_bound) {                                                        \
      glBindBuffer(type, buf_##name);                                          \
    }                                                                          \
    return name##_bound;                                                       \
  }                                                                            \
  inline void generate_##name() {                                              \
    name##_bound = true;                                                       \
    glGenBuffers(1, &buf_##name);                                              \
  }

class Drawable {
protected:
  int m_count;

public:
  Drawable();
  virtual ~Drawable();

  virtual void create() = 0;
  void destroy();

  virtual GLenum draw_mode();
  int elem_count();

  // MAKE SURE TO ADD ATTR's TO CONSTRUCTOR & DESTROY FN's
  GL_ATTRIBUTE_DEF(idx, GL_ELEMENT_ARRAY_BUFFER)
  GL_ATTRIBUTE_DEF(pos, GL_ARRAY_BUFFER)
  GL_ATTRIBUTE_DEF(col, GL_ARRAY_BUFFER)
};