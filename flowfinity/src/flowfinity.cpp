#include "flowfinity.h"

#include <iterator>

FlowFinity::FlowFinity() : m_pos(), m_vel() {}

FlowFinity::~FlowFinity() {}

void FlowFinity::performTimeStep(float dt) {
  // TODO
}

void FlowFinity::addAgent(float x, float y) {
  m_pos.push_back(glm::vec2(x, y));
  m_vel.push_back(glm::vec2(0, 0));
}

void FlowFinity::addAgent(const glm::vec2 &pos) { addAgent(pos.x, pos.y); }

void FlowFinity::removeAgent(size_t index) {
  if (index >= m_pos.size()) {
    throw "index out of bounds lol";
  }
  m_pos.erase(std::next(m_pos.begin(), index));
  m_vel.erase(std::next(m_vel.begin(), index));
}