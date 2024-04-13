#include "FlowFinityCmd.h"

#include <flowfinity.h>

#include <maya/MGlobal.h>

#include <list>

FlowFinityCmd::FlowFinityCmd() : MPxCommand()
{
}

FlowFinityCmd::~FlowFinityCmd() 
{
}

MStatus FlowFinityCmd::doIt( const MArgList& args )
{
	// message in Maya output window
    cout<<"Implement Me!"<<endl;
	std::cout.flush();

	// message in scriptor editor
	MGlobal::displayInfo("Implement Me!");

    return MStatus::kSuccess;
}

