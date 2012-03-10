/*
 *@file netgenmeshinputreader.cpp
 *
 *contains implementation of the meshinputreader
 *
 *@date Oct 20, 2009 last edited 10-27 by garrit
 *
 * @author Garrit Jentsch
 */

#include "netgenmeshinputreader.hh"

using namespace Hive;

NetgenMeshInputReader::NetgenMeshInputReader() {}

// check whether we need a real destructor that does something
NetgenMeshInputReader::~NetgenMeshInputReader() {}

// this version of calculating the center of a tetrahedron is based on calculating
// the intersection of the lines connecting the midpoints of opposite edges.
void NetgenMeshInputReader::calcCenters() {
	// coordinates of the nodes of the tetrahedron
	vector<double> one(3);
	vector<double> two(3);
	vector<double> three(3);
	vector<double> four(3);
	// resize the centers coordinates vector
	this->center_coordinates.resize(voxels.size());
	for (int i=0; i<voxels.size(); i++) {
		// allocate memory
		center_coordinates[i].resize(3);
		// get coordinates
		one   = coordinates[voxels[i].at(0)];
		two   = coordinates[voxels[i].at(1)];
		three = coordinates[voxels[i].at(2)];
		four  = coordinates[voxels[i].at(3)];
		// calculate halfpoint  between one and two
		vector<double> h12(3);
		h12[0] = one[0] - 0.5*(one[0]-two[0]);
		h12[1] = one[1] - 0.5*(one[1]-two[1]);
		h12[2] = one[2] - 0.5*(one[2]-two[2]);
		// calculate halfpoint betwee three and four
		vector<double> h34(3);
		h34[0] = three[0] - 0.5*(three[0]-four[0]);
		h34[1] = three[1] - 0.5*(three[1]-four[1]);
		h34[2] = three[2] - 0.5*(three[2]-four[2]);

		// the center is now located halfway between h12 and h34
		center_coordinates[i].at(0) = h12[0] - 0.5*(h12[0] - h34[0]);
		center_coordinates[i].at(1) = h12[1] - 0.5*(h12[1] - h34[1]);
		center_coordinates[i].at(2) = h12[2] - 0.5*(h12[2] - h34[2]);
	}
}

double NetgenMeshInputReader::calcInterfaceArea(vector<int>& vec) {
        // for calculating the surface area we make use of the cross product
        // first we get the two vectors span by the three points

        vector<double> vec_1(3);
        vector<double> vec_2(3);

        vec_1[0] = coordinates[vec[1]].at(0)-coordinates[vec[0]].at(0);
        vec_1[1] = coordinates[vec[1]].at(1)-coordinates[vec[0]].at(1);
        vec_1[2] = coordinates[vec[1]].at(2)-coordinates[vec[0]].at(2);

        vec_2[0] = coordinates[vec[2]].at(0)-coordinates[vec[0]].at(0);
        vec_2[1] = coordinates[vec[2]].at(1)-coordinates[vec[0]].at(1);
        vec_2[2] = coordinates[vec[2]].at(2)-coordinates[vec[0]].at(2);

        double interface_area;
        interface_area = pow(vec_1[1]*vec_2[2]-vec_1[2]*vec_2[1], 2) + pow(vec_1[2]
                        *vec_2[0]-vec_1[0]*vec_2[2], 2) + pow(vec_1[0]*vec_2[1]-vec_1[1]
                        *vec_2[0], 2);

        return sqrt(interface_area)/2.;
}

void NetgenMeshInputReader::calcVolumes() {
	volumes.resize(voxels.size());
	for (unsigned int i=0; i<voxels.size(); i++) {
		// numerical recipes matrix
		Mat_DP a(4, 4); // zeile , spalte
		// assign values to matrix
		for (int spalte = 0; spalte < 4; spalte++)
			a[0][spalte] = coordinates[voxels[i].at(spalte)].at(0);
		for (int spalte = 0; spalte < 4; spalte++)
			a[1][spalte] = coordinates[voxels[i].at(spalte)].at(1);
		for (int spalte = 0; spalte < 4; spalte++)
			a[2][spalte] = coordinates[voxels[i].at(spalte)].at(2);
		for (int spalte = 0; spalte < 4; spalte++)
			a[3][spalte] = 1;
		// vector for ludcmp method
		Vec_INT indx(4);
		DP d;
		// LU decomposition
		NR::ludcmp(a, indx, d);
		// calculate determinant
		for (int j=0; j<4; j++)
			d *= a[j][j];

		// calculate volume
		volumes[i] = (1./6.)*fabs(d);
	}
}

