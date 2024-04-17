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

  static MObject startState;
  static MObject currentState;
  static MObject nextState;
  static MObject currentTime;
};