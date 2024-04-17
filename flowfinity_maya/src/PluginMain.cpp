#pragma once
#include "FlowFinityCmd.h"
#include "FlowFinityNode.h"

#include <maya/MFnPlugin.h>
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

  return status;
}
