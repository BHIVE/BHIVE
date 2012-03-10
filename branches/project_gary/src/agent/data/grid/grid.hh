/**
 *@file  grid.hh
 *
 * this file contains the interface to  a new grid class
 *
 *
 *@date  Created on: Nov 16, 2009
 *@author garrit jentsch
 */

#ifndef GRID_HH_
#define GRID_HH_

#include "../data.hh"
#include "../../../util/util.hh"

#include "nvoxel.hh"

#include <string>
#include <vector>

using namespace std;

namespace Hive {

// some declarations
class Coordinate;
class NVoxel;
class OctTreeNode;
class OctTree;

/**
*@brief the grid class is laid out in such a way that we can store both quad and tetrahedral meshes.
*
*@todo
*
*@author Garrit Jentsch
*
*@date Created on Nov 16, 2009 Laster edited: Nov 16, 2009 by garrit
*/
class Grid : public Data {
public:
	/// constructor
	Grid(string name = "diffusion_grid", string type = "grid_type"):Data(name, type) { is_quad_grid = true;
	this->search_tree = NULL;};

	/// destructor
	~Grid();

	/// add coordinates
	void addCoordinate(Coordinate *c) { vertices.push_back(c); };

	void addSpecies(string, double);

	/// add voxel to grid
	void addVoxel(NVoxel *n) { this->grid.push_back(n); };

	// i doubt that there is a good way for outputting this information.
	void printContent(ostream& out);
	void printDBObservables(OutputWriter *ow) {};
	void setDBObservables(OutputWriter *ow) {};

	void setupSearchTree();

	double getDiffusionCoeff(int index);

	int getNumberOfVoxels();

	int getTotalNumberOfNeighbors();

	Coordinate* getVertex(int);

	NVoxel* getVoxel(Coordinate& c);

	NVoxel* getVoxel(int i);

	/// returns the voxel-id in which the point x,y,z lies
	int insideVoxel(double x, double y, double z);

	void setSpecies(vector<string>& );

	void setSpeciesDiffusionCoeffs(vector<double>& d);

	void setupCoordinates(vector<vector<double> >& coord);

    void setupTetraGrid(vector<vector<int> >& voxels, vector<vector<int> >& neighborhood,
				        vector<vector<double> >& interface_areas, vector<double>& volumes,
				        vector<vector<double> >& center_coor);

    void setupQuadGrid(vector<vector<int> >& voxels, vector<vector<int> >& neighborhood, vector<vector<double> >& rates);

    void outputGridForValidation();

protected:
	bool is_quad_grid;

	vector<string> species;
	vector<double> species_diffusion_coef;
	vector<NVoxel*> grid;
	vector<Coordinate* > vertices;

	// the octtree will only be needed for the tetrahedral meshes ... not really. how can we easily tell in which
	// quad-voxel an arbitrary point lies?
	OctTree* search_tree;
};
} // namespace Hive

#endif /* GRID_HH_ */
