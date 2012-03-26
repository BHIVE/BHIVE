/*
 * worldagentfactory.cpp
 *
 *  Created on: Dec 8, 2009
 *      Author: jentsch
 */

#include "worldagentfactory.hh"
#include "transwellgeometryconstructor.hh"
#include "singlewellgeometryconstructor.hh"

using namespace Hive;
using namespace Transwell;

WorldAgentFactory::WorldAgentFactory() {
	cerr << "# hello, i am the transwell's worldagentfactory" << endl;
	numbercells = 0;
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


	/// <----------------------- BEGIN TEST OF THE HYBRID SIMULATION ROUTINE
/*
	tgc->setRadii(1.5,2.5);
	tgc->setHeights(1,3);
	tgc->setResolution(1);

	Grid *g = tgc->returnGrid("world_agent_grid");

	for (int i=0; i<g->getNumberOfVoxels(); i++)
		g->getVoxel(i)->setNumberSpecies(1);
	g->addSpecies("IL2",0.0);
*/
	// <------------------------- END TEST OF HYBRID SIMULATION MEHTOD


	// <----------------------- BEGIN TEST IN CUBE GEOMETRY FOR FIGURING OUT THE OPTIMAL DISCRETISATION
	/*
	CubeGeometryConstructor *cgc = new CubeGeometryConstructor();
	cgc->setLength(1.0);
	cgc->setResolution(0.2);

	Grid *g = cgc->returnGrid("world_agent_grid");

	for (int i=0; i<g->getNumberOfVoxels(); i++)
		g->getVoxel(i)->setNumberSpecies(1);
	g->addSpecies("IL-2",0.0008);
	*/
	// <------------------------ END TEST IN CUBE GEOMETRY.



	// <------------------------------------- BEGIN DIFFUSION TEST IN TRANSWELL GEOMETRY
	/*
	// ALL IL2 INITIALLY SITUATED IN UPPER TRANSWELL
	// NOTE THAT WE ALSO NEED TO ADD THE CORRECT MESSAGE GENERATOR FOR SENDING OUTPUT INFORMATION TO THE SPECIAL AGENT
	// ONE MOST LIKELY ALSO WILL WANT TO USE A LARGER TIME STEP (SEE ADDSIMULATORTOAGENT METHOD)
	// YOU WILL ALSO HAVE TO CHECK THE MAIN FILE AND SPECIALAGENTFACTORY!
	// provide geometrical details
	tgc->setRadii(3.5, 8.25);
	tgc->setHeights(2.5, 7.5);
	tgc->setResolution(0.25);
	tgc->setThickness(1.25);

	// construct the geometry the geometry
	// grid is derived from data, hence it has a name
	Grid *g = tgc->returnGrid("world_agent_grid");

	//	cerr << g->getNumberOfVoxels() << endl;
	//	exit(1);
	int small=0; int large=0; int bla=0;
	for (int i=0; i<g->getNumberOfVoxels(); i++) {
		bla = tgc->getVoxelDomain(i);
		if (bla == 1)
			small += 1;
		else if (bla == 0)
			large += 1;
	}
	cerr << "# number voxels in upper well " <<  small << " number voxels in large well "
		 << large << " " << g->getNumberOfVoxels() << endl;
	//	exit(1);

	// set initial il-2 profile
	TVectorData<int> *domain_lookup = new TVectorData<int> ("domain_lookup", "tvectordata_int");
	domain_lookup->reserveSize(g->getNumberOfVoxels());
	for (int i=0; i<g->getNumberOfVoxels(); i++) {
		g->getVoxel(i)->setNumberSpecies(1);
		// NOTE THAT WE SET THE CONCENTRATIONS HERE
		if (tgc->getVoxelDomain(i) == 1) {
			// with the chosen length scale, this particle number corresponds to 100 pM
			g->getVoxel(i)->setConcentration(0, 940938);
			domain_lookup->at(i) = 1;
		} else if (tgc->getVoxelDomain(i) == 0)
			domain_lookup->at(i) = 0;
  		//	g->getVoxel(i)->setConcentration(0, Util::RANDOM(1));
	}
	db->addData(domain_lookup->getName(), domain_lookup);

	// generate output for manually validating whether everything is okay.
	//	g->outputGridForValidation();

	// remove the transwellgeometryconstructor, it is no longer needed
	//	delete tgc;

	// add information about the diffusing species to the grid
	// set the diffusion coefficient of il2 to 2e-4 mm**2/s
	g->addSpecies("IL2",0.0002);
	db->addData(g->getName(),g);
	*/
	// <-------------- END DIFFUSION TEST SIMULATION IN TRANSWELL

	
	// <------------------ BEGIN GEOMETRY FOR SIMULATION WITH 50,000 TCELLS
	// note that we have to chose the inner radius one stepwidht larger in order to
	// get the correct length. this is due to the way we assign the domains to the
	// the voxels.
	// sizes of the rescaled version
	tgc->setRadii(0.8, 2.3);
	tgc->setHeights(0.6,1.85);
	// hier geaendert auf 0.1
	tgc->setResolution(0.05);
	tgc->setThickness(0.3);

	// sizes of the real thing
	//	tgc->setRadii(3.6, 9.2);
	//	tgc->setHeights(2.4, 7.4);
	//	tgc->setResolution(0.2);
	//	tgc->setThickness(1.2);

	// construct the geometry the geometry
	// grid is derived from data, hence it has a name
	Grid *g = tgc->returnGrid("world_agent_grid");

	//	cerr << g->getNumberOfVoxels() << endl;
	//	exit(1);
	TVectorData<int> *domain_lookup = new TVectorData<int> ("domain_lookup", "tvectordata_int");
	domain_lookup->reserveSize(g->getNumberOfVoxels());
	int small=0; int large=0; int bla=0;
	for (int i=0; i<g->getNumberOfVoxels(); i++) {
		g->getVoxel(i)->setNumberSpecies(1);
		bla = tgc->getVoxelDomain(i);
		if (bla == 1) {
			domain_lookup->at(i) = 1;
			//	g->getVoxel(i)->setConcentration(0, 481760);
			small += 1;
		} else if (bla == 0) {
	//		g->getVoxel(i)->setConcentration(0,10000);
			domain_lookup->at(i) = 0;
			large += 1;
		}
	}
	g->addSpecies("IL2",1.25e-5); // diffusion coefficient of the rescaled system (org: 0.0002)
	//	g->addSpecies("IL2", 0.0008); // this is four times the D coeff. 
	db->addData(domain_lookup->getName(), domain_lookup);
	cerr << small << " " << large << endl;
	// add grid data to the data-base
	db->addData(g->getName(),g);
	// <-------------------- END GEOMETRY FOR SIMULATION WITH 50,000 TCELLS

	/*

	SinglewellGeometryConstructor *sgc = new SinglewellGeometryConstructor();
	sgc->setRadius(2.3);
	sgc->setHeight(1);
	sgc->setResolution(0.1);
	
	Grid *g = sgc->returnGrid("world_agent_grid");
	TVectorData<int> *domain_lookup = new TVectorData<int> ("domain_lookup", "tvectordata_int");
	domain_lookup->reserveSize(g->getNumberOfVoxels());
	for (int i=0; i<g->getNumberOfVoxels(); i++) {
	  g->getVoxel(i)->setNumberSpecies(1);
	  domain_lookup->at(i) = 0;
	}
	g->addSpecies("IL2",1.25e-5);
	db->addData(domain_lookup->getName(), domain_lookup);
	db->addData(g->getName(),g);

	delete sgc;
	*/

	// now we need to add a data structure in which we can store the voxel positions of the
	// cells
	TVectorData<int> *cell_voxel_position = new TVectorData<int> ("particles_voxel_positions", "tvectordata_int");
	cell_voxel_position->reserveSize(numbercells);
	db->addData(cell_voxel_position->getName(), cell_voxel_position);

	// time
	DoubleData *time = new DoubleData("time", 0.0);
	db->addData("time", time);

	// time until which it has to propagate
	DoubleData *propagate_until = new DoubleData("propagate_until", 0.0);
	db->addData("propagate_until", propagate_until);

	//data for the output ...
	IntegerData *counter = new IntegerData("counter", 216);
	IntegerData *number_between_outputs = new IntegerData("number_between_outputs", 216);
	db->addData("counter", counter);
	db->addData("number_between_outputs", number_between_outputs);

	IntegerData *cellsadded = new IntegerData("cellsadded", 0);
	db->addData("cellsadded",cellsadded);

	IntegerData *tregsadded = new IntegerData("tregsadded", 0);
	db->addData("tregsadded", tregsadded);

	IntegerData *counter_for_profile_output = new IntegerData("counter_for_profile_output",216);
	IntegerData *number_between_profile_output = new IntegerData("number_between_profile_output",216);
	db->addData("counter_for_profile_output",counter_for_profile_output);
	db->addData("number_between_profile_output", number_between_profile_output);

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

	Action *a3 = new InitWorldTregPositionAction();
	a3->setAgent(ag);

	ag->addAction(a1);
	ag->addAction(a2);
	ag->addAction(a3);
}

