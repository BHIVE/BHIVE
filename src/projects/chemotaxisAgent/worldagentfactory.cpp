/*
 * worldagentfactory.cpp
 *
 *@todo actions , messagegenerators
 *
 *  Created on: Oct 20, 2009
 *      Author: garrit jentsch
 */

#include "worldagentfactory.hh"
#include "chemotaxisworldactions.hh"

using namespace Chemotaxis;

WorldAgentFactory::WorldAgentFactory() {
	cerr << "hello i am  the worldagentfactory!" << endl;
}

WorldAgentFactory::WorldAgentFactory(InputSystemReader *isr):AgentFactory(isr) {}

Agent* WorldAgentFactory::createAgent() {
	Agent *world = new Agent();
	// first get the database of the agent
	Hive::Database *db = world->getDatabase();
	// we start setting up a dataitem
	DiffData *diff_data = new DiffData("diffusion_grid");

	Grid *grid = new Grid("garrits_diffusion_grid");

	// declare new meshinputreader
	mir = new NetgenMeshInputReader();

	// here we directly pass the name of the file
	// not very nice but will do for the moment
	ifstream input_test_file("test/forchris.ntf");
//	ifstream input_test_file("test/testMeshNov2.ntf");
//	ifstream input_test_file("test/testFile.ntf");
	// read mesh does everything. it calculates the neighborhood relations.
	// it calculates the volumes and the center coordinates.
	mir->readMesh(input_test_file);

	// name of diffusing species
	// this is certainly not the way to set up species and diffusions but for the moment it will work.
	vector<string> species;
	species.push_back("Chemoattractant");
	species.push_back("Waste");

	// diffusion coefficient
	vector<double> d;
	d.push_back(0.001);
	// the decapitated wifes hardly move anywhere ...
	d.push_back(0.00001);

	grid->setSpecies(species);
	grid->setSpeciesDiffusionCoeffs(d);
	grid->setupCoordinates(mir->getCoordinates());
	grid->setupTetraGrid(mir->getVoxels(), mir->getNeighborhood(),
					     mir->getInterfaceAreas(), mir->getVolumes(),
				         mir->getCenterCoordinates());

	// add information about species and diffusion coefficients to dataitem
	//	diff_data->setSpecies(species);
	//	diff_data->setSpeciesDiffusionCoefs(d);
	// setup the coordinates of the grid
	//	diff_data->setupCoordinates(mir->getCoordinates());
	// setup grid information
	//	diff_data->setupGrid(mir->getVoxels(), mir->getNeighborhood());
	// setup base diffusion rates
	//	diff_data->setupBaseRates(mir->getInterfaceAreas(), mir->getVolumes(), mir->getCenterCoordinates());
	//	diff_data->setupVerticesPointedAtByVoxels(mir->getNodesPointedAtByVoxels());
	//	diff_data->setupVolumes(mir->getVolumes());

	// set up center coordinates of the voxels in the grid
	//	vector<vector<double> > ctmp = mir->getCenterCoordinates();
	//	for (unsigned int i = 0; i<ctmp.size(); i++)
	//		diff_data->setVoxelCenterCoordinate(i, ctmp[i][0], ctmp[i][1], ctmp[i][2]);

	cerr << " done setting up the grid" << endl;
	grid->setupSearchTree();

//	exit(1);
	//  we have to generate the octtree for the diff_data ...
	//	diff_data->createTree();
	//	diff_data->printTree();

//	cerr << " here " << endl;
	Coordinate bla;
	for (int i=0; i<1; i++) {
		double x = Util::RANDOM_CLOSED()*1;
		double y = Util::RANDOM_CLOSED()*1;
		double z = Util::RANDOM_CLOSED()*1;
		bla.setCoordinates(x,y,z);
		NVoxel *r = grid->getVoxel(bla);
		if (r == NULL) {
			cerr << "halloele" << endl;
		} else {
			cerr << "in grid " << x << " " << y << " " << z << " " << r->getVoxelID() << endl;
		}
	}
//	for (int i=0; i<grid->g)


	exit(1);
	for (int i=0; i<diff_data->getNumberOfVoxels(); i++) {
		Coordinates c = diff_data->getVoxelCenterCoordinate(i);
		Voxel* Result = diff_data->getVoxel(0.96875, 0.09375, 0.125);
		long int containingVoxel = -1;
		if (Result != NULL) {
			containingVoxel = Result->voxel_id;
		}
		if (Result == NULL) {
			cerr << "garrit" << endl;
//		if (containingVoxel != i) {
			cerr << i << " " << c.x << " " << c.y << " " << c.z <<  " " << containingVoxel << endl;
			diff_data->printVerticesOfVoxelOnSterr(i);
			exit(1);
		}
	}


/*
	Voxel *r = diff_data->getVoxel(10,10,10);
	if (r == NULL) {
		cerr << "REASADFADSFSAFD" << endl;
		exit(0);
	}
*/
	// initialisation of the chemical concentrations
	for (int i=0; i<diff_data->getNumberOfVoxels(); i++) {
		if (mir->getCenterCoordinates()[i][0] < 1.25) {
			diff_data->getVoxel(i)->concentrations[0] = 1.0/(1+10.*mir->getCenterCoordinates()[i][0]);
			diff_data->getVoxel(i)->newConcentrations[0] = 0.0;
		} else {
			diff_data->getVoxel(i)->concentrations[0] = 0.0;
			diff_data->getVoxel(i)->newConcentrations[0] = 0.0;
		}
	}

//	diff_data->getVoxel(4950)->concentrations[0] = 1.0;

	// add dataitem to agent's diffusion database
	db->addData(diff_data->getName(),diff_data);
	cerr << "here2" << endl;
	// add cell positions to world
	DoubleMatrixData *cell_coordinates = new DoubleMatrixData("cell_coordinates", this->number_cells, 3);
	db->addData(cell_coordinates->getName(), cell_coordinates);
	for (int i=0; i<this->number_cells; i++) {
		cell_coordinates->setDouble(0.51001,i,0);
		cell_coordinates->setDouble(0.51001,i,1);
		cell_coordinates->setDouble(0.51001,i,2);
	}

	// next data item to be added is the cells_voxel_positions
	IntVectorData *cells_voxel_positions = new IntVectorData("cells_voxel_positions", this->number_cells);
	db->addData(cells_voxel_positions->getName(), cells_voxel_positions);
	for (int i=0; i<this->number_cells; i++) {
		Voxel *res = diff_data->getVoxel(0.51001, 0.51001, 0.51001);
		if (res == NULL)
			cerr << "still fuck" << endl;
		cells_voxel_positions->setInt(res->voxel_id, i);
	}

	// now add simulators to the agent
	this->addSimulatorToAgent(world);

	// finally add the actions to the simulator
	this->addActionsToAgentsActionSet(world);

	// add messagegenerators to agent
	this->addMessageGenerators(world);

	// get rid of the meshinputreader. we no longer need it.
	delete mir;

	return world;
}

