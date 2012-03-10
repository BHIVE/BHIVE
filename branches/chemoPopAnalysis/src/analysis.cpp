#include "analysis.hh"

Analysis::Analysis() {
  this->number_cells    = 0;
  this->number_sweeps   = 0;
  this->output_interval = 0;

  this->binary_position_input  = false;
  this->binary_cell_data_input = false;
}

//cells, sweeps, size of output step
Analysis::Analysis(int n, int s, double d) {
  this->number_cells    = n;
  this->number_sweeps   = s;
  this->output_interval = d;
}


Analysis::~Analysis() { }


void Analysis::analysePositions() {
  vector<double> output(3);
  vector<double> position(3,0);
  vector<vector<double> > *block = new vector<vector<double> > (this->number_cells, position);
  vector<vector<double> > *positions;

  // loop overa all the data
  for (int i=0; i<this->number_sweeps; i++) {
    // output the time
    outputfile << (i+1)*output_interval << " ";
    // get the entire data block
    if (!this->binary_position_input)
      this->getBlock(block);
    else {
    	this->getBlockBinary(block);
    }
    // iterate over all classes
    for (iter = cell_classes.begin(); iter != cell_classes.end(); iter++) {
      // get the positions of all the cells in the class
    	//cerr << iter->first << endl;
      positions = new vector<vector<double> >(iter->second.size(),position);
    //  cerr << i << " " << iter->second.size() << endl;
      for (int j=0; j<iter->second.size(); j++) {
    	  positions->at(j)[0] = block->at(iter->second[j])[0];
    	  positions->at(j)[1] = block->at(iter->second[j])[1];
    	  positions->at(j)[2] = block->at(iter->second[j])[2];
      }
      // calculate the center of mass
      this->calculateCenterOfMass(positions, output);
      // delete the positions matrix
      delete positions;
      // output the results
      outputfile << " " << output[0] << " " << output[1] << " " << output[2] << " ";
    }
    outputfile << endl;
  }
}

void Analysis::analyseDistances() {
	vector<double> output(3);
	vector<double> position(3,0);
	vector<vector<double> > *block = new vector<vector<double> > (this->number_cells, position);
	vector<vector<double> > *positions;
	for (int i=0; i<this->number_sweeps; i++) {
	    // output the time
	    outputfile << (i+1)*output_interval << " ";
	    // get the entire data block
	    if (!this->binary_position_input)
	      this->getBlock(block);
	    else
	      this->getBlockBinary(block);
	    // iterate over all classes
	    for (iter = cell_classes.begin(); iter != cell_classes.end(); iter++) {
	      // get the positions of all the cells in the class
	      positions = new vector<vector<double> >(iter->second.size(),position);
	      for (int j=0; j<iter->second.size(); j++) {
	    	  positions->at(j)[0] = block->at(iter->second[j])[0];
	    	  positions->at(j)[1] = block->at(iter->second[j])[1];
	    	  positions->at(j)[2] = block->at(iter->second[j])[2];
	      }
	      // calculate the center of mass
	      this->calculateAverageDistanceFromInitialPosition(positions, output);
	      // delete the positions matrix
	      delete positions;
	      // output the results
	      outputfile << " " << output[0] << " " << output[1] << " " << output[2] << " ";
	    }
	    outputfile << endl;
	  }


}

int Analysis::getNumberOfOutputs() {
	int number = 0;
	vector<double> pos(3);
	vector<vector<double> > *matrix = new vector<vector<double> > (this->number_cells,pos);
	while (this->getBlockBinary(matrix) == 0) {
		number++;
	}
	position_input.clear();
	position_input.seekg(ios_base::beg);

	this->number_sweeps = number;
	return number;
}


void Analysis::getBlock(vector<vector<double> >* matrix) {
  string line = "";
  double d = 0;
  getline(position_input,line);
  for (int i=0; i<this->number_cells; i++) {
    position_input >> d;
    matrix->at(i)[0] = d;
    position_input >> d;
    matrix->at(i)[1] = d;
    position_input >> d;
    matrix->at(i)[2] = d;
  }
  getline(position_input,line);
  getline(position_input,line);
  getline(position_input,line);
}


int Analysis::getBlockBinary(vector<vector<double> >* matrix) {
  double d;
  if (position_input.eof())
	  return 1;
  else {
	  for (int i=0; i<this->number_cells; i++) {
		  position_input.read(reinterpret_cast<char*>(&d), sizeof(d));
		  matrix->at(i)[0] = d;
		  position_input.read(reinterpret_cast<char*>(&d), sizeof(d));
		  matrix->at(i)[1] = d;
		  position_input.read(reinterpret_cast<char*>(&d), sizeof(d));
		  matrix->at(i)[2] = d;
	  }
	  return 0;
  }
}