void WorldAgentFactory::addMessageGeneratorsToAgent(Agent *ag) {
	// there is only one message-generator associated to the world
	WorldNotifyIL2ToCellMessageGenerator *wnitcmg = new WorldNotifyIL2ToCellMessageGenerator(ag);
	ag->addMessageGenerator(wnitcmg);

	// THIS MESSAGENERATOR IS NEEDED WHEN SIMULATING DIFFUSION OF IL2!!!
	WorldTellIL2ToSpecialAgentMessageGenerator *wtil2tsamg = new WorldTellIL2ToSpecialAgentMessageGenerator(ag);
	ag->addMessageGenerator(wtil2tsamg);

	WorldTellIL2ProfileToSpecialAgentMessageGenerator *wtil2ptsamg =
		new WorldTellIL2ProfileToSpecialAgentMessageGenerator(ag);
	//	ag->addMessageGenerator(wtil2ptsamg);
}

void WorldAgentFactory::addSimulatorToAgent(Agent *ag) {
	// here we add the crank-nicolson simulator to the agent
	CrankNicolson *crank = new CrankNicolson();

	// for testing
	//	crank->testing2();
	//	exit(1);

	crank->setTimeStep(1.0);
	crank->setAgent(ag);
	
	ag->addSimulator(crank);
	cerr << "# added CrankNicolson simulator to agent" << endl;

	// alternatively we can use the runge kutta integrator
	//	RKDiff *rkd = new RKDiff();
	//	rkd->testing();

	// adding mixing simulator to the agent
	Mixer *mixer = new Mixer();
	// do the mixing every 10h. 
	mixer->setTimeStep(10*3600);
	mixer->setAgent(ag);

	//	ag->addSimulator(mixer);

	cerr << "# added Mixer to agent" << endl;

	//	exit(1);

}

void WorldAgentFactory::setNumberCells(int i) {
	this->numbercells = i;
}