Agent* WorldAgentFactory::createAgent(Action** ag) {
	Agent *world = new Agent();

	return world;
}

Agent* WorldAgentFactory::duplicateAgent(Agent *ag) {
	return NULL;
}

void WorldAgentFactory::setMeshInputReader(NetgenMeshInputReader *mir) {
	this->mir = mir;
}


void WorldAgentFactory::finalise() {
//	mir->setInputfile(input_mesh_file);
}

void WorldAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	Action *a1 = new UpdateCellVoxelPositionAction(0);
	a1->setAgent(ag);
	Action *a2 = new UpdateChemicalConcentrationAction(1);
	a2->setAgent(ag);
	ag->addAction(a1);
	ag->addAction(a2);
}

void WorldAgentFactory::addMessageGenerators(Agent *ag) {
	WorldNotifyPosToCellMessageGenerator *wnptcmg = new WorldNotifyPosToCellMessageGenerator(ag);
	ag->addMessageGenerator(wnptcmg);
	WorldNotifyChemicalConcentrationToCellMessageGenerator *wncctcmg = new WorldNotifyChemicalConcentrationToCellMessageGenerator(ag);
	ag->addMessageGenerator(wncctcmg);
}

void WorldAgentFactory::addSimulatorToAgent(Agent *a) {
	// create a pde simulator
	PDESimulator *pde = new PDESimulator();
	// tell the simulator that it acts on agent a ... this connects the agents database to the simulator
	pde->setAgent(a);
	// add simulator to agent
	a->addSimulator(pde);
}
