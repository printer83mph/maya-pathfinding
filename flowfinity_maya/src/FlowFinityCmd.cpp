#include "FlowFinityCmd.h"
#include "FlowFinityNode.h"
#include <flowfinity.h>

#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>

FlowFinityCmd::FlowFinityCmd() : MPxCommand() {}

FlowFinityCmd::~FlowFinityCmd() {}

MStatus FlowFinityCmd::doIt(const MArgList& args)
{
  /* The Gospel: (we need to turn this to c++ somehow)

  disconnectAttr nParticleShape1.currentState nucleus1.inputActive[0];
  disconnectAttr nParticleShape1.startState nucleus1.inputActiveStart[0];
  disconnectAttr nucleus1.outputObjects[0] nParticleShape1.nextState;

  connectAttr FlowFinityNode1.nextState[0] nParticleShape1.nextState;
  connectAttr nParticleShape1.currentState FlowFinityNode1.currentState[0];
  connectAttr nParticleShape1.startState FlowFinityNode1.startState[0];
  connectAttr time1.outTime FlowFinityNode1.currentTime;

  */
  // Get the selected objects
  MSelectionList selection;
  MGlobal::getActiveSelectionList(selection);

  // Check if the selected object is an MnParticle
  MObject selectedObject;
  selection.getDependNode(0, selectedObject);
  if (selectedObject.hasFn(MFn::kNParticle)) {
    MGlobal::displayInfo("Selected");
  } else {
    MGlobal::displayInfo(selectedObject.apiTypeStr());
  }

  return MStatus::kSuccess;
}
