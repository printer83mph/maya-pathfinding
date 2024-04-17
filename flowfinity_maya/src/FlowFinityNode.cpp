#include "FlowFinityNode.h"

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

    MFnNObjectData nData(inputData);
    MnParticle* nObj = NULL;
    nData.getObjectPtr(nObj);

    // Get the particle positions
    MFloatPointArray points;
    nObj->getPositions(points);

    // Get the particle velocities
    MFloatPointArray velocities;
    nObj->getVelocities(velocities);

    // Store the current target using the bounce, friction, and thickness attributes
    MFloatArray pathx;
    nObj->getBounce(pathx);
    MFloatArray pathy;
    nObj->getFriction(pathy);
    MFloatArray pathz;
    nObj->getThickness(pathz);

    unsigned int ii;
    for (ii = 0; ii < points.length(); ii++) {
      points[ii].y = (float)sin(points[ii].x + currTime.value() * 4.0f * (3.1415f / 180.0f));
    }
    nObj->setPositions(points);

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