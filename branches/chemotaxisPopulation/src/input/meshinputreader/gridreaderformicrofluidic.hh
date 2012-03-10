/*
 * gridreaderformicrofluidic.hh
 *
 *  Created on: Sep 27, 2010
 *      Author: jentsch
 */

#ifndef GRIDREADERFORMICROFLUIDIC_HH_
#define GRIDREADERFORMICROFLUIDIC_HH_

#include <algorithm>

#include "gridreader.hh"

using namespace Hive;
using namespace std;

class GridReaderForMicroFluidic : public GridReader {
public:
	GridReaderForMicroFluidic();
	~GridReaderForMicroFluidic();

	void setResolution(double r);
	void readInitialConcentrations(ifstream&);
	void readInitialConcentrations(string name) {
			ifstream input2(name.c_str());
			this->readInitialConcentrations(input2);
		};
protected:
	void readGrid();
	void addVoxel(int, int, int&, vector<vector<int> >&);
	void determineNeighborhood(vector<vector<int> >& voxels, vector<vector<int> >& neighborhood,
							   vector<vector<double> >& rates) ;
	void setupLookupTables(ifstream&);

	double resolution;

	vector<vector<int>* > *voxel_lookup;
	vector<vector<int>* > *coordinate_lookup;
	vector<vector<int>* > *coordinate_lookup_upper;

	int used_coordinates;
	int used_voxels;
};

#endif /* GRIDREADERFORMICROFLUIDIC_HH_ */
