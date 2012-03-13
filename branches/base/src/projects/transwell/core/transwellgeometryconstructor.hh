/*
 * transwellgeometryconstructor.hh
 *
 *  Created on: Dec 8, 2009
 *      Author: jentsch
 */

#ifndef TRANSWELLGEOMETRYCONSTRUCTOR_HH_
#define TRANSWELLGEOMETRYCONSTRUCTOR_HH_

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
	class TranswellGeometryConstructor {
	public:
		TranswellGeometryConstructor();
		~TranswellGeometryConstructor();

		// return a pointer to the constructed grid.
		// calls the methods for constructing a grid
		Grid* returnGrid(string name);

		int getVoxelDomain(int v);

		void setHeights(double inner, double outer);
		void setRadii(double inner, double outer);
		void setResolution(double);
		void setThickness(double);

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

		// radius of inner well
		double inner_radius;
		// radius of outer well
		double outer_radius;

		// height between base of outer well and base of inner well
		double inner_height;

		// total height of the transwell
		double outer_height;

		// thickness of the inner well's wall
		double thickness;

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


#endif /* TRANSWELLGEOMETRYCONSTRUCTOR_HH_ */
