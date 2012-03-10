/*
 * chemotaxiscomposer.cpp
 *
 *  Created on: Oct 20, 2009
 *      Author: jentsch
 */

#include "chemotaxiscomposer.hh"

using namespace Hive;
using namespace Chemotaxis;

ChemotaxisComposer::ChemotaxisComposer () {
	cerr << "hello i am the chemotaxis composer" << endl;
}

ChemotaxisComposer::~ChemotaxisComposer() {}

void ChemotaxisComposer::setupSimulation() {
	int number_cells = 1;

	// initialise the agent factories
	this->initializeAgentFactories();
	((WorldAgentFactory *)factories[0])->setNumberCells(number_cells);

	// create the world
	maestro = this->createAgent(*factories[0]);
	orchestra.push_back(maestro);

	// Create the cells
	for(int c=0; c<number_cells; c++)
	{
		// create a cell
		Agent *thisCell = factories.at(1)->createAgent();
		orchestra.push_back(thisCell);
	}


	// finally create the chemotaxis nfsim systems
	for(int c=0; c<number_cells; c++)
	{
		Agent *thisNFsim = factories.at(2)->createAgent();

		NFsimNotifyMotorConfToCellMessageGenerator *nnmctcmg = new NFsimNotifyMotorConfToCellMessageGenerator(thisNFsim);
		thisNFsim->addMessageGenerator(nnmctcmg);

		UpdateLigandConcentration * ulc = new UpdateLigandConcentration();
		thisNFsim->addAction(ulc);
		ulc->setAgent(thisNFsim);

		orchestra.push_back(thisNFsim);
	}




	// set up the hierarchy among the agents
	this->setupAgentHierarchy();

	// finally add a serial communciator
	this->addSerialCommunicator();

}


void ChemotaxisComposer::initializeAgentFactories() {
	// add world agent factory to factories of the composer
	factories.push_back(new WorldAgentFactory());
	factories.push_back(new ChemotaxisCellFactory());

	map <string,string> argMap;
	//NF::parseArguments(argc, const_cast<const char**>(argv), argMap);
	argMap.insert(pair<string,string>("xml","test/nf/motor.xml"));

	Hive::NF_AgentFactory *nfaf = new Hive::NF_AgentFactory(argMap);
	factories.push_back(nfaf);
}


void ChemotaxisComposer::setupAgentHierarchy() {


	int number_cells = ((WorldAgentFactory *)factories[0])->getNumberCells();

	for(int c=1; c<=number_cells; c++)
	{
		Agent *cell = orchestra.at(c);
		Agent *nf   = orchestra.at(c+number_cells);

		maestro->addChild(cell->getAgentId());
		cell->setParent(maestro->getAgentId());

		cell->addChild(nf->getAgentId());
		nf->setParent(cell->getAgentId());
	}



}


Agent* ChemotaxisComposer::createAgent(AgentFactory& af) {
	return af.createAgent();
}

void ChemotaxisComposer::generateBugOutput(ofstream& output) {
	// at this point the output looks as follows:
	// first number of cells
	// x y z of first cell
	// ...
	// x y z of last cell
	// biomass of first cell
	// ...
	// biomass of last cell
	int numberCells = 0;
	Database *db = this->maestro->getDatabase();
	DoubleMatrixData *cell_coordinates = (DoubleMatrixData *) db->getDataItem("cell_coordinates");
	numberCells = cell_coordinates->getSizeXDim();
	output << numberCells << endl;
	cell_coordinates->printContent(output);

	for (int i=0; i<numberCells; i++) {
		// here i make use of the fact that the cells are stored consecutively within the orchestra
		// starting from position 1 !!!
		db = this->orchestra[i+1]->getDatabase();
		DoubleVectorData *dvd = (DoubleVectorData* ) db->getDataItem("Chemical");
		output << dvd->getDouble(0) << endl;
	}
}

void ChemotaxisComposer::generateChemicalOutput(ofstream& output) {
	// output format:
	// number of voxels
	// voxel's center_coordinate that is x y z followed by the two chemical's concentrations
	Database *db = this->maestro->getDatabase();
	DiffData *grid = (DiffData *) db->getDataItem("diffusion_grid");
	int number_voxels = grid->getNumberOfVoxels();
	output << number_voxels << endl;
	// obviously we know that there are two chemical concentrations ...
	for (int i=0; i<number_voxels; i++) {
		Coordinates c = grid->getVoxelCenterCoordinate(i);
		output << c.x << " " << c.y << " " << c.z << " " << grid->getVoxel(i)->concentrations[0]
		       << "  " << grid->getVoxel(i)->concentrations[1] << endl;
	}
}

void ChemotaxisComposer::addSerialCommunicator() {
	SerialCommunicator *post = new SerialCommunicator();
	for (unsigned int i=0; i<this->orchestra.size(); i++) {
		post->addAgent(orchestra[i]);
		orchestra[i]->addCommunicator(post);
	}
}
