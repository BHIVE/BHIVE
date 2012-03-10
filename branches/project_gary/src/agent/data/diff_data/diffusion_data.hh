/**
 * @file data.hh
 *
 * this file contains the interfaces to the classes Data, ...
 *
 *
 * @date Oct 7, 2009  Last edited: Oct 14, 2009 by Garrit and Michael
 *
 * @author Garrit Jentsch
 */

#ifndef DIFFUSION_DATA_HPP_
#define DIFFUSION_DATA_HPP_
#define TREE_LIMIT 5

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../data.hh"
#include "../../../util/util.hh"

using namespace std;
using namespace Hive;

/**
 * @brief individual element of the database
 *
 *  specific class to store concentration grid for a diffusion simulator
 *
 *  @todo
 *
 *  @date Oct 19, 2009 Last edited: Oct 19, 2009 by Garrit and Michael
 *
 *  @author Garrit Jentsch and Chris Henry
 */

struct Coordinates {
	double x;
	double y;
	double z;
};

struct Voxel {    //
	vector<double> concentrations;
	vector<double> newConcentrations;
	vector<int> neighbors;            // The indecies of the neighbors of this element
	vector<double> baseRates;         // A/(VL) for each neightbor of this element
	vector<int> vertices;             // The indecies of the voxal vertices
	double volume;                    // volume of the voxel
	int voxel_id;
	Coordinates center;
};

struct node {
	Coordinates* Center;
	node* parent;
	double Length;
	node** children;
	vector<int> points;
};

class DiffData : public Data {
public:

	/**
	 * constructor
	 * @param name dataname
	 */
	DiffData(string name="diffusion_grid", string type="grid_type"):Data(name, type){searchTreeRoot = NULL;};

	/// Destructor
	~DiffData();

	/// virtual method for print the content of a data item on sterr.
	void printContent(ostream& out);

	Voxel* getVoxel(int index);

	Voxel* getVoxel(double x,double y,double z);

	/// returns center coordinate of the voxel whose identifyer is given as a parameter
	Coordinates getVoxelCenterCoordinate(int i);

	int getNumberOfVoxels();

	double getDiffusionCoef(int index);

	void setVoxelCenterCoordinate(int i, double x, double y, double z);

	void setSpecies(vector<string>& s);

	void setSpeciesDiffusionCoefs(vector<double>& d);

	void setupCoordinates(vector<vector<double> >& coord);

	void setupGrid(vector<vector<int> >& voxels, vector<vector<int> >& neighborhood);

	void setupBaseRates(vector<vector<double> >& interface_areas, vector<double>& volumes,
						vector<vector<double> >& center_coor);

	void setupVerticesPointedAtByVoxels(vector<vector<int> >&);

	void setupVolumes(vector<double>& volumes);

	int addVertex(double x,double y,double z);

	Coordinates* initializeCoordinates(double x,double y,double z);

	node* initializeNode(double length,Coordinates* center);

	double calcDistance(Coordinates* One,Coordinates* Two);

	void createTree();

	void addPointToTree(node* inNode,int index);

	void deleteTree(node* inNode);

	int nearestNeighbor(double x,double y,double z,node* inNode,bool backtrack = true);

	node* initializeNode(double length,Coordinates* center,node* parent);

	Voxel* findVetexVoxelWithPoint(double x,double y,double z,int closest_vertex);

	void printVerticesOfVoxelOnSterr(int i);

	void printTree(node* n );
	void printTree() { this->printTree(searchTreeRoot); } ;

protected:
	vector<string> species;
	vector<double> speciesDiffusionCoef;

	vector<Voxel*> grid;

	vector<Coordinates*> vertices;

	/// vertices_pointed_at_by_voxels[i] returns a vector of the voxelIDs that have vertices[i] as one of the vertices
	vector<vector<int> > vertices_pointed_at_by_voxels;

	node* searchTreeRoot;
};



#endif /* DATA_HPP_ */
