/*
 * singlewellgeometryconstructor.hh
 *
 *  Created on: March, 11, 2010
 *      Author: jentsch
 */

#ifndef SINGLEWELLGEOMETRYCONSTRUCTOR_HH_
#define SINGLEWELLGEOMETRYCONSTRUCTOR_HH_

#include "../data/grid/grid.hh"
#include "../data/grid/nvoxel.hh"

#include <map>
#include <string>

using namespace std;
using namespace Hive;

namespace Transwell {
	/** @brief class for setting up the transwell geometry
	 *
	 *	@author Garrit Jentsch
	 *
	 *	@date Dec 8th, 2009 last edited: Dec 8th, 2009
	 */
	class SinglewellGeometryConstructor {
	public:
		SinglewellGeometryConstructor();
		~SinglewellGeometryConstructor();

		// return a pointer to the constructed grid.
		// calls the methods for constructing a grid
		Grid* returnGrid(string name);

		int getVoxelDomain(int v);

		void setHeight(double height);
		void setRadius(double radius);
		void setResolution(double);

		// we hide the methods for the actual construction of the grid from the
		// user
	protected:
		/// determines neighborhood-matrix
		void determineNeighborhood();
		/// construct coordinates
		void setupCoordinates();
		/// sets up quad voxels.
		void setupVoxels();

	private:
		// grid of the geometry
		Grid *grid;

		// radius of well
		double radius;

		// height of well
		double height;

		// dx = dy = dz
		double stepsize;

		// some more local variables
		vector<vector<vector<int> > > coordinate_index_lookup;
		vector<vector<vector<int> > > coordinate_domain_lookup;

		int number_coordinates;
		int number_points_plane;
		int number_points_height;

		vector<vector<int> > node_covered_by_voxels;
		vector<vector<int> > voxels;
		vector<int> voxels_domain_lookup;

		vector<vector<int> > connectivity;
		vector<vector<int> > connecting_directions;
		vector<vector<double> > rates;
	};


}


#endif /* SINGLEWELLGEOMETRYCONSTRUCTOR_HH_ */
