/*
 * worldagentfactory.cpp
 *
 *  Created on: Dec 8, 2009
 *      Author: jentsch
 */

#include "worldagentfactory.hh"
#include "transwellgeometryconstructor.hh"

using namespace Hive;
using namespace Transwell;

WorldAgentFactory::WorldAgentFactory() {
	cerr << "# hello, i am the transwell's worldagentfactory" << endl;
	numbercells = 0;
}

WorldAgentFactory::WorldAgentFactory(InputSystemReader *isr):AgentFactory(isr) {
	this->numbercells = 0;
}

WorldAgentFactory::~WorldAgentFactory() {
	cerr << "# deleteing worldfactory" << endl;
	delete tgc;
	cerr << "# done deleting the worldagentfactory" << endl;

}

Agent* WorldAgentFactory::createAgent() {
	// create a new agent
	Agent *world = new Agent();

	// get the database of the agent
	Hive::Database *db = world->getDatabase();

	// now we add data to the world agent's data-base
	// setup transwellgeometryconstructor to construct the grid of the transwell
	tgc = new TranswellGeometryConstructor();

	// provide geometrical details
	tgc->setRadii(1.5, 2.5);
	tgc->setHeights(1, 3);
	tgc->setResolution(1);

	// construct the geometry the geometry
	// grid is derived from data, hence it has a name
	Grid *g = tgc->returnGrid("world_agent_grid");

	// set concentration to some random value
	for (int i=0; i<g->getNumberOfVoxels(); i++) {
		g->getVoxel(i)->setNumberSpecies(1);
		// NOTE THAT WE SET THE CONCENTRATIONS HERE
		//	g->getVoxel(i)->setConcentration(0, Util::RANDOM(1));
	}

	// generate output for manually validating whether everything is okay.
	//	g->outputGridForValidation();

	// remove the transwellgeometryconstructor, it is no longer needed
	//	delete tgc;

	// add information about the diffusing species to the grid
	//	g->addSpecies("IL-2", 0.1);
	// FOR VALIDATING THE SYSTEM. SIMULATING THE SYSTEM IN A WELL STIRRED MEDIUM
	g->addSpecies("IL2",0.0);

	// add data to the data-base
	db->addData(g->getName(),g);

	// now we need to add a data structure in which we can store the voxel positions of the
	// cells
	TVectorData<int> *cell_voxel_position = new TVectorData<int> ("particles_voxel_positions", "tvectordata_int");
	cell_voxel_position->reserveSize(numbercells);
	db->addData(cell_voxel_position->getName(), cell_voxel_position);


	// add simulator to world agent
	this->addSimulatorToAgent(world);
	this->addActionsToAgentsActionSet(world);
	this->addMessageGeneratorsToAgent(world);

	return world;
}

Agent* WorldAgentFactory::createAgent(Action **as) {
	Agent *world = new Agent();

	cerr << "# Transwell::WorldAgentFactory::createAgent(Action **as) only creates dummyagent with no functionality" << endl;

	return world;
}

Agent* WorldAgentFactory::duplicateAgent(Agent *ag) {
	return NULL;
}

void WorldAgentFactory::finalise() { }

void WorldAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	Action *a1 = new UpdateWorldSeenIL2Action();
	a1->setAgent(ag);

	Action *a2 = new InitWorldCellPositionAction();
	a2->setAgent(ag);

	ag->addAction(a1);
	ag->addAction(a2);
}

void WorldAgentFactory::addMessageGeneratorsToAgent(Agent *ag) {
	// there is only one message-generator associated to the world
	WorldNotifyIL2ToCellMessageGenerator *wnitcmg = new WorldNotifyIL2ToCellMessageGenerator(ag);
	ag->addMessageGenerator(wnitcmg);
}

void WorldAgentFactory::addSimulatorToAgent(Agent *ag) {
	// here we add the crank-nicolson simulator to the agent
	CrankNicolson *crank = new CrankNicolson();

	// for testing
	//	crank->testing();
	//	exit(1);

	crank->setTimeStep(1.0);
	crank->setAgent(ag);

	ag->addSimulator(crank);
	cerr << endl << endl;
	cerr << "# added CrankNicolson simulator to agent" << endl;
	//	crank->generateOutputForValidation();
}

void WorldAgentFactory::setNumberCells(int i) {
	this->numbercells = i;
}
