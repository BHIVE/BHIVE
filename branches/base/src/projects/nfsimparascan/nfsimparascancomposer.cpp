#include "nfsimparascancomposer.hh"

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
using namespace Hive;
using namespace nfsimparascan;


NFsimParaScanComposer::NFsimParaScanComposer() {
	cerr << "# you are calling me without a command line. i can not tolerate that." << endl;
	exit(1);
}

NFsimParaScanComposer::NFsimParaScanComposer(int argn, char *argv[]) {
	cerr << "# hello, i am the NFsimParaScanComposer" << endl;

	/// read the commandline: not sure what needs to be read from there
	int index = 1;
	string first, second;
	while (index < argn) {
		first = argv[index];
		index++;
		if (index == argn) {
			cerr << "wrong command-line" << endl;
			exit(1);
		}
		second = argv[index];
		commandline[first] = second;
		index++;
	}




}

NFsimParaScanComposer::~NFsimParaScanComposer() { }

void NFsimParaScanComposer::setupSimulation() {
	this->initializeAgentFactories();
}

void NFsimParaScanComposer::initializeAgentFactories() {
	WorldAgentFactory *waf = new WorldAgentFactory();

}

void NFsimParaScanComposer::setupAgentHierarchy() { }

void NFsimParaScanComposer::addSerialCommunicator() { }

// do we need something like this?
void NFsimParaScanComposer::addParallelCommunicator() { }

Agent* NFsimParaScanComposer::createAgent(AgentFactory& af) {

}