void NetgenMeshInputReader::determineNeighborhood() {
	 // allocate memory for neighbors
	neighborhood.resize(voxels.size());
	interface_areas.resize(voxels.size());

	int a, b;
	double area;
	vector<int> tmp(3);
	// use nodes pointed at by simplex for determining neighborhood
	for (unsigned int index=0; index<nodes_pointed_at_by_voxels.size(); index++) {
		for (unsigned int i=0; i<nodes_pointed_at_by_voxels[index].size(); i++) {
			for (unsigned int j=i+1; j<nodes_pointed_at_by_voxels[index].size(); j++) {
				a = nodes_pointed_at_by_voxels[index].at(i);
				b = nodes_pointed_at_by_voxels[index].at(j);
				bool neigh = false;
				for (unsigned int k=0; k<neighborhood[a].size(); k++)
					if (neighborhood[a].at(k) == b) {
						neigh = true;
						break;
					}
				if ((!neigh) && commonInterface(a, b, tmp) && (geometry_voxel_lookup[a] == geometry_voxel_lookup[b])) {
					neighborhood[a].push_back(b);
					neighborhood[b].push_back(a);
					area = calcInterfaceArea(tmp);
					interface_areas[a].push_back(area);
					interface_areas[b].push_back(area);
				}
			}
		}
	}
}

bool NetgenMeshInputReader::commonInterface(int a, int b, vector<int>& p) {
        // check whether three nodes of a can also be found in b
        bool result = false;
        int identical = 0;
        for (int i=0; i<4; i++) {
                for (int j=0; j<4; j++) {
                        if (voxels[a].at(i) == voxels[b].at(j))
                                p[identical++] = voxels[a].at(i);
                }
        }
        if (identical == 3)
                result = true;
        return result;
}


void NetgenMeshInputReader::readMesh(ifstream& input) {
	int numberOfVertices;
	double dummy;
	int dummie;
	input >> numberOfVertices;
	coordinates.resize(numberOfVertices);
	for (int i=0; i<numberOfVertices; i++) {
		coordinates[i].resize(3);
		input >> dummy;
		coordinates[i][0] = dummy;
		input >> dummy;
		coordinates[i][1] = dummy;
		input >> dummy;
		coordinates[i][2] = dummy;
	}

	int numberOfVoxels;
	input >> numberOfVoxels;
	voxels.resize(numberOfVoxels);
	geometry_voxel_lookup.resize(numberOfVoxels);
	nodes_pointed_at_by_voxels.resize(numberOfVertices);
	for (int i=0; i<numberOfVoxels; i++) {
		voxels[i].resize(4);
		input >> dummie;
		geometry_voxel_lookup[i] = dummie-1; // change geometry offset to start at zero
		input >> dummie;
		voxels[i][0] = dummie-1; // the offsets are at zero.
		nodes_pointed_at_by_voxels[dummie-1].push_back(i);
		input >> dummie;
		voxels[i][1] = dummie-1;
		nodes_pointed_at_by_voxels[dummie-1].push_back(i);
		input >> dummie;
		voxels[i][2] = dummie-1;
		nodes_pointed_at_by_voxels[dummie-1].push_back(i);
		input >> dummie;
		voxels[i][3] = dummie-1;
		nodes_pointed_at_by_voxels[dummie-1].push_back(i);
 	}
	int number_of_interface_elements;
	input >> number_of_interface_elements;
	interfaces.resize(number_of_interface_elements);
	interface_lookup.resize(number_of_interface_elements);
	for (int i=0; i<number_of_interface_elements; i++) {
		interfaces[i].resize(3);
		input >> dummie;
		interface_lookup[i] = dummie-1;
		input >> dummie;
		interfaces[i][0] = dummie-1;
		input >> dummie;
		interfaces[i][1] = dummie-1;
		input >> dummie;
		interfaces[i][2] = dummie-1;
	}

	this->determineNeighborhood();
	this->calcCenters();
	this->calcVolumes();
}
