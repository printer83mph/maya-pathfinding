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
  std::istringstream frameStream(simulationData);
  std::string frame;
  while (std::getline(frameStream, frame, '\n')) {
    if (frame.empty())
      continue;

    std::vector<std::pair<glm::vec2, glm::vec2>> agents;

    // split by semicolon (agent)
    std::istringstream tokenStream(simulationData);
    std::string token;
    while (std::getline(tokenStream, token, ';')) {
      if (token.empty())
        continue;

      std::vector<float> agentData;

      // split by comma
      std::istringstream agentStream(token);
      std::string agentToken;
      while (std::getline(agentStream, agentToken, ',')) {
        agentData.push_back(std::stof(agentToken));
      }

      // make sure we have all the data we need
      if (agentData.size() != 4) {
        throw "Invalid agent data";
      }

      agents.push_back(
          {glm::vec2(agentData[0], agentData[1]), glm::vec2(agentData[2], agentData[3])});
    }

    result.push_back(agents);
  }

  return result;
}
