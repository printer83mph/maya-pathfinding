#pragma once

#include <maya/MPxNode.h>

class FlowFinityNode : public MPxNode
{
public:
  MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

  static void* creator();

  static MStatus initialize();
  static const MTypeId id;

  static const char* className();

  // per-agent config params
  static MObject agentMaxSpeed;
  static MObject agentAcceleration;
  static MObject agentDrag;
  static MObject agentRadius;
  static MObject agentAggressiveness;
  static MObject spawnRate;
  static MObject maxAgents;
  static MObject obstacleTransforms;
  static MObject inOutFlows;
  static MObject startTime;
  static MObject endTime;

  // data storage + time for fetching agent data
  static MObject simulationData;
  static MObject currentTime;

  // output data for instancer node
  static MObject outputPoints;
};
