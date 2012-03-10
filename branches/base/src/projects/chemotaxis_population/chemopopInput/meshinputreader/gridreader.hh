/*
 * gridreader.hh
 *
 *  Created on: Sep 27, 2010
 *      Author: jentsch
 */

#ifndef GRIDREADER_HH_
#define GRIDREADER_HH_

#include "../../chemopopData/grid/grid.hh"

#include "../../../../core/exception/exception.hh"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace Hive;

/**
 * base class from which to derive grid reader classes.
 *
 */

class GridReader {
public:
	GridReader();
	virtual ~GridReader();

	Grid* returnGrid();
	Grid* returnGrid(string);

	void setInputFile(string input);

	void addSpecies(string, double);
	void setSpecies(vector<string>&, vector<double>&);

protected:
	virtual void readGrid();
	bool inputset;
	ifstream input;
	Grid *g;
	int number_species;
	vector<string> names;
	vector<double> diffs;
};


#endif /* GRIDREADER_HH_ */
