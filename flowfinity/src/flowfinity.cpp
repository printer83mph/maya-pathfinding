#include "flowfinity.h"

#include <iterator>

FlowFinity::FlowFinity()
    : m_rvoPos(), m_rvoVel(), m_rvoTarget(), m_targets({glm::vec2(3, 2)}) {}

FlowFinity::~FlowFinity() {}

void FlowFinity::performTimeStep(float dt) {
  // TODO
}

void FlowFinity::addAgent(const glm::vec2 &pos, unsigned int target) {
  m_rvoPos.push_back(pos);
  m_rvoVel.push_back(glm::vec2(0, 0));
  m_rvoTarget.push_back(target);
}

void FlowFinity::removeAgent(unsigned int index) {
  if (index >= m_rvoPos.size()) {
    throw "index out of bounds lol";
  }
  m_rvoPos.erase(std::next(m_rvoPos.begin(), index));
  m_rvoVel.erase(std::next(m_rvoVel.begin(), index));
  m_rvoTarget.erase(std::next(m_rvoTarget.begin(), index));
}
