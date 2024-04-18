#pragma once
#include "FlowFinityCmd.h"
#include "FlowFinityNode.h"

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include <maya/MStatus.h>

MStatus initializePlugin(MObject obj)
{
  MStatus status = MStatus::kSuccess;
  MFnPlugin plugin(obj, "FlowFinity", "1.0", "Any");

  status = plugin.registerNode("FlowFinityNode", FlowFinityNode::id, FlowFinityNode::creator,
                               FlowFinityNode::initialize);
  if (!status) {
    status.perror("registerNode");
    return status;
  }

  status = plugin.registerCommand("FlowFinityCmd", FlowFinityCmd::creator);
  if (!status) {
    status.perror("registerCommand");
    return status;
  }
  // Create Menu
  MString melCommand = "if (`menu -exists FlowFinityMenu` == 0) {\n\tmenu -label \"FlowFinity\" "
                       "-parent $gMainWindow FlowFinityMenu;\n}";
  melCommand +=
      "\nmenuItem -label \"FlowFinityGUI\" -parent FlowFinityMenu -command \"FlowFinityCmd\";";

  status = MGlobal::executeCommand(melCommand, true);

  return status;
}

MStatus uninitializePlugin(MObject obj)
{
  MStatus status = MStatus::kSuccess;
  MFnPlugin plugin(obj);

  status = plugin.deregisterCommand("FlowFinityCmd");
  if (!status) {
    status.perror("deregisterCommand");
    return status;
  }

  status = plugin.deregisterNode(FlowFinityNode::id);
  if (!status) {
    status.perror("deregisterNode");
    return status;
  }

  // Remove Menu
  MString melCommand = "if (`menu -exists FlowFinityMenu`) deleteUI FlowFinityMenu;";
  MGlobal::executeCommand(melCommand, true);

  return status;
}
