#pragma once

#include <maya/MPxCommand.h>

class FlowFinityCmd : public MPxCommand
{
public:
  FlowFinityCmd();
  virtual ~FlowFinityCmd();
  static void* creator() { return new FlowFinityCmd(); }
  MStatus doIt(const MArgList& args);
};
