#include "flowfinity/crowdsim.h"
#include "flowfinity/encoding.h"
#include "flowfinity/navigation/visibilitygraph.h"

#include <iostream>

int main()
{
  CrowdSim sim;
  auto navMethod = VisibilityGraph();

  navMethod.addCubeObstacle({0.1, 0.2}, {1., 2.}, 12.f);

  sim.m_config.inOutFlows.push_back({{3.f, 3.f}, {-3.f, -3.f}});

  auto out = FFEncoding::encodeSimulation(sim, 24 * 5, 1.f / 24.f, navMethod);

  std::cout << out << std::endl;

  return 0;
}
