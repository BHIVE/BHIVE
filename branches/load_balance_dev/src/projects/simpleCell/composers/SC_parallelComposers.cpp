/*
 * SC_parallelComposers.cpp
 *
 *  Created on: Feb 23, 2010
 *      Author: msneddon
 */

#include "SC_parallelComposers.hh"
#include "../simpleCell.hh"
#include "../../../parallel/parallel.hh"

#include <vector>
#include <string>
#include <fstream> // for ifstream, ofstream, ios_base
#include <iostream>// for cin, cout, endl
#include <exception>
#include <stdexcept>

using namespace std;
using namespace Hive;
using namespace SimpleCell;




SimpleCellDefaultParallelComposer::SimpleCellDefaultParallelComposer(string inputFileName, string outputFileName, double eqTime) {

}

SimpleCellDefaultParallelComposer::~SimpleCellDefaultParallelComposer()
{

}



void SimpleCellDefaultParallelComposer::setupSimulation()
{
}




void SimpleCellDefaultParallelComposer::parseInputFileBuffer(vector <string> &buffer)
{
}




void SimpleCellDefaultParallelComposer::outputNow(double currentTime)
{
	
}

void SimpleCellDefaultParallelComposer::updateAllChemicalConc(double c)
{
	
}


// Not called for the SimpleCellDefaultComposer
void SimpleCellDefaultParallelComposer::initializeAgentFactories() { }


void SimpleCellDefaultParallelComposer::setupAgentHierarchy()
{
	

}


Agent* SimpleCellDefaultParallelComposer::createAgent(AgentFactory& af) {
	return NULL;
}



void SimpleCellDefaultParallelComposer::addSerialCommunicator()
{

}

