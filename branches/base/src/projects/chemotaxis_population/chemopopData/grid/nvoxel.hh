/*
 * nvoxel.hh
 *
 *this file contains the interface to the classes Coordinatel, Nvoxel, OctTreeNode, and OctTree.
 *the classes in here are a reimplementation of chris henry's stuff with some changes to it.
 *
 *@author Garrit Jentsch
 *
 *@date  Created on: Nov 12, 2009 Last edited: Nov 16, 2009 by garrit
 *
 */

#ifndef NVOXEL_HH_
#define NVOXEL_HH_

#include "grid.hh"

#include <climits>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

namespace Hive {

// declaration of Grid class
class Grid;

/**
 * @brief a simple coordinate class.
 *
 * @author Garrit Jentsch
 *
 * @date Created on: Nov 12, 2009 Last edited: Nov 16, 2009 by garrit
 */
class Coordinate : public Data {
public:
	/// constructors
	Coordinate(string name="coordinate", string type="coordinate_type"):Data(name,type) {};
	Coordinate(Coordinate*);
	Coordinate(double x, double y, double z);
	~Coordinate();

	double calcDistance(Coordinate c);
	double calcDistance(Coordinate* c);

	void getCoordinates(double& x, double& y, double& z);
	int  getDimension();


	void setCoordinates(double x, double y, double z=0);
	void setCoordinates(Coordinate& c);

	/**
	 * sets the coordinates of this to c1-c2
	 */
	void setCoordinatesToDifference(Coordinate c1, Coordinate c2);

	void setCoordinatesToSum(Coordinate c1, Coordinate c2);

	bool larger(Coordinate c);
	bool smaller(Coordinate c);

	/**
	 * calculates the scalar product with c2
	 */
	double calcScalarProduct(Coordinate c2);

	void multiplyWithScalar(double);

	/**
	 * calculates the cross-product and returns the result in r
	 */
	void calcCrossProduct(Coordinate c1, Coordinate c2);

	void normalise();

	void printContent(ostream& out);

	double getX() const { return coor[0]; };
	double getY() const { return coor[1]; };
	double getZ() const { return coor[2]; };

	double at(int i) const { return coor[i]; };

	friend class OctTree;

protected:
	vector<double> coor;
};

class CoordinateComparison2D {
public:
	int operator() (const Coordinate& c1, const Coordinate& c2) const {
		return ((c1.getX() <= c2.getX()) && (c1.getY() <= c2.getY()) && (c1.getZ() <= c2.getZ()));
	}
};

/**
 * @brief individual voxel of the diffusion grid. note that we do not specify the dimensionality
 * and shape of the voxel.
 *
 * @todo
 *
 * @bug
 *
 * @date Nov 12, 2009, Last edited: Nov 12, 2009
 *
 * @author Garrit Jentsch
 */

class NVoxel : public Data {
public:
	NVoxel(string name="new_voxel", string type="voxel_type"):Data(name, type) {};

	NVoxel(NVoxel*);

	~NVoxel();

	void addVertex(int v);

	void printContent(ostream& out);

	double getConcentration(int species) { return concentrations[species]; };

	double getOldConcentration(int species) { return this->old_concentrations[species]; };

	/// returns coordinate i of voxel
	void getCoordinate(int, Coordinate& c);

	int getNumberNeighbors();

	int getNeighborID(int neighnum);

	bool isBoundary(int wall);

	double getBaseDiffusionRate(int num_neigh);

	void getVertices(vector<int>& argument) { argument = vertices; };

	int getVoxelID() { return voxel_id; };

	/**
	 * checks whether a coordinate lives inside the boundaries of the voxel
	 */
	bool housesCoordinate(Coordinate&);

	bool housesCoordinateQuadGrid(Coordinate&);

	bool housesCoordinateTetGrid(Coordinate&);

	void setNumberSpecies(int i) { this->concentrations.resize(i); this->old_concentrations.resize(i); };

	void setConcentration(int species, double value);

	/// copies data from concentrations onto old_concentrations
	void copyConcentration();

	friend class Grid;

	// wieso ist der ganze scheiss public?! das kann doch wohl nicht wahr sein?

	// protected:
	// will this really be concentrations or will it be particle numbers?
	vector<double> concentrations;
	vector<double> old_concentrations;
	vector<int>    neighbors;
	vector<int>    boundaries;
	// we need this even for the Quad mesh for the two transwells.
	vector<double> base_diffusion_rates;
	vector<int>    vertices;
	// for a quad-mesh with all voxels having the same size, this is redundant
	double		   volume;
	int 		   voxel_id;
	Coordinate	   center;         // i am not sure that this information is ever set.
	Grid          *grid_ptr;       // pointer to grid on which this voxel lives
};



///// ----------------------------------------------------------------------------------------------------
///// i do not think that the OctTree classes are working nor that they are any good! need to be improved.
/////
/**
 * @brief node of the oct tree
 *
 * @date Nov 16, 2009, Last edited: Nov 16, 2009
 *
 * @author Garrit Jentsch
 */

class OctTreeNode : public Data {
public:
	OctTreeNode(string name="OctTreeNode", string type="octtreenodetype"):Data(name,type) { this->parent = NULL; };
	~OctTreeNode();

	double getDistance(Coordinate c);
	void   initialiseNode(double, Coordinate);
	void   initialiseNode(double, Coordinate, OctTreeNode*);

	NVoxel* findVoxel(Coordinate&);

	void printContent(ostream& out);

	int getNodeIndex() { return node_index; };

	/**
	 * checks whether part of a given voxel is inside this octtreenode
	 */
	bool coveringVoxel(NVoxel*);

	/**
	 * method returns true if the direct line between between points l1 and l2 intersects
	 * the plane spanned by points p1-p3.
	 */
	bool lineIntersectingPlane(Coordinate& p1, Coordinate& p2, Coordinate& p3,
							   Coordinate& l1, Coordinate& l2);

	/**
	 * method for calculating whether the coordinate c lies inside the plane
	 * spanned by p1,p2,p3 using barycentric coordinates.
	 */
	bool pointOnTriangle(Coordinate& c, Coordinate& p1, Coordinate& p2, Coordinate& p3);

	friend class OctTree;

	static const int OCT_TREE_LIMIT = 5;

protected:
	Coordinate center;
	double length;
	OctTreeNode *parent;
	int node_index;
	vector<OctTreeNode* > children;
	vector<NVoxel *> coveredVoxels;
};

/**
 * @brief octtree for efficiently looking up in which voxel a certain coordinate lies
 *
 * @date Nov 16, 2009 Last edited: Nov 16, 2009
 *
 * @author Garrit Jentsch
 */
class OctTree : public Data {
public:
	OctTree(string name="OctTree", string type="octtreetype"):Data(name,type) { this->root_node = NULL;
		total_number_nodes = 1; };
	~OctTree();

	void addVoxelToTree(OctTreeNode *node, NVoxel *vox);
	void createTree(vector<NVoxel*>&, vector<Coordinate* >&);
	void deleteTree(OctTreeNode*);

	OctTreeNode* getRoot() { return root_node; };

	NVoxel* findVoxel(Coordinate&);

	void printContent(ostream &out);
private:
	int total_number_nodes;
	OctTreeNode *root_node;
};

}

#endif /* NVOXEL_HH_ */
