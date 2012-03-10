/*
 * cubegeometryconstructor.hh
 *
 *  Created on: Feb 22, 2010
 *      Author: jentsch
 */

#ifndef CUBEGEOMETRYCONSTRUCTOR_HH_
#define CUBEGEOMETRYCONSTRUCTOR_HH_

#include "../../agent/data/grid/grid.hh"
#include "../../agent/data/grid/nvoxel.hh"

#include <string>

using namespace std;
using namespace Hive;

namespace ChemoPop {
/**@brief  class for setting up a test cube for running tests of what discretisation is needed for the il2 simulations
 *
 * @author garrit
 * @date Feb 22, 2010
 */

	class CubeGeometryConstructor {
	public:
		CubeGeometryConstructor();
		~CubeGeometryConstructor();

		Grid *returnGrid(string name);

		void setLength(double);
		void setResolution(double);
		void setSpecies(int, vector<string>&, vector<double>&);

	protected:
		void determineNeighborhood();
		void setupCoordinates();
		void setupVoxels();

	private:
		Grid *grid;

		double length;
		double stepsize;

		int number_coordinates;
		int number_points_height;
		int number_points_plane;

		int number_species;
		vector<string> species_names;
		vector<double> diff_species;

		vector<vector<int> > node_covered_by_voxels;
		vector<vector<int > > voxels;

		vector<vector<int> > connectivity;
		vector<vector<int> > connecting_directions;
		vector<vector<double> > rates;

		vector<vector<vector<int> > > coordinate_index_lookup;

	};

}


#endif /* CUBEGEOMETRYCONSTRUCTOR_HH_ */
