#include "FlowFinityNode.h"
#include "flowfinity/crowdsim.h"
#include "flowfinity/navigation/visibilitygraph.h"
#include <maya/MGlobal.h>

#include <maya/MFloatArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnNObjectData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MnParticle.h>

const MTypeId FlowFinityNode::id(0x85003);
MObject FlowFinityNode::startState;
MObject FlowFinityNode::currentState;
MObject FlowFinityNode::nextState;
MObject FlowFinityNode::currentTime;
VisibilityGraph FlowFinityNode::m_visibilityGraph;

void* FlowFinityNode::creator() { return new FlowFinityNode(); }

const char* FlowFinityNode::className() { return "FlowFinityNode"; }

inline void statCheck(MStatus stat, MString msg)
{
  if (!stat) {
    cout << msg << "\n";
  }
}

MStatus FlowFinityNode::initialize()
{
  MStatus stat;

  MFnTypedAttribute tAttr;

  m_visibilityGraph = VisibilityGraph();

  // These would be inputs from the GUI
  glm::vec2 translation = glm::vec2(1, 4);
  glm::vec2 scale = glm::vec2(2, 1);

  glm::vec2 translation2 = glm::vec2(5, 6);
  glm::vec2 scale2 = glm::vec2(1, 2);

  m_visibilityGraph.addCubeObstacle(translation, scale, 0.0f);
  m_visibilityGraph.addCubeObstacle(translation2, scale2, 100.0f);

  m_visibilityGraph.createGraph();

  startState = tAttr.create("startState", "sst", MFnData::kNObject, MObject::kNullObj, &stat);

  statCheck(stat, "failed to create startState");
  tAttr.setWritable(true);
  tAttr.setStorable(true);
  // tAttr.setHidden(true);
  tAttr.setArray(true);

  currentState = tAttr.create("currentState", "cst", MFnData::kNObject, MObject::kNullObj, &stat);

  statCheck(stat, "failed to create currentState");
  tAttr.setWritable(true);
  tAttr.setStorable(true);
  // tAttr.setHidden(true);
  tAttr.setArray(true);

  nextState = tAttr.create("nextState", "nst", MFnData::kNObject, MObject::kNullObj, &stat);

  statCheck(stat, "failed to create nextState");
  tAttr.setWritable(true);
  tAttr.setStorable(true);
  // tAttr.setHidden(true);
  tAttr.setArray(true);

  MFnUnitAttribute uniAttr;
  currentTime = uniAttr.create("currentTime", "ctm", MFnUnitAttribute::kTime, 0.0, &stat);

  addAttribute(startState);
  addAttribute(currentState);
  addAttribute(nextState);
  addAttribute(currentTime);

  attributeAffects(startState, nextState);
  attributeAffects(currentState, nextState);
  attributeAffects(currentTime, nextState);

  return MStatus::kSuccess;
}

MStatus FlowFinityNode::compute(const MPlug& plug, MDataBlock& data)
{
  MStatus stat;
  if (plug == nextState) {

    // Get current time
    MTime currTime = data.inputValue(currentTime).asTime();
    MObject inputData;

    // Depending on the time, pull on start or current state

    if (currTime.value() <= 0.0) {
      MArrayDataHandle multiDataHandle = data.inputArrayValue(startState);
      multiDataHandle.jumpToElement(0);
      inputData = multiDataHandle.inputValue().data();
    } else {
      MArrayDataHandle multiDataHandle = data.inputArrayValue(currentState);
      multiDataHandle.jumpToElement(0);
      inputData = multiDataHandle.inputValue().data();
    }

    // Create a Crowd Sim Instance
    CrowdSim crowdSim;
    crowdSim.m_config.inOutFlows.push_back(std::make_pair(glm::vec2(-5, -5), glm::vec2(10, 10)));

    MFnNObjectData nData(inputData);
    MnParticle* nObj = NULL;
    nData.getObjectPtr(nObj);

    // Get the particle positions
    MFloatPointArray points;
    nObj->getPositions(points);

    // Get the particle velocities
    MFloatPointArray velocities;
    nObj->getVelocities(velocities);

    // Import the agents into the crowd sim
    std::vector<glm::vec2> pos;
    std::vector<glm::vec2> vel;
    for (unsigned int ii = 0; ii < points.length(); ii++) {
      pos.push_back(glm::vec2(points[ii].x, points[ii].z));
      vel.push_back(glm::vec2(velocities[ii].x, velocities[ii].z));
    }
    crowdSim.importAgents(pos, vel);

    // unsigned int ii;
    // for (ii = 0; ii < points.length(); ii++) {
    //   points[ii].y = (float)sin(points[ii].x + currTime.value() * 4.0f * (3.1415f / 180.0f));
    // }

    // Perform a time step
    crowdSim.unfastComputeAllTargetsFromFirstInOutFlow(&m_visibilityGraph);
    auto x = std::string();
    x += "\nfinal target x: " + std::to_string(crowdSim.getAgentFinalTargets().at(0).x);
    x += "\nfinal target y: " + std::to_string(crowdSim.getAgentFinalTargets().at(0).y);
    x += "\n\ncurrent target x: " + std::to_string(crowdSim.getAgentCurrentTargets().at(0).x);
    x += "\ncurrent target y: " + std::to_string(crowdSim.getAgentCurrentTargets().at(0).y);
    MGlobal::displayInfo(x.c_str());
    crowdSim.performTimeStep(0.1f);

    // Get the new positions
    pos = crowdSim.getAgentPositions();
    vel = crowdSim.getAgentVelocities();
    for (unsigned int ii = 0; ii < points.length(); ii++) {
      points[ii].x = pos[ii].x;
      points[ii].z = pos[ii].y;
      velocities[ii].x = vel[ii].x;
      velocities[ii].z = vel[ii].y;
    }

    nObj->setPositions(points);
    nObj->setVelocities(velocities);

    delete nObj;
    data.setClean(plug);
  }

  else if (plug == currentState) {
    data.setClean(plug);
  }

  else if (plug == startState) {
    data.setClean(plug);
  } else {
    stat = MS::kUnknownParameter;
  }

  return MS::kSuccess;
}
