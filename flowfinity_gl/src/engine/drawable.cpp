#include "drawable.h"

Drawable::AttributeRef::AttributeRef(GLenum type) : m_type(type), m_buffer(), m_isBound(false) {}
Drawable::AttributeRef::~AttributeRef() {}

Drawable::Attributes::Attributes()
    : idx(GL_ELEMENT_ARRAY_BUFFER), pos(GL_ARRAY_BUFFER), col(GL_ARRAY_BUFFER), nor(GL_ARRAY_BUFFER)
{
}

Drawable::Drawable() : m_count(-1), m_attributes() {}

Drawable::~Drawable() {}

void Drawable::destroy()
{
  m_attributes.idx.destroy();
  m_attributes.pos.destroy();
  m_attributes.col.destroy();
  m_attributes.nor.destroy();
}

// default draw mode is `GL_TRIANGLES`
GLenum Drawable::drawMode() { return GL_TRIANGLES; }

int Drawable::elemCount() { return m_count; }