void Analysis::outputMap() {
	this->outputfile << "# time ";
	for (iter = this->cell_classes.begin(); iter != this->cell_classes.end(); iter++)
		this->outputfile << iter->first << " " ;
	this->outputfile << endl;
}

void Analysis::setCellDataInputFile(string name, bool isbinary) {
  if (isbinary) {
    this->cell_data_input.open(name.c_str(), ios_base::binary);
    this->binary_cell_data_input = true;
  } else
    this->cell_data_input.open(name.c_str());
}


void Analysis::setCellPositionsInputFile(string name, bool isbinary) {
  if (isbinary) {
    this->position_input.open(name.c_str(), ios_base::binary);
    this->binary_position_input = true;
  } else
    this->position_input.open(name.c_str());
}


void Analysis::setInitialPosition(double x, double y, double z) {
  this->initial_position.resize(3);
  initial_position[0] = x;
  initial_position[1] = y;
  initial_position[2] = z;
}


void Analysis::setOutputFile(string name) {
  outputfile.open(name.c_str());
}

void Analysis::setCellClasses(map<double, vector<int> >& arg) {
	this->cell_classes = arg;
}


void Analysis::calculateCenterOfMass(vector<vector<double> > *input,
				     vector<double>& output) {
  double x = 0;
  double y = 0;
  double z = 0;
  for (int i=0; i<input->size(); i++) {
    x += input->at(i)[0];
    y += input->at(i)[1];
    z += input->at(i)[2];
  }

  if (output.size() != 3)
    output.resize(3);
//  cerr << x << endl;
  output[0] = x/input->size();
  output[1] = y/input->size();
  output[2] = z/input->size();
}


void Analysis::calculateAverageDistanceFromInitialPosition(vector<vector<double> > *input,
							   vector<double>& output ) {

  double mean_distance        = 0;
  double mean_square_distance = 0;
  double variance             = 0;
  double add                  = 0;
  for (int i=0; i<input->size(); i++) {
    add = ((input->at(i)[0]-initial_position[0])*(input->at(i)[0]-initial_position[0])+
	   (input->at(i)[1]-initial_position[1])*(input->at(i)[1]-initial_position[1])+
	   (input->at(i)[2]-initial_position[2])*(input->at(i)[2]-initial_position[2]));
    mean_distance += add;
    mean_square_distance += add*add;
  }

  mean_distance /= input->size();
  mean_square_distance /= input->size();

  variance = mean_square_distance - mean_distance*mean_distance;

  if (output.size() != 3)
    output.resize(3);
  output[0] = mean_distance;
  output[1] = variance;
  output[2] = sqrt(variance);
}

void Analysis::outputHistogramOfXposForPop(double pop) {
	vector<double> position(3,0);
	vector<vector<double> > *block = new vector<vector<double> > (this->number_cells, position);
	vector<vector<double> > *positions;

	for (int i=0; i<this->number_sweeps; i++) {
		if (!this->binary_position_input)
			this->getBlock(block);
		else
			this->getBlockBinary(block);
		positions = new vector<vector<double> > (this->cell_classes[pop].size(),position);
		for (int j=0; j<this->cell_classes[pop].size(); j++) {
			positions->at(j)[0] = block->at(cell_classes[pop][j])[0];
			positions->at(j)[1] = block->at(cell_classes[pop][j])[1];
			positions->at(j)[2] = block->at(cell_classes[pop][j])[2];
		}
		vector<double> minmax(2);
		this->getMinMaxXPos(positions,minmax);

		int num_bins = (int)((minmax[1]-minmax[0])/this->bin_width)+1;

		vector<double> histogramvec;
		histogramvec.resize(num_bins+1);

		this->generateHistogramXPos(positions, histogramvec, minmax[0]);

		cout << "# time: " << (i+1)*this->output_interval << endl;
		for (int i=0; i<histogramvec.size(); i++)
			cout << minmax[0]+i*bin_width+0.5*bin_width << " " << histogramvec[i] << endl;
		cout << endl << endl;
		delete positions;
	}
}

