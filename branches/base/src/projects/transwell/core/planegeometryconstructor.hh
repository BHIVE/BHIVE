/*
 * planegeometryconstructor.hh
 *
 *  Created on: May 5, 2010
 *      Author: jentsch
 */

#ifndef PLANEGEOMETRYCONSTRUCTOR_HH_
#define PLANEGEOMETRYCONSTRUCTOR_HH_

#include "../data/grid/grid.hh"
#include "../data/grid/nvoxel.hh"

#include <map>
#include <string>

using namespace std;
using namespace Hive;

namespace Transwell {
/** @brief  class for setting up a test cube for running tests of what discretisation is needed for the il2 simulations
 *
 * @author garrit
 * @date Feb 22, 2010
 */

	class PlaneGeometryConstructor {
	public:
		PlaneGeometryConstructor();
		~PlaneGeometryConstructor();

		Grid *returnGrid(string name);

		void setLength(double);
		void setResolution(double);

	protected:
		void determineNeighborhood();
		void establishPeriodicBoundaryConditions();
		void setupCoordinates();
		void setupVoxels();

	private:
		Grid *grid;

		double length;
		double stepsize;

		int number_coordinates;
		int number_points_height;
		int number_points_plane;

		vector<vector<int> > node_covered_by_voxels;
		vector<vector<int > > voxels;

		vector<vector<int> > connectivity;
		vector<vector<int> > connecting_directions;
		vector<vector<double> > rates;

		vector<vector<vector<int> > > coordinate_index_lookup;

	};

}


#endif /* PLANEGEOMETRYCONSTRUCTOR_HH_ */
