#include "simplestconsumptionsimulatorforworld.hh"

using namespace std;
using namespace Hive;

SimplestConsumptionSimulatorForWorld::SimplestConsumptionSimulatorForWorld(Agent *a) : Simulator() {
	this->agent = a;
	this->db    = a->getDatabase();
	this->prepare();
}

SimplestConsumptionSimulatorForWorld::~SimplestConsumptionSimulatorForWorld() {}

void SimplestConsumptionSimulatorForWorld::setAgent(Agent *ag) {
	this->agent = ag;
	this->db    = ag->getDatabase();
	this->prepare();
}

void SimplestConsumptionSimulatorForWorld::step(double dt) {
	// first generate a map of the occupied voxels
	map<int, vector<int> > voxel_cells_map;
	map<int, double> voxel_total_desired_consumption;
	for (int i=0; i<cell_positions->size(); i++) {
		voxel_cells_map[cell_positions->at(i)].push_back(i);
		voxel_total_desired_consumption[cell_positions->at(i)] += dt*this->desired_cell_consumption->at(i);
	}

	// some local variables that we need
	map<int, vector<int> >::iterator iter;
	map<int, double>::iterator iter2;
	int voxel_index;
	double amount_in_voxel;
	vector<int> cells;

	bool split = false;

	// iterate over all voxels having cells
	iter2 = voxel_total_desired_consumption.begin();
	for (iter = voxel_cells_map.begin(); iter != voxel_cells_map.end(); iter++) {
		voxel_index     = iter->first;
		// get the amount of nutrient that is in the voxel
		amount_in_voxel = cube->getVoxel(voxel_index)->getConcentration(0);
		cells           = iter->second;

		// if there is less than zero food (from numerical precision), set it to zero
		if(amount_in_voxel<=0) {
			for (unsigned int j=0; j<cells.size(); j++) {
				// account for the time step
				this->desired_cell_consumption->at(cells[j]) = 0;
				// cerr << this->desired_cell_consumption->at(cells[j]) << endl;
				cube->getVoxel(voxel_index)->setConcentration(0, 0);
			}
		}
		// only change something if there is not enought nutrient for all cells in the voxel
		else if (amount_in_voxel < iter2->second) {
			// iterate over cells in the voxel
			split = true;
			for (unsigned int j=0; j<cells.size(); j++) {
				// update the desired cell consumption of the cell by the dt
				this->desired_cell_consumption->at(cells[j]) = (dt*this->desired_cell_consumption->at(cells[j])/iter2->second) *amount_in_voxel;
				// update the amount of nutrient in the voxel

				this->totalNutrient->subtractFromValue(this->desired_cell_consumption->at(cells[j]));
				cube->getVoxel(voxel_index)->setConcentration(0, cube->getVoxel(voxel_index)->getConcentration(0) - this->desired_cell_consumption->at(cells[j]));
			}
		} else {
			// only update the amount of nutrient in the voxel
			for (unsigned int j=0; j<cells.size(); j++) {
				// account for the time step
				this->desired_cell_consumption->at(cells[j]) = dt*this->desired_cell_consumption->at(cells[j]);

				//cerr << "-"<<cells[j]<<" d:"<<this->desired_cell_consumption->at(cells[j]) << endl;

				this->totalNutrient->subtractFromValue(this->desired_cell_consumption->at(cells[j]));
				cube->getVoxel(voxel_index)->setConcentration(0, cube->getVoxel(voxel_index)->getConcentration(0) - this->desired_cell_consumption->at(cells[j]));
			}
		}
		if (cube->getVoxel(voxel_index)->getConcentration(0) < 0)
			cube->getVoxel(voxel_index)->setConcentration(0,0);

		//if(split) cerr<<"split"<<endl;
	}

}

void SimplestConsumptionSimulatorForWorld::prepare()
{
	this->cube = (Grid*) this->db->getDataItem("environment");

	this->totalNutrient = (DoubleData*) this->db->getDataItem("total_nutrient");
	double sum = 0;
	for(int k=0; k<this->cube->getNumberOfVoxels(); k++)
		sum += cube->getVoxel(k)->getConcentration(0);

	cerr<<"Total nutrient in world: "<<sum<<endl;
	totalNutrient->setDouble(sum);

	this->desired_cell_consumption = (TVectorData<double>*) this->db->getDataItem("desired_cell_consumption");
	this->cell_positions = (TVectorData<int> *) this->db->getDataItem("cell_voxel_positions");
	this->number_cells_per_voxel = (TVectorData<int> *) this->db->getDataItem("number_cells_per_voxel");
}

void SimplestConsumptionSimulatorForWorld::synchroniseWithDatabase() {
}

void SimplestConsumptionSimulatorForWorld::initialise() { }
