#pragma once

#include <maya/MPxNode.h>

#include "flowfinity/navigation/visibilitygraph.h"

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

  // spawning params
  static MObject spawnRate;
  static MObject maxAgents;

  // frame logic
  static MObject startState;
  static MObject currentState;
  static MObject nextState;
  static MObject currentTime;

  static VisibilityGraph m_visibilityGraph;
};
