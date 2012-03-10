/*
 * quadtreeformicrofluidic.hh
 *
 * quad tree for looking up cell positions when simulating a chemotaxis in
 * a microfluidic device.
 *
 *  Created on: Oct 4, 2010
 *      Author: jentsch
 */

#ifndef QUADTREEFORMICROFLUIDIC_HH_
#define QUADTREEFORMICROFLUIDIC_HH_

#include "../../../../core/agent/data/data.hh"

#include "grid.hh"
#include "nvoxel.hh"

#include <algorithm>
#include <limits>
#include <vector>

using namespace std;

namespace Hive {
	/// node of the quad tree. as with all data objects this is derived from the
	/// abstract data class.
	class QuadTreeNodeForMicroFluidic : public Data{
	public:
		/// default constructor
		QuadTreeNodeForMicroFluidic(string name="quad_tree_node", string type="quad_tree_node_type");
		/// constructor passing the parent node
		QuadTreeNodeForMicroFluidic(QuadTreeNodeForMicroFluidic*, int);
		/// constructor passing the center coordinate and the capacity
		QuadTreeNodeForMicroFluidic(Coordinate, int);
		/// destructor
		~QuadTreeNodeForMicroFluidic();
		/// addVoxel to the node
		void addVoxel(NVoxel*);
		/// checks whether a coordinate is within one of the covered_voxels
		int coordinateInsideCoveredVoxels(Coordinate& c);
		int coordinateInsideCoveredVoxels(Coordinate *c);

		void printContent(ostream&);


		friend class QuadTreeForMicroFluidic;

	protected:
		int getQuadrant(Coordinate& c);
		bool voxelWithinBounds(NVoxel *);

		int node_id;     /// how do i keep track of this
		static int used_ids;
		double resolution;
		int capacity;
		Coordinate center;
		vector<Coordinate> bounding_box_coordinates;
		vector<QuadTreeNodeForMicroFluidic* > children;
		QuadTreeNodeForMicroFluidic *parent;
		vector<NVoxel* > covered_voxels;
		bool isleaf;
	};

	/// quad tree, also derived from the abstract data class
	class QuadTreeForMicroFluidic : public Data {
	public:
		/// default constructor not doing anything
		QuadTreeForMicroFluidic(string name="quad_tree_for_micro_fluidic", string type="quad_tree_type");
		/// constructor building the quad tree with the given grid
		QuadTreeForMicroFluidic(Grid*);
		/// destructor
		~QuadTreeForMicroFluidic();
		/// construct the tree with the given Grid
		void constructTree(Grid*);
		/// get index of voxel inside of which Coordinate c lies
		int     getVoxelIndex(Coordinate&);
		int     getVoxelIndex(Coordinate*);
		/// return pointer to voxel inside of which coordinate c lies
		NVoxel* getVoxel(Coordinate&);
		NVoxel* getVoxel(Coordinate*);
		void    setGrid(Grid *g) { this->g = g;};
		void    setResolution(double r) { this->resolution = r; };
		void 	printContent(ostream&);


	protected:
		void initialiseRoot();
		// pointer to grid on which the
		Grid *g;
		// resolution of the grid
		double resolution;
		// root of the tree
		QuadTreeNodeForMicroFluidic *root;
	};

}

#endif /* QUADTREEFORMICROFLUIDIC_HH_ */
