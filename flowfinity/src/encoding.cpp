#include "flowfinity/encoding.h"

#include <sstream>

std::string FFEncoding::encodeSimulation(CrowdSim& crowdSim, int frameCount, float frameDeltaTime,
                                         NavMethod& navMethod)
{
  std::string result;
  for (int i = 0; i < frameCount; ++i) {
    crowdSim.performTimeStep(frameDeltaTime, &navMethod);
    auto& pos = crowdSim.getAgentPositions();
    auto& vel = crowdSim.getAgentVelocities();

    for (int i = 0; i < crowdSim.size(); ++i) {
      result += std::to_string(pos[i].x) + "," + std::to_string(pos[i].y) + "," +
                std::to_string(vel[i].x) + "," + std::to_string(vel[i].y) + ";";
    }
    result += "\n";
  }
  return result;
}

std::vector<std::vector<std::pair<glm::vec2, glm::vec2>>>
FFEncoding::decodeSimulation(const std::string& simulationData)
{
  std::vector<std::vector<std::pair<glm::vec2, glm::vec2>>> result;

  // split by newline (frame)
  std::istringstream iss(simulationData);
  std::string line;
  while (std::getline(iss, line)) {
    std::vector<std::pair<glm::vec2, glm::vec2>> frame;
    // split by ;
    std::istringstream iss2(line);
    std::string agentData;
    while (std::getline(iss2, agentData, ';')) {
      // split by ,
      std::istringstream iss3(agentData);
      std::string token;
      std::vector<std::string> tokens;
      while (std::getline(iss3, token, ',')) {
        tokens.push_back(token);
      }
      if (tokens.size() == 4) {
        frame.push_back({{std::stof(tokens[0]), std::stof(tokens[1])},
                         {std::stof(tokens[2]), std::stof(tokens[3])}});
      }
    }
    result.push_back(frame);
  }

  return result;
}
