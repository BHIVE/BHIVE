/**
 *@file  grid.hh
 *
 * this file contains the interface to  a new grid class ... this is a quick an dirty implementation
 * it should be updated an made better ...
 *
 *@date  Created on: Nov 16, 2009
 *@author garrit jentsch
 */

#ifndef GRID_HH_
#define GRID_HH_

#include "../../../../core/agent/data/data.hh"
#include "../../../../core/util/util.hh"


// this is the file containing the declaration of the voxel and
// coordinate class
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
	Grid(string name = "diffusion_grid", string type = "grid_type"):Data(name, type) {
		is_quad_grid = true;
		this->search_tree = NULL;
		is_cube = false; };

	/// destructor
	~Grid();

	/// adds coordinate to grid for an incremental building of the grid.
	void addCoordinate(Coordinate *c) { vertices.push_back(c); };

	/// adds a  coordinate to the mesh for incremental grid building
	void addCoordinate(double x, double y, double z);

	// adds species to grid.
	void addSpecies(string, double);

	/// add voxel to grid
	void addVoxel(NVoxel *n) { this->grid.push_back(n); };

	/// checks whether a coordinate is already in the system. return index of coordinate,
	/// if it is already in the mesh otherwise it returns -1
	int coordinateAlreadyPresent(Coordinate *c);

	/// checks whether a coordinate is already in the system.
	int coordinateAlreadyPresent(double x, double y, double z);

	// i doubt that there is a good way for outputting this information.
	void printContent(ostream& out);

	/// i do not think that this one is working
	void setupSearchTree();

	/// returns center of voxel
	/// note that this method also calculates the center of the coordinate. the
	/// voxel itself is oblivious about its real coordinates.
	void getCenterOfVoxel(int vindex, vector<double>&);

	/// returns diffusion-coeff of the species stored at index index
	double getDiffusionCoeff(int index);

	/// returns number of voxels
	int getNumberOfVoxels();

	/// no clue, why we ever need this method
	int getTotalNumberOfNeighbors();

	/// the second argument is the time. it is not used. needed because
	/// an environment might be a mathematical function.
	/// do not think that we still need this. seems all a little odd here.
	double getValue(vector<double>&, double);

	vector<Coordinate*>& getCoordinates() { return vertices;};

	/// returns whether this grid is a quad grid
	bool isQuadGrid() { return this->is_quad_grid; };

	/// returns pointer to coordinate at position int
	Coordinate* getVertex(int);

	Coordinate* getVertexOfVoxel(int ver, int vox);

	/// returns pointer to voxel in which coordinate c happens to live
	NVoxel* getVoxel(Coordinate& c);

	/// returns pointer to voxel i
	NVoxel* getVoxel(int i);

	/// returns the voxel-id in which the point x,y,z lies
	int insideVoxel(double x, double y, double z);

	// returns whether point x,y,z is inside voxel, given voxel-id
	bool insideThisVoxel(double x, double y, double z, int voxid);

	void setConcentration(int vindex, int sindex, double v);

	void setIsCubeGrid(bool b) { this->is_cube = b; };

	void setIsQuadGrid(bool b) { this->is_quad_grid = b; };

	/// set names of species that exist on grid
	void setSpecies(vector<string>& );

	/// set diffusion coefficients of the species
	void setSpeciesDiffusionCoeffs(vector<double>& d);

	/// adds coordinates to hte system
	void setupCoordinates(vector<vector<double> >& coord);

	/// for setting up a tetrahedral grid
    void setupTetraGrid(vector<vector<int> >& voxels, vector<vector<int> >& neighborhood,
				        vector<vector<double> >& interface_areas, vector<double>& volumes,
				        vector<vector<double> >& center_coor);

    /// for setting up a voxel grid (quads!)
    void setupQuadGrid(vector<vector<int> >& voxels, vector<vector<int> >& neighborhood, vector<vector<double> >& rates);

    /// generates grid output
    void outputGridForValidation();

    /// i guess this is for cube quad grids
    void setCubeInformation(double s, double l) { this->stepsize = s; this->sidelength = l; };

    /// again for qubic gridth
    double getCubeSideLength() const { return this->sidelength; };

    /// returns stepsize dx
    double getCubeStepSize() const { return this->stepsize; };


protected:
    /// flag to see whether grid is composed of cubic voxels
	bool is_quad_grid;

	/// flag to see whether a grid is of cubic geometry
	bool is_cube;

	// keep track of the number of chemical species on the grid
	int num_species;

	// keep track of the number of voxels on the grid
	int num_voxels;

	/// names of the species
	vector<string> species;

	/// diff coeff of species
	vector<double> species_diffusion_coef;

	/// actual grid
	vector<NVoxel*> grid;

	/// coordinates of the grid
	vector<Coordinate* > vertices;

	// the octtree will only be needed for the tetrahedral meshes ... not really. how can we easily tell in which
	// quad-voxel an arbitrary point lies? remove this. there should be extra classes.
	OctTree* search_tree;

	/// geometric information for cube grid. resolution. don't understand why it needs to be stored in the grid class.
	double stepsize;

	/// geometric information for the cube grid: sidelenght. same as above.
	double sidelength;
};
} // namespace Hive

#endif /* GRID_HH_ */
