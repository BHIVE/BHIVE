#include "gridreaderformicrofluidic.hh"


using namespace std;
using namespace Hive;

GridReaderForMicroFluidic::GridReaderForMicroFluidic() {
	this->inputset = false;
	this->g = new Grid();
	g->setIsCubeGrid(true);
	this->resolution = 0;
}

GridReaderForMicroFluidic::~GridReaderForMicroFluidic() {}

void GridReaderForMicroFluidic::setResolution(double r) {
	this->resolution = r;
}

void GridReaderForMicroFluidic::readGrid() {
	// check whether inputfile exists
	if (inputset == false) {
		// throw an exception, if the input does not exist
		throw HiveException("no input file assigned to GridReaderForMicroFluidic", "GridReaderForMicroFluidic::readGrid()");
	}
	// check whether we have a resolution / stepsize for building the grid
	if (resolution == 0) {
		// throw exception
		throw HiveException("no resolution assigned to  GridReaderForMicroFluidic", "GridReaderForMicroFluidic::readGrid()");
	}

	if (number_species == 0) {
		throw HiveException("trying to generate a grid without any chemical species", "GridReaderForMicroFluidic::readGrid()");
	}

	for (int i=0; i<names.size(); i++)
		g->addSpecies(names[i],diffs[i]);

	/// for the lookup tables
	this->used_voxels      = 0;
	this->used_coordinates = 0;
	this->setupLookupTables(input);
	input.clear();

	// some declarations for manipulating the input such that it can be
	// read with the help of an inputstringstream
	string line = "";
	string from = ",";
	string to   = " ";
	size_t lookHere;
	size_t foundHere;
	istringstream iss;
	// declarations for actual data processing
	int value;
	int xindex = 0;
	int yindex = 0;
	int number_coordinates_added = 0;
	// vectors for constructing the grid
	vector<vector<int> > voxels;
	vector<vector<int> > neighbors;
	vector<vector<double> > rates;
 	// iterate over the entire input
	while (getline(input,line)) {
		lookHere = 0;
		// remove comma delimiters and change them to empty spaces
		while ((foundHere = line.find(from, lookHere)) != string::npos) {
			line.replace(foundHere,from.size(),to);
			lookHere = foundHere + to.size();
		}
		iss.clear();
		iss.str(line);
		// do the actual data processing.
		// iterate over the row!
		while (iss >> value) {
			// only do something if there is a voxel
			if (value == 1)
				this->addVoxel(xindex, yindex, number_coordinates_added, voxels);
			xindex += 1;
		}
		// reset xindex to zero
		xindex  = 0;
		// decrease yindex
		yindex -= 1;
	}

	this->determineNeighborhood(voxels, neighbors, rates);
	g->setupQuadGrid(voxels, neighbors, rates);
}