void Analysis::outputHistogramOfPosWithLimits(double pop, int axis, double minLimit, double maxLimit)
{
	vector<double> position(3,0);
	vector<vector<double> > *block = new vector<vector<double> > (this->number_cells, position);
	vector<vector<double> > *positions;

	for (int i=0; i<this->number_sweeps; i++) {
		if (!this->binary_position_input)
			this->getBlock(block);
		else
			this->getBlockBinary(block);
		positions = new vector<vector<double> > (this->cell_classes[pop].size(),position);
		for (int j=0; j<this->cell_classes[pop].size(); j++) {
			positions->at(j)[0] = block->at(cell_classes[pop][j])[0];
			positions->at(j)[1] = block->at(cell_classes[pop][j])[1];
			positions->at(j)[2] = block->at(cell_classes[pop][j])[2];
		}
		vector<double> minmax(2);
		minmax[0] = minLimit;
		minmax[1] = maxLimit;

		int num_bins = (int)((minmax[1]-minmax[0])/this->bin_width)+1;

		vector<double> histogramvec;
		histogramvec.resize(num_bins+1);

		this->generateHistogramXPos(positions, histogramvec, minmax[0]);
		generateHistogramPos(positions, histogramvec, minmax[0],axis);

		cout << "# time: " << (i+1)*this->output_interval << endl;
		for (int i=0; i<histogramvec.size(); i++)
			cout << minmax[0]+i*bin_width+0.5*bin_width << " " << histogramvec[i] << endl;
		cout << endl << endl;
		delete positions;
	}


}



void Analysis::outputHistogramOfDistForPop(double x)
{
	vector<double> position(3,0);
	vector<vector<double> > *block = new vector<vector<double> > (this->number_cells, position);
	vector<double> *distances;

	for (int i=0; i<this->number_sweeps; i++) {
		if (!this->binary_position_input)
			this->getBlock(block);
		else
			this->getBlockBinary(block);
		distances = new vector<double> (this->cell_classes[x].size());
		for (int j=0; j<this->cell_classes[x].size(); j++) {
			distances->at(j) = pow(block->at(cell_classes[x][j])[0] - this->initial_position[0],2) +
				pow(block->at(cell_classes[x][j])[1] - this->initial_position[1],2) +
				pow(block->at(cell_classes[x][j])[2] - this->initial_position[2],2) ;
			distances->at(j) = sqrt(distances->at(j));
		}
		vector<double> minmax(2);
		this->getMinMaxDist(distances, minmax);
		int num_bins = (int)((minmax[1]-minmax[0])/this->bin_width)+1;
		vector<double> histogramvec(num_bins);

		cout << "#time: " << (i+1)*this->output_interval << endl;
		for (int i=0; i<histogramvec.size(); i++)
			cout << minmax[0]+i*bin_width+0.5*bin_width << " " << histogramvec[i] << endl;
		cout << endl << endl;
		delete distances;

	}
}

void Analysis::getMinMaxXPos(vector<vector<double> >* input, vector<double>& res) {
	double maximus = -1000000000000000;
	double minimus =  1000000000000000;
	for (int i=0; i<input->size(); i++) {
		if (maximus < input->at(i)[0])
			maximus = input->at(i)[0];
		if (minimus > input->at(i)[0])
			minimus = input->at(i)[0];
	}
	res[0] = minimus;
	res[1] = maximus;
}

void Analysis::getMinMaxDist(vector<double> *input, vector<double>& res) {
	double maximus = -10000000000000000000000;
	double minimus =  10000000000000000000000;
	for (int i=0; i<input->size(); i++) {
		if (maximus < input->at(i))
			maximus = input->at(i);
		if (minimus > input->at(i))
			minimus = input->at(i);
		}
		res[0] = minimus;
		res[1] = maximus;
}


void Analysis::generateHistogramXPos(vector<vector<double> > *input, vector<double>& histo, double min) {
	int index;
	double add = 1./input->size();

	for (int i=0; i<input->size(); i++) {
		index = (int)((input->at(i)[0]-min)/bin_width);
		histo[index] += add;
	}
}

void Analysis::generateHistogramPos(vector<vector<double> > *input, vector<double>& histo, double min, int axis)
{
	int index;
	double add = 1./input->size();

	for (int i=0; i<input->size(); i++) {
		index = (int)((input->at(i)[axis]-min)/bin_width);
		histo[index] += add;
	}
}

void Analysis::generateHistogramDist(vector<double> *input, vector<double>& histo, double min) {
	int index;
	double add = 1./input->size();
	for (int i=0; i<input->size(); i++) {
		index = (int)((input->at(i)-min)/bin_width);
		histo[index] += add;
	}
}

