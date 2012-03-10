#include "cellpositioninitialiser.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

CellPositionInitialiser::CellPositionInitialiser() { this->isCubeGrid = false; }


CellPositionInitialiser::~CellPositionInitialiser() { }

void CellPositionInitialiser::initAllInSamePosition(Agent *ag, double x, double y, double z) {
	Database *db = ag->getDatabase();
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	TVectorData<int> *cell_voxel_positions;
	TVectorData<int> *number_cells_per_voxel;
	Grid *g;
	int voxel_index;
	if (this->isCubeGrid) {
		cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
		number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
		g = (Grid*) db->getDataItem("environment");
		voxel_index = g->insideVoxel(x,y,z);
	}
	for (int i=0; i<positions->size(); i++) {
		positions->at(i)->at(0) = x;
		positions->at(i)->at(1) = y;
		positions->at(i)->at(2) = z;
		if (this->isCubeGrid) {
			cell_voxel_positions->at(i) = voxel_index;
			number_cells_per_voxel->at(voxel_index) = number_cells_per_voxel->at(voxel_index) + 1;
		}
	}
}

void CellPositionInitialiser::initRandomlyAroundPosition(Agent *ag, double x, double y, double z, double r) {
	Database *db = ag->getDatabase();
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	TVectorData<int> *cell_voxel_positions;
	TVectorData<int> *number_cells_per_voxel;
	Grid *g;
	int voxel_index;
	if (this->isCubeGrid) {
		cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
		number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
		g = (Grid*) db->getDataItem("environment");
	}
	double posx, posy, posz;
	for (int i=0; i<positions->size(); i++) {
		do {
			posx = x-r+2*r*Util::RANDOM_CLOSED();
			posy = y-r+2*r*Util::RANDOM_CLOSED();
			posz = z-r+2*r*Util::RANDOM_CLOSED();
		} while (((posx-x)*(posx-x)+(posy-y)*(posy-y)+(posz-z)*(posz-z))>r*r);
		positions->at(i)->at(0) = posx;
		positions->at(i)->at(1) = posy;
		positions->at(i)->at(2) = posz;
		if (this->isCubeGrid) {
			voxel_index = g->insideVoxel(posx, posy, posz);
			cell_voxel_positions->at(i) = voxel_index;
			number_cells_per_voxel->at(voxel_index) = number_cells_per_voxel->at(voxel_index) + 1;
		}
	}
}


void CellPositionInitialiser::initRandomlyInCube(Agent *ag) {

	if(this->isCubeGrid) {
		Database *db = ag->getDatabase();
		TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
		TVectorData<int> *cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
		TVectorData<int> *number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
		Grid *g = (Grid*) db->getDataItem("environment");
		double sideLength = g->getCubeSideLength();

		double posx=0, posy=0, posz=0; int voxel_index = 0;
		for (int i=0; i<positions->size(); i++) {

			posx = sideLength*Util::RANDOM_CLOSED();
			posy = sideLength*Util::RANDOM_CLOSED();
			posz = sideLength*Util::RANDOM_CLOSED();

			positions->at(i)->at(0) = posx;
			positions->at(i)->at(1) = posy;
			positions->at(i)->at(2) = posz;

			voxel_index = g->insideVoxel(posx, posy, posz);
			cell_voxel_positions->at(i) = voxel_index;
			number_cells_per_voxel->at(voxel_index) = number_cells_per_voxel->at(voxel_index) + 1;

		}


	} else {
		throw(HiveException("world is not a cube grid, so you cannot start randomly in the cube",
				"CellPositionInitialiser::initRandomlyAroundCube(Agent *ag)"));
	}
}

void CellPositionInitialiser::initFromFile(Agent *ag, ifstream& input) {
	Database *db = ag->getDatabase();
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	TVectorData<int> *cell_voxel_positions;
	TVectorData<int> *number_cells_per_voxel;
	Grid *g;
	int voxel_index;
	if (this->isCubeGrid) {
		cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
		number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
		g = (Grid*) db->getDataItem("environment");
	}
	double x, y, z;
	for (int i=0; i<positions->size(); i++) {
		input >> x;
		positions->at(i)->at(0) = x;
		input >> y;
		positions->at(i)->at(1) = y;
		input >> z;
		positions->at(i)->at(2) = z;
		if (this->isCubeGrid) {
			voxel_index = g->insideVoxel(x,y,z);
			cell_voxel_positions->at(i) = voxel_index;
			number_cells_per_voxel->at(voxel_index) = number_cells_per_voxel->at(voxel_index) + 1;
		}
	}
}