void GridReaderForMicroFluidic::addVoxel(int xindex, int yindex, int& number_coordinates_added,
										 vector<vector<int> >& voxels) {
	// local declarations
	double x,y,z;
	int coordinate_index;
	vector<int> voxel(8);
	// set z to 0
	z = 0;
	// upper left corner of voxel
	coordinate_index = this->coordinate_lookup->at(-yindex)->at(xindex);
	x = xindex*resolution;
	y = yindex*resolution;
	if (coordinate_index == -1) {
		g->addCoordinate(x,y,z);
		coordinate_lookup->at(-yindex)->at(xindex) = used_coordinates;
		voxel.at(0) = used_coordinates++;
	} else
		voxel.at(0) = coordinate_index;

	// upper right corner of voxel
	coordinate_index = this->coordinate_lookup->at(-yindex)->at(xindex+1);
	x = (xindex+1)*resolution;
	y = yindex*resolution;
	if (coordinate_index == -1) {
		g->addCoordinate(x,y,z);
		coordinate_lookup->at(-yindex)->at(xindex+1) = used_coordinates;
		voxel.at(1) =  used_coordinates++;
	} else
		voxel.at(1) = coordinate_index;

	// lower right corner of voxel
	coordinate_index = this->coordinate_lookup->at(-(yindex-1))->at(xindex+1);
	x = (xindex+1)*resolution;
	y = (yindex-1)*resolution;
	if (coordinate_index == -1) {
		g->addCoordinate(x,y,z);
		coordinate_lookup->at(-(yindex-1))->at(xindex+1) = used_coordinates;
		voxel.at(2) = used_coordinates++;
	} else
		voxel.at(2) = coordinate_index;

	// lower left corner of voxel
	coordinate_index = this->coordinate_lookup->at(-(yindex-1))->at(xindex);
	x = xindex*resolution;
	y = (yindex-1)*resolution;
	if (coordinate_index == -1) {
		g->addCoordinate(x,y,z);
		coordinate_lookup->at(-(yindex-1))->at(xindex) = used_coordinates;
		voxel.at(3) = used_coordinates++;
	} else
		voxel.at(3) = coordinate_index;

	/// change z coordinate
	// z = resolution
	z = resolution;

	// upper left corner
	coordinate_index = this->coordinate_lookup_upper->at(-yindex)->at(xindex);
	x = xindex*resolution;
	y = yindex*resolution;
	if (coordinate_index == -1) {
		g->addCoordinate(x,y,z);
		coordinate_lookup_upper->at(-yindex)->at(xindex) = used_coordinates;
		voxel.at(4) = used_coordinates++;
	} else
		voxel.at(4) = coordinate_index;

	// upper right corner
	coordinate_index = this->coordinate_lookup_upper->at(-yindex)->at(xindex+1);
	x = (xindex+1)*resolution;
	y = yindex*resolution;
	if (coordinate_index == -1) {
		g->addCoordinate(x,y,z);
		coordinate_lookup_upper->at(-yindex)->at(xindex+1) = used_coordinates;
		voxel.at(5) = used_coordinates++;
	} else
		voxel.at(5) = coordinate_index;


	// lower right corner
	coordinate_index = this->coordinate_lookup_upper->at(-(yindex-1))->at(xindex+1);
	x = (xindex+1)*resolution;
	y = (yindex-1)*resolution;
	if (coordinate_index == -1) {
		g->addCoordinate(x,y,z);
		coordinate_lookup_upper->at(-(yindex-1))->at(xindex+1) = used_coordinates;
		voxel.at(6) = used_coordinates++;
	} else
		voxel.at(6) = coordinate_index;


	// lower left corner
	coordinate_index = this->coordinate_lookup_upper->at(-(yindex-1))->at(xindex);
	x = xindex*resolution;
	y = (yindex-1)*resolution;
	if (coordinate_index == -1) {
		g->addCoordinate(x,y,z);
		this->coordinate_lookup_upper->at(-(yindex-1))->at(xindex) = used_coordinates;
		voxel.at(7) = used_coordinates++;
	} else
		voxel.at(7) = coordinate_index;


	// add voxel to voxel lookup
	this->voxel_lookup->at(-yindex)->at(xindex) = used_voxels++;

	// add voxel to voxels vector
	voxels.push_back(voxel);

}


void GridReaderForMicroFluidic::determineNeighborhood(vector<vector<int> >& voxels,
													  vector<vector<int> >& neighborhood,
													  vector<vector<double> >& rates) {
	cerr << "in here" << endl;
	neighborhood.resize(voxels.size());
	rates.resize(voxels.size());

	int voxel_a, voxel_b;
	double basal_rate = 1/(resolution*resolution);
	for (unsigned int i=0; i<voxel_lookup->size(); i++) {
		for (unsigned int j=0; j<voxel_lookup->at(i)->size(); j++) {
			voxel_a = voxel_lookup->at(i)->at(j);
			if (voxel_a != -1) {
				// i+1
				if (i+1 < voxel_lookup->size()) {
					voxel_b = voxel_lookup->at(i+1)->at(j);
					if (voxel_b != -1) {
						neighborhood.at(voxel_a).push_back(voxel_b);
						neighborhood.at(voxel_b).push_back(voxel_a);
						rates.at(voxel_a).push_back(basal_rate);
						rates.at(voxel_b).push_back(basal_rate);
					}
				}
				// j+1
				if (j+1 < voxel_lookup->at(i)->size()) {
					voxel_b = voxel_lookup->at(i)->at(j+1);
					if (voxel_b != -1) {
						neighborhood.at(voxel_a).push_back(voxel_b);
						neighborhood.at(voxel_b).push_back(voxel_a);
						rates.at(voxel_a).push_back(basal_rate);
						rates.at(voxel_b).push_back(basal_rate);
					}
				}
			}
		}
	}


	/// we are done using these matrices ... hence we can remove them.
	for (unsigned int i=0; i<voxel_lookup->size(); i++)
		delete voxel_lookup->at(i);
	delete voxel_lookup;
	for (unsigned int i=0; i<coordinate_lookup->size(); i++)
		delete coordinate_lookup->at(i);
	delete coordinate_lookup;
	for (unsigned int i=0; i<coordinate_lookup_upper->size(); i++)
			delete coordinate_lookup_upper->at(i);
	delete coordinate_lookup_upper;

}


