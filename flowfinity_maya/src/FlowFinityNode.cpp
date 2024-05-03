#include "FlowFinityNode.h"
#include "flowfinity/crowdsim.h"
#include "flowfinity/encoding.h"
#include "flowfinity/navigation/visibilitygraph.h"

#include <maya/MApiNamespace.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnArrayAttrsData.h>
#include <maya/MFnData.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNObjectData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MnParticle.h>

#include <string>

const MTypeId FlowFinityNode::id(0x85003);

MObject FlowFinityNode::agentMaxSpeed;
MObject FlowFinityNode::agentAcceleration;
MObject FlowFinityNode::agentDrag;
MObject FlowFinityNode::agentRadius;
MObject FlowFinityNode::agentAggressiveness;
MObject FlowFinityNode::spawnRate;
MObject FlowFinityNode::maxAgents;
MObject FlowFinityNode::obstacleTransforms;
MObject FlowFinityNode::inOutFlows;
MObject FlowFinityNode::startTime;
MObject FlowFinityNode::endTime;
// Encoded simulation data, dependent on sim config attrs. `outputPoints` is dependent on this.
MObject FlowFinityNode::simulationData;
MObject FlowFinityNode::currentTime;
MObject FlowFinityNode::outputPoints;

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

  MFnNumericAttribute nAttr;
  MFnTypedAttribute tAttr;
  MFnUnitAttribute uniAttr;
  MFnMatrixAttribute mAttr;

  // These would be inputs from the GUI
  glm::vec2 translation = glm::vec2(3, 3);
  glm::vec2 scale = glm::vec2(2, 2);

  glm::vec2 translation2 = glm::vec2(7, 7);
  glm::vec2 scale2 = glm::vec2(1, 2);

  // --------- --------- SETUP CROWDSIM CONFIG ATTRIBUTES --------- ---------

  agentMaxSpeed = nAttr.create("agentMaxSpeed", "ams", MFnNumericData::kDouble, 1.0, &stat);
  statCheck(stat, "failed to create agentMaxSpeed");
  nAttr.setWritable(true);
  nAttr.setStorable(true);
  nAttr.setKeyable(true);

  agentAcceleration =
      nAttr.create("agentAcceleration", "aac", MFnNumericData::kDouble, 20.0, &stat);
  statCheck(stat, "failed to create agentAcceleration");
  nAttr.setWritable(true);
  nAttr.setStorable(true);
  nAttr.setKeyable(true);

  agentDrag = nAttr.create("agentDrag", "adr", MFnNumericData::kDouble, 12., &stat);
  statCheck(stat, "failed to create agentDrag");
  nAttr.setWritable(true);
  nAttr.setStorable(true);
  nAttr.setKeyable(true);

  agentRadius = nAttr.create("agentRadius", "ard", MFnNumericData::kDouble, 0.25, &stat);
  statCheck(stat, "failed to create agentRadius");
  nAttr.setWritable(true);
  nAttr.setStorable(true);
  nAttr.setKeyable(true);

  agentAggressiveness =
      nAttr.create("agentAggressiveness", "aag", MFnNumericData::kDouble, 0.5, &stat);
  statCheck(stat, "failed to create agentAggressiveness");
  nAttr.setWritable(true);
  nAttr.setStorable(true);
  nAttr.setKeyable(true);

  spawnRate = nAttr.create("spawnRate", "spr", MFnNumericData::kDouble, .5, &stat);
  statCheck(stat, "failed to create spawnRate");
  nAttr.setWritable(true);
  nAttr.setStorable(true);
  nAttr.setKeyable(true);

  maxAgents = nAttr.create("maxAgents", "max", MFnNumericData::kInt, 100, &stat);
  statCheck(stat, "failed to create maxAgents");
  nAttr.setWritable(true);
  nAttr.setStorable(true);
  nAttr.setKeyable(true);

  obstacleTransforms = mAttr.create("obstacleTransforms", "ot", MFnMatrixAttribute::kDouble, &stat);
  statCheck(stat, "failed to create obstacleTransforms");
  mAttr.setArray(true);
  mAttr.setUsesArrayDataBuilder(true);
  mAttr.setWritable(true);

  inOutFlows = mAttr.create("inOutFlows", "io", MFnMatrixAttribute::kDouble, &stat);
  statCheck(stat, "failed to create inOutFlows");
  mAttr.setArray(true);
  mAttr.setUsesArrayDataBuilder(true);
  mAttr.setWritable(true);

  startTime = uniAttr.create("startTime", "st", MFnUnitAttribute::kTime, 0.0, &stat);
  statCheck(stat, "failed to create startTime");
  uniAttr.setWritable(true);
  uniAttr.setStorable(true);

  endTime = uniAttr.create("endTime", "et", MFnUnitAttribute::kTime, 240.0, &stat);
  statCheck(stat, "failed to create endTime");
  uniAttr.setWritable(true);
  uniAttr.setStorable(true);

  addAttribute(agentMaxSpeed);
  addAttribute(agentAcceleration);
  addAttribute(agentDrag);
  addAttribute(agentRadius);
  addAttribute(agentAggressiveness);
  addAttribute(spawnRate);
  addAttribute(maxAgents);
  addAttribute(obstacleTransforms);
  addAttribute(inOutFlows);
  addAttribute(startTime);
  addAttribute(endTime);

  // --------- --------- SETUP SIM CACHE ATTRIBUTES --------- ---------

  simulationData = tAttr.create("simulationData", "sd", MFnData::kString, MObject::kNullObj, &stat);
  statCheck(stat, "failed to create simulationData");
  tAttr.setWritable(false);
  tAttr.setStorable(true);
  tAttr.setHidden(true);

  currentTime = uniAttr.create("currentTime", "ct", MFnUnitAttribute::kTime, 0.0, &stat);
  statCheck(stat, "failed to create currentTime");

  addAttribute(simulationData);
  addAttribute(currentTime);

  // --------- --------- SETUP OUTPUT DATA ATTRIBUTE --------- ---------

  outputPoints =
      tAttr.create("outputPoints", "od", MFnData::kDynArrayAttrs, MObject::kNullObj, &stat);
  statCheck(stat, "failed to create outputPoints");
  tAttr.setWritable(false);
  tAttr.setReadable(true);

  addAttribute(outputPoints);

  // --------- --------- SETUP ATTRIBUTE EFFECTS --------- ---------

  // recompute sim when any of the following attributes change
  attributeAffects(agentMaxSpeed, simulationData);
  attributeAffects(agentAcceleration, simulationData);
  attributeAffects(agentDrag, simulationData);
  attributeAffects(agentRadius, simulationData);
  attributeAffects(agentAggressiveness, simulationData);
  attributeAffects(spawnRate, simulationData);
  attributeAffects(maxAgents, simulationData);
  attributeAffects(obstacleTransforms, simulationData);
  attributeAffects(inOutFlows, simulationData);
  attributeAffects(startTime, simulationData);
  attributeAffects(endTime, simulationData);

  // outputPoints just fetches a snapshot of simulationData
  attributeAffects(simulationData, outputPoints);
  attributeAffects(currentTime, outputPoints);

  return MStatus::kSuccess;
}

