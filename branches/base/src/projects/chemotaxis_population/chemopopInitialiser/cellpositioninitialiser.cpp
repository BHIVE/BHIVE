#include "cellpositioninitialiser.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

CellPositionInitialiser::CellPositionInitialiser() { this->isCubeGrid = false; }

CellPositionInitialiser::~CellPositionInitialiser() { }

void CellPositionInitialiser::initAllInSamePosition(Agent *ag, double x, double y, double z, int num) {
	cerr << "#  starting all cells in the same position (initAllInSamePosition): " << x << " "  << y  << " " << z << " " << num << endl;
	Database *db = ag->getDatabase();
	IntegerData *cellnumber = (IntegerData* ) db->getDataItem("numberofcells");
	cellnumber->setInteger(num);
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	positions->reserveSize(num);
	TVectorData<int> *cell_voxel_positions;
	TVectorData<int> *number_cells_per_voxel;
	Grid *g;
	int voxel_index;
	this->cellnumber = num;
	if (this->isCubeGrid) {
		cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
		number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
		g = (Grid*) db->getDataItem("environment");
		voxel_index = g->insideVoxel(x,y,z);
		cell_voxel_positions->reserveSize(num);
		number_cells_per_voxel->reserveSize(g->getNumberOfVoxels());
	}
	for (int i=0; i<positions->size(); i++) {
		positions->at(i) = new TVectorData<double> ("vec", "tvector_double");
		positions->at(i)->reserveSize(3);
		positions->at(i)->at(0) = x;
		positions->at(i)->at(1) = y;
		positions->at(i)->at(2) = z;
		if (this->isCubeGrid) {
			cell_voxel_positions->at(i) = voxel_index;
			number_cells_per_voxel->at(voxel_index) = number_cells_per_voxel->at(voxel_index) + 1;
		}
	}
	this->doTheRest(ag);
}


void CellPositionInitialiser::initUniformDistAlongXAxis(Agent *ag, double minX, double maxX, int num) {
	cerr << "#  starting all cells uniformly along x-axis (initUniformDistAlongXAxis): " << minX << " to "  << maxX  << " " << num << endl;

	if (minX>=maxX) {
		throw HiveException("when init cell positions uniformly, maxX must be larger than minX", "CellPositionInitialiser::initUniformDistAlongXAxis(Agent *ag, double minX, double maxX, int num)");
	}
	Database *db = ag->getDatabase();
	IntegerData *cellnumber = (IntegerData* ) db->getDataItem("numberofcells");
	cellnumber->setInteger(num);
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	positions->reserveSize(num);
//	TVectorData<int> *cell_voxel_positions;
//	TVectorData<int> *number_cells_per_voxel;
//	Grid *g;
//	int voxel_index;
	this->cellnumber = num;
	if (this->isCubeGrid) {
		cerr<<" when init cell positions uniformly along x-axis, cubegrid not yet supported."<<endl; exit(1);
//		cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
//		number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
//		g = (Grid*) db->getDataItem("environment");
//		voxel_index = g->insideVoxel(x,y,z);
//		cell_voxel_positions->reserveSize(num);
//		number_cells_per_voxel->reserveSize(g->getNumberOfVoxels());
	}


	for (int i=0; i<positions->size(); i++) {
		positions->at(i) = new TVectorData<double> ("vec", "tvector_double");
		positions->at(i)->reserveSize(3);

		//generate random x-position, and set the others to zero
		positions->at(i)->at(0) = Util::RANDOM_CLOSED()*(maxX-minX)+minX;
		positions->at(i)->at(1) = 0;
		positions->at(i)->at(2) = 0;
//		if (this->isCubeGrid) {
//			cell_voxel_positions->at(i) = voxel_index;
//			number_cells_per_voxel->at(voxel_index) = number_cells_per_voxel->at(voxel_index) + 1;
//		}
	}
	this->doTheRest(ag);
}



