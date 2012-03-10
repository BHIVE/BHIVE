#include "nfsimagentfactory.hh"

using namespace std;
using namespace Hive;
using namespace nfsimparascan;

NFsimAgentFactory::NFsimAgentFactory() {
	cerr << "# hello, this is the NFsimAgentFactory" << endl;
	this->filename = "";
	this->blockSameComplexBinding = false;
	this->cb = false;
	this->turnOnComplexBookkeeping = false;
	this->globalMoleculeLimit = 100000;
	this->verbose = false;
}

NFsimAgentFactory::~NFsimAgentFactory() { }

Agent* NFsimAgentFactory::createAgent() {
	// create agent and get his database
	Agent *bond = new Agent();
	Hive::Database *db = bond->getDatabase();

	// create the NFsim System
	if (turnOnComplexBookkeeping || blockSameComplexBinding)
		this->cb = true;
	int suggestedTraveralLimit = ReactionClass::NO_LIMIT;
	System *s = NFinput::initializeFromXML(filename, cb, globalMoleculeLimit, verbose, suggestedTraveralLimit);

	NFsimData *nfsd = new NFsimData("nfsim_system");
	nfsd->setNFsimSystem(s);
	db->addData(nfsd);

	// now add the system to the database of the agent.
	// for this the system has to be a part of the
	return bond;
}

Agent* NFsimAgentFactory::createAgent(Action **as) { }

Agent* NFsimAgentFactory::duplicateAgent(Agent *ag) { }

void NFsimAgentFactory::finalise() { }

		// methods to bring private members to life
void NFsimAgentFactory::setXMLFilename(string s) {
	this->filename = s;
}

void NFsimAgentFactory::setBlockSameComplexBinding(bool b) {
	this->blockSameComplexBinding = b;
}

void NFsimAgentFactory::setTurnOnComplexBookkeeping(bool b) {
	this->turnOnComplexBookkeeping = b;
}

void NFsimAgentFactory::setGlobalMoleculeLimit(int i) {
	this->globalMoleculeLimit = i;
}

void NFsimAgentFactory::addActionsToAgentsActionSet(Agent *ag) {

}

void NFsimAgentFactory::addSimulatorToAgent(Agent *ag) {

}

void NFsimAgentFactory::addMessageGeneratorsToAgent(Agent *ag) {

}
