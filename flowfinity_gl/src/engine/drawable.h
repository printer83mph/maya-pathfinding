#pragma once

#include <GL/glew.h>

class Drawable {
  class AttributeRef {
  private:
    GLuint m_buffer;
    bool m_isBound;
    // type of VBO – i.e. GL_ARRAY_BUFFER
    GLenum m_type;

  public:
    AttributeRef(GLenum type);
    ~AttributeRef();

    inline void bind() { glBindBuffer(m_type, m_buffer); }
    inline bool tryBind() {
      if (m_isBound)
        bind();
      return m_isBound;
    }
    inline void generate() {
      m_isBound = true;
      glGenBuffers(1, &m_buffer);
    }
    inline void destroy() {
      m_isBound = false;
      glDeleteBuffers(1, &m_buffer);
    }
  };

  struct Attributes {
    AttributeRef idx;
    AttributeRef pos;
    AttributeRef col;

  public:
    Attributes();
  };

protected:
  int m_count;

public:
  Drawable();
  virtual ~Drawable();

  virtual void create() = 0;
  void destroy();

  virtual GLenum drawMode();
  int elemCount();

  Attributes m_attributes;
};