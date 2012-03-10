
#include <iostream>
#include <vector>
#include "CellPropertyMapper.hh"
#include "analysis.hh"


using namespace std;

int main(int argc, char *argv[])
{
	// 1) PARSE ARGS
	if(argc<4){
		cerr<<"not enough args"<<endl;
		cerr<<"run as: ./analyze [cell_data.bdat] [cell_positions.bdat] [cell_properties.dat] [outputfileName]"<<endl;
		exit(1);
	}
	string cell_data = argv[1];
	string cell_positions = argv[2];
	string cell_properties = argv[3];
	string outputfile = argv[4];

	cerr<<" cell_data file:        "<<cell_data<<endl;
	cerr<<" cell_positions file:   "<<cell_positions<<endl;
	cerr<<" cell_properties file:  "<<cell_properties<<endl;
	cerr<<" outputfile file:       "<<outputfile<<endl;




	// 2) FIGURE OUT WHAT CELLS WE ARE DEALING WITH
	CellPropertyMapper *cpm = new CellPropertyMapper();
	cpm->openFileStream(cell_properties);


	// 3) CREATE THE ANALYZER
	int n_cells = cpm->get_n_cells();
	//cout<<"n_cells"<<n_cells<<endl;

	// this is bad: we do not know a priori how often the program has produced an output?!
	// add an analysis method, that does this.
	int sweeps = 10;
	double outputinterval = 10.0;

	Analysis *a = new Analysis(n_cells,sweeps,outputinterval);
	a->setOutputFile(outputfile);
	a->setInitialPosition(0,0,0);
	a->setCellDataInputFile(cell_data,true);
	a->setCellPositionsInputFile(cell_positions,true);
	/// this analyses how many outputs were produced and set the number of sweeps variable accordingly
	a->getNumberOfOutputs();


	// 4) CREATE THE CELL MAP

	//Based on taus
    vector <double> binCenters;
	binCenters.push_back(2);
	binCenters.push_back(6);
	binCenters.push_back(8);
	binCenters.push_back(9);
	binCenters.push_back(13);
	binCenters.push_back(17);

	map<double,vector<int> > tau_map;
	cpm->constructTauMap(binCenters,tau_map);
	a->setCellClasses(tau_map);


	// for generating the map of the entire population
	/*
	map<double,vector<int> > default_map;
	cpm->constructDefaultMap(default_map);
	a->setCellClasses(default_map);
	//cpm->printMap(default_map);
   */


	// 4) ANALYZE
	// for analysing the x-coordinate of the center of mass
	// a->outputMap();
	// a->analysePositions();

	// for producing a histogram of the x position of all cells of a particular class
	a->setBinWidth(100);
	//a->outputHistogramOfXposForPop(8);
	int axis = 0; //set to X axis
	a->outputHistogramOfPosWithLimits(8,0,-8000,2000);

	// 5) EXIT NICELY
	// what are you trying to tell me? i am not closing my file-streams nor am i using a correct destructor?!
	cpm->closeFileStream();
	delete cpm;
	delete a;

	cerr<<"done."<<endl;
	return 0;



	/* garrit main file ... i think we can remove this ...
	 *
		Analysis *ana = new Analysis(9200,331,10.0);

		CellPropertyMapper *cpm = new CellPropertyMapper();


		map<double,vector<int> > default_map;
		cpm->openFileStream("huengwonII.dat");

		//Create bin centers
		vector <double> binCenters;
		binCenters.push_back(2.5);
		binCenters.push_back(7.5);
		binCenters.push_back(12.5);
		binCenters.push_back(17.5);

		cpm->constructTauMap(binCenters, default_map);


		ana->setCellClasses(default_map);

		ana->setCellPositionsInputFile("no_gradient.bdat",true);
		ana->setOutputFile("ergebnis_no");

		ana->analysePositions();


	//	vector<double> pos(3);
	//	vector<vector<double> > *mat = new vector<vector<double> >(9200,pos);
	//	int index = 1;
	//	while (ana->getBlockBinary(mat) == 0) {
	//		ana->calculateCenterOfMass(mat, pos);
	//		cerr << index++ << " " << pos[0] << " " << pos[1] << " " << pos[2] << endl;
	//	}

		delete cpm;


		cout<<"done."<<endl;
		return 0;
		*/
}