void CellPositionInitialiser::initRandomlyAroundPosition(Agent *ag, double x, double y, double z, double r, int num) {
	cerr << "#  starting all cells randomly around position(initRandomlyAroundPosition): " << x << " "  << y  << " " << z << " " << r << " " << num << endl;

	Database *db = ag->getDatabase();
	IntegerData *numcells = (IntegerData*) db->getDataItem("numberofcells");
	numcells->setInteger(num);
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	positions->reserveSize(num);
	TVectorData<int> *cell_voxel_positions;
	TVectorData<int> *number_cells_per_voxel;
	Grid *g;
	int voxel_index;
	this->cellnumber = num;
	if (this->isCubeGrid) {
		cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
		number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
		g = (Grid*) db->getDataItem("environment");
		cell_voxel_positions->reserveSize(num);
		number_cells_per_voxel->reserveSize(g->getNumberOfVoxels());
	}
	double posx, posy, posz;
	for (int i=0; i<positions->size(); i++) {
		do {
			posx = x-r+2*r*Util::RANDOM_CLOSED();
			posy = y-r+2*r*Util::RANDOM_CLOSED();
			posz = z-r+2*r*Util::RANDOM_CLOSED();
		} while (((posx-x)*(posx-x)+(posy-y)*(posy-y)+(posz-z)*(posz-z))>r*r);
		positions->at(i) = new TVectorData<double> ("vec", "tvector_double");
		positions->at(i)->reserveSize(3);
		positions->at(i)->at(0) = posx;
		positions->at(i)->at(1) = posy;
		positions->at(i)->at(2) = posz;
		if (this->isCubeGrid) {
			voxel_index = g->insideVoxel(posx, posy, posz);
			cell_voxel_positions->at(i) = voxel_index;
			number_cells_per_voxel->at(voxel_index) = number_cells_per_voxel->at(voxel_index) + 1;
		}
	}
	this->doTheRest(ag);
}


