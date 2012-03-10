/*
 * CellPropertyMapper.hh
 *
 *  Created on: Apr 22, 2010
 *      Author: msneddon
 */

#ifndef CELLPROPERTYMAPPER_HH_
#define CELLPROPERTYMAPPER_HH_


#include <iostream>
#include <string>

#include <vector>
#include <map>
#include <fstream>


using namespace std;

class CellPropertyMapper {

	public:

		CellPropertyMapper();
		~CellPropertyMapper();


		void openFileStream(string populationFileName);
		void closeFileStream();


		// creates a map where the key is the values of the binCenters and
		// the data is a vector of integers giving the position of the cell
		// in the other output data files.
		void constructTauMap(vector <double> &binCenters, map <double,vector<int> > &tauMap);
		void constructCheYpMap(vector <double> &binCenters, map <double,vector<int> > &cheYpMap);
		void constructNoiseMap(vector <double> &binCenters, map <double,vector<int> > &noiseMap);

		int get_n_cells();


		// constructs a default map with all cells put into a single class with key 0.0
		void constructDefaultMap(map<double, vector<int> > &defaultMap);

		void printMap(map <double,vector<int> > &popMap);

		static const int TAU = 0;
		static const int NOISE = 1;
		static const int NUMOFMOTORS = 2;
		static const int ISCOORDINATED = 3;
		static const int FLAGELLAMODEL = 4;
		static const int MINFLAGELLATORUN = 5;
		static const int SEMIWAITINGTIME = 6;
		static const int CELLSPEED = 7;
		static const int MEANCHEYP = 8;


	protected:
		string populationFileName;
		ifstream populationFileStream;
		int n_cells;

		void parse(vector <double> &binCenters, map <double,vector<int> > &popMap, int field);



};

#endif /* CELLPROPERTYMAPPER_HH_ */
