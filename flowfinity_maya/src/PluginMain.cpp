#include "FlowFinityCmd.h"

#include <maya/MArgList.h>
#include <maya/MDGModifier.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MIOStream.h>
#include <maya/MPlugArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MPxCommand.h>
#include <maya/MSimple.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MVector.h>

#include <list>

MStatus initializePlugin(MObject obj)
{
  MStatus status = MStatus::kSuccess;
  MFnPlugin plugin(obj, "FlowFinity", "1.0", "Any");

  // Register Command
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

  return status;
}