MStatus FlowFinityNode::compute(const MPlug& plug, MDataBlock& data)
{
  MStatus stat;
  // Run through simulation if simulationData is dirty
  if (plug == simulationData) {

    // Get the start and end times
    MDataHandle startTimeHandle = data.inputValue(startTime);
    statCheck(stat, "failed to get startTimeHandle");
    MDataHandle endTimeHandle = data.inputValue(endTime);
    statCheck(stat, "failed to get endTimeHandle");

    // Get number of frames needed and deltaTime
    MTime startTime = startTimeHandle.asTime();
    MTime endTime = endTimeHandle.asTime();
    int numFrames = (int)(endTime.value() - startTime.value());
    float deltaTime = 1.f / 24.f;

    MDataHandle agentMaxSpeedHandle = data.inputValue(agentMaxSpeed);
    statCheck(stat, "failed to get agentMaxSpeedHandle");
    MDataHandle agentAccelerationHandle = data.inputValue(agentAcceleration);
    statCheck(stat, "failed to get agentAccelerationHandle");
    MDataHandle agentDragHandle = data.inputValue(agentDrag);
    statCheck(stat, "failed to get agentDragHandle");
    MDataHandle agentRadiusHandle = data.inputValue(agentRadius);
    statCheck(stat, "failed to get agentRadiusHandle");
    MDataHandle agentAggressivenessHandle = data.inputValue(agentAggressiveness);
    statCheck(stat, "failed to get agentAggressivenessHandle");
    MArrayDataHandle obstacleTransformsHandle = data.inputValue(obstacleTransforms);
    statCheck(stat, "failed to get obstacleTransformsHandle");
    MArrayDataHandle inOutFlowsHandle = data.inputValue(inOutFlows);
    statCheck(stat, "failed to get inOutFlowsHandle");
    MDataHandle spawnRateHandle = data.inputValue(spawnRate);
    statCheck(stat, "failed to get spawnRateHandle");
    MDataHandle maxAgentsHandle = data.inputValue(maxAgents);
    statCheck(stat, "failed to get maxAgentsHandle");

    // Create a Crowd Sim instance for simulation
    CrowdSim crowdSim;

    // Set config values from attributes
    crowdSim.m_config.maxSpeed = agentMaxSpeedHandle.asDouble();
    crowdSim.m_config.acceleration = agentAccelerationHandle.asDouble();
    crowdSim.m_config.drag = agentDragHandle.asDouble();
    crowdSim.m_config.radius = agentRadiusHandle.asDouble();
    crowdSim.m_config.aggressiveness = agentAggressivenessHandle.asDouble();
    crowdSim.m_config.spawnRate = spawnRateHandle.asDouble();
    crowdSim.m_config.maxAgents = maxAgentsHandle.asInt();

    // Read in/out flows from attributes
    for (unsigned int i = 0; i < inOutFlowsHandle.elementCount(); i += 2) {
      // get inflow transform
      inOutFlowsHandle.jumpToElement(i);
      MDataHandle inOutFlowHandle = inOutFlowsHandle.inputValue();
      MMatrix inOutFlowMatrix = inOutFlowHandle.asMatrix();
      MTransformationMatrix inFlowTrans(inOutFlowMatrix);

      // get outflow transform
      inOutFlowsHandle.jumpToElement(i + 1);
      inOutFlowHandle = inOutFlowsHandle.inputValue();
      inOutFlowMatrix = inOutFlowHandle.asMatrix();
      MTransformationMatrix outFlowTrans(inOutFlowMatrix);

      crowdSim.m_config.inOutFlows.push_back(
          std::make_pair(glm::vec2(inFlowTrans.getTranslation(MSpace::kWorld).x,
                                   inFlowTrans.getTranslation(MSpace::kWorld).z),
                         glm::vec2(outFlowTrans.getTranslation(MSpace::kWorld).x,
                                   outFlowTrans.getTranslation(MSpace::kWorld).z)));
    }

    // Create visgraph
    VisibilityGraph visGraph;

    // Read obstacles from attributes
    for (unsigned int i = 0; i < obstacleTransformsHandle.elementCount(); i++) {
      obstacleTransformsHandle.jumpToElement(i);
      MDataHandle obstacleTransformHandle = obstacleTransformsHandle.inputValue();
      MMatrix obstacleTransformMatrix = obstacleTransformHandle.asMatrix();
      MTransformationMatrix obstacleTrans(obstacleTransformMatrix);

      MVector translation = obstacleTrans.getTranslation(MSpace::kWorld);
      double scale[3];
      obstacleTrans.getScale(scale, MSpace::kWorld);
      auto rotation = obstacleTrans.eulerRotation();

      visGraph.addCubeObstacle(glm::vec2(translation.x, translation.z),
                               glm::vec2(scale[0], scale[2]), rotation.y);
    }

    // Run sim and encode the data in a string!
    auto encodedSim = FFEncoding::encodeSimulation(crowdSim, numFrames, deltaTime, visGraph);

    // Save string into simulationData attribute
    MDataHandle simDataHandle = data.outputValue(simulationData, &stat);
    statCheck(stat, "failed to get simDataHandle");
    simDataHandle.set(MFnStringData().create(encodedSim.c_str()));

    data.setClean(plug);

    return stat;
  }

  // If outputPoints is dirty, just fetch from simulationData
  if (plug == outputPoints) {

    // Get attribute handles

    MDataHandle startTimeHandle = data.inputValue(startTime, &stat);
    statCheck(stat, "failed to get startTimeHandle");
    MDataHandle currentTimeHandle = data.inputValue(currentTime, &stat);
    statCheck(stat, "failed to get currentTimeHandle");
    MDataHandle simDataHandle = data.inputValue(simulationData, &stat);
    statCheck(stat, "failed to get simDataHandle");
    MDataHandle outputPointsHandle = data.outputValue(outputPoints, &stat);
    statCheck(stat, "failed to get outputPointsHandle");

    // Get current time
    MTime currTime = data.inputValue(currentTime).asTime();

    // Read sim data from simulationData attribute and decode it
    auto encodedSim = std::string(simDataHandle.asString().asChar());
    auto simulation = FFEncoding::decodeSimulation(encodedSim);

    // Create a new nObjectData to store the output points
    // https://help.autodesk.com/cloudhelp/2023/ENU/Maya-Tech-Docs/Nodes/instancer.html#attrinputPoints
    MFnArrayAttrsData nObjData;
    auto nObj = nObjData.create();

    auto frame = simulation.at(currTime.value() - startTimeHandle.asTime().value());

    auto posArray = nObjData.vectorArray("position");
    auto rotArray = nObjData.vectorArray("aimDirection");
    auto rotType = nObjData.intArray("rotationType");
    // TODO: maybe we need ID array?

    for (const auto& agent : frame) {
      posArray.append(MVector(agent.first.x, 0, agent.first.y));
      rotArray.append(MVector(agent.second.x, 0, agent.second.y));
      rotType.append(1);
    }

    // Send nObjectData to the outputPoints attribute
    outputPointsHandle.setMObject(nObj);

    data.setClean(plug);
  }

  return MS::kSuccess;
}
