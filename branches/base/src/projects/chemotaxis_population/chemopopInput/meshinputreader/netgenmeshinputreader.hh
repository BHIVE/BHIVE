/*
 * @file netgenmeshinputreader.hh
 *
 *this file contains the interface for the meshinputreader.
 *
 *@date Oct 20, 2009 last edited Oct 20, 2009
 *
 *@author garrit jentsch
 */

#ifndef NETGENMESHINPUTREADER_HH_
#define NETGENMESHINPUTREADER_HH_

#include <cmath>
#include <fstream>
#include <vector>

#include "../../../../core/util/nr/nr.h"

// using namespace Hive;

using namespace std;

namespace Hive {
/**
	 * @brief mesh input reader for reading a mesh from a file
	 *
	 * this class reads a mesh stored in the ntf format.
	 *
	 *  @todo
	 *
	 *  @bug
	 *
	 *  @author Garrit Jentsch
	 *
	 *  @date Oct 20th, 2009 last edited: Oct 20, 2009 by Garrit
	 */

	class NetgenMeshInputReader {
	public:
		NetgenMeshInputReader();
		~NetgenMeshInputReader();

		void determineNeighborhood();
		void readMesh(ifstream&);

		vector<vector<double> >& getCenterCoordinates() { return center_coordinates; };
		vector<vector<double> >& getCoordinates() { return coordinates; };
		vector<vector<int> >& getVoxels() { return voxels; };
		vector<vector<int> >& getNeighborhood() { return neighborhood; };
		vector<vector<double> >& getInterfaceAreas() { return interface_areas; };
		vector<double>& getVolumes() { return volumes; };
		vector<vector<int> >& getNodesPointedAtByVoxels() { return nodes_pointed_at_by_voxels; };

	protected:
		void calcCenters();
		double calcInterfaceArea(vector<int>& p);
		void calcVolumes();
		bool commonInterface(int a, int b, vector<int>& p);

		vector<vector<double> > center_coordinates;
		/// this stores the corrdinates of the mesh
		vector<vector<double> > coordinates;
		/// this stores which points belong to a voxel
		vector<vector<int> > voxels;
		/// this stores the neighborhood relationship among the voxels
		vector<vector<int> > neighborhood;
		/// this stores the interface elements
		vector<vector<int> > interfaces;
		/// this stores to which part of the geometry a voxel belong
		vector<int> geometry_voxel_lookup;
		/// this stores to which interface an interface element belongs
		vector<int> interface_lookup;
		/// this ...
		vector<vector<int> > nodes_pointed_at_by_voxels;
		/// this ...
		vector<vector<double> > interface_areas;
		/// this stores the volumes
		vector<double> volumes;

 	};
}
#endif /* NETGENMESHINPUTREADER_HH_ */