void CellPositionInitialiser::initRandomlyInCube(Agent *ag, int num) {
	cerr << "#  starting all cells randomly in cube (initRandomlyInCube): " << " " << num << endl;

	this->cellnumber = num;
	if(this->isCubeGrid) {
		Database *db = ag->getDatabase();
		IntegerData *numcells = (IntegerData*) db->getDataItem("numberofcells");
		numcells->setInteger(num);
		TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
		TVectorData<int> *cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
		TVectorData<int> *number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
		Grid *g = (Grid*) db->getDataItem("environment");
		positions->reserveSize(num);
		cell_voxel_positions->reserveSize(num);
		number_cells_per_voxel->reserveSize(g->getNumberOfVoxels());
		double sideLength = g->getCubeSideLength();

		double posx=0, posy=0, posz=0; int voxel_index = 0;
		for (int i=0; i<positions->size(); i++) {

			posx = sideLength*Util::RANDOM_CLOSED();
			posy = sideLength*Util::RANDOM_CLOSED();
			posz = sideLength*Util::RANDOM_CLOSED();

			positions->at(i)->reserveSize(3);
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
	this->doTheRest(ag);
}

void CellPositionInitialiser::initFromFile(Agent *ag, ifstream& input) {
	cerr << "#  starting all cells at a position based on a file (initFromFile) " << endl;


	// some local variables
	Database *db = ag->getDatabase();
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	TVectorData<int> *cell_voxel_positions;
	TVectorData<int> *number_cells_per_voxel;
	Grid *g;
	int voxel_index;
	double x,y,z;
	if (this->isCubeGrid) {
		cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
		number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
		g = (Grid*) db->getDataItem("environment");
		number_cells_per_voxel->reserveSize(g->getNumberOfVoxels());
		for (int i=0; i<g->getNumberOfVoxels(); i++)
			number_cells_per_voxel->at(i) = 0;
	}
	istringstream iss;
	string line = "";
	this->cellnumber = 0;
	// do the parsing of the input file
	while (getline(input,line)) {
		iss.clear();
		iss.str(line);
		iss >> x;
		iss >> y;
		iss >> z;
		TVectorData<double> *vec = new TVectorData<double>("position", "tvector_double");
		vec->reserveSize(3);
		vec->at(0) = x; vec->at(1) = y; vec->at(2) = z;
		positions->addElementToEnd(vec);
		if (this->isCubeGrid) {
			voxel_index = g->insideVoxel(x,y,z);
			cell_voxel_positions->addElementToEnd(voxel_index);
			number_cells_per_voxel->at(voxel_index) = number_cells_per_voxel->at(voxel_index) + 1;
		}
		this->cellnumber += 1;
	}
	this->doTheRest(ag);
}

void CellPositionInitialiser::initFromFileForMicroFluidic(Agent *ag, ifstream& input, ifstream& geo) {
	cerr << "#  starting all cells at a position based on a microfluidic file (initFromFileForMicroFluidic) " << endl;

	//go to the beginning of the file
	input.seekg(0,ios::beg);
	geo.seekg(0,ios::beg);

	// some local variables.
	string line = "";
	istringstream iss;
	string line2 = "";
	istringstream iss2;

	string from = ",";
	string to   = " ";
	size_t lookHere;
	size_t foundHere;

	int voxel_index = 0;
	int num_cells   = 0;
	int value       = 0;
	vector<double> coordinate(3);

	// get the database of the agent
	Database *db = ag->getDatabase();
	// get cell_positions
	TVectorData<TVectorData<double>* > *cell_positions =
			(TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	// get environment grid
	Grid *g = (Grid *) db->getDataItem("environment");
	// get cell_voxel_positions
	TVectorData<int> *cell_voxel_positions = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
	// get number cells_per_voxel
	TVectorData<int> *number_cells_per_voxel = (TVectorData<int>* ) db->getDataItem("number_cells_per_voxel");
	number_cells_per_voxel->reserveSize(g->getNumberOfVoxels());
	for (int i=0; i<g->getNumberOfVoxels(); i++)
		number_cells_per_voxel->at(i) = 0;

	// iterate over the input
	while (getline(input,line)) {
		lookHere = 0;
		// remove comma delimiters and change them to empty spaces
		while ((foundHere = line.find(from, lookHere)) != string::npos) {
			line.replace(foundHere,from.size(),to);
			lookHere = foundHere + to.size();
		}
		// remove comma delimiters from geometry file
		getline(geo,line2);
		lookHere = 0;
		while ((foundHere = line2.find(from, lookHere)) != string::npos) {
			line2.replace(foundHere,from.size(),to);
			lookHere = foundHere + to.size();
		}
		iss.clear();      // cell input
		iss.str(line);
		iss2.clear(); 	  // geometry input
		iss2.str(line2);
		while (iss2 >> value) {
			iss >> num_cells;
			// if we have a real voxel
			if (value == 1) {
				// get the center coordinate of the voxel that we currently are dealing with
				g->getCenterOfVoxel(voxel_index++, coordinate);
				// add cells to the system
				for (int i=0; i<num_cells; i++) {
					// add a position to the cell positions vector
					TVectorData<double> *pos = new TVectorData<double>("position", "tvectordata_double");
					pos->reserveSize(3);
					pos->at(0) = coordinate[0]; pos->at(1) = coordinate[1]; pos->at(2) = coordinate[2];
					cell_positions->addElementToEnd(pos);
					// add an entry to the cell_voxel_positions vector
					cell_voxel_positions->addElementToEnd(voxel_index-1);
					// update number cells per voxel
					number_cells_per_voxel->at(voxel_index-1) = number_cells_per_voxel->at(voxel_index-1) + 1;
				}
			}
		}
	}
	// set the numberofcells to the correct value
	IntegerData *nc = (IntegerData*) db->getDataItem("numberofcells");
	nc->setInteger(cell_positions->size());
	this->cellnumber = cell_positions->size();
	// initialise all the remaining vectors
	this->doTheRest(ag);
}


void CellPositionInitialiser::doTheRest(Agent *ag) {
	/// obtain the database of the agent
	Database *db = ag->getDatabase();

	// get all vectors from the agent of which the size depends on the
	// number of cells in the system and set the size accordingly
	// swimmingstates
	if (db->doesDataItemExist("swimmingstates")) {
		TVectorData<int> *swimmingstates = (TVectorData<int> *) db->getDataItem("swimmingstates");
		swimmingstates->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++)
			swimmingstates->at(i) = 0;
	}
	// lastswimmingstates
	if (db->doesDataItemExist("lastswimmingstates")) {
		TVectorData<int> *lastswimmingstates = (TVectorData<int> *) db->getDataItem("lastswimmingstates");
		lastswimmingstates->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++)
			lastswimmingstates->at(i) = 0;
	}
	// cellnextturnangle
	if (db->doesDataItemExist("cellnextturnangle")) {
		TVectorData<double> *cellnextturnangle = (TVectorData<double>* ) db->getDataItem("cellnextturnangle");
		cellnextturnangle->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++)
			cellnextturnangle->at(i) = 0.0;
	}
	// desired_cell_consumption
	if (db->doesDataItemExist("desired_cell_consumption")) {
		TVectorData<double> *desired_cell_consumption = (TVectorData<double>* ) db->getDataItem("desired_cell_consumption");
		desired_cell_consumption->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++)
			desired_cell_consumption->at(i) = 0;
	}
	// celldirvecs
	if (db->doesDataItemExist("celldirvecs")) {
		TVectorData<TVectorData<double>* > *cell_dir_vecs =
				(TVectorData<TVectorData<double>* >*) db->getDataItem("celldirvecs");
		cell_dir_vecs->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++) {
			cell_dir_vecs->at(i) = new TVectorData<double> ("vec", "tvector_double");
			cell_dir_vecs->at(i)->reserveSize(3);
			cell_dir_vecs->at(i)->at(0) = 1;
			cell_dir_vecs->at(i)->at(1) = 0;
			cell_dir_vecs->at(i)->at(2) = 0;
		}
	}
	// up vector
	if (db->doesDataItemExist("cellupvecs")) {
		TVectorData<TVectorData<double>* > *cell_up_vecs =
				(TVectorData<TVectorData<double>* >*) db->getDataItem("cellupvecs");
		cell_up_vecs->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++) {
			cell_up_vecs->at(i) = new TVectorData<double> ("vec", "tvector_double");
			cell_up_vecs->at(i)->reserveSize(3);
			cell_up_vecs->at(i)->at(0) = 0;
			cell_up_vecs->at(i)->at(1) = 1;
			cell_up_vecs->at(i)->at(2) = 0;
		}
	}
	// cellspeed
	if (db->doesDataItemExist("cellspeeds")) {
		TVectorData<double> *cell_speeds = (TVectorData<double>*) db->getDataItem("cellspeeds");
		cell_speeds->reserveSize(this->cellnumber);
		// do the initialisation
		double speedToUse = 20;
		if (db->doesDataItemExist("all_cell_speeds")) {
			speedToUse = ((DoubleData *)db->getDataItem("all_cell_speeds"))->getDouble();
		}
		for (int i=0; i<this->cellnumber; i++)
			cell_speeds->at(i) = speedToUse;
	}
	// cellnumber
	if (db->doesDataItemExist("numberofcells")) {
		IntegerData *cellnum = (IntegerData *) db->getDataItem("numberofcells");
		cellnum->setInteger(this->cellnumber);
	}



	// add array to remember last cell directions
	double startTimeForHistory = 500+30; double endTimeForHistory = 9999999;
	double historyLengthInSeconds = 60;
	int historyLength = (int)(historyLengthInSeconds/0.01);

	if (db->doesDataItemExist("xdir_history") && db->doesDataItemExist("xdirTumble_history") ) {
		TVectorData <HistoryCollector *> *hcDir= (TVectorData <HistoryCollector *> *) db->getDataItem("xdir_history");
		hcDir->reserveSize(this->cellnumber);
		for (int i=0; i<this->cellnumber; i++) {
			hcDir->at(i) = new HistoryCollector ("xhistory",historyLength);
		}

		TVectorData <HistoryAggregator *> *haDir= (TVectorData <HistoryAggregator *> *) db->getDataItem("xdirTumble_history");
		haDir->reserveSize(this->cellnumber);
		for (int i=0; i<this->cellnumber; i++) {
			haDir->at(i) = new HistoryAggregator ("xhistory_agg", hcDir->at(i), startTimeForHistory, endTimeForHistory);
		}
	}





}

