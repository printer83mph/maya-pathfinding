#pragma once

#include "crowdsim.h"
#include "navigation/navmethod.h"

#include <glm/vec2.hpp>
#include <string>
#include <vector>

namespace FFEncoding
{
/**
 * Encode simulation data from a CrowdSim object into a string for use in Maya.
 *
 * The format of the string is:
 *
 * pos.x,pos.y,vel.x,vel.y;pos.x,pos.y,vel.x,vel.y;...
 * pos.x,pos.y,vel.x,vel.y;pos.x,pos.y,vel.x,vel.y;...
 *
 * Where each agent is separated by a semicolon and each agent's position and velocity are
 * separated by a comma. Each time step is separated by a newline.
 *
 * The resulting string data can then be parsed by the decodeSimulation function to retreive
 * stored positions and velocities for every frame.
 */
std::string encodeSimulation(CrowdSim& crowdSim, int frameCount, float frameDeltaTime,
                             NavMethod& navMethod);

/**
 * Decode simulation data from a string into a vector of vectors of pairs of glm::vec2.
 *
 * The format of the string is:
 *
 * pos.x,pos.y,vel.x,vel.y;pos.x,pos.y,vel.x,vel.y;...
 * pos.x,pos.y,vel.x,vel.y;pos.x,pos.y,vel.x,vel.y;...
 *
 * Where each agent is separated by a semicolon and each agent's position and velocity are
 * separated by a comma. Each time step is separated by a newline.
 *
 * The resulting vector of vectors of pairs of glm::vec2 contains the position and velocity
 * data for each agent at each frame.
 */
std::vector<std::vector<std::pair<glm::vec2, glm::vec2>>>
decodeSimulation(const std::string& simulationData);
} // namespace FFEncoding