void GridReaderForMicroFluidic::setupLookupTables(ifstream &input) {
	/// some local variables
	int number_lines;
	int number_columns;
	string line="";
	string from = ",";
	string to   = " ";
	size_t lookHere;
	size_t foundHere;
	istringstream iss;

	// count number of lines
	number_lines = count(istreambuf_iterator<char>(input), istreambuf_iterator<char>(), '\n');

	// go back to the beginning
	input.seekg(0,ios::beg);

	// get a line from the input and remove comma delimiters and change them to empty spaces
	getline(input,line);
	lookHere = 0;
	while ((foundHere = line.find(from, lookHere)) != string::npos) {
		line.replace(foundHere,from.size(),to);
		lookHere = foundHere + to.size();
	}

	// count number of columns
	iss.clear();
	iss.str(line);
	number_columns = 0;
	while (iss >> lookHere)
		number_columns++;

	// reset input to the beginning
	input.seekg(0,ios::beg);

	// reserve size for the lookup tables
	voxel_lookup = new vector<vector<int>* > (number_lines);
	for (int i=0; i<number_lines; i++) {
		voxel_lookup->at(i) = new vector<int> (number_columns);
		for (int j=0; j<number_columns; j++)
			voxel_lookup->at(i)->at(j) = -1;
	}
	coordinate_lookup = new vector<vector<int>* > (number_lines+1);
	for (int i=0; i<number_lines+1; i++) {
		coordinate_lookup->at(i) = new vector<int> (number_columns+1);
		for (int j=0; j<number_columns+1; j++)
			coordinate_lookup->at(i)->at(j) = -1;
	}
	coordinate_lookup_upper = new vector<vector<int>* > (number_lines+1);
	for (int i=0; i<number_lines+1; i++) {
		coordinate_lookup_upper->at(i) = new vector<int> (number_columns+1);
		for (int j=0; j<number_columns+1; j++)
			coordinate_lookup_upper->at(i)->at(j) = -1;
	}
}


//
void GridReaderForMicroFluidic::readInitialConcentrations(ifstream& concfile) {
	// local variable
	int voxel_index = 0;
	int value;
	double conc;

	// go to the correct position
	input.clear();
	input.seekg(0,ios::beg);
	concfile.seekg(0,ios::beg);


	// local variables for reading the input
	string line1 = "";
	istringstream iss1;
	string line2 = "";
	istringstream iss2;

	string from = ",";
	string to   = " ";
	size_t lookHere;
	size_t foundHere;

	// iterate over the input
	while (getline(input,line1)) {
		lookHere = 0;
		// remove comma delimiters and change them to empty spaces
		while ((foundHere = line1.find(from, lookHere)) != string::npos) {
			line1.replace(foundHere,from.size(),to);
			lookHere = foundHere + to.size();
		}
		// read from the second file
		getline(concfile,line2);
		lookHere = 0;
		while ((foundHere = line2.find(from,lookHere)) != string::npos) {
			line2.replace(foundHere,from.size(),to);
			lookHere = foundHere + to.size();
		}
		// push cells into
		iss1.clear();
		iss2.clear();
		iss1.str(line1);
		iss2.str(line2);

		while (iss1 >> value) {
			iss2 >> conc;
			if (value == 1) {
				g->setConcentration(voxel_index++, 0, conc);
			}
		}
	}
}
